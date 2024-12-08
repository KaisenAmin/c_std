/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Random
*/

#ifndef RANDOM_H_
#define RANDOM_H_ 

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif 


#define RANDOM_LOGGING_ENABLE

#ifdef RANDOM_LOGGING_ENABLE
    #define RANDOM_LOG(fmt, ...) \
        do { fprintf(stderr, "[RANDOM LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define RANDOM_LOG(fmt, ...) do { } while (0)
#endif

void random_seed(unsigned int seed);
void random_shuffle(void *array, size_t n, size_t size);
void random_choices(void *array, size_t n, size_t size, size_t num_choices, void *choices, double *weights);
void random_sample(void *array, size_t n, size_t size, size_t num_samples, void *samples);
void random_getstate(unsigned int *state);
void random_setstate(const unsigned int *state);

void* random_choice(void* arrray, size_t n, size_t size);

int random_randint(int a, int b);
int random_randrange(int a, int b, int step);
int random_getrandbits(int a);

double random_random();
double random_uniform(double a, double b);
double random_triangular(double low, double high, double mode);
double random_gauss(double mean, double stddev);
double random_expo(double lambda);
double random_lognormal(double mean, double stddev);
double random_gamma(double shape, double scale);
double random_pareto(double shape, double scale);
double random_beta(double alpha, double beta);
double random_weibull(double shape, double scale);
double random_vonmises(double mu, double kappa);

#ifdef __cplusplus
}
#endif 

#endif 