#include "random.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

static unsigned int rand_state;

void random_seed(unsigned int seed) {
    rand_state = seed;
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

void* random_choice(void* array, size_t n, size_t size) {
    if (array == NULL) {
        fprintf(stderr, "Error: array is null in random_choice.\n");
        return NULL;
    }
    else if (n == 0) {
        fprintf(stderr, "Error: size of array is zero.\n");
        return NULL;
    }

    size_t index = random_randint(0, n - 1);
    return (char*)array + index * size;
}

double random_triangular(double low, double high, double mode) {
    if (low > high) {
        double temp = low;
        low = high;
        high = temp;
    }
    if (mode < low || mode > high) {
        mode = (low + high) / 2;
    }

    double u = random_random();
    double c = (mode - low) / (high - low);

    if (u <= c) {
        return low + sqrt(u * (high - low) * (mode - low));
    } 
    else {
        return high - sqrt((1 - u) * (high - low) * (high - mode));
    }
}

void random_choices(void *array, size_t n, size_t size, size_t num_choices, void *choices, double *weights) {
    if (array == NULL) {
        fprintf(stderr, "Error: array is null in random_choices.\n");
        return;
    }
    if (n == 0) {
        fprintf(stderr, "Error: size of array is zero.\n");
        return;
    }
    if (num_choices == 0) {
        fprintf(stderr, "Error: number of choices is zero.\n");
        return;
    }
    if (weights == NULL) {
        fprintf(stderr, "Error: weights array is null in random_choices.\n");
        return;
    }

    double *cumulative_weights = (double *)malloc(n * sizeof(double));
    if (cumulative_weights == NULL) {
        fprintf(stderr, "Error: Allocation failed in random_choices.\n");
        return;
    }

    cumulative_weights[0] = weights[0];
    for (size_t i = 1; i < n; i++) {
        cumulative_weights[i] = cumulative_weights[i - 1] + weights[i];
    }

    for (size_t i = 0; i < num_choices; i++) {
        double r = random_uniform(0.0, cumulative_weights[n - 1]);
        size_t index = 0;
        while (index < n - 1 && r > cumulative_weights[index]) {
            index++;
        }
        memcpy((char *)choices + i * size, (char *)array + index * size, size);
    }

    free(cumulative_weights);
}

void random_sample(void *array, size_t n, size_t size, size_t num_samples, void *samples) {
    if (array == NULL) {
        fprintf(stderr, "Error: array is null in random_sample.\n");
        return;
    }
    if (n == 0) {
        fprintf(stderr, "Error: size of array is zero.\n");
        return;
    }
    if (num_samples == 0) {
        fprintf(stderr, "Error: number of samples is zero.\n");
        return;
    }
    if (num_samples > n) {
        fprintf(stderr, "Error: number of samples is greater than the number of elements in the array.\n");
        return;
    }

    size_t *indices = (size_t *)malloc(n * sizeof(size_t));
    if (indices == NULL) {
        fprintf(stderr, "Error: Allocation failed in random_sample.\n");
        return;
    }

    for (size_t i = 0; i < n; i++) {
        indices[i] = i;
    }

    for (size_t i = 0; i < num_samples; i++) {
        size_t j = i + random_randint(0, n - i - 1);
        size_t temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (size_t i = 0; i < num_samples; i++) {
        memcpy((char *)samples + i * size, (char *)array + indices[i] * size, size);
    }

    free(indices);
}

void random_setstate(const unsigned int *state) {
    if (state != NULL) {
        rand_state = *state;
        srand(rand_state);
    }
}

void random_getstate(unsigned int *state) {
    if (state != NULL) {
        *state = rand_state;
    }
}