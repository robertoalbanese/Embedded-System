/* 
 * File:   spi.h
 * Author:  ralba
 * Comments:
 * Revision history: 
 */

#include <xc.h> // include processor files - each processor file is guarded.  

void spi_config();
void spi_send_char(char c);
void spi_clear_row(char row);
void spi_send_string(char *c);