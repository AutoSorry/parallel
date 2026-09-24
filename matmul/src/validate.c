#include "validate.h"

#include <math.h>
#include <stdio.h>

int validate_result(const double *C, const double *ref, int n) {
    double max_abs = 0.0;
    double max_ref = 0.0;

    for (int i = 0; i < n * n; ++i) {
        double err = fabs(C[i] - ref[i]);
        double mag = fabs(ref[i]);
        if (err > max_abs) {
            max_abs = err;
        }
        if (mag > max_ref) {
            max_ref = mag;
        }
    }

    double rel = max_abs / (max_ref > 0.0 ? max_ref : 1.0);
    printf("max_abs_error=%.6e max_rel_error=%.6e ", max_abs, rel);
    return rel < 1e-9;
}
