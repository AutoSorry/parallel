#include <stdio.h>
#include <time.h>
#include <omp.h>

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static double f(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "n must be a positive integer\n");
        return 1;
    }

    double sum = 0.0;
    double step = 1.0 / n;
    double start = now_seconds();

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int i_start = thread_id * n / num_threads;
        int i_end = (thread_id + 1) * n / num_threads;
        double local_sum = 0.0;

        for (int i = i_start; i < i_end; ++i) {
            double x = (i + 0.5) * step;
            local_sum += f(x);
        }

        #pragma omp critical
        {
            sum += local_sum;
        }
    }

    double end = now_seconds();
    double pi = sum * step;

    printf("%.10f\n", pi);
    printf("time: %.6f seconds\n", end - start);

    return 0;
}
