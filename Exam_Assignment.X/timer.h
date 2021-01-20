/* 
 * File:   timer.h
 * Author:  ralba & andre
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "config.h"
#endif

#define TIMER1 1
#define TIMER2 2
#define TIMER3 3

void tmr_wait_ms(int timer,int ms);
void tmr_setup_period(int timer , int ms);
void tmr_wait_period(int timer);
void choose_prescaler(int ms, int* pr, int* tckps);