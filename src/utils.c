
#include <stdint.h>

int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

int ishexdigit(char c) {
    return 
        (c >= '0' && c <= '9') || 
        (c >= 'a' && c <= 'f') || 
        (c >= 'A' && c <= 'F');
}

// Convert a hex string to an integer and update the pointer to the first character after the number
int *axtoaddr(char **num_p) {
    unsigned int value = 0;
    char *cur_c = *num_p;
    while (ishexdigit(*cur_c))
        value = value * 16 + hexval(*cur_c++);

    if (*cur_c++ != ':') {
        return (void*)0;
    }
    *num_p = cur_c;
    return (int*)(unsigned long int)value;
}

// Converts two hex characters to an integer of 16 bits, updating the pointer to the next character
// Assumes there are at least two chars to convert in *num_p
int axtoi16(char **num_p) {
    char *cur_c = *num_p;
    int val = (hexval(*cur_c++) << 4);
    val += hexval(*cur_c++);
    *num_p = cur_c;
    return val;
}
