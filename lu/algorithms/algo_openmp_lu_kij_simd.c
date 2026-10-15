#include "lu.h"

#include <stddef.h>

#ifdef _OPENMP
#include <omp.h>
#endif

static void run_openmp_lu_kij_simd(kernel_data *data,
                                   const kernel_options *options) {
    lu_task *task = data->task;
    matrix *a = task->a;
    int n = a->n;
    int threads = options != NULL ? options->threads : 0;

#ifdef _OPENMP
#pragma omp parallel num_threads(threads > 0 ? threads : omp_get_max_threads())
    {
        for (int k = 0; k < n; ++k) {
            double *pivot_row = &a->data[k * n];
            double pivot = pivot_row[k];

#pragma omp for schedule(static)
            for (int i = k + 1; i < n; ++i) {
                double *current_row = &a->data[i * n];
                current_row[k] /= pivot;
                double multiplier = current_row[k];

#pragma omp simd
                for (int j = k + 1; j < n; ++j) {
                    current_row[j] -= multiplier * pivot_row[j];
                }
            }
        }
    }
#else
    (void)threads;
#endif
}

void lu_register_openmp_lu_kij_simd(void) {
    static const kernel_module module = {
        "openmp_lu_kij_simd",
        "OpenMP LU with parallel rows and SIMD columns",
        run_openmp_lu_kij_simd
    };
    kernel_register(&module);
}
