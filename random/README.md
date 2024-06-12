# Random Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

This Random library in C provides a set of functions for generating random numbers within specified ranges. It is designed to be easy to integrate into C projects, offering essential functionality for random number generation.

## Key Features

- **Random Integer Generation:** Functions to generate random integers within specified ranges.
- **Random Floating-Point Generation:** Functions to generate random floating-point numbers between 0 and 1, or within specified ranges.
- **Array Shuffling:** A generic function to shuffle elements of an array, supporting different data types.


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

## Examples


Several examples are provided to demonstrate the usage of the Random library in various scenarios.

### Example 1: Basic Random Integer Generation with `random_seed` and `random_randint`

```c
#include "random.h"
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

    random = random_randrange(10, 1, 2);
    if (random == -1) {
        printf("Invalid range input.\n");
    }

    random = random_randrange(1, 10, 0);
    if (random == -1) {
        printf("Invalid step input.\n");
    }

    return 0;
}
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

### Example 4 : Genrate random floating point numbers between range with `random_uniform`

```c
#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    for (size_t i = 0; i < 5; i++) {
        double random = random_uniform(1.0, 10.256);
        printf("%lf\n", random);
    }
    
    return 0;
}
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
## Conclusion

This Random library simplifies the generation of random numbers in C projects, providing an easy-to-use interface for generating random integers within specified ranges. The provided examples illustrate how to use the library for common random number generation tasks.

