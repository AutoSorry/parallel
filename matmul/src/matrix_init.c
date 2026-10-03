#include "matmul_matrix.h"

#include <stdio.h>
#include <string.h>

static unsigned int next_value(unsigned int *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static void fill_random(double *matrix, int n, unsigned int seed) {
    unsigned int state = seed;

    for (int i = 0; i < n * n; ++i) {
        matrix[i] = (double)(next_value(&state) % 1000u) / 1000.0;
    }
}

static int load_file(double *matrix, int n, const char *source,
                     char *error, size_t error_size) {
    FILE *file = fopen(source, "r");
    if (file == NULL) {
        snprintf(error, error_size, "cannot open matrix file '%s'", source);
        return -1;
    }

    int file_n = 0;
    if (fscanf(file, "%d", &file_n) != 1 || file_n != n) {
        fclose(file);
        snprintf(error, error_size,
                 "matrix file '%s' has an invalid dimension", source);
        return -1;
    }

    for (int i = 0; i < n * n; ++i) {
        if (fscanf(file, "%lf", &matrix[i]) != 1) {
            fclose(file);
            snprintf(error, error_size,
                     "matrix file '%s' has too few values", source);
            return -1;
        }
    }

    char extra;
    if (fscanf(file, " %c", &extra) == 1) {
        fclose(file);
        snprintf(error, error_size,
                 "matrix file '%s' has extra data", source);
        return -1;
    }

    fclose(file);
    return 0;
}

int matmul_matrix_source_dimension(const char *source,
                                   int *dimension,
                                   char *error,
                                   size_t error_size) {
    if (strcmp(source, "random") == 0) {
        *dimension = 0;
        return 0;
    }

    FILE *file = fopen(source, "r");
    if (file == NULL) {
        snprintf(error, error_size, "cannot open matrix file '%s'", source);
        return -1;
    }

    if (fscanf(file, "%d", dimension) != 1 || *dimension <= 0) {
        fclose(file);
        snprintf(error, error_size,
                 "matrix file '%s' has an invalid dimension", source);
        return -1;
    }

    fclose(file);
    return 0;
}

int matmul_matrix_init(matmul_problem *problem,
                       const char *source_a,
                       const char *source_b,
                       char *error,
                       size_t error_size) {
    if (strcmp(source_a, "random") == 0) {
        fill_random(problem->a, problem->n, 123456789u);
    } else if (load_file(problem->a, problem->n, source_a,
                         error, error_size) != 0) {
        return -1;
    }

    if (strcmp(source_b, "random") == 0) {
        fill_random(problem->b, problem->n, 987654321u);
    } else if (load_file(problem->b, problem->n, source_b,
                         error, error_size) != 0) {
        return -1;
    }

    matmul_matrix_zero(problem->c, problem->n);
    return 0;
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
