/*
 * File:   main.c
 * Author: andre & ralba
 *
 * Created on 11 dicembre 2020, 15.36
 */

// DSPIC30F4011 Configuration Bit Settings

// 'C' source line config statements

// FOSC
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_TASKS 6

// Defines a struct in which it is possible to store and read current and temperature values 

typedef struct {
    double current;
    double temperature;
} sensor_data;

// Stores messages received from UART and rpm value for controlling DC motors

typedef struct {
    parser_state* parser;
    int velocity;
} velocity_data;

//conficuration struct of tasks

typedef struct {
    void (*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

heartbeat schedInfo[MAX_TASKS];

//This task receives an rpm value and uses it to generate a pwm signal 

void* task_pwm_control_motor(void* params) {
    int* data = (int*) params;
    double dutyCycle = *data / 1000.0; //The duty cycle varies from 0 to the duration of the period
    PDC2 = 2 * PTPER * dutyCycle;
    return NULL;
}

void* task_uart_reciver(void* params) {
    velocity_data* new_data = (velocity_data*) params;
    int tmp = NO_MESSAGE;
    //Check if an overflow in the receive buffer is occurred
    if (U1STAbits.OERR == 1)
        U1STAbits.OERR = 0; //Reset overflow flag

    while (U1STAbits.URXDA == 1 && tmp == NO_MESSAGE)
        tmp = parse_byte(new_data->parser, U1RXREG);
    //Store the data of the velocity (rpm) only if a MCREF message with a non empty payload has been recived.
    if (tmp == NEW_MESSAGE && strcmp(new_data->parser->msg_type, "MCREF") == 0 && strlen(new_data->parser->msg_payload) > 0)
        new_data->velocity = atoi(new_data->parser->msg_payload);
    return NULL;
}

// This task transmits the values of current(AN2) and temperature(AN3) to the UART

void* task_uart_transmitter(void* params) {
    sensor_data* new_data = (sensor_data*) params;
    int i;
    //ASCII protocol string builder
    char output_string[20];
    sprintf(output_string, "$MCFBK,%2.1f,%2.1f*", new_data->current, new_data->temperature);
    // Sending ASCII message to UART
    for (i = 0; i < strlen(output_string); i++) {
        while (U1STAbits.UTXBF == 1);
        U1TXREG = output_string[i];
    }
    return NULL;
}

//This task reads from the potentiometer and the temperature sensor data 

void* task_get_sensor_data(void* params) {
    sensor_data* sensors = (sensor_data*) params;

    while (ADCON1bits.DONE == 0); //wait until  the conversion done
    ADCON1bits.DONE = 0;

    //Potentiometer data, current conversion
    int potBits = ADCBUF0;
    double potVolts = 3.0 + 2.0 * (double) potBits / 1024.0; // voltage of signal [3:5]V
    sensors->current = (potVolts - 3.0)*10.0;

    // Temperature data
    int tempBits = ADCBUF1;
    double tempVolts = (double) tempBits * 5.0 / 1024.0;
    sensors->temperature = (tempVolts - 0.75)*100.0 + 25.0;
    return NULL;
}

// This task will blink the led D3 to let the user the correct execution of the program

void* task_led_blinkD3() {
    LATBbits.LATB0 = !LATBbits.LATB0;
    return NULL;
}

// This task will turn on the led D4 if the current value exceeds 15A 

void* task_led_blinkD4(void* params) {
    sensor_data* sensors = (sensor_data*) params;
    if (sensors->current >= 15)
        LATBbits.LATB1 = 1;
    else
        LATBbits.LATB1 = 0;
    return NULL;
}

//Core of the program.

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
    // LED config
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    //timer config (control loop at 200Hz)
    tmr_setup_period(TIMER1, 5);

    // velocity_data struct initialization
    velocity_data velocity_msg;
    velocity_msg.parser->state = STATE_DOLLAR;
    velocity_msg.parser->index_type = 0;
    velocity_msg.parser->index_payload = 0;
    velocity_msg.velocity = 0;

    // sensor data struct
    sensor_data sensors;

    // Assignment of task function
    schedInfo[0].task = &task_pwm_control_motor;
    schedInfo[1].task = &task_uart_transmitter;
    schedInfo[2].task = &task_uart_reciver;
    schedInfo[3].task = &task_get_sensor_data;
    schedInfo[4].task = &task_led_blinkD3;
    schedInfo[5].task = &task_led_blinkD4;

    // Assignment of input parameters for each task
    schedInfo[0].params = &velocity_msg.velocity;
    schedInfo[1].params = &sensors;
    schedInfo[2].params = &velocity_msg;
    schedInfo[3].params = &sensors;
    schedInfo[4].params = NULL;
    schedInfo[5].params = &sensors;

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
    schedInfo[3].N = 40; //200 ms (oppure 300 ms (60))
    schedInfo[4].N = 200; //1000 ms
    schedInfo[5].N = 20; //100 ms

    //Control loop
    while (1) {
        scheduler();
        tmr_wait_period(TIMER1);
    }
    return 0;
}