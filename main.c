#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_random(10, 10, 1, 10);

    if (!matrix) {
        fmt_printf("Error in creation matrix object");
        return -1;        
    }

    fmt_printf("Matrix random is : \n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
