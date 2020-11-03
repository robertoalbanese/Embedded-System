/* 
 * File:   uart.h
 * Author:  ralba
 * Comments:
 * Revision history: 
 */

#include <xc.h> // include processor files - each processor file is guarded.  

void U2ART_config();
void U2ART_spi_new_char_recived (int prev,int count, int  position);