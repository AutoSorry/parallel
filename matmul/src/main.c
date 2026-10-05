#include "app.h"
#include "cli.h"
#include "timer.h"
#include "validate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int run_module(const kernel_module *module,
                      kernel_data *data,
                      const matrix *reference,
                      int runs,
    const kernel_options *options) {
    printf("\nmodule: %s\n", module->name);
    matmul_task *task = data->task;
    timer measurement;
    double total_ms = 0.0;

    for (int run = 0; run < runs; ++run) {
        matrix_zero(task->c);
        timer_start(&measurement);
        module->run(data, options);
        timer_stop(&measurement);

        double elapsed_ms = timer_elapsed_ms(&measurement);
        total_ms += elapsed_ms;
        printf("  run %d: %.3f ms\n", run + 1, elapsed_ms);
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
    int n;
    if (input_resolve_size(cli.source_a, cli.source_b,
                           cli.n, cli.size_specified,
                           &n, error, sizeof(error)) != 0) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }

    size_t bytes = (size_t)n * (size_t)n * sizeof(double);
    double *a = NULL;
    double *b = NULL;
    double *c = NULL;
    double *reference = NULL;
    int status = 1;

    a = malloc(bytes);
    b = malloc(bytes);
    c = malloc(bytes);
    reference = malloc(bytes);
    if (a == NULL || b == NULL || c == NULL || reference == NULL) {
        fprintf(stderr, "failed to allocate matrices\n");
        goto cleanup;
    }

    matrix a_matrix = {.n = n, .data = a};
    matrix b_matrix = {.n = n, .data = b};
    matrix c_matrix = {.n = n, .data = c};
    matrix reference_matrix = {.n = n, .data = reference};
    int random_a = strcmp(cli.source_a, "random") == 0;
    int random_b = strcmp(cli.source_b, "random") == 0;
    char seed[MATRIX_SEED_LENGTH + 1];
    if (random_a || random_b) {
        if (cli.seed_specified) {
            memcpy(seed, cli.seed, sizeof(seed));
        } else {
            input_generate_seed(seed);
        }
    }
    if (input_fill_matrix(&a_matrix, cli.source_a, seed, 'A',
                          error, sizeof(error)) != 0 ||
        input_fill_matrix(&b_matrix, cli.source_b, seed, 'B',
                          error, sizeof(error)) != 0) {
        fprintf(stderr, "%s\n", error);
        goto cleanup;
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
    naive->run(&reference_data, &options);

    printf("N=%d runs=%d threads=%s\n", n, cli.runs,
           cli.threads > 0 ? "requested" : "default");

    kernel_data data = {.task = &task, .workspace = NULL};
    int failed = 0;
    if (cli.run_all) {
        for (int i = 0; i < kernel_count(); ++i) {
            const kernel_module *module = kernel_at(i);
            if (strcmp(module->name, "stub") != 0) {
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

    status = failed == 0 ? 0 : 1;

cleanup:
    free(a);
    free(b);
    free(c);
    free(reference);
    return status;
}
