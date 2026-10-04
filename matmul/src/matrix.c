#include "matrix.h"

void matrix_zero(matrix *value) {
    for (int i = 0; i < value->n * value->n; ++i) {
        value->data[i] = 0.0;
    }
}

double matrix_sum(const matrix *value) {
    double sum = 0.0;
    for (int i = 0; i < value->n * value->n; ++i) {
        sum += value->data[i];
    }
    return sum;
}
