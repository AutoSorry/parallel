#include <stdio.h>
#include <omp.h>

#ifndef NUM_STEPS
#define NUM_STEPS 1000000000L
#endif

#ifndef NUM_THREADS
#define NUM_THREADS 16
#endif

int main(void) {
    const double step = 1.0 / (double)NUM_STEPS;
    double sum = 0.0;
    omp_set_num_threads(NUM_THREADS);

    double start = omp_get_wtime();

    #pragma omp parallel for reduction(+:sum)
    for (long i = 1; i <= NUM_STEPS; ++i) {
        const double x = (i - 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    const double pi = sum * step;
    double end = omp_get_wtime();
    printf("pi=%.15f time=%.9f seconds\n", pi, end - start);
    return 0;
}
