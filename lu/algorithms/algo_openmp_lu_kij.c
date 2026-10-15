#include "lu.h"

#include <stddef.h>

#ifdef _OPENMP
#include <omp.h>
#endif

static void run_openmp_lu_kij(kernel_data *data,
                              const kernel_options *options) {
    lu_task *task = data->task;
    matrix *a = task->a;
    int n = a->n;
    int threads = options != NULL ? options->threads : 0;

#ifdef _OPENMP
#pragma omp parallel num_threads(threads > 0 ? threads : omp_get_max_threads())
    {
        for (int k = 0; k < n; ++k) {
            double pivot = a->data[k * n + k];

#pragma omp for schedule(static)
            for (int i = k + 1; i < n; ++i) {
                a->data[i * n + k] /= pivot;
                for (int j = k + 1; j < n; ++j) {
                    a->data[i * n + j] -=
                        a->data[i * n + k] * a->data[k * n + j];
                }
            }
        }
    }
#else
    (void)threads;
#endif
}

void lu_register_openmp_lu_kij(void) {
    static const kernel_module module = {
        "openmp_lu_kij",
        "OpenMP LU with k-i-j loop order and static scheduling",
        run_openmp_lu_kij
    };
    kernel_register(&module);
}
