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