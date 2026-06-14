/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class BigInt
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in bigint.c (file-level convention).
 *
 * Arbitrary-precision integer wrapper over GMP `mpz_t`. Every
 * factory returns a heap-allocated `BigInt*`; release with
 * `bigint_deallocate`.
 */

#ifndef BIGINT_H_
#define BIGINT_H_

#include <gmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define BIGINT_LOGGING_ENABLE */

#ifdef BIGINT_LOGGING_ENABLE
    #define BIGINT_LOG(fmt, ...) \
        do { fprintf(stderr, "[BIGINT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define BIGINT_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    mpz_t value;
} BigInt;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

BigInt*       bigint_create                    (void);
BigInt*       bigint_from_string               (const char* str);
BigInt*       bigint_from_int                  (long value);
BigInt*       bigint_copy                      (const BigInt* src);
BigInt*       bigint_random                    (unsigned long bits);


/* ------------------------------------------------------------------ */
/* Destruction / normalisation                                        */
/* ------------------------------------------------------------------ */

void          bigint_deallocate                (BigInt* bi);
void          bigint_normalize                 (BigInt* bi);


/* ------------------------------------------------------------------ */
/* Arithmetic                                                         */
/* ------------------------------------------------------------------ */

BigInt*       bigint_add                       (const BigInt* a, const BigInt* b);
BigInt*       bigint_subtract                  (const BigInt* a, const BigInt* b);
BigInt*       bigint_multiply                  (const BigInt* a, const BigInt* b);
BigInt*       bigint_divide                    (const BigInt* a, const BigInt* b, BigInt** remainder);
BigInt*       bigint_mod                       (const BigInt* a, const BigInt* b);
BigInt*       bigint_pow                       (const BigInt* base, const BigInt* exponent);
BigInt*       bigint_powmod                    (const BigInt* base, const BigInt* exponent, const BigInt* modulus);
BigInt*       bigint_negate                    (const BigInt* a);
BigInt*       bigint_abs                       (const BigInt* a);
BigInt*       bigint_inc                       (const BigInt* a);
BigInt*       bigint_dec                       (const BigInt* a);


/* ------------------------------------------------------------------ */
/* Number theory                                                      */
/* ------------------------------------------------------------------ */

BigInt*       bigint_gcd                       (const BigInt* a, const BigInt* b);
BigInt*       bigint_lcm                       (const BigInt* a, const BigInt* b);
BigInt*       bigint_modinv                    (const BigInt* a, const BigInt* modulus);
BigInt*       bigint_next_prime                (const BigInt* a);
BigInt*       bigint_factorial                 (unsigned long n);
BigInt*       bigint_sqrt                      (const BigInt* a, BigInt** rem);
BigInt*       bigint_sqrt_exact                (const BigInt* a);


/* ------------------------------------------------------------------ */
/* Bitwise                                                            */
/* ------------------------------------------------------------------ */

BigInt*       bigint_and                       (const BigInt* a, const BigInt* b);
BigInt*       bigint_or                        (const BigInt* a, const BigInt* b);
BigInt*       bigint_xor                       (const BigInt* a, const BigInt* b);
BigInt*       bigint_shift_left                (const BigInt* a, unsigned long bits);
BigInt*       bigint_shift_right               (const BigInt* a, unsigned long bits);


/* ------------------------------------------------------------------ */
/* Comparison / predicates                                            */
/* ------------------------------------------------------------------ */

int           bigint_compare                   (const BigInt* a, const BigInt* b);
int           bigint_cmp_abs                   (const BigInt* a, const BigInt* b);
int           bigint_sign                      (const BigInt* a);
int           bigint_is_prime                  (const BigInt* a, int reps);
bool          bigint_is_zero                   (const BigInt* bi);
bool          bigint_is_even                   (const BigInt* a);
bool          bigint_to_long                   (const BigInt* a, long* out);


/* ------------------------------------------------------------------ */
/* Introspection                                                      */
/* ------------------------------------------------------------------ */

unsigned long bigint_bit_length                (const BigInt* a);
unsigned long bigint_num_digits                (const BigInt* a);
unsigned long bigint_log2                      (const BigInt* a);
unsigned long bigint_sum_digits                (const BigInt* a);


/* ------------------------------------------------------------------ */
/* Formatting                                                         */
/* ------------------------------------------------------------------ */

char*         bigint_to_string                 (const BigInt* bi);
char*         bigint_to_hex                    (const BigInt* a);


#ifdef __cplusplus
}
#endif

#endif 
