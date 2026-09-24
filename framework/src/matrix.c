#include "matrix.h"

void matrix_zero(double *matrix, int n) {
    for (int i = 0; i < n * n; ++i) {
        matrix[i] = 0.0;
    }
}

void matrix_copy(double *destination, const double *source, int n) {
    for (int i = 0; i < n * n; ++i) {
        destination[i] = source[i];
    }
}

double matrix_sum(const double *matrix, int n) {
    double sum = 0.0;
    for (int i = 0; i < n * n; ++i) {
        sum += matrix[i];
    }
    return sum;
}
