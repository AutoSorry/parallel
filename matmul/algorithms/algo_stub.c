#include "app.h"

static void run_stub(kernel_data *data, const kernel_options *options) {
    (void)options;
    matmul_task *task = data->task;
    for (int i = 0; i < task->c->n * task->c->n; ++i) {
        task->c->data[i] = 0.0;
    }
}

void matmul_register_stub(void) {
    static const kernel_module module = {
        "stub", "placeholder algorithm, expected to fail validation", run_stub
    };
    kernel_register(&module);
}
