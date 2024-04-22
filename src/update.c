#include "utils.h"
#include "update.h"


#define ASSERT_CHAR(c_ptr, expected) if (*(*c_ptr)++ != expected) return ERR_DIFF_SYNTAX;
#define ASSERT_ALLOC(ptr) if (ptr == NULL) return ERR_DIFF_ALLOC;
/*
This places the function in the .lower section, making it possible to shift the program code
in higher memory regions if needed for an update.
*/
#define LOWER_ATOMIC __attribute__((critical, lower))
typedef struct diff_entry {
    char  opcode;
    uint16_t* addr1;
    uint16_t* addr2;
    uint16_t  no_words;
} diff_entry_t;

dsu_err_t LOWER_ATOMIC decode_w(diff_entry_t* diff_entry, char** diff_p) {
    diff_entry->opcode = 'W';
    diff_entry->addr1 = axtoaddr(diff_p);

    // We should be pointing to a ':'
    ASSERT_CHAR(diff_p, ':');

    // Next is the number of words argument
    // This is a write op, so addr2 is an array containing the data to write
    uint16_t no_words = axtoi16(diff_p);
    diff_entry->no_words = no_words;
    diff_entry->addr2 = malloc(no_words * sizeof(uint16_t));
    ASSERT_ALLOC(diff_entry->addr2);

    // We should be pointing to a 'w'
    ASSERT_CHAR(diff_p, 'w');

    // Next is data
    int i = 0;
    for (; i < no_words; i++) {
        diff_entry->addr2[i] = axtoi16(diff_p);
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
dsu_err_t LOWER_ATOMIC decode(char* diff, diff_entry_t** diff_arr, uint16_t diff_size) {
    // TODO: Remove need for dynamic memory allocation
    // initial address and instruction
    char* diff_p = diff;
    uint16_t cur_op_i = 0;
    int max_ops = diff_size;
    
    do {
        // Resize the array if necessary
        if (cur_op_i >= max_ops) {
            // Double the size of the array
            max_ops <<= 1;
            *diff_arr = realloc(diff_arr, max_ops);
            ASSERT_ALLOC(diff_arr);
        }

        char opcode = *diff_p++;
        switch (opcode) {
            case 'W':
                if (decode_w(&(*diff_arr)[cur_op_i], &diff_p) != OK) {
                    return ERR_DIFF_SYNTAX;
                }
                break;

            case 'S':
                if (decode_s(&(*diff_arr)[cur_op_i], &diff_p) != OK) {
                    return ERR_DIFF_SYNTAX;
                }
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

void LOWER_ATOMIC cleanup(diff_entry_t* diff_arr, int diff_size) {
    // TODO: Remove need for dynamic memory allocation
    int i;
    for (i = 0; i < diff_size; i++) {
        if (diff_arr[i].opcode == 'W')
            free(diff_arr[i].addr2);
    }
    free(diff_arr);
}


int LOWER_ATOMIC update(char* diff) {
    // TODO: Remove need for dynamic memory allocation
    int size = 4;
    diff_entry_t* diff_arr = calloc(size, sizeof(diff_entry_t));
    dsu_err_t result = decode(diff, &diff_arr, size);
    if (result != OK) {
        return result;
    }
    apply(diff_arr);
    cleanup(diff_arr, size);
    return OK;
}
