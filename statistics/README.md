# Statistics Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License


## Overview

This Statistics library in C provides a set of functions for calculating mathematical statistics of numeric data. this lib is completly cross platform.

## Installation and Compilation

To use this library, include `statistics.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./statistics/statistics.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like generating random integers within specified ranges.

### Function Descriptions

- `double statistics_mean(const double* data, size_t n)`: this function calculate and return mean (average) of data.
- `double statistics_median(const double* data, size_t n)`: this function calculates the median of the data. this function also sorts the data in ascending order before calculating the median.
- `double statistics_median_low(const double* data, size_t n)`: this function calculates the low median of the given data set. This function also sorts the data in ascending order before calculating the low median.
- `double statistics_median_high(const double* data, size_t n)`: this function calculates the high median of the given data set. This function also sorts the data in ascending order before calculating the high median.
- `double statistics_median_grouped(const double *data, size_t n, double interval)`: this function calculates the median of grouped continuous data, calculated as the 50th percentile. this function treats the data points as continuous data and calculates the 50% percentile median by first finding the median range using specified interval width , and then interpolating within that range using the position of the values from the data set that fall in that range.

- `double statistics_variance(const double* data, size_t n, bool xbar_provided, double xbar)`: this function calculates the sample variance of the data. if xbar_provided is false, the mean is calculated internally; otherwise, the provided mean (xbar) is used.

- `double statistics_stdev(const double* data, size_t n, bool xbar_provided, double xbar)`: this function calculates the standard deviation from a sample of data. if xbar_provided is false, the mean is calculated internally; otherwise, the provided mean (xbar) is used.

- `double statistics_pvariance(const double* data, size_t n, bool mu_provided, double mu)`: this function calculates the variance of an entire population. if mu_provided is false, the mean is calculated internally; otherwise, the provided mean (mu) is used.

- `double statistics_pstdev(const double* data, size_t n, bool mu_provided, double mu)`: this function calculates the standard deviation from an entire population. if mu_provided is false, the mean is calculated internally; otherwise, the provided mean (mu) is used.

- `double statistics_fmean(const double* data, size_t n, const double* weights)`: this function calculate mean value with weight.
- `double statistics_geometric_mean(const double* data, size_t n)`: this function calculate geometric_mean of data.
- `double statistics_harmonic_mean(const double *data, size_t n, const double* weights)`: this function calculate harmonic mean of data.
- `void* statistics_mode(void* data, size_t n, size_t size)`: this function return the most commong data in array of any type.
- `void* statistics_multimode(void* data, size_t n, size_t size, size_t* mode_count)`: this function Return a list of the most frequently occurring values. 
- `double statistics_covariance(const double* x, const double* y, size_t n)`: Return the sample covariance of two inputs *x* and *y*. Covariance
is a measure of the joint variability of two inputs.

- `double statistics_correlation(const double* x, const double* y, size_t n, CorrelationMethod method)`: this function computes the correlation coefficient between two array x and y of length n. It calculates either the Pearson correlation if the method is CORRELATION_LINEAR or the Spearman rank correlation if the method is CORRELATION_RANKED, based on the specified CorrelationMethod enum.

- `LinearRegression statistics_linear_regression(const double* x, const double* y, size_t n, bool proportional)`: this function return the slope and intercept of simple linear regression parameters estimated using ordinary least squares.

## Examples

Several examples are provided to demonstrate the usage of the Statistics library in various scenarios.

### Example 1 : calulate mean or average of data with `statistics_mean`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    double mean = statistics_mean(data, n);
    if (isnan(mean)) {
        fmt_printf("An error occurred while calculating the mean.\n");
    } 
    else {
        fmt_printf("Mean: %f\n", mean);
    }

    return 0;
}
```
**Result:**
```
Mean: 3.000000
```

### Example 2 : calculate the median of data with `statistics_median`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    double median = statistics_median(data, n);
    if (isnan(median)) {
        fmt_printf("An error occurred while calculating the median.\n");
    } 
    else {
        fmt_printf("Median: %f\n", median);
    }

    return 0;
}
```
**Result:**
```
Median: 3.000000
```

### Example 3 : get low median of sequence of data with `statistics_median_low`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data[] = {-11, 5.5, -3.4, 7.1, -9, 22};
    size_t n = sizeof(data) / sizeof(data[0]);

    double median_low = statistics_median_low(data, n);
    if (isnan(median_low)) {
        fmt_printf("An error occurred while calculating the low median.\n");
    } 
    else {
        fmt_printf("Low Median: %.2f\n", median_low);
    }

    return 0;
}
```
**Result:**
```
Low Median: -3.40
```

### Example 4 : calculate high median of data with `statistics_median_high`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data[] = {-11, 5.5, -3.4, 7.1, -9, 22};
    size_t n = sizeof(data) / sizeof(data[0]);

    double median_high = statistics_median_high(data, n);
    if (isnan(median_high)) {
        fmt_printf("An error occurred while calculating the high median.\n");
    } 
    else {
        fmt_printf("High Median: %.2f\n", median_high);
    }

    return 0;
}
```
**Result:**
```
High Median: 5.50
```

### Example 5: Calculate the median of grouped continuous data with `statistics_median_grouped`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data1[] = {1, 2, 3, 4}; 
    size_t n1 = sizeof(data1) / sizeof(data1[0]);
    double interval1 = 1.0;

    double median_grouped1 = statistics_median_grouped(data1, n1, interval1);
    if (isnan(median_grouped1)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 1.0): %.2f\n", median_grouped1); 
    }

   
    double data2[] = {1, 2, 3, 4, 5}; 
    size_t n2 = sizeof(data2) / sizeof(data2[0]);
    double interval2 = 1.0;

    double median_grouped2 = statistics_median_grouped(data2, n2, interval2);
    if (isnan(median_grouped2)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 1.0): %.2f\n", median_grouped2); 
    }

    
    double data3[] = {1, 2, 3, 4}; 
    size_t n3 = sizeof(data3) / sizeof(data3[0]);
    double interval3 = 2.0;

    double median_grouped3 = statistics_median_grouped(data3, n3, interval3);
    if (isnan(median_grouped3)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 2.0): %.2f\n", median_grouped3); 
    }

    double data4[] = {1, 2, 3, 4}; 
    size_t n4 = sizeof(data4) / sizeof(data4[0]);
    double interval4 = 3.0;

    double median_grouped4 = statistics_median_grouped(data4, n4, interval4);
    if (isnan(median_grouped4)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 3.0): %.2f\n", median_grouped4); 
    }


    double data5[] = {1, 2, 3, 4}; 
    size_t n5 = sizeof(data5) / sizeof(data5[0]);
    double interval5 = 5.0;

    double median_grouped5 = statistics_median_grouped(data5, n5, interval5);
    if (isnan(median_grouped5)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 5.0): %.2f\n", median_grouped5); 
    }

    return 0;
}
```
**Result:**
```
Grouped Median (interval 1.0): 2.50
Grouped Median (interval 1.0): 3.00
Grouped Median (interval 2.0): 2.00
Grouped Median (interval 3.0): 1.50
Grouped Median (interval 5.0): 0.50
```

### Example 6 : variance of data with or without xbar `statistics_variance`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data[] = {2, 2.5, 1.25, 3.1, 1.75, 2.8};
    size_t n = sizeof(data) / sizeof(data[0]);
    double var = statistics_variance(data, n, false, 0.0);

    if (!isnan(var)) {
        fmt_printf("Variance: %f\n", var);
    } 
    else {
        fmt_printf("An error occurred while calculating the variance.\n");
    }

    double var_with_mean = statistics_variance(data, n, true, 5);

    if (!isnan(var_with_mean)) {
        fmt_printf("Variance with provided mean: %f\n", var_with_mean);
    } 
    else {
        fmt_printf("An error occurred while calculating the variance with provided mean.\n");
    }

    return 0;
}
```
**Result:**
```
Variance: 0.479667
Variance with provided mean: 9.665000
```

### Example 7 : standar deviation with `statistics_stdev`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data1[] = {1, 30, 50, 100};
    size_t n1 = sizeof(data1) / sizeof(data1[0]);
    double stdev1 = statistics_stdev(data1, n1, true, 20.0);

    if (!isnan(stdev1)) {
        fmt_printf("Standard Deviation: %f\n", stdev1);
    } 
    else {
       fmt_printf("An error occurred while calculating the standard deviation.\n");
    }

    double data2[] = {2, 2.5, 1.25, 3.1, 1.75, 2.8};
    size_t n2 = sizeof(data2) / sizeof(data2[0]);
    double stdev2 = statistics_stdev(data2, n2, false, 0.0);

    if (!isnan(stdev2)) {
        fmt_printf("Standard Deviation: %f\n", stdev2);
    } 
    else {
        fmt_printf("An error occurred while calculating the standard deviation.\n");
    }

    return 0;
}
```
**Result:**
```
Standard Deviation: 50.862560
Standard Deviation: 0.692580
```

### Example 7 : get poplulation variance with `statistics_pvariance`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data1[] = {1, 3, 5, 7, 9, 11};
    size_t n1 = sizeof(data1) / sizeof(data1[0]);
    double pvar1 = statistics_pvariance(data1, n1, true, 20.0);

    fmt_printf("Population Variance with mu : %f\n", pvar1);

    double data2[] = {2, 2.5, 1.25, 3.1, 1.75, 2.8};
    size_t n2 = sizeof(data2) / sizeof(data2[0]);
    double pvar2 = statistics_pvariance(data2, n2, false, 0.0);

    fmt_printf("Population Variance: %f\n", pvar2);

    double data3[] = {-11, 5.5, -3.4, 7.1};
    size_t n3 = sizeof(data3) / sizeof(data3[0]);
    double pvar3 = statistics_pvariance(data3, n3, true, 32.32);

    fmt_printf("Population Variance: %f\n", pvar3);

    double data4[] = {1, 30, 50, 100};
    size_t n4 = sizeof(data4) / sizeof(data4[0]);
    double pvar4 = statistics_pvariance(data4, n4, false, 0.0);

    fmt_printf("Population Variance: %f\n", pvar4);

    return 0;
}
```
**Result:**
```
Population Variance with mu : 207.666667
Population Variance: 0.399722
Population Variance: 1126.975400
Population Variance: 1302.687500
```

### Example 8 : calculate pstdev of sample data with `statistics_pstdev`

```c
#include "statistics/statistics.h"
#include "fmt/fmt.h"

#include <math.h>

int main() {
    double data1[] = {1, 3, 5, 7, 9, 11};
    size_t n1 = sizeof(data1) / sizeof(data1[0]);
    double pstdev1 = statistics_pstdev(data1, n1, false, 0.0);

    if (!isnan(pstdev1)) {
        fmt_printf("Population Standard Deviation: %f\n", pstdev1);
    } 
    else {
        fmt_printf("An error occurred while calculating the population standard deviation.\n");
    }

    double mean = statistics_mean(data1, n1);
    double pstdev2 = statistics_pstdev(data1, n1, true, mean);

    if (!isnan(pstdev2)) {
        fmt_printf("Population Standard Deviation with provided mean: %f\n", pstdev2);
    } 
    else {
        fmt_printf("An error occurred while calculating the population standard deviation with provided mean.\n");
    }

    return 0;
}
```
**Result:**
```
Population Standard Deviation: 3.415650
Population Standard Deviation with provided mean: 3.415650
```

### Example 9 : genereate mena value with weight `statistics_fmean`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>


int main() {
    double data[] = {3.5, 4.0, 5.25};
    double weights[] = {0.2, 0.3, 0.5};
    size_t n = sizeof(data) / sizeof(data[0]);

    double fmean_result = statistics_fmean(data, n, weights);
    if (!isnan(fmean_result)) {
        fmt_printf("Weighted fmean is %lf\n", fmean_result);
    }
    else {
        fmt_printf("result value is NAN");
    }

    double result_without_weights = statistics_fmean(data, n, NULL);

    if (!isnan(result_without_weights)) {
        fmt_printf("Arithmetic mean without weights: %f\n", result_without_weights);
    }
    else {
        fmt_printf("result_without_weights has error.\n");
    }
    
    return 0;
}
```
**Result:**
```
Weighted fmean is 4.525000
Arithmetic mean without weights: 4.250000
```

### Example 10 : calculate the geometric_mean of data with `statistics_geometric_mean`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>


int main() {
    double data[] = {54, 24, 36};
    size_t n = sizeof(data) / sizeof(data[0]);

    double result = statistics_geometric_mean(data, n);

    if (!isnan(result)) {
        fmt_printf("Geometric mean: %f\n", result);
    }
    
    return 0;
}
```
**Result:**
```
Geometric mean: 36.000000
```

### Example 11 : calculation harmonic mean with `statistics_harmonic_mean`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>


int main() {
    double data1[] = {40, 60};
    size_t n1 = sizeof(data1) / sizeof(data1[0]);

    double result_without_weights = statistics_harmonic_mean(data1, n1, NULL);
    if (!isnan(result_without_weights)) {
        fmt_printf("Harmonic mean without weights: %lf\n", result_without_weights);
    }

    double data2[] = {40, 60};
    double weights[] = {5, 30};
    size_t n2 = sizeof(data2) / sizeof(data2[0]);

    double result_with_weights = statistics_harmonic_mean(data2, n2, weights);
    if (!isnan(result_with_weights)) {
        fmt_printf("Harmonic mean with weights: %lf\n", result_with_weights);
    }

    return 0;
}
```
**Result:**
```
Harmonic mean without weights: 48.000000
Harmonic mean with weights: 56.000000
```

### Example 12 : get most common data in array `statistics_mode`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>
#include <stdlib.h>


int main() {
    int data1[] = {1, 1, 2, 3, 3, 3, 3, 4};
    size_t n1 = sizeof(data1) / sizeof(data1[0]);

    int* result_mode_int = (int*)statistics_mode(data1, n1, sizeof(int));
    if (result_mode_int != NULL) {
        fmt_printf("Mode (int): %d\n", *result_mode_int);
        free(result_mode_int);
    } 
    else {
        fmt_printf("Mode (int): Error computing mode\n");
    }

    const char* data2[] = {"red", "blue", "blue", "red", "green", "red", "red"};
    size_t n2 = sizeof(data2) / sizeof(data2[0]);

    char** result_mode_str = (char**)statistics_mode(data2, n2, sizeof(const char*));
    if (result_mode_str != NULL) {
        fmt_printf("Mode (str): %s\n", *result_mode_str);
        free(result_mode_str);
    } 
    else {
        fmt_printf("Mode (str): Error computing mode\n");
    }

    return 0;
}
```
**Result:**
```
Mode (int): 3
Mode (str): red
```

### Example 13 : list of most frequently occurring values `statistics_multimode`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>
#include <stdlib.h>


int main() {
    int data1[] = {1, 1, 2, 3, 3, 3, 3, 4, 4, 4, 4};
    size_t n1 = sizeof(data1) / sizeof(data1[0]);
    size_t mode_count1;

    int* result_mode_int = (int*)statistics_multimode(data1, n1, sizeof(int), &mode_count1);
    if (result_mode_int != NULL) {
        fmt_printf("Multimode (int):");
        for (size_t i = 0; i < mode_count1; i++) {
            fmt_printf(" %d", result_mode_int[i]);
        }
        fmt_printf("\n");

        free(result_mode_int);
    } 
    else {
        fmt_printf("Multimode (int): Error computing mode\n");
    }

    const char* data2[] = {"red", "blue", "blue", "red", "green", "red", "red", "blue"};
    size_t n2 = sizeof(data2) / sizeof(data2[0]);
    size_t mode_count2;

    const char** result_mode_str = (const char**)statistics_multimode(data2, n2, sizeof(const char*), &mode_count2);
    if (result_mode_str != NULL) {
        fmt_printf("Multimode (str):");
        for (size_t i = 0; i < mode_count2; i++) {
            fmt_printf(" %s", result_mode_str[i]);
        }
        fmt_printf("\n");

        free(result_mode_str);
    } 
    else {
        fmt_printf("Multimode (str): Error computing mode\n");
    }
    return 0;
}
```
**Result:**
```
Multimode (int): 3 4
Multimode (str): red
```

### Example 14 : calculate covariance with `statistics_covariance`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>


int main() {
    double x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    double y[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
    size_t n = sizeof(x) / sizeof(x[0]);

    double result_cov_xy = statistics_covariance(x, y, n);
    if (!isnan(result_cov_xy)) {
        fmt_printf("Covariance (x, y): %f\n", result_cov_xy);
    }

    double z[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    double result_cov_xz = statistics_covariance(x, z, n);

    if (!isnan(result_cov_xz)) {
        fmt_printf("Covariance (x, z): %f\n", result_cov_xz);
    }

    double result_cov_zx = statistics_covariance(z, x, n);
    if (!isnan(result_cov_zx)) {
        fmt_printf("Covariance (z, x): %f\n", result_cov_zx);
    }
    
    return 0;
}
```
**Result:**
```
Covariance (x, y): 0.750000
Covariance (x, z): -7.500000
Covariance (z, x): -7.500000
```

### Example 15 : calculate correlation of two x, y `statistics_correlation`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>


int main() {
    double x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    double y[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    size_t n = sizeof(x) / sizeof(x[0]);

    double result_pearson = statistics_correlation(x, y, n, CORRELATION_LINEAR);
    if (!isnan(result_pearson)) {
        fmt_printf("Pearson correlation (x, y): %f\n", result_pearson);
    } 
    else {
        fmt_printf("Error calculating Pearson correlation (x, y).\n");
    }

    double result_spearman = statistics_correlation(x, y, n, CORRELATION_RANKED);
    if (!isnan(result_spearman)) {
        fmt_printf("Spearman correlation (x, y): %f\n", result_spearman);
    } 
    else {
        fmt_printf("Error calculating Spearman correlation (x, y).\n");
    }

    return 0;
}
```
**Result:**
```
Pearson correlation (x, y): -1.000000
Spearman correlation (x, y): -1.000000
```

### Example 16 : Calculate slope and intercept of linear regression with `statistics_linear_regression`

```c
#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>


int main() {
    double x[] = {1, 2, 3, 4, 5};
    double y[] = {3, 6, 9, 12, 15};
    size_t n = sizeof(x) / sizeof(x[0]);
    LinearRegression result = statistics_linear_regression(x, y, n, false);

    fmt_printf("Linear regression (slope, intercept): (%f, %f)\n", result.slope, result.intercept);

    double y_proportional[] = {3, 6, 9, 12, 15};
    result = statistics_linear_regression(x, y_proportional, n, true);

    fmt_printf("Proportional linear regression (slope, intercept): (%f, %f)\n", result.slope, result.intercept);

    return 0;
}
```
**Result:**
```
Linear regression (slope, intercept): (3.000000, 0.000000)
Proportional linear regression (slope, intercept): (3.000000, 0.000000)
```

## Conclusion

The Statistics library in C provides robust functions for calculating various statistical measures such as mean, median, low median, and high median and etc ... These functions are optimized for performance and ease of use, making it an excellent choice for developers needing statistical analysis in their C projects. With clear documentation and examples, the library is straightforward to integrate and apply to different data sets.