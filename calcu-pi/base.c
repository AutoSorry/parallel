#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

int main() {
    int n;
    scanf("%d", &n);

    double sum = 0.0;
    int num_threads = omp_get_max_threads();
    double *local_sum = calloc(num_threads, sizeof(double));
    if (local_sum == NULL) {
        fprintf(stderr, "failed to allocate local_sum\n");
        return 1;
    }

    double start = now_seconds();

    #pragma omp parallel 
    {
        int thread_id = omp_get_thread_num();
        int actual_threads = omp_get_num_threads();
        int i_start = thread_id * n / actual_threads;
        int i_end = (thread_id + 1) * n / actual_threads;
        double i_sum = 0.0;

        for (int i = i_start; i < i_end; ++i) {
            double x = (i + 0.5) / n;
            i_sum += f(x);
        }
        local_sum[thread_id] = i_sum;
    }
    
    for (int i = 0; i < num_threads; ++i) {
        sum += local_sum[i];
    }

    double end = now_seconds();

    double pi = sum / n;
    printf("%.10f\n", pi);
    printf("time: %.6f seconds\n", end - start);

    free(local_sum);
    return 0;
}
