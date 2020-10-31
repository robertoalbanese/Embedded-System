/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 31, 2020, 12:32 PM
 */

#include <stdio.h>
#include "timer.h"
#include "spi.h"
#include "uart.h"
#include "xc.h"

int main(void) {
    tmr_wait_ms(TIMER1, 1000);
    spi_config();
    U2ART_config();
    TRISDbits.TRISD0 = 1;
    TRISEbits.TRISE8 = 1;

    int prev_butt = 1;
    const int first_row_pos[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F};
    char second_row[16];
    int char_count = 0;

    while (1) {

        if (button_E8_pressed(&prev_butt) == 1)
            U2TXREG = char_count;

        if (button_D0_pressed(&prev_butt) == 1) {
            spi_clear_row(0x80);
            spi_clear_row(0xC0);
            char_count = 0;
            sprintf(second_row, "Char Recv: %d", char_count);
            spi_send_string(second_row);
        }

        if (U2STAbits.URXDA == 1) {
            char_count++;
            if (U2RXREG == '\r' || U2RXREG == '\n')
                spi_clear_row(0x80);
            else {
                spi_send_char(first_row_pos[char_count]);
                spi_send_char(U2RXREG);
            }
            if (char_count == 15)
                spi_clear_row(0x80);

            sprintf(second_row, "Char Recv: %d", char_count);
            spi_send_string(second_row);
        }
    }

    return 0;
}
