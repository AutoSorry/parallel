#include "matmul_matrix.h"

static unsigned int next_value(unsigned int *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

void matmul_matrix_init(matmul_problem *problem) {
    unsigned int state = 123456789u;

    for (int i = 0; i < problem->n * problem->n; ++i) {
        problem->c[i] = 0.0;
        problem->a[i] = (double)(next_value(&state) % 1000u) / 1000.0;
        problem->b[i] = (double)(next_value(&state) % 1000u) / 1000.0;
    }
}

void matmul_matrix_zero(double *matrix, int n) {
    for (int i = 0; i < n * n; ++i) {
        matrix[i] = 0.0;
    }
}

double matmul_matrix_sum(const double *matrix, int n) {
    double sum = 0.0;
    for (int i = 0; i < n * n; ++i) {
        sum += matrix[i];
    }
    return sum;
}
