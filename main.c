#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double values[] = {
        -2.5, 3.1, 5.0,
        7.2, -8.6, 1.0,
        4.5, -9.1, 2.3
    };
    Matrix* matrix = matrix_from_array(values, 3, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Given Matrix:\n");
    matrix_print(matrix);

    double minElement = matrix_min_element(matrix);
    fmt_printf("\nMinimum element in the matrix: %lf\n", minElement);

    double maxElement = matrix_max_element(matrix);
    fmt_printf("Maximum element in the matrix: %lf\n", maxElement);

    matrix_deallocate(matrix);
    return 0;
}
