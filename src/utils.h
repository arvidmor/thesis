#pragma once

#include <string.h> 
#include <stdint.h>

/*
'lower' places the function in the .lower section, making it possible to shift the program code
in higher memory regions if needed for an update.
'critical' disables interrupts while the function is running
*/
#define LOWER_CRITICAL __attribute__((critical, lower))

// Type aliases
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;


// Function prototypes
uint16_t  hexval(char c);
uint16_t  ishexdigit(char c);
uint16_t* axtoaddr(char **num_p);
uchar axtoi8(char **num_p);
uint16_t axtoi16(char **num_p);
void init_diff(char* diff);
