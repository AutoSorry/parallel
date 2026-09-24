#include "lu.h"
#include "lu_config.h"
#include "lu_matrix.h"
#include "lu_validate.h"
#include "matrix.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SELECTED 64

static void usage(const char *program) {
    printf("Usage: %s [--list|-l] [--module|-m NAME] "
           "[--runs|-r N] [--size|-n N]\n", program);
}

static int run_module(const kernel_module *module,
                      kernel_data *data,
                      const double *original,
                      int runs,
                      const kernel_options *options) {
    double total_ms = 0.0;

    printf("\nmodule: %s\n", module->name);
    for (int run = 0; run < runs; ++run) {
        double start = now_seconds();
        module->run(data, options);
        double end = now_seconds();
        double milliseconds = (end - start) * 1000.0;
        total_ms += milliseconds;
        printf("  run %d: %.3f ms\n", run + 1, milliseconds);
    }

    printf("  avg: %.3f ms\n", total_ms / runs);
    printf("  validation: ");
    int passed = lu_validate_result(original, data->c, data->n);
    printf("%s\n", passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}

int main(int argc, char **argv) {
    int n = MATRIX_N;
    int runs = 5;
    const kernel_module *selected[MAX_SELECTED];
    int selected_count = 0;

    lu_register_algorithms();

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0) {
            kernel_list_all();
            return 0;
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
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    size_t bytes = (size_t)n * (size_t)n * sizeof(double);
    double *original = malloc(bytes);
    double *packed_lu = malloc(bytes);
    if (original == NULL || packed_lu == NULL) {
        fprintf(stderr, "failed to allocate matrices\n");
        free(original);
        free(packed_lu);
        return 1;
    }

    lu_matrix_init(original, n);
    kernel_data data = {.a = original, .b = NULL, .c = packed_lu, .n = n, .extra = NULL};
    kernel_options options = {.runs = runs, .threads = 0};
    printf("N=%d runs=%d pivoting=none\n", n, runs);

    int failed = 0;
    if (selected_count == 0) {
        for (int i = 0; i < kernel_count(); ++i) {
            const kernel_module *module = kernel_at(i);
            if (module != NULL) {
                failed += run_module(module, &data, original, runs, &options);
            }
        }
    } else {
        for (int i = 0; i < selected_count; ++i) {
            failed += run_module(selected[i], &data, original, runs, &options);
        }
    }

    free(original);
    free(packed_lu);
    return failed == 0 ? 0 : 1;
}
