#ifndef LU_H
#define LU_H

#include "kernel.h"
#include "lu_matrix.h"
#include "registry.h"

typedef struct {
    matrix *a;
    int chunk_size;
} lu_task;

void lu_register_algorithms(void);
void lu_register_serial_lu_kij(void);
void lu_register_serial_lu_kji(void);
void lu_register_openmp_lu_kij(void);
void lu_register_openmp_lu_kij_simd(void);
void lu_register_openmp_lu_kji(void);
void lu_register_openmp_lu_kji_simd(void);
void lu_register_openmp_lu_dynamic(void);

#endif
