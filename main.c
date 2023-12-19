#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ELEMENTS 100000000

int main() 
{
    struct timespec start, end;
    double time_sum = 0;


    Vector* vec = vector_create(sizeof(int));
    if (!vec) 
    {
        perror("Vector creation failed");
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
        
    for (int i = 0; i < NUM_ELEMENTS; i++) 
        vec->push_back(vec, &i);
        
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sum += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    double average_time = time_sum / NUM_ITERATIONS;
    printf("Average Custom Vector Time: %f seconds\n", average_time);

    vec->deallocate(vec);

    return EXIT_SUCCESS;
}
