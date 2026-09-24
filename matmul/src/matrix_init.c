#include "matmul_matrix.h"

static unsigned int next_value(unsigned int *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

void matmul_matrix_init(double *a, double *b, double *c, int n) {
    unsigned int state = 123456789u;

    for (int i = 0; i < n * n; ++i) {
        a[i] = (double)(next_value(&state) % 1000u) / 1000.0;
        b[i] = (double)(next_value(&state) % 1000u) / 1000.0;
        c[i] = 0.0;
    }
}
