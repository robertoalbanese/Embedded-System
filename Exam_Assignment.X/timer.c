/*
 * File:   timerFunction.c
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#include "xc.h"
#include "timer.h"
#include "config.h"

// Timer 2 ISR - Set motor velocity to zeros and blink led D4
// After 5 secs with no reference signals we store the value in the struct and we pass to TIMEOUT MODE

void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt() {
    IEC0bits.T2IE = 0; // Disable interrupt of timer t2
    // Set timeout state
    state = STATE_TIMEOUT;
    //Stop the motors and store the new rpm data in the global struct
    rpm_info.rpm1 = 0;
    rpm_info.rpm2 = 0;
    // Stop asynchronously the motors
    PDC2 = 0.5 * 2.0 * PTPER; //DutyCycle = 0.5 if rpm=0
    PDC3 = 0.5 * 2.0 * PTPER; //DutyCycle = 0.5 if rpm=0

    IFS0bits.T2IF = 0; // Reset interrupt flag for timer 2
    T2CONbits.TON = 0; // Stop the timer
}

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
        T1CONbits.TCKPS = tckps; //prescaler
        T1CONbits.TON = 1; //starts the timer
    } else if (timer == TIMER2) {
        T2CONbits.TON = 0;
        TMR2 = 0; //reset the timer counter
        choose_prescaler(ms, &pr, &tckps);
        PR2 = pr;
        T2CONbits.TCKPS = tckps; //prescaler
        T2CONbits.TON = 1; //starts the timer
        IEC0bits.T2IE = 1; // Enable interrupt of timer t2
        IFS0bits.T2IF = 0; // Set the timer flag to zero to be notified of a new event
        //Timeout interrupt flag intialization
    } else if (timer == TIMER3) {
        T3CONbits.TON = 0;
        TMR3 = 0; //reset the timer counter
        choose_prescaler(ms, &pr, &tckps);
        PR3 = pr;
        T3CONbits.TCKPS = tckps; //prescaler
        T3CONbits.TON = 1; //starts the timer
        IEC0bits.T3IE = 1; // Enable interrupt of timer t3
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