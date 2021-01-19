/* 
 * File:   spi.h
 * Author:  andre & ralba
 * Comments:
 * Revision history: 
 */
#define _SPI_H_
#define FIRST_ROW 0
#define SECOND_ROW 1
#include <xc.h> // include processor files - each processor file is guarded.  

void spi_config(); //SPI configuration routine
void spi_send_char(char c); //Routine to send a character to the LCD
void spi_clear_lcd(); //Routine to clear a row in the LCD
void spi_send_string(char *s, char row); //Routine to send a string to the LCD
//spi_move_cursor(int row, int column);
void* displayCaseA();
void* displayCaseB();
