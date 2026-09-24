#include "matmul.h"

static void matmul_naive(const double *A, const double *B, double *C, int n, const matmul_options *options) {
    (void)options;

    for (int i = 0; i < n * n; ++i) {
        C[i] = 0.0;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

REGISTER_ALGORITHM("naive", "serial triple-loop baseline", matmul_naive)
