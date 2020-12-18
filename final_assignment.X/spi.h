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
void spi_clear_row(char row); //Routine to clear a row in the LCD
void spi_send_string(char *s); //Routine to send a string to the LCD
void spi_uart_print_second_row(int count); //Routine to print in the second row "Char Recv: %d"
//spi_move_cursor(int row, int column);
