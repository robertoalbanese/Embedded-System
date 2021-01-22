/*
 * File:   uart.c
 * Author: ralba & andre
 *
 * Created on January 12, 2020, 6:46 PM
 */

#include "xc.h"
#include "uart.h"
#include "config.h"
#include <string.h>

// ISR for a new received character in the UART

void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt() {
    IFS1bits.U2RXIF = 0; // Reset rx interrupt flag
    char val = U2RXREG; // Read from rx register
    UART_writeOnBuffer(&buffer, val); // Save value in buffer
}

// UART buffer configuration

void UART_config() {
    U2BRG = 47; // ((1843200) / (16 * 2400)) - 1 (Since we can process 10 messages per sec)
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;
    IEC1bits.U2RXIE = 1; // Enable Rx interrupt for UART
    U2STAbits.URXISEL = 0b10; //0b10 for longer words
}

// This function checks the dimension of the circular buffer of unread data

int UART_buffDim(uart_buffer *buffer) {
    if (buffer->headIndex >= buffer->tailIndex)
        return (buffer->headIndex - buffer->tailIndex);
    else
        return (RXDIM - (buffer->tailIndex - buffer->headIndex));
}

//This task writes the new received character from the UART in the circular buffer

void UART_writeOnBuffer(uart_buffer *buffer, char val) {
    buffer->buffer[buffer->headIndex] = val;
    buffer->headIndex++;
    if (buffer->headIndex == RXDIM)
        buffer->headIndex = 0;
}

//This function reads the new received character from the UART in the circular buffer

char UART_readOnBuffer(uart_buffer *buffer) {
    char readChar;
    IEC1bits.U2RXIE = 0; // Disable interrupt of UART
    if (buffer->tailIndex == buffer->headIndex) { // We've finished reading
        IEC1bits.U2RXIE = 1; // Enable interrupt of UART
        return 0;
    }
    readChar = buffer->buffer[buffer->tailIndex];
    buffer->tailIndex++;
    if (buffer->tailIndex == RXDIM)
        buffer->tailIndex = 0;

    IEC1bits.U2RXIE = 1; // Enable interrupt of UART

    return readChar;
}

//This function sends an ASCII message to the PC

int UART_sendMsg(char* message) {
    char msg[50];
    int i;
    strcpy(msg, message);
    for (i = 0; i < strlen(msg); i++) {
        while (U1STAbits.UTXBF == 1);
        U1TXREG = msg[i];
    }
    return 0;
}
