#ifndef LU_MATRIX_H
#define LU_MATRIX_H

typedef struct {
    int n;
    double *data;
} matrix;

void lu_matrix_init(matrix *value);
void matrix_copy(matrix *destination, const matrix *source);
void matrix_zero(matrix *value);

#endif
