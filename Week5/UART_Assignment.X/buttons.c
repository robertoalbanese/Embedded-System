/*
 * File:   buttons.c
 * Author: ralba
 *
 * Created on October 31, 2020, 5:00 PM
 */


#include "xc.h"
#include "buttons.h"

//Routine to check if the button S5 has been pressed
//  Input: 
//      int *prev: previous state of the button
//  Output:
//      int ret:    1 if button has been pressed, 0 otherwise

int button_E8_pressed(int *prev) {
    int currButt = 1;
    int ret = 0;
    //Get current state of the button
    currButt = PORTEbits.RE8;
    //If different from previous state and current button is 0(button pressed)
    if (*prev != currButt) {
        if (currButt == 0) {
            ret = 1;
        }
    }
    //Store current state in 'prev' for the next check
    *prev = currButt;    
    return ret;
}

//Routine to check if the button S5 has been pressed
//  Input: 
//      int *prev:  previous state of the button
//  Output:
//      int ret:    1 if button has been pressed, 0 otherwise

int button_D0_pressed(int *prev) {
    int currButt = 1;
    int ret = 0;
    //Get current state of the button
    currButt = PORTDbits.RD0;
    //If different from previous state and current button is 0(button pressed)
    if (*prev != currButt) {
        if (currButt == 0) {
            ret = 1;
        }
    }
    //Store current state in 'prev' for the next check
    *prev = currButt;    
    return ret;
}
