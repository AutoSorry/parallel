#include "lu_validate.h"

#include <math.h>
#include <stdio.h>

int lu_validate_result(const double *original, const double *packed_lu, int n) {
    double max_abs = 0.0;
    double max_original = 0.0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double reconstructed = 0.0;
            for (int k = 0; k < n; ++k) {
                double l = i == k ? 1.0 : (i > k ? packed_lu[i * n + k] : 0.0);
                double u = k <= j ? packed_lu[k * n + j] : 0.0;
                reconstructed += l * u;
            }

            int index = i * n + j;
            double absolute_error = fabs(reconstructed - original[index]);
            double original_magnitude = fabs(original[index]);
            if (absolute_error > max_abs) {
                max_abs = absolute_error;
            }
            if (original_magnitude > max_original) {
                max_original = original_magnitude;
            }
        }
    }

    double relative_error = max_abs / (max_original > 0.0 ? max_original : 1.0);
    printf("max_abs_error=%.6e max_rel_error=%.6e ", max_abs, relative_error);
    return relative_error < 1e-9;
}
