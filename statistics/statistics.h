/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Statistics
*/

#ifndef STATISTICS_H_
#define STATISTICS_H_ 

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define STATISTICS_LOGGING_ENABLE 

#ifdef STATISTICS_LOGGING_ENABLE
    #define STATISTICS_LOG(fmt, ...) \
        do { fprintf(stderr, "[STATISTICS LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define STATISTICS_LOG(fmt, ...) do { } while (0)
#endif

typedef struct {
    double slope;
    double intercept;
    
} LinearRegression;

typedef struct {
    void* element;
    size_t count;

} __StatisticsElementCount;

typedef struct {
    double value;
    size_t index;

} __StatisticsIndexedValue;

typedef enum {
    CORRELATION_LINEAR,
    CORRELATION_RANKED

} CorrelationMethod;


double statistics_mean(const double* data, size_t n);
double statistics_fmean(const double* data, size_t n, const double* weights);
double statistics_geometric_mean(const double* data, size_t n);
double statistics_harmonic_mean(const double *data, size_t n, const double* weights);
double statistics_median(const double* data, size_t n);
double statistics_median_low(const double* data, size_t n);
double statistics_median_high(const double* data, size_t n);
double statistics_median_grouped(const double *data, size_t n, double interval);
double statistics_pstdev(const double* data, size_t n, bool mu_provided, double mu);
double statistics_pvariance(const double* data, size_t n, bool mu_provided, double mu);
double statistics_stdev(const double* data, size_t n, bool xbar_provided, double xbar);
double statistics_variance(const double* data, size_t n, bool xbar_provided, double xbar);
double statistics_covariance(const double* x, const double *y, size_t n);
double statistics_correlation(const double* x, const double* y, size_t n, CorrelationMethod method);

LinearRegression statistics_linear_regression(const double* x, const double* y, size_t n, bool proportional);

void* statistics_multimode(void* data, size_t n, size_t size, size_t* mode_count);
void* statistics_mode(void* data, size_t n, size_t size);

#ifdef __cplusplus 
}
#endif 

#endif 