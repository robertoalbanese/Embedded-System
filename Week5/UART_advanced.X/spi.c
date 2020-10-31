/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 24, 2020, 4:03 PM
 */

#include <stdio.h>
#include "timer.h"
#include "spi.h"
#include "xc.h"

void spi_config() {
    SPI1CONbits.MSTEN = 1; // master mode
    SPI1CONbits.MODE16 = 0; // 8?bit mode
    SPI1CONbits.PPRE = 3; // 1:1 primary prescaler
    SPI1CONbits.SPRE = 6; // 2:1 secondary prescaler
    SPI1STATbits.SPIEN = 1; // enable SPI
}

void spi_send_char(char c) {
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = c;
}