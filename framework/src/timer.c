#include "timer.h"

void timer_start(timer *value) {
    clock_gettime(CLOCK_MONOTONIC, &value->start_time);
}

void timer_stop(timer *value) {
    clock_gettime(CLOCK_MONOTONIC, &value->stop_time);
}

double timer_elapsed_seconds(const timer *value) {
    double seconds = (double)(value->stop_time.tv_sec - value->start_time.tv_sec);
    double nanoseconds =
        (double)(value->stop_time.tv_nsec - value->start_time.tv_nsec) * 1e-9;
    return seconds + nanoseconds;
}

double timer_elapsed_ms(const timer *value) {
    return timer_elapsed_seconds(value) * 1000.0;
}
