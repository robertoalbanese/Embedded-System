/* 
 * File:   timer.h
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TIMER_H
#define XC_HEADER_TIMER_H

#include <xc.h> // include processor files - each processor file is guarded.

#define TIMER1 1
#define TIMER2 2
#define TIMER3 3

void tmr_wait_ms(int timer,int ms);
void tmr_setup_period(int timer , int ms);
void tmr_wait_period(int timer);
void choose_prescaler(int ms, int* pr, int* tckps);

#endif
