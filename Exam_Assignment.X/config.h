/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 18, 2021, 10:59 AM
 */
#ifndef XC_HEADER_TEMPLATE_H
#define XC_HEADER_TEMPLATE_H

#include <xc.h> 

#endif

#define MAX_TASKS 7
#define TXDIM 100
#define FCYC 1843200
#define MIN_DC (-10000)
#define MAX_DC (+10000)
#define MAX_RPM (+8000)
#define MIN_RPM (-8000)

#define STATE_COMMAND 0
#define STATE_TIMEOUT 1
#define STATE_SAFE 2

#define STATE_DOLLAR  (1) // we discard everything until a dollar is found
#define STATE_TYPE    (2) // we are reading the type of msg until a comma is found
#define STATE_PAYLOAD (3) // we read the payload until an asterix is found
#define NEW_MESSAGE (1) // new message received and parsed completely
#define NO_MESSAGE (0) // no new messages

typedef struct {
    void (*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

// Stores rpm values to control DC motors

typedef struct {
    int rpm1;
    int rpm2;
    int maxRPM;
    int minRPM;
    double dutyCycle1;
    double dutyCycle2;
} rpm_data;

typedef struct {
    int buffer[TXDIM];
    int headIndex;
    int tailIndex;
    int unreadData;
} uart_buffer;

typedef struct {
    double temp_records[10]; //FIFO buffer
    double temperature;
    double average;
} temperature_info;

typedef struct {
    temperature_info* temp_info; //FIFO buffer
    rpm_data* rpm_info;
} display_info;

typedef struct {
    void (*format[2])(display_info*);
    display_info* display_info;
    int index;
} display_lcd;

typedef struct { 
	int state;
	char msg_type[6]; // type is 5 chars + string terminator
	char msg_payload[100];  // assume payload cannot be longer than 100 chars
	int index_type;
	int index_payload;
} parser_state;

typedef struct {
    uart_buffer* buffer;
    parser_state* pstate;
    rpm_data* rpm;
    temperature_info* temperature;
    display_lcd* display;
} program_info;

//Circular rx buffer for UART data
uart_buffer buffer;
//Timeout flag
int timeout_flag;
//S5 button pressed flag
int s5_flag;
//S6 button pressed flag
int s6_flag;
//State of the program
int state;
//State enumeration
char state_info[3] = {'C', 'T', 'H'};