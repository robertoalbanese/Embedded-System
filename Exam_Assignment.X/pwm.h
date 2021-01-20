/* 
 * File:   
 * Author: ralba & andre
 * Comments:
 * Revision history: 
 */

#ifndef XC_HEADER_PWM_H
#define XC_HEADER_PWM_H

#include <xc.h> // include processor files - each processor file is guarded. 

// Stores rpm values to control DC motors

typedef struct {
    int rpm1;
    int rpm2;
    int maxRPM;
    int minRPM;
    double dutyCycle1;
    double dutyCycle2;
} rpm_data;

void pwm_config();
void sendPWM(rpm_data* rpm_info);
void satRPM(rpm_data* rpm_info);

#endif