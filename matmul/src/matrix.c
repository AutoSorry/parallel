#include "matrix.h"

void matmul_matrix_zero(double *matrix, int n) {
    for (int i = 0; i < n * n; ++i) {
        matrix[i] = 0.0;
    }
}

double matmul_matrix_sum(const double *matrix, int n) {
    double sum = 0.0;
    for (int i = 0; i < n * n; ++i) {
        sum += matrix[i];
    }
    return sum;
}
