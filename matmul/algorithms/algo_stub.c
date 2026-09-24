#include "matmul.h"

static void run_stub(kernel_data *data, const kernel_options *options) {
    (void)options;
    for (int i = 0; i < data->n * data->n; ++i) {
        data->c[i] = 0.0;
    }
}

void matmul_register_stub(void) {
    static const kernel_module module = {
        "stub", "placeholder algorithm, expected to fail validation", run_stub
    };
    kernel_register(&module);
}
