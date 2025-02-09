/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class BigFloat
*/

#ifndef BIGFLOAT_H_
#define BIGFLOAT_H_
 
#include <mpfr.h>
#include <stdbool.h>
#include <stdio.h>
 
#ifdef __cplusplus
extern "C" {
#endif
 
//  #define BIGFLOAT_LOGGING_ENABLE

#ifdef BIGFLOAT_LOGGING_ENABLE
    #define BIGFLOAT_LOG(fmt, ...) \
        do { fprintf(stderr, "[BIGFLOAT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define BIGFLOAT_LOG(fmt, ...) do { } while (0)
#endif
 

typedef struct {
    mpfr_t value;
} BigFloat;
 
BigFloat* bigfloat_create(void);
BigFloat* bigfloat_from_string(const char* str);
BigFloat* bigfloat_from_double(double value);
BigFloat* bigfloat_add(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_subtract(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_multiply(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_divide(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_sqrt(const BigFloat* a);
BigFloat* bigfloat_pow(const BigFloat* base, const BigFloat* exponent);
BigFloat* bigfloat_log(const BigFloat* a);
BigFloat* bigfloat_exp(const BigFloat* a);
BigFloat* bigfloat_trig_sin(const BigFloat* a);
BigFloat* bigfloat_trig_cos(const BigFloat* a);
BigFloat* bigfloat_trig_tan(const BigFloat* a);
BigFloat* bigfloat_abs(const BigFloat* a);
BigFloat* bigfloat_negate(const BigFloat* a);
BigFloat* bigfloat_floor(const BigFloat* a);
BigFloat* bigfloat_ceil(const BigFloat* a);
BigFloat* bigfloat_round(const BigFloat* a);
BigFloat* bigfloat_trunc(const BigFloat* a);
BigFloat* bigfloat_reciprocal(const BigFloat* a);
BigFloat* bigfloat_mod(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_exp2(const BigFloat* a);
BigFloat* bigfloat_log10(const BigFloat* a);
BigFloat* bigfloat_cbrt(const BigFloat* a);
BigFloat* bigfloat_hypot(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_fmod(const BigFloat* a, const BigFloat* b);
BigFloat* bigfloat_lgamma(const BigFloat* a);
BigFloat* bigfloat_erf(const BigFloat* a);
BigFloat* bigfloat_expm1(const BigFloat* a);


char* bigfloat_to_string(const BigFloat* a);
 
int bigfloat_compare(const BigFloat* a, const BigFloat* b);
void bigfloat_deallocate(BigFloat* bf);

bool bigfloat_is_zero(const BigFloat* a);
bool bigfloat_is_negative(const BigFloat* a);


#ifdef __cplusplus
}
#endif
 
#endif 
 