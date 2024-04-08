#include <stdio.h>
char diff[256] = {0};

__INT16_TYPE__ text[] = {
    1, 2, 3, 4
};
__INT16_TYPE__ text2[] = {
    4, 3, 2, 1
};

__INT16_TYPE__ hexval(char c) {
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

unsigned int axtoi( char *num, char** instruction) {
    unsigned int value = 0;
    while (*num && ishexdigit(*num))
        value = value * 16 + hexval(*num++);

    num++;
    *instruction = num;
    return value;
}

// Convert a hex string to an integer and update the pointer to the first character after the number
__INT16_TYPE__ *axtoaddr(char **num_p) {
    unsigned int value = 0;
    char *cur_c = *num_p;
    while (ishexdigit(*cur_c))
        value = value * 16 + hexval(*cur_c++);

    if (*cur_c++ != ':') {
        printf("Error: no ':' after address!\n");
        return NULL;
    }
    *num_p = cur_c;
    return (__INT16_TYPE__*)(__UINTPTR_TYPE__)value;
}

// Converts two hex characters to an integer of 16 bits, updating the pointer to the next character
unsigned int axtoi16(char **num_p) {
    char *cur_c = *num_p;
    unsigned int val = (hexval(*cur_c++) << 4) + hexval(*cur_c++);
    *num_p = cur_c;
    return val;
}

int update() {
    // initial address and instruction
    char* mc_p = diff;
    __INT16_TYPE__* mock_addr = axtoaddr(&mc_p);
    __INT16_TYPE__* addr = text;

    unsigned int val = 0;
    while (*mc_p != '\0') {
        // Get next microcode int 
        val = axtoi16(&mc_p);
        // Write to memory
        *addr++ = val;

        // If new address is coming
        if (*mc_p == ';') {
            mc_p++;
            addr = axtoaddr(&mc_p);
            addr = text2;
        }
    }
}

int main(void) {
    sprintf(diff, "%x:02030405;%x:03020100", text, text2);

    printf("Diff: %s\n", diff);

    if (update()) {
        printf("Error\n");
    }

    for (int i = 0; i < 4; i++) {
        printf("Text[%d]: %d\n", i, text[i]);
    }
    for (int i = 0; i < 4; i++) {
        printf("Text2[%d]: %d\n", i, text2[i]);
    }
    return 0;
}

