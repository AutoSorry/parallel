#ifndef KERNEL_MATRIX_H
#define KERNEL_MATRIX_H

void matrix_zero(double *matrix, int n);
void matrix_copy(double *destination, const double *source, int n);
double matrix_sum(const double *matrix, int n);

#endif
