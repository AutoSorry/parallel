#include "app.h"

static void run_naive(kernel_data *data, const kernel_options *options) {
    (void)options;
    matmul_task *task = data->task;

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

void matmul_register_naive(void) {
    static const kernel_module module = {
        "naive", "serial triple-loop baseline", run_naive
    };
    kernel_register(&module);
}
