#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int n;
    double *data;
} matrix;

void matrix_zero(matrix *value);
double matrix_sum(const matrix *value);

#endif
