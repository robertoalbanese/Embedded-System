/*
 * File:   buttons.c
 * Author: ralba
 *
 * Created on October 31, 2020, 5:00 PM
 */


#include "xc.h"
#include "buttons.h"

int button_E8_pressed(int *prev) {
    int currButt = 1;
    int ret = 0;

    currButt = PORTEbits.RE8;
    if ((*prev^currButt) == 1) {
        if (currButt == 0) {
            ret = 1;
        }
    }
   *prev = currButt;
    return ret;
}

int button_D0_pressed(int *prev) {
    int currButt = 1;
    int ret = 0;

    currButt = PORTDbits.RD0;
    if ((*prev^currButt) == 1) {
        if (currButt == 0) {
            ret = 1;
        }
    }
    *prev = currButt;
    return ret;
}
