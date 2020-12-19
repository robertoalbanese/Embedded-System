/*
 * File:   pwm.c
 * Author: ralba
 *
 * Created on December 18, 2020, 6:49 PM
 */


#include "xc.h"

void pwm_config() {
    PTCONbits.PTMOD = 0; //free running
    PTCONbits.PTCKPS = 1; // 1:4 Prescaler
    PWMCON1bits.PEN2H = 1; //see manual
    // se manual, anyway PTPER = [Fcy(=1843200)/Fpwm(=50Hz)*PrescalerPWM] - 1
    // NOT ENOUGH BIT FOR 36863 since 15 bit PTPER, so we have to use the prescaler(i.e 1:4)
    PTPER = 9215; // [Fcy / (4* 50Hz)] - 1
    PTCONbits.PTEN = 1; //Enable PWM
}
