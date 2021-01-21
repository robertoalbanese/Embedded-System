/*
 * File:   buttons.c
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */


#include "xc.h"
#include "buttons.h"
#include "config.h"
#include "pwm.h"
#include "spi.h"

// Function enabling interrupts of buttons

void buttons_config() {
    IEC0bits.T3IE = 1; // Enable interrupt of debouncing timer t3
    IFS0bits.INT0IF = 0; // Reset interrupt flag for S5 button
    IFS1bits.INT1IF = 0; // Reset interrupt flag for S6 button
    IEC0bits.INT0IE = 1; // Enable interrupt for S5 button
    IEC1bits.INT1IE = 1; // Enable interrupt for S6 button
}

// S5 button ISR
// If button s5 has been pressed we store the value 0 in the rpm struct

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt() {
    IFS0bits.INT0IF = 0; // Reset interrupt flag
    IEC0bits.INT0IE = 0; // Disable interrupt of button s5
    //Change system state to SAFE
    state = STATE_SAFE;
    //Stop the motors and store the new rpm data in the global struct
    rpm_info.rpm1 = 0;
    rpm_info.rpm2 = 0;
    // Stop asyncronously the motors
    PDC2 = 0.5 * 2.0 * PTPER; //DutyCycle = 0.5 if rpm=0
    PDC3 = 0.5 * 2.0 * PTPER; //DutyCycle = 0.5 if rpm=0

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
    display.index = !display.index;
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