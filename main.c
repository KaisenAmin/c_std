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