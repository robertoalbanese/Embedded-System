/* 
 * File:   uart.h
 * Author:  andre& ralba
 * Comments:
 * Revision history: 
 */

#include <xc.h> // include processor files - each processor file is guarded.  
#include "config.h"

void UART_config(); //Routine to configure UART
void UART_bufferInit(uart_buffer *buffer);
void UART_writeOnBuffer(uart_buffer *buffer, int val);
char UART_readOnBuffer(uart_buffer *buffer);
int UART_sendMsg(char* message);