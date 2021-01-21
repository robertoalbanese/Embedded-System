/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */
#ifndef XC_HEADER_CONFIG_H
#define XC_HEADER_CONFIG_H

#include <xc.h> 
#include "uart.h"
#include "pwm.h"
#include "spi.h"

#define MAX_TASKS 7

#define STATE_COMMAND 0
#define STATE_TIMEOUT 1
#define STATE_SAFE 2

//Circular rx buffer for UART data
extern uart_buffer buffer;
//RPM data
extern rpm_data rpm_info;
//Structure deciding which format to display
extern display_lcd display;
//State of the program
extern int state;
//State enumeration
extern char state_info[];

#endif