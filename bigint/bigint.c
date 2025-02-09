/**
 * @file bigint.c
 * @author amin tahmasebi
 * @brief Implementation of the BigInt library using GMP.
 */

#include "bigint.h"
#include <gmp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/**
 * @brief Creates a new BigInt initialized to zero.
 *
 * Allocates memory for a BigInt and initializes its value to 0.
 *
 * @return Pointer to the new BigInt, or NULL if allocation fails.
 */
BigInt *bigint_create(void)
{
    BigInt *bi = malloc(sizeof(BigInt));
    if (!bi){
        BIGINT_LOG("[bigint_create]: Memory allocation for BigInt failed.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_create]: Memory allocation for BigInt succeeded.");
    }

    mpz_init(bi->value); // Initializes to 0 by default
    BIGINT_LOG("[bigint_create]: Initialized BigInt value to 0.");

    return bi;
}

/**
 * @brief Creates a BigInt from a decimal string.
 *
 * Parses a null-terminated decimal string and converts it into a BigInt.
 *
 * @param str A pointer to the decimal string representation.
 * @return Pointer to the new BigInt, or NULL if the string is invalid or allocation fails.
 */
BigInt *bigint_from_string(const char *str) {
    if (!str) {
        BIGINT_LOG("[bigint_from_string]: Input string is NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_from_string]: Received input string: %s", str);
    }

    BigInt *bi = bigint_create();
    if (!bi) {
        BIGINT_LOG("[bigint_from_string]: Failed to allocate BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_from_string]: BigInt allocated successfully.");
    }

    if (mpz_set_str(bi->value, str, 10) != 0) {
        BIGINT_LOG("[bigint_from_string]: Failed to convert string '%s' to BigInt.", str);
        bigint_deallocate(bi);
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_from_string]: Successfully converted string '%s' to BigInt.", str);
    }

    return bi;
}

/**
 * @brief Creates a copy of an existing BigInt.
 *
 * Allocates a new BigInt and copies the value from the source BigInt.
 *
 * @param src Pointer to the source BigInt.
 * @return Pointer to the newly created BigInt, or NULL if allocation fails.
 */
BigInt *bigint_copy(const BigInt *src) {
    if (!src) {
        BIGINT_LOG("[bigint_copy]: Source BigInt is NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_copy]: Source BigInt is valid.");
    }

    BigInt *bi = bigint_create();
    if (!bi) {
        BIGINT_LOG("[bigint_copy]: Memory allocation failed for copy.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_copy]: Memory allocation for copy succeeded.");
    }

    mpz_set(bi->value, src->value);
    BIGINT_LOG("[bigint_copy]: Copied BigInt value from source.");

    return bi;
}

/**
 * @brief Deallocates a BigInt.
 *
 * Frees all memory associated with the BigInt, including the internal GMP value.
 *
 * @param bi Pointer to the BigInt to deallocate.
 */
void bigint_deallocate(BigInt *bi) {
    if (!bi) {
        BIGINT_LOG("[bigint_deallocate]: Provided BigInt pointer is NULL, nothing to deallocate.");
    }
    else {
        mpz_clear(bi->value);
        free(bi);
        BIGINT_LOG("[bigint_deallocate]: Successfully deallocated BigInt.");
    }
}

/**
 * @brief Converts a BigInt to a decimal string.
 *
 * Converts the internal GMP value to a null-terminated decimal string.
 * The returned string is allocated dynamically and must be freed by the caller.
 *
 * @param bi Pointer to the BigInt to convert.
 * @return Pointer to the string representation, or NULL if bi is NULL.
 */
char *bigint_to_string(const BigInt *bi) {
    if (!bi) {
        BIGINT_LOG("[bigint_to_string]: Provided BigInt pointer is NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_to_string]: Converting BigInt to string.");
    }

    char *str = mpz_get_str(NULL, 10, bi->value);
    if (!str) {
        BIGINT_LOG("[bigint_to_string]: Conversion failed, returned NULL.");
    }
    else {
        BIGINT_LOG("[bigint_to_string]: Conversion succeeded. Result: %s", str);
    }

    return str;
}

/**
 * @brief Adds two BigInts.
 *
 * Computes the sum of two BigInts and returns the result as a new BigInt.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to the new BigInt representing a + b, or NULL on error.
 */
BigInt *bigint_add(const BigInt *a, const BigInt *b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_add]: One or both input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_add]: Both input BigInts are valid.");
    }

    BigInt *res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_add]: Failed to allocate result BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_add]: Allocated result BigInt successfully.");
    }

    mpz_add(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_add]: Successfully added BigInts.");

    return res;
}

/**
 * @brief Subtracts one BigInt from another.
 *
 * Computes the difference between two BigInts (a - b) and returns the result as a new BigInt.
 *
 * @param a Pointer to the minuend BigInt.
 * @param b Pointer to the subtrahend BigInt.
 * @return Pointer to the new BigInt representing a - b, or NULL on error.
 */
BigInt *bigint_subtract(const BigInt *a, const BigInt *b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_subtract]: One or both input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_subtract]: Both input BigInts are valid.");
    }

    BigInt *res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_subtract]: Failed to allocate result BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_subtract]: Allocated result BigInt successfully.");
    }

    mpz_sub(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_subtract]: Successfully subtracted BigInts.");

    return res;
}

/**
 * @brief Multiplies two BigInts.
 *
 * Computes the product of two BigInts and returns the result as a new BigInt.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to the new BigInt representing a * b, or NULL on error.
 */
BigInt *bigint_multiply(const BigInt *a, const BigInt *b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_multiply]: One or both input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_multiply]: Both input BigInts are valid.");
    }

    BigInt *res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_multiply]: Failed to allocate result BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_multiply]: Allocated result BigInt successfully.");
    }

    mpz_mul(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_multiply]: Successfully multiplied BigInts.");

    return res;
}

/**
 * @brief Divides one BigInt by another.
 *
 * Performs integer division of BigInt a by BigInt b and returns the quotient.
 * Optionally, the remainder is stored in the provided pointer.
 *
 * @param a Pointer to the dividend BigInt.
 * @param b Pointer to the divisor BigInt.
 * @param remainder Pointer to a BigInt pointer where the remainder will be stored.
 *                  If NULL, the remainder is discarded.
 * @return Pointer to the new BigInt representing the quotient, or NULL if division by zero occurs.
 */
BigInt *bigint_divide(const BigInt *a, const BigInt *b, BigInt **remainder) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_divide]: One or both input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_divide]: Both input BigInts are valid.");
    }

    if (mpz_cmp_ui(b->value, 0) == 0) {
        BIGINT_LOG("[bigint_divide]: Division by zero attempted.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_divide]: Divisor BigInt is non-zero.");
    }

    BigInt *quot = bigint_create();
    BigInt *rem = bigint_create();

    if (!quot || !rem) {
        BIGINT_LOG("[bigint_divide]: Failed to allocate quotient and/or remainder BigInt.");
        bigint_deallocate(quot);
        bigint_deallocate(rem);
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_divide]: Allocated quotient and remainder BigInts successfully.");
    }

    mpz_tdiv_qr(quot->value, rem->value, a->value, b->value);
    BIGINT_LOG("[bigint_divide]: Successfully divided BigInts (quotient and remainder computed).");

    if (remainder) {
        *remainder = rem;
        BIGINT_LOG("[bigint_divide]: Stored remainder in provided pointer.");
    }
    else {
        BIGINT_LOG("[bigint_divide]: No remainder pointer provided; deallocating remainder.");
        bigint_deallocate(rem);
    }

    return quot;
}

/**
 * @brief Compares two BigInts.
 *
 * Compares the values of two BigInts.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return -1 if a < b, 0 if a == b, and 1 if a > b.
 */
int bigint_compare(const BigInt *a, const BigInt *b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_compare]: One or both input BigInts are NULL.");
        return 0;
    }
    else {
        BIGINT_LOG("[bigint_compare]: Both input BigInts are valid.");
    }

    int cmp = mpz_cmp(a->value, b->value);
    BIGINT_LOG("[bigint_compare]: Compared BigInts, result: %d", cmp);
    return cmp;
}

/**
 * @brief Normalizes the BigInt.
 *
 * Reallocates the internal storage of the BigInt to the minimal number of bits required
 * to represent its value. This helps reclaim any extra allocated memory that is no longer needed.
 *
 * @param bi Pointer to the BigInt to normalize.
 */
void bigint_normalize(BigInt *bi) {
    if (bi == NULL) {
        BIGINT_LOG("[bigint_normalize]: Provided BigInt pointer is NULL.");
        return;
    }
    else {
        BIGINT_LOG("[bigint_normalize]: Normalizing BigInt.");
    }

    // Get the number of bits required to represent the BigInt in base 2.
    mp_bitcnt_t bits = mpz_sizeinbase(bi->value, 2);
    mpz_realloc2(bi->value, bits);

    BIGINT_LOG("[bigint_normalize]: Normalized BigInt to %lu bits", (unsigned long)bits);
}

/**
 * @brief Checks if a BigInt is zero.
 *
 * Determines whether the BigInt is equal to zero.
 *
 * @param bi Pointer to the BigInt to check.
 * @return true if the BigInt is zero, false otherwise.
 */
bool bigint_is_zero(const BigInt *bi) {
    if (!bi) {
        BIGINT_LOG("[bigint_is_zero]: Provided BigInt pointer is NULL.");
        return false;
    }
    else {
        BIGINT_LOG("[bigint_is_zero]: Checking if BigInt is zero.");
    }

    bool isZero = (mpz_cmp_ui(bi->value, 0) == 0);
    BIGINT_LOG("[bigint_is_zero]: BigInt is %szero", isZero ? "" : "not ");
    return isZero;
}

/**
 * @brief Raises a BigInt to a power.
 *
 * Computes the result of raising the base BigInt to the exponent BigInt.
 * Note: The exponent is assumed to be non-negative.
 *
 * @param base Pointer to the base BigInt.
 * @param exponent Pointer to the exponent BigInt.
 * @return Pointer to the new BigInt representing base^exponent, or NULL if exponent is negative or an error occurs.
 */
BigInt *bigint_pow(const BigInt *base, const BigInt *exponent) {
    if (!base || !exponent) {
        BIGINT_LOG("[bigint_pow]: One or both input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_pow]: Both input BigInts are valid.");
    }

    if (mpz_sgn(exponent->value) < 0) {
        BIGINT_LOG("[bigint_pow]: Negative exponent detected; not supported.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_pow]: Exponent is non-negative.");
    }

    BigInt *res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_pow]: Failed to allocate result BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_pow]: Allocated result BigInt successfully.");
    }

    unsigned long exp = mpz_get_ui(exponent->value); // Assumes exponent fits in unsigned long
    mpz_pow_ui(res->value, base->value, exp);
    BIGINT_LOG("[bigint_pow]: Computed power (base^exponent) successfully.");

    return res;
}

/**
 * @brief Computes the modulus of one BigInt by another.
 *
 * Returns the remainder of dividing BigInt a by BigInt b.
 *
 * @param a Pointer to the dividend BigInt.
 * @param b Pointer to the divisor BigInt.
 * @return Pointer to the new BigInt representing a mod b, or NULL if b is zero.
 */
BigInt *bigint_mod(const BigInt *a, const BigInt *b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_mod]: One or both input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_mod]: Both input BigInts are valid.");
    }

    if (mpz_cmp_ui(b->value, 0) == 0) {
        BIGINT_LOG("[bigint_mod]: Modulus by zero attempted.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_mod]: Divisor is non-zero.");
    }

    BigInt *res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_mod]: Failed to allocate result BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_mod]: Allocated result BigInt successfully.");
    }

    mpz_mod(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_mod]: Computed modulus successfully.");

    return res;
}

/**
 * @brief Computes the modular exponentiation.
 *
 * Computes (base^exponent) mod modulus and returns the result as a new BigInt.
 *
 * @param base Pointer to the base BigInt.
 * @param exponent Pointer to the exponent BigInt.
 * @param modulus Pointer to the modulus BigInt. Must be non-zero.
 * @return Pointer to a new BigInt representing (base^exponent) mod modulus, or NULL if any
 *         input is NULL or if modulus is zero.
 */
BigInt *bigint_powmod(const BigInt *base, const BigInt *exponent, const BigInt *modulus) {
    if (!base || !exponent || !modulus) {
        BIGINT_LOG("[bigint_powmod]: One or more input BigInts are NULL.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_powmod]: All input BigInts are valid.");
    }

    if (mpz_cmp_ui(modulus->value, 0) == 0) {
        BIGINT_LOG("[bigint_powmod]: Modulus is zero, cannot compute modular exponentiation.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_powmod]: Modulus is non-zero.");
    }

    BigInt *res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_powmod]: Failed to allocate result BigInt.");
        return NULL;
    }
    else {
        BIGINT_LOG("[bigint_powmod]: Allocated result BigInt successfully.");
    }

    mpz_powm(res->value, base->value, exponent->value, modulus->value);
    BIGINT_LOG("[bigint_powmod]: Computed modular exponentiation successfully.");

    return res;
}

/**
 * @brief Returns the negation of a BigInt.
 *
 * Allocates and returns a new BigInt which is the negative of the input.
 *
 * @param a Pointer to the BigInt to negate.
 * @return Pointer to a new BigInt representing -a, or NULL on error.
 */
BigInt* bigint_negate(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_negate]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_negate]: Input BigInt is valid.");
    }
    
    BigInt* res = bigint_copy(a);
    if (!res) {
        BIGINT_LOG("[bigint_negate]: Failed to allocate copy for negation.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_negate]: Successfully allocated copy for negation.");
    }
    
    mpz_neg(res->value, res->value);
    BIGINT_LOG("[bigint_negate]: Negated BigInt successfully.");
    return res;
}

/**
 * @brief Returns the absolute value of a BigInt.
 *
 * Allocates and returns a new BigInt representing the absolute value of the input.
 *
 * @param a Pointer to the BigInt.
 * @return Pointer to a new BigInt representing |a|, or NULL on error.
 */
BigInt* bigint_abs(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_abs]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_abs]: Input BigInt is valid.");
    }
    
    BigInt* res = bigint_copy(a);
    if (!res) {
        BIGINT_LOG("[bigint_abs]: Failed to allocate copy for absolute value.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_abs]: Successfully allocated copy for absolute value.");
    }
    
    mpz_abs(res->value, res->value);
    BIGINT_LOG("[bigint_abs]: Computed absolute value successfully.");
    return res;
}

/**
 * @brief Computes the greatest common divisor (GCD) of two BigInts.
 *
 * Allocates and returns a new BigInt that is the GCD of the two input BigInts.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to a new BigInt representing gcd(a, b), or NULL on error.
 */
BigInt* bigint_gcd(const BigInt* a, const BigInt* b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_gcd]: One or both input BigInts are NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_gcd]: Both input BigInts are valid.");
    }
    
    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_gcd]: Failed to allocate result BigInt for GCD.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_gcd]: Allocated result BigInt for GCD successfully.");
    }
    
    mpz_gcd(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_gcd]: Computed GCD successfully.");
    return res;
}

/**
 * @brief Computes the least common multiple (LCM) of two BigInts.
 *
 * Allocates and returns a new BigInt that is the LCM of the two input BigInts.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to a new BigInt representing lcm(a, b), or NULL on error.
 */
BigInt* bigint_lcm(const BigInt* a, const BigInt* b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_lcm]: One or both input BigInts are NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_lcm]: Both input BigInts are valid.");
    }
    
    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_lcm]: Failed to allocate result BigInt for LCM.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_lcm]: Allocated result BigInt for LCM successfully.");
    }
    
    mpz_lcm(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_lcm]: Computed LCM successfully.");
    return res;
}

/**
 * @brief Checks whether a BigInt is prime.
 *
 * Uses GMP's probabilistic primality test (mpz_probab_prime_p) with the given number
 * of repetitions to determine primality.
 *
 * @param a Pointer to the BigInt to test.
 * @param reps Number of repetitions (the higher, the more accurate).
 * @return 0 if composite, 1 if probably prime, or 2 if definitely prime.
 */
int bigint_is_prime(const BigInt* a, int reps) {
    if (!a) {
        BIGINT_LOG("[bigint_is_prime]: Input BigInt is NULL.");
        return 0;
    } 
    else {
        BIGINT_LOG("[bigint_is_prime]: Input BigInt is valid.");
    }
    
    int result = mpz_probab_prime_p(a->value, reps);
    BIGINT_LOG("[bigint_is_prime]: Primality test result = %d", result);
    return result;
}

/**
 * @brief Computes the next prime number greater than a given BigInt.
 *
 * Uses GMP's mpz_nextprime function to compute the next prime.
 *
 * @param a Pointer to the BigInt.
 * @return Pointer to a new BigInt representing the next prime, or NULL on error.
 */
BigInt* bigint_next_prime(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_next_prime]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_next_prime]: Input BigInt is valid.");
    }
    
    BigInt* res = bigint_copy(a);
    if (!res) {
        BIGINT_LOG("[bigint_next_prime]: Failed to allocate copy for next prime.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_next_prime]: Successfully allocated copy for next prime.");
    }
    
    mpz_nextprime(res->value, res->value);
    BIGINT_LOG("[bigint_next_prime]: Computed next prime successfully.");
    return res;
}

/**
 * @brief Computes the integer square root of a BigInt.
 *
 * Computes the integer square root (the floor of the true square root) of a BigInt,
 * optionally storing the remainder.
 *
 * @param a Pointer to the BigInt.
 * @param rem Pointer to a BigInt pointer where the remainder will be stored.
 *            If NULL, the remainder is discarded.
 * @return Pointer to a new BigInt representing the integer square root, or NULL on error.
 */
BigInt* bigint_sqrt(const BigInt* a, BigInt** rem) {
    if (!a) {
        BIGINT_LOG("[bigint_sqrt]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_sqrt]: Input BigInt is valid.");
    }
    
    BigInt* root = bigint_create();
    if (!root) {
        BIGINT_LOG("[bigint_sqrt]: Failed to allocate result BigInt for square root.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_sqrt]: Allocated result BigInt for square root successfully.");
    }
    
    BigInt* remainder = NULL;
    if (rem != NULL) {
        remainder = bigint_create();

        if (!remainder) {
            BIGINT_LOG("[bigint_sqrt]: Failed to allocate BigInt for remainder.");
            bigint_deallocate(root);
            return NULL;
        } 
        else {
            BIGINT_LOG("[bigint_sqrt]: Allocated BigInt for remainder successfully.");
        }
    }
    
    mpz_sqrtrem(root->value, (rem != NULL ? remainder->value : NULL), a->value);
    BIGINT_LOG("[bigint_sqrt]: Computed square root successfully.");
    
    if (rem != NULL) {
        *rem = remainder;
        BIGINT_LOG("[bigint_sqrt]: Stored remainder in provided pointer.");
    } 
    else {
        if (remainder != NULL) {
            bigint_deallocate(remainder);
        }
    }
    
    return root;
}

/**
 * @brief Generates a random BigInt with a specified number of bits.
 *
 * Uses GMP's mpz_urandomb function to generate a random BigInt with the given
 * number of bits. A new random state is initialized for each call.
 *
 * @param bits The number of bits for the random BigInt.
 * @return Pointer to a new BigInt containing a random value, or NULL on error.
 */
BigInt* bigint_random(unsigned long bits) {
    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_random]: Failed to allocate result BigInt.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_random]: Allocated result BigInt successfully.");
    }
    
    gmp_randstate_t state;
    gmp_randinit_default(state);
    
    /* Seed the random state with current time */
    unsigned long seed = (unsigned long) time(NULL);
    gmp_randseed_ui(state, seed);
    BIGINT_LOG("[bigint_random]: Random state seeded with %lu.", seed);
    
    mpz_urandomb(res->value, state, bits);
    BIGINT_LOG("[bigint_random]: Generated random BigInt with %lu bits.", bits);
    
    gmp_randclear(state);
    return res;
}

/**
 * @brief Computes the factorial of n.
 *
 * Computes n! (n factorial) iteratively and returns the result as a BigInt.
 *
 * @param n The unsigned long integer whose factorial is to be computed.
 * @return Pointer to a new BigInt representing n!, or NULL on error.
 */
BigInt* bigint_factorial(unsigned long n) {
    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_factorial]: Failed to allocate result BigInt.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_factorial]: Allocated result BigInt successfully.");
    }
    
    /* Initialize result to 1 */
    mpz_set_ui(res->value, 1);
    BIGINT_LOG("[bigint_factorial]: Initialized result to 1.");
    
    for (unsigned long i = 2; i <= n; i++) {
        mpz_mul_ui(res->value, res->value, i);
        BIGINT_LOG("[bigint_factorial]: Multiplied result by %lu.", i);
    }
    
    BIGINT_LOG("[bigint_factorial]: Computed factorial of %lu successfully.", n);
    return res;
}

/**
 * @brief Increments a BigInt by 1.
 *
 * Allocates and returns a new BigInt that is equal to (a + 1).
 *
 * @param a Pointer to the BigInt to increment.
 * @return Pointer to a new BigInt representing (a + 1), or NULL on error.
 */
BigInt* bigint_inc(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_inc]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_inc]: Input BigInt is valid.");
    }
    
    BigInt* res = bigint_copy(a);
    if (!res) {
        BIGINT_LOG("[bigint_inc]: Failed to copy input BigInt.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_inc]: Copied input BigInt successfully.");
    }
    
    mpz_add_ui(res->value, res->value, 1);
    BIGINT_LOG("[bigint_inc]: Incremented BigInt by 1.");
    return res;
}

/**
 * @brief Decrements a BigInt by 1.
 *
 * Allocates and returns a new BigInt that is equal to (a - 1).
 *
 * @param a Pointer to the BigInt to decrement.
 * @return Pointer to a new BigInt representing (a - 1), or NULL on error.
 */
BigInt* bigint_dec(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_dec]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_dec]: Input BigInt is valid.");
    }
    
    BigInt* res = bigint_copy(a);
    if (!res) {
        BIGINT_LOG("[bigint_dec]: Failed to copy input BigInt.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_dec]: Copied input BigInt successfully.");
    }
    
    mpz_sub_ui(res->value, res->value, 1);
    BIGINT_LOG("[bigint_dec]: Decremented BigInt by 1.");
    return res;
}

/**
 * @brief Returns the bit-length of a BigInt.
 *
 * Computes the number of bits required to represent the BigInt in binary.
 *
 * @param a Pointer to the BigInt.
 * @return The number of bits required, or 0 if a is NULL.
 */
unsigned long bigint_bit_length(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_bit_length]: Input BigInt is NULL.");
        return 0;
    } 
    else {
        BIGINT_LOG("[bigint_bit_length]: Input BigInt is valid.");
    }
    
    unsigned long bits = mpz_sizeinbase(a->value, 2);
    BIGINT_LOG("[bigint_bit_length]: Computed bit-length: %lu", bits);
    return bits;
}

/**
 * @brief Checks whether a BigInt is even.
 *
 * Determines if the BigInt is even.
 *
 * @param a Pointer to the BigInt.
 * @return true if the BigInt is even, false otherwise.
 */
bool bigint_is_even(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_is_even]: Input BigInt is NULL.");
        return false;
    } 
    else {
        BIGINT_LOG("[bigint_is_even]: Input BigInt is valid.");
    }
    
    bool even = (mpz_tstbit(a->value, 0) == 0);
    BIGINT_LOG("[bigint_is_even]: BigInt is %s", even ? "even" : "odd");
    return even;
}

/**
 * @brief Computes the modular inverse of a BigInt.
 *
 * Computes the inverse of a modulo modulus such that (a * a_inv) mod modulus = 1.
 *
 * @param a Pointer to the BigInt whose inverse is to be computed.
 * @param modulus Pointer to the modulus BigInt. Must be non-zero.
 * @return Pointer to a new BigInt representing the modular inverse of a modulo modulus,
 *         or NULL if the inverse does not exist or if an error occurs.
 *
 * @details
 * - Validates that neither input is NULL and that modulus is non-zero.
 * - Uses GMP's mpz_invert function to compute the inverse.
 * - Logs all steps of the operation.
 */
BigInt* bigint_modinv(const BigInt* a, const BigInt* modulus) {
    if (!a || !modulus) {
        BIGINT_LOG("[bigint_modinv]: One or both inputs are NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_modinv]: Both inputs are valid.");
    }

    if (mpz_cmp_ui(modulus->value, 0) == 0) {
        BIGINT_LOG("[bigint_modinv]: Modulus is zero; cannot compute inverse.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_modinv]: Modulus is non-zero.");
    }

    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_modinv]: Failed to allocate result BigInt.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_modinv]: Allocated result BigInt successfully.");
    }

    // mpz_invert returns 0 if the inverse does not exist.
    if (mpz_invert(res->value, a->value, modulus->value) == 0) {
        BIGINT_LOG("[bigint_modinv]: Modular inverse does not exist.");
        bigint_deallocate(res);
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_modinv]: Computed modular inverse successfully.");
    }
    return res;
}

/**
 * @brief Computes the exact square root of a BigInt if it is a perfect square.
 *
 * Checks if the given BigInt is a perfect square. If it is, computes and returns its exact
 * square root. Otherwise, returns NULL.
 *
 * @param a Pointer to the BigInt.
 * @return Pointer to a new BigInt representing the exact square root, or NULL if a is not a perfect square.
 *
 * @details
 * - Uses GMP's mpz_perfect_square_p to check if the number is a perfect square.
 * - If true, uses mpz_sqrt to compute the square root.
 * - Logs each step of the operation.
 */
BigInt* bigint_sqrt_exact(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_sqrt_exact]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_sqrt_exact]: Input BigInt is valid.");
    }
    
    if (mpz_perfect_square_p(a->value) == 0) {
        BIGINT_LOG("[bigint_sqrt_exact]: BigInt is not a perfect square.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_sqrt_exact]: BigInt is a perfect square.");
    }
    
    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_sqrt_exact]: Failed to allocate result BigInt.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_sqrt_exact]: Allocated result BigInt successfully.");
    }
    
    mpz_sqrt(res->value, a->value);
    BIGINT_LOG("[bigint_sqrt_exact]: Computed exact square root successfully.");
    return res;
}

/**
 * @brief Converts a BigInt to its hexadecimal string representation.
 *
 * Converts the internal GMP value of the BigInt to a null-terminated hexadecimal string.
 * The returned string is allocated dynamically and must be freed by the caller.
 *
 * @param a Pointer to the BigInt.
 * @return Pointer to the hexadecimal string representation, or NULL if a is NULL.
 *
 * @details
 * - Uses GMP's mpz_get_str with base 16.
 * - Logs the conversion operation.
 */
char* bigint_to_hex(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_to_hex]: Input BigInt is NULL.");
        return NULL;
    } 
    else {
        BIGINT_LOG("[bigint_to_hex]: Converting BigInt to hexadecimal string.");
    }
    
    char* hexStr = mpz_get_str(NULL, 16, a->value);
    if (!hexStr) {
        BIGINT_LOG("[bigint_to_hex]: Conversion failed, returned NULL.");
    } 
    else {
        BIGINT_LOG("[bigint_to_hex]: Conversion succeeded. Result: %s", hexStr);
    }
    
    return hexStr;
}

/**
 * @brief Returns the number of decimal digits of a BigInt.
 *
 * Computes the number of digits required to represent the BigInt in base 10.
 *
 * @param a Pointer to the BigInt.
 * @return The number of decimal digits, or 0 if a is NULL.
 *
 * @details
 * - Uses GMP's mpz_sizeinbase with base 10.
 * - Logs the computed number of digits.
 */
unsigned long bigint_num_digits(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_num_digits]: Input BigInt is NULL.");
        return 0;
    } else {
        BIGINT_LOG("[bigint_num_digits]: Input BigInt is valid.");
    }
    
    unsigned long digits = mpz_sizeinbase(a->value, 10);
    BIGINT_LOG("[bigint_num_digits]: Computed number of decimal digits: %lu", digits);
    return digits;
}

/**
 * @brief Computes the sum of the decimal digits of a BigInt.
 *
 * Converts the BigInt to its decimal string representation and sums up all numeric digits.
 * Non-digit characters, such as a minus sign, are ignored.
 *
 * @param a Pointer to the BigInt.
 * @return The sum of the decimal digits of the BigInt, or 0 if a is NULL.
 *
 * @details
 * - The function converts the BigInt to a string using bigint_to_string.
 * - Iterates over each character to accumulate the sum.
 * - Logs the computed sum.
 */
unsigned long bigint_sum_digits(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_sum_digits]: Input BigInt is NULL.");
        return 0;
    }

    char* str = bigint_to_string(a);
    if (!str) {
        BIGINT_LOG("[bigint_sum_digits]: Conversion to string failed.");
        return 0;
    }

    unsigned long sum = 0;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            sum += (unsigned long)(str[i] - '0');
        }
    }

    BIGINT_LOG("[bigint_sum_digits]: Computed sum of digits: %lu", sum);
    free(str);

    return sum;
}

/**
 * @brief Compares the absolute values of two BigInts.
 *
 * Compares |a| and |b| and returns:
 * - -1 if |a| is less than |b|
 * - 0 if |a| is equal to |b|
 * - 1 if |a| is greater than |b|
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return -1 if |a| < |b|, 0 if |a| == |b|, and 1 if |a| > |b|.
 *
 * @details
 * - Uses GMP's mpz_cmpabs function to compare the absolute values.
 * - Logs the result of the comparison.
 */
int bigint_cmp_abs(const BigInt* a, const BigInt* b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_cmp_abs]: One or both input BigInts are NULL.");
        return 0;
    }
    
    int cmp = mpz_cmpabs(a->value, b->value);
    int normalized;

    if (cmp < 0) {
        normalized = -1;
    } 
    else if (cmp > 0) {
        normalized = 1;
    } 
    else {
        normalized = 0;
    }

    BIGINT_LOG("[bigint_cmp_abs]: Compared absolute values, result: %d", normalized);
    return normalized;
}

/**
 * @brief Computes the floor of the base-2 logarithm of a BigInt.
 *
 * Computes the largest integer L such that 2^L is less than or equal to the absolute value of a.
 * For a value of 0 or if a is NULL, the function returns 0.
 *
 * @param a Pointer to the BigInt.
 * @return The floor of log₂(|a|) (i.e., the number of bits required minus one), or 0 if a is NULL or a is zero.
 *
 * @details
 * - This is computed as bigint_bit_length(a) - 1 for non-zero values.
 * - Logs each step of the computation.
 */
unsigned long bigint_log2(const BigInt* a) {
    if (!a) {
        BIGINT_LOG("[bigint_log2]: Input BigInt is NULL.");
        return 0;
    }
    if (bigint_is_zero(a)) {
        BIGINT_LOG("[bigint_log2]: BigInt is zero, returning 0.");
        return 0;
    }
    unsigned long bits = mpz_sizeinbase(a->value, 2);
    unsigned long log2_val = (bits > 0) ? bits - 1 : 0;
    BIGINT_LOG("[bigint_log2]: Computed floor(log2(|a|)) as %lu", log2_val);

    return log2_val;
}

/**
 * @brief Computes the bitwise AND of two BigInts.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to a new BigInt representing a & b, or NULL on error.
 */
BigInt* bigint_and(const BigInt* a, const BigInt* b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_and]: One or both input BigInts are NULL.");
        return NULL;
    }

    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_and]: Memory allocation failed.");
        return NULL;
    }

    mpz_and(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_and]: Computed bitwise AND successfully.");
    return res;
}

/**
 * @brief Computes the bitwise OR of two BigInts.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to a new BigInt representing a | b, or NULL on error.
 */
BigInt* bigint_or(const BigInt* a, const BigInt* b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_or]: One or both input BigInts are NULL.");
        return NULL;
    }

    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_or]: Memory allocation failed.");
        return NULL;
    }

    mpz_ior(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_or]: Computed bitwise OR successfully.");
    return res;
}

/**
 * @brief Computes the bitwise XOR of two BigInts.
 *
 * @param a Pointer to the first BigInt.
 * @param b Pointer to the second BigInt.
 * @return Pointer to a new BigInt representing a ^ b, or NULL on error.
 */
BigInt* bigint_xor(const BigInt* a, const BigInt* b) {
    if (!a || !b) {
        BIGINT_LOG("[bigint_xor]: One or both input BigInts are NULL.");
        return NULL;
    }

    BigInt* res = bigint_create();
    if (!res) {
        BIGINT_LOG("[bigint_xor]: Memory allocation failed.");
        return NULL;
    }

    mpz_xor(res->value, a->value, b->value);
    BIGINT_LOG("[bigint_xor]: Computed bitwise XOR successfully.");
    return res;
}
