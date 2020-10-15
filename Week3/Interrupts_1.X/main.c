/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 15, 2020, 10:44 AM
 */


#include "xc.h"
#include "timer.h"

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt
() {
    IFS0bits.INT0IF = 0; // reset interrupt flag
    LATBbits.LATB1 = !LATBbits.LATB1;
}

int main(void) {
    IEC0bits.INT0IE = 1; // enable INT0 interrupt!
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    tmr_setup_period(TIMER1, 500);
    tmr_setup_period(TIMER2, 250);

    while (1) {
        // code to blink LED
        if (IFS0bits.T2IF == 1) {
            IFS0bits.INT0IF = 1; // set interrupt flag high
            IFS0bits.T2IF = 0;
        }
        if (IFS0bits.T1IF == 1) {
            LATBbits.LATB0 = !LATBbits.LATB0;
            IFS0bits.T1IF = 0;
        }
    }
    return 0;
}
