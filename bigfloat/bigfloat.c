#include "bigfloat.h"
#include <stdlib.h>

/**
 * @brief Creates a new BigFloat initialized to 0.0.
 * @return Pointer to the new BigFloat, or NULL if allocation fails.
 */
BigFloat* bigfloat_create(void) {
    BigFloat* bf = malloc(sizeof(BigFloat));
    if (!bf) {
        BIGFLOAT_LOG("[bigfloat_create]: Memory allocation failed.");
        return NULL;
    }

    mpfr_init(bf->value); // Default is 0.0
    BIGFLOAT_LOG("[bigfloat_create]: Initialized BigFloat to 0.0.");

    return bf;
}

/**
 * @brief Creates a BigFloat from a string.
 * @param str A pointer to the string representation.
 * @return Pointer to the new BigFloat, or NULL on error.
 */
BigFloat* bigfloat_from_string(const char* str) {
    if (!str) {
        BIGFLOAT_LOG("[bigfloat_from_string]: Input string is NULL.");
        return NULL;
    }

    BigFloat* bf = bigfloat_create();
    if (!bf) {
        return NULL;
    }

    if (mpfr_set_str(bf->value, str, 10, MPFR_RNDN) != 0) {
        BIGFLOAT_LOG("[bigfloat_from_string]: Failed to convert string '%s' to BigFloat.", str);
        bigfloat_deallocate(bf);
        return NULL;
    }

    BIGFLOAT_LOG("[bigfloat_from_string]: Converted string '%s' to BigFloat.", str);
    return bf;
}

/**
 * @brief Creates a BigFloat from a double.
 * @param value The double value.
 * @return Pointer to the new BigFloat.
 */
BigFloat* bigfloat_from_double(double value) {
    BigFloat* bf = bigfloat_create();
    if (!bf) {
        return NULL;
    }

    mpfr_set_d(bf->value, value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_from_double]: Created BigFloat from double: %f", value);

    return bf;
}

/**
 * @brief Frees the memory of a BigFloat.
 * @param bf Pointer to the BigFloat to deallocate.
 */
void bigfloat_deallocate(BigFloat* bf) {
    if (!bf) {
        return;
    }

    mpfr_clear(bf->value);
    free(bf);
    BIGFLOAT_LOG("[bigfloat_deallocate]: Deallocated BigFloat.");
}

/**
 * @brief Adds two BigFloat numbers.
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return Pointer to a new BigFloat representing a + b, or NULL on error.
 */
BigFloat* bigfloat_add(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_add]: One or both input BigFloats are NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_add(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_add]: Computed addition successfully.");

    return res;
}

/**
 * @brief Subtracts two BigFloat numbers.
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return Pointer to a new BigFloat representing a - b, or NULL on error.
 */
BigFloat* bigfloat_subtract(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_subtract]: One or both input BigFloats are NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_sub(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_subtract]: Computed subtraction successfully.");

    return res;
}

/**
 * @brief Multiplies two BigFloat numbers.
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return Pointer to a new BigFloat representing a * b, or NULL on error.
 */
BigFloat* bigfloat_multiply(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_multiply]: One or both input BigFloats are NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_mul(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_multiply]: Computed multiplication successfully.");

    return res;
}

/**
 * @brief Divides two BigFloat numbers.
 * @param a Pointer to the first BigFloat (numerator).
 * @param b Pointer to the second BigFloat (denominator).
 * @return Pointer to a new BigFloat representing a / b, or NULL on error.
 */
BigFloat* bigfloat_divide(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_divide]: One or both input BigFloats are NULL.");
        return NULL;
    }
    if (mpfr_cmp_ui(b->value, 0) == 0) {
        BIGFLOAT_LOG("[bigfloat_divide]: Division by zero error.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_div(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_divide]: Computed division successfully.");

    return res;
}

/**
 * @brief Computes the square root of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return Pointer to a new BigFloat representing sqrt(a), or NULL on error.
 */
BigFloat* bigfloat_sqrt(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_sqrt]: Input BigFloat is NULL.");
        return NULL;
    }

    if (mpfr_sgn(a->value) < 0) {
        BIGFLOAT_LOG("[bigfloat_sqrt]: Square root of negative number is not defined.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_sqrt(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_sqrt]: Computed square root successfully.");

    return res;
}

/**
 * @brief Computes the power of a BigFloat raised to another BigFloat.
 * @param base Pointer to the base BigFloat.
 * @param exponent Pointer to the exponent BigFloat.
 * @return Pointer to a new BigFloat representing base^exponent, or NULL on error.
 */
BigFloat* bigfloat_pow(const BigFloat* base, const BigFloat* exponent) {
    if (!base || !exponent) {
        BIGFLOAT_LOG("[bigfloat_pow]: One or both input BigFloats are NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_pow(res->value, base->value, exponent->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_pow]: Computed power successfully.");

    return res;
}

/**
 * @brief Computes the natural logarithm (ln) of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return Pointer to a new BigFloat representing ln(a), or NULL on error.
 */
BigFloat* bigfloat_log(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_log]: Input BigFloat is NULL.");
        return NULL;
    }

    if (mpfr_sgn(a->value) <= 0) {
        BIGFLOAT_LOG("[bigfloat_log]: Logarithm of non-positive number is undefined.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_log(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_log]: Computed natural logarithm successfully.");

    return res;
}

/**
 * @brief Computes the exponential function (e^a) of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return Pointer to a new BigFloat representing e^a, or NULL on error.
 */
BigFloat* bigfloat_exp(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_exp]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_exp(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_exp]: Computed exponential function successfully.");

    return res;
}

/**
 * @brief Computes the sine of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return Pointer to a new BigFloat representing sin(a), or NULL on error.
 */
BigFloat* bigfloat_trig_sin(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_trig_sin]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_sin(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_trig_sin]: Computed sine successfully.");

    return res;
}

/**
 * @brief Computes the cosine of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return Pointer to a new BigFloat representing cos(a), or NULL on error.
 */
BigFloat* bigfloat_trig_cos(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_trig_cos]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_cos(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_trig_cos]: Computed cosine successfully.");

    return res;
}

/**
 * @brief Computes the tangent of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return Pointer to a new BigFloat representing tan(a), or NULL on error.
 */
BigFloat* bigfloat_trig_tan(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_trig_tan]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_tan(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_trig_tan]: Computed tangent successfully.");

    return res;
}

/**
 * @brief Compares two BigFloats.
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return 
 *      - `-1` if a < b
 *      - `0` if a == b
 *      - `1` if a > b
 */
int bigfloat_compare(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_compare]: One or both input BigFloats are NULL.");
        return 0; // Defaulting to equal if invalid input
    }

    int cmp_result = mpfr_cmp(a->value, b->value);
    BIGFLOAT_LOG("[bigfloat_compare]: Comparison result = %d", cmp_result);

    return cmp_result;
}

/**
 * @brief Checks if a BigFloat is zero.
 * @param a Pointer to the BigFloat.
 * @return `true` if the BigFloat is exactly 0, `false` otherwise.
 */
bool bigfloat_is_zero(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_is_zero]: Input BigFloat is NULL.");
        return false;
    }

    bool result = (mpfr_cmp_ui(a->value, 0) == 0);
    BIGFLOAT_LOG("[bigfloat_is_zero]: The number is %s.", result ? "zero" : "non-zero");

    return result;
}

/**
 * @brief Checks if a BigFloat is negative.
 * @param a Pointer to the BigFloat.
 * @return `true` if the BigFloat is negative, `false` otherwise.
 */
bool bigfloat_is_negative(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_is_negative]: Input BigFloat is NULL.");
        return false;
    }

    bool result = (mpfr_sgn(a->value) < 0);
    BIGFLOAT_LOG("[bigfloat_is_negative]: The number is %s.", result ? "negative" : "non-negative");

    return result;
}

/**
 * @brief Converts a BigFloat to a string representation.
 * @param a Pointer to the BigFloat.
 * @return A dynamically allocated string representing the BigFloat, or NULL on error.
 */
char* bigfloat_to_string(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_to_string]: Input BigFloat is NULL.");
        return NULL;
    }

    // Allocate a buffer for string representation
    size_t buf_size = 128; 
    char* str = malloc(buf_size);
    if (!str) {
        BIGFLOAT_LOG("[bigfloat_to_string]: Memory allocation failed.");
        return NULL;
    }

    mpfr_sprintf(str, "%.50Rf", a->value);  // 50 decimal places precision
    BIGFLOAT_LOG("[bigfloat_to_string]: Converted BigFloat to string: %s", str);

    return str;
}

/**
 * @brief Returns the absolute value of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing |a|, or NULL on error.
 */
BigFloat* bigfloat_abs(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_abs]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_abs(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_abs]: Computed absolute value.");

    return res;
}

/**
 * @brief Negates a BigFloat (multiplies it by -1).
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing -a, or NULL on error.
 */
BigFloat* bigfloat_negate(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_negate]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_neg(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_negate]: Negated BigFloat.");

    return res;
}

/**
 * @brief Computes the floor of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing the floor(a), or NULL on error.
 */
BigFloat* bigfloat_floor(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_floor]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_floor(res->value, a->value);
    BIGFLOAT_LOG("[bigfloat_floor]: Computed floor.");

    return res;
}

/**
 * @brief Computes the ceiling of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing ceil(a), or NULL on error.
 */
BigFloat* bigfloat_ceil(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_ceil]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_ceil(res->value, a->value);
    BIGFLOAT_LOG("[bigfloat_ceil]: Computed ceiling.");

    return res;
}

/**
 * @brief Rounds a BigFloat to the nearest integer.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing round(a), or NULL on error.
 */
BigFloat* bigfloat_round(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_round]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_round(res->value, a->value);
    BIGFLOAT_LOG("[bigfloat_round]: Rounded BigFloat.");

    return res;
}

/**
 * @brief Truncates the decimal part of a BigFloat (rounds towards zero).
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing trunc(a), or NULL on error.
 */
BigFloat* bigfloat_trunc(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_trunc]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_trunc(res->value, a->value);
    BIGFLOAT_LOG("[bigfloat_trunc]: Truncated BigFloat.");

    return res;
}

/**
 * @brief Computes the reciprocal (1 / a) of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing 1 / a, or NULL if a is zero.
 */
BigFloat* bigfloat_reciprocal(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_reciprocal]: Input BigFloat is NULL.");
        return NULL;
    }

    if (bigfloat_is_zero(a)) {
        BIGFLOAT_LOG("[bigfloat_reciprocal]: Cannot compute reciprocal of zero.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_ui_div(res->value, 1, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_reciprocal]: Computed reciprocal.");

    return res;
}

/**
 * @brief Computes the remainder of division (a mod b).
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return A new BigFloat representing a mod b, or NULL if b is zero.
 */
BigFloat* bigfloat_mod(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_mod]: One or both input BigFloats are NULL.");
        return NULL;
    }

    if (bigfloat_is_zero(b)) {
        BIGFLOAT_LOG("[bigfloat_mod]: Cannot compute modulo with zero.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_fmod(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_mod]: Computed modulo.");

    return res;
}

/**
 * @brief Computes 2^a (exponential function with base 2).
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing 2^a, or NULL on error.
 */
BigFloat* bigfloat_exp2(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_exp2]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_exp2(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_exp2]: Computed 2^a.");

    return res;
}

/**
 * @brief Computes the base-10 logarithm of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing log10(a), or NULL if a is invalid.
 */
BigFloat* bigfloat_log10(const BigFloat* a) {
    if (!a || bigfloat_is_zero(a) || bigfloat_is_negative(a)) {
        BIGFLOAT_LOG("[bigfloat_log10]: Invalid input.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_log10(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_log10]: Computed log10.");

    return res;
}

/**
 * @brief Computes the cube root of a BigFloat.
 * @param a Pointer to the BigFloat.
 * @return A new BigFloat representing cbrt(a), or NULL on error.
 */
BigFloat* bigfloat_cbrt(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_cbrt]: Input BigFloat is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_cbrt(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_cbrt]: Computed cube root.");

    return res;
}

/**
 * @brief Computes the Euclidean norm (hypotenuse) of two BigFloat numbers.
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return A new BigFloat representing sqrt(a^2 + b^2), or NULL on error.
 */
BigFloat* bigfloat_hypot(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_hypot]: One or both input BigFloats are NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_hypot(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_hypot]: Computed hypotenuse.");

    return res;
}

/**
 * @brief Computes the floating-point remainder of division (a % b).
 * @param a Pointer to the first BigFloat.
 * @param b Pointer to the second BigFloat.
 * @return A new BigFloat representing a % b, or NULL if b is zero.
 */
BigFloat* bigfloat_fmod(const BigFloat* a, const BigFloat* b) {
    if (!a || !b) {
        BIGFLOAT_LOG("[bigfloat_fmod]: One or both input BigFloats are NULL.");
        return NULL;
    }

    if (bigfloat_is_zero(b)) {
        BIGFLOAT_LOG("[bigfloat_fmod]: Cannot compute fmod with zero.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_fmod(res->value, a->value, b->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_fmod]: Computed floating-point remainder.");

    return res;
}


/**
 * @brief Computes the logarithm of the absolute value of the gamma function of a BigFloat.
 *
 * @param a Pointer to the input BigFloat.
 * @return Pointer to a new BigFloat containing lgamma(a), or NULL on error.
 */
BigFloat* bigfloat_lgamma(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_lgamma]: Input is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    int sign;
    mpfr_lgamma(res->value, &sign, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_lgamma]: Computed lgamma successfully with sign: %d.", sign);

    return res;
}

/**
 * @brief Computes the error function (erf) of a BigFloat.
 *
 * @param a Pointer to the input BigFloat.
 * @return Pointer to a new BigFloat containing erf(a), or NULL on error.
 */
BigFloat* bigfloat_erf(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_erf]: Input is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) { 
        BIGFLOAT_LOG("[bigfloat_erf]: memory allocation failed.");
        return NULL;
    }

    mpfr_erf(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_erf]: Computed erf successfully.");

    return res;
}

/**
 * @brief Computes exp(a) - 1 with high precision.
 *
 * @param a Pointer to the input BigFloat.
 * @return Pointer to a new BigFloat containing exp(a) - 1, or NULL on error.
 */
BigFloat* bigfloat_expm1(const BigFloat* a) {
    if (!a) {
        BIGFLOAT_LOG("[bigfloat_expm1]: Input is NULL.");
        return NULL;
    }

    BigFloat* res = bigfloat_create();
    if (!res) {
        return NULL;
    }

    mpfr_expm1(res->value, a->value, MPFR_RNDN);
    BIGFLOAT_LOG("[bigfloat_expm1]: Computed expm1 successfully.");

    return res;
}
