/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 30, 2020, 4:32 PM
 */

#include "timer.h"
#include "xc.h"

void setup_SPI() {
    SPI1CONbits.MSTEN = 1; // master mode
    SPI1CONbits.MODE16 = 0; // 8?bit mode
    SPI1CONbits.PPRE = 3; // 1:1 primary prescaler
    SPI1CONbits.SPRE = 6; // 2:1 secondary prescaler
    SPI1STATbits.SPIEN = 1; // enable SPI
}

void exercise_1() {
    tmr_wait_ms(TIMER1, 1000);
    int c = 0;

    setup_SPI();

    U2BRG = 11; // ((1843200) / (16 * 9600)) - 1
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;

    while (1) {
        if (U2STAbits.URXDA == 1) {
            c = U2RXREG;
        }

        while (SPI1STATbits.SPITBF == 1);
        SPI1BUF = c;
    }
}

void exercise_2() {
}

void exercise_3() {
}

int main(void) {
    exercise_1();
    return 0;
}
