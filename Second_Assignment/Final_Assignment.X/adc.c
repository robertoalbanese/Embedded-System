/*
 * File:   adc.c
 * Author: ralba & andre
 *
 * Created on December 18, 2020, 6:46 PM
 */

#include "xc.h"

void adc_config() {

    ADCON3bits.ADCS = 8; //setting TAD

    //manual sampling and manual conversion start
    ADCON1bits.ASAM = 1; //how sampling start
    ADCON1bits.SSRC = 7;
    ADCON3bits.SAMC = 1; //1 Tad
    // channel selection to convert - select channel 0
    ADCON2bits.CHPS = 1;

    ADCHSbits.CH0SA = 2;
    ADCHSbits.CH0NA = 0;
    //ADCHSbits = CH0SA = 3;
    ADCHSbits.CH123SA = 1; //channel 1 pos INput in An3 
    ADCHSbits.CH123NA = 0;
    ADPCFG = 0xFFFF;
    ADPCFGbits.PCFG2 = 0;
    ADPCFGbits.PCFG3 = 0;
    //turn on the peripheral
    ADCON1bits.ADON = 1;
}