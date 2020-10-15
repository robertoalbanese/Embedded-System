/* 
 * File:   timer.h
 * Author:  ralba
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.

#define	XC_HEADER_TEMPLATE_H
#define TIMER1 1
#define TIMER2 2

void tmr_wait_ms(int timer,int ms);
void tar_setup_period(int timer , int ms);
void tar_wait_period(int timer);

#endif /* __cplusplus */

// TODO If C++ is being used, regular C code needs function names to have C 
// linkage so the functions can be used by the c code. 


