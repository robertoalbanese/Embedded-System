/*
 * File:   uart.c
 * Author: andre
 *
 * Created on 11 dicembre 2020, 15.48
 */


#include "xc.h"

void UART2_config() {
    U2BRG = 11; // ((1843200) / (16 * 9600)) - 1
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;
}
