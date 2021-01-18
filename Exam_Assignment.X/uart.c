/*
 * File:   uart.c
 * Author: andre
 *
 * Created on 11 dicembre 2020, 15.48
 */

#include "xc.h"
#include "config.h"

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
    writeOnBuffer(&reciverBuffer, val); // Save value in buffer
}

void UART_bufferInit(UARTBuffer *buffer) {
    buffer->buffer = 0;
    buffer->headIndex = 0; //wrinting index
    buffer->tailIndex = 0; //reading index
    buffer->unreadData = 0;
}

int UART_buffDim(UARTBuffer *buffer) {
    if (buffer->headIndex >= buffer->tailIndex)
        return (buffer->headIndex - buffer->tailIndex);
    else
        return (TXDIM - (buffer->tailIndex - buffer->headIndex));
}

void UART_writeOnBuffer(UARTBuffer *buffer, int val) {
    buffer->buffer[buffer->headIndex] = val;
    buffer->headIndex++;
    if (buffer->headIndex == TXDIM)
        buffer->headIndex = 0;
}

char UART_readOnBuffer(UARTBuffer *buffer) {
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
    strcpy(msg, message);
    for (int i = 0; i < strlen(msg); i++) {
        while (U1STAbits.UTXBF == 1);
        U1TXREG = msg[i];
    }
}
