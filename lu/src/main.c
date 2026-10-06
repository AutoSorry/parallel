#include "lu.h"
#include "lu_validate.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *program) {
    printf("Usage: %s [--list|-l] [--module|-m NAME] "
           "[--runs|-r N] [--size|-n N] [--threads|-t N]\n", program);
}

static int run_module(const kernel_module *module,
                      kernel_data *data,
                      const matrix *original,
                      int runs,
                      const kernel_options *options) {
    lu_task *task = data->task;
    double total_ms = 0.0;

    printf("\nmodule: %s\n", module->name);
    for (int run = 0; run < runs; ++run) {
        matrix_zero(task->packed_lu);
        timer measurement;
        timer_start(&measurement);
        module->run(data, options);
        timer_stop(&measurement);

        double elapsed_ms = timer_elapsed_ms(&measurement);
        total_ms += elapsed_ms;
        printf("  run %d: %.3f ms\n", run + 1, elapsed_ms);
    }

    printf("  avg: %.3f ms\n", total_ms / runs);
    printf("  validation: ");
    int passed = lu_validate_result(original, task->packed_lu);
    printf("%s\n", passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}

int main(int argc, char **argv) {
    int n = 0;
    int runs = 5;
    int threads = 0;
    const kernel_module *selected = NULL;

    lu_register_algorithms();

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0) {
            kernel_list_all();
            return 0;
        } else if (strcmp(argv[i], "--module") == 0 || strcmp(argv[i], "-m") == 0) {
            if (++i >= argc) {
                usage(argv[0]);
                return 1;
            }
            selected = kernel_find(argv[i]);
            if (selected == NULL) {
                fprintf(stderr, "unknown module: %s\n", argv[i]);
                kernel_list_all();
                return 1;
            }
        } else if (strcmp(argv[i], "--runs") == 0 || strcmp(argv[i], "-r") == 0) {
            if (++i >= argc) {
                usage(argv[0]);
                return 1;
            }
            runs = atoi(argv[i]);
            if (runs <= 0) {
                fprintf(stderr, "runs must be positive\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--size") == 0 || strcmp(argv[i], "-n") == 0) {
            if (++i >= argc) {
                usage(argv[0]);
                return 1;
            }
            n = atoi(argv[i]);
            if (n <= 0) {
                fprintf(stderr, "size must be positive\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0) {
            if (++i >= argc) {
                usage(argv[0]);
                return 1;
            }
            threads = atoi(argv[i]);
            if (threads <= 0) {
                fprintf(stderr, "threads must be positive\n");
                return 1;
            }
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    if (n <= 0) {
        fprintf(stderr, "--size is required\n");
        return 1;
    }

    size_t bytes = (size_t)n * (size_t)n * sizeof(double);
    double *original_data = malloc(bytes);
    double *packed_data = malloc(bytes);
    if (original_data == NULL || packed_data == NULL) {
        fprintf(stderr, "failed to allocate matrices\n");
        free(original_data);
        free(packed_data);
        return 1;
    }

    matrix original = {.n = n, .data = original_data};
    matrix packed_lu = {.n = n, .data = packed_data};
    lu_matrix_init(&original);
    lu_task task = {.a = &original, .packed_lu = &packed_lu};
    kernel_data data = {.task = &task, .workspace = NULL};
    kernel_options options = {.threads = threads};

    printf("N=%d runs=%d threads=%s pivoting=none\n", n, runs,
           threads > 0 ? "requested" : "default");

    int failed = 0;
    if (selected != NULL) {
        failed = run_module(selected, &data, &original, runs, &options);
    } else {
        for (int i = 0; i < kernel_count(); ++i) {
            failed += run_module(kernel_at(i), &data, &original,
                                 runs, &options);
        }
    }

    free(original_data);
    free(packed_data);
    return failed == 0 ? 0 : 1;
}
