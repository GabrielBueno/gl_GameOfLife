#include "matrix.h"

#include <stdio.h>

void fill_white_noise(uint32_t *matrix, size_t width, size_t height) {
    size_t len = width * height;

    for (size_t i = 0; i < len; i++) 
        matrix[i] = rand() % 2;
}

void display_matrix(uint32_t *matrix, size_t width, size_t height) {
    size_t len = width * height;

    for (size_t i = 0; i < len; i++) {
        printf("%d ", matrix[i]);

        if ((i + 1) % width == 0)
            printf("\n");
    }
}