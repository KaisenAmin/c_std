# Numbers Library in C

Author: amin tahmasebi
Release Date: 2024
License: ISC License


The Numbers library in C provides constants for common mathematical values, analogous to the `<numbers>` header introduced in C++20. It offers a simple way to access important mathematical constants in C programming, enhancing the precision and readability of mathematical computations.

## Compilation

To compile the Numbers library with your main program, include the `numbers.h` file in your project and use a standard GCC compilation command. If other libraries are needed, include their `.c` files in the command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c 
```

Make sure that GCC is installed on your system and that the `numbers.h` file is in the correct directory.

## Usage

To use the Numbers library, simply include the `numbers.h` header file in your C source files:

```c
#include "numbers.h"
```

## Constants Description

Each constant in the `numbers.h` library represents a fundamental mathematical value, providing high precision for mathematical calculations:

- `NUMBERS_E`: Represents Euler's number, the base of natural logarithms, approximately 2.718.
- `NUMBERS_LOG2E`: The logarithm base 2 of Euler's number, approximately 1.442.
- `NUMBERS_LOG10E`: The logarithm base 10 of Euler's number, approximately 0.434.
- `NUMBERS_PI`: Represents the mathematical constant pi, the ratio of the circumference of a circle to its diameter, approximately 3.14159.
- `NUMBERS_INV_PI`: The multiplicative inverse of pi, approximately 0.318.
- `NUMBERS_INV_SQRTPI`: The inverse of the square root of pi, approximately 0.564.
- `NUMBERS_LN2`: The natural logarithm of 2, approximately 0.693.
- `NUMBERS_LN10`: The natural logarithm of 10, approximately 2.303.
- `NUMBERS_SQRT2`: The square root of 2, representing the length of the diagonal of a square with side length 1, approximately 1.414.
- `NUMBERS_SQRT3`: The square root of 3, important in various geometrical and trigonometric contexts, approximately 1.732.
- `NUMBERS_INV_SQRT3`: The inverse of the square root of 3, approximately 0.577.
- `NUMBERS_EGAMMA`: Euler-Mascheroni constant, a recurring constant in number theory and analysis, approximately 0.577.
- `NUMBERS_PHI`: The golden ratio, a special number that appears in various areas of mathematics and art, approximately 1.618.

### Example 1: Calculating Compound Interest Using Euler's Number `(e)`

```c
#include "numbers.h"
#include <stdio.h>

int main() {
    double principal = 1000.0; // Initial investment
    double rate = 0.05;        // Annual interest rate
    int years = 10;            // Number of years

    // Compound interest formula: A = P * e^(rt)
    double amount = principal * pow(NUMBERS_E, rate * years);

    printf("Amount after %d years: $%.2f\n", years, amount);
    return 0;
}
```

### Example 2: Converting Radians to Degrees Using `Pi`

```c
#include "numbers.h"
#include <stdio.h>

int main() {
    double radians = 1.0; // Radians
    double degrees = radians * (180.0 / NUMBERS_PI); // Convert to degrees

    printf("%.2f radians is equal to %.2f degrees\n", radians, degrees);
    return 0;
}
```

### Example 3: Using the Golden Ratio `(Phi)`

```c
#include "numbers.h"
#include <stdio.h>

int main() {
    int n = 10; // nth position in Fibonacci sequence
    // Approximating the nth Fibonacci number using Golden Ratio
    double fib_n = (pow(NUMBERS_PHI, n) - pow(-NUMBERS_PHI, -n)) / sqrt(5);

    printf("Approximate %dth Fibonacci number: %.0f\n", n, fib_n);
    return 0;
}
```
