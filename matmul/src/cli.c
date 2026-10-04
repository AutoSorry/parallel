#include "cli.h"

#include "registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cli_defaults(cli_options *options) {
    options->show_list = 0;
    options->run_all = 1;
    options->runs = 5;
    options->n = 0;
    options->size_specified = 0;
    options->threads = 0;
    options->seed_specified = 0;
    options->seed[0] = '\0';
    options->source_a = "random";
    options->source_b = "random";
    options->selected_count = 0;
}

void cli_print_usage(const char *program) {
    printf("Usage: %s [--list|-l] [--all|-a] [--module|-m NAME] "
           "[--runs|-r N] [--size|-n N] [--threads|-t N] "
           "[--input-a|-A SOURCE] [--input-b|-B SOURCE] [--seed SEED]\n", program);
    printf("SOURCE is 'random' or a matrix file whose first value is N, "
           "followed by N*N values.\n");
}

static int require_value(const char *program, int index, int argc) {
    if (index + 1 < argc) {
        return 0;
    }
    cli_print_usage(program);
    return -1;
}

int cli_parse(cli_options *options, int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0) {
            options->show_list = 1;
        } else if (strcmp(argv[i], "--all") == 0 || strcmp(argv[i], "-a") == 0) {
            options->run_all = 1;
        } else if (strcmp(argv[i], "--module") == 0 || strcmp(argv[i], "-m") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            const kernel_module *module = kernel_find(argv[++i]);
            if (module == NULL) {
                fprintf(stderr, "unknown module: %s\n", argv[i]);
                kernel_list_all();
                return -1;
            }
            if (options->selected_count >= MAX_SELECTED) {
                fprintf(stderr, "too many selected modules\n");
                return -1;
            }
            options->selected[options->selected_count++] = module;
            options->run_all = 0;
        } else if (strcmp(argv[i], "--runs") == 0 || strcmp(argv[i], "-r") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            options->runs = atoi(argv[++i]);
            if (options->runs <= 0) {
                fprintf(stderr, "runs must be positive\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--size") == 0 || strcmp(argv[i], "-n") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            options->n = atoi(argv[++i]);
            if (options->n <= 0) {
                fprintf(stderr, "size must be positive\n");
                return -1;
            }
            options->size_specified = 1;
        } else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            options->threads = atoi(argv[++i]);
            if (options->threads <= 0) {
                fprintf(stderr, "threads must be positive\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--input-a") == 0 || strcmp(argv[i], "-A") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            options->source_a = argv[++i];
        } else if (strcmp(argv[i], "--input-b") == 0 || strcmp(argv[i], "-B") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            options->source_b = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0) {
            if (require_value(argv[0], i, argc) != 0) {
                return -1;
            }
            const char *text = argv[++i];
            if (strlen(text) != SEED_LENGTH) {
                fprintf(stderr, "seed must contain exactly 8 uppercase letters\n");
                return -1;
            }
            for (int j = 0; j < SEED_LENGTH; ++j) {
                if (text[j] < 'A' || text[j] > 'Z') {
                    fprintf(stderr, "seed must contain exactly 8 uppercase letters\n");
                    return -1;
                }
            }
            memcpy(options->seed, text, SEED_LENGTH + 1);
            options->seed_specified = 1;
        } else {
            cli_print_usage(argv[0]);
            return -1;
        }
    }

    return 0;
}
