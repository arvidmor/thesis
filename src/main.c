
#include <msp430.h>
#include "utils.h"
#include "update.h"

__attribute__((noinit)) static char diff[256] = {0};

__attribute((section(".data"))) volatile static long init_green = 20000;
__attribute((section(".data"))) static long init_red = 20000;

static inline void gpio_init(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure GPIO
    // LED (P1.0)
    P1OUT = BIT0;           // Set P1.0 to high
    P1DIR |= (BIT0 | BIT1); // Set P1.0-1 to output direction

    // Button P5.5 and P5.6
    P5REN |= (BIT5 | BIT6); // Enable pull-up resistor
    P5OUT |= (BIT5 | BIT6); // Set P5.5 to high

    // Enable interrupts on P5.5 & P5.6
    P5IE |= (BIT5 | BIT6);  // Enable interrupt
    P5IES |= (BIT5 | BIT6); // high-to-low transition
    P5IFG &= 0;             // Clear flags
}

int main(void)
{
    gpio_init();
    _BIS_SR(GIE);

    init_diff(diff);

    long i = init_red;
    long j = init_green;
    while (1)
    {
        if (i == 0)
        {
            P1OUT ^= BIT0; // Toggle LED
            i = init_red;
        }
        if (j == 0)
        {
            P1OUT ^= BIT1; // Toggle LED2
            j = init_green;
        }
        i--;
        j--;
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = PORT5_VECTOR
__interrupt void Port_5(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(PORT5_VECTOR))) Port_5(void)
#else
#error Compiler not supported!
#endif
{
    if (P5IFG & BIT5)
    { // Button 1 pressed
        update(diff);
    }

    P5IFG &= 0; // Clear interrupt flag
}
