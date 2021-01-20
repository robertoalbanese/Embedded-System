/*
 * File:   pwm.c
 * Author: ralba
 *
 * Created on December 18, 2020, 6:49 PM
 */


#include "xc.h"
#include "pwm.h"

void pwm_config() {
    PTCONbits.PTMOD = 0; //free running
    PTCONbits.PTCKPS = 0; // 1:1 Prescaler
    PWMCON1bits.PEN2H = 1; //see manual
    PWMCON1bits.PEN3H = 1; //see manual
    //PTPER = [Fcy(=1843200)/Fpwm(=1000Hz)*PrescalerPWM] - 1 must be less then 32767 (15 bits))
    PTPER = (FCYC / (1 * 1000)) - 1; //[Fcy / (1* 1kHz)] - 1
    PTCONbits.PTEN = 1; //Enable PWM
}

void sendPWM(rpm_data* rpm_info) {
    // Define the duty cycle
    rpm_info->dutyCycle1 = (rpm_info->rpm1 - MIN_DC) / (MAX_DC - MIN_DC);
    rpm_info->dutyCycle2 = (rpm_info->rpm2 - MIN_DC) / (MAX_DC - MIN_DC);

    // Assign it to the corresponding output pins
    PDC2 = rpm_info->dutyCycle1 * 2.0 * PTPER;
    PDC3 = rpm_info->dutyCycle2 * 2.0 * PTPER;
}

void satRPM(rpm_data* rpm_info) {

    if (rpm_info->rpm1 > rpm_info->maxRPM) {
        rpm_info->rpm1 = rpm_info->maxRPM;
    } else if (rpm_info->rpm1 < rpm_info->minRPM) {
        rpm_info->rpm1 = rpm_info->minRPM;
    }

    if (rpm_info->rpm2 > rpm_info->maxRPM) {
        rpm_info->rpm2 = rpm_info->maxRPM;
    } else if (rpm_info->rpm2 < rpm_info->minRPM) {
        rpm_info->rpm2 = rpm_info->minRPM;
    }
}