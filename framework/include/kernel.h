#ifndef KERNEL_H
#define KERNEL_H

typedef struct {
    /* The application owns and defines the problem object. */
    void *problem;
    /* Optional algorithm-owned temporary storage. */
    void *workspace;
} kernel_data;

typedef struct {
    /* Zero means that the algorithm should use its default. */
    int threads;
} kernel_options;

typedef void (*kernel_fn)(kernel_data *data, const kernel_options *options);

typedef struct {
    const char *name;
    const char *description;
    kernel_fn run;
} kernel_module;

#endif
