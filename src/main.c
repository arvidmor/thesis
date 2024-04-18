
#include <msp430.h>
#include "utils.h"
#include "update.h"

static char diff[256] = {0};

static inline void gpio_init(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure GPIO
    // LED (P1.0)
    P1OUT &= 0;  // Set P1.0 to high 
    P1DIR |= BIT0;  // Set P1.0 to output direction

    // Button P5.5
    P5REN |= BIT5;  // Enable pull-up resistor on P5.6
    P5OUT |= BIT5;   // Set P5.5 to high

    // Enable interrupts on P5.5
    P5IE |= BIT5;   // Enable interrupt
    P5IES |= BIT5;  // high-to-low transition
    P5IFG &= 0;     // Clear flags
}

int main(void)
{
    gpio_init();
    _BIS_SR(GIE);

    init_diff(diff);

    long i = 20000;

    while(1) {
        if (i == 0) {
            P1OUT ^= BIT0;                 // Toggle P1.0 using exclusive-OR
            i = 20000;
        }
        i--;
    }

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT5_VECTOR))) Port_5 (void)
#else
#error Compiler not supported!
#endif
{
    if (P5IFG & BIT5) {
        // TODO: 
        update(diff);
        P5IFG &= ~BIT5; // Clear interrupt flag
    }
    P5IFG |= 0;
}

