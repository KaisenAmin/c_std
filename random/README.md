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

- `int random_randint(int a, int b)`: Generates a random integer between `a` and `b` (inclusive).
- `int random_randrange(int a, int b, int step)`: Generates a random number within the range [a, b) (excluding b), with a specified step.
- `void random_seed(unsigned int seed)`: random_seed initializes the random number generator with a given seed.
- `double random_random()`:  Generates a random floating-point number between 0 and 1.
- `double random_uniform(double a, double b)`: Generates a random floating-point number between a and b.
- `int random_getrandbits(int a)`:  Generates a random integer with a random bits. arg should be greater than z and less than 32.
- `void random_shuffle(void *array, size_t n, size_t size)`: Shuffles an array of any type.
- `void* random_choice(void* array, size_t n, size_t size)`: this function can randomly select an element from an array.
- `double random_triangular(double low, double high, double mode)`: this function returns a random float number between two given parameters, you can also set a mode parameter to specify the midpoint between the two other parameters.
- `void random_choices(void *array, size_t n, size_t size, size_t num_choices, void *choices, double *weights)`: this function select multiple random elements from an array with specified weights.
- `void random_sample(void *array, size_t n, size_t size, size_t num_samples, void *samples)`: this function a specified number of unique random elements from an array.
- `void random_getstate(unsigned int *state)`: this function get the current state of the random number generator.
- `void random_setstate(const unisgned int* state)`: this function set the state of the random number generator.
- `double random_gauss(double mean, doube stddev)`: this function generate a random float number based on the Gaussian distribution. also `stddev` is the standard deviation (sigma) of the distribution.
- `double random_expo(double lambda)`: this function generate a random float number based on the Exponential distribution. also `lambda` is the rate parameter (1/mean) of the distribution.
- `double random_lognormal(double mean, double stddev)`: this function generate a random float number based on the `log-normal` distribution. `mean` param is underlying normal distrubution. and `stddev` is rate parameter (1/mean) of the distribution.
- `double random_gamma(double shape, double scale)`: this function generate a random float number based on the Gamma distribution. based on this formula `https://numpy.org/doc/stable/reference/random/generated/numpy.random.gamma.html`

- `double random_beta(double alpha, double beta)`: this function generate a random float number based on the Beta distribution. also `alpha` param is shap1 of the distrubution, and `beta` is the shape2 o fthe distrubution. based on this formaul `https://numpy.org/doc/stable/reference/random/generated/numpy.random.beta.html`

- `double random_pareto(double shape, double scale)`: this function generate a random float number based on the Pareto distribution. also `shape` is param (a) of the distrubution and `scale` is param (m) of the distrubution. based on `https://numpy.org/doc/stable/reference/random/generated/numpy.random.pareto.html`

- `double random_weibull(double shape, double scale)`: this function generate a random float number based on the Weibull distribution.
also `shape` is param (a) of the distrubution and `scale` is param (λ) of the distribution. based on `https://numpy.org/doc/stable/reference/random/generated/numpy.random.weibull.html`.

- `double random_vonmises(double mu, double kappa)`: this function generate a random float number based on the von Mises distribution.
also `mu` is the mode (mean direction) of the distrubution. and `kappa` is the concentration parameter (dispersion) of the distribution. based on  `https://numpy.org/doc/stable/reference/random/generated/numpy.random.vonmises.html`.


## Examples


Several examples are provided to demonstrate the usage of the Random library in various scenarios.

### Example 1: Basic Random Integer Generation with `random_seed` and `random_randint`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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

### Example 2 : get random number and check invalid steps and range in `random_randrange`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 3 : Generate floating point random number with `random_random`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 4 : Genrate random floating point numbers between range with `random_uniform`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 5: shuffle arrays of different types with `random_shuffle`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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

### Example 6 : how to choice and element from an array with `random_choice`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 7 : how to set mode between range with `random_triangular`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 8 : how to set weight for choices with `random_choices`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 9 : how to get sequence of unique elements with `random_sample`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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

### Example 10 : set and get state `random_setstate`, `random_getstate`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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
### Example 11 : how to generate random number with gauss or normal distrubution `random_gauss`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

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

### Example 12 : get random number based on log-normal distrubution `random_lognormal`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"
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

### Example 13 : random numbers based on gamma distrubution `random_gamma`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"
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
### Example 14 : generate random number based on pareto distrubution with `random_pareto`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"
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

### Example 15 : generate random number based on von mises distrubution with `random_vonmises`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"
#include <math.h>

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    for (int i = 0; i < 50; i++) {
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

## Conclusion

This Random library simplifies the generation of random numbers in C projects, providing an easy-to-use interface for generating random integers within specified ranges. The provided examples illustrate how to use the library for common random number generation tasks.

