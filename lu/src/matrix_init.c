#include "lu_matrix.h"

void lu_matrix_init(matrix *value) {
    int n = value->n;
    for (int i = 0; i < n; ++i) {
        double off_diagonal_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double entry = 0.01 * (double)(((i + 3) * (j + 5)) % 17 + 1);
                value->data[i * n + j] = entry;
                off_diagonal_sum += entry;
            }
        }
        value->data[i * n + i] = off_diagonal_sum + 1.0;
    }
}

void matrix_copy(matrix *destination, const matrix *source) {
    for (int i = 0; i < source->n * source->n; ++i) {
        destination->data[i] = source->data[i];
    }
}

void matrix_zero(matrix *value) {
    for (int i = 0; i < value->n * value->n; ++i) {
        value->data[i] = 0.0;
    }
}
