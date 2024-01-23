# Numeric Library in C

Author: Amin Tahmasebi
Release Date: 2024
License: ISC License

## Overview

The Numeric library in C is designed to bring the functionality of the C++ `<numeric>` header to C programming. It provides a range of numeric algorithms that simplify various calculations and numeric operations, making it a valuable asset for C developers who need advanced numeric capabilities similar to those in C++.

This library is part of a larger effort to reimplement features of the C++ standard library in C. It aims to provide a bridge for C++ developers working in C environments and to enhance the numeric capabilities of C with powerful, easy-to-use functions.

## Key Features

- **Generic Numeric Operations:** Includes functions like `numeric_iota`, which fills a range with successive increments of a starting value, similar to C++'s `std::iota`.
- **Advanced Mathematical Computations:** Implements functions for calculating the greatest common divisor (`gcd`), least common multiple (`lcm`), and various saturation arithmetic operations.
- **Interpolation and Reduction Operations:** Offers functions for linear interpolation (`lerp`), midpoint calculation, and various forms of reduction (`reduce`, `transform_reduce`).
- **Flexible and Generic:** Designed to work with various data types, allowing for a wide range of numeric operations across different types.

## Installation and Compilation

To utilize this library in your projects, include `numeric.h` in your source code. Compile the source files alongside your main C program using a C compiler like GCC. Example compilation command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main main.c numeric/numeric.c
```

Ensure that GCC or a compatible C compiler is installed and that all source files are correctly organized in your project's directory.

## Documentation

The documentation section provides detailed descriptions of all functions, their usage, parameters, and return values. This section helps users understand how to effectively use the library's features in various scenarios.

## Examples

The library comes with a variety of examples that demonstrate the usage of different functions. These examples cover scenarios like filling an array with incrementing values using `numeric_iota`, calculating the greatest common divisor and least common multiple, and performing linear interpolation.

## License

Distributed under the GNU General Public License v3.0, this library can be freely used, modified, and distributed under the same license terms.

---

## Function Descriptions

### Factor Operations
- `numeric_gcd(int, int)`: Computes the greatest common divisor of two integers.
- `numeric_lcm(int, int)`: Computes the least common multiple of two integers.

### Interpolation Operations
- `numeric_midpoint(type, type)`: Calculates the midpoint between two numbers or pointers.
- `numeric_lerp(type, type, double)`: Performs linear interpolation between two values.

### Saturation Arithmetic (C++26 Features)
- `numeric_add_sat(type, type)`: Saturating addition operation for two integers.
- `numeric_sub_sat(type, type)`: Saturating subtraction operation for two integers.
- `numeric_mul_sat(type, type)`: Saturating multiplication operation for two integers.
- `numeric_div_sat(type, type)`: Saturating division operation for two integers.
- `numeric_saturate_cast(type, type)`: Clamps an integer value to the range of another integer type.

### Numeric Operations
- `numeric_iota(void*, const void*, size_t, void*, IncrementFunc)`: Fills a range with successive increments of the starting value.
- `numeric_accumulate(type*, type*, type)`: Sums up elements in a range.
- `numeric_reduce(type*, type*, type)`: Performs a reduction on a range of elements, similar to `accumulate` but allows out-of-order execution.
- `numeric_transform_reduce(type*, type*, type, TransformFunc, ReduceFunc)`: Applies a transformation function and then reduces the range.
- `numeric_inner_product(type*, type*, type*, type)`: Computes the inner product of two ranges.
- `numeric_adjacent_difference(type*, type*, type*)`: Calculates the difference between adjacent elements in a range.
- `numeric_partial_sum(type*, type*, type*)`: Computes the partial sum of a range.
- `numeric_inclusive_scan(type*, type*, type*)`: Calculates an inclusive scan of a range.
- `numeric_exclusive_scan(type*, type*, type*)`: Calculates an exclusive scan of a range.
- `numeric_transform_inclusive_scan(type*, type*, TransformFunc, type*)`: Applies a transformation and calculates an inclusive scan.
- `numeric_transform_exclusive_scan(type*, type*, TransformFunc, type*)`: Applies a transformation and calculates an exclusive scan.

### Compilation
```bash
gcc -std=c17 -o program main.c numeric/numeric.c
```

### Example 1 : how to use `numeric_iota`
```c
#include "numeric/numeric.h"
#include "array/array.h"
#include "algorithm/algorithm.h"

void increment_int(void* input) {
    (*(int*)input)++;
}

void increment_char(void* input) {
    (*(char*)input)++;
}

inline static void print_int(void *input) {
    printf("%d ", *(int*)input);
}

inline static void print_char(void* input) {
    printf("%c ", *(char*)input);
}

int main() {
    int array[5] = {0};
    int element = 10;
    numeric_iota(array, array + 5, sizeof(int), &element, increment_int);

    printf("Integers : ");
    algorithm_for_each(array, 5, sizeof(int), print_int);
    printf("\n");

    char chars[5] = {0};
    char ch = 'a';
    numeric_iota(chars, chars + 5, sizeof(char), &ch, increment_char);

    printf("Chars : ");
    algorithm_for_each(chars, 5, sizeof(char), print_char);
    printf("\n");

    Array* arr = array_create(sizeof(int), 10);
    int number = 1;
    numeric_iota(array_begin(arr), array_end(arr), sizeof(int), &number, increment_int);

    printf("Array[int] : ");
    algorithm_for_each(array_begin(arr), array_size(arr), sizeof(int), print_int);
    printf("\n");

    array_deallocate(arr);
    return 0;
}
```

## Example 2 : find 'gcd' and 'lcm' with `numeric_gcd` and `numeric_lcm`

```c
#include "numeric/numeric.h"

int main() {
    int a = 10, b = 15;

    printf("GCD(%d, %d) = %d\n", a, b, numeric_gcd(a, b));
    printf("LCM(%d, %d) = %d\n", a, b, numeric_lcm(a, b));

    a = 35, b = 10;
    printf("GCD(%d, %d) = %d\n", a, b, numeric_gcd(a, b));
    printf("LCM(%d, %d) = %d\n", a, b, numeric_lcm(a, b));

    a = 31, b = 2;
    printf("GCD(%d, %d) = %d\n", a, b, numeric_gcd(a, b));
    printf("LCM(%d, %d) = %d\n", a, b, numeric_lcm(a, b));

    return 0;
}
```

## Example : How to work with midpoint in c_std and std C++ `numeric_midpoint`

```c
#include "numeric/numeric.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Example with uint64_t
    uint64_t e = 10000000000ULL, f = 20000000000ULL;
    uint64_t* midpoint_uint64 = (uint64_t*) numeric_midpoint(&e, &f, NUMERIC_UINT64, 0);
    printf("Midpoint uint64_t: %llu\n", *midpoint_uint64);
    
    // Example with int32_t
    int32_t a = 8, b = 4;
    int32_t* midpoint_int32 = (int32_t*) numeric_midpoint(&a, &b, NUMERIC_INT32, sizeof(int32_t));
    printf("Midpoint int32_t: %d\n", *midpoint_int32);

    // Example with int16_t
    int16_t c = 3000, d = 6000;
    int16_t* midpoint_int16 = (int16_t*) numeric_midpoint(&c, &d, NUMERIC_INT16, sizeof(int16_t));
    printf("Midpoint int16_t: %d\n", *midpoint_int16);

    // Example with uint16_t
    uint16_t g = 500, h = 1000;
    uint16_t* midpoint_uint16 = (uint16_t*) numeric_midpoint(&g, &h, NUMERIC_UINT16, sizeof(uint16_t));
    printf("Midpoint uint16_t: %u\n", *midpoint_uint16);

    const char text[] = "0123456789";
    const char* p = text + 2; // Pointing to '2'
    const char* q = text + 5; // Pointing to '5'
    const char* midpoint_char = (const char*) numeric_midpoint((void*)p, (void*)q, NUMERIC_POINTER, sizeof(char));
    printf("Midpoint with pointers ('%c' and '%c'): '%c'\n", *p, *q, *midpoint_char);
    
    // Free allocated memory
    free(midpoint_uint64);
    free(midpoint_int32);
    free(midpoint_int16);
    free(midpoint_uint16);

    return 0;
}
```

`C++ Same code `
```cpp
#include <iostream>
#include <numeric>
#include <cstdint>
#include <limits>

int main() {
    // Using std::midpoint with uint32_t
    std::uint32_t u32_max = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t u32_a = u32_max;
    std::uint32_t u32_b = u32_max - 2;
    std::cout << "Midpoint of uint32_t max and max-2: " << std::midpoint(u32_a, u32_b) << "\n";

    // Using std::midpoint with int32_t
    int32_t i32_a = 8, i32_b = 4;
    std::cout << "Midpoint of 8 and 4 (int32_t): " << std::midpoint(i32_a, i32_b) << "\n";

    // Using std::midpoint with int16_t
    int16_t i16_a = 3000, i16_b = 6000;
    std::cout << "Midpoint of 3000 and 6000 (int16_t): " << std::midpoint(i16_a, i16_b) << "\n";

    // Using std::midpoint with uint16_t
    uint16_t u16_a = 500, u16_b = 1000;
    std::cout << "Midpoint of 500 and 1000 (uint16_t): " << std::midpoint(u16_a, u16_b) << "\n";

    // Using std::midpoint with pointers
    const char* text = "0123456789";
    const char* p = text + 2;
    const char* q = text + 5;
    std::cout << "std::midpoint with pointers ('" << *p << "' and '" << *q << "'): '" << *std::midpoint(p, q) << "'\n";

    return 0;
}

```