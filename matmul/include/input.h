#ifndef INPUT_H
#define INPUT_H

#include "problem.h"

#include <stddef.h>

int matrix_source_dimension(const char *source,
                            int *dimension,
                            char *error,
                            size_t error_size);
int matrix_problem_init(matmul_problem *problem,
                        const char *source_a,
                        const char *source_b,
                        char *error,
                        size_t error_size);

#endif
