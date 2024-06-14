#include "statistics.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Return the index where to insert item x in list a, assuming a is sorted.
static size_t bisect_left(double* data, size_t n, double x) {
    size_t lo = 0;
    size_t hi = n;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        if (data[mid] < x) {
            lo = mid + 1;
        } 
        else {
            hi = mid;
        }
    }

    return lo;
}

// Return the index where to insert item x in list a, assuming a is sorted.
static size_t bisect_right(double* data, size_t n, double x) {
    size_t lo = 0;
    size_t hi = n;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        if (x < data[mid]) {
            hi = mid;
        } 
        else {
            lo = mid + 1;
        }
    }

    return lo;
}

static int statistics_compare_doubles(const void* a, const void* b) {
    double arg1 = *(const double*)a;
    double arg2 = *(const double*)b;

    if (arg1 < arg2) {
        return -1;
    }
    if (arg1 > arg2) {
        return 1;
    }

    return 0;
}

double statistics_mean(double* data, size_t n) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    }
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }

    double sum = 0.0;
    for (size_t index = 0; index < n; index++) {
        sum += data[index];
    }

    return sum / n;
}

double statistics_median(double* data, size_t n) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    }
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }

    double* sorted_data = malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);

    double median;
    if (n % 2 == 0) {
        median = (sorted_data[n / 2 - 1] + sorted_data[n / 2]) / 2.0;
    } 
    else {
        median = sorted_data[n / 2];
    }

    free(sorted_data);
    return median;
}

double statistics_median_low(double* data, size_t n) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    } 
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }

    double* sorted_data = malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);

    double median_low;
    if (n % 2 == 0) {
        median_low = sorted_data[n / 2 - 1];
    } 
    else {
        median_low = sorted_data[n / 2];
    }

    free(sorted_data);
    return median_low;
}

double statistics_median_high(double* data, size_t n) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    } 
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }

    double* sorted_data = malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);

    double median_high;
    if (n % 2 == 0) {
        median_high = sorted_data[n / 2];
    } 
    else {
        median_high = sorted_data[n / 2];
    }

    free(sorted_data);
    return median_high;
}

// based on python implementation
double statistics_median_grouped(double* data, size_t n, double interval) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    } 
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    } 
    else if (interval <= 0) {
        fprintf(stderr, "Error: interval must be positive.\n");
        return NAN;
    }

    double* sorted_data = malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);

    size_t midpoint_index = n / 2;
    double x = sorted_data[midpoint_index];

    size_t i = bisect_left(sorted_data, n, x);
    size_t j = bisect_right(sorted_data, n, x);

    double L = x - interval / 2.0;
    size_t cf = i;
    size_t f = j - i;
    double median_grouped = L + interval * ((n / 2.0 - cf) / f);

    free(sorted_data);
    return median_grouped;
}

double statistics_variance(double* data, size_t n, bool xbar_provided, double xbar) {
    if (!data || n < 2) {
        fprintf(stderr, "Invalid input. Data should have at least two elements.\n");
        return NAN;
    }

    double mean = xbar_provided ? xbar : statistics_mean(data, n);
    double sum_sq_diff = 0.0;

    for (size_t i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_sq_diff += diff * diff;
    }

    return sum_sq_diff / (n - 1);
}

double statistics_stdev(double* data, size_t n, bool xbar_provided, double xbar) {
    if (!data || n < 2) {
        fprintf(stderr, "Invalid input. Data should have at least two elements.\n");
        return NAN;
    }

    double variance = statistics_variance(data, n, xbar_provided, xbar);

    return sqrt(variance);
}

double statistics_pvariance(double* data, size_t n, bool mu_provided, double mu) {
    if (!data || n < 1) {
        fprintf(stderr, "Invalid input. Data should have at least one element.\n");
        return NAN;
    }

    double mean = mu_provided ? mu : statistics_mean(data, n);
    double sum_of_diff = 0.0;

    for (size_t i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_of_diff += diff * diff;
    }

    return sum_of_diff / n;
}

double statistics_pstdev(double* data, size_t n, bool mu_provided, double mu) {
    if (!data || n < 1) {
        fprintf(stderr, "Invalid input. Data should have at least one element.\n");
        return NAN;
    }

    double variance = statistics_pvariance(data, n, mu_provided, mu);
    return sqrt(variance);
}