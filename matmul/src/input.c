#include "input.h"
#include "matrix.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static unsigned int next_value(unsigned int *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static unsigned int seed_state(const char *seed, char stream) {
    unsigned int state = 2166136261u;
    for (int i = 0; i < 8; ++i) {
        state ^= (unsigned char)seed[i];
        state *= 16777619u;
    }
    state ^= (unsigned char)stream;
    state *= 16777619u;
    return state;
}

void matrix_fill_random(matrix *value, const char *seed, char stream) {
    unsigned int state = seed_state(seed, stream);

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

static int matrix_source_dimension(const char *source,
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

int input_resolve_size(const char *source_a,
                       const char *source_b,
                       int requested_n,
                       int size_specified,
                       int *n,
                       char *error,
                       size_t error_size) {
    int source_a_n;
    int source_b_n;
    if (matrix_source_dimension(source_a, &source_a_n,
                                error, error_size) != 0 ||
        matrix_source_dimension(source_b, &source_b_n,
                                error, error_size) != 0) {
        return -1;
    }

    if (source_a_n != 0 && source_b_n != 0 && source_a_n != source_b_n) {
        snprintf(error, error_size,
                 "matrix input files must have the same dimension");
        return -1;
    }

    int file_n = source_a_n != 0 ? source_a_n : source_b_n;
    if (file_n == 0) {
        if (!size_specified) {
            snprintf(error, error_size,
                     "--size is required when both inputs are random");
            return -1;
        }
        *n = requested_n;
        return 0;
    }

    if (size_specified && requested_n != file_n) {
        snprintf(error, error_size,
                 "--size does not match the matrix file dimension");
        return -1;
    }

    *n = file_n;
    return 0;
}

void input_generate_seed(char seed[MATRIX_SEED_LENGTH + 1]) {
    struct timespec now;
    unsigned long long value;
    if (clock_gettime(CLOCK_REALTIME, &now) == 0) {
        value = (unsigned long long)now.tv_sec ^
                (unsigned long long)now.tv_nsec;
    } else {
        value = (unsigned long long)time(NULL);
    }

    for (int i = 0; i < MATRIX_SEED_LENGTH; ++i) {
        value = value * 6364136223846793005ULL + 1442695040888963407ULL;
        seed[i] = (char)('A' + (value % 26));
    }
    seed[MATRIX_SEED_LENGTH] = '\0';
}

int input_fill_matrix(matrix *value,
                      const char *source,
                      const char *seed,
                      char stream,
                      char *error,
                      size_t error_size) {
    if (strcmp(source, "random") == 0) {
        matrix_fill_random(value, seed, stream);
        return 0;
    }
    return matrix_read_file(value, source, error, error_size);
}
