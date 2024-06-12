#include "random.h"

#include <stdlib.h>


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