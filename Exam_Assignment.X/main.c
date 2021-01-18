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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    int buffer[TXDIM];
    int headIndex;
    int tailIndex;
    int unreadData;
} UARTBuffer;

UARTBuffer reciverBuffer;

typedef struct {
    double average[10]; //FIFO buffer
    double temperature;
} temperature_info;

// Defines a struct in which it is possible to store and read current and temperature values



// Stores rpm values to control DC motors

typedef struct {
    int rpm1;
    int rpm2;
    int maxRPM;
    int minRPM;
} rpm_data;

rpm_data velocity;
//Configuration struct of tasks

typedef struct {
    void (*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

heartbeat schedInfo[MAX_TASKS];

// Parser state variable
parser_state pstate;

int state;
//This task receives an rpm value and uses it to generate a pwm signal

void* task_pwm_control_motor(void* params) {
    rpm_data* data = (rpm_data*) params;

    int rpm1 = data->rpm1;
    int rpm2 = data->rpm2;

    // Define the duty cycle
    int dutyCycle1 = (rpm1 - MIN_DC) / (MAX_DC - MIN_DC);
    int dutyCycle2 = (rpm2 - MIN_DC) / (MAX_DC - MIN_DC);
    // Assign it to the corresponding output pins
    PDC2 = dutyCycle1 * 2.0 * PTPER;
    PDC3 = dutyCycle2 * 2.0 * PTPER;

    return 0;
}

void* task_uart_reciver(void* params) {

    UARTBuffer* newBuffer = (UARTBuffer*) params;
    char temp;
    int parser;

    while (UART_buffDim(&newBuffer) > 0) {
        temp = UART_readOnBuffer(&newBuffer);

        parser = parse_byte(&pstate, temp);
        if (parser == NEW_MESSAGE) {
            int tempRPM1, tempRPM2;
            // Message Decoding Routine
            if (strcmp(pstate->msg_type, "HLREF") == 0) {
                sscanf(pstate->msg_payload, "%d,%d", &tempRPM1, &tempRPM2);
                // Saturate RPM if they're above the allowed threshold
                tempRPM1 = satRPM(tempRPM1);
                tempRPM2 = satRPM(tempRPM2);

                velocity->rpm1 = tempRPM1;
                velocity->rpm2 = tempRPM2;

            } else if (strcmp(pstate->msg_type, "HLSAT") == 0) {
                int tempMax, tempMin;
                sscanf(pstate->msg_payload, "%d,%d", &tempMax, &tempMin);
                if (tempMax < MAX_RPM && tempMax > 0 && tempMin > MIN_RPM && tempMin < 0 && tempMin < tempMax) {
                    velocity.maxRPM = tempMax;
                    velocity.minRPM = tempMin;
                } else {
                    //return negative act error
                }
            } else if (strcmp(pstate->msg_type, "HLENA") == 0) {
                //if self mode, than return to command mode
            }
        }
    }
    
    return NULL;
    /*velocity_data* new_data = (velocity_data*) params;
    int tmp = NO_MESSAGE;
    //Check if an overflow in the receive buffer is occurred
    if (U1STAbits.OERR == 1)
        U1STAbits.OERR = 0; //Reset overflow flag

    while (U1STAbits.URXDA == 1)
        tmp = parse_byte(new_data->parser, U1RXREG);
    //Store the data of the velocity (rpm) only if a MCREF message with a non empty payload has been recived.
    if (tmp == NEW_MESSAGE && strcmp(new_data->parser->msg_type, "MCREF") == 0 && strlen(new_data->parser->msg_payload) > 0){
        new_data->velocity = atoi(new_data->parser->msg_payload);
        if (new_data->velocity > 1000)
            new_data->velocity = 1000; // Saturation if greater then 1000 rpm
    }
        return NULL;*/
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
    return NULL;
}

int main(void) {
    // device config
    adc_config();
    pwm_config();
    UART_config();
    UART_bufferInit(&reciverBuffer);
    // LED config
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    // Initialization of velocity limits
    velocity.maxRPM = MAX_RPM;
    velocity.minRPM = MIN_RPM;
    //Temperature
    temperature_info temperature;
    temperature.temperature = 0;

    state = STATE_COMMAND;

    // velocity_data struct initialization
    /*velocity_data velocity_msg;
    velocity_msg.parser->state = STATE_DOLLAR;
    velocity_msg.parser->index_type = 0;
    velocity_msg.parser->index_payload = 0;
    velocity_msg.velocity = 0;*/

    // Assignment of task function
    schedInfo[0].task = &task_pwm_control_motor;
    schedInfo[1].task = &task_uart_reciver;
    schedInfo[2].task = &task_acquire_temperature;
    schedInfo[3].task = &task_average_temperature;
    schedInfo[4].task = &task_led_blink;
    schedInfo[5].task = &task_print_lcd;

    // Assignment of input parameters for each task
    schedInfo[0].params = &velocity;
    schedInfo[1].params = &reciverBuffer;
    schedInfo[2].params = &temperature;
    schedInfo[3].params = &temperature;
    schedInfo[4].params = &temperature;
    schedInfo[5].params = NULL;

    // Inizialization of counters
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    schedInfo[5].n = 0;

    // Initialization of execution time for each task (t=N*5ms)
    schedInfo[0].N = 1; //5 ms
    schedInfo[1].N = 200; //1000 ms
    schedInfo[2].N = 10; //50 ms
    schedInfo[3].N = 10; //50 ms
    schedInfo[4].N = 40; //200 ms
    schedInfo[5].N = 100; //500 ms

    //Control loop
    while (1) {
        scheduler();
        tmr_wait_period(TIMER1);
    }
    return 0;
}
