#include "algorithm.h"
#include <string.h>
#include <stdlib.h>

// Simple insertion sort implementation for the algorithm_sort function
void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    char* temp = (char*) malloc(sizeof(char) * size);

    if (temp) {
        for (size_t i = 1; i < num; i++) {
            memcpy(temp, (char*)base + i * size, size);
            size_t j = i;
            while (j > 0 && comp((char*)base + (j - 1) * size, temp) > 0) {
                memcpy((char*)base + j * size, (char*)base + (j - 1) * size, size);
                j--;
            }
            memcpy((char*)base + j * size, temp, size);
        }
    }
    else {
        perror("Can not allocate memory for sorting");
       exit(-1);
    }
}
