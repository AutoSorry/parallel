#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct {
    struct timespec start_time;
    struct timespec stop_time;
} timer;

void timer_start(timer *value);
void timer_stop(timer *value);
double timer_elapsed_seconds(const timer *value);
double timer_elapsed_ms(const timer *value);

#endif
