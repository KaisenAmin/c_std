/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Statistics
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in statistics.c (file-level convention).
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


/* #define STATISTICS_LOGGING_ENABLE */

#ifdef STATISTICS_LOGGING_ENABLE
    #define STATISTICS_LOG(...) \
        do { fprintf(stderr, "[STATISTICS LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
    #define STATISTICS_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    double slope;
    double intercept;
} LinearRegression;


typedef struct {
    void*  element;
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


/* ------------------------------------------------------------------ */
/* Central-tendency averages                                          */
/* ------------------------------------------------------------------ */

double            statistics_mean             (const double* data, size_t n);
double            statistics_fmean            (const double* data, size_t n, const double* weights);
double            statistics_geometric_mean   (const double* data, size_t n);
double            statistics_harmonic_mean    (const double* data, size_t n, const double* weights);


/* ------------------------------------------------------------------ */
/* Median family                                                      */
/* ------------------------------------------------------------------ */

double            statistics_median           (const double* data, size_t n);
double            statistics_median_low       (const double* data, size_t n);
double            statistics_median_high      (const double* data, size_t n);
double            statistics_median_grouped   (const double* data, size_t n, double interval);


/* ------------------------------------------------------------------ */
/* Quantiles / percentiles                                            */
/* ------------------------------------------------------------------ */

double            statistics_percentile       (const double* data, size_t n, double p);
double*           statistics_quantiles        (const double* data, size_t n, size_t n_quantiles, bool exclusive, size_t* out_count);


/* ------------------------------------------------------------------ */
/* Spread (variance / standard deviation)                             */
/* ------------------------------------------------------------------ */

double            statistics_pvariance        (const double* data, size_t n, bool mu_provided,   double mu);
double            statistics_pstdev           (const double* data, size_t n, bool mu_provided,   double mu);
double            statistics_variance         (const double* data, size_t n, bool xbar_provided, double xbar);
double            statistics_stdev            (const double* data, size_t n, bool xbar_provided, double xbar);


/* ------------------------------------------------------------------ */
/* Bivariate statistics                                               */
/* ------------------------------------------------------------------ */

double            statistics_covariance       (const double* x, const double* y, size_t n);
double            statistics_correlation      (const double* x, const double* y, size_t n, CorrelationMethod method);
LinearRegression  statistics_linear_regression(const double* x, const double* y, size_t n, bool proportional);


/* ------------------------------------------------------------------ */
/* Mode (generic over element type)                                   */
/* ------------------------------------------------------------------ */

/* Elements are equal when their bytes are equal (memcmp). For values whose
   bytes do not decide equality - strings (char* pointers), doubles where
   0.0 == -0.0, structs with padding - use the _by variants with a qsort-style
   comparator (0 = equal). */
typedef int (*StatisticsCompareFunc)(const void* a, const void* b);

void*             statistics_mode             (void* data, size_t n, size_t size);
void*             statistics_multimode        (void* data, size_t n, size_t size, size_t* mode_count);
void*             statistics_mode_by          (void* data, size_t n, size_t size, StatisticsCompareFunc compare);
void*             statistics_multimode_by     (void* data, size_t n, size_t size, StatisticsCompareFunc compare, size_t* mode_count);


#ifdef __cplusplus
}
#endif

#endif 
