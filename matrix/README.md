# Matrix Lib in C 

## Example 1 : how to create matrix object and set and get value from it also print 

```c
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
```

## Example 2 : how to add two Matrix with `matrix_add`

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <time.h>

void fillMatrix(Matrix *mat) {
    matrix_set(mat, 0, 0, rand() % 10 + 1);
    matrix_set(mat, 0, 1, rand() % 10 + 1);
    matrix_set(mat, 1, 0, rand() % 10 + 1);
    matrix_set(mat, 1, 1, rand() % 10 + 1);
}

int main() {
    srand(time(NULL));
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
```

## Example 3 : subtract two matrix with `matrix_subtract`

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <time.h>

void fillMatrix(Matrix *mat) {
    matrix_set(mat, 0, 0, rand() % 10 + 1);
    matrix_set(mat, 0, 1, rand() % 10 + 1);
    matrix_set(mat, 1, 0, rand() % 10 + 1);
    matrix_set(mat, 1, 1, rand() % 10 + 1);
}

int main() {
    srand(time(NULL));
    Matrix* matrix1 = matrix_create(2, 2);
    Matrix* matrix2 = matrix_create(2, 2);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }

    fillMatrix(matrix1);
    fillMatrix(matrix2);

    Matrix* subtraction = matrix_subtract(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(subtraction);

    matrix_deallocate(subtraction);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}
```