#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_walsh(16);

    if (!matrix) {
        fmt_printf("Error in creation matrix object");
        return -1;        
    }

    fmt_printf("Matrix walsh is : \n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
