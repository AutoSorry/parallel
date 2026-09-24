#ifndef KERNEL_H
#define KERNEL_H

typedef struct {
    const double *a;
    const double *b;
    double *c;
    int n;
    void *extra;
} kernel_data;

typedef struct {
    int runs;
    int threads;
} kernel_options;

typedef void (*kernel_fn)(kernel_data *data, const kernel_options *options);

typedef struct {
    const char *name;
    const char *description;
    kernel_fn run;
} kernel_module;

#endif
