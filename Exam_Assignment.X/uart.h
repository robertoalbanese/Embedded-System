/* 
 * File:   uart.h
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#ifndef XC_HEADER_UART_H
#define XC_HEADER_UART_H

#include <xc.h> // include processor files - each processor file is guarded.

#define RXDIM 100

typedef struct {
    int buffer[RXDIM];
    int headIndex;
    int tailIndex;
    int unreadData;
} uart_buffer;

void UART_config(); //Routine to configure UART
void UART_bufferInit(uart_buffer *buffer);
int UART_buffDim(uart_buffer *buffer);
void UART_writeOnBuffer(uart_buffer *buffer, int val);
char UART_readOnBuffer(uart_buffer *buffer);
int UART_sendMsg(char* message);

#endif