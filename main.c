#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    size_t n = 4; 
    Matrix* matrix = matrix_create(n, n);
    
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return EXIT_FAILURE;
    }

    double values[] = {1, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7};
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i + j < n) {
                matrix_set(matrix, i, j, values[i + j]);
            } 
            else {
                matrix_set(matrix, i, j, values[n - 1 + (i + j - n + 1)]);
            }
        }
    }

    fmt_printf("Matrix:\n");
    matrix_print(matrix);

    if (matrix_is_hankel(matrix)) {
        fmt_printf("The matrix is Hankel.\n");
    } 
    else {
        fmt_printf("The matrix is not Hankel.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}