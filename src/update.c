#include "utils.h"
#include "update.h"

typedef struct diff_entry {
    char  opcode;
    uint16_t* addr1;
    uint16_t* addr2;
    uint16_t  no_words;
} diff_entry_t;


// Static array to store diff entries and data
#define MAX_ENTRIES 64
#define MAX_DATA 1024
static diff_entry_t diff_arr[MAX_ENTRIES];
static uint16_t diff_data[MAX_DATA];

#define ASSERT_CHAR(c_ptr, expected) if (*(*c_ptr)++ != expected) return ERR_DIFF_SYNTAX;
#define ASSERT_OK(result) if ((result) != OK) return result;
/*
This places the function in the .lower section, making it possible to shift the program code
in higher memory regions if needed for an update.
*/
#define LOWER_ATOMIC __attribute__((critical, lower))

dsu_err_t LOWER_ATOMIC decode_w(diff_entry_t* diff_entry, uint16_t** data_p, char** diff_p) {
    diff_entry->opcode = 'W';
    diff_entry->addr1 = axtoaddr(diff_p);

    // We should be pointing to a ':'
    ASSERT_CHAR(diff_p, ':');

    // Next is the number of words argument
    // This is a write op, so addr2 is an array containing the data to write
    uint16_t no_words = axtoi16(diff_p);
    diff_entry->no_words = no_words;
    diff_entry->addr2 = *data_p;

    // We should be pointing to a 'w'
    ASSERT_CHAR(diff_p, 'w');

    // Next is data
    uint16_t* max = diff_entry->addr2 + no_words;
    for (; *data_p < max ;) {
        *(*data_p)++ = axtoi16(diff_p);
    }
    return OK;
}

dsu_err_t LOWER_ATOMIC decode_s(diff_entry_t* diff_entry, char** diff_p) {
    diff_entry->opcode = 'S';
    diff_entry->addr1 = axtoaddr(diff_p);

    // We should be pointing to a '-', followed by the end address of this shift
    ASSERT_CHAR(diff_p, '-');
    diff_entry->addr2 = axtoaddr(diff_p);

    // We should be pointing to a ':', followed by the number of words to shift
    ASSERT_CHAR(diff_p, ':');
    diff_entry->no_words = axtoi16(diff_p);

    // no_words argument is followed by a 'w'
    ASSERT_CHAR(diff_p, 'w');

    return OK;
}

// Decode the diff string into an array of diff_op_t
// Returns the number of entries in the array
dsu_err_t LOWER_ATOMIC decode(char* diff) {
    char* diff_p = diff;
    uint16_t cur_op_i = 0;
    uint16_t* data_p = diff_data;
    
    do {
        // Makw sure we don't overflow the array
        if (cur_op_i >= MAX_ENTRIES) {
            return ERR_DIFF_OOM;
        }

        char opcode = *diff_p++;
        switch (opcode) {
            case 'W':
                ASSERT_OK(decode_w(&diff_arr[cur_op_i], &data_p, &diff_p) != OK)
                break;

            case 'S':
                ASSERT_OK(decode_s(&diff_arr[cur_op_i], &diff_p) != OK)
                break;

            default:
                return ERR_DIFF_SYNTAX;
                break;
        }
        cur_op_i++;
        // If there are more operations, there should be a ';'
        if (*diff_p++ != ';') 
            break;

    } while (*diff_p != '\0');
    return OK;
}


void LOWER_ATOMIC apply(diff_entry_t* diff_arr) {
    int i = 0;
    char opcode;
    uint16_t* src;
    uint16_t* dst;
    uint16_t size;
    do {
        diff_entry_t entry = diff_arr[i++];
        opcode = entry.opcode;
        switch (opcode) {
            // TODO: Look into optimizing with DMA
            case 'W':
                src = entry.addr2;
                dst = entry.addr1;
                for (int j = 0; j < entry.no_words; j++) 
                    dst[j] = src[j];
                break;

            case 'S':
                src = entry.addr1;
                dst = entry.addr1 + entry.no_words;
                size = entry.addr2 - entry.addr1;
                // Loop in reverse to not overwrite upcoming iteration data
                for (int j = size; j >= 0; j--) 
                    dst[j] = src[j];
                
                break;

            default:
                break;
        }
    } while (opcode != 0);
}

dsu_err_t LOWER_ATOMIC update(char* diff) {
    dsu_err_t result = decode(diff);
    ASSERT_OK(result);
    apply(diff_arr);
    // Clear arrays for future updates
    memset(diff_arr, 0, MAX_ENTRIES * sizeof(diff_entry_t));
    memset(diff_data, 0, MAX_DATA * sizeof(uint16_t));

    return OK;
}
