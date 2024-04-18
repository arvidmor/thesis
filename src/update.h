#pragma once
#include "utils.h"

// Type aliases
typedef struct diff_entry diff_entry_t;
typedef enum {
    OK,
    ERR_DIFF_SYNTAX,
    ERR_DIFF_ALLOC,
} DIFF_ERROR_T;

DIFF_ERROR_T decode(char* diff, diff_entry_t** diff_arr, uint16_t diff_size);
int update(char* diff);