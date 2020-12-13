/*
 * File:   main.c
 * Author: andre
 *
 * Created on 11 dicembre 2020, 15.36
 */


#include "xc.h"
#include "timer.h"
#include "spi.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_TASKS 5
void adc_config();
void pwm_config();
void scheduler();

typedef struct {
    void (*task)(void*);
    void *params;
    int n;
    int N;
} heartbeat;

//heartbeat schedInfo[MAX_TASKS];

typedef struct {
    char* longString;
    int startIndex;
    int written; //count bit writeen
    int active; //boolean
    int blanks;
} slidingInfo;

void* control_motor(void* params){
    return NULL; 
}
void* task_uart_comm(void* params){
    return NULL;
}
void* task_current_sim(void* params){  
        while(ADCON1bits.DONE == 0);//wait until  the conversion done
        ADCON1bits.DONE = 0;
        int potBits = ADCBUF0;
        double voltage = 3 + 2 * potBits/1024.0; // voltage of signal [3:5]V
        double current = (voltage - 3)*10; 
        //PDC2 = 2 * PTPER *dutyCycle;
    return NULL;
}
void* task_led_blinkD3(void* params){
    
    LATBbits.LATB0 = !LATBbits.LATB0;
    return NULL;
}

void* task_led_blinkD4(void* params){
    
    LATBbits.LATB1 = !LATBbits.LATB1;
    return NULL;
}
int main(void) {
    
    
    // device config
    spi_config();
    adc_config();
    pwm_config();
    // LED config
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISEbits.TRISE8 = 1;
    TRISDbits.TRISD0 = 1;
    //timer config
    tmr_wait_ms(TIMER1, 1000);
    tmr_setup_period(TIMER1, 5);
    
    heartbeat schedInfo[MAX_TASKS];
    schedInfo[0].task = &control_motor;
    schedInfo[1].task = &task_uart_comm;
    schedInfo[2].task = &task_current_sim;
    schedInfo[3].task = &task_led_blinkD3;
    schedInfo[4].task = &task_led_blinkD4;
    
    schedInfo[0].params = NULL;
    schedInfo[1].params = NULL;
    schedInfo[2].params = NULL;
    schedInfo[3].params = NULL;
    schedInfo[4].params = NULL;
    
    schedInfo[0].n = 0;
    schedInfo[1].n = 0;
    schedInfo[2].n = 0;
    schedInfo[3].n = 0;
    schedInfo[4].n = 0;
    
    schedInfo[0].N = 1;
    schedInfo[1].N = 50;
    schedInfo[2].N = 100;
    schedInfo[3].N = 4;
    schedInfo[4].N = 4;
    
    while(1) {
        scheduler(&schedInfo);
        tmr_wait_period(TIMER1);
    }
    return 0;
}

void scheduler(heartbeat* schedInfo){
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].n >= schedInfo[i].N) {
            schedInfo[i].task(schedInfo[i].params);
            schedInfo[i].n = 0;
            }
        }
    }


void adc_config()
{
    
    ADCON3bits.ADCS = 8; //setting TAD
    
    //manual sampling and manual conversion start
    ADCON1bits.ASAM = 1; //how sampling start
    ADCON1bits.SSRC = 7;
    ADCON3bits.SAMC = 1; //1 Tad
    // channel selection to convert - select channel 0
    ADCON2bits.CHPS= 1;
    
    ADCHSbits.CH0SA = 2;
    ADCHSbits.CH0NA = 0;
    //ADCHSbits = CH0SA = 3;
    ADCHSbits.CH123SA = 1;//channel 1 pos INput in An3 
    ADCHSbits.CH123NA = 0;
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG2 = 0;
    ADPCFGbits.PCFG3 = 0;
    //turn on the peripheral
    ADCON1bits.ADON = 1;
    
   
}
void pwm_config(){
    
    PTCONbits.PTMOD = 0; //free running
    PTCONbits.PTCKPS = 1; // 1:4 Prescaler
    PWMCON1bits.PEN2H = 1; //see manual
    // se manual, anyway PTPER = [Fcy(=1843200)/Fpwm(=50Hz)*PrescalerPWM] - 1
    // NOT ENOUGH BIT FOR 36863 since 15 bit PTPER, so we have to use the prescaler(i.e 1:4)
    PTPER = 9215; // [Fcy / (4* 50Hz)] - 1
    PTCONbits.PTEN = 1;//Enable PWM
}