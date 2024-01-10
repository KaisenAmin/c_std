#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdbool.h>

bool int_equal(const void* a, const void* b) {
    return *(const int*)a == *(const int*)b;
}

int main() {
    int myints[] = {10, 20, 30, 30, 20, 10, 10, 20};
    size_t n = sizeof(myints) / sizeof(myints[0]);
    int val = 30;

    const int* result = (const int*)algorithm_search_n(myints, myints + n, sizeof(int), 2, &val, int_equal);

    if (result != myints + n) {
        printf("Two 30s found at position %lld\n", result - myints);
    } 
    else {
        printf("Match not found\n");
    }
    
    val = 10;
    const int* new_result = (const int*)algorithm_search_n(myints, myints + n, sizeof(int), 2, &val, int_equal);

    if (new_result != myints + n) {
        printf("Two 10 found at %lld", new_result - myints);
    }
    else {
        printf("Match not found");
    }

    return 0;
}