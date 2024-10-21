/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Random
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static unsigned int rand_state;

/**
 * @brief This function sets the seed for the random number generator, allowing
 *      for reproducible sequences of random numbers.
 *
 * @param seed The seed value to initialize the random number generator.
 */
void random_seed(unsigned int seed) {
    RANDOM_LOG("[random_seed]: Entering with seed: %u", seed);

    rand_state = seed;
    srand(seed);

    RANDOM_LOG("[random_seed]: Seed set to: %u", seed);
    RANDOM_LOG("[random_seed]: Exiting");
}


/**
 * @brief This function generates a random integer in the range [a, b], inclusive.
 * if `a` is greater than `b`, their values are swapped to ensure the correct range.
 *
 * @param a The lower bound of the range (inclusive).
 * @param b The upper bound of the range (inclusive).
 * 
 * @return A random integer within the range [a, b].
 */
int random_randint(int a, int b) {
    RANDOM_LOG("[random_randint]: Entering with a: %d, b: %d", a, b);

    if (a > b) {
        RANDOM_LOG("[random_randint]: Swapping a and b as a > b.");
        int temp = a;
        a = b;
        b = temp;
    }

    int result = a + rand() % (b - a + 1);
    RANDOM_LOG("[random_randint]: Generated random integer: %d in range [%d, %d]", result, a, b);
    RANDOM_LOG("[random_randint]: Exiting with result: %d", result);

    return result;
}


/**
 * @brief This function generates a random integer in the range `[a, b)` with a specified step.
 *
 * @param a The start of the range (inclusive).
 * @param b The end of the range (exclusive).
 * @param step The step value (can be positive or negative, but not zero).
 * 
 * @return A random integer within the range `[a, b)` with the given step.
 *         Returns -1 if `a >= b` or `step <= 0`.
 */
int random_randrange(int a, int b, int step) {
    RANDOM_LOG("[random_randrange]: Entering with a: %d, b: %d, step: %d", a, b, step);

    if (a == b || step == 0) {
        RANDOM_LOG("[random_randrange]: Error: invalid range (a == b) or step (step == 0).");
        return -1;
    }
    if ((step > 0 && a > b) || (step < 0 && a < b)) {
        RANDOM_LOG("[random_randrange]: Error: logic error with range and step values.");
        return -1;
    }

    int range = abs(b - a);
    int num_steps = (range + abs(step) - 1) / abs(step);
    RANDOM_LOG("[random_randrange]: Calculated num_steps: %d", num_steps);

    int random_step = rand() % num_steps;
    RANDOM_LOG("[random_randrange]: Generated random_step: %d", random_step);

    int result = a + random_step * step;
    RANDOM_LOG("[random_randrange]: Exiting with result: %d", result);

    return result;
}


/**
 * @brief This function generates a random double precision floating-point number
 *      in the range [0, 1) using the standard library `rand()` function.
 *
 * @return A random double in the range [0, 1).
 */
double random_random() {
    RANDOM_LOG("[random_random]: Entering");

    double result = rand() / (double)RAND_MAX;
    RANDOM_LOG("[random_random]: Generated random double: %f in range [0, 1)", result);

    RANDOM_LOG("[random_random]: Exiting with result: %f", result);
    return result;
}


/**
 * @brief This function generates a random double precision floating-point number
 *      in the range [a, b). If `a` is greater than `b`, their values are swapped.
 *
 * @param a The lower bound of the range (inclusive).
 * @param b The upper bound of the range (exclusive).
 * 
 * @return A random double in the range [a, b).
 */
double random_uniform(double a, double b) {
    RANDOM_LOG("[random_uniform]: Entering random_uniform with a: %f, b: %f", a, b);

    if (a > b) {
        RANDOM_LOG("[random_uniform]: Swapping a and b as a > b.");
        double temp = a;
        a = b;
        b = temp;
    }

    double result = a + (rand() / (double)RAND_MAX) * (b - a);
    RANDOM_LOG("[random_uniform]: Exiting random_uniform with result: %f", result);

    return result;
}

/**
 * @brief Generates a random integer with a specified number of random bits.
 *
 * @param a The number of random bits to generate (must be > 0 and <= 32).
 * @return A random integer with `a` random bits. Returns -1 for invalid input.
 */
int random_getrandbits(int a) {
    RANDOM_LOG("[random_getrandbits]: Entering random_getrandbits with a: %d", a);

    if (a <= 0 || a > (int)(sizeof(int) * 8)) {
        RANDOM_LOG("[random_getrandbits]: Error: a is out of valid range (1 to 32).");
        return -1; // invalid input
    }

    int result = 0;
    for (int i = 0; i < a; i++) {
        result = (result << 1) | (rand() & 1);
        RANDOM_LOG("[random_getrandbits]: Generated random bit: %d, current result: %d", (rand() & 1), result);
    }

    RANDOM_LOG("[random_getrandbits]: Exiting random_getrandbits with result: %d", result);
    return result;
}


/**
 * @brief Shuffles an array of any type.

 * @param array Pointer to the array to shuffle. if array is NULL function print error message and return nothing.
 * @param n The number of elements in the array. if n == 0 function print error message and return nothing.
 * @param size The size of each element in the array.
 * 
 * @note This function shuffles an array of any type using the Fisher-Yates algorithm.
 */
void random_shuffle(void *array, size_t n, size_t size) {
    RANDOM_LOG("[random_shuffle]: Entering random_shuffle with array: %p, n: %zu, size: %zu", (void*)array, n, size);

    if (array == NULL) {
        RANDOM_LOG("[random_shuffle]: Error: array is NULL in random_shuffle.");
        return;
    }
    if (n == 0) {
        RANDOM_LOG("[random_shuffle]: Error: size of array is zero in random_shuffle.");
        return;
    }

    char* arr = (char*)array;
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + random_randint(0, n - i - 1);
        RANDOM_LOG("[random_shuffle]: Swapping element %zu with element %zu", i, j);

        void* temp = malloc(size);
        if (temp == NULL) {
            RANDOM_LOG("[random_shuffle]: Error: memory allocation failed in random_shuffle.");
            return;
        }

        memcpy(temp, arr + j * size, size);
        memcpy(arr + j * size, arr + i * size, size);
        memcpy(arr + i * size, temp, size);
        free(temp);

        RANDOM_LOG("[random_shuffle]: Completed swap between elements %zu and %zu", i, j);
    }

    RANDOM_LOG("[random_shuffle]: Exiting random_shuffle");
}


/**
 * @brief Randomly selects an element from an array and returns a pointer to an element from the array.
 *
 * @param array Pointer to the array to select from. if array is NULL function return NULL.
 * @param n The number of elements in the array. if n == 0 function return NULL.
 * @param size The size of each element in the array.
 * 
 * @return Pointer to the randomly selected element. Returns NULL for invalid input.
 */
void* random_choice(void* array, size_t n, size_t size) {
    RANDOM_LOG("[random_choice]: Entering random_choice with array: %p, n: %zu, size: %zu", (void*)array, n, size);

    if (array == NULL) {
        RANDOM_LOG("[random_choice]: Error: array is NULL in random_choice.");
        return NULL;
    }
    if (n == 0) {
        RANDOM_LOG("[random_choice]: Error: size of array is zero in random_choice.");
        return NULL;
    }

    size_t index = random_randint(0, n - 1);
    RANDOM_LOG("[random_choice]: Selected index: %zu", index);

    void* result = (char*)array + index * size;
    RANDOM_LOG("[random_choice]: Exiting random_choice with result: %p", (void*)result);

    return result;
}

/**
 * @brief Generates a random double number based on the Triangular distribution.
 *
 * @param low The lower limit of the distribution.
 * @param high The upper limit of the distribution.
 * @param mode The mode (peak) of the distribution. if `mode` is not within the range [low, high],
 *             it is set to the midpoint of `low` and `high`.
 * @return A random double number from the Triangular distribution.
 *
 * @note If `low` is greater than `high`, their values are swapped.
 * @note The function uses the inverse transform sampling method to produce the triangular-distributed number.
 */
double random_triangular(double low, double high, double mode) {
    RANDOM_LOG("[random_triangular]: Entering random_triangular with low: %f, high: %f, mode: %f", low, high, mode);

    if (low > high) {
        RANDOM_LOG("[random_triangular]: Swapping low and high, as low > high.");
        double temp = low;
        low = high;
        high = temp;
    }

    if (mode < low || mode > high) {
        RANDOM_LOG("[random_triangular]: Mode is out of bounds, setting mode to midpoint of low and high.");
        mode = (low + high) / 2;
    }

    double u = random_random();
    double c = (mode - low) / (high - low);
    RANDOM_LOG("[random_triangular]: Generated random number u: %f, calculated c: %f", u, c);

    double result;
    if (u <= c) {
        result = low + sqrt(u * (high - low) * (mode - low));
    }
    else {
        result = high - sqrt((1 - u) * (high - low) * (high - mode));
    }

    RANDOM_LOG("[random_triangular]: Exiting random_triangular with result: %f", result);
    return result;
}


/**
 * @brief Selects multiple random elements from an array with specified weights.
 * The selected elements are copied into the provided choices array.
 *
 * @param array Pointer to the input array from which to select elements.
 * @param n The number of elements in the input array.
 * @param size The size of each element in the input array.
 * @param num_choices The number of elements to select.
 * @param choices Pointer to the array where the selected elements will be stored.
 * @param weights Pointer to the array of weights corresponding to each element in the input array.
 *
 * @note The function performs error checking to ensure valid inputs. If any error is detected,
 *       an error message is printed and the function returns without performing the selection.
 *
 * @note The function uses cumulative weights to perform weighted random sampling.
 */
void random_choices(void *array, size_t n, size_t size, size_t num_choices, void *choices, double *weights) {
    RANDOM_LOG("[random_choices]: Entering random_choices with array: %p, n: %zu, size: %zu, num_choices: %zu, choices: %p, weights: %p", (void*)array, n, size, num_choices, (void*)choices, (void*)weights);

    if (array == NULL) {
        RANDOM_LOG("[random_choices]: Error: array is NULL in random_choices.");
        return;
    }
    if (n == 0) {
        RANDOM_LOG("[random_choices]: Error: size of array is zero in random_choices.");
        return;
    }
    if (num_choices == 0) {
        RANDOM_LOG("[random_choices]: Error: number of choices is zero in random_choices.");
        return;
    }
    if (weights == NULL) {
        RANDOM_LOG("[random_choices]: Error: weights array is NULL in random_choices.");
        return;
    }

    // Allocate memory for cumulative weights
    double *cumulative_weights = (double *)malloc(n * sizeof(double));
    if (cumulative_weights == NULL) {
        RANDOM_LOG("[random_choices]: Error: memory allocation failed for cumulative_weights in random_choices.");
        return;
    }

    // Calculate cumulative weights
    RANDOM_LOG("[random_choices]: Calculating cumulative weights.");
    cumulative_weights[0] = weights[0];
    RANDOM_LOG("[random_choices]: cumulative_weights[0] = %f", cumulative_weights[0]);
    
    for (size_t i = 1; i < n; i++) {
        cumulative_weights[i] = cumulative_weights[i - 1] + weights[i];
        RANDOM_LOG("[random_choices]: cumulative_weights[%zu] = %f", i, cumulative_weights[i]);
    }

    // Make random choices
    for (size_t i = 0; i < num_choices; i++) {
        double r = random_uniform(0.0, cumulative_weights[n - 1]);
        RANDOM_LOG("[random_choices]: Generated random number r = %f for choice %zu", r, i);

        size_t index = 0;
        while (index < n - 1 && r > cumulative_weights[index]) {
            index++;
        }
        RANDOM_LOG("[random_choices]: Selected index %zu for choice %zu", index, i);

        memcpy((char *)choices + i * size, (char *)array + index * size, size);
        RANDOM_LOG("[random_choices]: Copied choice %zu from array index %zu", i, index);
    }

    free(cumulative_weights);

    RANDOM_LOG("[random_choices]: Freed cumulative_weights memory.");
    RANDOM_LOG("[random_choices]: Exiting random_choices.");
}

/**
 * @brief Samples a specified number of unique random elements from an array.
 * The sampled elements are copied into the provided samples array.
 *
 * @param array Pointer to the input array from which to sample elements.
 * @param n The number of elements in the input array.
 * @param size The size of each element in the input array.
 * @param num_samples The number of elements to sample.
 * @param samples Pointer to the array where the sampled elements will be stored.
 *
 * @note The function performs error checking to ensure valid inputs. If any error is detected,
 *       an error message is printed and the function returns without performing sampling.
 * 
 * @note This function uses a Fisher-Yates shuffle algorithm to ensure unique random sampling.
 */
void random_sample(void *array, size_t n, size_t size, size_t num_samples, void *samples) {
    RANDOM_LOG("[random_sample]: Entering random_sample with array: %p, n: %zu, size: %zu, num_samples: %zu, samples: %p", (void*)array, n, size, num_samples, (void*)samples);

    if (array == NULL) {
        RANDOM_LOG("[random_sample]: Error: array is NULL in random_sample.");
        return;
    }
    if (n == 0) {
        RANDOM_LOG("[random_sample]: Error: size of array is zero in random_sample.");
        return;
    }
    if (num_samples == 0) {
        RANDOM_LOG("[random_sample]: Error: number of samples is zero in random_sample.");
        return;
    }
    if (num_samples > n) {
        RANDOM_LOG("[random_sample]: Error: number of samples is greater than the number of elements in array.");
        return;
    }

    size_t *indices = (size_t *)malloc(n * sizeof(size_t));
    if (indices == NULL) {
        RANDOM_LOG("[random_sample]: Error: memory allocation failed for indices in random_sample.");
        return;
    }

    // Initialize indices
    RANDOM_LOG("[random_sample]: Initializing indices.");
    for (size_t i = 0; i < n; i++) {
        indices[i] = i;
    }

    // Shuffle the first num_samples elements
    RANDOM_LOG("[random_sample]: Shuffling array indices to select %zu samples.", num_samples);
    for (size_t i = 0; i < num_samples; i++) {
        size_t j = i + random_randint(0, n - i - 1);
        RANDOM_LOG("[random_sample]: Swapping indices[%zu] with indices[%zu]", i, j);
        size_t temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    // Copy the selected elements into the samples array
    RANDOM_LOG("[random_sample]: Copying selected samples to output array.");
    for (size_t i = 0; i < num_samples; i++) {
        memcpy((char *)samples + i * size, (char *)array + indices[i] * size, size);
        RANDOM_LOG("[random_sample]: Copied sample %zu from array index %zu", i, indices[i]);
    }

    free(indices);
    RANDOM_LOG("[random_sample]: Exiting random_sample.");
}

/**
 * @brief This function sets the state of the random number generator to the specified value.
 * 
 * @param state A pointer to the state value to set the random number generator to.
 *       if `state` is NULL, the function does nothing.
 */
void random_setstate(const unsigned int *state) {
    RANDOM_LOG("[random_setstate]: Entering random_setstate with state: %p", (void*)state);

    if (state != NULL) {
        rand_state = *state;
        srand(rand_state);
        RANDOM_LOG("[random_setstate]: State set to: %u", rand_state);
    } 
    else {
        RANDOM_LOG("[random_setstate]: Error: state is NULL in random_setstate.");
    }

    RANDOM_LOG("[random_setstate]: Exiting random_setstate");
}

/**
 * @brief This function retrieves the current state of the random number generator.
 * 
 * @param state A pointer to where the current state of the random number generator will be stored. 
 *       if `state` is NULL, the function does nothing.
 */
void random_getstate(unsigned int *state) {
    RANDOM_LOG("[random_getstate]: Entering random_getstate with state: %p", (void*)state);

    if (state != NULL) {
        *state = rand_state;
        RANDOM_LOG("[random_getstate]: Retrieved state: %u", rand_state);
    } 
    else {
        RANDOM_LOG("[random_getstate]: Error: state is NULL in random_getstate.");
    }

    RANDOM_LOG("[random_getstate]: Exiting random_getstate");
}

/**
 * @brief Generates a random double number based on the Gaussian (normal) distribution.
 *
 * @param mean The mean (mu) of the distribution.
 * @param stddev The standard deviation (sigma) of the distribution.
 * @return A random double number from the Gaussian distribution.
 *
 * @note The function uses the Box-Muller transform to generate a normally-distributed number.
 * 
 */
double random_gauss(double mean, double stddev) {
    RANDOM_LOG("[random_gauss]: Entering random_gauss with mean: %f, stddev: %f", mean, stddev);

    static int hasSpare = 0;
    static double spare;

    if (hasSpare) {
        hasSpare = 0;

        RANDOM_LOG("[random_gauss]: Using spare value for Gaussian generation: %f", spare);
        double result = mean + stddev * spare;
        RANDOM_LOG("[random_gauss]: Exiting random_gauss with result: %f", result);

        return result;
    }

    hasSpare = 1;
    double u, v, s;
    do {
        u = random_random() * 2.0 - 1.0;
        v = random_random() * 2.0 - 1.0;
        s = u * u + v * v;

        RANDOM_LOG("[random_gauss]: Generated u: %f, v: %f, s: %f", u, v, s);
    } while (s >= 1.0 || s == 0.0);

    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;
    RANDOM_LOG("[random_gauss]: Spare value stored: %f", spare);

    double result = mean + stddev * (u * s);
    RANDOM_LOG("[random_gauss]: Exiting random_gauss with result: %f", result);

    return result;
}

/**
 * @brief Generates a random double number based on the Exponential distribution.
 *
 * @param lambda The rate parameter (1/mean) of the distribution (must be greater than 0).
 * @return A random double number from the Exponential distribution.
 *         If `lambda` is less than or equal to 0, the function prints an error message
 *         and returns NAN.
 *
 * @note The function uses the inverse transform sampling method to produce the exponentially-distributed number.
 *       If `lambda` is less than or equal to 0, the function will print error message and return NAN.
 */
double random_expo(double lambda) {
    RANDOM_LOG("[random_expo]: Entering random_expo with lambda: %f", lambda);

    if (lambda <= 0.0) {
        RANDOM_LOG("[random_expo]: Error: lambda is less than or equal to 0 in random_expo.");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
        RANDOM_LOG("[random_expo]: Generated expo: %f", expo);
    } while (expo == 0.0);

    double result = -log(expo) / lambda;
    RANDOM_LOG("[random_expo]: Exiting random_expo with result: %f", result);

    return result;
}

/**
 * @brief Generates a random double number based on the Log-normal distribution.
 *
 * @param mean The mean parameter of the underlying normal distribution.
 * @param stddev The standard deviation parameter of the underlying normal distribution.
 * @return A random double number from the Log-normal distribution.
 * 
 */
double random_lognormal(double mean, double stddev) {
    RANDOM_LOG("[random_lognormal]: Entering random_lognormal with mean: %f, stddev: %f", mean, stddev);

    double normal_value = random_gauss(mean, stddev);
    RANDOM_LOG("[random_lognormal]: Generated normal_value: %f from underlying normal distribution", normal_value);

    double result = exp(normal_value);
    RANDOM_LOG("[random_lognormal]: Exiting random_lognormal with result: %f", result);

    return result;
}

/**
 * @brief Generates a random double number based on the Gamma distribution.
 *
 * @param shape The shape parameter of the distribution (must be greater than 0).
 * @param scale The scale parameter of the distribution (must be greater than 0).
 * @return A random double number from the Gamma distribution.
 *         If `shape` or `scale` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 *
 * @note The function uses different methods depending on the value of `shape`:
 *       - For `shape` < 1, Johnk's generator is used.
 *       - For `shape` >= 1, Marsaglia and Tsang's method is used.
 */
double random_gamma(double shape, double scale) {
    RANDOM_LOG("[random_gamma]: Entering random_gamma with shape: %f, scale: %f", shape, scale);

    if (shape <= 0.0 || scale <= 0.0) {
        RANDOM_LOG("[random_gamma]: Error: shape or scale is less than or equal to 0 in random_gamma.");
        return NAN;
    }

    if (shape < 1.0) {
        // Johnk's generator for shape < 1
        RANDOM_LOG("[random_gamma]: Using Johnk's generator for shape < 1");
        double expo;
        do {
            expo = random_random();
            RANDOM_LOG("[random_gamma]: Generated expo: %f", expo);
        } while (expo == 0.0);

        double result = random_gamma(1.0 + shape, scale) * pow(expo, 1.0 / shape);
        RANDOM_LOG("[random_gamma]: Exiting random_gamma (Johnk's) with result: %f", result);
        return result;
    }

    // Marsaglia and Tsang's method for shape >= 1
    RANDOM_LOG("[random_gamma]: Using Marsaglia and Tsang's method for shape >= 1");
    double d = shape - 1.0 / 3.0;
    double c = 1.0 / sqrt(9.0 * d);
    double v;

    while (1) {
        double u = random_random();
        double z = random_gauss(0.0, 1.0);
        v = pow(1.0 + c * z, 3);
        RANDOM_LOG("[random_gamma]: Generated u: %f, z: %f, v: %f", u, z, v);
        if (u < 1.0 - 0.0331 * (z * z) * (z * z)) {
            RANDOM_LOG("[random_gamma]: Break condition met in Marsaglia and Tsang's method (u condition).");
            break;
        }
        if (log(u) < 0.5 * z * z + d * (1.0 - v + log(v))) {
            RANDOM_LOG("[random_gamma]: Break condition met in Marsaglia and Tsang's method (log(u) condition).");
            break;
        }
    }

    double result = d * v * scale;
    RANDOM_LOG("[random_gamma]: Exiting random_gamma (Marsaglia and Tsang's) with result: %f", result);
    return result;
}

/**
 * @brief Generates a random double number based on the Beta distribution.
 *
 * @param alpha The shape1 parameter of the distribution (must be greater than 0).
 * @param beta The shape2 parameter of the distribution (must be greater than 0).
 * @return A random double number from the Beta distribution.
 *         If `alpha` or `beta` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 */
double random_beta(double alpha, double beta) {
    RANDOM_LOG("[random_beta]: Entering random_beta with alpha: %f, beta: %f", alpha, beta);

    if (alpha <= 0.0 || beta <= 0.0) {
        RANDOM_LOG("[random_beta]: Error: alpha or beta is less than or equal to 0 in random_beta.");
        return NAN;
    }

    double x = random_gamma(alpha, 1.0);
    double y = random_gamma(beta, 1.0);
    RANDOM_LOG("[random_beta]: Generated x: %f, y: %f for random_beta", x, y);

    double result = x / (x + y);
    RANDOM_LOG("[random_beta]: Exiting random_beta with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the Pareto distribution.
 * which is a continuous probability distribution.
 *
 * @param shape The shape parameter (a) of the distribution (must be greater than 0).
 * @param scale The scale parameter (m) of the distribution (must be greater than 0).
 * @return A random double number from the Pareto distribution.
 *         If `shape` or `scale` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 */
double random_pareto(double shape, double scale) {
    RANDOM_LOG("[random_pareto]: Entering random_pareto with shape: %f, scale: %f", shape, scale);

    if (shape <= 0.0 || scale <= 0.0) {
        RANDOM_LOG("[random_pareto]: Error: shape or scale is less than or equal to 0 in random_pareto.");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
        RANDOM_LOG("[random_pareto]: Generated expo: %f", expo);
    } while (expo == 0.0);

    double result = scale * pow((1.0 / expo), (1.0 / shape));
    RANDOM_LOG("[random_pareto]: Exiting random_pareto with result: %f", result);

    return result;
}

/**
 * @brief Generates a random double number based on the Weibull distribution.
 * which is a continuous probability distribution.
 *
 * @param shape The shape parameter (a) of the distribution (must be greater than 0).
 * @param scale The scale parameter (λ) of the distribution (must be greater than 0).
 * @return A random double number from the Weibull distribution.
 *         If `shape` or `scale` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 */
double random_weibull(double shape, double scale) {
    RANDOM_LOG("[random_weibull]: Entering random_weibull with shape: %f, scale: %f", shape, scale);

    if (shape <= 0.0 || scale <= 0.0) {
        RANDOM_LOG("[random_weibull]: Error: shape or scale is less than or equal to 0 in random_weibull.");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
    } while (expo == 0.0);

    double result = scale * pow(-log(expo), 1.0 / shape);
    RANDOM_LOG("[random_weibull]: Exiting random_weibull with result: %f", result);

    return result;
}

/**
 * @brief Generates a random double number based on the von Mises distribution.
 * which is a continuous probability distribution on the circle.
 *
 * @param mu The mean direction of the distribution (in radians).
 * @param kappa The concentration parameter (must be greater than 0). If `kappa` is less than or equal to 0, 
 *              the function prints an error message and returns NAN.
 * @return A random double number from the von Mises distribution.
 *         If `kappa` is less than or equal to 0, the function prints an error message and returns NAN.
 *         
 */
double random_vonmises(double mu, double kappa) {
    RANDOM_LOG("[random_vonmises]: Entering random_vonmises with mu: %f, kappa: %f", mu, kappa);
    if (kappa <= 0.0) {
        RANDOM_LOG("[random_vonmises]: Error: kappa is less than or equal to 0 in random_vonmises.");
        return NAN;
    }

    const double tau = 2 * M_PI;
    double r = 1 + sqrt(1 + 4 * kappa * kappa);
    double rho = (r - sqrt(2 * r)) / (2 * kappa);
    double s = (1 + rho * rho) / (2 * rho);

    double U, W, Y, Z, V;
    do {
        U = random_random();
        Z = cos(M_PI * U);
        W = (1 + s * Z) / (s + Z);
        V = random_random();
        
    } while (kappa * (s - W) - log(4 * V * (s - 1)) < 0);

    Y = 2 * random_random() - 1;
    if (Y < 0) {
        W = -W;
    }

    double result = fmod(mu + acos(W), tau);
    RANDOM_LOG("[random_vonmises]: Exiting random_vonmises with result: %f", result);

    return result;
}
