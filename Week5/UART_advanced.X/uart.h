/* 
 * File:   uart.h
 * Author:  ralba
 * Comments:
 * Revision history: 
 */

#include <xc.h> // include processor files - each processor file is guarded.  

void U2ART_config();
void spi_send_char(char c);