#include "lu.h"

void lu_register_algorithms(void) {
    lu_register_serial_lu();
    lu_register_openmp_lu();
}
