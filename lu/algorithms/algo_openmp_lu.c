#include "lu.h"

#include <math.h>
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

static void run_openmp_lu(kernel_data *data,
                           const kernel_options *options) {
    lu_task *task = data->task;
    int n = task->a->n;
    int threads = options != NULL ? options->threads : 0;

    matrix_copy(task->packed_lu, task->a);

    for (int k = 0; k < n; ++k) {
        double pivot = task->packed_lu->data[k * n + k];
        if (fabs(pivot) < 1e-15) {
            fprintf(stderr, "openmp_lu: zero pivot at row %d\n", k);
            return;
        }

#ifdef _OPENMP
#pragma omp parallel for num_threads(threads > 0 ? threads : omp_get_max_threads())
#endif
        for (int i = k + 1; i < n; ++i) {
            task->packed_lu->data[i * n + k] /= pivot;
            for (int j = k + 1; j < n; ++j) {
                task->packed_lu->data[i * n + j] -=
                    task->packed_lu->data[i * n + k] *
                    task->packed_lu->data[k * n + j];
            }
        }
    }
}

void lu_register_openmp_lu(void) {
    static const kernel_module module = {
        "openmp_lu", "OpenMP LU with parallel row updates", run_openmp_lu
    };
    kernel_register(&module);
}
