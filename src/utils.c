#include "utils.h"

uint16_t LOWER_CRITICAL hexval(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

uint16_t LOWER_CRITICAL ishexdigit(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

// Convert a hex string to an integer and update the pointer to the first character after the number
uint16_t LOWER_CRITICAL *axtoaddr(char **num_p)
{
    unsigned long value = 0;
    char *cur_c = *num_p;
    while (ishexdigit(*cur_c))
        value = (value * 16) + hexval(*cur_c++);

    *num_p = cur_c;
    return (uint16_t *)(uintptr_t)value;
}

// Converts 2 hex characters to an integer of 8 bits, updating the pointer to the next character
// Assumes there are at least 2 chars to convert in *num_p
unsigned char LOWER_CRITICAL axtoi8(char **num_p)
{
    char *cur_c = *num_p;
    unsigned char val = hexval(*cur_c++) * 16;
    val += hexval(*cur_c++);
    *num_p = cur_c;
    return val;
}

// Converts 4 hex characters to an integer of 16 bits, updating the pointer to the next character
// Assumes there are at least 4 chars to convert in *num_p
uint16_t LOWER_CRITICAL axtoi16(char **num_p)
{
    char *cur_c = *num_p;
    uint16_t val = 0;
    uint16_t i;
    for (i = 0; i < 4; i++)
    {
        val = (val * 16) + hexval(*cur_c++);
    }
    *num_p = cur_c;
    return val;
}

void init_arrays(char *diff, uint16_t *sim_data)
{
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    const char *diff_s = "S0004392-000443a:0009w;W0004392:0009wB0F20040025C240550B203E8280063822802";
#elif defined(__GNUC__)
    const char *diff_s = "S44F8-450C:000Bw;W44F6:000Cw2011B0F20040025C2407184050B203E81C04184063821C06";
#else
#error Compiler not supported!
#endif

    memcpy(diff, diff_s, strlen(diff_s) + 1);
}
