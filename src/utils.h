#pragma once

// Type aliases
typedef struct diff_entry diff_op_t;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

void init_diff(char* diff);

int update(char* diff);
