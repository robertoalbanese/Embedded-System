/* 
 * File:   uart.h
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#ifndef XC_HEADER_UART_H
#define XC_HEADER_UART_H

#include <xc.h> // include processor files - each processor file is guarded.

#define RXDIM 100 //Max dimension of the circula buffer

// Stores circular buffer data and read/write position
typedef struct {
    char buffer[RXDIM];
    int headIndex;
    int tailIndex;
} uart_buffer;

void UART_config(); //Routine to configure UART
int UART_buffDim(uart_buffer *buffer); // This function checks the dimension of the circular buffer of unread data
void UART_writeOnBuffer(uart_buffer *buffer, char val); //This task writes the new received character from the UART in the circular buffer
char UART_readOnBuffer(uart_buffer *buffer); //This task reads the new received character from the UART in the circular buffer
int UART_sendMsg(char* message); //This task sends an ASCII message to the PC

#endif