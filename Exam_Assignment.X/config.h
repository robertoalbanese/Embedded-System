/*
 * File:   main.c
 * Author: ralba & andre
 *
 * Created on January 18, 2021, 10:59 AM
 */

#include <xc.h> 

#define MAX_TASKS 5
#define TXDIM 100
#define FCYC 1843200
#define MIN_DC (-10000)
#define MAX_DC (+10000)
#define MAX_RPM (+8000)
#define MIN_RPM (-8000)

#define STATE_COMMAND 0
#define STATE_TIMEOUT 1
#define STATE_SAFE 2

