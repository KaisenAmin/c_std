#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdbool.h>


int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;

    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    int myints[] = {5, 20, 5, 30, 30, 20, 10, 10, 20}
    size_t num_elements = sizeof(myints) / sizeof(myints[0]);

    int* it = algorithm_adjacent_find(myints, num_elements, sizeof(int), compare_ints);
    if (it != NULL) {
        printf("The first pair of repeated elements are: %d\n", *it);
		it++;
    }

    int* it2 = algorithm_adjacent_find(it, num_elements - (it - myints), sizeof(int), compare_ints);
    if (it2 != NULL) {
        printf("The second pair of repeated elements are: %d\n", *it2);
    }

    return 0;
}

