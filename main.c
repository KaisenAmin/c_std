#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include "random/random.h"
#include "time/time.h"

void fillMatrix(Matrix *mat) {
    matrix_set(mat, 0, 0, random_randint(1, 10));
    matrix_set(mat, 0, 1, random_randint(1, 10));
    matrix_set(mat, 1, 0, random_randint(1, 10));
    matrix_set(mat, 1, 1, random_randint(1, 10));
}

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    Matrix* matrix1 = matrix_create(2, 2);
    Matrix* matrix2 = matrix_create(2, 2);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }

    fillMatrix(matrix1);
    fillMatrix(matrix2);

    matrix_print(matrix1);
    fmt_printf("\n");
    matrix_print(matrix2);

    Matrix* sum = matrix_add(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(sum);

    matrix_deallocate(sum);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}