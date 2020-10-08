
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

#define TIMER1 1
#define TIMER2 2
void tmr_setup_period(int timer, int ms);
void tmr_wait_period(int timer);

int main() {
    // initialization code
    TRISBbits.TRISB0 = 0;
    
    tmr_setup_period(TIMER1, 1);
    while (1) {
        // code to blink LED
        tmr_wait_period(TIMER1);
        LATBbits.LATB0 = !LATBbits.LATB0;
    }
}

void tmr_setup_period(int timer, int ms) {
    if (timer == TIMER1) {
        TMR1 = 0; //reset the timer counter
        // Fcy = 1843200 (number of clocks in one second)
        // in 1 second there would be 921600 clocks steps
        // this is too high to be put in a 16 bit register (max 65535)
        // If we set a prescaler of 1:64 we have 1843200/16 = 28800 clock steps, OK!
        float rate_in_sec = (float) ms / 1000000;
        PR1 = 28800 * rate_in_sec;

        T1CONbits.TCKPS = 2; //prescaler 1:64
        T1CONbits.TCS = 0; //use internal clock
        T1CONbits.TON = 1; //starts the timer
    } else if (timer == TIMER2) {
        TMR2 = 0; //reset the timer counter
        // Fcy = 1843200 (number of clocks in one second)
        // in 1 second there would be 921600 clocks steps
        // this is too high to be put in a 16 bit register (max 65535)
        // If we set a prescaler of 1:64 we have 1843200/16 = 28800 clock steps, OK!
        float rate_in_sec = (float) ms / 1000;
        PR2 = 28800; // * rate_in_sec;

        T2CONbits.TCKPS = 2; //prescaler 1:64
        T2CONbits.TCS = 0; //use internal clock
        T2CONbits.TON = 1; //starts the timer
    }
}

void tmr_wait_period(int timer) {
    switch (timer) {
        case TIMER1:
        {
            while (IFS0bits.T1IF == 0) {
            }
            IFS0bits.T1IF = 0;
            TMR1 = 0;
            break;
        }
        case TIMER2:
        {
             while (IFS0bits.T2IF == 0) {
            }
            IFS0bits.T2IF = 0;
            TMR2 = 0;
            break;
        }
    }
}