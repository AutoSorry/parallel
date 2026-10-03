#include "validate.h"

#include <math.h>
#include <stdio.h>

int matmul_validate_result(const double *actual, const double *reference, int n) {
    double max_abs = 0.0;
    double max_reference = 0.0;

    for (int i = 0; i < n * n; ++i) {
        double absolute_error = fabs(actual[i] - reference[i]);
        double reference_magnitude = fabs(reference[i]);
        if (absolute_error > max_abs) {
            max_abs = absolute_error;
        }
        if (reference_magnitude > max_reference) {
            max_reference = reference_magnitude;
        }
    }

    double relative_error = max_abs / (max_reference > 0.0 ? max_reference : 1.0);
    printf("max_abs_error=%.6e max_rel_error=%.6e ", max_abs, relative_error);
    return relative_error < 1e-9;
}
