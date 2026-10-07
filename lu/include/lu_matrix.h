#ifndef LU_MATRIX_H
#define LU_MATRIX_H

#define LU_SEED_LENGTH 8

typedef struct {
    int n;
    double *data;
} matrix;

void lu_matrix_init(matrix *value, const char *seed);
void lu_generate_seed(char seed[LU_SEED_LENGTH + 1]);
void matrix_copy(matrix *destination, const matrix *source);

#endif
