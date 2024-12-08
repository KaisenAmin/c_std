/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Statistics
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "statistics.h"

/**
 * @brief This function performs a binary search to find the index where a value x should be inserted in the 
 * sorted array data while maintaining the order. It returns the index where x could be inserted without 
 * changing the sort order.
 */
static size_t bisect_left(const double* data, size_t n, double x) {
    STATISTICS_LOG("[bisect_left]: Entering with array: %p, size: %zu, value to insert: %f", (void*)data, n, x);

    size_t lo = 0;
    size_t hi = n;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        STATISTICS_LOG("[bisect_left]: Checking mid index: %zu, data[mid]: %f", mid, data[mid]);
        
        if (data[mid] < x) {
            STATISTICS_LOG("[bisect_left]: data[mid] < x, updating lo to: %zu", mid + 1);
            lo = mid + 1;
        } 
        else {
            STATISTICS_LOG("[bisect_left]: data[mid] >= x, updating hi to: %zu", mid);
            hi = mid;
        }
    }

    STATISTICS_LOG("[bisect_left]: Exiting with insertion index: %zu", lo);
    return lo;
}

/**
 * @brief Similar to bisect_left, but it returns the index where x should 
 * be inserted to the right of existing elements that are equal to x. This is useful when handling duplicate values.
 */
static size_t bisect_right(const double* data, size_t n, double x) {
    STATISTICS_LOG("[bisect_right]: Entering with array: %p, size: %zu, value to insert: %f", (void*)data, n, x);

    size_t lo = 0;
    size_t hi = n;

    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        STATISTICS_LOG("[bisect_right]: Checking mid index: %zu, data[mid]: %f", mid, data[mid]);
        
        if (x < data[mid]) {
            STATISTICS_LOG("[bisect_right]: x < data[mid], updating hi to: %zu", mid);
            hi = mid;
        } 
        else {
            STATISTICS_LOG("[bisect_right]: x >= data[mid], updating lo to: %zu", mid + 1);
            lo = mid + 1;
        }
    }

    STATISTICS_LOG("[bisect_right]: Exiting with insertion index: %zu", lo);
    return lo;
}

/**
 * @brief Calculates the sum of the elements in the array data with n elements.
 */
static double statistics_sum(const double* data, size_t n) {
    STATISTICS_LOG("[statistics_sum]: Entering with array: %p, size: %zu", (void*)data, n);

    double total = 0.0;

    for (size_t i = 0; i < n; i++) {
        STATISTICS_LOG("[statistics_sum]: Adding data[%zu] = %f to total.", i, data[i]);
        total += data[i];
    }

    STATISTICS_LOG("[statistics_sum]: Exiting with total sum: %f", total);
    return total;
}

/**
 * @brief Checks if there are any negative elements in the array data. 
 * If any element is negative, it returns true; otherwise, it returns false.
 */
static bool fail_neg(const double* data, size_t n) {
    STATISTICS_LOG("[fail_neg]: Entering with array: %p, size: %zu", (void*)data, n);

    for (size_t i = 0; i < n; i++) {
        STATISTICS_LOG("[fail_neg]: Checking data[%zu] = %f", i, data[i]);
        if (data[i] < 0) {
            STATISTICS_LOG("[fail_neg]: Found negative value at index %zu", i);
            return true;
        }
    }

    STATISTICS_LOG("[fail_neg]: No negative values found");
    return false;
}

/**
 * @brief Compares two elements based on their memory content using memcmp. 
 * This function is useful for comparing elements of different types in generic data structures.
 */
static int statistics_compare_elements(const void* a, const void* b, size_t size) {
    STATISTICS_LOG("[statistics_compare_elements]: Comparing two elements with size: %zu", size);
    int result = memcmp(a, b, size);
    STATISTICS_LOG("[statistics_compare_elements]: Result of comparison: %d", result);

    return result;
}

/**
 * @brief Compares two elements of the type __StatisticsElementCount based on their count field. 
 * This function is used to sort elements by their frequency in statistics functions like mode.
 */
static int statistics_compare_counts(const void* a, const void* b) {
    __StatisticsElementCount* elemA = (__StatisticsElementCount*)a;
    __StatisticsElementCount* elemB = (__StatisticsElementCount*)b;

    STATISTICS_LOG("[statistics_compare_counts]: Comparing counts: %zu vs %zu", elemA->count, elemB->count);
    int result = elemB->count - elemA->count;
    STATISTICS_LOG("[statistics_compare_counts]: Result of comparison: %d", result);

    return result;
}

/**
 * @brief Compares two double values. It returns -1 if the first value is less than the second,
 * 1 if the first value is greater than the second, and 0 if they are equal.
 */
static int statistics_compare_doubles(const void* a, const void* b) {
    double arg1 = *(const double*)a;
    double arg2 = *(const double*)b;

    STATISTICS_LOG("[statistics_compare_doubles]: Comparing values: %f and %f", arg1, arg2);

    if (arg1 < arg2) {
        STATISTICS_LOG("[statistics_compare_doubles]: %f is less than %f", arg1, arg2);
        return -1;
    }
    if (arg1 > arg2) {
        STATISTICS_LOG("[statistics_compare_doubles]: %f is greater than %f", arg1, arg2);
        return 1;
    }

    STATISTICS_LOG("[statistics_compare_doubles]: %f is equal to %f", arg1, arg2);
    return 0;
}

/**
 * @brief A comparator function used for sorting __StatisticsIndexedValue structures, 
 * which hold both the original value and its index in the dataset. It sorts based on the value field.
 */
static int statistics_compare_index_struct(const void* a, const void* b) {
    double arg1 = ((__StatisticsIndexedValue*)a)->value;
    double arg2 = ((__StatisticsIndexedValue*)b)->value;

    STATISTICS_LOG("[statistics_compare_index_struct]: Comparing indexed values: %f and %f", arg1, arg2);

    if (arg1 < arg2) {
        STATISTICS_LOG("[statistics_compare_index_struct]: %f is less than %f", arg1, arg2);
        return -1;
    }
    if (arg1 > arg2) {
        STATISTICS_LOG("[statistics_compare_index_struct]: %f is greater than %f", arg1, arg2);
        return 1;
    }

    STATISTICS_LOG("[statistics_compare_index_struct]: %f is equal to %f", arg1, arg2);
    return 0;
}

// this helper function function has been corrected to handle ties by averaging the ranks of tied values.
static void statistics_rank_data(const double* data, size_t n, double* ranked_data) {
    STATISTICS_LOG("[statistics_rank_data]: Entering function with n = %zu", n);

    __StatisticsIndexedValue* indexed_data = (__StatisticsIndexedValue*)malloc(n * sizeof(__StatisticsIndexedValue));
    if (indexed_data == NULL) {
        STATISTICS_LOG("[statistics_rank_data]: Error: memory allocation failed.");
        return;
    }

    for (size_t i = 0; i < n; i++) {
        indexed_data[i].value = data[i];
        indexed_data[i].index = i;
    }

    qsort(indexed_data, n, sizeof(__StatisticsIndexedValue), statistics_compare_index_struct);
    STATISTICS_LOG("[statistics_rank_data]: Data sorted.");

    size_t i = 0;
    while (i < n) {
        size_t start = i;
        double sum_ranks = 0.0;

        while (i < n && indexed_data[i].value == indexed_data[start].value) {
            sum_ranks += i + 1;
            i++;
        }

        double avg_rank = sum_ranks / (i - start);
        STATISTICS_LOG("[statistics_rank_data]: Rank for tied values from index %zu to %zu is %f", start, i - 1, avg_rank);

        for (size_t j = start; j < i; j++) {
            ranked_data[indexed_data[j].index] = avg_rank;
        }
    }

    free(indexed_data);
    STATISTICS_LOG("[statistics_rank_data]: Exiting function.");
}

static double statistics_sumprod(const double* x, const double* y, size_t n) {
    STATISTICS_LOG("[statistics_sumprod]: Entering function with n = %zu", n);

    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += x[i] * y[i];
    }

    STATISTICS_LOG("[statistics_sumprod]: Sum of products = %f", sum);
    return sum;
}

// helper functin for add square of each element in data 
static double statistics_sum_of_squares(const double* data, size_t n) {
    STATISTICS_LOG("[statistics_sum_of_squares]: Entering function with n = %zu", n);

    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += data[i] * data[i];
    }

    STATISTICS_LOG("[statistics_sum_of_squares]: Sum of squares = %f", sum);
    return sum;
}

// this helper function calculates Spearman's rank correlation by transforming the ranks and recomputing the differences and sums.
static double statistics_spearman_correlation(const double* x, const double* y, size_t n) {
    STATISTICS_LOG("[statistics_spearman_correlation]: Entering function with n = %zu", n);

    double* x_ranked = (double*)malloc(n * sizeof(double));
    double* y_ranked = (double*)malloc(n * sizeof(double));
    if (x_ranked == NULL || y_ranked == NULL) {
        STATISTICS_LOG("[statistics_spearman_correlation]: Error: memory allocation failed.");
        
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
    STATISTICS_LOG("[statistics_spearman_correlation]: Spearman's rho = %f", spearman_rho);

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
    STATISTICS_LOG("[statistics_mean]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_mean]: Error: data argument is null.");
        return NAN;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_mean]: Error: number of elements is zero.");
        return NAN;
    }

    double sum = 0.0;
    for (size_t index = 0; index < n; index++) {
        sum += data[index];
    }

    STATISTICS_LOG("[statistics_mean]: Sum = %f, Mean = %f", sum, sum / n);
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
    STATISTICS_LOG("[statistics_median]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_median]: Error: data argument is null.");
        return NAN;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_median]: Error: number of elements is zero.");
        return NAN;
    }

    double* sorted_data = (double*)malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        STATISTICS_LOG("[statistics_median]: Error: memory allocation failed.");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);
    STATISTICS_LOG("[statistics_median]: Data sorted.");

    double median;
    if (n % 2 == 0) {
        median = (sorted_data[n / 2 - 1] + sorted_data[n / 2]) / 2.0;
    } 
    else {
        median = sorted_data[n / 2];
    }

    STATISTICS_LOG("[statistics_median]: Median value = %f", median);

    free(sorted_data);
    STATISTICS_LOG("[statistics_median]: Exiting function.");
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
    STATISTICS_LOG("[statistics_median_low]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_median_low]: Error: data argument is null.");
        return NAN;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_median_low]: Error: number of elements is zero.");
        return NAN;
    }

    double* sorted_data = (double*)malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        STATISTICS_LOG("[statistics_median_low]: Error: memory allocation failed.");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);
    STATISTICS_LOG("[statistics_median_low]: Data sorted.");

    double median_low;
    if (n % 2 == 0) {
        median_low = sorted_data[n / 2 - 1];
    } 
    else {
        median_low = sorted_data[n / 2];
    }

    STATISTICS_LOG("[statistics_median_low]: Median low value = %f", median_low);

    free(sorted_data);
    STATISTICS_LOG("[statistics_median_low]: Exiting function.");
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
    STATISTICS_LOG("[statistics_median_high]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_median_high]: Error: data argument is null.");
        return NAN;
    } 
    if (n == 0) {
        STATISTICS_LOG("[statistics_median_high]: Error: number of elements is zero.");
        return NAN;
    }

    double* sorted_data = (double*)malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        STATISTICS_LOG("[statistics_median_high]: Error: memory allocation failed.");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);
    STATISTICS_LOG("[statistics_median_high]: Data sorted.");

    double median_high = sorted_data[n / 2];
    STATISTICS_LOG("[statistics_median_high]: Median high value = %f", median_high);

    free(sorted_data);
    STATISTICS_LOG("[statistics_median_high]: Exiting function.");

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
    STATISTICS_LOG("[statistics_median_grouped]: Entering function with n = %zu and interval = %f", n, interval);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_median_grouped]: Error: data argument is null.");
        return NAN;
    } 
    if (n == 0) {
        STATISTICS_LOG("[statistics_median_grouped]: Error: number of elements is zero.");
        return NAN;
    } 
    if (interval <= 0) {
        STATISTICS_LOG("[statistics_median_grouped]: Error: interval must be positive.");
        return NAN;
    }

    double* sorted_data = (double*)malloc(n * sizeof(double));
    if (sorted_data == NULL) {
        STATISTICS_LOG("[statistics_median_grouped]: Error: memory allocation failed.");
        return NAN;
    }

    for (size_t i = 0; i < n; i++) {
        sorted_data[i] = data[i];
    }

    qsort(sorted_data, n, sizeof(double), statistics_compare_doubles);
    STATISTICS_LOG("[statistics_median_grouped]: Data sorted.");

    size_t midpoint_index = n / 2;
    double x = sorted_data[midpoint_index];
    STATISTICS_LOG("[statistics_median_grouped]: Midpoint value = %f", x);

    size_t i = bisect_left(sorted_data, n, x);
    size_t j = bisect_right(sorted_data, n, x);
    STATISTICS_LOG("[statistics_median_grouped]: Bisect positions - i: %zu, j: %zu", i, j);

    double L = x - interval / 2.0;
    size_t cf = i;
    size_t f = j - i;
    double median_grouped = L + interval * ((n / 2.0 - cf) / f);
    STATISTICS_LOG("[statistics_median_grouped]: Grouped median = %f", median_grouped);

    free(sorted_data);
    STATISTICS_LOG("[statistics_median_grouped]: Exiting function.");
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
    STATISTICS_LOG("[statistics_variance]: Entering function with n = %zu", n);

    if (!data || n < 2) {
        STATISTICS_LOG("[statistics_variance]: Error: Invalid input. Data should have at least two elements.");
        return NAN;
    }

    double mean = xbar_provided ? xbar : statistics_mean(data, n);
    STATISTICS_LOG("[statistics_variance]: Mean = %f", mean);

    double sum_sq_diff = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_sq_diff += diff * diff;
    }

    double variance = sum_sq_diff / (n - 1);
    STATISTICS_LOG("[statistics_variance]: Variance = %f", variance);
    return variance;
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
    STATISTICS_LOG("[statistics_stdev]: Entering function with n = %zu", n);

    if (!data || n < 2) {
        STATISTICS_LOG("[statistics_stdev]: Error: Invalid input. Data should have at least two elements.");
        return NAN;
    }

    double variance = statistics_variance(data, n, xbar_provided, xbar);
    double stdev = sqrt(variance);

    STATISTICS_LOG("[statistics_stdev]: Standard deviation = %f", stdev);
    return stdev;
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
    STATISTICS_LOG("[statistics_pvariance]: Entering function with n = %zu", n);

    if (!data || n < 1) {
        STATISTICS_LOG("[statistics_pvariance]: Error: Invalid input. Data should have at least one element.");
        return NAN;
    }

    double mean = mu_provided ? mu : statistics_mean(data, n);
    STATISTICS_LOG("[statistics_pvariance]: Mean = %f", mean);

    double sum_of_diff = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_of_diff += diff * diff;
    }

    double pvariance = sum_of_diff / n;
    STATISTICS_LOG("[statistics_pvariance]: Population variance = %f", pvariance);

    return pvariance;
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
    STATISTICS_LOG("[statistics_pstdev]: Entering function with n = %zu", n);

    if (!data || n < 1) {
        STATISTICS_LOG("[statistics_pstdev]: Error: Invalid input. Data should have at least one element.");
        return NAN;
    }

    double variance = statistics_pvariance(data, n, mu_provided, mu);
    double stdev = sqrt(variance);

    STATISTICS_LOG("[statistics_pstdev]: Standard deviation = %f", stdev);
    return stdev;
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
    STATISTICS_LOG("[statistics_fmean]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_fmean]: Error: data argument is null.");
        return NAN;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_fmean]: Error: number of elements is zero.");
        return NAN;
    }

    if (weights == NULL) {
        double total = 0.0;
        for (size_t i = 0; i < n; i++) {
            total += data[i];
        }

        double mean = total / n;
        STATISTICS_LOG("[statistics_fmean]: Unweighted mean = %f", mean);
        return mean;
    } 
    else {
        double num = 0.0;
        double den = 0.0;

        for (size_t i = 0; i < n; i++) {
            num += data[i] * weights[i];
            den += weights[i];
        }

        if (den == 0) {
            STATISTICS_LOG("[statistics_fmean]: Error: sum of weights must be non-zero.");
            return NAN;
        }

        double weighted_mean = num / den;
        STATISTICS_LOG("[statistics_fmean]: Weighted mean = %f", weighted_mean);
        return weighted_mean;
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
    STATISTICS_LOG("[statistics_geometric_mean]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_geometric_mean]: Error: data argument is null.");
        return NAN;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_geometric_mean]: Error: number of elements is zero.");
        return NAN;
    }

    double total = 0.0;
    for (size_t i = 0; i < n; i++) {
        if (data[i] > 0.0) {
            total += log(data[i]);
        } 
        else if (data[i] == 0.0) {
            STATISTICS_LOG("[statistics_geometric_mean]: Data contains zero, returning 0.0.");
            return 0.0;
        } 
        else {
            STATISTICS_LOG("[statistics_geometric_mean]: Error: No negative inputs allowed. Invalid value: %f", data[i]);
            return NAN;
        }
    }

    if (total == -INFINITY) {
        STATISTICS_LOG("[statistics_geometric_mean]: Sum of logs is -INFINITY, returning 0.0.");
        return 0.0;
    }

    double mean_log = total / n;
    double geo_mean = exp(mean_log);

    STATISTICS_LOG("[statistics_geometric_mean]: Geometric mean = %f", geo_mean);
    return geo_mean;
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
    STATISTICS_LOG("[statistics_harmonic_mean]: Entering function with n = %zu", n);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_harmonic_mean]: Error: data argument is null.");
        return NAN;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_harmonic_mean]: Error: number of elements is zero.");
        return NAN;
    }
    if (fail_neg(data, n)) {
        STATISTICS_LOG("[statistics_harmonic_mean]: Error: harmonic mean does not support negative values.");
        return NAN;
    }

    double sum_weights = 0.0;
    if (weights == NULL) {
        sum_weights = n;
    } 
    else {
        if (fail_neg(weights, n)) {
            STATISTICS_LOG("[statistics_harmonic_mean]: Error: harmonic mean does not support negative weights.");
            return NAN;
        }
        sum_weights = statistics_sum(weights, n);
        if (sum_weights == 0.0) {
            STATISTICS_LOG("[statistics_harmonic_mean]: Error: sum of weights must be non-zero.");
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
        STATISTICS_LOG("[statistics_harmonic_mean]: Error: weighted sum must be positive.");
        return NAN;
    }

    double harmonic_mean = sum_weights / total;
    STATISTICS_LOG("[statistics_harmonic_mean]: Harmonic mean = %f", harmonic_mean);
    return harmonic_mean;
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
    STATISTICS_LOG("[statistics_mode]: Entering function with n = %zu, size = %zu", n, size);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_mode]: Error: data argument is null.");
        return NULL;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_mode]: Error: number of elements is zero.");
        return NULL;
    }

    __StatisticsElementCount* counts = (__StatisticsElementCount*) malloc(n * sizeof(__StatisticsElementCount));
    if (counts == NULL) {
        STATISTICS_LOG("[statistics_mode]: Error: memory allocation for counts failed.");
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

        STATISTICS_LOG("[statistics_mode]: Processed element %zu, unique_elements = %zu", i, unique_elements);
    }

    qsort(counts, unique_elements, sizeof(__StatisticsElementCount), statistics_compare_counts);

    void* mode_element = malloc(size);
    if (mode_element == NULL) {
        STATISTICS_LOG("[statistics_mode]: Error: memory allocation for mode_element failed.");
        free(counts);
        return NULL;
    }

    memcpy(mode_element, counts[0].element, size);
    STATISTICS_LOG("[statistics_mode]: Mode element found at index 0 with count = %zu", counts[0].count);

    free(counts);
    STATISTICS_LOG("[statistics_mode]: Exiting function, mode_element = %p", mode_element);
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
    STATISTICS_LOG("[statistics_multimode]: Entering function with n = %zu, size = %zu", n, size);

    if (data == NULL) {
        STATISTICS_LOG("[statistics_multimode]: Error: data argument is null.");
        return NULL;
    }
    if (n == 0) {
        STATISTICS_LOG("[statistics_multimode]: Error: number of elements is zero.");
        *mode_count = 0;
        return NULL;
    }

    __StatisticsElementCount* counts = (__StatisticsElementCount*) malloc(n * sizeof(__StatisticsElementCount));
    if (counts == NULL) {
        STATISTICS_LOG("[statistics_multimode]: Error: memory allocation for counts failed.");
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

        STATISTICS_LOG("[statistics_multimode]: Processing element %zu, unique_elements = %zu", i, unique_elements);
    }

    size_t max_count = 0;
    for (size_t i = 0; i < unique_elements; i++) {
        if (counts[i].count > max_count) {
            max_count = counts[i].count;
        }
    }

    STATISTICS_LOG("[statistics_multimode]: max_count = %zu", max_count);

    size_t mode_elements_count = 0;
    for (size_t i = 0; i < unique_elements; i++) {
        if (counts[i].count == max_count) {
            mode_elements_count++;
        }
    }

    STATISTICS_LOG("[statistics_multimode]: mode_elements_count = %zu", mode_elements_count);

    void* modes = malloc(mode_elements_count * size);
    if (modes == NULL) {
        STATISTICS_LOG("[statistics_multimode]: Error: memory allocation for modes failed.");
        free(counts);
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < unique_elements; i++) {
        if (counts[i].count == max_count) {
            memcpy((char*)modes + index * size, counts[i].element, size);
            STATISTICS_LOG("[statistics_multimode]: Mode element %zu: %p", index, counts[i].element);
            index++;
        }
    }

    free(counts);
    *mode_count = mode_elements_count;

    STATISTICS_LOG("[statistics_multimode]: Exiting function with mode_count = %zu", *mode_count);

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
    STATISTICS_LOG("[statistics_covariance]: Entering function with n = %zu", n);

    if (x == NULL || y == NULL) {
        STATISTICS_LOG("[statistics_covariance]: Error: x or y argument is null.");
        return NAN;
    }
    if (n < 2) {
        STATISTICS_LOG("[statistics_covariance]: Error: covariance requires at least two data points.");
        return NAN;
    }

    double xbar = statistics_mean(x, n);
    double ybar = statistics_mean(y, n);
    STATISTICS_LOG("[statistics_covariance]: xbar = %f, ybar = %f", xbar, ybar);

    double sxy = 0.0;
    for (size_t i = 0; i < n; i++) {
        sxy += (x[i] - xbar) * (y[i] - ybar);
        STATISTICS_LOG("[statistics_covariance]: sxy += (%f - %f) * (%f - %f)", x[i], xbar, y[i], ybar);
    }

    double result = sxy / (n - 1);
    STATISTICS_LOG("[statistics_covariance]: Result = %f", result);
    return result;
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
    STATISTICS_LOG("[statistics_correlation]: Entering function with n = %zu, method = %d", n, method);

    if (x == NULL || y == NULL) {
        STATISTICS_LOG("[statistics_correlation]: Error: x or y argument is null.");
        return NAN;
    }
    if (n < 2) {
        STATISTICS_LOG("[statistics_correlation]: Error: correlation requires at least two data points.");
        return NAN;
    }
    if (method != CORRELATION_LINEAR && method != CORRELATION_RANKED) {
        STATISTICS_LOG("[statistics_correlation]: Error: Unsupported method. Supported methods are 'linear' and 'ranked'.");
        return NAN;
    }

    if (method == CORRELATION_RANKED) {
        STATISTICS_LOG("[statistics_correlation]: Using Spearman ranked correlation.");
        return statistics_spearman_correlation(x, y, n);
    }

    STATISTICS_LOG("[statistics_correlation]: Allocating memory for copies of x and y.");
    double* x_copy = (double*) malloc(n * sizeof(double));
    double* y_copy = (double*) malloc(n * sizeof(double));

    if (x_copy == NULL || y_copy == NULL) {
        STATISTICS_LOG("[statistics_correlation]: Error: memory allocation failed.");
        free(x_copy);
        free(y_copy);
        return NAN;
    }

    memcpy(x_copy, x, n * sizeof(double));
    memcpy(y_copy, y, n * sizeof(double));

    double xbar = statistics_mean(x, n);
    double ybar = statistics_mean(y, n);
    STATISTICS_LOG("[statistics_correlation]: xbar = %f, ybar = %f", xbar, ybar);

    for (size_t i = 0; i < n; i++) {
        x_copy[i] -= xbar;
        y_copy[i] -= ybar;
        STATISTICS_LOG("[statistics_correlation]: x_copy[%zu] = %f, y_copy[%zu] = %f", i, x_copy[i], i, y_copy[i]);
    }

    double sxy = statistics_sumprod(x_copy, y_copy, n);
    double sxx = statistics_sum_of_squares(x_copy, n);
    double syy = statistics_sum_of_squares(y_copy, n);
    STATISTICS_LOG("[statistics_correlation]: sxy = %f, sxx = %f, syy = %f", sxy, sxx, syy);

    free(x_copy);
    free(y_copy);

    double result = sxy / sqrt(sxx * syy);
    STATISTICS_LOG("[statistics_correlation]: Result = %f", result);
    return result;
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
    STATISTICS_LOG("[statistics_linear_regression]: Entering function with n = %zu, proportional = %d", n, proportional);

    if (x == NULL || y == NULL) {
        STATISTICS_LOG("[statistics_linear_regression]: Error: x or y argument is null.");
        return result;
    }
    else if (n < 2) {
        STATISTICS_LOG("[statistics_linear_regression]: Error: linear regression requires at least two data points.");
        return result;
    }

    double xbar = 0.0;
    double ybar = 0.0;

    if (!proportional) {
        STATISTICS_LOG("[statistics_linear_regression]: Calculating means for non-proportional regression.");
        xbar = statistics_mean(x, n);
        ybar = statistics_mean(y, n);
        STATISTICS_LOG("[statistics_linear_regression]: xbar = %f, ybar = %f", xbar, ybar);

        double* x_centered = (double*) malloc(n * sizeof(double));
        double* y_centered = (double*) malloc(n * sizeof(double));

        if (x_centered == NULL || y_centered == NULL) {
            STATISTICS_LOG("[statistics_linear_regression]: Error: memory allocation failed.");
            free(x_centered);
            free(y_centered);
            return result;
        }

        STATISTICS_LOG("[statistics_linear_regression]: Centering x and y arrays.");
        for (size_t i = 0; i < n; i++) {
            x_centered[i] = x[i] - xbar;
            y_centered[i] = y[i] - ybar;
            STATISTICS_LOG("[statistics_linear_regression]: x_centered[%zu] = %f, y_centered[%zu] = %f", i, x_centered[i], i, y_centered[i]);
        }

        double sxy = statistics_sumprod(x_centered, y_centered, n);
        double sxx = statistics_sum_of_squares(x_centered, n);
        STATISTICS_LOG("[statistics_linear_regression]: sxy = %f, sxx = %f", sxy, sxx);

        free(x_centered);
        free(y_centered);

        if (sxx == 0.0) {
            STATISTICS_LOG("[statistics_linear_regression]: Error: x is constant.");
            return result;
        }

        result.slope = sxy / sxx;
        result.intercept = ybar - result.slope * xbar;
        STATISTICS_LOG("[statistics_linear_regression]: slope = %f, intercept = %f", result.slope, result.intercept);
    } 
    else {
        STATISTICS_LOG("[statistics_linear_regression]: Performing proportional regression.");
        double sxy = statistics_sumprod(x, y, n);
        double sxx = statistics_sum_of_squares(x, n);
        STATISTICS_LOG("[statistics_linear_regression]: sxy = %f, sxx = %f", sxy, sxx);

        if (sxx == 0.0) {
            STATISTICS_LOG("[statistics_linear_regression]: Error: x is constant.");
            return result;
        }

        result.slope = sxy / sxx;
        result.intercept = 0.0;
        STATISTICS_LOG("[statistics_linear_regression]: slope = %f, intercept = %f", result.slope, result.intercept);
    }

    STATISTICS_LOG("[statistics_linear_regression]: Exiting function with slope = %f, intercept = %f", result.slope, result.intercept);
    return result;
}
