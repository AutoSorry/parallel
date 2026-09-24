#include "matmul.h"

#ifdef _OPENMP
#include <omp.h>
#endif

static void matmul_openmp(const double *A, const double *B, double *C, int n, const matmul_options *options) {
#ifdef _OPENMP
    int requested_threads = options ? options->threads : 0;
    int threads = requested_threads > 0 ? requested_threads : omp_get_max_threads();
#pragma omp parallel for num_threads(threads)
#endif
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

REGISTER_ALGORITHM("openmp", "OpenMP parallel outer-loop baseline", matmul_openmp)
