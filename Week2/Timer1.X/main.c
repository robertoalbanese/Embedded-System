
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

void choose_prescaler(int ms, int* ps, int* tckps);
void tmr_setup_period(int timer, int ms);
void tmr_wait_period(int timer);

int main() {
    // initialization code
    TRISBbits.TRISB0 = 0;

    tmr_setup_period(TIMER1, 500);
    while (1) {
        // code to blink LED
        tmr_wait_period(TIMER1);
        LATBbits.LATB0 = !LATBbits.LATB0;
    }
}

void choose_prescaler(int ms, int* pr, int* tckps) {
    long ticks = 1843.2 * ms;
    if (ticks <= 65535) {
        //prescaler 1:1
        *pr = ticks;
        *tckps = 0;
        return;
    }
    ticks = ticks / 8;
    if (ticks <= 65535) {
        //prescaler 1:8
        *pr = ticks;
        *tckps = 1;
        return;
    }
    ticks = ticks / 8;
    if (ticks <= 65535) {
        //prescaler 1:64
        *pr = ticks;
        *tckps = 2;
        return;
    }
    ticks = ticks / 4;
    if (ticks <= 65535) {
        //prescaler 1:256 
        *pr = ticks;
        *tckps = 3;
        return;
    }
}

void tmr_setup_period(int timer, int ms) {
    int pr;
    int tckps;
    if (timer == TIMER1) {
        T1CONbits.TON = 0;
        TMR1 = 0; //reset the timer counter
        choose_prescaler(ms, &pr, &tckps);
        PR1 = pr;
        T1CONbits.TCKPS = tckps; //prescaler 1:64
        T1CONbits.TON = 1; //starts the timer
    } else if (timer == TIMER2) {
        T2CONbits.TON = 0;
        TMR2 = 0; //reset the timer counter
        choose_prescaler(ms, &pr, &tckps);
        PR2 = pr;
        T2CONbits.TCKPS = tckps; //prescaler 1:64
        T2CONbits.TON = 1; //starts the timer
    }
}

void tmr_wait_period(int timer) {
    switch (timer) {
        case TIMER1:
        {
            while (IFS0bits.T1IF == 0);
            IFS0bits.T1IF = 0;
            break;
        }
        case TIMER2:
        {
            while (IFS0bits.T2IF == 0);
            IFS0bits.T2IF = 0;
            break;
        }
    }
}