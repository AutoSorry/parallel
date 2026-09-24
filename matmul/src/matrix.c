#include "matrix.h"

void matrix_zero(double *C, int n) {
    for (int i = 0; i < n * n; ++i) {
        C[i] = 0.0;
    }
}

void matrix_init(double *A, double *B, double *C, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i * n + j] = (double)(i + j + 1);
            B[i * n + j] = 1.0;
            C[i * n + j] = 0.0;
        }
    }
}

double matrix_sum(const double *C, int n) {
    double sum = 0.0;
    for (int i = 0; i < n * n; ++i) {
        sum += C[i];
    }
    return sum;
}
