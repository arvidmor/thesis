#include <stdio.h>
#include "utils.h"

char diff[256] = {0};

__INT16_TYPE__ text[] = {
    1, 2, 3, 4
};
__INT16_TYPE__ text2[] = {
    4, 3, 2, 1
};

int main(void) {
    sprintf(diff, "%x-02:0002000300040005;%x-02:0003000200010000", text, text2);

    printf("Diff: %s\n", diff);

    if (update(diff)) {
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

