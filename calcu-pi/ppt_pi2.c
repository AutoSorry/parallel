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
    double sum[NUM_THREADS];
    double pi = 0.0;
    omp_set_num_threads(NUM_THREADS);

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        const int id = omp_get_thread_num();
        double x;
        sum[id] = 0.0;

        #pragma omp for
        for (long i = 0; i < NUM_STEPS; ++i) {
            x = (i + 0.5) * step;
            sum[id] += 4.0 / (1.0 + x * x);
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pi += sum[i] * step;
    }

    double end = omp_get_wtime();
    printf("pi=%.15f time=%.9f seconds\n", pi, end - start);
    return 0;
}
