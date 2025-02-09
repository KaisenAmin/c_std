/**
 * @author amin tahmasebi
 * @date 2025
 * @class BigInt
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

// #define BIGINT_LOGGING_ENABLE

#ifdef BIGINT_LOGGING_ENABLE
    #define BIGINT_LOG(fmt, ...) \
        do { fprintf(stderr, "[BIGINT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define BIGINT_LOG(fmt, ...) do { } while (0)
#endif
 
// Represents an arbitrary-precision integer.
typedef struct {
    mpz_t value;
} BigInt;
 
 
BigInt* bigint_create(void);
BigInt* bigint_from_string(const char* str);
BigInt* bigint_copy(const BigInt* src);
BigInt* bigint_add(const BigInt* a, const BigInt* b);
BigInt* bigint_subtract(const BigInt* a, const BigInt* b);
BigInt* bigint_multiply(const BigInt* a, const BigInt* b);
BigInt* bigint_divide(const BigInt* a, const BigInt* b, BigInt** remainder);
BigInt* bigint_pow(const BigInt* base, const BigInt* exponent);
BigInt* bigint_mod(const BigInt* a, const BigInt* b);
BigInt* bigint_powmod(const BigInt* base, const BigInt* exponent, const BigInt* modulus);
BigInt* bigint_negate(const BigInt* a);
BigInt* bigint_abs(const BigInt* a);
BigInt* bigint_gcd(const BigInt* a, const BigInt* b);
BigInt* bigint_lcm(const BigInt* a, const BigInt* b);
BigInt* bigint_next_prime(const BigInt* a);
BigInt* bigint_sqrt(const BigInt* a, BigInt** rem);
BigInt* bigint_random(unsigned long bits);
BigInt* bigint_factorial(unsigned long n);
BigInt* bigint_inc(const BigInt* a);
BigInt* bigint_dec(const BigInt* a);
BigInt* bigint_modinv(const BigInt* a, const BigInt* modulus);
BigInt* bigint_sqrt_exact(const BigInt* a);
BigInt* bigint_and(const BigInt* a, const BigInt* b);
BigInt* bigint_or(const BigInt* a, const BigInt* b);
BigInt* bigint_xor(const BigInt* a, const BigInt* b);


void bigint_deallocate(BigInt* bi);
void bigint_normalize(BigInt* bi);

int bigint_compare(const BigInt* a, const BigInt* b);
int bigint_is_prime(const BigInt* a, int reps);
int bigint_cmp_abs(const BigInt* a, const BigInt* b);

bool bigint_is_zero(const BigInt* bi);
bool bigint_is_even(const BigInt* a);

char* bigint_to_hex(const BigInt* a);
char* bigint_to_string(const BigInt* bi);

unsigned long bigint_bit_length(const BigInt* a);
unsigned long bigint_num_digits(const BigInt* a);
unsigned long bigint_log2(const BigInt* a);
unsigned long bigint_sum_digits(const BigInt* a);

#ifdef __cplusplus
}
#endif
 
#endif
 