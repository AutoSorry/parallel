#include "lu.h"

#include <math.h>
#include <stdio.h>

static void run_serial_lu_kij(kernel_data *data,
                              const kernel_options *options) {
    (void)options;
    lu_task *task = data->task;
    matrix *a = task->a;
    int n = a->n;

    for (int k = 0; k < n; ++k) {
        double pivot = a->data[k * n + k];
        if (fabs(pivot) < 1e-15) {
            fprintf(stderr, "serial_lu_kij: zero pivot at row %d\n", k);
            return;
        }

        for (int i = k + 1; i < n; ++i) {
            a->data[i * n + k] /= pivot;
            for (int j = k + 1; j < n; ++j) {
                a->data[i * n + j] -=
                    a->data[i * n + k] * a->data[k * n + j];
            }
        }
    }
}

void lu_register_serial_lu_kij(void) {
    static const kernel_module module = {
        "serial_lu_kij",
        "serial LU with k-i-j loop order",
        run_serial_lu_kij
    };
    kernel_register(&module);
}
