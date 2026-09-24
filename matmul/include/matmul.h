#ifndef MATMUL_H
#define MATMUL_H

#ifndef DEFAULT_MATRIX_N
#define DEFAULT_MATRIX_N 1024
#endif

typedef struct {
    int threads;
} matmul_options;

typedef void (*matmul_fn)(const double *, const double *, double *, int, const matmul_options *);

typedef struct {
    const char *name;
    const char *description;
    matmul_fn fn;
} matmul_module;

int matmul_register(const matmul_module *module);
const matmul_module *matmul_get(const char *name);
int matmul_count(void);
const matmul_module *matmul_at(int index);
void matmul_list_all(void);

#define REGISTER_ALGORITHM(name, desc, fn) \
    static void matmul_register_##fn(void) __attribute__((constructor)); \
    static void matmul_register_##fn(void) { \
        static const matmul_module mod = {name, desc, fn}; \
        matmul_register(&mod); \
    }

#endif // MATMUL_H
