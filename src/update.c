#include "utils.h"
#include "update.h"

#define ASSERT_CHAR(c_ptr, expected) if (*(*c_ptr)++ != expected) return ERR_DIFF_SYNTAX;
#define ASSERT_ALLOC(ptr) if (ptr == NULL) return ERR_DIFF_ALLOC;

typedef struct diff_entry {
    char  opcode;
    uint16_t* addr1;
    uint16_t* addr2;
    uint16_t  no_words;
} diff_entry_t;

DIFF_ERROR_T decode_w(diff_entry_t* diff_entry, char** diff_p) {
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

DIFF_ERROR_T decode_s(diff_entry_t* diff_entry, char** diff_p) {
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
DIFF_ERROR_T decode(char* diff, diff_entry_t** diff_arr, uint16_t diff_size) {
    // TODO: Remove need for dynamic memory allocation
    // initial address and instruction
    char* diff_p = diff;
    uint16_t cur_op_i = 0;
    int max_ops = diff_size;
    
    do {
        // Resize the array if necessary
        if (cur_op_i >= max_ops) {
            max_ops *= 2;
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


void apply(diff_entry_t* diff_arr) {
    int i = 0;
    char opcode;
    do {
        diff_entry_t entry = diff_arr[i++];
        opcode = entry.opcode;
        switch (opcode) {
            // TODO: Look into optimizing with DMA
            case 'W':
                for (int j = 0; j < entry.no_words; j++) 
                    entry.addr1[j] = entry.addr2[j];
                
                break;

            case 'S':
                uint16_t* src = entry.addr1;
                uint16_t* dst = entry.addr1 + entry.no_words;
                uint16_t size = entry.addr2 - entry.addr1 + 1;
                for (int j = size-1; j >= 0; j--) 
                    dst[j] = src[j];
                
                break;

            default:
                break;
        }
    } while (opcode != 0);
}

void cleanup(diff_entry_t* diff_arr, int diff_size) {
    // TODO: Remove need for dynamic memory allocation
    int i;
    for (i = 0; i < diff_size; i++) {
        if (diff_arr[i].opcode == 'W')
            free(diff_arr[i].addr2);
    }
    free(diff_arr);
}

int update(char* diff) {
    // TODO: Remove need for dynamic memory allocation
    int size = 4;
    diff_entry_t* diff_arr = calloc(size, sizeof(diff_entry_t));
    DIFF_ERROR_T result = decode(diff, &diff_arr, size);
    if (result != OK) {
        return result;
    }
    apply(diff_arr);
    cleanup(diff_arr, size);
    return OK;
}
