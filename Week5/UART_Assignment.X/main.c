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
#include "buttons.h"

int main(void) {

    tmr_wait_ms(TIMER1, 1000); //Wait 1 second to let the LCD setup
    spi_config(); //Configuration of the SPI
    UART2_config(); //Configuration of UART2

    TRISEbits.TRISE8 = 1; //Set button S5 as input
    TRISDbits.TRISD0 = 1; //Set button S6 as input

    char prev_char; //Stores the previous character recived to check the reception of '\n' or '\r'
    int prev_butt_E8 = 1; //Stores the previouse state of button S5
    int prev_butt_D0 = 1; //Stores the previouse state of button S6
    const int first_row_pos[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F}; //Used to move the index of the LCD
    char second_row[16]; //Stores the string printed in the second row
    int char_count = 0; //Count the amount of received character

    //Behavior of the program
    while (1) {
        //Check if an overflow in the receive buffer is occurred
        if (U2STAbits.OERR == 1) {

            //Send all the data inside the receive buffer to the LCD
            UART2_spi_new_char_recived(&prev_char, &char_count, first_row_pos);

            //Reset overflow flag
            U2STAbits.OERR = 0;
        }

        //Check if button S5 has been pressed
        if (button_E8_pressed(&prev_butt_E8) == 1) {

            //Wait if the transmit buffer is full
            while (U2STAbits.UTXBF == 1);

            //Send the current counter to UART2
            U2TXREG = char_count;
        }

        //Check if button S6 has been pressed
        if (button_D0_pressed(&prev_butt_D0) == 1) {

            //Clear first row of the LCD
            spi_clear_row(0x80);
            //Clear second row of the LCD
            spi_clear_row(0xC0);
            //Reset the counter
            char_count = 0;
            //Display in the LCD the second row updated
            sprintf(second_row, "Char Recv: %d", char_count);
            spi_send_string(second_row);
        }

        //Send all the data inside the receive buffer to the LCD
        UART2_spi_new_char_recived(&prev_char, &char_count, first_row_pos);
    }
    return 0;
}
