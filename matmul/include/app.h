#ifndef APP_H
#define APP_H

#include "kernel.h"
#include "input.h"
#include "matrix.h"
#include "registry.h"

typedef struct {
    const matrix *a;
    const matrix *b;
    matrix *c;
} matmul_task;

void matmul_register_algorithms(void);
void matmul_register_naive(void);
void matmul_register_openmp(void);
void matmul_register_stub(void);

#endif
