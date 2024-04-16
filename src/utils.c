
#include <stdint.h>
#include <string.h> 
#include <stdlib.h>
#include "utils.h"

typedef struct diff_entry {
    uint*  addr;
    uint*  data;
    ushort no_words;
} diff_op_t;

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
uint *axtoaddr(char **num_p) {
    ulong value = 0;
    char *cur_c = *num_p;
    while (ishexdigit(*cur_c))
        value = value * 16 + hexval(*cur_c++);

    *num_p = cur_c;
    return (uint*)(uintptr_t)value;
}

int axtoi8(char **num_p) {
    char *cur_c = *num_p;
    int val = (hexval(*cur_c++) << 4);
    val += hexval(*cur_c++);
    *num_p = cur_c;
    return val;
}

// Converts two hex characters to an integer of 16 bits, updating the pointer to the next character
// Assumes there are at least 4 chars to convert in *num_p
int axtoi16(char **num_p) {
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
    char* diff_s = "010550-01:1388";
    #elif defined(__GNUC__)
    char* diff_s = "01008c-01:1388";
    #else
    #error Compiler not supported!
    #endif
    memcpy(diff, diff_s, strlen(diff_s));
}

diff_op_t diff_op_create(uint* addr, uint words) {
    diff_op_t operation;
    operation.addr = addr;
    operation.no_words = words;
    operation.data = malloc(words * sizeof(int));
    return operation;
}

// Decode the diff string into an array of diff_op_t
// Returns the number of entries in the array
int decode(char* diff, diff_op_t* diff_arr[], uint diff_size) {
    // initial address and instruction
    char* mc_p = diff;
    uint cur_op_i = 0;
    int max_ops = diff_size;
    do {
        uint* addr = axtoaddr(&mc_p);
        // We should be pointing to a '-' now
        if (*mc_p++ != '-') {
            return -1;
        }
        ushort no_words = axtoi8(&mc_p);

        uint val = 0;
        // Resize the array if necessary
        if (cur_op_i >= max_ops) {
            max_ops *= 2;
            *diff_arr = realloc(diff_arr, max_ops);
        }
        diff_op_t cur_op = diff_op_create(addr, no_words);
        
        // We should be pointing to a ':'
        if (*mc_p++ != ':') {
            return -1;
        }

        // Parse the microcode values
        int i;
        for (i = 0; i < no_words; i++) {
            // Store next microinstruction
            val = axtoi16(&mc_p);
            cur_op.data[i] = val;
        }
        (*diff_arr)[cur_op_i++] = cur_op;

        // If no new instruction, we are done
        if (*mc_p != ';') {
            max_ops = cur_op_i;
            break;
        }
        
        // Skip the ';'
        mc_p++;

    } while (*mc_p != '\0');
    return max_ops;
}

void apply(diff_op_t* diff_arr, int diff_size) {
    int i = diff_size - 1;
    for (; i > 0; i--) {
        diff_op_t operation = diff_arr[i];
        memcpy(operation.addr, operation.data, operation.no_words * sizeof(uint));
    }
}

void cleanup(diff_op_t* diff_arr, int diff_size) {
    int i;
    for (i = 0; i < diff_size; i++) {
        free(diff_arr[i].data);
    }
    free(diff_arr);
}


int update(char* diff) {
    int size = 4;
    diff_op_t* diff_arr = malloc(size * sizeof(diff_op_t));

    size = decode(diff, &diff_arr, size);
    apply(diff_arr, size);
    cleanup(diff_arr, size);
    return 0;
}
