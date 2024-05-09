#pragma once
#include "utils.h"

// Type aliases
typedef enum
{
    OK,
    E_SYNTAX,
    E_OOM,
} DSUERR;

DSUERR decode(char *diff);
DSUERR update(char *diff);
