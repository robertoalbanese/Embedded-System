/* 
 * File:   uart.h
 * Author:  ralba
 * Comments:
 * Revision history: 
 */

#include <xc.h> // include processor files - each processor file is guarded.  

void UART2_config(); //Routine to counfigure UART2
void UART2_spi_new_char_recived(char *prev, int *count, const int position[]); //Routine to send the new recived character to the LCD