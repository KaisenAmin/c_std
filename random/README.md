# Random Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

This Random library in C provides a set of functions for generating random numbers within specified ranges. It is designed to be easy to integrate into C projects, offering essential functionality for random number generation.

## Key Features

- **Random Integer Generation:** Functions to generate random integers within specified ranges.

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

## Examples

Several examples are provided to demonstrate the usage of the Random library in various scenarios.

### Example 1: Basic Random Integer Generation

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


### Example 2 : get random number and check invalid steps and range in 'random_randrange'

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

## Conclusion

This Random library simplifies the generation of random numbers in C projects, providing an easy-to-use interface for generating random integers within specified ranges. The provided examples illustrate how to use the library for common random number generation tasks.

