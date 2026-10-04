#include "app.h"
#include <stddef.h>

#ifdef _OPENMP
#include <omp.h>
#endif

static void run_openmp(kernel_data *data, const kernel_options *options) {
    matmul_task *task = data->task;
#ifdef _OPENMP
    int requested_threads = options != NULL ? options->threads : 0;
    int threads = requested_threads > 0 ? requested_threads : omp_get_max_threads();
#pragma omp parallel for num_threads(threads)
#else
    (void)options;
#endif
    for (int i = 0; i < task->c->n; ++i) {
        for (int j = 0; j < task->c->n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < task->c->n; ++k) {
                sum += task->a->data[i * task->a->n + k] *
                       task->b->data[k * task->b->n + j];
            }
            task->c->data[i * task->c->n + j] = sum;
        }
    }
}

void matmul_register_openmp(void) {
    static const kernel_module module = {
        "openmp", "OpenMP parallel outer-loop baseline", run_openmp
    };
    kernel_register(&module);
}
