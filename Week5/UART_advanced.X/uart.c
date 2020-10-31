/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 30, 2020, 4:32 PM
 */

#include "timer.h"
#include "spi.h"
#include "xc.h"

void U2ART_config() {
    U2BRG = 11; // ((1843200) / (16 * 9600)) - 1
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;
}
