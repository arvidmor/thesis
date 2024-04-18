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

int main(void) {
    sprintf(diff, "W%llx:0004w0002000300040005;S%llx-%llx:0002w", text, text2, text2+1);

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
    printf("\n");
    return 0;
}
