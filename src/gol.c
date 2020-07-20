#include "gol.h"

void gol_iterate(uint32_t *grid, size_t width, size_t height) {
    size_t grid_len  = width * height;
    uint32_t *buffer = (uint32_t*)malloc(sizeof(uint32_t) * grid_len);

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            uint32_t cell = grid[y * width + x];

            uint32_t upper  = y > 0          ? grid[(y - 1) * width + x] : 0;
            uint32_t bottom = y < height - 1 ? grid[(y + 1) * width + x] : 0;
            uint32_t left   = x > 0          ? grid[y * width + (x - 1)] : 0;
            uint32_t right  = x < width - 1  ? grid[y * width + (x + 1)] : 0;

            uint32_t upper_left   = y > 0          && x > 0         ? grid[(y - 1) * width + (x - 1)] : 0;
            uint32_t upper_right  = y > 0          && x < width - 1 ? grid[(y - 1) * width + (x + 1)] : 0;
            uint32_t bottom_left  = y < height - 1 && x > 0         ? grid[(y + 1) * width + (x - 1)] : 0;
            uint32_t bottom_right = y < height - 1 && x < width - 1 ? grid[(y + 1) * width + (x + 1)] : 0;

            uint32_t alives = upper + bottom + left + right + upper_left + upper_right + bottom_left + bottom_right;

            if (cell)
                buffer[y * width + x] = alives >= 2 && alives <= 3;
            else
                buffer[y * width + x] = alives == 3;
        }
    }

    for (size_t i = 0; i < grid_len; i++)
        grid[i] = buffer[i];

    free(buffer);
    buffer = NULL;
}