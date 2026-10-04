#include "app.h"
#include "cli.h"
#include "validate.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void default_seed(char *seed) {
    struct timespec now;
    unsigned long long value;
    if (clock_gettime(CLOCK_REALTIME, &now) == 0) {
        value = (unsigned long long)now.tv_sec ^
                (unsigned long long)now.tv_nsec;
    } else {
        value = (unsigned long long)time(NULL);
    }
    for (int i = 0; i < SEED_LENGTH; ++i) {
        value = value * 6364136223846793005ULL + 1442695040888963407ULL;
        seed[i] = (char)('A' + (value % 26));
    }
    seed[SEED_LENGTH] = '\0';
}

static int run_module(const kernel_module *module,
                      kernel_data *data,
                      const matrix *reference,
                      int runs,
                      const kernel_options *options) {
    double total_ms = 0.0;

    printf("\nmodule: %s\n", module->name);
    matmul_task *task = data->task;
    for (int run = 0; run < runs; ++run) {
        matrix_zero(task->c);
        double start = now_seconds();
        module->run(data, options);
        double end = now_seconds();
        double milliseconds = (end - start) * 1000.0;
        total_ms += milliseconds;
        printf("  run %d: %.3f ms\n", run + 1, milliseconds);
    }

    printf("  avg: %.3f ms\n", total_ms / runs);
    printf("  sum: %.6f\n", matrix_sum(task->c));
    printf("  validation: ");
    int passed = matmul_validate_result(task->c->data, reference->data,
                                        task->c->n);
    printf("%s\n", passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}

void matmul_register_algorithms(void) {
    matmul_register_naive();
    matmul_register_openmp();
    matmul_register_stub();
}

int main(int argc, char **argv) {
    cli_options cli;

    matmul_register_algorithms();

    cli_defaults(&cli);
    if (cli_parse(&cli, argc, argv) != 0) {
        return 1;
    }
    if (cli.show_list) {
        kernel_list_all();
        return 0;
    }

    char error[256];
    int source_a_n = 0;
    int source_b_n = 0;
    if (matrix_source_dimension(cli.source_a, &source_a_n,
                                error, sizeof(error)) != 0 ||
        matrix_source_dimension(cli.source_b, &source_b_n,
                                error, sizeof(error)) != 0) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }

    int n = cli.n;
    int file_n = source_a_n != 0 ? source_a_n : source_b_n;
    if (source_a_n != 0 && source_b_n != 0 && source_a_n != source_b_n) {
        fprintf(stderr, "matrix input files must have the same dimension\n");
        return 1;
    }
    if (file_n != 0) {
        if (cli.size_specified && n != file_n) {
            fprintf(stderr, "--size does not match the matrix file dimension\n");
            return 1;
        }
        n = file_n;
    } else if (!cli.size_specified) {
        fprintf(stderr, "--size is required when both inputs are random\n");
        return 1;
    }

    size_t bytes = (size_t)n * (size_t)n * sizeof(double);
    double *a = malloc(bytes);
    double *b = malloc(bytes);
    double *c = malloc(bytes);
    double *reference = malloc(bytes);
    if (a == NULL || b == NULL || c == NULL || reference == NULL) {
        fprintf(stderr, "failed to allocate matrices\n");
        free(a);
        free(b);
        free(c);
        free(reference);
        return 1;
    }

    matrix a_matrix = {.n = n, .data = a};
    matrix b_matrix = {.n = n, .data = b};
    matrix c_matrix = {.n = n, .data = c};
    matrix reference_matrix = {.n = n, .data = reference};
    int random_a = strcmp(cli.source_a, "random") == 0;
    int random_b = strcmp(cli.source_b, "random") == 0;
    char seed[SEED_LENGTH + 1];
    if (random_a || random_b) {
        if (cli.seed_specified) {
            memcpy(seed, cli.seed, sizeof(seed));
        } else {
            default_seed(seed);
        }
    }
    int input_failed = 0;
    if (random_a) {
        matrix_fill_random(&a_matrix, seed, 'A');
    } else {
        input_failed = matrix_read_file(&a_matrix, cli.source_a,
                                        error, sizeof(error));
    }
    if (!input_failed) {
        if (random_b) {
            matrix_fill_random(&b_matrix, seed, 'B');
        } else {
            input_failed = matrix_read_file(&b_matrix, cli.source_b,
                                            error, sizeof(error));
        }
    }
    if (input_failed) {
        fprintf(stderr, "%s\n", error);
        free(a);
        free(b);
        free(c);
        free(reference);
        return 1;
    }
    if (random_a || random_b) {
        printf("seed=%s\n", seed);
    }
    kernel_options options = {.threads = cli.threads};
    matmul_task task = {
        .a = &a_matrix,
        .b = &b_matrix,
        .c = &c_matrix
    };
    matmul_task reference_task = {
        .a = &a_matrix,
        .b = &b_matrix,
        .c = &reference_matrix
    };
    kernel_data reference_data = {.task = &reference_task, .workspace = NULL};
    const kernel_module *naive = kernel_find("naive");
    if (naive == NULL) {
        fprintf(stderr, "naive reference module is missing\n");
        free(a);
        free(b);
        free(c);
        free(reference);
        return 1;
    }
    naive->run(&reference_data, &options);

    printf("N=%d runs=%d threads=%s\n", n, cli.runs,
           cli.threads > 0 ? "requested" : "default");

    kernel_data data = {.task = &task, .workspace = NULL};
    int failed = 0;
    if (cli.run_all) {
        for (int i = 0; i < kernel_count(); ++i) {
            const kernel_module *module = kernel_at(i);
            if (module != NULL && strcmp(module->name, "stub") != 0) {
                failed += run_module(module, &data, &reference_matrix,
                                     cli.runs, &options);
            }
        }
    } else {
        for (int i = 0; i < cli.selected_count; ++i) {
            failed += run_module(cli.selected[i], &data, &reference_matrix,
                                 cli.runs, &options);
        }
    }

    free(a);
    free(b);
    free(c);
    free(reference);
    return failed == 0 ? 0 : 1;
}
