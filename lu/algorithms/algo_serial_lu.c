#include "lu.h"

#include <math.h>
#include <stdio.h>

static void run_serial_lu(kernel_data *data, const kernel_options *options) {
    (void)options;

    for (int i = 0; i < data->n * data->n; ++i) {
        data->c[i] = data->a[i];
    }

    for (int k = 0; k < data->n; ++k) {
        double pivot = data->c[k * data->n + k];
        if (fabs(pivot) < 1e-15) {
            fprintf(stderr, "serial_lu: zero pivot at row %d\n", k);
            return;
        }

        for (int i = k + 1; i < data->n; ++i) {
            data->c[i * data->n + k] /= pivot;
            for (int j = k + 1; j < data->n; ++j) {
                data->c[i * data->n + j] -=
                    data->c[i * data->n + k] * data->c[k * data->n + j];
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
