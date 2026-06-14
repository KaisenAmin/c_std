/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Random
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in random.c (file-level convention).
 *
 * A Python-`random`-flavoured pseudo-random module. Independent from
 * the cryptographically-secure `secrets` module — `random` is fast
 * and reproducible (`random_seed`), `secrets` is unpredictable.
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define RANDOM_LOGGING_ENABLE */

#ifdef RANDOM_LOGGING_ENABLE
    #define RANDOM_LOG(fmt, ...) \
        do { fprintf(stderr, "[RANDOM LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define RANDOM_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Engine state                                                       */
/* ------------------------------------------------------------------ */

void     random_seed                 (unsigned int seed);
void     random_getstate             (unsigned int* state);
void     random_setstate             (const unsigned int* state);


/* ------------------------------------------------------------------ */
/* Integer generators                                                 */
/* ------------------------------------------------------------------ */

int      random_randint              (int a, int b);
int      random_randrange            (int a, int b, int step);
int      random_getrandbits          (int a);
void     random_randbytes            (unsigned char* buffer, size_t n);


/* ------------------------------------------------------------------ */
/* Floating-point generators                                          */
/* ------------------------------------------------------------------ */

double   random_random               (void);
double   random_uniform              (double a, double b);
double   random_triangular           (double low, double high, double mode);
bool     random_boolean              (double p);


/* ------------------------------------------------------------------ */
/* Statistical distributions                                          */
/* ------------------------------------------------------------------ */

double   random_gauss                (double mean, double stddev);
double   random_lognormal            (double mean, double stddev);
double   random_expo                 (double lambda);
double   random_gamma                (double shape, double scale);
double   random_beta                 (double alpha, double beta);
double   random_pareto               (double shape, double scale);
double   random_weibull              (double shape, double scale);
double   random_vonmises             (double mu, double kappa);


/* ------------------------------------------------------------------ */
/* Sequence sampling                                                  */
/* ------------------------------------------------------------------ */

void     random_shuffle              (void* array, size_t n, size_t size);
void*    random_choice               (void* array, size_t n, size_t size);
void     random_choices              (void* array, size_t n, size_t size, size_t num_choices, void* choices, double* weights);
void     random_sample               (void* array, size_t n, size_t size, size_t num_samples, void* samples);


#ifdef __cplusplus
}
#endif

#endif 
