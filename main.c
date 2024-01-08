#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>
#include <time.h>

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
	Vector* vec = vector_create(sizeof(int));

	for (size_t index = 1; index <= 10; index++) {
		vector_push_back(vec, &index);
	}

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
    do {

    } while (algorithm_next_permutation(vector_begin(vec), vector_end(vec), sizeof(int), less_int));
	clock_gettime(CLOCK_MONOTONIC, &end);

	double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("%lf", elapsed);

    return 0;
}