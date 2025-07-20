# Random Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

This Random library in C provides a set of functions for generating random numbers within specified ranges. It is designed to be easy to integrate into C projects, offering essential functionality for random number generation. this library is complete cross platform.

## Key Features

- **Random Integer Generation:** Functions to generate random integers within specified ranges.
- **Random Floating-Point Generation:** Functions to generate random floating-point numbers between 0 and 1, or within specified ranges.
- **Array Shuffling:** A generic function to shuffle elements of an array, supporting different data types.
- **Documentation:** This library is fully and comprehensively documented using Doxygen format.

## Installation and Compilation

To use this library, include `random.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./random/random.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like generating random integers within specified ranges.

### Function Descriptions

### `void random_seed(unsigned int seed)`

- **Purpose**: Sets the seed for the random number generator, allowing reproducible sequences of random numbers.
- **Parameters**:
  - `seed`: The seed value used to initialize the random number generator.
- **Return**: No return value.
- **Details**:
  - This function initializes the random number generator with a specific seed value to ensure that subsequent calls to random number generation functions produce a reproducible sequence.
  - It logs the seed value and uses `srand()` to set the seed, ensuring that the random generator produces the same numbers when the same seed is provided.
  - Logs entry, seed setting, and exit.

---

### `int random_randint(int a, int b)`

- **Purpose**: Generates a random integer in the range `[a, b]`, inclusive.
- **Parameters**:
  - `a`: The lower bound of the range (inclusive).
  - `b`: The upper bound of the range (inclusive).
- **Return**: A random integer within the range `[a, b]`.
- **Details**:
  - If `a > b`, their values are swapped to ensure the correct range.
  - It uses the `rand()` function to generate a random integer and constrains the result to the desired range.
  - Logs the input values, any necessary swaps, and the final result.

---

### `int random_randrange(int a, int b, int step)`

- **Purpose**: Generates a random integer in the range `[a, b)` with a specified step value.
- **Parameters**:
  - `a`: The start of the range (inclusive).
  - `b`: The end of the range (exclusive).
  - `step`: The step value for the range (cannot be zero).
- **Return**: A random integer in the range `[a, b)` that follows the step value, or `-1` if the input is invalid.
- **Details**:
  - This function generates a random integer from the sequence that starts at `a` and increments by `step` until it reaches `b`.
  - If `a == b` or `step == 0`, it logs an error and returns `-1`. It also validates that the step direction is correct based on the relationship between `a` and `b`.
  - Logs the range, step, and the final result.

---

### `double random_random()`

- **Purpose**: Generates a random double in the range `[0, 1)` using the standard library's `rand()` function.
- **Parameters**: None.
- **Return**: A random double precision floating-point number in the range `[0, 1)`.
- **Details**:
  - This function generates a random double by dividing the result of `rand()` by `RAND_MAX` to constrain the output to the range `[0, 1)`.
  - Logs the generated random double and exits with the result.

---

### `double random_uniform(double a, double b)`

- **Purpose**: Generates a random double in the range `[a, b)`. If `a > b`, their values are swapped to ensure the correct range.
- **Parameters**:
  - `a`: The lower bound of the range (inclusive).
  - `b`: The upper bound of the range (exclusive).
- **Return**: A random double in the range `[a, b)`.
- **Details**:
  - This function generates a random double precision floating-point number between `a` and `b`, exclusive of `b`.
  - If `a > b`, their values are swapped to ensure the correct range.
  - It uses the `rand()` function, scaling the result by `(b - a)` to generate a value within the desired range.
  - Logs the input values, any necessary swaps, and the final result.

---

### `int random_getrandbits(int a)`

- **Purpose**: Generates a random integer with a specified number of random bits.
- **Parameters**:
  - `a`: The number of random bits to generate (must be greater than 0 and less than or equal to 32).
- **Return**: A random integer with `a` random bits. Returns `-1` for invalid input.
- **Details**:
  - This function generates a random integer by creating `a` random bits. It shifts and combines random bits to form the final integer.
  - If the input `a` is out of the valid range (less than or equal to 0 or greater than 32), it logs an error and returns `-1`.
  - Logs each generated random bit and the current state of the result during the process, then returns the final integer.

---

### `void random_shuffle(void *array, size_t n, size_t size)`

- **Purpose**: Shuffles an array of any type using the Fisher-Yates algorithm.
- **Parameters**:
  - `array`: Pointer to the array to shuffle.
  - `n`: The number of elements in the array.
  - `size`: The size of each element in the array.
- **Return**: No return value.
- **Details**:
  - The function shuffles the elements of the array in place. It logs an error if the array is `NULL` or if `n` is zero.
  - The Fisher-Yates shuffle is performed by randomly swapping each element with another randomly selected element from the remaining unshuffled portion of the array.
  - Memory for swapping elements is dynamically allocated. If memory allocation fails during the swap, the function logs an error and exits.
  - Logs the swap of elements during each iteration and exits once the shuffle is complete.

---

### `void* random_choice(void* array, size_t n, size_t size)`

- **Purpose**: Randomly selects an element from an array and returns a pointer to it.
- **Parameters**:
  - `array`: Pointer to the array to select from.
  - `n`: The number of elements in the array.
  - `size`: The size of each element in the array.
- **Return**: Pointer to the randomly selected element or `NULL` for invalid input.
- **Details**:
  - This function returns a randomly selected element from the array. It logs an error and returns `NULL` if the array is `NULL` or `n` is zero.
  - A random index is generated using `random_randint`, and the corresponding element is retrieved from the array.
  - Logs the selected index and the pointer to the result before returning the randomly chosen element.

---

### `double random_triangular(double low, double high, double mode)`

- **Purpose**: Generates a random double number based on the Triangular distribution.
- **Parameters**:
  - `low`: The lower limit of the distribution.
  - `high`: The upper limit of the distribution.
  - `mode`: The mode (peak) of the distribution. If the `mode` is not within the range `[low, high]`, it is set to the midpoint of `low` and `high`.
- **Return**: A random double number from the Triangular distribution.
- **Details**:
  - If `low` is greater than `high`, their values are swapped.
  - The function uses inverse transform sampling to generate the triangular-distributed random value.
  - A random number `u` is generated, and based on its comparison with `c`, the triangular-distributed number is computed.
  - Logs the entire process, including swaps, random number generation, and result calculations.

---

### `void random_choices(void *array, size_t n, size_t size, size_t num_choices, void *choices, double *weights)`

- **Purpose**: Selects multiple random elements from an array based on specified weights.
- **Parameters**:
  - `array`: Pointer to the input array from which elements are selected.
  - `n`: The number of elements in the input array.
  - `size`: The size of each element in the input array.
  - `num_choices`: The number of elements to select.
  - `choices`: Pointer to the array where the selected elements will be stored.
  - `weights`: Pointer to the array of weights corresponding to each element in the input array.
- **Return**: No return value.
- **Details**:
  - The function checks for valid inputs, including whether the `array`, `weights`, or `num_choices` are valid.
  - It uses cumulative weights to perform weighted random sampling. The weights determine the likelihood of selecting an element.
  - The function calculates cumulative weights, generates a random number for each choice, and finds the corresponding element in the array based on that random number.
  - The selected elements are copied into the `choices` array.
  - The function logs errors, cumulative weight calculation, and every random selection step.
  - Memory for cumulative weights is dynamically allocated and freed after use.

---

### `void random_sample(void *array, size_t n, size_t size, size_t num_samples, void *samples)`

- **Purpose**: Samples a specified number of unique random elements from an array using the Fisher-Yates shuffle algorithm.
- **Parameters**:
  - `array`: Pointer to the input array from which elements are sampled.
  - `n`: The number of elements in the input array.
  - `size`: The size of each element in the input array.
  - `num_samples`: The number of elements to sample.
  - `samples`: Pointer to the array where the sampled elements will be stored.
- **Details**:
  - The function ensures unique random sampling by shuffling indices and selecting a subset of them.
  - It performs error checking, such as verifying if the `array` is not `NULL`, ensuring that `num_samples` does not exceed `n`, and handling memory allocation for indices.
  - After shuffling indices, it copies the corresponding elements into the `samples` array.
  - Logs the process of initialization, shuffling, copying, and memory management.

---

### `void random_setstate(const unsigned int *state)`

- **Purpose**: Sets the state of the random number generator to a specified value.
- **Parameters**:
  - `state`: A pointer to the state value to set the random number generator. If `state` is `NULL`, the function does nothing.
- **Details**:
  - This function allows the user to set the random number generator state to make the sequence of generated random numbers reproducible.
  - Logs the entry into the function, state assignment, and exit. If the `state` is `NULL`, it logs an error.

---

### `void random_getstate(unsigned int *state)`

- **Purpose**: Retrieves the current state of the random number generator.
- **Parameters**:
  - `state`: A pointer where the current state of the random number generator will be stored. If `state` is `NULL`, the function does nothing.
- **Details**:
  - The function captures the current state of the random number generator, which can be useful for saving the state for future reproducibility.
  - Logs the entry into the function, state retrieval, and exit. If the `state` is `NULL`, it logs an error.

---

### `double random_gauss(double mean, double stddev)`

- **Purpose**: Generates a random double number based on the Gaussian (normal) distribution using the Box-Muller transform.
- **Parameters**:
  - `mean`: The mean (mu) of the distribution.
  - `stddev`: The standard deviation (sigma) of the distribution.
- **Details**:
  - Utilizes the Box-Muller transform to produce normally-distributed numbers. It generates two independent random values from a uniform distribution, transforms them into normally distributed numbers, and returns one. The spare value is stored for the next call to avoid recomputation.
  - If a spare value is already available, the function uses that instead of recalculating.

---

### `double random_expo(double lambda)`

- **Purpose**: Generates a random double number based on the Exponential distribution.
- **Parameters**:
  - `lambda`: The rate parameter (1/mean) of the distribution.
- **Details**:
  - Uses the inverse transform sampling method to generate a random number from the Exponential distribution. It computes the result as `-log(uniform_random_value) / lambda`.
  - If `lambda` is less than or equal to 0, the function logs an error and returns `NAN`.

---

### `double random_lognormal(double mean, double stddev)`

- **Purpose**: Generates a random double number based on the Log-normal distribution.
- **Parameters**:
  - `mean`: The mean parameter of the underlying normal distribution.
  - `stddev`: The standard deviation parameter of the underlying normal distribution.
- **Details**:
  - First generates a random number from the Gaussian (normal) distribution using `random_gauss`, then exponentiates that value to obtain a number from the Log-normal distribution.
  - The log-normal distribution is useful when modeling data that is positively skewed.

---

### `double random_gamma(double shape, double scale)`

- **Purpose**: Generates a random double number based on the Gamma distribution.
- **Parameters**:
  - `shape`: The shape parameter of the distribution.
  - `scale`: The scale parameter of the distribution.
- **Details**:
  - Two different methods are used depending on the value of `shape`:
    - For `shape < 1`, the function uses Johnk's generator.
    - For `shape >= 1`, Marsaglia and Tsang's method is used, which is more efficient for larger shape values.
  - Both methods produce a random number from the Gamma distribution, which is commonly used in fields like queuing theory, reliability analysis, and Bayesian statistics.

---

### `double random_beta(double alpha, double beta)`

- **Purpose**: Generates a random double number based on the Beta distribution.
- **Parameters**:
  - `alpha`: The shape1 parameter of the distribution.
  - `beta`: The shape2 parameter of the distribution.
- **Details**:
  - The Beta distribution is generated by drawing two independent Gamma-distributed random numbers (with parameters `alpha` and `beta`), and then normalizing the result.
  - Returns `NaN` if `alpha` or `beta` is less than or equal to 0, otherwise, the result is a number between 0 and 1.

---

### `double random_pareto(double shape, double scale)`

- **Purpose**: Generates a random double number based on the Pareto distribution.
- **Parameters**:
  - `shape`: The shape parameter (a) of the distribution.
  - `scale`: The scale parameter (m) of the distribution.
- **Details**:
  - The Pareto distribution is generated using the inverse transform method. It returns a random number from the distribution defined by the `shape` and `scale` parameters.
  - Returns `NaN` if `shape` or `scale` is less than or equal to 0.

---

### `double random_weibull(double shape, double scale)`

- **Purpose**: Generates a random double number based on the Weibull distribution.
- **Parameters**:
  - `shape`: The shape parameter (a) of the distribution.
  - `scale`: The scale parameter (λ) of the distribution.
- **Details**:
  - The Weibull distribution is generated using the inverse transform sampling method. It returns a number based on the `shape` and `scale` parameters, commonly used in reliability analysis and survival models.
  - Returns `NaN` if `shape` or `scale` is less than or equal to 0.

---

### `double random_vonmises(double mu, double kappa)`

- **Purpose**: Generates a random double number based on the von Mises distribution, which is used for modeling circular data.
- **Parameters**:
  - `mu`: The mean direction of the distribution (in radians).
  - `kappa`: The concentration parameter (must be greater than 0).
- **Details**:
  - The von Mises distribution is a continuous probability distribution on the circle, useful in directional statistics. The function uses a rejection sampling algorithm to generate a random number from the von Mises distribution.
  - Returns `NaN` if `kappa` is less than or equal to 0.

---

## Examples

### Example 1: Basic Random Integer Generation with `random_seed` and `random_randint`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    int random = random_randint(1, 10);
    fmt_printf("Random number between 1 and 10: %d\n", random);

    random = random_randint(-10, -30);
    fmt_printf("Random number between -10 and -30: %d\n", random);

    size_t counter = 1;

    while (counter < 10) {
        fmt_printf("Counter %zu: %d\n", counter, random_randint(counter, 10));
        counter++;
    }

    return 0;
}
```
**Result:**
```
Random number between 1 and 10: 3
Random number between -10 and -30: -10
Counter 1: 8
Counter 2: 5
Counter 3: 6
Counter 4: 8
Counter 5: 5
Counter 6: 10
Counter 7: 9
Counter 8: 8
Counter 9: 10
```

---

### Example 2 : get random number and check invalid steps and range in `random_randrange`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());
    
    int random = 0;
    for (int i = 0; i < 5; i++) {
        random = random_randrange(50, 100, 10);
        fmt_printf("Random number between 50 and 100 with step 10: %d\n", random);
    }

    random = random_randrange(10, 2, -1);
    if (random == -1) {
        fmt_printf("Invalid range input.\n");
    }
    else {
        fmt_printf("Value is %d\n", random);
    }

    random = random_randrange(1, 10, 0);
    if (random == -1) {
        fmt_printf("Invalid step input.\n");
    }

    return 0;
}
```
**Result:**
```
Random number between 50 and 100 with step 10: 80
Random number between 50 and 100 with step 10: 80
Random number between 50 and 100 with step 10: 70
Random number between 50 and 100 with step 10: 60
Random number between 50 and 100 with step 10: 80
Value is 4
Error: invalid range or step value
Invalid step input.
```

---

### Example 3 : Generate floating point random number with `random_random`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        double random = random_random();
        fmt_printf("Random floating-point number between 0 and 1: %f\n", random);
    }

    return 0;
}
```
**Result:**
```
Random floating-point number between 0 and 1: 0.392407
Random floating-point number between 0 and 1: 0.790857
Random floating-point number between 0 and 1: 0.478957
Random floating-point number between 0 and 1: 0.041414
Random floating-point number between 0 and 1: 0.143132
```

---

### Example 4 : Genrate random floating point numbers between range with `random_uniform`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    for (size_t i = 0; i < 5; i++) {
        double random = random_uniform(1.0, 10.256);
        fmt_printf("%lf\n", random);
    }
    
    return 0;
}
```
**Result:**
```
4.655282
10.174928
2.001672
4.704716
1.116099
```

---

### Example 5: shuffle arrays of different types with `random_shuffle`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

void print_int_array(int* array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        fmt_printf("%d ", array[i]);
    }
    fmt_printf("\n");
}

void print_string_array(char** array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        fmt_printf("%s ", array[i]);
    }
    fmt_printf("\n");
}

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    int int_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);

    fmt_printf("Original integer array:\n");
    print_int_array(int_array, int_size);

    random_shuffle(int_array, int_size, sizeof(int));

    fmt_printf("Shuffled integer array:\n");
    print_int_array(int_array, int_size);

    char* string_array[] = {"apple", "banana", "cherry", "date", "elderberry", "fig", "grape"};
    size_t string_size = sizeof(string_array) / sizeof(string_array[0]);

    fmt_printf("Original string array:\n");
    print_string_array(string_array, string_size);

    random_shuffle(string_array, string_size, sizeof(char*));

    fmt_printf("Shuffled string array:\n");
    print_string_array(string_array, string_size);

    return 0;
}
```
**Result:**
```
Original integer array:
1 2 3 4 5 6 7 8 9 10 
Shuffled integer array:
2 6 10 7 8 3 1 9 5 4
Original string array:
apple banana cherry date elderberry fig grape
Shuffled string array:
fig banana grape date elderberry apple cherry
```

---

### Example 6 : how to choice and element from an array with `random_choice`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    int int_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);

    int *random_int = (int *)random_choice(int_array, int_size, sizeof(int));
    fmt_printf("Random choice from integer array: %d\n", *random_int);

    char *string_array[] = {"apple", "banana", "cherry", "date", "elderberry", "fig", "grape"};
    size_t string_size = sizeof(string_array) / sizeof(string_array[0]);

    char **random_string = (char **)random_choice(string_array, string_size, sizeof(char *));
    fmt_printf("Random choice from string array: %s\n", *random_string);
    
    return 0;
}
```
**Result:**
```
Random choice from integer array: 7
Random choice from string array: apple
```

---

### Example 7 : how to set mode between range with `random_triangular`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        double random = random_triangular(1.0, 10.0, 3.0);
        fmt_printf("Random triangular number between 1.0 and 10.0 with mode 5.0: %f\n", random);
    }

    return 0;
}
```
**Result:**
```
Random triangular number between 1.0 and 10.0 with mode 5.0: 3.865636
Random triangular number between 1.0 and 10.0 with mode 5.0: 4.834278
Random triangular number between 1.0 and 10.0 with mode 5.0: 5.182280
Random triangular number between 1.0 and 10.0 with mode 5.0: 5.509249
Random triangular number between 1.0 and 10.0 with mode 5.0: 4.765507
```

---

### Example 8 : how to set weight for choices with `random_choices`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    int int_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);
    int choices[5];
    double int_weights[] = {0.05, 0.05, 0.05, 0.05, 0.7, 0.05, 0.05, 0.05, 0.05, 0.05};

    random_choices(int_array, int_size, sizeof(int), 5, choices, int_weights);
    fmt_printf("Random choices from integer array with weights\n");

    for (size_t index = 0; index < 5; index++) {
        fmt_printf("%d\n", choices[index]);
    }

    fmt_printf("--------------\n");

    char *string_array[] = {"apple", "banana", "cherry", "date", "elderberry", "fig", "grape"};
    size_t string_size = sizeof(string_array) / sizeof(string_array[0]);
    char *string_choices[5];
    double string_weights[] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.7};

    random_choices(string_array, string_size, sizeof(char *), 5, string_choices, string_weights);
    fmt_printf("Random choices from string array with weights\n");

    for (size_t index = 0; index < 5; index++) {
        fmt_printf("%s\n", string_choices[index]);
    }

    return 0;
}
```
**Result:**
```
Random choices from integer array with weights
5
3
6
5
5
--------------
Random choices from string array with weights
grape
grape
grape
elderberry
grape
```

---

### Example 9 : how to get sequence of unique elements with `random_sample`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    int int_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);
    int int_samples[5];

    random_sample(int_array, int_size, sizeof(int), 5, int_samples);
    fmt_printf("Random sample from integer array\n");

    for (size_t index = 0; index < 5; index++) {
        fmt_printf("%d\n", int_samples[index]);
    }

    fmt_printf("--------------\n");

    char *string_array[] = {"C++", "C", "Python", "Go", "Rust", "Zig", "Php"};
    size_t string_size = sizeof(string_array) / sizeof(string_array[0]);
    char *string_samples[3];

    random_sample(string_array, string_size, sizeof(char *), 3, string_samples);
    fmt_printf("Random sample from string array\n");

    for (size_t index = 0; index < 3; index++) {
        fmt_printf("%s\n", string_samples[index]);
    }

    return 0;
}
```
**Result:**
```
Random sample from integer array
9
8
3
2
7
--------------
Random sample from string array
Python
C++
Zig
```

---

### Example 10 : set and get state `random_setstate`, `random_getstate`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    unsigned int state;

    random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        fmt_printf("Random number %d: %d\n", i + 1, random_randint(1, 100));
    }

    // Get the current state
    random_getstate(&state);
    fmt_printf("State saved.\n");

    for (int i = 0; i < 5; i++) {
        fmt_printf("Random number %d: %d\n", i + 6, random_randint(1, 100));
    }

    // Restore the state
    random_setstate(&state);
    fmt_printf("State restored.\n");

    for (int i = 0; i < 5; i++) {
        fmt_printf("Random number %d: %d\n", i + 6, random_randint(1, 100));
    }

    return 0;
}
```
**Result:**
```
Random number 1: 67
Random number 2: 71
Random number 3: 62
Random number 4: 86
Random number 5: 58
State saved.
Random number 6: 37
Random number 7: 15
Random number 8: 15
Random number 9: 67
Random number 10: 15
State restored.
Random number 6: 67
Random number 7: 71
Random number 8: 62
Random number 9: 86
Random number 10: 58
```

---

### Example 11 : how to generate random number with gauss or normal distrubution `random_gauss`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 10; i++) {
        double random_value = random_gauss(0.0, 1.0);
        fmt_printf("Random Gaussian value: %f\n", random_value);
    }

    return 0;
}
```
**Result:**
```
Random Gaussian value: -0.103868
Random Gaussian value: 0.550835
Random Gaussian value: -1.493940
Random Gaussian value: 1.779333
Random Gaussian value: -0.984763
Random Gaussian value: -1.616925
Random Gaussian value: -0.558794
Random Gaussian value: -0.141184
Random Gaussian value: -1.986525
Random Gaussian value: 1.008667
```

---

### Example 12 : get random number based on log-normal distrubution `random_lognormal`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"
#include <math.h>

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        double random_value = random_lognormal(0.0, 1.0);

        if (!isnan(random_value))
            fmt_printf("Random Log-Normal value: %f\n", random_value);
    }

    return 0;
}
```
**Result:**
```
Random Log-Normal value: 0.846263
Random Log-Normal value: 0.434523
Random Log-Normal value: 1.591718
Random Log-Normal value: 4.173639
Random Log-Normal value: 1.140356
```

---

### Example 13 : random numbers based on gamma distrubution `random_gamma`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"
#include <math.h>

int main() {
     random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 10; i++) {
        double random_value = random_gamma(2.0, 2.0);

        if (!isnan(random_value))
            fmt_printf("Random Gamma value: %f\n", random_value);
    }

    return 0;
}
```
**Result:**
```
Random Gamma value: 4.169290
Random Gamma value: 3.011071
Random Gamma value: 7.138485
Random Gamma value: 1.068895
Random Gamma value: 6.594782
```

---

### Example 14 : generate random number based on pareto distrubution with `random_pareto`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"
#include <math.h>

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 10; i++) {
        double random_value = random_pareto(2.0, 1.0);

        if (!isnan(random_value))
            fmt_printf("Random Pareto value: %f\n", random_value);
    }

    return 0;
}
```
**Result:**
```
Random Pareto value: 1.524975
Random Pareto value: 1.486391
Random Pareto value: 10.856653
Random Pareto value: 1.058269
Random Pareto value: 1.108613
```

---

### Example 15 : generate random number based on von mises distrubution with `random_vonmises`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/std_time.h"
#include <math.h>

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        double random_value = random_vonmises(0.0, 1.0);

        if (!isnan(random_value))
            fmt_printf("Random von Mises value: %f\n", random_value);
    }

    return 0;
}
```
**Result:**
```
Random von Mises value: 2.388292
Random von Mises value: 1.929918
Random von Mises value: 1.556270
Random von Mises value: 2.903683
Random von Mises value: 2.583023
```

---

## Integration with Statistics Library

The Random library works seamlessly with the Statistics library to provide comprehensive data analysis capabilities. Here's an example of how to use both libraries together:

### Example: Statistical Analysis of Random Data

```c
#include "random/random.h"
#include "statistics/statistics.h"
#include <stdio.h>

int main() {
    // Set seed for reproducible results
    random_seed(42);
    
    // Generate random data from different distributions
    double data[1000];
    for (int i = 0; i < 1000; i++) {
        if (i < 333) {
            // Gaussian distribution
            data[i] = random_gauss(50.0, 10.0);
        } else if (i < 666) {
            // Uniform distribution
            data[i] = random_uniform(30.0, 70.0);
        } else {
            // Exponential distribution
            data[i] = random_expo(0.1);
        }
    }
    
    // Calculate statistical measures
    double mean = statistics_mean(data, 1000);
    double median = statistics_median(data, 1000);
    double variance = statistics_variance(data, 1000, false, 0.0);
    double stdev = statistics_stdev(data, 1000, false, 0.0);
    
    printf("Statistical Analysis of Random Data:\n");
    printf("Mean: %.4f\n", mean);
    printf("Median: %.4f\n", median);
    printf("Variance: %.4f\n", variance);
    printf("Standard Deviation: %.4f\n", stdev);
    
    return 0;
}
```

### Example: Correlation Analysis with Random Data

```c
#include "random/random.h"
#include "statistics/statistics.h"
#include <stdio.h>

int main() {
    random_seed(12345);
    
    // Generate correlated data
    double x[500], y[500];
    for (int i = 0; i < 500; i++) {
        x[i] = i + random_gauss(0.0, 2.0);
        y[i] = 2.0 * x[i] + random_gauss(0.0, 5.0);
    }
    
    // Calculate correlation
    double correlation = statistics_correlation(x, y, 500, CORRELATION_LINEAR);
    double spearman = statistics_correlation(x, y, 500, CORRELATION_RANKED);
    
    printf("Correlation Analysis:\n");
    printf("Pearson correlation: %.4f\n", correlation);
    printf("Spearman correlation: %.4f\n", spearman);
    
    // Linear regression
    LinearRegression regression = statistics_linear_regression(x, y, 500, false);
    printf("Linear Regression: y = %.4f * x + %.4f\n", 
           regression.slope, regression.intercept);
    
    return 0;
}
```

## Conclusion

This Random library simplifies the generation of random numbers in C projects, providing an easy-to-use interface for generating random integers within specified ranges. The provided examples illustrate how to use the library for common random number generation tasks.

When combined with the Statistics library, it provides powerful capabilities for generating random data and performing comprehensive statistical analysis, making it ideal for scientific computing, data analysis, and simulation applications.


## License

This project is open-source and available under [ISC License].