#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


bool int_equal(const void* a, const void* b) {
    return *(const int*)a == *(const int*)b;
}

int main() {
    int haystack[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    int needle[] = {40, 50, 60, 70};

    const int* result = (const int* )algorithm_search(haystack, haystack + 9, sizeof(int), needle, needle + 4, sizeof(int), int_equal);

    if (result != haystack + 9) {
        printf("Subsequence found at position %lld\n", result - haystack);
    } 
    else {
        printf("Subsequence not found\n");
    }

    return 0;
}