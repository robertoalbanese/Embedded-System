/*
 * File:   main.c
 * Author: Andrea Tiranti
 *
 * Created on October 24, 2020, 4:03 PM
 */

#include <stdio.h>
#include "spi.h"
#include "config.h"
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

void spi_clear_lcd() {
    int i = 0;
    //Move to the first index of the first row
    spi_send_char(0x80);
    //Send ' ' character to clean the row 
    while (i < 16) {
        spi_send_char(' ');
        i++;
    }
    //Move to the first index of the second row
    spi_send_char(0xC0);
    //Send ' ' character to clean the row 
    while (i < 16) {
        spi_send_char(' ');
        i++;
    }
}

//Routine to send a string to the LCD
//  Input: 
//      char s: string to be sent

void spi_send_string(char *s, char row) {
    int i;
    spi_send_char(row);
    //Do until the end of the string is reached
    while (s[i] != '\0') {
        //Send the character to the LCD
        spi_send_char(s[i]);
        i++;
    }
}

void* displayCaseA() {
    char first_row[16];
    char second_row[16];

    spi_clear_lcd();
    //Temperature Display
    sprintf(first_row, "ST:%c;T:%2.1f", state_info[state], temperature->average);
    spi_send_string(first_row, 0x80);
    //RPM Display: CHECK IF 2ND ROW VERY IMPORTANT
    sprintf(second_row, "R:%d,%d", velocity->rpm1, velocity->rpm2);
    spi_send_string(second_row, 0xC0);
    return NULL;
}

void* displayCaseB() {
    char first_row[16];
    char second_row[16];

    spi_clear_lcd();
    //Temperature Display
    sprintf(first_row, "SA:%d,%d", velocity->minRPM, velocity->maxRPM);
    spi_send_string(first_row, 0x80);
    //RPM Display: CHECK IF 2ND ROW VERY IMPORTANT
    sprintf(second_row, "R:%d,%d", velocity->dutyCycle1, velocity->dutyCycle2);
    spi_send_string(second_row, 0xC0);
    return NULL;
}