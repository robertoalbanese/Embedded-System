/*
 * File:   main.c
 * Author: Andrea Tiranti
 *
 * Created on October 24, 2020, 4:03 PM
 */

#include <stdio.h>
#include "spi.h"
#include "xc.h"

//SPI configuration routine

void spi_config() {
    SPI1CONbits.MSTEN = 1; // master mode
    SPI1CONbits.MODE16 = 0; // 8 bit mode
    SPI1CONbits.PPRE = 3; // 1:1 primary prescaler
    SPI1CONbits.SPRE = 6; // 2:1 secondary prescaler
    SPI1STATbits.SPIEN = 1; // enable SPI
}

//Routine to send a character to the LCD
//  Input: 
//      char c: character to be sent

void spi_send_char(char c) {
    //Wait if the transmit buffer is still full
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = c;
}

//Routine to clear a row in the LCD
//  Input: 
//      char row: row (0x80 for first row, 0xC0 for second row)

void spi_clear_row(char row) {
    int i = 0;
    //Move to the first index of the row
    spi_send_char(row);
    //Send ' ' character to clean the row 
    while (i < 16) {
        spi_send_char(' ');
        i++;
    }
    //Move to the first index of the row
    spi_send_char(row);
}

//Routine to send a string to the LCD
//  Input: 
//      char s: string to be sent

void spi_send_string(char *s) {
    int i;
    //Do until the end of the string is reached
    while (s[i] != '\0') {
        if (i == 17) {
            //Send the character to the LCD
            spi_send_char(s[i]);
        }
        i++;
    }
}

//Routine to print in the second row "Char Recv: %d"
//  Input: 
//      int counter : current number of character received

void spi_uart_print_second_row(int count) {
    char second_row[16];
    //Move to the first index of the second row
    spi_send_char(0xC0);
    //Build the string to be displayed
    sprintf(second_row, "Char Recv: %d", count);
    //Send the string
    spi_send_string(second_row);
}