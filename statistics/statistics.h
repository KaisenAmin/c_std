#ifndef STATISTICS_H_
#define STATISTICS_H_ 

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    double slope;
    double intercept;
    
} LinearRegression;


double statistics_mean(double* data, size_t n);
double statistics_fmean(double* data, size_t n, double* weights);
double statistics_geometric_mean(double* data, size_t n);
double statistics_harmonic_mean(double *data, size_t n, double* weights);
double statistics_median(double* data, size_t n);
double statistics_median_low(double* data, size_t n);
double statistics_median_high(double* data, size_t n);
double statistics_median_grouped(double *data, size_t n, double interval);
double statistics_mode(double* data, size_t n);
double statistics_multimode(double* data, size_t n);
double statistics_pstdev(double* data, size_t n, bool mu_provided, double mu);
double statistics_pvariance(double* data, size_t n, bool mu_provided, double mu);
double statistics_stdev(double* data, size_t n, bool xbar_provided, double xbar);
double statistics_variance(double* data, size_t n, bool xbar_provided, double xbar);
double statistics_covariance(double* x, double *y, size_t n);
double statistics_correlation(double* x, double* y, size_t n, const char* method);

LinearRegression statistics_linear_regression(double* x, double* y, size_t n, bool proportional);

#endif 