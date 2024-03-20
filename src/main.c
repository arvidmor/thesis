#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;  // Set P1.0 to output direction
    P1OUT |= BIT0;  // Set P1.0 to high

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    TA0CCTL0 = CCIE;                        // TACCR0 interrupt enabled
    TA0CCR0 = 30000;
    TA0CTL = TASSEL__SMCLK | MC__UP;        // SMCLK, UP mode

    __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0 w/ interrupt
    __no_operation();                       // For debugger
}

// Timer0_A0 interrupt service routine: toggles the P1.0 LED
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer0_A0_ISR (void)
{
    P1OUT ^= BIT0;
}