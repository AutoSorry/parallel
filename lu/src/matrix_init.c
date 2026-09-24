#include "lu_matrix.h"

void lu_matrix_init(double *matrix, int n) {
    for (int i = 0; i < n; ++i) {
        double off_diagonal_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double value = 0.01 * (double)(((i + 3) * (j + 5)) % 17 + 1);
                matrix[i * n + j] = value;
                off_diagonal_sum += value;
            }
        }
        matrix[i * n + i] = off_diagonal_sum + 1.0;
    }
}
