/*
 * File:   buttons.c
 * Author: ralba
 *
 * Created on January 19, 2021, 3:39 PM
 */


#include "xc.h"
#include "buttons.h"
#include "config.h"

// Function enabling interrupts of buttons

void buttons_config() {
    IEC0bits.T3IE = 1; // Enable interrupt of debouncing timer t3
    IFS0bits.INT0IF = 0; // Reset interrupt flag for S5 button
    IFS1bits.INT1IF = 0; // Reset interrupt flag for S6 button
    IEC0bits.INT0IE = 1; // Enable interrupt for S5 button
    IEC1bits.INT1IE = 1; // Enable interrupt for S6 button
    s5_flag = 0;
    s6_flag = 0;
}

// S5 button ISR

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt() {
    IFS0bits.INT0IF = 0; // Reset interrupt flag
    IEC0bits.INT0IE = 0; // Disable interrupt of button s5
    //Set the flag high
    s5_flag = 1;
    //Change system state to SAFE
    state = STATE_SAFE;
    
    // Stop asyncronously the motors
    PDC2 = 0.5 * 2.0 * PTPER;
    PDC3 = 0.5 * 2.0 * PTPER;
    
    // Stop timeout mode timer
    T2CONbits.TON = 0;
    IEC0bits.T2IE = 0; // Disable interrupt for timeout mode
    IFS0bits.T2IF = 0; // Set the flag = 0

    tmr_setup_period(TIMER3, 50); // Start debouncing timer
}

// S6 buttons ISR

void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt() {
    IFS1bits.INT1IF = 0; // Reset interrupt flag
    IEC1bits.INT1IE = 0; // Disable interrupt of button s6
    //Set the flag high
    s6_flag = 1;    
    tmr_setup_period(TIMER3, 50); // Start debouncing timer
}

// Debouncing timer ISR

void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt() {
    IFS0bits.T3IF = 0; // Reset interrupt flag of timer 3
    IFS0bits.INT0IF = 0; // Reset interrupt flag of button s5
    IFS1bits.INT1IF = 0; // Reset interrupt flag of button s6
    IEC0bits.INT0IE = 1; // Enable interrupt of button s5
    IEC1bits.INT1IE = 1; // Enable interrupt of button s6
    T3CONbits.TON = 0; // Stop debouncing timer
}