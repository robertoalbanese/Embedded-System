/*
 * File:   main.c
 * Author: Roberto Albanese - Andrea Tiranti
 *
 * Created on October 30, 2020, 4:32 PM
 */

#include "timer.h"
#include "spi.h"
#include "xc.h"

//Routine to counfigure UART2
//  Input: 
//      int counter : current number of character received

void UART2_config() {
    U2BRG = 11; // ((1843200) / (16 * 9600)) - 1
    U2MODEbits.UARTEN = 1; // enable UART
    U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
    U2STAbits.URXISEL = 1;
}

//Routine to send the new recived character to the LCD
//  Input: 
//      char *prev : previous character recived to check if we have recived '\n' or '\r'
//      int *count :  current number of character received
//      const int  position[] : positions of the first row to move the index of the LCD in

void UART2_spi_new_char_recived(char *prev, int *count, const int position[]) {
    //While there is something in the receive buffer
    while (U2STAbits.URXDA == 1) {
        //Check if a '\n' or '\n' chat has been received
        if ((U2RXREG == 'r' || U2RXREG == 'n') && *prev == '\\')
            //Clear first row
            spi_clear_row(0x80);
        else {
            //Move the index of the LCD in the correct position
            spi_send_char(position[*count % 16]);
            //Send the character
            spi_send_char(U2RXREG);
            //Store the current char for the next cycle
            *prev = U2RXREG;
        }
        *count += 1; //We increment the counter also if we receive '\r' or '\n'
        
        //If we have reached the end of the fist row we clear it
        if (*count == 16)
            spi_clear_row(0x80);
        //Display the updated second row in the LCD
        spi_uart_print_second_row(*count);
    }
}
