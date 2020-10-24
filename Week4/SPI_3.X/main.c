/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 24, 2020, 4:32 PM
 */

#include <stdio.h>
#include "timer.h"
#include "xc.h"

int main(void) {
    SPI1CONbits.MSTEN = 1; // master mode
    SPI1CONbits.MODE16 = 0; // 8?bit mode
    SPI1CONbits.PPRE = 3; // 1:1 primary prescaler
    SPI1CONbits.SPRE = 6; // 2:1 secondary prescaler
    SPI1STATbits.SPIEN = 1; // enable SPI
    tmr_setup_period(TIMER1, 1000);
    TRISEbits.TRISE8 = 1; //Button S5 input
    int count = 0;
    int prevButt = 1;
    int currButt = 1;

    while (1) {
        currButt = PORTEbits.RE8;
        if ((prevButt^currButt) == 1) {
            if (currButt == 0) {
                count = 0;
                while (SPI1STATbits.SPITBF == 1);
                SPI1BUF = count;
            }
        }
        prevButt = currButt;

        if (IFS0bits.T1IF == 1) {
            IFS0bits.T1IF = 0; // reset timer
            count += 1;
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = count;
        }
    }
    return 0;
}
