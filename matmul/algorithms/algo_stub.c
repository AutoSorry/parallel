#include "app.h"

static void run_stub(kernel_data *data, const kernel_options *options) {
    (void)options;
    matmul_problem *problem = data->problem;
    for (int i = 0; i < problem->n * problem->n; ++i) {
        problem->c[i] = 0.0;
    }
}

void matmul_register_stub(void) {
    static const kernel_module module = {
        "stub", "placeholder algorithm, expected to fail validation", run_stub
    };
    kernel_register(&module);
}
