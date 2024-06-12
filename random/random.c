#include "random.h"

#include <stdlib.h>
#include <string.h>


void random_seed(unsigned int seed) {
    srand(seed);
}

int random_randint(int a, int b) {
    if (a > b) {
        int temp = a;
        a = b;
        b = temp;
    }
    
    return a + rand() % (b - a + 1);
}

int random_randrange(int a, int b, int step) {
    if (a >= b || step <= 0) {
        return -1; 
    }

    int range = (b - a + step - 1) / step;
    return a + (rand() % range) * step;
}

double random_random() {
    return rand() / (double)RAND_MAX;
}

double random_uniform(double a, double b) {
    if (a > b) {
        double temp = a;
        a = b;
        b = temp;
    }

    return a + (rand() / (double)RAND_MAX) * (b - a);
}

int random_getrandbits(int a) {
    if (a <= 0 || a > (int)(sizeof(int) * 8)) {
        return -1; // invalid input
    }

    int result = 0;
    for (int i = 0; i < a; i++) {
        result = (result << 1) | (rand() & 1);
    }

    return result;
}

void random_shuffle(void *array, size_t n, size_t size) {
    char* arr = (char*)array;
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + random_randint(0, n - i - 1);

        void* temp = malloc(size);
        if (temp == NULL) {
            fprintf(stderr, "Error: Allocation failed in random_shuffle.\n");
            return; 
        }

        memcpy(temp, arr + j * size, size);
        memcpy(arr + j * size, arr + i * size, size);
        memcpy(arr + i * size, temp, size);
        free(temp);
    }
}