#include "statistics.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Return the index where to insert item x in list a, assuming a is sorted.
static size_t bisect_left(const double* data, size_t n, double x) {
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
static size_t bisect_right(const double* data, size_t n, double x) {
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

// helper function for compute some of element 
static double statistics_sum(const double* data, size_t n) {
    double total = 0.0;

    for (size_t i = 0; i < n; i++) {
        total += data[i];
    }

    return total;
}

// helper function for checking if any element in data is negative return true
static bool fail_neg(const double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (data[i] < 0) {
            return true;
        }
    }

    return false;
}

// helper function for compares the counts of two elements.
static int statistics_compare_elements(const void* a, const void* b, size_t size) {
    return memcmp(a, b, size);
}

// helper function for compares two elements based on their memory content.
static int statistics_compare_counts(const void* a, const void* b) {
    __StatisticsElementCount* elemA = (__StatisticsElementCount*)a;
    __StatisticsElementCount* elemB = (__StatisticsElementCount*)b;

    return elemB->count - elemA->count;
}

// helper function for compares two elements of type double
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

// this helper function is a comparator function used for qsort to sort data for statistics_rank_data
static int statistics_compare_index_struct(const void* a, const void* b) {
    double arg1 = ((__StatisticsIndexedValue*)a)->value;
    double arg2 = ((__StatisticsIndexedValue*)b)->value;

    if (arg1 < arg2) {
        return -1;
    }
    if (arg1 > arg2) { 
        return 1;
    }

    return 0;
}

// this helper function function has been corrected to handle ties by averaging the ranks of tied values.
static void statistics_rank_data(const double* data, size_t n, double* ranked_data) {
    __StatisticsIndexedValue* indexed_data = malloc(n * sizeof(__StatisticsIndexedValue));
    if (indexed_data == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return;
    }

    for (size_t i = 0; i < n; i++) {
        indexed_data[i].value = data[i];
        indexed_data[i].index = i;
    }

    qsort(indexed_data, n, sizeof(__StatisticsIndexedValue), statistics_compare_index_struct);

    size_t i = 0;
    while (i < n) {
        size_t start = i;
        double sum_ranks = 0.0;

        while (i < n && indexed_data[i].value == indexed_data[start].value) {
            sum_ranks += i + 1;
            i++;
        }

        double avg_rank = sum_ranks / (i - start);
        for (size_t j = start; j < i; j++) {
            ranked_data[indexed_data[j].index] = avg_rank;
        }
    }

    free(indexed_data);
}

static double statistics_sumprod(const double* x, const double* y, size_t n) {
    double sum = 0.0;

    for (size_t i = 0; i < n; i++) {
        sum += x[i] * y[i];
    }

    return sum;
}

// helper functin for add square of each element in data 
static double statistics_sum_of_squares(const double* data, size_t n) {
    double sum = 0.0;

    for (size_t i = 0; i < n; i++) {
        sum += data[i] * data[i];
    }

    return sum;
}

// this helper function calculates Spearman's rank correlation by transforming the ranks and recomputing the differences and sums.
static double statistics_spearman_correlation(const double* x, const double* y, size_t n) {
    double* x_ranked = malloc(n * sizeof(double));
    double* y_ranked = malloc(n * sizeof(double));
    if (x_ranked == NULL || y_ranked == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");

        free(x_ranked);
        free(y_ranked);
        
        return NAN;
    }

    statistics_rank_data(x, n, x_ranked);
    statistics_rank_data(y, n, y_ranked);

    // Calculate d^2
    double d_squared_sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        double d = x_ranked[i] - y_ranked[i];
        d_squared_sum += d * d;
    }

    free(x_ranked);
    free(y_ranked);

    // Spearman's rank correlation coefficient
    double spearman_rho = 1 - (6 * d_squared_sum) / (n * (n * n - 1));
    return spearman_rho;
}

/**
 * @brief This function calculates the arithmetic mean (average) of the given data points.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @return The arithmetic mean as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_mean(const double* data, size_t n) {
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

/**
 * @brief This function calculates the median of the given data points.
 * The median is the middle value when the data is sorted in ascending order.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @return The median as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_median(const double* data, size_t n) {
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

/**
 * @brief  This function calculates the low median of the given data points.
 * The low median is the smaller of the two middle values in an even-sized data set,
 * or the middle value in an odd-sized data set.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @return The low median as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_median_low(const double* data, size_t n) {
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

/**
 * @brief This function calculates the high median of the given data points.
 * The high median is the larger of the two middle values in an even-sized data set,
 * or the middle value in an odd-sized data set.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @return The high median as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_median_high(const double* data, size_t n) {
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

/**
 * @brief This function calculates the grouped median of the given data points using a specified interval.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param interval The interval to use for grouping.
 * @return The grouped median as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, or if `interval` is not positive, 
 *          the function prints an error message to stderr and returns `NAN`.
 */
double statistics_median_grouped(const double* data, size_t n, double interval) {
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

/**
 * @brief This function calculates the sample variance of the given data points.
 * Optionally, it can use a provided mean (xbar) instead of computing it from the da ta.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param xbar_provided Boolean indicating whether a mean (xbar) is provided.
 * @param xbar The mean value to use if `xbar_provided` is `true`. If `xbar_provided` is `false`, this parameter is ignored.
 * @return The sample variance as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is less than 2, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_variance(const double* data, size_t n, bool xbar_provided, double xbar) {
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

/**
 * @brief This function calculates the sample standard deviation of the given data points.
 * Optionally, it can use a provided mean (xbar) instead of computing it from the data.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param xbar_provided Boolean indicating whether a mean (xbar) is provided.
 * @param xbar The mean value to use if `xbar_provided` is `true`. If `xbar_provided` is `false`, this parameter is ignored.
 * @return The sample standard deviation as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is less than 2, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_stdev(const double* data, size_t n, bool xbar_provided, double xbar) {
    if (!data || n < 2) {
        fprintf(stderr, "Invalid input. Data should have at least two elements.\n");
        return NAN;
    }

    double variance = statistics_variance(data, n, xbar_provided, xbar);

    return sqrt(variance);
}

/**
 * @brief This function calculates the population variance of the given data points.
 * Optionally, it can use a provided mean (mu) instead of computing it from the data.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param mu_provided Boolean indicating whether a mean (mu) is provided.
 * @param mu The mean value to use if `mu_provided` is `true`. If `mu_provided` is `false`, this parameter is ignored.
 * @return The population variance as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is less than 1, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_pvariance(const double* data, size_t n, bool mu_provided, double mu) {
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

/**
 * @brief This function calculates the population standard deviation of the given data points.
 * Optionally, it can use a provided mean (mu) instead of computing it from the data.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param mu_provided Boolean indicating whether a mean (mu) is provided.
 * @param mu The mean value to use if `mu_provided` is `true`. If `mu_provided` is `false`, this parameter is ignored.
 * @return The population standard deviation as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is less than 1, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_pstdev(const double* data, size_t n, bool mu_provided, double mu) {
    if (!data || n < 1) {
        fprintf(stderr, "Invalid input. Data should have at least one element.\n");
        return NAN;
    }

    double variance = statistics_pvariance(data, n, mu_provided, mu);
    return sqrt(variance);
}

/**
 * @brief This function calculates the arithmetic mean (average) of the given data points.
 * Optionally, it can use weights to compute the weighted arithmetic mean.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param weights Pointer to an array of doubles representing the weights. If `NULL`, 
 *          the unweighted arithmetic mean is computed.
 * @return The arithmetic mean as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 * @note If weights are provided, the sum of weights must be non-zero. 
 *          If the sum of weights is zero, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_fmean(const double* data, size_t n, const double* weights) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    }
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }

    if (weights == NULL) {
        double total = 0.0;
        for (size_t i = 0; i < n; i++) {
            total += data[i];
        }

        return total / n;
    } 
    else {
        double num = 0.0;
        double den = 0.0;

        for (size_t i = 0; i < n; i++) {
            num += data[i] * weights[i];
            den += weights[i];
        }

        if (den == 0) {
            fprintf(stderr, "Error: sum of weights must be non-zero.\n");
            return NAN;
        }

        return num / den;
    }
}

/**
 * @brief This function calculates the geometric mean of the given data points.
 * The geometric mean is the nth root of the product of n numbers.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @return The geometric mean as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 * @note The function does not support negative values in `data`. If any negative values are found, it prints an error message to stderr and returns `NAN`.
 * @note If any element in `data` is zero, the geometric mean is zero.
 * @note If the computed mean of the logarithms is `-INFINITY`, the function returns zero.
 */
double statistics_geometric_mean(const double* data, size_t n) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    }
    else if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }

    double total = 0.0;
    for (size_t i = 0; i < n; i++) {
        if (data[i] > 0.0 || isnan(data[i])) {
            total += log(data[i]);
        } 
        else if (data[i] == 0.0) {
            return 0.0;
        } 
        else {
            fprintf(stderr, "Error: No negative inputs allowed. Invalid value: %f\n", data[i]);
            return NAN;
        }
    }

    if (total == -INFINITY) {
        return 0.0;
    }

    double mean_log = total / n;
    if (isnan(mean_log)) {
        return NAN;
    }

    return exp(mean_log);
}

/**
 * @brief This function calculates the harmonic mean of the given data points. Optionally, it can use
 * weights to compute the weighted harmonic mean.
 *
 * @param data Pointer to an array of doubles representing the data set.
 * @param n The number of elements in the data set.
 * @param weights Pointer to an array of doubles representing the weights. If `NULL`, 
 *          the unweighted harmonic mean is computed.
 * @return The harmonic mean as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr and returns `NAN`.
 * @note The function does not support negative values in `data` or `weights`. 
 *          If any negative values are found, it prints an error message to stderr and returns `NAN`.
 * @note If the sum of weights is zero or if the weighted sum of the reciprocals is non-positive, 
 *          the function prints an error message to stderr and returns `NAN`.
 */
double statistics_harmonic_mean(const double* data, size_t n, const double* weights) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NAN;
    }
    if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NAN;
    }
    if (fail_neg(data, n)) {
        fprintf(stderr, "Error: harmonic mean does not support negative values.\n");
        return NAN;
    }

    double sum_weights = 0.0;
    if (weights == NULL) {
        sum_weights = n;
    } 
    else {
        if (fail_neg(weights, n)) {
            fprintf(stderr, "Error: harmonic mean does not support negative weights.\n");
            return NAN;
        }
        sum_weights = statistics_sum(weights, n);
        if (sum_weights == 0.0) {
            fprintf(stderr, "Error: sum of weights must be non-zero.\n");
            return NAN;
        }
    }

    double total = 0.0;
    for (size_t i = 0; i < n; i++) {
        if (weights == NULL) {
            total += 1.0 / data[i];
        } 
        else {
            total += weights[i] / data[i];
        }
    }

    if (total <= 0.0) {
        fprintf(stderr, "Error: weighted sum must be positive.\n");
        return NAN;
    }

    return sum_weights / total;
}

/**
 * @brief This function computes the mode of the given data set. If there are multiple elements with the same highest
 * frequency, one of them is returned.
 *
 * @param data Pointer to the data set.
 * @param n The number of elements in the data set.
 * @param size The size of each element in the data set.
 * @return A pointer to the mode element. If an error occurs, the function returns `NULL`.
 *
 * @note The returned mode element should be freed by the caller to avoid memory leaks.
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr
 *       and returns `NULL`.
 * @note If memory allocation fails, the function prints an error message to stderr and returns `NULL`.
 */
void* statistics_mode(void* data, size_t n, size_t size) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NULL;
    }
    if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        return NULL;
    }

    __StatisticsElementCount* counts = malloc(n * sizeof(__StatisticsElementCount));
    if (counts == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NULL;
    }

    size_t unique_elements = 0;
    for (size_t i = 0; i < n; i++) {
        void* element = (char*)data + i * size;
        int found = 0;
        for (size_t j = 0; j < unique_elements; j++) {
            if (statistics_compare_elements(element, counts[j].element, size) == 0) {
                counts[j].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            counts[unique_elements].element = element;
            counts[unique_elements].count = 1;
            unique_elements++;
        }
    }

    qsort(counts, unique_elements, sizeof(__StatisticsElementCount), statistics_compare_counts);

    void* mode_element = malloc(size);
    if (mode_element == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(counts);
        return NULL;
    }
    memcpy(mode_element, counts[0].element, size);

    free(counts);
    return mode_element;
}

/**
 * @brief This function computes the modes of the given data set If there are multiple elements with the highest frequency,
 * all of them are returned.
 *
 * @param data Pointer to the data set.
 * @param n The number of elements in the data set.
 * @param size The size of each element in the data set.
 * @param mode_count Pointer to a variable where the function will store the number of modes found.
 * @return A pointer to the array of modes. If an error occurs, the function returns `NULL`.
 *
 * @note The returned array of modes should be freed by the caller to avoid memory leaks.
 * @note If `data` is `NULL`, or if `n` is zero, the function prints an error message to stderr
 *       and returns `NULL`.
 * @note If memory allocation fails, the function prints an error message to stderr and returns `NULL`.
 */
void* statistics_multimode(void* data, size_t n, size_t size, size_t* mode_count) {
    if (data == NULL) {
        fprintf(stderr, "Error: data argument is null.\n");
        return NULL;
    }
    if (n == 0) {
        fprintf(stderr, "Error: number of elements is zero.\n");
        *mode_count = 0;
        return NULL;
    }

    __StatisticsElementCount* counts = malloc(n * sizeof(__StatisticsElementCount));
    if (counts == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        return NULL;
    }

    size_t unique_elements = 0;
    for (size_t i = 0; i < n; i++) {
        void* element = (char*)data + i * size;
        int found = 0;
        for (size_t j = 0; j < unique_elements; j++) {
            if (statistics_compare_elements(element, counts[j].element, size) == 0) {
                counts[j].count++;
                found = 1;
                break;
            }
        }
        if (!found) {
            counts[unique_elements].element = element;
            counts[unique_elements].count = 1;
            unique_elements++;
        }
    }

    size_t max_count = 0;
    for (size_t i = 0; i < unique_elements; i++) {
        if (counts[i].count > max_count) {
            max_count = counts[i].count;
        }
    }

    size_t mode_elements_count = 0;
    for (size_t i = 0; i < unique_elements; i++) {
        if (counts[i].count == max_count) {
            mode_elements_count++;
        }
    }

    void* modes = malloc(mode_elements_count * size);
    if (modes == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(counts);
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < unique_elements; i++) {
        if (counts[i].count == max_count) {
            memcpy((char*)modes + index * size, counts[i].element, size);
            index++;
        }
    }

    free(counts);
    *mode_count = mode_elements_count;

    return modes;
}

/**
 * @brief This function calculates the covariance between the given data points.
 * Covariance is a measure of how much two random variables vary together.
 *
 * This function uses an unbiased estimator (bias is set to false), meaning that the 
 * result is divided by \( n - 1 \), which is suitable for sample data rather than 
 * population data.
 *
 * @param x Pointer to an array of doubles representing the first data set.
 * @param y Pointer to an array of doubles representing the second data set.
 * @param n The number of data points in each data set.
 * @return The covariance as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `x` or `y` is NULL, or if `n` is less than 2, the function prints an error message to stderr and returns `NAN`.
 */
double statistics_covariance(const double* x, const double* y, size_t n) {
    if (x == NULL || y == NULL) {
        fprintf(stderr, "Error: x or y argument is null.\n");
        return NAN;
    }
    if (n < 2) {
        fprintf(stderr, "Error: covariance requires at least two data points.\n");
        return NAN;
    }

    double xbar = statistics_mean(x, n);
    double ybar = statistics_mean(y, n);
    double sxy = 0.0;

    for (size_t i = 0; i < n; i++) {
        sxy += (x[i] - xbar) * (y[i] - ybar);
    }

    return sxy / (n - 1);
}

/**
 * @brief This function calculates the correlation coefficient between the given data points. It supports
 * both Pearson's linear correlation and Spearman's ranked correlation.
 *
 * @param x Pointer to an array of doubles representing the first data set.
 * @param y Pointer to an array of doubles representing the second data set.
 * @param n The number of data points in each data set.
 * @param method The method to use for calculating the correlation. Supported methods are
 *        `CORRELATION_LINEAR` for Pearson's correlation and `CORRELATION_RANKED` for Spearman's correlation.
 * @return The correlation coefficient as a double. If an error occurs, the function returns `NAN`.
 *
 * @note If `x` or `y` is NULL, or if `n` is less than 2, or if an unsupported method is specified,
 *       the function prints an error message to stderr and returns `NAN`.
 * @note For `CORRELATION_LINEAR`, the function dynamically allocates memory for copies of the data points.
 *       Ensure that there is enough available memory to avoid allocation failure.
 * @note If the variance of either data set is zero, the function returns `NAN`.
 */
double statistics_correlation(const double* x, const double* y, size_t n, CorrelationMethod method) {
    if (x == NULL || y == NULL) {
        fprintf(stderr, "Error: x or y argument is null.\n");
        return NAN;
    }
    else if (n < 2) {
        fprintf(stderr, "Error: correlation requires at least two data points.\n");
        return NAN;
    }
    else if (method != CORRELATION_LINEAR && method != CORRELATION_RANKED) {
        fprintf(stderr, "Error: Unsupported method .Supported methods are 'linear' and 'ranked'.\n");
        return NAN;
    }

    if (method == CORRELATION_RANKED) {
        return statistics_spearman_correlation(x, y, n);
    }

    double* x_copy = malloc(n * sizeof(double));
    double* y_copy = malloc(n * sizeof(double));

    if (x_copy == NULL || y_copy == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");

        free(x_copy);
        free(y_copy);

        return NAN;
    }

    memcpy(x_copy, x, n * sizeof(double));
    memcpy(y_copy, y, n * sizeof(double));

    double xbar = statistics_mean(x, n);
    double ybar = statistics_mean(y, n);

    for (size_t i = 0; i < n; i++) {
        x_copy[i] -= xbar;
        y_copy[i] -= ybar;
    }

    double sxy = statistics_sumprod(x_copy, y_copy, n);
    double sxx = statistics_sum_of_squares(x_copy, n);
    double syy = statistics_sum_of_squares(y_copy, n);

    free(x_copy);
    free(y_copy);

    return sxy / sqrt(sxx * syy);
}

/**
 * @brief This function performs linear regression on the given data points. If `proportional` is `true`,
 * the regression line is forced through the origin, and only the slope is calculated. Otherwise,
 * both slope and intercept are calculated.
 *
 * @param x Pointer to an array of doubles representing the x-coordinates of the data points.
 * @param y Pointer to an array of doubles representing the y-coordinates of the data points.
 * @param n The number of data points.
 * @param proportional Boolean indicating whether the regression line should be forced through the origin.
 * @return A LinearRegression structure containing the slope and intercept of the regression line.
 *
 * @note If `x` or `y` is NULL, or if `n` is less than 2, the function returns a LinearRegression
 *       structure with both slope and intercept set to 0.0 and prints an error message to stderr.
 * @note The function dynamically allocates memory for the centered data points if `proportional` is false.
 *       Ensure that there is enough available memory to avoid allocation failure.
 * @note If the variance of the x-coordinates is zero, the function returns a LinearRegression 
 *       structure with both slope and intercept set to 0.0 and prints an error message to stderr.
 */
LinearRegression statistics_linear_regression(const double* x, const double* y, size_t n, bool proportional) {
    LinearRegression result = {0.0, 0.0};

    if (x == NULL || y == NULL) {
        fprintf(stderr, "Error: x or y argument is null.\n");
        return result;
    }
    else if (n < 2) {
        fprintf(stderr, "Error: linear regression requires at least two data points.\n");
        return result;
    }

    double xbar = 0.0;
    double ybar = 0.0;

    if (!proportional) {
        xbar = statistics_mean(x, n);
        ybar = statistics_mean(y, n);

        double* x_centered = malloc(n * sizeof(double));
        double* y_centered = malloc(n * sizeof(double));

        if (x_centered == NULL || y_centered == NULL) {
            fprintf(stderr, "Error: memory allocation failed.\n");

            free(x_centered);
            free(y_centered);

            return result;
        }

        for (size_t i = 0; i < n; i++) {
            x_centered[i] = x[i] - xbar;
            y_centered[i] = y[i] - ybar;
        }

        double sxy = statistics_sumprod(x_centered, y_centered, n);
        double sxx = statistics_sum_of_squares(x_centered, n);

        free(x_centered);
        free(y_centered);

        if (sxx == 0.0) {
            fprintf(stderr, "Error: x is constant.\n");
            return result;
        }

        result.slope = sxy / sxx;
        result.intercept = ybar - result.slope * xbar;
    } 
    else {
        double sxy = statistics_sumprod(x, y, n);
        double sxx = statistics_sum_of_squares(x, n);

        if (sxx == 0.0) {
            fprintf(stderr, "Error: x is constant.\n");
            return result;
        }

        result.slope = sxy / sxx;
        result.intercept = 0.0;
    }

    return result;
}