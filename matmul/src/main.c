#include "matmul.h"
#include "matrix.h"
#include "timer.h"
#include "validate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SELECTED 64

static void usage(const char *prog) {
    printf("Usage: %s [--list|-l] [--all|-a] [--module|-m NAME] [--runs|-r N] [--size|-n N] [--threads|-t N]\n", prog);
}

static int run_module(const matmul_module *module,
                      const double *A,
                      const double *B,
                      double *C,
                      const double *ref,
                      int runs,
                      int n,
                      const matmul_options *options) {
    double total_ms = 0.0;

    printf("\nmodule: %s\n", module->name);
    for (int r = 0; r < runs; ++r) {
        double t0 = now_seconds();
        module->fn(A, B, C, n, options);
        double t1 = now_seconds();
        double ms = (t1 - t0) * 1000.0;
        total_ms += ms;
        printf("  run %d: %.3f ms\n", r + 1, ms);
    }

    printf("  avg: %.3f ms\n", total_ms / runs);
    printf("  sum: %.6f\n", matrix_sum(C, n));
    printf("  validation: ");
    int ok = validate_result(C, ref, n);
    printf("%s\n", ok ? "PASS" : "FAIL");

    return ok ? 0 : 1;
}

int main(int argc, char **argv) {
    int n = DEFAULT_MATRIX_N;
    int runs = 5;
    int threads = 0;
    int run_all = 1;
    const matmul_module *selected[MAX_SELECTED];
    int selected_count = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0) {
            matmul_list_all();
            return 0;
        } else if (strcmp(argv[i], "--all") == 0 || strcmp(argv[i], "-a") == 0) {
            run_all = 1;
        } else if (strcmp(argv[i], "--module") == 0 || strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 1;
            }
            const matmul_module *mod = matmul_get(argv[++i]);
            if (mod == NULL) {
                fprintf(stderr, "unknown module: %s\n", argv[i]);
                matmul_list_all();
                return 1;
            }
            if (selected_count >= MAX_SELECTED) {
                fprintf(stderr, "too many selected modules\n");
                return 1;
            }
            selected[selected_count++] = mod;
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
    double *A = (double *)malloc(bytes);
    double *B = (double *)malloc(bytes);
    double *C = (double *)malloc(bytes);
    double *ref = (double *)malloc(bytes);

    if (A == NULL || B == NULL || C == NULL || ref == NULL) {
        fprintf(stderr, "failed to allocate matrices\n");
        free(A);
        free(B);
        free(C);
        free(ref);
        return 1;
    }

    matrix_init(A, B, C, n);

    const matmul_module *naive = matmul_get("naive");
    if (naive == NULL) {
        fprintf(stderr, "naive reference module is missing\n");
        free(A);
        free(B);
        free(C);
        free(ref);
        return 1;
    }
    matmul_options options = {
        .threads = threads,
    };

    naive->fn(A, B, ref, n, &options);

    if (threads > 0) {
        printf("N=%d runs=%d threads=%d\n", n, runs, threads);
    } else {
        printf("N=%d runs=%d threads=default\n", n, runs);
    }

    int failed = 0;
    if (run_all) {
        for (int i = 0; i < matmul_count(); ++i) {
            const matmul_module *mod = matmul_at(i);
            if (mod != NULL) {
                failed += run_module(mod, A, B, C, ref, runs, n, &options);
            }
        }
    } else {
        for (int i = 0; i < selected_count; ++i) {
            failed += run_module(selected[i], A, B, C, ref, runs, n, &options);
        }
    }

    free(A);
    free(B);
    free(C);
    free(ref);

    return failed == 0 ? 0 : 1;
}
