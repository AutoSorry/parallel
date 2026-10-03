#include "app.h"

static void run_naive(kernel_data *data, const kernel_options *options) {
    (void)options;
    matmul_problem *problem = data->problem;

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

void matmul_register_naive(void) {
    static const kernel_module module = {
        "naive", "serial triple-loop baseline", run_naive
    };
    kernel_register(&module);
}
