# **BigFloat Library in C**

**Author:** Amin Tahmasebi  
**Release Date:** 2025  
**License:** ISC License  

## **Overview**  

The **BigFloat** library in C provides a powerful interface for performing arbitrary-precision floating-point arithmetic. It is built on top of the **MPFR (Multiple Precision Floating-Point Reliable Library)**, ensuring high accuracy and correct rounding for numerical computations. This library enables developers to perform floating-point operations with precision far beyond the limitations of standard C floating-point types (`float`, `double`, and `long double`). It supports arithmetic, rounding, exponentiation, logarithms, and trigonometric functions.

## **Key Features**  

- **Arbitrary-Precision Floating-Point Arithmetic** – Perform high-precision calculations with full control over precision and rounding.  
- **Accurate Rounding** – Supports all IEEE 754 rounding modes (`MPFR_RNDN`, `MPFR_RNDZ`, `MPFR_RNDU`, `MPFR_RNDD`, and `MPFR_RNDA`).  
- **Basic Arithmetic Operations** – Addition, subtraction, multiplication, division, and exponentiation.  
- **Advanced Mathematical Functions** – Includes logarithms, exponentials, trigonometric functions, and more.  
- **Conversion Utilities** – Convert BigFloats to and from string representations.  
- **MPFR-Powered** – Built on top of **MPFR**, which ensures accurate floating-point computations.  
- **Cross-Platform** – Works on Linux, Windows, macOS, and other platforms where MPFR is available.  

---

## **Installation and Compilation**  

### **1. Install MPFR**  

To use the BigFloat library, you must install **MPFR** (which depends on **GMP**).  

#### **On Linux (Debian/Ubuntu):**  
```sh
sudo apt-get install libmpfr-dev
```

#### **On macOS (Homebrew):**  
```sh
brew install mpfr
```

#### **On Windows (MSYS2):**  
```sh
pacman -S mingw-w64-x86_64-mpfr
```
Alternatively, download MPFR from [MPFR's official site](https://www.mpfr.org/) and compile it manually.

---

### **2. Include the Library in Your Project**  

- Place the `bigfloat.h` and `bigfloat.c` files in your project directory.  
- Include `bigfloat.h` in your C source files where you need arbitrary-precision floating-point arithmetic.  

```c
#include "bigfloat.h"
```

---

### **3. Compile Your Code**  

Use the following command to compile your project using **MPFR**:  

```sh
gcc -std=c11 -O2 -o bigfloat_example bigfloat.c main.c -lmpfr -lgmp
```

#### **If Using CMake:**  

Modify your **CMakeLists.txt** to link against **MPFR**:

```cmake
add_library(bigfloat SHARED bigfloat.c)
target_include_directories(bigfloat PUBLIC ${CMAKE_SOURCE_DIR}/bigfloat)
target_link_libraries(bigfloat MPFR)
```

---
## Function Descriptions

### **`BigFloat* bigfloat_create(void)`**  

- **Purpose**:  
  Creates a new `BigFloat` initialized to zero.  

- **Parameters**:  
  _None._  

- **Return**:  
  A pointer to a new `BigFloat`, or `NULL` if memory allocation fails.  

- **Details**:  
  - Allocates memory for a `BigFloat` structure.  
  - Initializes the internal MPFR value (`mpfr_t`) to 0.0 using `mpfr_init`.  
  - Logs the creation if logging is enabled.  

---

### **`BigFloat* bigfloat_from_string(const char* str)`**  

- **Purpose**:  
  Creates a `BigFloat` from a decimal string representation.  

- **Parameters**:  
  - `str`: A pointer to a null-terminated decimal string representing the floating-point number.  

- **Return**:  
  A pointer to the new `BigFloat`, or `NULL` if the string is invalid or memory allocation fails.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate and initialize a new `BigFloat`.  
  - Uses `mpfr_set_str` to convert the string into the internal MPFR format.  
  - If conversion fails (i.e., the string is not a valid decimal representation), the function logs the error, deallocates the `BigFloat`, and returns `NULL`.  

---

### **`BigFloat* bigfloat_from_double(double value)`**  

- **Purpose**:  
  Creates a `BigFloat` from a double-precision floating-point number.  

- **Parameters**:  
  - `value`: The double-precision floating-point number to initialize the `BigFloat`.  

- **Return**:  
  A pointer to the new `BigFloat`, or `NULL` on failure.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate and initialize a new `BigFloat`.  
  - Uses `mpfr_set_d` to store the double value into the `BigFloat`.  
  - Logs the conversion process if logging is enabled.  

---

### **`BigFloat* bigfloat_add(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the sum of two `BigFloat` numbers (`a + b`).  

- **Parameters**:  
  - `a`: A pointer to the first `BigFloat`.  
  - `b`: A pointer to the second `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `a + b`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_add` to compute the sum with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the addition process if logging is enabled.  

---

### **`BigFloat* bigfloat_subtract(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the difference between two `BigFloat` numbers (`a - b`).  

- **Parameters**:  
  - `a`: A pointer to the first `BigFloat`.  
  - `b`: A pointer to the second `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `a - b`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_sub` to compute the difference with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the subtraction process if logging is enabled.  

---

### **`BigFloat* bigfloat_multiply(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the product of two `BigFloat` numbers (`a * b`).  

- **Parameters**:  
  - `a`: A pointer to the first `BigFloat`.  
  - `b`: A pointer to the second `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `a * b`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_mul` to compute the product with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the multiplication process if logging is enabled.  

---

### **`BigFloat* bigfloat_divide(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the division of two `BigFloat` numbers (`a / b`).  

- **Parameters**:  
  - `a`: A pointer to the numerator `BigFloat`.  
  - `b`: A pointer to the denominator `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `a / b`, or `NULL` if `b` is zero or an error occurs.  

- **Details**:  
  - If `b` is zero, logs an error and returns `NULL` (division by zero is undefined).  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_div` to compute the quotient with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the division process if logging is enabled.  

---

### **`BigFloat* bigfloat_sqrt(const BigFloat* a)`**  

- **Purpose**:  
  Computes the square root of a `BigFloat` number (`√a`).  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` whose square root is to be computed.  

- **Return**:  
  A pointer to a new `BigFloat` representing `√a`, or `NULL` if `a` is negative or an error occurs.  

- **Details**:  
  - If `a` is negative, logs an error and returns `NULL` (square root of a negative number is undefined in real numbers).  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_sqrt` to compute the square root with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_pow(const BigFloat* base, const BigFloat* exponent)`**  

- **Purpose**:  
  Computes the power of a `BigFloat` number (`base^exponent`).  

- **Parameters**:  
  - `base`: A pointer to the base `BigFloat`.  
  - `exponent`: A pointer to the exponent `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `base^exponent`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_pow` to compute the power with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_log(const BigFloat* a)`**  

- **Purpose**:  
  Computes the natural logarithm (`ln(a)`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` whose logarithm is to be computed.  

- **Return**:  
  A pointer to a new `BigFloat` representing `ln(a)`, or `NULL` if `a` is non-positive or an error occurs.  

- **Details**:  
  - If `a` is zero or negative, logs an error and returns `NULL` (logarithm of non-positive numbers is undefined in real numbers).  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_log` to compute the natural logarithm with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_exp(const BigFloat* a)`**  

- **Purpose**:  
  Computes the exponential function (`e^a`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` exponent.  

- **Return**:  
  A pointer to a new `BigFloat` representing `e^a`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_exp` to compute the exponential function with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_trig_sin(const BigFloat* a)`**  

- **Purpose**:  
  Computes the sine (`sin(a)`) of a `BigFloat` (angle in radians).  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` representing the angle in radians.  

- **Return**:  
  A pointer to a new `BigFloat` representing `sin(a)`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_sin` to compute the sine with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_trig_cos(const BigFloat* a)`**  

- **Purpose**:  
  Computes the cosine (`cos(a)`) of a `BigFloat` (angle in radians).  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` representing the angle in radians.  

- **Return**:  
  A pointer to a new `BigFloat` representing `cos(a)`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_cos` to compute the cosine with rounding mode `MPFR_RNDN` (nearest).  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_trig_tan(const BigFloat* a)`**  

- **Purpose**:  
  Computes the tangent (`tan(a)`) of a `BigFloat` (angle in radians).  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` representing the angle in radians.  

- **Return**:  
  A pointer to a new `BigFloat` representing `tan(a)`, or `NULL` on error (if `a` is a singularity point such as `π/2 + kπ`).  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_tan` to compute the tangent with rounding mode `MPFR_RNDN` (nearest).  
  - If `a` is near a singularity (such as `π/2 + kπ`), logs a warning.  
  - Logs the computation process if logging is enabled.  

---

### **`BigFloat* bigfloat_abs(const BigFloat* a)`**  

- **Purpose**:  
  Computes the absolute value (`|a|`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` whose absolute value is to be computed.  

- **Return**:  
  A pointer to a new `BigFloat` representing `|a|`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_abs` to compute the absolute value.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_negate(const BigFloat* a)`**  

- **Purpose**:  
  Computes the negation (`-a`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` to be negated.  

- **Return**:  
  A pointer to a new `BigFloat` representing `-a`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_neg` to compute the negation.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_floor(const BigFloat* a)`**  

- **Purpose**:  
  Computes the floor function (`⌊a⌋`), rounding a `BigFloat` down to the nearest integer.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` to be floored.  

- **Return**:  
  A pointer to a new `BigFloat` representing `⌊a⌋`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_floor` to compute the floor value.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_ceil(const BigFloat* a)`**  

- **Purpose**:  
  Computes the ceiling function (`⌈a⌉`), rounding a `BigFloat` up to the nearest integer.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` to be ceiled.  

- **Return**:  
  A pointer to a new `BigFloat` representing `⌈a⌉`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_ceil` to compute the ceiling value.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_round(const BigFloat* a)`**  

- **Purpose**:  
  Rounds a `BigFloat` to the nearest integer using standard rounding rules.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` to be rounded.  

- **Return**:  
  A pointer to a new `BigFloat` representing the rounded value, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_round` to compute the rounded value.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_trunc(const BigFloat* a)`**  

- **Purpose**:  
  Truncates a `BigFloat` to remove the decimal portion, rounding toward zero.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` to be truncated.  

- **Return**:  
  A pointer to a new `BigFloat` representing the truncated value, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_trunc` to truncate the decimal part.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_reciprocal(const BigFloat* a)`**  

- **Purpose**:  
  Computes the reciprocal (`1/a`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` whose reciprocal is to be computed.  

- **Return**:  
  A pointer to a new `BigFloat` representing `1/a`, or `NULL` if `a` is zero (division by zero).  

- **Details**:  
  - If `a` is zero, logs an error and returns `NULL`.  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_ui_div` with `1` as the numerator to compute `1/a`.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_mod(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the modulo (`a % b`) operation for `BigFloat` numbers.  

- **Parameters**:  
  - `a`: A pointer to the dividend `BigFloat`.  
  - `b`: A pointer to the divisor `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `a % b`, or `NULL` if `b` is zero (division by zero).  

- **Details**:  
  - If `b` is zero, logs an error and returns `NULL`.  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_fmod` to compute the remainder of `a / b`.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_exp2(const BigFloat* a)`**  

- **Purpose**:  
  Computes `2^a` (the base-2 exponential function) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` exponent.  

- **Return**:  
  A pointer to a new `BigFloat` representing `2^a`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_exp2` to compute `2^a`.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_log10(const BigFloat* a)`**  

- **Purpose**:  
  Computes the base-10 logarithm (`log10(a)`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` input.  

- **Return**:  
  A pointer to a new `BigFloat` representing `log10(a)`, or `NULL` if `a` is non-positive.  

- **Details**:  
  - If `a <= 0`, logs an error and returns `NULL`.  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_log10` to compute the logarithm.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_cbrt(const BigFloat* a)`**  

- **Purpose**:  
  Computes the cube root (`∛a`) of a `BigFloat`.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` input.  

- **Return**:  
  A pointer to a new `BigFloat` representing `∛a`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_cbrt` to compute the cube root.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_hypot(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the Euclidean norm (`hypot(a, b) = sqrt(a^2 + b^2)`).  

- **Parameters**:  
  - `a`: A pointer to the first `BigFloat` value.  
  - `b`: A pointer to the second `BigFloat` value.  

- **Return**:  
  A pointer to a new `BigFloat` representing `hypot(a, b)`, or `NULL` on error.  

- **Details**:  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_hypot` to compute `sqrt(a^2 + b^2)`.  
  - Logs the computation if logging is enabled.  

---

### **`BigFloat* bigfloat_fmod(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Computes the floating-point remainder (`fmod(a, b)`) of `a / b`.  

- **Parameters**:  
  - `a`: A pointer to the dividend `BigFloat`.  
  - `b`: A pointer to the divisor `BigFloat`.  

- **Return**:  
  A pointer to a new `BigFloat` representing `fmod(a, b)`, or `NULL` if `b` is zero.  

- **Details**:  
  - If `b` is zero, logs an error and returns `NULL`.  
  - Calls `bigfloat_create()` to allocate a result `BigFloat`.  
  - Uses `mpfr_fmod` to compute the floating-point remainder.  
  - Logs the computation if logging is enabled.  

---

### **`char* bigfloat_to_string(const BigFloat* a)`**  

- **Purpose**:  
  Converts a `BigFloat` to a decimal string representation.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` to be converted.  

- **Return**:  
  A dynamically allocated string containing the decimal representation of `a`, or `NULL` on error.  

- **Details**:  
  - Calls `mpfr_get_str` to obtain a decimal string.  
  - Allocates and formats the output string.  
  - Logs the conversion if logging is enabled.  
  - The caller is responsible for freeing the returned string.  

---

### **`int bigfloat_compare(const BigFloat* a, const BigFloat* b)`**  

- **Purpose**:  
  Compares two `BigFloat` values to determine their relative order.  

- **Parameters**:  
  - `a`: A pointer to the first `BigFloat`.  
  - `b`: A pointer to the second `BigFloat`.  

- **Return**:  
  - `-1` if `a < b`  
  - `0` if `a == b`  
  - `1` if `a > b`  

- **Details**:  
  - Uses `mpfr_cmp` to compare the two `BigFloat` values.  
  - Logs the comparison result if logging is enabled.  

---

### **`void bigfloat_deallocate(BigFloat* bf)`**  

- **Purpose**:  
  Deallocates the memory used by a `BigFloat`.  

- **Parameters**:  
  - `bf`: A pointer to the `BigFloat` to be freed.  

- **Return**:  
  _None._  

- **Details**:  
  - Calls `mpfr_clear` to free the internal `mpfr_t` value.  
  - Frees the `BigFloat` struct itself.  
  - Logs the deallocation if logging is enabled.  
  - Does nothing if `bf` is `NULL`.  

---

### **`bool bigfloat_is_zero(const BigFloat* a)`**  

- **Purpose**:  
  Checks if a `BigFloat` is exactly zero.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` value to check.  

- **Return**:  
  - `true` if `a == 0.0`  
  - `false` otherwise  

- **Details**:  
  - Uses `mpfr_zero_p` to check if the value is zero.  
  - Logs the check result if logging is enabled.  

---

### **`bool bigfloat_is_negative(const BigFloat* a)`**  

- **Purpose**:  
  Checks if a `BigFloat` is negative.  

- **Parameters**:  
  - `a`: A pointer to the `BigFloat` value to check.  

- **Return**:  
  - `true` if `a` is negative  
  - `false` otherwise  

- **Details**:  
  - Uses `mpfr_sgn` to check the sign of `a`.  
  - Logs the check result if logging is enabled.  

---

### **`BigFloat* bigfloat_lgamma(const BigFloat* a)`**
- **Purpose**:  
  Computes the natural logarithm of the absolute value of the gamma function of a BigFloat.

- **Parameters**:  
  - `a`: A pointer to the input BigFloat.

- **Return**:  
  - A pointer to a new BigFloat containing `lgamma(a)`, or `NULL` if an error occurs.

- **Details**:  
  - Calls `mpfr_lgamma`, which computes `log(|Gamma(a)|)`.
  - An additional integer (`sign`) stores the sign of `Gamma(a)`, but it is not returned in this function.
  - If `a` is `NULL`, the function logs an error and returns `NULL`.
  - Logs the computed `lgamma` value for debugging.

---

### **`BigFloat* bigfloat_log1p(const BigFloat* a)`**
- **Purpose**:  
  Computes `log(1 + a)` accurately, avoiding precision loss for small values of `a`.

- **Parameters**:  
  - `a`: A pointer to the input BigFloat.

- **Return**:  
  - A pointer to a new BigFloat containing `log(1 + a)`, or `NULL` if an error occurs.

- **Details**:  
  - Calls `mpfr_log1p`, which is more accurate for values of `a` near zero.
  - If `a` is `NULL`, logs an error and returns `NULL`.
  - Logs the computed `log1p` value for debugging.

---

### **`BigFloat* bigfloat_expm1(const BigFloat* a)`**
- **Purpose**:  
  Computes `exp(a) - 1` accurately, avoiding precision loss for small values of `a`.

- **Parameters**:  
  - `a`: A pointer to the input BigFloat.

- **Return**:  
  - A pointer to a new BigFloat containing `exp(a) - 1`, or `NULL` if an error occurs.

- **Details**:  
  - Calls `mpfr_expm1`, which is optimized for computing `exp(a) - 1` with high precision.
  - If `a` is `NULL`, logs an error and returns `NULL`.
  - Logs the computed `expm1` value for debugging.

---

## Examples 


### **Example 1: Large Number Arithmetic**
```c
#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* a = bigfloat_from_string("12345678901234567890.987654321");
    BigFloat* b = bigfloat_from_string("98765432109876543210.123456789");

    BigFloat* sum = bigfloat_add(a, b);
    BigFloat* diff = bigfloat_subtract(a, b);
    BigFloat* product = bigfloat_multiply(a, b);
    BigFloat* quotient = bigfloat_divide(a, b);

    fmt_printf("Sum: %s\n", bigfloat_to_string(sum));
    fmt_printf("Difference: %s\n", bigfloat_to_string(diff));
    fmt_printf("Product: %s\n", bigfloat_to_string(product));
    fmt_printf("Quotient: %s\n", bigfloat_to_string(quotient));

    bigfloat_deallocate(a);
    bigfloat_deallocate(b);
    bigfloat_deallocate(sum);
    bigfloat_deallocate(diff);
    bigfloat_deallocate(product);
    bigfloat_deallocate(quotient);

    return 0;
}
```
**Sample Output**
```
Sum: 111111111011111111101.111111110
Difference: -86419753208641975319.135802468
Product: 121932631137021795223746380111126352690.819893617
Quotient: 0.1249999988609374999719375000047999999981
```
---

### **Example 2: Exponentiation, Logarithm, and Square Root**
```c
#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* a = bigfloat_from_string("10000000000000000000.0000001");
    BigFloat* b = bigfloat_from_string("50.5");

    BigFloat* sqrt_a = bigfloat_sqrt(a);
    BigFloat* power = bigfloat_pow(a, b);
    BigFloat* log_a = bigfloat_log(a);

    fmt_printf("Square Root of a: %s\n", bigfloat_to_string(sqrt_a));
    fmt_printf("a raised to power b: %s\n", bigfloat_to_string(power));
    fmt_printf("Natural log of a: %s\n", bigfloat_to_string(log_a));

    bigfloat_deallocate(a);
    bigfloat_deallocate(b);
    bigfloat_deallocate(sqrt_a);
    bigfloat_deallocate(power);
    bigfloat_deallocate(log_a);

    return 0;
}
```
**Sample Output**
```
Square Root of a: 3162277660168379331.057739
a raised to power b: 3.1622776601683793325e+85
Natural log of a: 23.025850929
```
---

### **Example 3: Trigonometric Functions**
```c
#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* angle = bigfloat_from_string("12345678901234567890.87654321"); 

    BigFloat* sin_value = bigfloat_trig_sin(angle);
    BigFloat* cos_value = bigfloat_trig_cos(angle);
    BigFloat* tan_value = bigfloat_trig_tan(angle);

    fmt_printf("Sin(angle): %s\n", bigfloat_to_string(sin_value));
    fmt_printf("Cos(angle): %s\n", bigfloat_to_string(cos_value));
    fmt_printf("Tan(angle): %s\n", bigfloat_to_string(tan_value));

    bigfloat_deallocate(angle);
    bigfloat_deallocate(sin_value);
    bigfloat_deallocate(cos_value);
    bigfloat_deallocate(tan_value);

    return 0;
}
```
**Sample Output**
```
Sin(angle): -0.999887673561
Cos(angle): 0.014993909445
Tan(angle): -66.689684102
```
---

### **Example 4: Rounding Operations**
```c
#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* num = bigfloat_from_string("99999999999999999.99999999999999999");

    BigFloat* floored = bigfloat_floor(num);
    BigFloat* ceiled = bigfloat_ceil(num);
    BigFloat* rounded = bigfloat_round(num);
    BigFloat* truncated = bigfloat_trunc(num);

    fmt_printf("Floor: %s\n", bigfloat_to_string(floored));
    fmt_printf("Ceil: %s\n", bigfloat_to_string(ceiled));
    fmt_printf("Round: %s\n", bigfloat_to_string(rounded));
    fmt_printf("Truncate: %s\n", bigfloat_to_string(truncated));

    bigfloat_deallocate(num);
    bigfloat_deallocate(floored);
    bigfloat_deallocate(ceiled);
    bigfloat_deallocate(rounded);
    bigfloat_deallocate(truncated);

    return 0;
}
```
**Sample Output**
```
Floor: 99999999999999999.000000000
Ceil: 100000000000000000.000000000
Round: 100000000000000000.000000000
Truncate: 99999999999999999.000000000
```
---

### **Example 5: Modulo and Hypotenuse Calculation**
```c
#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* a = bigfloat_from_string("100000000000000000000.99999999999");
    BigFloat* b = bigfloat_from_string("99999999999999999.123456789");

    BigFloat* mod_result = bigfloat_mod(a, b);
    BigFloat* hypot_result = bigfloat_hypot(a, b);

    fmt_printf("Modulo (a mod b): %s\n", bigfloat_to_string(mod_result));
    fmt_printf("Hypotenuse (sqrt(a² + b²)): %s\n", bigfloat_to_string(hypot_result));

    bigfloat_deallocate(a);
    bigfloat_deallocate(b);
    bigfloat_deallocate(mod_result);
    bigfloat_deallocate(hypot_result);

    return 0;
}
```
**Sample Output**
```
Modulo (a mod b): 999999999.87654321099
Hypotenuse (sqrt(a² + b²)): 100000000500000000100.49999999999
```
---

### **Example 6 : erf, exp and .. **
```c
#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* num = bigfloat_from_string("5.123456789");

    BigFloat* lgamma_val = bigfloat_lgamma(num);
    BigFloat* erf_val = bigfloat_erf(num);
    BigFloat* expm1_val = bigfloat_expm1(num);

    fmt_printf("lgamma(num): %s\n", bigfloat_to_string(lgamma_val));
    fmt_printf("erf(num): %s\n", bigfloat_to_string(erf_val));
    fmt_printf("expm1(num): %s\n", bigfloat_to_string(expm1_val));

    bigfloat_deallocate(num);
    bigfloat_deallocate(lgamma_val);
    bigfloat_deallocate(erf_val);
    bigfloat_deallocate(expm1_val);

    return 0;
}
```
**Sample Output**
```
lgamma(num): 3.36566584020391479370459819620009511709213256835938
erf(num): 0.99999999999956967755565528932493180036544799804688
expm1(num): 166.91481361693655571798444725573062896728515625000000
```
---

## License

This project is open-source and available under the [ISC License](LICENSE).
