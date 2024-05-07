#pragma once

#include <string.h>
#include <stdint.h>

/*
'lower' places the function in the .lower section, making it possible to shift the program code
in higher memory regions if needed for an update.
'critical' disables interrupts while the function is running
*/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#define LOWER_CRITICAL __attribute__((section(".lower.text")))

#elif defined(__GNUC__)
#define LOWER_CRITICAL __attribute__((critical, lower))

#else
#error Compiler not supported!

#endif

// Function prototypes
uint16_t hexval(char c);
uint16_t ishexdigit(char c);
uint16_t *axtoaddr(char **num_p);
unsigned char axtoi8(char **num_p);
uint16_t axtoi16(char **num_p);
void init_diff(char *diff);
