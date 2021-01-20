/* 
 * File:   spi.h
 * Author:  andre & ralba
 * Comments:
 * Revision history: 
 */
#ifndef XC_HEADER_TEMPLATE_H
#define XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "config.h"

#endif

void spi_config(); //SPI configuration routine
void spi_send_char(char c); //Routine to send a character to the LCD
void spi_clear_lcd(); //Routine to clear a row in the LCD
void spi_send_string(char *s, char row); //Routine to send a string to the LCD
void* displayCaseA(display_info* info);
void* displayCaseB(display_info* info);
