#include "msp430.h"
#include "msp430fr5994.h"

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR |= BIT0;              // Set P1.0 to output direction
    P1OUT &= ~BIT0;             // Set the LED off

    while(1) {
        P1OUT ^= BIT0;           // Toggle the LED
        __delay_cycles(100000); // Delay for a while
    }
}
