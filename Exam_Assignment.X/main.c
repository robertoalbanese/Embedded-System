/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#pragma config FPR = XT                 // Primary Oscillator Mode (XT)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

#include "xc.h"
#include "timer.h"
#include "spi.h"
#include "parser.h"
#include "uart.h"
#include "pwm.h"
#include "adc.h"
#include "buttons.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    void *(*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

typedef struct {
    uart_buffer* buffer;
    parser_state* pstate;
    rpm_data* rpm;
    temperature_info* temperature;
    display_lcd* display;
} program_info;

//Tasks info structure
heartbeat schedInfo[MAX_TASKS];
//Circular rx buffer for UART data
uart_buffer buffer;
//RPM data
rpm_data rpm_info;
//Structure deciding which format to display
display_lcd display;
//State of the program
int state;
//State (char))
char state_info[] = {'C', 'T', 'H'};

//This task receives an rpm value and uses it to generate a pwm signal

void* task_pwm_control_motor(void* params) {
    rpm_data* data = (rpm_data*) params;
    //Generate and send PWM signals
    sendPWM(data);
    return NULL;
}
// This task reads within the circular buffer and decode a new received message from the PC

void* task_uart_reciver(void* params) {
    program_info* info = (program_info*) params;
    char temp; //Received character
    int parser;

    while (UART_buffDim(info->buffer) > 0) {
        temp = UART_readOnBuffer(info->buffer);

        parser = parse_byte(info->pstate, temp);
        if (parser == NEW_MESSAGE) {
            // Message Decoding Routine
            // Message typer received - HLREF
            if (strcmp(info->pstate->msg_type, "HLREF") == 0) {
                //If we are in SAFE STATE, ignore the reference message
                if (state == STATE_SAFE) {
                } else {
                    //If we are in TIMEOUT STATE, go back to COMMAND STATE
                    if (state == STATE_TIMEOUT) {
                        state = STATE_COMMAND;
                    }
                    //Store new rpm values in the rpm struct
                    sscanf(info->pstate->msg_payload, "%d,%d", &info->rpm->rpm1, &info->rpm->rpm2);
                    // Saturate rpm if they're over the allowed threshold
                    satRPM(info->rpm);
                    //reset timer 2 and interrupt
                    T2CONbits.TON = 0; //starts the timer
                    TMR2 = 0; //reset the timer
                    IEC0bits.T2IE = 1; // Enable interrupt of timer t2
                    IFS0bits.T2IF = 0; // Set the timer flag to zero to be notified of a new event
                    T2CONbits.TON = 1; //starts the timer
                }
                // Message type received - HLREF
            } else if (strcmp(info->pstate->msg_type, "HLSAT") == 0) {
                int tempMax, tempMin;
                //Store new rpm thresholds values in the rpm struct
                sscanf(info->pstate->msg_payload, "%d,%d", &tempMin, &tempMax);
                //Check if the new values are acceptable
                if (tempMax < MAX_RPM && tempMax >= 0 && tempMin > MIN_RPM && tempMin <= 0) {
                    //Update rpm boundaries
                    info->rpm->maxRPM = tempMax;
                    info->rpm->minRPM = tempMin;
                    //Return positive feedback
                    char ack[50] = "$MCACK,SAT,1*";
                    UART_sendMsg(ack);
                } else {
                    //If the new rpm thresholds are rejected, return negative feedback
                    char ack[50] = "$MCACK,SAT,0*";
                    UART_sendMsg(ack);
                }
                // Message typer received - HLREF
            } else if (strcmp(info->pstate->msg_type, "HLENA") == 0) {
                //If we are in the SAFE STATE, then we exit and we restart TIMER2 for the timeout condition
                if (state == STATE_SAFE) {
                    state = STATE_COMMAND;
                    //reset timer 2 and interrupt
                    TMR2 = 0; //reset the timer
                    IEC0bits.T2IE = 1; // Enable interrupt of timer t2
                    IFS0bits.T2IF = 0; // Set the timer flag to zero to be notified of a new event
                    T2CONbits.TON = 1; //starts the timer
                    //Return positive feedback
                    char ack[50] = "$MCACK,ENA,1*";
                    UART_sendMsg(ack);
                    //Otherwise we do nothing
                } else {
                    //do nothing or
                    //return negative feedback
                    /*ack={"$MCACK,ENA,0*"};
                    UART_sendMsg(ack);*/
                }
            }
        }
    }
    return NULL;
}

//This task reads temperature values from the sensor

void* task_acquire_temperature(void* params) {
    temperature_info* temp_info = (temperature_info*) params;

    while (ADCON1bits.DONE == 0); //wait until  the conversion is done
    ADCON1bits.DONE = 0;
    int i;
    // Temperature data
    int tempBits = ADCBUF0;
    double tempVolts = (double) tempBits * 5.0 / 1024.0;
    temp_info->temperature = (tempVolts - 0.75)*100.0 + 25.0;
    //Update temperature FIFO buffer
    for (i = (sizeof (temp_info->temp_records) / sizeof (temp_info->temp_records[0])); i > 0; i--) {
        temp_info->temp_records[i] = temp_info->temp_records[i - 1];
    }
    temp_info->temp_records[0] = temp_info->temperature;

    return NULL;
}
// This task sends the current average value of the temperature. We compute it using the last ten values

void* task_average_temperature(void* params) {
    temperature_info* temp_info = (temperature_info*) params;
    char msg[50];
    int i;
    int arrayDimension = sizeof (temp_info->temp_records) / sizeof (temp_info->temp_records[0]);
    for (i = 0; i < arrayDimension; i++) {
        temp_info->average = temp_info->average + temp_info->temp_records[i];
    }
    temp_info->average = temp_info->average / arrayDimension;

    //Send average routine
    sprintf(msg, "$MCFBK,%2.1f*", temp_info->average);
    UART_sendMsg(msg);

    return NULL;
}
// This stask blinks led D3 and D4: 
// - D3 blinks to show the correct execution of the program
// - D4 blinks only if we are in TIMEOUT STATE

void* task_led_blink(void* params) {
    //Blink D3
    LATBbits.LATB0 = !LATBbits.LATB0;
    //Blink D4 if in timeout state
    if (state == STATE_TIMEOUT)
        LATBbits.LATB1 = !LATBbits.LATB1;
    else
        LATBbits.LATB1 = 0;
    return NULL;
}
// This task prints informations on the lcd 

void* task_print_lcd(void* params) {
    display_lcd* info = (display_lcd*) params;
    //If button s6 has been pressed, then the display changes (this is done in the button s& routine of the interrupt)
    info->format[info->index](info->display_info);
    return NULL;
}
// This task sends a feedback message to the PC

void* task_send_feedback(void* params) {
    rpm_data* rpm_info = (rpm_data*) params;
    char feedback[50];
    sprintf(feedback, "$MCFBK,%d,%d,%d*", rpm_info->rpm1, rpm_info->rpm2, state);
    UART_sendMsg(feedback);
    return NULL;
}
//Scheduler of the program.

void scheduler() {
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].n >= schedInfo[i].N) {
            schedInfo[i].task(schedInfo[i].params);
            schedInfo[i].n = 0;
        }
    }
}

int main(void) {
    // device config
    adc_config();
    pwm_config();
    UART_config();
    buttons_config();
    // LED config
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;

    // Initialization of rpm_info limits
    rpm_info.maxRPM = MAX_RPM;
    rpm_info.minRPM = MIN_RPM;
    // The motors are initially stopped
    rpm_info.rpm1 = 0;
    rpm_info.rpm2 = 0;
    // Duty cycle at 50% (Motors stopped)
    rpm_info.dutyCycle1 = 0.5;
    rpm_info.dutyCycle2 = 0.5;

    //Circular buffer init
    buffer.headIndex = 0; //wrinting index
    buffer.tailIndex = 0; //reading index

    //Temperature informations
    temperature_info temperature;
    //Temperature init
    temperature.temperature = 0;
    temperature.average = 0;

    //Display info initialization
    display_info display_info;
    display_info.rpm_info = &rpm_info;
    display_info.temp_info = &temperature;

    //Display lcd structure
    display.index = 0;
    display.display_info = &display_info;
    display.format[0] = &displayCaseA;
    display.format[1] = &displayCaseB;

    //Parser initialization
    parser_state pstate;
    pstate.state = STATE_DOLLAR;
    pstate.index_payload = 0;
    pstate.index_type = 0;

    //Configuration struct containing all the info struct of the program
    program_info program_info;
    program_info.buffer = &buffer;
    program_info.pstate = &pstate;
    program_info.rpm = &rpm_info;
    program_info.temperature = &temperature;
    program_info.display = &display;

    //System starts in COMMAND state
    state = STATE_COMMAND;

    // Assignment of task function
    schedInfo[0].task = &task_pwm_control_motor;
    schedInfo[1].task = &task_uart_reciver;
    schedInfo[2].task = &task_acquire_temperature;
    schedInfo[3].task = &task_average_temperature;
    schedInfo[4].task = &task_led_blink;
    schedInfo[5].task = &task_print_lcd;
    schedInfo[6].task = &task_send_feedback;

    // Assignment of input parameters for each task
    schedInfo[0].params = &rpm_info;
    schedInfo[1].params = &program_info;
    schedInfo[2].params = &temperature;
    schedInfo[3].params = &temperature;
    schedInfo[4].params = NULL;
    schedInfo[5].params = &display;
    schedInfo[6].params = &rpm_info;

    // Initialization of counters
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    schedInfo[5].n = 0;
    schedInfo[6].n = 0;

    // Initialization of execution time for each task (t=N*50ms)
    schedInfo[0].N = 1; //Send pwm signal               100Hz
    schedInfo[1].N = 1; //UART rx                       100Hz
    schedInfo[2].N = 10; //Acquire temperature          10Hz
    schedInfo[3].N = 100; //Send average temperature    1Hz
    schedInfo[4].N = 50; //Leds blink                   1Hz
    schedInfo[5].N = 10; //Print to lcd                 10Hz
    schedInfo[6].N = 20; //Feedback msg                 5Hz

    //Timer 1 config (control loop at 100Hz)
    tmr_setup_period(TIMER1, 10);
    //Timer 2 config (Timeout mode timer)
    tmr_setup_period(TIMER2, 5000);

    //Control loop
    while (1) {
        scheduler();
        tmr_wait_period(TIMER1);
    }
    return 0;
}
