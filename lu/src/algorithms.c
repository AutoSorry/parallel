#include "lu.h"

void lu_register_serial_lu(void);

void lu_register_algorithms(void) {
    lu_register_serial_lu();
}
