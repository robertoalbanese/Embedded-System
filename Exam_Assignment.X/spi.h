/* 
 * File:   spi.h
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */
/*
 * File:   spi.h
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#ifndef XC_HEADER_SPI_H
#define XC_HEADER_SPI_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "pwm.h"

// Stores temperature data

typedef struct {
    double temp_records[10]; //FIFO buffer
    double temperature;
    double average;
} temperature_info;

// Store info data to display
typedef struct {
    temperature_info* temp_info; //FIFO buffer
    rpm_data* rpm_info;
} display_info;

// Stores data to display and a pointer to function to select the right info to display
typedef struct {
    void *(*format[2])(display_info*);
    display_info* display_info;
    int index;
} display_lcd;

void spi_config(); //SPI configuration routine
void spi_send_char(char c); //Routine to send a character to the LCD
void spi_clear_lcd(); //Routine to clear a row in the LCD
void spi_send_string(char *s, char row); //Routine to send a string to the LCD
void *displayCaseA(display_info* info); // First display case: here we show the program state, the average temperature and the current rpm values
void *displayCaseB(display_info* info); //Second display case: here we show the new rpm boundaries and the duty cycles 

#endif
