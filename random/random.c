#include "random.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    rand_state = seed;
    srand(seed);
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
    if (a > b) {
        int temp = a;
        a = b;
        b = temp;
    }
    
    return a + rand() % (b - a + 1);
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
    if (a == b || step == 0) {
        fprintf(stderr, "Error: invalid range or step value\n");
        return -1;
    }
    if ((step > 0 && a > b) || (step < 0 && a < b)) {
        fprintf(stderr, "Error: logic error with range and step values\n");
        return -1;
    }

    int range = abs(b - a);
    int num_steps = (range + abs(step) - 1) / abs(step);  
    int random_step = rand() % num_steps;  

    return a + random_step * step;  
}

/**
 * @brief This function generates a random double precision floating-point number
 *      in the range [0, 1) using the standard library `rand()` function.
 *
 * @return A random double in the range [0, 1).
 */
double random_random() {
    return rand() / (double)RAND_MAX;
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
    if (a > b) {
        double temp = a;
        a = b;
        b = temp;
    }

    return a + (rand() / (double)RAND_MAX) * (b - a);
}

/**
 * @brief Generates a random integer with a specified number of random bits.
 *
 * @param a The number of random bits to generate (must be > 0 and <= 32).
 * @return A random integer with `a` random bits. Returns -1 for invalid input.
 */
int random_getrandbits(int a) {
    if (a <= 0 || a > (int)(sizeof(int) * 8)) {
        fprintf(stderr, "Error: a is less than zero or bigger than 32.\n");
        return -1; // invalid input
    }

    int result = 0;
    for (int i = 0; i < a; i++) {
        result = (result << 1) | (rand() & 1);
    }

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
    if (array == NULL) {
        fprintf(stderr, "Error: array is null in random_choice.\n");
        return;
    }
    else if (n == 0) {
        fprintf(stderr, "Error: size of array is zero.\n");
        return;
    }

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

/**
 * @brief This function sets the state of the random number generator to the specified value.
 * 
 * @param state A pointer to the state value to set the random number generator to.
 *       if `state` is NULL, the function does nothing.
 */
void random_setstate(const unsigned int *state) {
    if (state != NULL) {
        rand_state = *state;
        srand(rand_state);
    }
}

/**
 * @brief This function retrieves the current state of the random number generator.
 * 
 * @param state A pointer to where the current state of the random number generator will be stored. 
 *       if `state` is NULL, the function does nothing.
 */
void random_getstate(unsigned int *state) {
    if (state != NULL) {
        *state = rand_state;
    }
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
    static int hasSpare = 0;
    static double spare;

    if (hasSpare) {
        hasSpare = 0;
        return mean + stddev * spare;
    }

    hasSpare = 1;
    double u, v, s;
    do {
        u = random_random() * 2.0 - 1.0;
        v = random_random() * 2.0 - 1.0;
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);

    s = sqrt(-2.0 * log(s) / s);
    spare = v * s;

    return mean + stddev * (u * s);
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
    if (lambda <= 0.0) {
        fprintf(stderr, "Error: lambda must be greater than 0 in random_expo.\n");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
    } while (expo == 0.0);

    return -log(expo) / lambda;
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
    double normal_value = random_gauss(mean, stddev);

    return exp(normal_value);
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
    if (shape <= 0.0 || scale <= 0.0) {
        fprintf(stderr, "Error: shape and scale parameters must be greater than 0 in random_gamma.\n");
        return NAN;
    }

    if (shape < 1.0) {
        // Johnk's generator for shape < 1
        double expo;
        do {
            expo = random_random();
        } while (expo == 0.0);

        return random_gamma(1.0 + shape, scale) * pow(expo, 1.0 / shape);
    }

    // Marsaglia and Tsang's method for shape >= 1
    double d = shape - 1.0 / 3.0;
    double c = 1.0 / sqrt(9.0 * d);
    double v;

    while (1) {
        double u = random_random();
        double z = random_gauss(0.0, 1.0);
        v = pow(1.0 + c * z, 3);
        if (u < 1.0 - 0.0331 * (z * z) * (z * z)) {
            break;
        }
        if (log(u) < 0.5 * z * z + d * (1.0 - v + log(v))) {
            break;
        }
    }

    return d * v * scale;
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
    if (alpha <= 0.0 || beta <= 0.0) {
        fprintf(stderr, "Error: alpha and beta parameters must be greater than 0 in random_beta.\n");
        return NAN;
    }

    double x = random_gamma(alpha, 1.0);
    double y = random_gamma(beta, 1.0);

    return x / (x + y);
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
    if (shape <= 0.0 || scale <= 0.0) {
        fprintf(stderr, "Error: shape and scale parameters must be greater than 0 in random_pareto.\n");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
    } while (expo == 0.0);

    return scale * pow((1.0 / expo), (1.0 / shape));
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
    if (shape <= 0.0 || scale <= 0.0) {
        fprintf(stderr, "Error: shape and scale parameters must be greater than 0 in random_weibull.\n");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
    } while (expo == 0.0);

    return scale * pow(-log(expo), 1.0 / shape);
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
    if (kappa <= 0.0) {
        fprintf(stderr, "Error: kappa must be greater than 0 in random_vonmises.\n");
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

    return fmod(mu + acos(W), tau);
}
