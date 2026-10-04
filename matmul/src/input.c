#include "input.h"
#include "matrix.h"

#include <stdio.h>
#include <string.h>

static unsigned int next_value(unsigned int *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

void matrix_fill_random(matrix *value, unsigned int seed) {
    unsigned int state = seed;

    for (int i = 0; i < value->n * value->n; ++i) {
        value->data[i] = (double)(next_value(&state) % 1000u) / 1000.0;
    }
}

int matrix_read_file(matrix *value, const char *path,
                     char *error, size_t error_size) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        snprintf(error, error_size, "cannot open matrix file '%s'", path);
        return -1;
    }

    int file_n = 0;
    if (fscanf(file, "%d", &file_n) != 1 || file_n != value->n) {
        fclose(file);
        snprintf(error, error_size,
                 "matrix file '%s' has an invalid dimension", path);
        return -1;
    }

    for (int i = 0; i < value->n * value->n; ++i) {
        if (fscanf(file, "%lf", &value->data[i]) != 1) {
            fclose(file);
            snprintf(error, error_size,
                     "matrix file '%s' has too few values", path);
            return -1;
        }
    }

    char extra;
    if (fscanf(file, " %c", &extra) == 1) {
        fclose(file);
        snprintf(error, error_size,
                 "matrix file '%s' has extra data", path);
        return -1;
    }

    fclose(file);
    return 0;
}

int matrix_source_dimension(const char *source,
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
