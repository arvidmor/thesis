
#include "utils.h"

uint16_t LOWER_CRITICAL hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

uint16_t LOWER_CRITICAL ishexdigit(char c) {
    return 
        (c >= '0' && c <= '9') || 
        (c >= 'a' && c <= 'f') || 
        (c >= 'A' && c <= 'F');
}

// Convert a hex string to an integer and update the pointer to the first character after the number
uint16_t LOWER_CRITICAL *axtoaddr(char **num_p) {
    ulong value = 0;
    char *cur_c = *num_p;
    while (ishexdigit(*cur_c))
        value = value * 16 + hexval(*cur_c++);

    *num_p = cur_c;
    return (uint16_t*)(uintptr_t)value;
}

uchar LOWER_CRITICAL axtoi8(char **num_p) {
    char *cur_c = *num_p;
    unsigned char val = (hexval(*cur_c++) << 4);
    val += hexval(*cur_c++);
    *num_p = cur_c;
    return val;
}

// Converts 4 hex characters to an integer of 16 bits, updating the pointer to the next character
// Assumes there are at least 4 chars to convert in *num_p
uint16_t LOWER_CRITICAL axtoi16(char **num_p) {
    char *cur_c = *num_p;
    uint val = 0;
    uint i;
    for (i = 4; i > 0; i--) {
        val = (val << 4) + hexval(*cur_c++);
    }
    *num_p = cur_c;
    return val;
}

void init_diff(char* diff) {
    #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    char* diff_s = "W010550:01w1388";
    #elif defined(__GNUC__)
    char* diff_s = "W01008c:01w1388";
    #else
    #error Compiler not supported!
    #endif
    memcpy(diff, diff_s, strlen(diff_s));
}
