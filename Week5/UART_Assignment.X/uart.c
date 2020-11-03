/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 30, 2020, 4:32 PM
 */

#include "timer.h"
#include "spi.h"
#include "xc.h"

void U2ART_config() {
    U2BRG = 11; // ((1843200) / (16 * 9600)) - 1
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;
}

void U2ART_spi_new_char_recived (int *prev,int *count, int  position[])
{
    while (U2STAbits.URXDA == 1) {
            if ((U2RXREG == 'r' || U2RXREG == 'n') && *prev == '\\')
                spi_clear_row(0x80);
            else {
                spi_send_char(position[*count % 16]);
                spi_send_char(U2RXREG);
                *prev = U2RXREG;
            }
            *count += 1 ; //We increment the counter also if we receive '\r' or '\n'

            if (*count == 16)
                spi_clear_row(0x80);

            spi_uart_print_second_row(*count);
        }
}
