#ifndef MATMUL_MATRIX_H
#define MATMUL_MATRIX_H

#include "matmul_problem.h"

void matmul_matrix_init(matmul_problem *problem);
void matmul_matrix_zero(double *matrix, int n);
double matmul_matrix_sum(const double *matrix, int n);

#endif
