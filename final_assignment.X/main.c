/*
 * File:   main.c
 * Author: andre & ralba
 *
 * Created on 11 dicembre 2020, 15.36
 */

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

void scheduler();
int snprintf(char * s, size_t n, const char * format, ...);

typedef struct {
    double current;
    double temperature;
} sensor_data;

typedef struct {
    parser_state* parser;
    int velocity;
} velocity_data;

typedef struct {
    void (*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

heartbeat schedInfo[MAX_TASKS];

void* pwm_control_motor(void* params) {
    int* data = (int*) params;
    double dutyCycle = *data / 1000.0;
    PDC2 = 2 * PTPER * dutyCycle;
    return NULL;
}

void* task_uart_reciver(void* params) {
    velocity_data* new_data = (velocity_data*) params;
    int tmp = NO_MESSAGE;
    //Check if an overflow in the receive buffer is occurred
    if (U1STAbits.OERR == 1) {
        //Reset overflow flag
        U1STAbits.OERR = 0;
    }

    while (U1STAbits.URXDA == 1 && tmp == NO_MESSAGE) {
        tmp = parse_byte(new_data->parser, U1RXREG);
    }
    if (tmp == NEW_MESSAGE && strcmp(new_data->parser->msg_type, "MCREF") == 0 && sizeof (new_data->parser->msg_payload)>sizeof (char)) {
        new_data->velocity = atoi(new_data->parser->msg_payload);
    }
    return NULL;
}

void* task_uart_transmitter(void* params) {
    sensor_data* new_data = (sensor_data*) params;
    int i;
    char output_string [100];
    snprintf(output_string, sizeof (output_string), "MCFBK,%f,%f*", new_data->current, new_data->temperature);

    for (i = 0; i<sizeof (output_string); i++) {
        U2TXREG = output_string[i];
    }
    return NULL;
}

void* get_sensor_data(void* params) {
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
    //PDC2 = 2 * PTPER *dutyCycle;
    return NULL;
}

void* task_led_blinkD3() {
    LATBbits.LATB0 = !LATBbits.LATB0;
    return NULL;
}

void* task_led_blinkD4(void* params) {
    sensor_data* sensors = (sensor_data*) params;
    if (sensors->current >= 15)
        LATBbits.LATB1 = 1;
    else
        LATBbits.LATB1 = 0;
    return NULL;
}

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
    //timer config
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER1, 5);

    // parser initialization
    velocity_data velocity_msg;
    velocity_msg.parser->state = STATE_DOLLAR;
    velocity_msg.parser->index_type = 0;
    velocity_msg.parser->index_payload = 0;
    
    // sensor data struct
    sensor_data sensors;

    schedInfo[0].task = &pwm_control_motor;
    schedInfo[1].task = &task_uart_transmitter;
    schedInfo[2].task = &task_uart_reciver;
    schedInfo[3].task = &get_sensor_data;
    schedInfo[4].task = &task_led_blinkD3;
    schedInfo[5].task = &task_led_blinkD4;

    schedInfo[0].params = &velocity_msg.velocity;
    schedInfo[1].params = &sensors;
    schedInfo[2].params = &velocity_msg;
    schedInfo[3].params = &sensors;
    schedInfo[4].params = NULL;
    schedInfo[5].params = &sensors;

    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    schedInfo[5].n = 0;

    schedInfo[0].N = 4; //20 ms
    schedInfo[1].N = 200; //1000 ms
    schedInfo[2].N = 16; //80 ms
    schedInfo[3].N = 40; //200 ms (oppure 300 ms (60))
    schedInfo[4].N = 20; //100 ms
    schedInfo[5].N = 20; //100 ms

    while (1) {
        //scheduler(schedInfo);
        scheduler();
        tmr_wait_period(TIMER1);
    }
    return 0;
}