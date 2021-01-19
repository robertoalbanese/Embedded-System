/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 18, 2021, 10:59 AM
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

//This task receives an rpm value and uses it to generate a pwm signal

void* task_pwm_control_motor(void* params) {
    rpm_data* data = (rpm_data*) params;

    int rpm1 = data->rpm1;
    int rpm2 = data->rpm2;
    //Generate and send PWM signals
    sendPWM(&rpm1, &rpm2);

    return NULL;
}

void* task_uart_reciver(void* params) {
    uart_buffer* newBuffer = (uart_buffer*) params;
    char temp; //Received character
    int parser;
    char ack[50];

    while (UART_buffDim(&newBuffer) > 0) {
        temp = UART_readOnBuffer(&newBuffer);

        parser = parse_byte(&pstate, temp);
        if (parser == NEW_MESSAGE) {
            int tempRPM1, tempRPM2;
            // Message Decoding Routine
            if (strcmp(pstate->msg_type, "HLREF") == 0) {
                if (state == STATE_SAFE) {
                } else {
                    if (state == STATE_TIMEOUT) {
                        state = STATE_COMMAND;
                        //reset timer 2 and interrupt
                        TMR2 = 0; //reset the timer
                        IEC0bits.T2IE = 1; // Enable interrupt of timer t2
                        T2CONbits.TON = 1; //starts the timer
                    }
                    sscanf(pstate->msg_payload, "%d,%d", &tempRPM1, &tempRPM2);
                    // Saturate RPM if they're above the allowed threshold
                    tempRPM1 = satRPM(tempRPM1);
                    tempRPM2 = satRPM(tempRPM2);

                    velocity->rpm1 = tempRPM1;
                    velocity->rpm2 = tempRPM2;
                }
            } else if (strcmp(pstate->msg_type, "HLSAT") == 0) {
                int tempMax, tempMin;
                sscanf(pstate->msg_payload, "%d,%d", &tempMax, &tempMin);
                if (tempMax < MAX_RPM && tempMax > 0 && tempMin > MIN_RPM && tempMin < 0 && tempMin < tempMax) {
                    velocity.maxRPM = tempMax;
                    velocity.minRPM = tempMin;
                    //return positive feedback
                    ack={"$MCACK,SAT,1*"};
                    UART_sendMsg(ack);
                } else {
                    //return negative feedback
                    ack={"$MCACK,SAT,0*"};
                    UART_sendMsg(ack);
                }
            } else if (strcmp(pstate->msg_type, "HLENA") == 0) {
                if (state == STATE_SAFE) {
                    state = STATE_COMMAND;
                    //reset timer 2 and interrupt
                    TMR2 = 0; //reset the timer
                    IEC0bits.T2IE = 1; // Enable interrupt of timer t2
                    T2CONbits.TON = 1; //starts the timer
                    //return positive feedback
                    ack={"$MCACK,ENA,1*"};
                    UART_sendMsg(ack);
                    
                } else {
                    //do nothing or
                    //return positive feedback
                    /*ack={"$MCACK,ENA,0*"};
                    UART_sendMsg(ack);*/
                }
            }
        }
    }
    return NULL;
}

void* task_acquire_temperature(void* params) {
    temperature_info* temp_info = (temperature_info*) params;

    while (ADCON1bits.DONE == 0); //wait until  the conversion done
    ADCON1bits.DONE = 0;

    // Temperature data
    int tempBits = ADCBUF0;
    double tempVolts = (double) tempBits * 5.0 / 1024.0;
    temp_info->temperature = (tempVolts - 0.75)*100.0 + 25.0;
    //Update FIFO buffer
    for (int i = (sizeof (temp_info->average) / sizeof (temp_info->average[0])); i > 0; i--) {
        temp_info->average[i] = temp_info->average[i - 1];
    }
    temp_info->average[0] = temp_info->temperature;

    return NULL;
}

void* task_average_temperature(void* params) {
    temperature_info* temp_info = (temperature_info*) params;
    double average = 0.0;
    char msg[50];
    int arrayDimension = sizeof (temp_info->average) / sizeof (temp_info->average[0]);
    for (int i = 0; i < arrayDimension; i++) {
        average = average + temp_info->average[i];
    }
    average = average / arrayDimension;

    //Send average routine
    sprintf(msg, "$MCFBK,2.1f*", average);
    UART_sendMsg(msg);

    return NULL;
}

void* task_led_blink(void* params) {
    //Blink D3
    LATBbits.LATB0 = !LATBbits.LATB0;
    //Blink D4 if in timeout state
    if (state == STATE_TIMEOUT)
        LATBbits.LATB1 = !LATBbits.LATB1;
    return NULL;
}

void* task_print_lcd(void* params) {
    display.format[display.index];
    return NULL;
}

void* task_send_feedback(void* params) {
    char feedback[50];
    sprintf(feedback, "$MCFBK,%d,%d,%d*", velocity.rpm1, velocity.rpm2,state);
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
    UART_bufferInit(&reciverBuffer);
    buttons_config();
    // LED config
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    // Initialization of velocity limits
    velocity.maxRPM = MAX_RPM;
    velocity.minRPM = MIN_RPM;
    //Temperature
    temperature.temperature = 0;
    //Display lcd structure
    display.index = 0;
    display.format[0] = &displayCaseA;
    display.format[1] = &displayCaseB;

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
    schedInfo[0].params = &velocity;
    schedInfo[1].params = &reciverBuffer;
    schedInfo[2].params = &temperature;
    schedInfo[3].params = &temperature;
    schedInfo[4].params = &temperature;
    schedInfo[5].params = NULL;
    schedInfo[6].params = NULL;

    // Initialization of counters
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    schedInfo[5].n = 0;
    schedInfo[6].n = 0;

    // Initialization of execution time for each task (t=N*5ms)
    schedInfo[0].N = 1; //5 ms
    schedInfo[1].N = 200; //1000 ms
    schedInfo[2].N = 10; //50 ms
    schedInfo[3].N = 10; //50 ms
    schedInfo[4].N = 40; //200 ms
    schedInfo[5].N = 100; //500 ms
    schedInfo[6].N = 100; //500 ms

    //Timer 1 config (control loop at 10Hz)
    tmr_setup_period(TIMER1, 100);
    //Timer 2 config (Timeout mode timer)
    tmr_setup_period(TIMER2, 5000);

    //Control loop
    while (1) {
        scheduler();
        tmr_wait_period(TIMER1);
    }
    return 0;
}
