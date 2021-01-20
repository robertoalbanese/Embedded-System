/*
 * File:   uart.c
 * Author: andre
 *
 * Created on 11 dicembre 2020, 15.48
 */

#include "xc.h"
#include "uart.h"
#include "config.h"
#include <string.h>

void UART_config() {
    U2BRG = 11; // ((1843200) / (16 * 9600)) - 1
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;
    IEC1bits.U2RXIE = 1; // Enable Rx interrupt for UART
    U2STAbits.URXISEL = 0b10; //0b10 for longer words
}

void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt() {
    IFS1bits.U2RXIF = 0; // Reset rx interrupt flag
    int val = U2RXREG; // Read from rx register
    UART_writeOnBuffer(&buffer, val); // Save value in buffer
}

void UART_bufferInit(uart_buffer *buffer) {
    buffer->headIndex = 0; //wrinting index
    buffer->tailIndex = 0; //reading index
    buffer->unreadData = 0;
}

int UART_buffDim(uart_buffer *buffer) {
    if (buffer->headIndex >= buffer->tailIndex)
        return (buffer->headIndex - buffer->tailIndex);
    else
        return (TXDIM - (buffer->tailIndex - buffer->headIndex));
}

void UART_writeOnBuffer(uart_buffer *buffer, int val) {
    buffer->buffer[buffer->headIndex] = val;
    buffer->headIndex++;
    if (buffer->headIndex == TXDIM)
        buffer->headIndex = 0;
}

char UART_readOnBuffer(uart_buffer *buffer) {
    char readChar;
    IEC1bits.U2RXIE = 0; // Disable interrupt of UART
    if (buffer->tailIndex == buffer->headIndex) { // We've finished reading
        IEC1bits.U2RXIE = 1; // Enable interrupt of UART
        return 0;
    }
    readChar = (char) buffer->buffer[buffer->tailIndex];
    buffer->tailIndex++;
    if (buffer->tailIndex == TXDIM)
        buffer->tailIndex = 0;

    IEC1bits.U2RXIE = 1; // Enable interrupt of UART

    return readChar;
}

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
