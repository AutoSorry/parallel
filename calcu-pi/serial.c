#include <stdio.h>
#include <time.h>

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

    double start = now_seconds();

    for (int i = 0; i < n; ++i) {
        double x = (i + 0.5) / n;
        sum += f(x);
    }

    double end = now_seconds();

    double pi = sum / n;
    printf("%.10f\n", pi);
    printf("time: %.6f seconds\n", end - start);

    return 0;
}
