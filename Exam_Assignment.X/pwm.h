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

void pwm_config(); //Pwm configuration
void sendPWM(rpm_data* rpm_info); // This task computes the duty cycle and send a pwm signal for both the motors
void satRPM(rpm_data* rpm_info); //This task saturates the new received rpm values

#endif