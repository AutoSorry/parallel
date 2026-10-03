#include "app.h"

void matmul_register_naive(void);
void matmul_register_openmp(void);
void matmul_register_stub(void);

void matmul_register_algorithms(void) {
    matmul_register_naive();
    matmul_register_openmp();
    matmul_register_stub();
}
