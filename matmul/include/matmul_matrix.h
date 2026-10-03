#ifndef MATMUL_MATRIX_H
#define MATMUL_MATRIX_H

#include "matmul_problem.h"

#include <stddef.h>

int matmul_matrix_source_dimension(const char *source,
                                   int *dimension,
                                   char *error,
                                   size_t error_size);
int matmul_matrix_init(matmul_problem *problem,
                       const char *source_a,
                       const char *source_b,
                       char *error,
                       size_t error_size);
void matmul_matrix_zero(double *matrix, int n);
double matmul_matrix_sum(const double *matrix, int n);

#endif
