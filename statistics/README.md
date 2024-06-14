# Statistics Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License


## Overview

This Statistics library in C provides a set of functions for calculating mathematical statistics of numeric data.

## Installation and Compilation

To use this library, include `statistics.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./statistics/statistics.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like generating random integers within specified ranges.

### Function Descriptions

- `double statistics_mean(double* data, size_t n)`: this function calculate and return mean (average) of data.
- `double statistics_median(double* data, size_t n)`: this function calculates the median of the data. this function also sorts the data in ascending order before calculating the median.
- `double statistics_median_low(double* data, size_t n)`: this function calculates the low median of the given data set. This function also sorts the data in ascending order before calculating the low median.
- `double statistics_median_high(double* data, size_t n)`: this function calculates the high median of the given data set. This function also sorts the data in ascending order before calculating the high median.
- `double statistics_median_grouped(double *data, size_t n, double interval)`: this function calculates the median of grouped continuous data, calculated as the 50th percentile. this function treats the data points as continuous data and calculates the 50% percentile median by first finding the median range using specified interval width , and then interpolating within that range using the position of the values from the data set that fall in that range.


## Examples

Several examples are provided to demonstrate the usage of the Random library in various scenarios.

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
        fmt_printf("Grouped Median (interval 1.0): %.2f\n", median_grouped1); // Should print 2.5
    }

   
    double data2[] = {1, 2, 3, 4, 5}; 
    size_t n2 = sizeof(data2) / sizeof(data2[0]);
    double interval2 = 1.0;

    double median_grouped2 = statistics_median_grouped(data2, n2, interval2);
    if (isnan(median_grouped2)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 1.0): %.2f\n", median_grouped2); // Should print 3.0
    }

    
    double data3[] = {1, 2, 3, 4}; 
    size_t n3 = sizeof(data3) / sizeof(data3[0]);
    double interval3 = 2.0;

    double median_grouped3 = statistics_median_grouped(data3, n3, interval3);
    if (isnan(median_grouped3)) {
        fmt_printf("An error occurred while calculating the grouped median.\n");
    } 
    else {
        fmt_printf("Grouped Median (interval 2.0): %.2f\n", median_grouped3); // Should print 2.0
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
## Conclusion

The Statistics library in C provides robust functions for calculating various statistical measures such as mean, median, low median, and high median and etc ... These functions are optimized for performance and ease of use, making it an excellent choice for developers needing statistical analysis in their C projects. With clear documentation and examples, the library is straightforward to integrate and apply to different data sets.