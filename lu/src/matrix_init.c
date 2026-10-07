#include "lu_matrix.h"

#include <time.h>

static unsigned int next_value(unsigned int *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static unsigned int seed_state(const char *seed) {
    unsigned int state = 2166136261u;
    for (int i = 0; i < LU_SEED_LENGTH; ++i) {
        state ^= (unsigned char)seed[i];
        state *= 16777619u;
    }
    return state;
}

void lu_matrix_init(matrix *value, const char *seed) {
    int n = value->n;
    unsigned int state = seed_state(seed);

    for (int i = 0; i < n; ++i) {
        double off_diagonal_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double entry =
                    ((double)(next_value(&state) % 1001u) / 1000.0) - 0.5;
                value->data[i * n + j] = entry;
                off_diagonal_sum += entry < 0.0 ? -entry : entry;
            }
        }
        value->data[i * n + i] = off_diagonal_sum + 1.0;
    }
}

void lu_generate_seed(char seed[LU_SEED_LENGTH + 1]) {
    struct timespec now;
    unsigned long long value;
    if (clock_gettime(CLOCK_REALTIME, &now) == 0) {
        value = (unsigned long long)now.tv_sec ^
                (unsigned long long)now.tv_nsec;
    } else {
        value = (unsigned long long)time(NULL);
    }

    for (int i = 0; i < LU_SEED_LENGTH; ++i) {
        value = value * 6364136223846793005ULL + 1442695040888963407ULL;
        seed[i] = (char)('A' + (value % 26));
    }
    seed[LU_SEED_LENGTH] = '\0';
}

void matrix_copy(matrix *destination, const matrix *source) {
    for (int i = 0; i < source->n * source->n; ++i) {
        destination->data[i] = source->data[i];
    }
}
