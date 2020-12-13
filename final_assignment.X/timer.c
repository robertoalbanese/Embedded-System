/*
 * File:   timerFunction.c
 * Author: francesco testa
 *
 * Created on 12 ottobre 2020, 9.43
 */

#include "xc.h"
#include "timer.h"

void choose_prescaler(int ms, int* pr, int* tckps);

void choose_prescaler(int ms, int* pr, int* tckps) {
    long ticks = 1843.2 * ms;
    if (ticks <= 65535) {
        //prescaler 1:1
        *pr = ticks;
        *tckps = 0;
        return;
    }
    ticks = ticks / 8;
    if (ticks <= 65535) {
        //prescaler 1:8
        *pr = ticks;
        *tckps = 1;
        return;
    }
    ticks = ticks / 8;
    if (ticks <= 65535) {
        //prescaler 1:64
        *pr = ticks;
        *tckps = 2;
        return;
    }
    ticks = ticks / 4;
    if (ticks <= 65535) {
        //prescaler 1:256 
        *pr = ticks;
        *tckps = 3;
        return;
    }
}

void tmr_wait_ms(int timer, int ms) {
    int pr;
    int tckps;

    switch (timer) {
        case 1:
        {
            T1CONbits.TON = 0;
            TMR1 = 0;
            T1CONbits.TCS = 0;
            choose_prescaler(ms, &pr, &tckps);
            T1CONbits.TCKPS = tckps;
            PR1 = pr;
            T1CONbits.TON = 1;

            //while loop waiting for the flag go up
            while (IFS0bits.T1IF == 0);

            IFS0bits.T1IF = 0; // reset TxIF to 0 again 
            T1CONbits.TON = 0; // otherwise another timer cannot start
            break;
        }

        case 2:
        {
            T2CONbits.TON = 0;
            TMR2 = 0;
            T2CONbits.TCS = 0;
            choose_prescaler(ms, &pr, &tckps);
            T2CONbits.TCKPS = tckps;
            PR2 = pr;
            T2CONbits.TON = 1;

            //while loop waiting for the flag go up
            while (IFS0bits.T2IF == 0);

            IFS0bits.T2IF = 0; // reset TxIF to 0 again 
            T2CONbits.TON = 0; // otherwise another timer cannot start
            break;
        }
    }

}

void tmr_setup_period(int timer, int ms) {
    int pr;
    int tckps;
    if (timer == TIMER1) {
        T1CONbits.TON = 0;
        TMR1 = 0; //reset the timer counter
        choose_prescaler(ms, &pr, &tckps);
        PR1 = pr;
        T1CONbits.TCKPS = tckps; //prescaler 1:64
        T1CONbits.TON = 1; //starts the timer
    } else if (timer == TIMER2) {
        T2CONbits.TON = 0;
        TMR2 = 0; //reset the timer counter
        choose_prescaler(ms, &pr, &tckps);
        PR2 = pr;
        T2CONbits.TCKPS = tckps; //prescaler 1:64
        T2CONbits.TON = 1; //starts the timer
    }
}

void tmr_wait_period(int timer) {
    switch (timer) {
        case TIMER1:
        {
            while (IFS0bits.T1IF == 0);
            IFS0bits.T1IF = 0;
            break;
        }
        case TIMER2:
        {
            while (IFS0bits.T2IF == 0);
            IFS0bits.T2IF = 0;
            break;
        }
    }
}