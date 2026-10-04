#ifndef INPUT_H
#define INPUT_H

#include "matrix.h"

#include <stddef.h>

int matrix_source_dimension(const char *source,
                            int *dimension,
                            char *error,
                            size_t error_size);
int matrix_read_file(matrix *value, const char *path,
                     char *error, size_t error_size);
void matrix_fill_random(matrix *value, unsigned int seed);

#endif
