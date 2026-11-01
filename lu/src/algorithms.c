#include "lu.h"

void lu_register_algorithms(void) {
    lu_register_serial_lu_kij();
    lu_register_serial_lu_kji();
    lu_register_openmp_lu_kij();
    lu_register_openmp_lu_kij_simd();
    lu_register_openmp_lu_kji();
    lu_register_openmp_lu_kji_simd();
    lu_register_openmp_lu_dynamic();
}
