#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double dataA[] = {1, 0, 1, 0}; // 2x2 matrix
    Matrix* A = matrix_from_array(dataA, 2, 2);
    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    fmt_printf("Matrix A Size is %zu\n", matrix_size(A));
    matrix_print(A);

    matrix_deallocate(A);
    return 0;
}
