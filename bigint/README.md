# BigInt Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2025  
**License:** ISC License

## Overview

The BigInt library in C provides a robust interface for performing arbitrary-precision integer arithmetic. It leverages the power of the GMP (GNU Multiple Precision Arithmetic) library under the hood, offering C developers a familiar API for creating, manipulating, and performing arithmetic on large integers that exceed the limitations of standard C integer types. This library supports both positive and negative numbers and implements common operations such as addition, subtraction, multiplication, division (with remainder), exponentiation, and modular arithmetic.

## Key Features

- **Arbitrary-Precision Arithmetic:** Perform computations with integers of virtually any size.
- **Full Range Support:** Supports both positive and negative numbers.
- **Basic Arithmetic Operations:** Includes addition, subtraction, multiplication, and division with remainder.
- **Conversion Utilities:** Convert BigInts to and from decimal string representations.
- **Comparison Functions:** Easily compare two BigInts.
- **GMP-Powered:** Built on top of GMP for high performance and reliability.
- **Cross-Platform:** Works on Linux, Windows, and other platforms where GMP is available.

## Installation and Compilation

To use the BigInt library in your project:

1. **Install GMP:**
   - **On Linux (Debian/Ubuntu):**
     ```bash
     sudo apt-get install libgmp-dev
     ```
   - **On macOS (with Homebrew):**
     ```bash
     brew install gmp
     ```
   - **On Windows:**
     Download GMP from [gmplib.org](https://gmplib.org/) or use a package manager such as vcpkg.

2. **Include the Library:**
   - Place the `bigint.h` header and `bigint.c` source file in your project.
   - Include `bigint.h` in your C source files where you need arbitrary-precision arithmetic.

3. **Compile Your Code:**
   Use a command similar to the following (adjust paths as needed):
   ```bash
   gcc -std=c11 -O2 -o bigint_example bigint.c main.c -lgmp

---

## BigInt Library Functions

### `BigInt* bigint_create(void)`

- **Purpose**:  
  Creates a new BigInt initialized to zero.

- **Parameters**:  
  _None._

- **Return**:  
  A pointer to a new BigInt, or `NULL` if memory allocation fails.

- **Details**:  
  - Allocates memory for a BigInt structure.
  - Initializes the internal GMP value (`mpz_t`) to 0 using `mpz_init`.
  - Logs the creation if logging is enabled.

---

### `BigInt* bigint_from_string(const char* str)`

- **Purpose**:  
  Creates a BigInt from a decimal string representation.

- **Parameters**:  
  - `str`: A pointer to a null-terminated decimal string representing the integer.

- **Return**:  
  A pointer to the new BigInt, or `NULL` if the string is invalid or memory allocation fails.

- **Details**:  
  - Calls `bigint_create()` to allocate and initialize a new BigInt.
  - Uses `mpz_set_str` to convert the string into the internal GMP format.
  - If conversion fails (i.e. the string is not a valid decimal representation), the function logs the error, deallocates the BigInt, and returns `NULL`.

---

### `BigInt* bigint_copy(const BigInt* src)`

- **Purpose**:  
  Creates an exact copy of an existing BigInt.

- **Parameters**:  
  - `src`: A pointer to the source BigInt to be copied.

- **Return**:  
  A pointer to the newly created BigInt with the same value as `src`, or `NULL` on failure.

- **Details**:  
  - Allocates a new BigInt using `bigint_create()`.
  - Copies the GMP value from `src` using `mpz_set`.
  - Logs the copy operation.

---

### `void bigint_deallocate(BigInt* bi)`

- **Purpose**:  
  Frees the memory associated with a BigInt.

- **Parameters**:  
  - `bi`: A pointer to the BigInt to be deallocated.

- **Return**:  
  _None._

- **Details**:  
  - Clears the GMP value using `mpz_clear`.
  - Frees the memory allocated for the BigInt structure.
  - Logs the deallocation if logging is enabled.

---

### `char* bigint_to_string(const BigInt* bi)`

- **Purpose**:  
  Converts a BigInt to its decimal string representation.

- **Parameters**:  
  - `bi`: A pointer to the BigInt to convert.

- **Return**:  
  A dynamically allocated string containing the decimal representation of the BigInt.  
  The caller is responsible for freeing this string.

- **Details**:  
  - Uses GMP's `mpz_get_str` to convert the internal `mpz_t` value to a string (base 10).
  - Logs the conversion operation.
  - Returns `NULL` if `bi` is `NULL`.

---

### `BigInt* bigint_add(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Computes the sum of two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first BigInt.
  - `b`: A pointer to the second BigInt.

- **Return**:  
  A pointer to a new BigInt representing the sum, or `NULL` on error.

- **Details**:  
  - Allocates a new BigInt using `bigint_create()`.
  - Uses `mpz_add` to add the two GMP values.
  - Logs the addition operation.

---

### `BigInt* bigint_subtract(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Subtracts one BigInt from another.

- **Parameters**:  
  - `a`: A pointer to the minuend BigInt.
  - `b`: A pointer to the subtrahend BigInt.

- **Return**:  
  A pointer to a new BigInt representing the result of `a - b`, or `NULL` on error.

- **Details**:  
  - Creates a new BigInt via `bigint_create()`.
  - Uses `mpz_sub` to perform the subtraction.
  - Logs the subtraction operation.

---

### `BigInt* bigint_multiply(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Multiplies two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first BigInt.
  - `b`: A pointer to the second BigInt.

- **Return**:  
  A pointer to a new BigInt representing the product, or `NULL` on error.

- **Details**:  
  - Allocates a new BigInt using `bigint_create()`.
  - Uses `mpz_mul` to compute the product.
  - Logs the multiplication operation.

---

### `BigInt* bigint_divide(const BigInt* a, const BigInt* b, BigInt** remainder)`

- **Purpose**:  
  Divides one BigInt by another and returns the quotient. Optionally, the remainder is also provided.

- **Parameters**:  
  - `a`: A pointer to the dividend BigInt.
  - `b`: A pointer to the divisor BigInt.
  - `remainder`: A pointer to a `BigInt*` that will receive the remainder.  
    If `NULL`, the remainder is discarded.

- **Return**:  
  A pointer to a new BigInt representing the quotient, or `NULL` if division by zero is attempted or an error occurs.

- **Details**:  
  - Checks if `b` is zero using `mpz_cmp_ui`.
  - Allocates new BigInts for the quotient (and remainder if needed).
  - Uses `mpz_tdiv_qr` to simultaneously compute the quotient and remainder.
  - Logs the division operation.
  - If `remainder` is `NULL`, deallocates the temporary remainder BigInt.

---

### `int bigint_compare(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Compares two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first BigInt.
  - `b`: A pointer to the second BigInt.

- **Return**:  
  - `-1` if `a < b`
  - `0` if `a == b`
  - `1` if `a > b`

- **Details**:  
  - Uses `mpz_cmp` to compare the underlying GMP values.
  - Logs the result of the comparison.

---

### `void bigint_normalize(BigInt* bi)`

- **Purpose**:  
  Normalizes the BigInt by removing any leading zeros in its internal representation.

- **Parameters**:  
  - `bi`: A pointer to the BigInt to normalize.

- **Return**:  
  _None._

- **Details**:  
  - GMP automatically maintains a normalized representation, so no explicit action is needed.
  - The function logs that normalization was (conceptually) performed.
  - Uses `(void)bi;` to suppress unused parameter warnings.

---

### `bool bigint_is_zero(const BigInt* bi)`

- **Purpose**:  
  Checks whether a BigInt is equal to zero.

- **Parameters**:  
  - `bi`: A pointer to the BigInt to check.

- **Return**:  
  `true` if the BigInt is zero, `false` otherwise.

- **Details**:  
  - Uses `mpz_cmp_ui` to compare the BigInt with 0.
  - Logs whether the BigInt was determined to be zero or not.

---

### `BigInt* bigint_pow(const BigInt* base, const BigInt* exponent)`

- **Purpose**:  
  Raises a BigInt (`base`) to the power of another BigInt (`exponent`).

- **Parameters**:  
  - `base`: A pointer to the base BigInt.
  - `exponent`: A pointer to the exponent BigInt (assumed to be non-negative).

- **Return**:  
  A pointer to a new BigInt representing `base^exponent`, or `NULL` if `exponent` is negative.

- **Details**:  
  - Checks that the exponent is non-negative using `mpz_sgn`.
  - Converts the exponent to an unsigned long with `mpz_get_ui` (assumes it fits).
  - Uses `mpz_pow_ui` to compute the power.
  - Logs the power operation.

---

### `BigInt* bigint_mod(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Computes the modulus (remainder) of one BigInt divided by another.

- **Parameters**:  
  - `a`: A pointer to the dividend BigInt.
  - `b`: A pointer to the divisor BigInt.

- **Return**:  
  A pointer to a new BigInt representing `a mod b`, or `NULL` if `b` is zero.

- **Details**:  
  - Checks for division by zero by comparing `b` to zero.
  - Allocates a new BigInt using `bigint_create()`.
  - Uses `mpz_mod` to compute the modulus.
  - Logs the modulus operation.

---

### `BigInt* bigint_powmod(const BigInt* base, const BigInt* exponent, const BigInt* modulus)`

- **Purpose**:  
  Computes the modular exponentiation of a BigInt. In other words, it calculates  
  \((\text{base}^{\text{exponent}}) \mod \text{modulus}\).

- **Parameters**:  
  - `base`: A pointer to the base BigInt.  
  - `exponent`: A pointer to the exponent BigInt.  
  - `modulus`: A pointer to the modulus BigInt (must be non-zero).

- **Return**:  
  A pointer to a new BigInt representing \((\text{base}^{\text{exponent}}) \mod \text{modulus}\), or `NULL` if any input is `NULL` or if the modulus is zero.

- **Details**:  
  - Validates that none of the input pointers are `NULL` and that the modulus is non-zero.
  - Uses GMP's `mpz_powm` function to compute the result.
  - Logs each step of the operation.

---

### `BigInt* bigint_negate(const BigInt* a)`

- **Purpose**:  
  Returns the negation of the given BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt to negate.

- **Return**:  
  A pointer to a new BigInt representing \(-a\), or `NULL` if the input is `NULL` or if memory allocation fails.

- **Details**:  
  - Validates that the input BigInt is not `NULL`.
  - Creates a copy of the input BigInt and then applies GMP's `mpz_neg` to negate it.
  - Logs the negation process.

---

### `BigInt* bigint_abs(const BigInt* a)`

- **Purpose**:  
  Returns the absolute value of a BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt whose absolute value is to be computed.

- **Return**:  
  A pointer to a new BigInt representing \(|a|\), or `NULL` if the input is `NULL` or if memory allocation fails.

- **Details**:  
  - Checks that the input is valid.
  - Creates a copy of the input BigInt and applies GMP's `mpz_abs` to obtain the absolute value.
  - Logs the absolute value computation.

---

### `BigInt* bigint_gcd(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Computes the greatest common divisor (GCD) of two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first BigInt.
  - `b`: A pointer to the second BigInt.

- **Return**:  
  A pointer to a new BigInt representing \(\gcd(a, b)\), or `NULL` if any input is `NULL` or if memory allocation fails.

- **Details**:  
  - Validates that both input BigInts are not `NULL`.
  - Uses GMP's `mpz_gcd` to compute the GCD.
  - Logs the GCD computation process.

---

### `BigInt* bigint_lcm(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Computes the least common multiple (LCM) of two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first BigInt.
  - `b`: A pointer to the second BigInt.

- **Return**:  
  A pointer to a new BigInt representing \(\text{lcm}(a, b)\), or `NULL` if any input is `NULL` or if memory allocation fails.

- **Details**:  
  - Checks that both input BigInts are valid.
  - Uses GMP's `mpz_lcm` function to calculate the LCM.
  - Logs the steps of the LCM computation.

---

### `BigInt* bigint_next_prime(const BigInt* a)`

- **Purpose**:  
  Computes the next prime number greater than the given BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt.

- **Return**:  
  A pointer to a new BigInt representing the next prime, or `NULL` if the input is `NULL` or if memory allocation fails.

- **Details**:  
  - Validates the input BigInt.
  - Creates a copy of the input BigInt and then uses GMP's `mpz_nextprime` to find the next prime.
  - Logs the process of computing the next prime.

---

### `BigInt* bigint_sqrt(const BigInt* a, BigInt** rem)`

- **Purpose**:  
  Computes the integer square root (the floor of the true square root) of a BigInt, and optionally returns the remainder.

- **Parameters**:  
  - `a`: A pointer to the BigInt whose square root is to be computed.
  - `rem`: A pointer to a `BigInt*` where the remainder will be stored (if not `NULL`).

- **Return**:  
  A pointer to a new BigInt representing the integer square root, or `NULL` if the input is `NULL` or if memory allocation fails.

- **Details**:  
  - Validates the input BigInt.
  - Uses GMP's `mpz_sqrtrem` function to compute both the square root and the remainder.
  - Logs whether the remainder was stored or discarded.

---

### `BigInt* bigint_random(unsigned long bits)`

- **Purpose**:  
  Generates a random BigInt with the specified number of bits.

- **Parameters**:  
  - `bits`: The number of bits for the random BigInt.

- **Return**:  
  A pointer to a new BigInt containing a random value, or `NULL` if memory allocation fails.

- **Details**:  
  - Initializes a GMP random state and seeds it (using the current time).
  - Uses GMP's `mpz_urandomb` function to generate a random BigInt with the specified bit length.
  - Logs the seeding and random generation process.
  - Clears the random state before returning the result.

---

### `BigInt* bigint_factorial(unsigned long n)`

- **Purpose**:  
  Computes the factorial of an unsigned long integer \(n\) (i.e., \(n!\)).

- **Parameters**:  
  - `n`: The number for which the factorial is to be computed.

- **Return**:  
  A pointer to a new BigInt representing \(n!\), or `NULL` if memory allocation fails.

- **Details**:  
  - Iteratively multiplies integers from 1 to \(n\) using GMP's multiplication functions.
  - Logs each multiplication step for debugging purposes.
  - Returns the computed factorial as a BigInt.

---

### `int bigint_is_prime(const BigInt* a, int reps)`

- **Purpose**:  
  Checks whether a BigInt is prime using a probabilistic test.

- **Parameters**:  
  - `a`: A pointer to the BigInt to test.
  - `reps`: The number of repetitions for the primality test (higher values yield greater accuracy).

- **Return**:  
  An integer result:
  - `0` if the BigInt is composite.
  - `1` if the BigInt is probably prime.
  - `2` if the BigInt is definitely prime.

- **Details**:  
  - Uses GMP's `mpz_probab_prime_p` function to perform the test.
  - Logs the input parameters and the result of the primality test.

--- 

### `BigInt* bigint_inc(const BigInt* a)`

- **Purpose**:  
  Increments a BigInt by 1.

- **Parameters**:  
  - `a`: A pointer to the BigInt to increment.

- **Return**:  
  A pointer to a new BigInt representing (a + 1), or `NULL` if the input is `NULL` or if memory allocation fails.

- **Details**:  
  - Checks that the input BigInt is not `NULL`.
  - Creates a copy of the input BigInt.
  - Uses GMP's `mpz_add_ui` to add 1 to the copied value.
  - Logs each step of the operation.

---

### `BigInt* bigint_dec(const BigInt* a)`

- **Purpose**:  
  Decrements a BigInt by 1.

- **Parameters**:  
  - `a`: A pointer to the BigInt to decrement.

- **Return**:  
  A pointer to a new BigInt representing (a - 1), or `NULL` if the input is `NULL` or if memory allocation fails.

- **Details**:  
  - Validates that the input BigInt is not `NULL`.
  - Creates a copy of the input BigInt.
  - Uses GMP's `mpz_sub_ui` to subtract 1 from the copied value.
  - Logs every decision and the result of the decrement operation.

---

### `unsigned long bigint_bit_length(const BigInt* a)`

- **Purpose**:  
  Returns the bit-length of a BigInt; that is, the number of bits required to represent the BigInt in binary.

- **Parameters**:  
  - `a`: A pointer to the BigInt whose bit-length is to be computed.

- **Return**:  
  The number of bits required to represent the BigInt, or `0` if the input is `NULL`.

- **Details**:  
  - Checks that the input is not `NULL`.
  - Uses GMP's `mpz_sizeinbase` with base 2 to determine the number of bits.
  - Logs the computed bit-length.

---

### `bool bigint_is_even(const BigInt* a)`

- **Purpose**:  
  Checks whether a BigInt is even.

- **Parameters**:  
  - `a`: A pointer to the BigInt to check.

- **Return**:  
  `true` if the BigInt is even, `false` otherwise. If the input is `NULL`, it returns `false`.

- **Details**:  
  - Validates that the input is not `NULL`.
  - Uses GMP's `mpz_tstbit` function to test the least significant bit (bit 0).
  - Logs whether the BigInt was determined to be even or odd.

---

### `BigInt* bigint_modinv(const BigInt* a, const BigInt* modulus)`

- **Purpose**:  
  Computes the modular inverse of a BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt whose inverse is to be computed.
  - `modulus`: A pointer to the modulus BigInt (must be non-zero).

- **Return**:  
  A pointer to a new BigInt representing the modular inverse of `a` modulo `modulus`, or `NULL` if the inverse does not exist or if an error occurs.

- **Details**:  
  - Validates that neither `a` nor `modulus` is `NULL`, and that `modulus` is non-zero.
  - Uses GMP's `mpz_invert` to compute the inverse.
  - Logs each step of the operation.

---

### `BigInt* bigint_sqrt_exact(const BigInt* a)`

- **Purpose**:  
  Computes the exact square root of a BigInt if it is a perfect square.

- **Parameters**:  
  - `a`: A pointer to the BigInt whose exact square root is to be computed.

- **Return**:  
  A pointer to a new BigInt representing the exact square root if `a` is a perfect square, or `NULL` if `a` is not a perfect square or if an error occurs.

- **Details**:  
  - Checks whether the given BigInt is a perfect square using GMP's `mpz_perfect_square_p`.
  - If it is a perfect square, computes the square root using GMP's `mpz_sqrt`.
  - Logs all steps of the computation.

---

### `char* bigint_to_hex(const BigInt* a)`

- **Purpose**:  
  Converts a BigInt to its hexadecimal string representation.

- **Parameters**:  
  - `a`: A pointer to the BigInt to convert.

- **Return**:  
  A pointer to the hexadecimal string representation, or `NULL` if `a` is `NULL`.

- **Details**:  
  - Uses GMP's `mpz_get_str` with base 16 to convert the BigInt.
  - Logs the conversion operation.

---


### `unsigned long bigint_num_digits(const BigInt* a)`

- **Purpose**:  
  Returns the number of decimal digits required to represent a BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt.

- **Return**:  
  The number of decimal digits needed to represent the BigInt, or `0` if `a` is `NULL`.

- **Details**:  
  - Uses GMP's `mpz_sizeinbase` with base 10 to compute the number of digits.
  - Logs the computed number of digits.


---

### `unsigned long bigint_log2(const BigInt* a)`

- **Purpose**:  
  Computes the floor of the base‑2 logarithm of a BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt.

- **Return**:  
  The floor of log₂(|a|) (i.e. the number of bits required minus one), or 0 if `a` is `NULL` or if `a` is zero.

- **Details**:  
  - This is computed as `bigint_bit_length(a) - 1` for non‑zero values.
  - Logs each step of the computation for debugging purposes.

---

### `int bigint_cmp_abs(const BigInt* a, const BigInt* b)`

- **Purpose**:  
  Compares the absolute values of two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first BigInt.
  - `b`: A pointer to the second BigInt.

- **Return**:  
  - `-1` if |a| < |b|
  - `0` if |a| == |b|
  - `1` if |a| > |b|

- **Details**:  
  - Uses GMP's `mpz_cmpabs` function (if available) to compare the absolute values.
  - Logs the comparison result for debugging.

### `unsigned long bigint_sum_digits(const BigInt* a)`

- **Purpose**:  
  Computes the sum of the decimal digits of a BigInt.

- **Parameters**:  
  - `a`: A pointer to the BigInt.

- **Return**:  
  The sum of the decimal digits of the BigInt, or `0` if `a` is `NULL`.

- **Details**:  
  - Converts the BigInt to its decimal string representation using `bigint_to_string`.
  - Iterates over the string, accumulating the sum of numeric digits while ignoring non-digit characters (such as a minus sign).
  - Logs the computed sum for debugging.

---

### **`BigInt* bigint_and(const BigInt* a, const BigInt* b)`**

- **Purpose**:  
  Computes the **bitwise AND** operation between two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first **BigInt** operand.
  - `b`: A pointer to the second **BigInt** operand.

- **Return**:  
  - A **pointer** to a new **BigInt** representing the result of `a & b`.
  - Returns `NULL` if **memory allocation fails** or if either input is `NULL`.

- **Details**:  
  - The function first **validates** that both input BigInts are not `NULL`.
  - A **new BigInt** is allocated using `bigint_create()`.
  - The **bitwise AND** operation is performed using **GMP's `mpz_and()`** function.
  - The result is stored in the newly allocated BigInt.
  - The function logs **errors** and **successful computation** if logging is enabled.
  - The caller is responsible for **deallocating** the returned BigInt.

---

### **`BigInt* bigint_or(const BigInt* a, const BigInt* b)`**

- **Purpose**:  
  Computes the **bitwise OR** operation between two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first **BigInt** operand.
  - `b`: A pointer to the second **BigInt** operand.

- **Return**:  
  - A **pointer** to a new **BigInt** representing the result of `a | b`.
  - Returns `NULL` if **memory allocation fails** or if either input is `NULL`.

- **Details**:  
  - The function first **validates** that both input BigInts are not `NULL`.
  - A **new BigInt** is allocated using `bigint_create()`.
  - The **bitwise OR** operation is performed using **GMP's `mpz_ior()`** function.
  - The result is stored in the newly allocated BigInt.
  - The function logs **errors** and **successful computation** if logging is enabled.
  - The caller is responsible for **deallocating** the returned BigInt.

---

### **`BigInt* bigint_xor(const BigInt* a, const BigInt* b)`**

- **Purpose**:  
  Computes the **bitwise XOR** operation between two BigInts.

- **Parameters**:  
  - `a`: A pointer to the first **BigInt** operand.
  - `b`: A pointer to the second **BigInt** operand.

- **Return**:  
  - A **pointer** to a new **BigInt** representing the result of `a ^ b`.
  - Returns `NULL` if **memory allocation fails** or if either input is `NULL`.

- **Details**:  
  - The function first **validates** that both input BigInts are not `NULL`.
  - A **new BigInt** is allocated using `bigint_create()`.
  - The **bitwise XOR** operation is performed using **GMP's `mpz_xor()`** function.
  - The result is stored in the newly allocated BigInt.
  - The function logs **errors** and **successful computation** if logging is enabled.
  - The caller is responsible for **deallocating** the returned BigInt.

---

### Example 1: Creating a BigInt Using `bigint_create`

This example shows how to create a new BigInt (which is initialized to 0) and print its value.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_create();
    if (!a) {
        fmt_fprintf(stderr, "Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }

    char* str = bigint_to_string(a);
    fmt_printf("Example 1 - Created BigInt: %s\n", str);

    free(str);
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 1 - Created BigInt: 0
```

---

### Example 2: Creating a BigInt from a String Using `bigint_from_string`

This example demonstrates how to create a BigInt from a decimal string.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("12345678901234567890");
    if (!a) {
        fmt_fprintf(stderr, "Failed to create BigInt from string.\n");
        return EXIT_FAILURE;
    }

    char* str = bigint_to_string(a);
    fmt_printf("Example 2 - BigInt from string: %s\n", str);

    free(str);
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 2 - BigInt from string: 12345678901234567890
```

---

### Example 3: Copying a BigInt Using `bigint_copy`

This example creates a BigInt from a string, makes a copy using `bigint_copy`, and prints both the original and its copy.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("98765432109876543210");
    if (!a) {
        fmt_fprintf(stderr, "Failed to create BigInt from string.\n");
        return EXIT_FAILURE;
    }

    BigInt* copy = bigint_copy(a);
    if (!copy) {
        fmt_fprintf(stderr, "Failed to copy BigInt.\n");
        bigint_deallocate(a);

        return EXIT_FAILURE;
    }
    char* str_a = bigint_to_string(a);
    char* str_copy = bigint_to_string(copy);

    fmt_printf("Example 3 - Original: %s\n", str_a);
    fmt_printf("Example 3 - Copy:     %s\n", str_copy);

    free(str_a);
    free(str_copy);
    bigint_deallocate(a);
    bigint_deallocate(copy);

    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 3 - Original: 98765432109876543210
Example 3 - Copy:     98765432109876543210
```

---

### Example 4: Adding Two BigInts Using `bigint_add`

This example demonstrates how to add two BigInts and print the result.

```c
#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"

int main(void) {
    BigInt* a = bigint_from_string("123456789");
    BigInt* b = bigint_from_string("987654321");
    if (!a || !b) {
        fmt_fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }

    BigInt* sum = bigint_add(a, b);
    char* str_sum = bigint_to_string(sum);
    fmt_printf("Example 4 - Sum: %s\n", str_sum);

    free(str_sum);
    bigint_deallocate(a);
    bigint_deallocate(b);
    bigint_deallocate(sum);

    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 4 - Sum: 1111111110
```

---

### Example 5: Subtracting Two BigInts Using `bigint_subtract`

This example shows how to subtract one BigInt from another.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("1000000000");
    BigInt* b = bigint_from_string("123456789");

    if (!a || !b) {
        fmt_fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }

    BigInt* diff = bigint_subtract(a, b);
    char* str_diff = bigint_to_string(diff);

    fmt_printf("Example 5 - Difference: %s\n", str_diff);

    free(str_diff);
    bigint_deallocate(a);
    bigint_deallocate(b);
    bigint_deallocate(diff);

    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 5 - Difference: 876543211
```

---

### Example 6: Multiplying Two BigInts Using `bigint_multiply`

This example demonstrates how to multiply two BigInts.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("12345");
    BigInt* b = bigint_from_string("67890");

    if (!a || !b) {
        fmt_fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }

    BigInt* prod = bigint_multiply(a, b);
    char* str_prod = bigint_to_string(prod);

    fmt_printf("Example 6 - Product: %s\n", str_prod);

    free(str_prod);
    bigint_deallocate(a);
    bigint_deallocate(b);
    bigint_deallocate(prod);

    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 6 - Product: 838102050
```

---

### Example 7: Dividing Two BigInts Using `bigint_divide`

This example shows how to perform integer division, obtaining both the quotient and remainder.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("1000000000");
    BigInt* b = bigint_from_string("12345");
    if (!a || !b) {
        fmt_fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }

    BigInt* remainder = NULL;
    BigInt* quot = bigint_divide(a, b, &remainder);
    if (!quot) {
        fmt_fprintf(stderr, "Division failed.\n");

        bigint_deallocate(a);
        bigint_deallocate(b);
        return EXIT_FAILURE;
    }

    char* str_quot = bigint_to_string(quot);
    char* str_rem = bigint_to_string(remainder);

    fmt_printf("Example 7 - Quotient: %s\n", str_quot);
    fmt_printf("Example 7 - Remainder: %s\n", str_rem);

    free(str_quot);
    free(str_rem);
    bigint_deallocate(a);
    bigint_deallocate(b);
    bigint_deallocate(quot);
    bigint_deallocate(remainder);

    return EXIT_SUCCESS;
}
```

**Sample Output:**  
*(The actual numbers depend on the division; for example, you might see something like)*
```
Example 7 - Quotient: 81000
Example 7 - Remainder: 370
```

---

### Example 8: Exponentiation Using `bigint_pow`

This example demonstrates how to raise a BigInt to a power.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* base = bigint_from_string("2");
    BigInt* exp = bigint_from_string("100");

    if (!base || !exp) {
        fmt_fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }

    BigInt* result = bigint_pow(base, exp);
    char* str_result = bigint_to_string(result);

    fmt_printf("Example 8 - 2^100: %s\n", str_result);

    free(str_result);
    bigint_deallocate(base);
    bigint_deallocate(exp);
    bigint_deallocate(result);

    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 8 - 2^100: 1267650600228229401496703205376
```

---

### Example 9: Computing Modulus Using `bigint_mod`

This example shows how to compute the modulus (remainder) of two BigInts.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("1000000000");
    BigInt* b = bigint_from_string("12345");

    if (!a || !b) {
        fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }

    BigInt* mod_result = bigint_mod(a, b);
    char* str_mod = bigint_to_string(mod_result);

    fmt_printf("Example 9 - Modulus: %s\n", str_mod);

    free(str_mod);
    bigint_deallocate(a);
    bigint_deallocate(b);
    bigint_deallocate(mod_result);

    return EXIT_SUCCESS;
}
```

**Sample Output:**  
*(The actual modulus depends on the division; for example, you might see)*
```
Example 9 - Modulus: 370
```

---

### Example 10: Comparing BigInts and Checking for Zero

This example demonstrates how to compare two BigInts using `bigint_compare` and check if a BigInt is zero using `bigint_is_zero`.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("0");
    BigInt* b = bigint_from_string("12345");
    if (!a || !b) {
        fmt_fprintf(stderr, "Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }
    
    // Compare a and b
    int cmp = bigint_compare(a, b);
    if (cmp < 0) {
        fmt_printf("Example 10 - Comparison: a is less than b.\n");
    } 
    else if (cmp == 0) {
        fmt_printf("Example 10 - Comparison: a is equal to b.\n");
    } 
    else {
        fmt_printf("Example 10 - Comparison: a is greater than b.\n");
    }
    
    // Check if a is zero
    if (bigint_is_zero(a)) {
        fmt_printf("Example 10 - BigInt a is zero.\n");
    } else {
        fmt_printf("Example 10 - BigInt a is not zero.\n");
    }
    
    bigint_deallocate(a);
    bigint_deallocate(b);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 10 - Comparison: a is less than b.
Example 10 - BigInt a is zero.
```

---

### Example 11: Modular Exponentiation (`bigint_powmod`)

This example computes \((3^{20}) \mod 17\).

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* base = bigint_from_string("3");
    BigInt* exponent = bigint_from_string("20");
    BigInt* modulus = bigint_from_string("17");
    if (!base || !exponent || !modulus) {
        fmt_fprintf(stderr, "Example 11 - Failed to create one of the BigInts.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* result = bigint_powmod(base, exponent, modulus);
    if (!result) {
        fmt_fprintf(stderr, "Example 11 - Modular exponentiation failed.\n");
    } 
    else {
        char* resStr = bigint_to_string(result);
        fmt_printf("Example 11 - (3^20 mod 17): %s\n", resStr);
        free(resStr);
        bigint_deallocate(result);
    }
    
    bigint_deallocate(base);
    bigint_deallocate(exponent);
    bigint_deallocate(modulus);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 11 - (3^20 mod 17): 13
```

---

### Example 12: Negating a BigInt (`bigint_negate`)

This example computes the negation of a BigInt.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("123456789");
    if (!a) {
        fmt_fprintf(stderr, "Example 12 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* neg = bigint_negate(a);
    if (!neg) {
        fmt_fprintf(stderr, "Example 12 - Negation failed.\n");
    } else {
        char* strNeg = bigint_to_string(neg);
        fmt_printf("Example 12 - Negation of 123456789: %s\n", strNeg);
        free(strNeg);
        bigint_deallocate(neg);
    }
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 12 - Negation of 123456789: -123456789
```

---

### Example 13: Absolute Value (`bigint_abs`)

This example computes the absolute value of a negative BigInt.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("-123456789");
    if (!a) {
        fmt_fprintf(stderr, "Example 13 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* absVal = bigint_abs(a);
    if (!absVal) {
        fmt_fprintf(stderr, "Example 13 - Absolute value computation failed.\n");
    } else {
        char* strAbs = bigint_to_string(absVal);
        fmt_printf("Example 13 - Absolute value of -123456789: %s\n", strAbs);
        free(strAbs);
        bigint_deallocate(absVal);
    }
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 13 - Absolute value of -123456789: 123456789
```

---

### Example 14: Greatest Common Divisor (`bigint_gcd`)

This example computes the GCD of two BigInts.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("48");
    BigInt* b = bigint_from_string("180");
    if (!a || !b) {
        fmt_fprintf(stderr, "Example 14 - Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* gcd = bigint_gcd(a, b);
    if (!gcd) {
        fmt_fprintf(stderr, "Example 14 - GCD computation failed.\n");
    } else {
        char* strGcd = bigint_to_string(gcd);
        fmt_printf("Example 14 - GCD of 48 and 180: %s\n", strGcd);
        free(strGcd);
        bigint_deallocate(gcd);
    }
    
    bigint_deallocate(a);
    bigint_deallocate(b);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 14 - GCD of 48 and 180: 12
```

---

### Example 15: Least Common Multiple (`bigint_lcm`)

This example computes the LCM of two BigInts.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("48");
    BigInt* b = bigint_from_string("180");
    if (!a || !b) {
        fmt_fprintf(stderr, "Example 15 - Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* lcm = bigint_lcm(a, b);
    if (!lcm) {
        fmt_fprintf(stderr, "Example 15 - LCM computation failed.\n");
    } else {
        char* strLcm = bigint_to_string(lcm);
        fmt_printf("Example 15 - LCM of 48 and 180: %s\n", strLcm);
        free(strLcm);
        bigint_deallocate(lcm);
    }
    
    bigint_deallocate(a);
    bigint_deallocate(b);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 15 - LCM of 48 and 180: 720
```

---

### Example 16: Next Prime (`bigint_next_prime`)

This example computes the next prime number greater than a given BigInt.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("100");
    if (!a) {
        fmt_fprintf(stderr, "Example 16 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* nextPrime = bigint_next_prime(a);
    if (!nextPrime) {
        fmt_fprintf(stderr, "Example 16 - Next prime computation failed.\n");
    } else {
        char* strPrime = bigint_to_string(nextPrime);
        fmt_printf("Example 16 - Next prime after 100: %s\n", strPrime);
        free(strPrime);
        bigint_deallocate(nextPrime);
    }
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 16 - Next prime after 100: 101
```

---

### Example 17: Integer Square Root (`bigint_sqrt`)

This example computes the integer square root of a BigInt and prints both the result and the remainder.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("144");
    if (!a) {
        fmt_fprintf(stderr, "Example 17 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* rem = NULL;
    BigInt* root = bigint_sqrt(a, &rem);
    if (!root) {
        fmt_fprintf(stderr, "Example 17 - Square root computation failed.\n");
    } else {
        char* strRoot = bigint_to_string(root);
        char* strRem  = bigint_to_string(rem);
        fmt_printf("Example 17 - Integer square root of 144: %s\n", strRoot);
        fmt_printf("Example 17 - Remainder: %s\n", strRem);
        free(strRoot);
        free(strRem);
        bigint_deallocate(root);
        bigint_deallocate(rem);
    }
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 17 - Integer square root of 144: 12
Example 17 - Remainder: 0
```

---

### Example 18: Generating a Random BigInt (`bigint_random`)

This example generates a random BigInt with 64 bits.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* randomVal = bigint_random(64);
    if (!randomVal) {
        fmt_fprintf(stderr, "Example 18 - Failed to generate random BigInt.\n");
        return EXIT_FAILURE;
    }
    
    char* strRand = bigint_to_string(randomVal);
    fmt_printf("Example 18 - Random 64-bit BigInt: %s\n", strRand);
    free(strRand);
    bigint_deallocate(randomVal);
    return EXIT_SUCCESS;
}
```

**Sample Output:**  
*(Output will vary with each run, for example:)*  
```
Example 18 - Random 64-bit BigInt: 12345678901234567890
```

---

### Example 19: Factorial Calculation (`bigint_factorial`)

This example computes the factorial of 20.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* fact = bigint_factorial(20);
    if (!fact) {
        fmt_fprintf(stderr, "Example 19 - Factorial computation failed.\n");
        return EXIT_FAILURE;
    }
    
    char* strFact = bigint_to_string(fact);
    fmt_printf("Example 19 - Factorial of 20: %s\n", strFact);
    free(strFact);
    bigint_deallocate(fact);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 19 - Factorial of 20: 2432902008176640000
```

---

### Example 20: Primality Test (`bigint_is_prime`)

This example checks the primality of two BigInts, one that is prime and one that is composite.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* primeCandidate = bigint_from_string("97");
    BigInt* compositeCandidate = bigint_from_string("100");
    if (!primeCandidate || !compositeCandidate) {
        fmt_fprintf(stderr, "Example 20 - Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }
    
    int resPrime = bigint_is_prime(primeCandidate, 15);
    int resComposite = bigint_is_prime(compositeCandidate, 15);
    
    fmt_printf("Example 20 - Primality test for 97: %d (2 means definitely prime, 1 means probably prime)\n", resPrime);
    fmt_printf("Example 20 - Primality test for 100: %d (0 means composite)\n", resComposite);
    
    bigint_deallocate(primeCandidate);
    bigint_deallocate(compositeCandidate);
    return EXIT_SUCCESS;
}
```

**Sample Output:**  
*(Values depend on the GMP probabilistic test, but typically you may see)*
```
Example 20 - Primality test for 97: 2 (2 means definitely prime, 1 means probably prime)
Example 20 - Primality test for 100: 0 (0 means composite)
```

### Example 21: Increment a BigInt (`bigint_inc`)

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("100");
    if (!a) {
        fmt_fprintf(stderr, "Example 21 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* incA = bigint_inc(a);
    if (!incA) {
        fmt_fprintf(stderr, "Example 21 - Increment failed.\n");
        bigint_deallocate(a);
        return EXIT_FAILURE;
    }
    
    char* strInc = bigint_to_string(incA);
    fmt_printf("Example 21 - 100 incremented by 1: %s\n", strInc);
    free(strInc);
    
    bigint_deallocate(a);
    bigint_deallocate(incA);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 21 - 100 incremented by 1: 101
```

---

### Example 22: Decrement a BigInt (`bigint_dec`)

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("100");
    if (!a) {
        fmt_fprintf(stderr, "Example 22 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* decA = bigint_dec(a);
    if (!decA) {
        fmt_fprintf(stderr, "Example 22 - Decrement failed.\n");
        bigint_deallocate(a);
        return EXIT_FAILURE;
    }
    
    char* strDec = bigint_to_string(decA);
    fmt_printf("Example 22 - 100 decremented by 1: %s\n", strDec);
    free(strDec);
    
    bigint_deallocate(a);
    bigint_deallocate(decA);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 22 - 100 decremented by 1: 99
```

---

### Example 23: Bit-Length of a BigInt (`bigint_bit_length`)

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("12345678901234567890");
    if (!a) {
        fmt_fprintf(stderr, "Example 23 - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    unsigned long bits = bigint_bit_length(a);
    fmt_printf("Example 23 - Bit-length of 12345678901234567890: %lu\n", bits);
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```

**Sample Output:**  
*(Output will vary; for example, you might see)*
```
Example 23 - Bit-length of 12345678901234567890: 65
```

---

### Example 24: Check if a BigInt is Even (`bigint_is_even`)

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* evenNum = bigint_from_string("123456788");  
    BigInt* oddNum  = bigint_from_string("123456789");  

    if (!evenNum || !oddNum) {
        fmt_fprintf(stderr, "Example 24 - Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }
    
    if (bigint_is_even(evenNum)) {
        fmt_printf("Example 24 - 123456788 is even.\n");
    } 
    else {
        fmt_printf("Example 24 - 123456788 is not even.\n");
    }
    
    if (bigint_is_even(oddNum)) {
        fmt_printf("Example 24 - 123456789 is even.\n");
    } 
    else {
        fmt_printf("Example 24 - 123456789 is not even.\n");
    }
    
    bigint_deallocate(evenNum);
    bigint_deallocate(oddNum);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example 24 - 123456788 is even.
Example 24 - 123456789 is not even.
```

---
### Example 25 : Use modinv 

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("3");
    BigInt* mod = bigint_from_string("11");

    if (!a || !mod) {
        fmt_fprintf(stderr, "Example A - Failed to create BigInts.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* inv = bigint_modinv(a, mod);
    if (!inv) {
        fmt_printf("Example A - Modular inverse does not exist.\n");
    } 
    else {
        char* strInv = bigint_to_string(inv);
        fmt_printf("Example A - Modular inverse of 3 mod 11: %s\n", strInv);
        free(strInv);
        bigint_deallocate(inv);
    }
    
    bigint_deallocate(a);
    bigint_deallocate(mod);
    return EXIT_SUCCESS;
}
```

**Sample Output:**
```
Example A - Modular inverse of 3 mod 11: 4

```

---

### Example 26 : Convert Bigint to Hex Value 

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("123456789");
    if (!a) {
        fmt_fprintf(stderr, "Example C - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    char* hexStr = bigint_to_hex(a);
    if (!hexStr) {
        fmt_fprintf(stderr, "Example C - Hex conversion failed.\n");
    } else {
        fmt_printf("Example C - Hexadecimal representation of 123456789: %s\n", hexStr);
        free(hexStr);
    }
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}
```
**Sample Output:**
```
Example C - Hexadecimal representation of 123456789: 75bcd15

```

---

### Example 27 : Compute the exact square root 

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("144");
    if (!a) {
        fmt_fprintf(stderr, "Example B - Failed to create BigInt.\n");
        return EXIT_FAILURE;
    }
    
    BigInt* sqrtExact = bigint_sqrt_exact(a);
    if (!sqrtExact) {
        fmt_printf("Example B - The number is not a perfect square.\n");
    } else {
        char* strSqrt = bigint_to_string(sqrtExact);
        fmt_printf("Example B - Exact square root of 144: %s\n", strSqrt);
        free(strSqrt);
        bigint_deallocate(sqrtExact);
    }
    
    bigint_deallocate(a);
    return EXIT_SUCCESS;
}

```

---

### Example 28: Below is an example program that demonstrates the usage of the three new functions: `bigint_sum_digits`, `bigint_cmp_abs`, and `bigint_log2`.

```c
#include <stdlib.h>
#include "bigint/bigint.h"
#include "fmt/fmt.h"

int main(void) {
    BigInt* a = bigint_from_string("98765432101234567890");
    if (!a) {
        fmt_fprintf(stderr, "Failed to create BigInt a.\n");
        return EXIT_FAILURE;
    }
    
    unsigned long sumDigits = bigint_sum_digits(a);
    fmt_printf("Sum of digits of a: %lu\n", sumDigits);
    
    BigInt* negA = bigint_negate(a);
    if (!negA) {
        fmt_fprintf(stderr, "Failed to compute negation of a.\n");
        bigint_deallocate(a);
        return EXIT_FAILURE;
    }
    int cmpAbs = bigint_cmp_abs(a, negA);
    fmt_printf("Comparison of |a| and | -a |: %d (0 means equal)\n", cmpAbs);

    unsigned long log2Val = bigint_log2(a);
    fmt_printf("Floor(log2(|a|)): %lu\n", log2Val);
    

    bigint_deallocate(a);
    bigint_deallocate(negA);

    return EXIT_SUCCESS;
}
```

**Sample Output:**  
```
Sum of digits of a: 90
Comparison of |a| and | -a |: 0 (0 means equal)
Floor(log2(|a|)): 66
```

---

## Conclusion

These examples illustrate the use of the BigInt library functions for creating, copying, adding, subtracting, multiplying, dividing, exponentiating, computing the modulus, and comparing arbitrary-precision integers in C. Each example is self-contained and demonstrates proper memory management by deallocating BigInts after use.

## License

This project is open-source and available under the [ISC License](LICENSE).
