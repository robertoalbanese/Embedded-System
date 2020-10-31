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

void exercise_1() {
    tmr_wait_ms(TIMER1, 1000);
    int c;

    spi_config();
    U2ART_config();

    while (1) {
        if (U2STAbits.URXDA == 1) {
            c = U2RXREG;
        }

        spi_send_char(c);
    }
}

void exercise_2() {
    U2ART_config();

    while (1) {
        if (U2STAbits.URXDA == 1) {
            U2TXREG = U2RXREG;
        }
    }
}

void exercise_3() {
    tmr_wait_ms(TIMER1, 1000);

    spi_config();
    U2ART_config();

    while (1) {
        if (U2STAbits.URXDA == 1) {
            U2TXREG = U2RXREG;
            spi_send_char(U2RXREG);
        }
    }
}

int main(void) {
    exercise_1();
    return 0;
}
