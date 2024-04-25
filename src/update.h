#pragma once
#include "utils.h"

// Type aliases
typedef enum {
    OK,
    ERR_DIFF_SYNTAX,
    ERR_DIFF_OOM,
} dsu_err_t;

dsu_err_t decode(char* diff);
dsu_err_t update(char* diff);
