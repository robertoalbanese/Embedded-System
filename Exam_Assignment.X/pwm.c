/*
 * File:   pwm.c
 * Author: ralba
 *
 * Created on December 18, 2020, 6:49 PM
 */


#include "xc.h"
#include "config.h"

void pwm_config() {
    PTCONbits.PTMOD = 0; //free running
    PTCONbits.PTCKPS = 0; // 1:1 Prescaler
    PWMCON1bits.PEN2H = 1; //see manual
    PWMCON1bits.PEN3H = 1; //see manual
    //PTPER = [Fcy(=1843200)/Fpwm(=1000Hz)*PrescalerPWM] - 1 must be less then 32767 (15 bits))
    PTPER = (FCYC/(1*1000))-1; //[Fcy / (1* 1kHz)] - 1
    PTCONbits.PTEN = 1; //Enable PWM
}

int sendPWM(int* rpm1,int* rpm2)
{
    // Define the duty cycle
    velocity->dutyCycle1 = (*rpm1 - MIN_DC) / (MAX_DC - MIN_DC);
    velocity->dutyCycle2 = (*rpm2 - MIN_DC) / (MAX_DC - MIN_DC);
    
    // Assign it to the corresponding output pins
    PDC2 = velocity->dutyCycle1 * 2.0 * PTPER;
    PDC3 = velocity->dutyCycle2 * 2.0 * PTPER;
    
    return 0;
}

int satRPM(int rpm) {
    
    if(rpm > velocity.maxRPM) {
        rpm = velocity.maxRPM;
    } else if (rpm < velocity.minRPM) {
        rpm = velocity.minRPM;
    }
    return rpm;
}