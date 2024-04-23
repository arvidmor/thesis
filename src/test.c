#include <stdio.h>
#include "utils.h"
#include "update.h"

char diff[256] = {0};

__INT16_TYPE__ text[] = {
    1, 2, 3, 4
};
__INT16_TYPE__ text2[] = {
    1,1,0,0
};
__INT16_TYPE__ text3[] = {
    0,0,0,0
};

int main(void) {
    sprintf(diff, "W%llx:0004w0001000200030004;S%llx-%llx:0002w;W%llx:0002w00010002;W%llx:0002w00030004", text, text2, text2+1, text3, text3+2);

    printf("Diff: %s\n", diff);

    if (update(diff)) {
        printf("Error\n");
    }
    printf("Text:");
    for (int i = 0; i < 4; i++) {
        printf(" %hu", text[i]);
    }
    printf("\nText2:");
    for (int i = 0; i < 4; i++) {
        printf(" %hu", text2[i]);
    }
    printf("\nText3:");
    for (int i = 0; i < 4; i++) {
        printf(" %hu", text3[i]);
    }
    printf("\n");
    return 0;
}
