#include "matmul.h"
#include "matmul_config.h"
#include "matmul_validate.h"
#include "matmul_matrix.h"
#include "matrix.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SELECTED 64

static void usage(const char *program) {
    printf("Usage: %s [--list|-l] [--all|-a] [--module|-m NAME] "
           "[--runs|-r N] [--size|-n N] [--threads|-t N]\n", program);
}

static int run_module(const kernel_module *module,
                      kernel_data *data,
                      const double *reference,
                      int runs,
                      const kernel_options *options) {
    double total_ms = 0.0;

    printf("\nmodule: %s\n", module->name);
    for (int run = 0; run < runs; ++run) {
        matrix_zero(data->c, data->n);
        double start = now_seconds();
        module->run(data, options);
        double end = now_seconds();
        double milliseconds = (end - start) * 1000.0;
        total_ms += milliseconds;
        printf("  run %d: %.3f ms\n", run + 1, milliseconds);
    }

    printf("  avg: %.3f ms\n", total_ms / runs);
    printf("  sum: %.6f\n", matrix_sum(data->c, data->n));
    printf("  validation: ");
    int passed = matmul_validate_result(data->c, reference, data->n);
    printf("%s\n", passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}

int main(int argc, char **argv) {
    int n = MATRIX_N;
    int runs = 5;
    int threads = 0;
    int run_all = 1;
    const kernel_module *selected[MAX_SELECTED];
    int selected_count = 0;

    matmul_register_algorithms();

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0) {
            kernel_list_all();
            return 0;
        } else if (strcmp(argv[i], "--all") == 0 || strcmp(argv[i], "-a") == 0) {
            run_all = 1;
        } else if (strcmp(argv[i], "--module") == 0 || strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 1;
            }
            const kernel_module *module = kernel_find(argv[++i]);
            if (module == NULL) {
                fprintf(stderr, "unknown module: %s\n", argv[i]);
                kernel_list_all();
                return 1;
            }
            if (selected_count >= MAX_SELECTED) {
                fprintf(stderr, "too many selected modules\n");
                return 1;
            }
            selected[selected_count++] = module;
            run_all = 0;
        } else if (strcmp(argv[i], "--runs") == 0 || strcmp(argv[i], "-r") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 1;
            }
            runs = atoi(argv[++i]);
            if (runs <= 0) {
                fprintf(stderr, "runs must be positive\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--size") == 0 || strcmp(argv[i], "-n") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 1;
            }
            n = atoi(argv[++i]);
            if (n <= 0) {
                fprintf(stderr, "size must be positive\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 1;
            }
            threads = atoi(argv[++i]);
            if (threads <= 0) {
                fprintf(stderr, "threads must be positive\n");
                return 1;
            }
        } else {
            usage(argv[0]);
            return 1;
        }
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

    matmul_matrix_init(a, b, c, n);
    kernel_options options = {.runs = runs, .threads = threads};
    kernel_data reference_data = {.a = a, .b = b, .c = reference, .n = n, .extra = NULL};
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

    printf("N=%d runs=%d threads=%s\n", n, runs,
           threads > 0 ? "requested" : "default");

    kernel_data data = {.a = a, .b = b, .c = c, .n = n, .extra = NULL};
    int failed = 0;
    if (run_all) {
        for (int i = 0; i < kernel_count(); ++i) {
            const kernel_module *module = kernel_at(i);
            if (module != NULL && strcmp(module->name, "stub") != 0) {
                failed += run_module(module, &data, reference, runs, &options);
            }
        }
    } else {
        for (int i = 0; i < selected_count; ++i) {
            failed += run_module(selected[i], &data, reference, runs, &options);
        }
    }

    free(a);
    free(b);
    free(c);
    free(reference);
    return failed == 0 ? 0 : 1;
}
