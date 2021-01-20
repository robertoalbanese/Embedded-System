/* 
 * File:   
 * Author: ralba & andre
 * Comments:
 * Revision history: 
 */

#ifndef XC_HEADER_TEMPLATE_H
#define XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include "config.h"

#endif
void pwm_config();
void sendPWM(rpm_data* rpm_info);
void satRPM(rpm_data* rpm_info);