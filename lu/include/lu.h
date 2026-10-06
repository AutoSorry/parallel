#ifndef LU_H
#define LU_H

#include "kernel.h"
#include "lu_matrix.h"
#include "registry.h"

typedef struct {
    const matrix *a;
    matrix *packed_lu;
} lu_task;

void lu_register_algorithms(void);
void lu_register_serial_lu(void);
void lu_register_openmp_lu(void);

#endif
