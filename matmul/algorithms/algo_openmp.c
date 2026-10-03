#include "app.h"
#include <stddef.h>

#ifdef _OPENMP
#include <omp.h>
#endif

static void run_openmp(kernel_data *data, const kernel_options *options) {
    matmul_problem *problem = data->problem;
#ifdef _OPENMP
    int requested_threads = options != NULL ? options->threads : 0;
    int threads = requested_threads > 0 ? requested_threads : omp_get_max_threads();
#pragma omp parallel for num_threads(threads)
#else
    (void)options;
#endif
    for (int i = 0; i < problem->n; ++i) {
        for (int j = 0; j < problem->n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < problem->n; ++k) {
                sum += problem->a[i * problem->n + k] *
                       problem->b[k * problem->n + j];
            }
            problem->c[i * problem->n + j] = sum;
        }
    }
}

void matmul_register_openmp(void) {
    static const kernel_module module = {
        "openmp", "OpenMP parallel outer-loop baseline", run_openmp
    };
    kernel_register(&module);
}
