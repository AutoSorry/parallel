#include "matmul.h"

static void matmul_stub(const double *A, const double *B, double *C, int n, const matmul_options *options) {
    (void)A;
    (void)B;
    (void)options;

    for (int i = 0; i < n * n; ++i) {
        C[i] = 0.0;
    }
}

REGISTER_ALGORITHM("stub", "placeholder algorithm, expected to fail validation", matmul_stub)
