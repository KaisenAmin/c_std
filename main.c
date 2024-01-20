#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main() {
    struct timespec start, end;
    double time_sum = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 10000; i++) {
        // Average Custom fmt_printf Time: 0.000327 seconds
        // fmt_printf("%d : %s\n", i, "امین طهماسبی"); 

        // Average Custom printf Time: 0.001184 seconds
        printf("%d : %s\n", i, "امین طهماسبی");
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sum += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    double average_time = time_sum / 10000;
    printf("Average Custom fmt_printf Time: %f seconds\n", average_time);

    return 0;
}