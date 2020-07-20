#ifndef GOL_MATRIX_H_
#define GOL_MATRIX_H_

#include <stdlib.h>
#include <stdint.h>

void fill_white_noise(uint32_t *matrix, size_t width, size_t height);
void display_matrix(uint32_t *matrix, size_t width, size_t height);

#endif