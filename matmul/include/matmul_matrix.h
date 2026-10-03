#ifndef MATMUL_MATRIX_H
#define MATMUL_MATRIX_H

typedef struct {
    int n;
    double *a;
    double *b;
    double *c;
} matmul_problem;

void matmul_matrix_init(matmul_problem *problem);
void matmul_matrix_zero(double *matrix, int n);
double matmul_matrix_sum(const double *matrix, int n);

#endif
