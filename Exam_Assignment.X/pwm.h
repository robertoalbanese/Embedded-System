/* 
 * File:   
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#ifndef XC_HEADER_PWM_H
#define XC_HEADER_PWM_H

#include <xc.h> // include processor files - each processor file is guarded. 

#define MAX_RPM (+8000)
#define MIN_RPM (-8000)

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