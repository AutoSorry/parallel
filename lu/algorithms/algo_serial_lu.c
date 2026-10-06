#include "lu.h"

#include <math.h>
#include <stdio.h>

static void run_serial_lu(kernel_data *data, const kernel_options *options) {
    (void)options;
    lu_task *task = data->task;
    int n = task->a->n;

    matrix_copy(task->packed_lu, task->a);

    for (int k = 0; k < n; ++k) {
        double pivot = task->packed_lu->data[k * n + k];
        if (fabs(pivot) < 1e-15) {
            fprintf(stderr, "serial_lu: zero pivot at row %d\n", k);
            return;
        }

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

void lu_register_serial_lu(void) {
    static const kernel_module module = {
        "serial_lu", "serial LU without pivoting, packed L/U output", run_serial_lu
    };
    kernel_register(&module);
}
