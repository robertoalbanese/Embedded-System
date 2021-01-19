/* 
 * File:   
 * Author: ralba & andre
 * Comments:
 * Revision history: 
 */


#include <xc.h> // include processor files - each processor file is guarded.  

void pwm_config();
int sendPWM(int* rpm1,int* rpm2);
int satRPM(int rpm);
