/*
 * File:   adc.c
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#include "xc.h"

// Configuration of A/D converter 

void adc_config() {

    ADCON3bits.ADCS = 8; //setting TAD

    //Automatic sampling and automatic conversion start
    ADCON1bits.ASAM = 1; // Set auto start
    ADCON1bits.SSRC = 7;
    ADCON3bits.SAMC = 1; //1 Tad
    // channel selection to convert - select channel 0
    ADCON2bits.CHPS = 0;

    ADCHSbits.CH0SA = 2;
    ADCHSbits.CH0NA = 0;
    //ADCHSbits = CH0SA = 3;
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG2 = 0;
    //turn on the peripheral
    ADCON1bits.ADON = 1;
}