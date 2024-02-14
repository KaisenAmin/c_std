#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 4);

    if (!matrix) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }
    
    matrix_set(matrix, 1, 1, 15.32); // row 1 and colon 1
    matrix_print(matrix);

    fmt_printf("Value of matrix in row %zu and col %zu is %lf\n", 1, 1, matrix_get(matrix, 1, 1));

    matrix_deallocate(matrix);
    return 0;
}