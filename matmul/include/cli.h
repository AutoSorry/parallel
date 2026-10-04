#ifndef CLI_H
#define CLI_H

#include "kernel.h"

#define MAX_SELECTED 64

typedef struct {
    int show_list;
    int run_all;
    int runs;
    int n;
    int size_specified;
    int threads;
    int seed_specified;
    unsigned int seed;
    const char *source_a;
    const char *source_b;
    const kernel_module *selected[MAX_SELECTED];
    int selected_count;
} cli_options;

void cli_defaults(cli_options *options);
int cli_parse(cli_options *options, int argc, char **argv);
void cli_print_usage(const char *program);

#endif
