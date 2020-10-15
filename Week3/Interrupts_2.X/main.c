/*
 * File:   main.c
 * Author: ralba
 *
 * Created on October 15, 2020, 3:36 PM
 */

// DSPIC30F4011 Configuration Bit Settings

// 'C' source line config statements

// FOSC
#pragma config FPR = XT                 // Primary Oscillator Mode (XT)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "timer.h"

void __attribute__((__interrupt__, __auto_psv__)) _INT1Interrupt
() {
    IFS1bits.INT1IF = 0; // reset interrupt flag
    LATBbits.LATB1 = !LATBbits.LATB1;
}

int main(void) {
    IEC1bits.INT1IE = 1; // enable INT0 interrupt!
    
    TRISBbits.TRISB0 = 0;   //Led D3 output
    TRISBbits.TRISB1 = 0;   //Led D4 output
    TRISEbits.TRISE8 = 1;   //Button S5 input

    tmr_setup_period(TIMER1, 500);  //Setup TIMER 1 period

    int prevButt = 1;
    int currButt = 1;

    while (1) {

        currButt = PORTEbits.RE8;
        if ((prevButt^currButt) == 1) {
            if (currButt == 0) {
                IFS1bits.INT1IF = 1; // set interrupt flag high
            }
        }
        prevButt = currButt;

        if (IFS0bits.T1IF == 1) {
            LATBbits.LATB0 = !LATBbits.LATB0;
            IFS0bits.T1IF = 0;
        }
    }
    return 0;
}
