#include <stdio.h>
#include "math.h"

#define IMAGE_WIDTH 255
#define IMAGE_HEIGHT 255

int main() {
    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    
    for (int j = 0; j < IMAGE_HEIGHT; ++j) {
        for (int i = 0; i < IMAGE_WIDTH; ++i) {
            int r = i;
            int g = j;
            int b = 0.0f;

            printf("%d %d %d\n", r, g, b);
        }
    }
    return 0;
}