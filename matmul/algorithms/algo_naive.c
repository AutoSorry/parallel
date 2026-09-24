#include "matmul.h"

static void run_naive(kernel_data *data, const kernel_options *options) {
    (void)options;

    for (int i = 0; i < data->n; ++i) {
        for (int j = 0; j < data->n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < data->n; ++k) {
                sum += data->a[i * data->n + k] * data->b[k * data->n + j];
            }
            data->c[i * data->n + j] = sum;
        }
    }
}

void matmul_register_naive(void) {
    static const kernel_module module = {
        "naive", "serial triple-loop baseline", run_naive
    };
    kernel_register(&module);
}
