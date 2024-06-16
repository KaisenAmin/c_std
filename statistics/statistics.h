#ifndef STATISTICS_H_
#define STATISTICS_H_ 

#include <stddef.h>
#include <stdbool.h>

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


double statistics_mean(double* data, size_t n);
double statistics_fmean(double* data, size_t n, double* weights);
double statistics_geometric_mean(double* data, size_t n);
double statistics_harmonic_mean(double *data, size_t n, double* weights);
double statistics_median(double* data, size_t n);
double statistics_median_low(double* data, size_t n);
double statistics_median_high(double* data, size_t n);
double statistics_median_grouped(double *data, size_t n, double interval);
double statistics_pstdev(double* data, size_t n, bool mu_provided, double mu);
double statistics_pvariance(double* data, size_t n, bool mu_provided, double mu);
double statistics_stdev(double* data, size_t n, bool xbar_provided, double xbar);
double statistics_variance(double* data, size_t n, bool xbar_provided, double xbar);
double statistics_covariance(double* x, double *y, size_t n);
double statistics_correlation(double* x, double* y, size_t n, CorrelationMethod method);

LinearRegression statistics_linear_regression(double* x, double* y, size_t n, bool proportional);

void* statistics_multimode(void* data, size_t n, size_t size, size_t* mode_count);
void* statistics_mode(void* data, size_t n, size_t size);

#endif 