
#include <stdint.h>
#include <string.h> 
#include <stdlib.h>
#include "utils.h"

typedef struct diff_entry {
    uint* addr;
    ushort words;
    uint* mc_values;
} diff_entry_t;

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
    char* diff_s = "01008c-01:1388";
    memcpy(diff, diff_s, strlen(diff_s));
}

diff_entry_t diff_entry_create(uint* addr, uint words) {
    diff_entry_t entry;
    entry.addr = addr;
    entry.words = words;
    entry.mc_values = malloc(words * sizeof(int));
    return entry;
}

// Decode the diff string into an array of diff_entry_t
// Returns the number of entries in the array
int decode_diff(char* diff, diff_entry_t* diff_arr[], uint diff_size) {
    // initial address and instruction
    char* mc_p = diff;
    uint cur_entry_i = 0;
    int max_entries = diff_size;
    do {
        uint* addr = axtoaddr(&mc_p);
        // We should be pointing to a '-' now
        if (*mc_p++ != '-') {
            return -1;
        }
        ushort no_words = axtoi8(&mc_p);

        uint val = 0;
        // Resize the array if necessary
        if (cur_entry_i >= max_entries) {
            max_entries *= 2;
            *diff_arr = realloc(diff_arr, max_entries);
        }
        diff_entry_t cur_entry = diff_entry_create(addr, no_words);
        
        // We should be pointing to a ':'
        if (*mc_p++ != ':') {
            return -1;
        }

        // Parse the microcode values
        int i;
        for (i = 0; i < no_words; i++) {
            // Store next microinstruction
            val = axtoi16(&mc_p);
            cur_entry.mc_values[i] = val;
        }
        (*diff_arr)[cur_entry_i++] = cur_entry;

        // If no new instruction, we are done
        if (*mc_p != ';') {
            max_entries = cur_entry_i;
            break;
        }
        
        // Skip the ';'
        mc_p++;

    } while (*mc_p != '\0');
    return max_entries;
}

int apply_update(diff_entry_t* diff_arr, int diff_size) {
    int i;
    for (i = 0; i < diff_size; i++) {
        diff_entry_t entry = diff_arr[i];
        int j;
        for (j = 0; j < entry.words; j++) {
            // __data_20_write_char()
            // __data_20_write_long()
            // __data_20_write_short(entry.addr[j], entry.mc_values[j]);
            entry.addr[j] = entry.mc_values[j];
        }
    }
    return 0;
}

void destroy_diff(diff_entry_t* diff_arr, int diff_size) {
    int i;
    for (i = 0; i < diff_size; i++) {
        free(diff_arr[i].mc_values);
    }
    free(diff_arr);
}


int update(char* diff) {
    int size = 4;
    diff_entry_t* diff_arr = malloc(size * sizeof(diff_entry_t));

    size = decode_diff(diff, &diff_arr, size);
    apply_update(diff_arr, size);
    destroy_diff(diff_arr, size);
    return 0;
}
