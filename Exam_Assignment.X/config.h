/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 18, 2021, 10:59 AM
 */
#ifndef XC_HEADER_CONFIG_H
#define XC_HEADER_CONFIG_H

#include <xc.h> 
#include "uart.h"

#define MAX_TASKS 7
#define TXDIM 100

#define MAX_RPM (+8000)
#define MIN_RPM (-8000)

#define STATE_COMMAND 0
#define STATE_TIMEOUT 1
#define STATE_SAFE 2

//Circular rx buffer for UART data
extern uart_buffer buffer;
//Timeout flag
extern int timeout_flag;
//S5 button pressed flag
extern int s5_flag;
//S6 button pressed flag
extern int s6_flag;
//State of the program
extern int state;
//State enumeration
extern char state_info[];

#endif