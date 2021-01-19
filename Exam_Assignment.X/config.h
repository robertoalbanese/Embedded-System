/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 18, 2021, 10:59 AM
 */

#include <xc.h> 
#include "parser.h"

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
    double average[10]; //FIFO buffer
    double temperature;
} temperature_info;

typedef struct {
    void (*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

typedef struct {
    void (*format[2])();
    int index;
}display_lcd;

//Tasks info structure
heartbeat schedInfo[MAX_TASKS];
//RPM data
extern rpm_data velocity;
//Temperature informations
extern temperature_info temperature;
//Circular rx buffer for UART data
uart_buffer reciverBuffer;
// Parser state variable
parser_state pstate;
//State of the program
extern int state;
//Structure deciding which format to display
extern display_lcd display;
//State enumeration
extern char state_info[3]={'C','T','H'};