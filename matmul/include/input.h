#ifndef INPUT_H
#define INPUT_H

#include "matrix.h"

#include <stddef.h>

#define MATRIX_SEED_LENGTH 8

int input_resolve_size(const char *source_a,
                       const char *source_b,
                       int requested_n,
                       int size_specified,
                       int *n,
                       char *error,
                       size_t error_size);
void input_generate_seed(char seed[MATRIX_SEED_LENGTH + 1]);
int input_fill_matrix(matrix *value,
                      const char *source,
                      const char *seed,
                      char stream,
                      char *error,
                      size_t error_size);
int matrix_read_file(matrix *value, const char *path,
                     char *error, size_t error_size);
void matrix_fill_random(matrix *value, const char *seed, char stream);

#endif
