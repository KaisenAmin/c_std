# Matrix Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

The matrix library provides a comprehensive set of functions for creating, manipulating, and analyzing matrices in C. It includes basic operations like addition, subtraction, and multiplication, as well as advanced features such as decompositions, transformations, and special matrix generation (e.g., identity, Hilbert, and Pascal matrices). The library also supports various matrix checks (e.g., square, symmetric, orthogonal) and utility functions for row and column operations. This versatile tool is designed to facilitate complex mathematical computations with matrices, making it ideal for scientific and engineering applications.

## Compilation

To compile the matrix library along with your main program, use the following GCC command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./matrix/matrix.c 
```

If you need other libraries, you can add them by including their `.c` files.

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Matrix library in your project, include the `matrix.h` header file in your source code:

```c
#include "matrix/matrix.h"
```

## Matrix Library API Overview

### `Matrix* matrix_create(size_t rows, size_t cols);` 
  - **Purpose**: Creates a new matrix with the specified number of rows and columns.
  - **Parameters:**
    - `rows`: Number of rows in the matrix.
    - `cols`: Number of columns in the matrix.
  - **Returns:** A pointer to the newly created matrix.

---

### `void matrix_deallocate(Matrix* matrix);` 
  - **Purpose**: Deallocates the memory associated with the matrix.
  - **Parameters:**
    - `matrix`: Pointer to the matrix to be deallocated.

---

### `Matrix* matrix_create_identity(size_t n);` 
  - **Purpose**: Creates an `n x n` identity matrix where all the diagonal elements are 1, and all other elements are 0.
  - **Parameters:**
    - `n`: Size of the identity matrix (number of rows and columns).
  - **Returns:** A pointer to the newly created identity matrix.

---

### `Matrix* matrix_create_submatrix(const Matrix* matrix, size_t excludeRow, size_t excludeCol);` 
  - **Purpose**: Creates a submatrix by removing the specified row and column from the original matrix.
  - **Parameters:**
    - `matrix`: The original matrix.
    - `excludeRow`: The index of the row to exclude.
    - `excludeCol`: The index of the column to exclude.
  - **Returns:** A pointer to the newly created submatrix.

---

### `Matrix* matrix_from_array(const double* data, size_t rows, size_t cols);` 
  - **Purpose**: Creates a matrix from a 1D array of data, filling it row by row.
  - **Parameters:**
    - `data`: Pointer to the array containing the matrix data.
    - `rows`: Number of rows in the matrix.
    - `cols`: Number of columns in the matrix.
  - **Returns:** A pointer to the newly created matrix.

---

### `Matrix* matrix_add(const Matrix* matrix1, const Matrix* matrix2);` 
  - **Purpose**: Adds two matrices element-wise.
  - **Parameters:**
    - `matrix1`: First matrix.
    - `matrix2`: Second matrix.
  - **Returns:** A new matrix containing the sum of the two matrices.

---

### `Matrix* matrix_subtract(const Matrix* matrix1, const Matrix* matrix2);` 
  - **Purpose**: Subtracts one matrix from another element-wise.
  - **Parameters:**
    - `matrix1`: First matrix.
    - `matrix2`: Second matrix.
  - **Returns:** A new matrix containing the difference of the two matrices.

---

### `Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2);` 
  - **Purpose**: Multiplies two matrices using standard matrix multiplication.
  - **Parameters:**
    - `matrix1`: First matrix.
    - `matrix2`: Second matrix.
  - **Returns:** A new matrix containing the product of the two matrices.

---

### `bool matrix_is_square(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is square (i.e., the number of rows equals the number of columns).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is square, otherwise `false`.

---

### `bool matrix_is_equal(const Matrix* matrix1, const Matrix* matrix2);` 
  - **Purpose**: Checks if two matrices are equal element-wise.
  - **Parameters:**
    - `matrix1`: First matrix.
    - `matrix2`: Second matrix.
  - **Returns:** `true` if the matrices are equal, otherwise `false`.

---

### `bool matrix_is_identity(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is an identity matrix.
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is an identity matrix, otherwise `false`.

---

### `bool matrix_is_idempotent(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is idempotent (i.e., `A * A = A`).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is idempotent, otherwise `false`.

---

### `bool matrix_is_symmetric(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is symmetric (i.e., `A[i][j] == A[j][i]`).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is symmetric, otherwise `false`.

---

### `bool matrix_is_upper_triangular(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is upper triangular.
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is upper triangular, otherwise `false`.

---

### `bool matrix_is_lower_triangular(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is lower triangular.
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is lower triangular, otherwise `false`.

---

### `bool matrix_is_skew_symmetric(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is skew-symmetric (i.e., `A[i][j] == -A[j][i]`).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is skew-symmetric, otherwise `false`.

---

### `bool matrix_is_diagonal(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is diagonal (i.e., all off-diagonal elements are zero).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is diagonal, otherwise `false`.

---

### `bool matrix_is_orthogonal(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is orthogonal (i.e., `A * A^T = I`).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is orthogonal, otherwise `false`.

---

### `bool matrix_is_hankel(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is a Hankel matrix (constant along anti-diagonals).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is a Hankel matrix, otherwise `false`.

---

### `bool matrix_is_toeplitz(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is a Toeplitz matrix (constant along diagonals).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is a Toeplitz matrix, otherwise `false`.

---

### `bool matrix_is_positive_definite(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is positive definite.
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is positive definite, otherwise `false`.

---

### `bool matrix_is_sparse(const Matrix* matrix);` 
  - **Purpose**: Checks if the matrix is sparse (i.e., contains mostly zero elements).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is sparse, otherwise `false`.

---

### `bool matrix_is_row(const Matrix* matrix);` 
  - **Purpose**:- Checks if the matrix is a row matrix (i.e., has only one row).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is a row matrix, otherwise `false`.

---

### `bool matrix_is_columnar(const Matrix* matrix);` 
  - **Purpose**:  Checks if the matrix is a column matrix (i.e., has only one column).
  - **Parameters:**
    - `matrix`: The matrix to check.
  - **Returns:** `true` if the matrix is a column matrix, otherwise `false`.

---

### `bool matrix_lu_decomposition(const Matrix* matrix, Matrix** L, Matrix** U);` 
  - **Purpose**: Performs LU decomposition of the matrix into lower (L) and upper (U) triangular matrices.
  - **Parameters:**
    - `matrix`: The matrix to decompose.
    - `L`: Pointer to the lower triangular matrix.
    - `U`: Pointer to the upper triangular matrix.
  - **Returns:** `true` if the decomposition is successful, otherwise `false`.

---

### `bool matrix_qr_decomposition(const Matrix* A, Matrix** Q, Matrix** R);` 
  - **Purpose**: Performs QR decomposition of the matrix into orthogonal (Q) and upper triangular (R) matrices.
  - **Parameters:**
    - `A`: The matrix to decompose.
    - `Q`: Pointer to the orthogonal matrix.
    - `R`: Pointer to the upper triangular matrix.
  - **Returns:** `true` if the decomposition is successful, otherwise `false`.

---

### `Matrix* matrix_cholesky_decomposition(const Matrix* matrix);` 
  - **Purpose**: Performs Cholesky decomposition of the matrix (must be positive definite).
  - **Parameters:**
    - `matrix`: The matrix to decompose.
  - **Returns:** A new matrix representing the Cholesky factor.

---

### `Matrix* matrix_transpose(const Matrix* matrix);` 
  - **Purpose**: Transposes the matrix (swaps rows and columns).
  - **Parameters:**
    - `matrix`: The matrix to transpose.
  - **Returns:** A new matrix representing the transpose of the input matrix.

---

### `Matrix* matrix_adjugate(const Matrix* matrix);` 
  - **Purpose**: Computes the adjugate (adjoint) of the matrix.
  - **Parameters:**
    - `matrix`: The matrix to compute the adjugate for.
  - **Returns:** A new matrix representing the adjugate.

---

### `Matrix* matrix_inverse(const Matrix* matrix);` 
  - **Purpose**: Computes the inverse of the matrix.
  - **Parameters:**
    - `matrix`: The matrix to invert.
  - **Returns:** A new matrix representing the inverse.

---

### `Matrix* matrix_power(const Matrix* matrix, int power);` 
  - **Purpose**: Raises the matrix to the specified integer power.
  - **Parameters:**
    - `matrix`: The matrix to raise to a power.
    - `power`: The power to raise the matrix to.
  - **Returns:** A new matrix representing the matrix raised to the given power.

---

### `Matrix* matrix_circulant(const Matrix* firstRow);` 
  - **Purpose**: Generates a circulant matrix from the first row. In a circulant matrix, each row is a cyclic shift of the previous row.
  - **Parameters:**
    - `firstRow`: The first row of the circulant matrix.
  - **Returns:** A new circulant matrix.

---

### `Matrix* matrix_helmert(size_t n, bool full);` 
  - **Purpose**: Generates a Helmert matrix, which is an orthogonal matrix used in multivariate statistics.
  - **Parameters:**
    - `n`: Size of the Helmert matrix (number of rows and columns).
    - `full`: If `true`, returns a full Helmert matrix; otherwise, returns a reduced Helmert matrix.
  - **Returns:** A new Helmert matrix.

---

### `Matrix* matrix_fiedler(const Matrix* matrix);` 
  - **Purpose**: Creates a Fiedler matrix, where each element is the absolute difference between two elements of the input matrix.
  - **Parameters:**
    - `matrix`: The matrix to use as a reference for generating the Fiedler matrix.
  - **Returns:** A new Fiedler matrix.

---

### `Matrix* matrix_block_diag(size_t count, ...);` 
  - **Purpose**: Creates a block diagonal matrix from multiple matrices. Each input matrix becomes a block along the diagonal of the resulting matrix.
  - **Parameters:**
    - `count`: The number of matrices to include.
    - `...`: Variable number of matrices.
  - **Returns:** A new block diagonal matrix.

---

### `Matrix* matrix_random(size_t row, size_t col, size_t start, size_t end);` 
  - **Purpose**: Generates a matrix filled with random values within the specified range.
  - **Parameters:**
    - `row`: Number of rows in the matrix.
    - `col`: Number of columns in the matrix.
    - `start`: The minimum random value (inclusive).
    - `end`: The maximum random value (exclusive).
  - **Returns:** A new matrix filled with random values.

---

### `void matrix_print(Matrix* matrix);` 
  - **Purpose**: Prints the matrix to the console in a readable format.
  - **Parameters:**
    - `matrix`: The matrix to print.
  - **Return**: None 

---

### `bool matrix_scalar_multiply(Matrix* matrix, double scalar);` 
  - **Purpose**: Multiplies every element of the matrix by a scalar.
  - **Parameters:**
    - `matrix`: The matrix to scale.
    - `scalar`: The scalar to multiply by.
  - **Returns:** `true` if the operation is successful, otherwise `false`.

---

### `bool matrix_fill(Matrix* matrix, double value);` 
  - **Purpose**: Fills the matrix with a specified value.
  - **Parameters:**
    - `matrix`: The matrix to fill.
    - `value`: The value to fill the matrix with.
  - **Returns:** `true` if the operation is successful, otherwise `false`.

---

### `double* matrix_to_array(const Matrix* matrix);` 
  - **Purpose**: Converts the matrix to a 1D array.
  - **Parameters:**
    - `matrix`: The matrix to convert.
  - **Returns:** A pointer to the newly created array containing the matrix elements.

---

### `size_t matrix_size(const Matrix* matrix);` 
  - **Purpose**: Returns the total number of elements in the matrix.
  - **Parameters:**
    - `matrix`: The matrix to get the size of.
  - **Returns:** The number of elements in the matrix.

---

### `void matrix_swap_rows(Matrix* mat, size_t row1, size_t row2);` 
  - **Purpose**: Swaps two rows in the matrix.
  - **Parameters:**
    - `mat`: The matrix in which to swap rows.
    - `row1`: The index of the first row.
    - `row2`: The index of the second row.

---

### `void matrix_swap_cols(Matrix* mat, size_t col1, size_t col2);` 
  - **Purpose**: Swaps two columns in the matrix.
  - **Parameters:**
    - `mat`: The matrix in which to swap columns.
    - `col1`: The index of the first column.
    - `col2`: The index of the second column.

---

### `void matrix_row_divide(Matrix* matrix, size_t row, double scalar);` 
  - **Purpose**: Divides every element of the specified row by a scalar.
  - **Parameters:**
    - `matrix`: The matrix in which to perform the division.
    - `row`: The index of the row to divide.
    - `scalar`: The scalar to divide by.

---

### `void matrix_row_subtract(Matrix* matrix, size_t targetRow, size_t subtractRow, double scalar);` 
  - **Purpose**: Subtracts a scaled version of one row from another row.
  - **Parameters:**
    - `matrix`: The matrix in which to perform the subtraction.
    - `targetRow`: The row that will be modified.
    - `subtractRow`: The row that will be scaled and subtracted.
    - `scalar`: The scalar to multiply the subtractRow by before subtracting.

---

### `bool matrix_apply_to_row(Matrix* matrix, size_t row, MatrixFunc func);` 
  - **Purpose**: Applies a function to each element of the specified row.
  - **Parameters:**
    - `matrix`: The matrix in which to apply the function.
    - `row`: The index of the row to apply the function to.
    - `func`: The function to apply to each element of the row.
  - **Returns:** `true` if the operation is successful, otherwise `false`.

---

### `bool matrix_apply_to_col(Matrix* matrix, size_t col, MatrixFunc func);` 
  - **Purpose**: Applies a function to each element of the specified column.
  - **Parameters:**
    - `matrix`: The matrix in which to apply the function.
    - `col`: The index of the column to apply the function to.
    - `func`: The function to apply to each element of the column.
  - **Returns:** `true` if the operation is successful, otherwise `false`.

---

### `bool matrix_row_addition(Matrix* matrix, size_t targetRow, size_t sourceRow, double scale);` 
  - **Purpose**: Adds a scaled version of one row to another row.
  - **Parameters:**
    - `matrix`: The matrix in which to perform the addition.
    - `targetRow`: The row that will be modified.
    - `sourceRow`: The row that will be scaled and added.
    - `scale`: The scalar to multiply the sourceRow by before adding.

---

### `bool matrix_col_addition(Matrix* matrix, size_t targetCol, size_t sourceCol, double scale);` 
  - **Purpose**: Adds a scaled version of one column to another column.
  - **Parameters:**
    - `matrix`: The matrix in which to perform the addition.
    - `targetCol`: The column that will be modified.
    - `sourceCol`: The column that will be scaled and added.
    - `scale`: The scalar to multiply the sourceCol by before adding.

---

### `Matrix* matrix_kronecker_product(const Matrix* A, const Matrix* B);` 
  - **Purpose**: Computes the Kronecker product of two matrices.
  - **Parameters:**
    - `A`: First matrix.
    - `B`: Second matrix.
  - **Returns:** A new matrix representing the Kronecker product.

---

### `Matrix* matrix_hankel(const Matrix* firstRow, const Matrix* lastCol);` 
  - **Purpose**: Generates a Hankel matrix from the first row and last column.
  - **Parameters:**
    - `firstRow`: The first row of the Hankel matrix.
    - `lastCol`: The last column of the Hankel matrix.
  - **Returns:** A new Hankel matrix.

---

### `Matrix* matrix_toeplitz(const Matrix* firstRow, const Matrix* firstCol);` 
  - **Purpose**: Generates a Toeplitz matrix from the first row and first column.
  - **Parameters:**
    - `firstRow`: The first row of the Toeplitz matrix.
    - `firstCol`: The first column of the Toeplitz matrix.
  - **Returns:** A new Toeplitz matrix.

---

### `Matrix* matrix_pascal(size_t n);` 
  - **Purpose**: Generates a Pascal matrix of size `n`.
  - **Parameters:**
    - `n`: The size of the Pascal matrix.
  - **Returns:** A new Pascal matrix.

---

### `Matrix* matrix_vandermonde(const Matrix* matrix, size_t n);` 
  - **Purpose**: Generates a Vandermonde matrix from the input matrix.
  - **Parameters:**
    - `matrix`: The input matrix.
    - `n`: The number of columns in the Vandermonde matrix.
  - **Returns:** A new Vandermonde matrix.

---

### `Matrix* matrix_companion(const Matrix* coefficients, size_t n);` 
  - **Purpose**: Generates a companion matrix from the given coefficients.
  - **Parameters:**
    - `coefficients`: The coefficients of the polynomial.
    - `n`: The size of the companion matrix.
  - **Returns:** A new companion matrix.

---

### `Matrix* matrix_walsh(size_t n);` 
  - **Purpose**: Generates a Walsh matrix of size `n`.
  - **Parameters:**
    - `n`: The size of the Walsh matrix.
  - **Returns:** A new Walsh matrix.

---

### `Matrix* matrix_inverse_gauss_jordan(const Matrix* matrix);` 
  - **Purpose**: Computes the inverse of the matrix using the Gauss-Jordan elimination method.
  - **Parameters:**
    - `matrix`: The matrix to invert.
  - **Returns:** A new matrix representing the inverse, or `NULL` if the matrix is not invertible.

---

### `Matrix* matrix_projection(const Matrix* matrix);` 
  - **Purpose**: Computes the projection matrix for the column space of the input matrix.
  - **Parameters:**
    - `matrix`: The matrix to project.
  - **Returns:** A new matrix representing the projection matrix.

---

### `Matrix* matrix_leslie(Matrix* f, size_t f_size, Matrix* s, size_t s_size);` 
  - **Purpose**: Generates a Leslie matrix, commonly used in population models, from fecundity and survival coefficients.
  - **Parameters:**
    - `f`: Fecundity matrix.
    - `f_size`: Size of the fecundity matrix.
    - `s`: Survival matrix.
    - `s_size`: Size of the survival matrix.
  - **Returns:** A new Leslie matrix.

---

### `Matrix* matrix_inverse_hilbert(size_t n);` 
  - **Purpose**: Computes the inverse of a Hilbert matrix of size `n x n`.
  - **Parameters:**
    - `n`: The size of the Hilbert matrix.
  
  - **Returns:** A new matrix representing the inverse of the Hilbert matrix.

---

### `Matrix* matrix_get_row(const Matrix* matrix, size_t row);`
  - **Purpose**: Extracts a specific row from the matrix as a new matrix.
  - **Parameters:**
    - `matrix`: The matrix from which to extract the row.
    - `row`: The index of the row to extract.
  - **Returns:** A new matrix representing the extracted row.

---

### `Matrix* matrix_get_col(const Matrix* matrix, size_t col);`
  - **Purpose**: Extracts a specific column from the matrix as a new matrix.
  - **Parameters:**
    - `matrix`: The matrix from which to extract the column.
    - `col`: The index of the column to extract.
  - **Returns:** A new matrix representing the extracted column.

---

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
**Result:**
```
|  0.00000  0.00000  0.00000  0.00000 |
|  0.00000 15.32000  0.00000  0.00000 |
|  0.00000  0.00000  0.00000  0.00000 |
Value of matrix in row 1 and col 1 is 15.320000
```

---

## Example 2 : how to add two Matrix with `matrix_add`

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include "random/random.h"
#include "time/std_time.h"

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
```
**Result:**
```
| 10.00000  6.00000 |
|  2.00000  4.00000 |

| 4.00000 8.00000 |
| 1.00000 9.00000 |

| 14.00000 14.00000 |
|  3.00000 13.00000 |
```

---

## Example 3 : subtract two matrix with `matrix_subtract`

```c
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

    Matrix* subtraction = matrix_subtract(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(subtraction);

    matrix_deallocate(subtraction);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}
```
**Result:**
```
| -2.00000  5.00000 |
| -7.00000  3.00000 |
```

---

## Example 4 : multiply two Matrix with `matrix_multiply`

```c
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

    Matrix* product = matrix_multiply(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(product);

    matrix_deallocate(product);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}
```
**Result:**
```
| 2.00000 7.00000 |
| 9.00000 6.00000 |

| 1.00000 8.00000 |
| 4.00000 5.00000 |

|  30.00000  51.00000 |
|  33.00000 102.00000 |
```

---

## Example 5 : mutiply elements of Matrix with a scalar 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // create 2 X 3 Matrices
    Matrix* matrix = matrix_create(2, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Cannot create matrix object\n");
        exit(-1);
    }

    matrix_set(matrix, 0, 0, 1.0);
    matrix_set(matrix, 0, 1, 2.0);
    matrix_set(matrix, 0, 2, 3.0);
    matrix_set(matrix, 1, 0, 4.0);
    matrix_set(matrix, 1, 1, 5.0);
    matrix_set(matrix, 1, 2, 6.0);

    fmt_printf("Original matrix:\n");
    matrix_print(matrix);

    double scalar = 2.0;
    if (matrix_scalar_multiply(matrix, scalar)) {
        fmt_printf("\nMatrix after scalar multiplication by %lf:\n", scalar);
        matrix_print(matrix);
    } 
    else {
        fmt_fprintf(stderr, "Scalar multiplication failed\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |

Matrix after scalar multiplication by 2.000000:
| 2.00000  4.00000  6.00000 |
| 8.00000 10.00000 12.00000 |
```

---

## Example 6 : check Matrix is square or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* mat = matrix_create(2, 3);

    if (!mat) {
        fmt_fprintf(stderr, "Can not create matrix");
        exit(-1);
    }

    if (matrix_is_square(mat)) {
        fmt_printf("Matrix is square");
    }
    else {
        fmt_printf("Matrix is not square");
    }
    
    return 0;
}
```
**Result:**
```
Matrix is not square
```

---

## Example 7 : create identity matrix and check is equal with other one or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create two identity matrix of size 3x3
    Matrix* matrix1 = matrix_create_identity(3);
    Matrix* matrix2 = matrix_create_identity(3);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Failed to create identity matrices\n");
        exit(-1); 
    }

    if (matrix_is_equal(matrix1, matrix2)) {
        fmt_printf("Matrix1 and Matrix2 are equal.\n");
    } 
    else {
        fmt_printf("Matrix1 and Matrix2 are not equal.\n");
    }

    matrix_set(matrix1, 2, 2, 2.0); 

    if (matrix_is_equal(matrix1, matrix2)) {
        fmt_printf("After modification, Matrix1 and Matrix2 are still equal.\n");
    } 
    else {
        fmt_printf("After modification, Matrix1 and Matrix2 are not equal.\n");
    }

    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);

    return 0;
}
```
**Result:**
```
Matrix1 and Matrix2 are equal.
After modification, Matrix1 and Matrix2 are not equal.
```

---

## Example 8 : check Matrix is identity or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* identityMatrix = matrix_create_identity(3);
    if (!identityMatrix) {
        fmt_fprintf(stderr, "Failed to create identity matrix\n");
        exit(-1);
    }

    if (matrix_is_identity(identityMatrix)) {
        fmt_printf("The matrix is an identity matrix.\n");
    } 
    else {
        fmt_printf("The matrix is not an identity matrix.\n");
    }

    Matrix* nonIdentityMatrix = matrix_create(3, 3);
    if (!nonIdentityMatrix) {
        fmt_fprintf(stderr, "Failed to create non-identity matrix\n");
        exit(-1);
    }

    matrix_set(nonIdentityMatrix, 0, 0, 1.0);
    matrix_set(nonIdentityMatrix, 1, 1, 1.0);
    matrix_set(nonIdentityMatrix, 2, 2, 0.5);


    if (matrix_is_identity(nonIdentityMatrix)) {
        fmt_printf("The matrix is an identity matrix.\n");
    } 
    else {
        fmt_printf("The matrix is not an identity matrix.\n");
    }

    matrix_deallocate(identityMatrix);
    matrix_deallocate(nonIdentityMatrix);

    return 0;
}
```
**Result:**
```
The matrix is an identity matrix.
The matrix is not an identity matrix.
```

---

## Example 9 : check Matrix is idempotent or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Idempotent matrix
    Matrix* matrix1 = matrix_create(3, 3);
    double data1[] = {-1, 1, 1, -1, 1, 1, -1, 1, 1};
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            matrix_set(matrix1, i, j, data1[i * 3 + j]);
        }
    }

    // Non-idempotent matrix
    Matrix* matrix2 = matrix_create(2, 2);
    double data2[] = {1, 2, 3, 4};
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            matrix_set(matrix2, i, j, data2[i * 2 + j]);
        }
    }

    if (matrix_is_idempotent(matrix1)) {
        fmt_printf("Matrix 1 is idempotent.\n");
    } 
    else {
        fmt_printf("Matrix 1 is not idempotent.\n");
    }

    if (matrix_is_idempotent(matrix2)) {
        fmt_printf("Matrix 2 is idempotent.\n");
    } 
    else {
        fmt_printf("Matrix 2 is not idempotent.\n");
    }

    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);

    return 0;
}
```
**Result:**
```
Matrix 1 is idempotent.
Matrix 2 is not idempotent.
```

---

## Example 10 : check row and columnar Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* rowMatrix = matrix_create(1, 3); 
    Matrix* columnMatrix = matrix_create(3, 1); 
    Matrix* regularMatrix = matrix_create(2, 2);

    if (matrix_is_row(rowMatrix)) {
        fmt_printf("RowMatrix is a row matrix.\n");
    } 
    else {
        fmt_printf("RowMatrix is not a row matrix.\n");
    }

    if (matrix_is_columnar(columnMatrix)) {
        fmt_printf("ColumnMatrix is a columnar matrix.\n");
    } 
    else {
        fmt_printf("ColumnMatrix is not a columnar matrix.\n");
    }

    if (!matrix_is_row(regularMatrix) && !matrix_is_columnar(regularMatrix)) {
        fmt_printf("RegularMatrix is neither a row matrix nor a columnar matrix.\n");
    }

    matrix_deallocate(rowMatrix);
    matrix_deallocate(columnMatrix);
    matrix_deallocate(regularMatrix);

    return 0;
}
```
**Result:**
```
RowMatrix is a row matrix.
ColumnMatrix is a columnar matrix.
RegularMatrix is neither a row matrix nor a columnar matrix.
```

---

## Example 11 : get diagonal Matrix as row or columns 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    Matrix* diagonalMatrix = matrix_get_main_diagonal_as_column(matrix);
    Matrix* diagonalRowMatrix = matrix_get_main_diagonal_as_row(matrix);

    if (diagonalMatrix) {
        fmt_printf("Main diagonal as a column matrix:\n");
        matrix_print(diagonalMatrix); 
    } 
    else {
        fmt_printf("Failed to get the main diagonal as a column matrix.\n");
    }

    if (diagonalRowMatrix) {
        fmt_printf("Main diagonal as a row matrix:\n");
        matrix_print(diagonalRowMatrix);
    } 
    else {
        fmt_printf("Failed to get the main diagonal as a row matrix.\n");
    }

    matrix_deallocate(matrix);
    matrix_deallocate(diagonalMatrix);
    matrix_deallocate(diagonalRowMatrix);

    return 0;
}
```
**Result:**
```
Main diagonal as a column matrix:
| 0 |
| 0 |
| 0 |
Main diagonal as a row matrix:
| 0 0 0 |
```

---

## Example 12 : get minor diagonal as row and columns 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_printf("Failed to create matrix.\n");
        return -1;
    }

    double values[9] = {
        1.0, 2.0, 3.0, // First row
        4.0, 5.0, 6.0, // Second row
        7.0, 8.0, 9.0  // Third row
    };

    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            matrix_set(matrix, i, j, values[i * 3 + j]);
        }
    }

    Matrix* minorDiagonalRow = matrix_get_minor_diagonal_as_row(matrix);
    if (minorDiagonalRow) {
        fmt_printf("Minor diagonal as a row matrix:\n");
        matrix_print(minorDiagonalRow); 
    } 
    else {
        fmt_printf("Failed to get the minor diagonal as a row matrix.\n");
    }

    Matrix* minorDiagonalColumn = matrix_get_minor_diagonal_as_column(matrix);
    if (minorDiagonalColumn) {
        fmt_printf("Minor diagonal as a column matrix:\n");
        matrix_print(minorDiagonalColumn); 
    } 
    else {
        fmt_printf("Failed to get the minor diagonal as a column matrix.\n");
    }

    matrix_deallocate(matrix);
    matrix_deallocate(minorDiagonalRow);
    matrix_deallocate(minorDiagonalColumn);

    return 0;
}
```
**Result:**
```
Minor diagonal as a row matrix:
| 3.00000 5.00000 7.00000 |
Minor diagonal as a column matrix:
| 3.00000 |
| 5.00000 |
| 7.00000 |
```

---

## Example 13 : create transpose Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Cannot create matrix object\n");
        exit(-1);
    }

    matrix_set(matrix, 0, 0, 1.0);
    matrix_set(matrix, 0, 1, 2.0);
    matrix_set(matrix, 0, 2, 3.0);
    matrix_set(matrix, 1, 0, 4.0);
    matrix_set(matrix, 1, 1, 5.0);
    matrix_set(matrix, 1, 2, 6.0);

    fmt_printf("Original matrix:\n");
    matrix_print(matrix);

    Matrix* transposedMatrix = matrix_transpose(matrix);
    if (!transposedMatrix) {
        fmt_fprintf(stderr, "Failed to transpose the matrix\n");
        matrix_deallocate(matrix);
        exit(-1);
    }

    fmt_printf("\nTransposed matrix:\n");
    matrix_print(transposedMatrix);

    matrix_deallocate(matrix);
    matrix_deallocate(transposedMatrix);

    return 0;
}
```
**Result:**
```
Original matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |

Transposed matrix:
| 1.00000 4.00000 |
| 2.00000 5.00000 |
| 3.00000 6.00000 |
```

---

## Example 14 : check Matrix is symmetric or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrixA = matrix_create(4, 4);
    double valuesA[16] = {9, 13, 3, 6, 13, 11, 7, 6, 3, 7, 4, 7, 6, 6, 7, 10};

    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            matrix_set(matrixA, i, j, valuesA[i * 4 + j]);
        }
    }

    if (matrix_is_symmetric(matrixA)) {
        fmt_printf("Matrix A is symmetric.\n");
    } 
    else {
        fmt_printf("Matrix A is not symmetric.\n");
    }

    matrix_deallocate(matrixA);
    return 0;
}
```
**Result:**
```
Matrix A is symmetric.
```

---

## Example 15 : Matrix is upper or lower triangular.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* upperTriangular = matrix_create(3, 3);
    matrix_set(upperTriangular, 0, 0, 1);
    matrix_set(upperTriangular, 0, 1, 2);
    matrix_set(upperTriangular, 0, 2, 3);
    

    Matrix* lowerTriangular = matrix_create(3, 3);
    matrix_set(lowerTriangular, 0, 0, 1);
    matrix_set(lowerTriangular, 1, 0, 2);
    matrix_set(lowerTriangular, 1, 1, 3);
    matrix_set(lowerTriangular, 2, 0, 4);
    matrix_set(lowerTriangular, 2, 1, 5);
    matrix_set(lowerTriangular, 2, 2, 6);
    

    if (matrix_is_upper_triangular(upperTriangular)) {
        fmt_printf("The first matrix is upper triangular.\n");
    } 
    else {
        fmt_printf("The first matrix is not upper triangular.\n");
    }

    if (matrix_is_lower_triangular(lowerTriangular)) {
        fmt_printf("The second matrix is lower triangular.\n");
    } 
    else {
        fmt_printf("The second matrix is not lower triangular.\n");
    }

    matrix_deallocate(upperTriangular);
    matrix_deallocate(lowerTriangular);

    return 0;
}
```
**Result:**
```
The first matrix is upper triangular.
The second matrix is lower triangular.
```

---

## Example 16 : is skew symmetric or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create a 2x2 skew-symmetric matrix
    Matrix* skewSymmetric = matrix_create(2, 2);
    
    matrix_set(skewSymmetric, 0, 0, 0); 
    matrix_set(skewSymmetric, 0, 1, 2); 
    matrix_set(skewSymmetric, 1, 0, -2); 
    matrix_set(skewSymmetric, 1, 1, 0); 

    if (matrix_is_skew_symmetric(skewSymmetric)) {
        fmt_printf("The matrix is skew-symmetric.\n");
    } 
    else {
        fmt_printf("The matrix is not skew-symmetric.\n");
    }

    matrix_deallocate(skewSymmetric);
    return 0;
}
```
**Result:**
```
The matrix is skew-symmetric.
```

---

## Example 17: Determinant of a 2X2 Matrix

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 2);

    matrix_set(matrix, 0, 0, 4); 
    matrix_set(matrix, 0, 1, 7); 
    matrix_set(matrix, 1, 0, 2); 
    matrix_set(matrix, 1, 1, 6); 

    double det = matrix_determinant(matrix);
    fmt_printf("Determinant of the 2x2 matrix is: %lf\n", det);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Determinant of the 2x2 matrix is: 10.000000
```

## Example 18: Determinant of a 3X3 Matrix

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);

    matrix_set(matrix, 0, 0, 6); 
    matrix_set(matrix, 0, 1, 1); 
    matrix_set(matrix, 0, 2, 1); 
    matrix_set(matrix, 1, 0, 4); 
    matrix_set(matrix, 1, 1, -2);
    matrix_set(matrix, 1, 2, 5); 
    matrix_set(matrix, 2, 0, 2); 
    matrix_set(matrix, 2, 1, 8); 
    matrix_set(matrix, 2, 2, 7); 

    double det = matrix_determinant(matrix);
    fmt_printf("Determinant of the 3x3 matrix is: %lf\n", det);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Determinant of the 3x3 matrix is: -306.000000
```

---

## Example 19: Determinant Calculation with Identity Matrix

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create a 4x4 identity matrix
    Matrix* identityMatrix = matrix_create_identity(4);

    double det = matrix_determinant(identityMatrix);
    fmt_printf("Determinant of the 4x4 identity matrix is: %lf\n", det);

    matrix_deallocate(identityMatrix);
    return 0;
}
```
**Result:**
```
Determinant of the 4x4 identity matrix is: 1.000000
```

---

## Example 20 : sum of diagonal elements of matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    
    if (!matrix) {
        fmt_fprintf(stderr, "Cannot create matrix object\n");
        exit(-1);
    }

    matrix_set(matrix, 0, 0, 1.0); 
    matrix_set(matrix, 0, 1, 2.0);
    matrix_set(matrix, 0, 2, 3.0);
    matrix_set(matrix, 1, 0, 4.0);
    matrix_set(matrix, 1, 1, 5.0); 
    matrix_set(matrix, 1, 2, 6.0);
    matrix_set(matrix, 2, 0, 7.0);
    matrix_set(matrix, 2, 1, 8.0);
    matrix_set(matrix, 2, 2, 9.0); 

    fmt_printf("Matrix:\n");
    matrix_print(matrix);

    double trace = matrix_trace(matrix);
    fmt_printf("Trace of the matrix is: %lf\n", trace);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
Trace of the matrix is: 15.000000
```

---

## Example 21 : get inverse of matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 2);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // | 4 7 |
    // | 2 6 |
    matrix_set(matrix, 0, 0, 4);
    matrix_set(matrix, 0, 1, 7);
    matrix_set(matrix, 1, 0, 2);
    matrix_set(matrix, 1, 1, 6);


    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    Matrix* inverseMatrix = matrix_inverse(matrix);
    if (!inverseMatrix) {
        fmt_fprintf(stderr, "Failed to calculate the inverse of the matrix.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nInverse Matrix:\n");
    matrix_print(inverseMatrix);

    matrix_deallocate(matrix);
    matrix_deallocate(inverseMatrix);

    return 0;
}
```
**Result:**
```
Original Matrix:
| 4.00000 7.00000 |
| 2.00000 6.00000 |

Inverse Matrix:
| 0.60000 -0.70000 |
|-0.20000  0.40000 |
```

---

## Example 22: How to raise a square matrix to an integer power also how to copy 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 2);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // | 1 2 |
    // | 3 4 |
    matrix_set(matrix, 0, 0, 1);
    matrix_set(matrix, 0, 1, 2);
    matrix_set(matrix, 1, 0, 3);
    matrix_set(matrix, 1, 1, 4);

    Matrix* matrixCopy = matrix_copy(matrix);
    if (!matrixCopy) {
        fmt_fprintf(stderr, "Failed to copy matrix.\n");
        matrix_deallocate(matrix);
        
        return -1;
    }

    int power = 2;
    Matrix* matrixPowered = matrix_power(matrix, power);
    if (!matrixPowered) {
        fmt_fprintf(stderr, "Failed to raise matrix to power %d.\n", power);
        matrix_deallocate(matrix);
        matrix_deallocate(matrixCopy);

        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    fmt_printf("\nCopied Matrix (should be identical to original):\n");
    matrix_print(matrixCopy);

    fmt_printf("\nMatrix Raised to Power %d:\n", power);
    matrix_print(matrixPowered);

    matrix_deallocate(matrix);
    matrix_deallocate(matrixCopy);
    matrix_deallocate(matrixPowered);

    return 0;
}
```
**Result:**
```
Original Matrix:
| 1.00000 2.00000 |
| 3.00000 4.00000 |

Copied Matrix (should be identical to original):
| 1.00000 2.00000 |
| 3.00000 4.00000 |

Matrix Raised to Power 2:
|  7.00000 10.00000 |
| 15.00000 22.00000 |
```

---

## Example 23 : get rank of matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // | 1 2 3 |
    // | 4 5 6 |
    // | 7 8 9 |
    matrix_set(matrix, 0, 0, 1);
    matrix_set(matrix, 0, 1, 2);
    matrix_set(matrix, 0, 2, 3);
    matrix_set(matrix, 1, 0, 4);
    matrix_set(matrix, 1, 1, 5);
    matrix_set(matrix, 1, 2, 6);
    matrix_set(matrix, 2, 0, 7);
    matrix_set(matrix, 2, 1, 8);
    matrix_set(matrix, 2, 2, 9);

    int rank = matrix_rank(matrix);

    fmt_printf("Matrix:\n");
    matrix_print(matrix);

    fmt_printf("\nRank of the matrix is: %d\n", rank);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

Rank of the matrix is: 2
```

---

## Example 24 : check Matrix is diagonal or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return EXIT_FAILURE;
    }

    matrix_set(matrix, 0, 0, 1.0);
    matrix_set(matrix, 1, 1, 5.0);
    matrix_set(matrix, 2, 2, 9.0);

    fmt_printf("Initial matrix:\n");
    matrix_print(matrix);

    // Check if the matrix is diagonal
    if (matrix_is_diagonal(matrix)) {
        fmt_printf("The matrix is diagonal.\n");
    } 
    else {
        fmt_printf("The matrix is not diagonal.\n");
    }

    // Modify the matrix to make it non-diagonal
    matrix_set(matrix, 0, 1, 2.0);

    fmt_printf("\nModified matrix:\n");
    matrix_print(matrix);

    if (matrix_is_diagonal(matrix)) {
        fmt_printf("The matrix is diagonal.\n");
    } 
    else {
        fmt_printf("The matrix is not diagonal.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Initial matrix:
| 1.00000 0.00000 0.00000 |
| 0.00000 5.00000 0.00000 |
| 0.00000 0.00000 9.00000 |
The matrix is diagonal.

Modified matrix:
| 1.00000 2.00000 0.00000 |
| 0.00000 5.00000 0.00000 |
| 0.00000 0.00000 9.00000 |
The matrix is not diagonal.
```

---

## Example 25 : Matrix is orthogonal or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Orthogonal matrix example: Q = [0 -1; 1 0], Q * Q^T = I
    Matrix* matrix = matrix_create(2, 2);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return EXIT_FAILURE;
    }

    matrix_set(matrix, 0, 0, 0);
    matrix_set(matrix, 0, 1, -1);
    matrix_set(matrix, 1, 0, 1);
    matrix_set(matrix, 1, 1, 0);

    fmt_printf("Original matrix:\n");
    matrix_print(matrix);

    if (matrix_is_orthogonal(matrix)) {
        fmt_printf("The matrix is orthogonal.\n");
    } 
    else {
        fmt_printf("The matrix is not orthogonal.\n");
    }

    // Modify the matrix to make it non-orthogonal
    matrix_set(matrix, 0, 0, 1); 

    fmt_printf("\nModified matrix:\n");
    matrix_print(matrix);

    if (matrix_is_orthogonal(matrix)) {
        fmt_printf("The modified matrix is orthogonal.\n");
    } 
    else {
        fmt_printf("The modified matrix is not orthogonal.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original matrix:
|  0.00000 -1.00000 |
|  1.00000  0.00000 |
The matrix is orthogonal.

Modified matrix:
|  1.00000 -1.00000 |
|  1.00000  0.00000 |
The modified matrix is not orthogonal.
```

---

## Example 26 : product matrix Kronecker 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Define matrix A (2x2)
    Matrix* A = matrix_create(2, 2);
    matrix_set(A, 0, 0, 1); 
    matrix_set(A, 0, 1, 2); 
    matrix_set(A, 1, 0, 3); 
    matrix_set(A, 1, 1, 4); 

    // Define matrix B (2x2)
    Matrix* B = matrix_create(2, 2);
    matrix_set(B, 0, 0, 0); 
    matrix_set(B, 0, 1, 5); 
    matrix_set(B, 1, 0, 6); 
    matrix_set(B, 1, 1, 7); 

    Matrix* C = matrix_kronecker_product(A, B);
    if (!C) {
        fmt_fprintf(stderr, "Failed to compute the Kronecker product.\n");
        matrix_deallocate(A);
        matrix_deallocate(B);
        
        return -1;
    }

    fmt_printf("Matrix A:\n");
    matrix_print(A);

    fmt_printf("\nMatrix B:\n");
    matrix_print(B);

    fmt_printf("\nKronecker product of A and B:\n");
    matrix_print(C);

    matrix_deallocate(A);
    matrix_deallocate(B);
    matrix_deallocate(C);

    return 0;
}
```
**Result:**
```
Matrix A:
| 1.00000 2.00000 |
| 3.00000 4.00000 |

Matrix B:
| 0.00000 5.00000 |
| 6.00000 7.00000 |

Kronecker product of A and B:
|  0.00000  5.00000  0.00000 10.00000 |
|  6.00000  7.00000 12.00000 14.00000 |
|  0.00000 15.00000  0.00000 20.00000 |
| 18.00000 21.00000 24.00000 28.00000 |
```

---

## Example 27 : create hankel Matrix from first row and last col 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double firstRowData[] = {1, 2, 3, 4};
    Matrix* firstRow = matrix_create(1, 4);
    for (size_t i = 0; i < 4; i++) {
        matrix_set(firstRow, 0, i, firstRowData[i]);
    }

    double lastColData[] = {4, 5, 6, 7};
    Matrix* lastCol = matrix_create(4, 1);
    for (size_t i = 0; i < 4; i++) {
        matrix_set(lastCol, i, 0, lastColData[i]);
    }

    // Generate the Hankel matrix
    Matrix* hankelMatrix = matrix_hankel(firstRow, lastCol);
    if (hankelMatrix) {
        fmt_printf("Hankel Matrix:\n");
        matrix_print(hankelMatrix);
    } 
    else {
        fmt_fprintf(stderr, "Failed to generate Hankel matrix.\n");
    }

    matrix_deallocate(firstRow);
    matrix_deallocate(lastCol);
    matrix_deallocate(hankelMatrix);

    return EXIT_SUCCESS;
}
```
**Result:**
```
Hankel Matrix:
| 1.00000 2.00000 3.00000 4.00000 |
| 2.00000 3.00000 4.00000 5.00000 |
| 3.00000 4.00000 5.00000 6.00000 |
| 4.00000 5.00000 6.00000 7.00000 |
```

---

## Example 28 : check matrix is hankle or not 

```c
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
```
**Result:**
```
Matrix:
| 1.00000 2.00000 3.00000 4.00000 |
| 2.00000 3.00000 4.00000 3.00000 |
| 3.00000 4.00000 3.00000 4.00000 |
| 4.00000 3.00000 4.00000 5.00000 |
The matrix is Hankel.
```

---

## Example 29 : create matrix from array then create toeplitz matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double firstRowData[] = {1, 4, 5, 6};
    double firstColData[] = {1, 2, 3};

    Matrix* firstRow = matrix_from_array(firstRowData, 1, sizeof(firstRowData) / sizeof(firstRowData[0]));
    Matrix* firstCol = matrix_from_array(firstColData, sizeof(firstColData) / sizeof(firstColData[0]), 1);

    fmt_printf("First row\n");
    matrix_print(firstRow);

    fmt_printf("First Col\n");
    matrix_print(firstCol);
    
    Matrix* toeplitzMatrix = matrix_toeplitz(firstRow, firstCol);

    fmt_printf("Toeplitz Matrix:\n");
    matrix_print(toeplitzMatrix);

    matrix_deallocate(firstRow);
    matrix_deallocate(firstCol);
    matrix_deallocate(toeplitzMatrix);

    return 0;
}
```
**Result:**
```
First row
| 1.00000 4.00000 5.00000 6.00000 |
First Col
| 1.00000 |
| 2.00000 |
| 3.00000 |
Toeplitz Matrix:
| 1.00000 4.00000 5.00000 6.00000 |
| 2.00000 1.00000 4.00000 5.00000 |
| 3.00000 2.00000 1.00000 4.00000 |
```

---

## Example 30 : create circulant matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double firstRowData[] = {1, 2, 3};
    Matrix* firstRow = matrix_from_array(firstRowData, 1, sizeof(firstRowData) / sizeof(firstRowData[0]));

    Matrix* circulantMatrix = matrix_circulant(firstRow);

    fmt_printf("Circulan Matrix:\n");
    matrix_print(circulantMatrix);

    matrix_deallocate(firstRow);
    matrix_deallocate(circulantMatrix);

    return 0;
}
```
**Result:**
```
Circulan Matrix:
| 1.00000 2.00000 3.00000 |
| 2.00000 3.00000 1.00000 |
| 3.00000 1.00000 2.00000 |
```

---

## Example 31 : create hilbert Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* hilbert = matrix_hilbert(3);

    fmt_printf("Hilbert Matrix : \n");
    matrix_print(hilbert);


    matrix_deallocate(hilbert);
    return 0;
}
```
**Result:**
```
Hilbert Matrix : 
| 1.00000 0.50000 0.33333 |
| 0.50000 0.33333 0.25000 |
| 0.33333 0.25000 0.20000 |
```

---

## Example 32 : create helmert matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* helmert = matrix_helmert(5, true);

    fmt_printf("Helmert Matrix : \n");
    matrix_print(helmert);

    matrix_deallocate(helmert);
    return 0;
}
```
**Result:**
```
Helmert Matrix : 
| 0.44721  0.44721  0.44721  0.44721  0.44721 |
| 0.70711 -0.70711        0        0        0 |
| 0.40825  0.40825 -0.81650        0        0 |
| 0.28868  0.28868  0.28868 -0.86603        0 |
| 0.22361  0.22361  0.22361  0.22361 -0.89443 |
```

---

## Example 33 : Cofactor of Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double arr[] = {1, 9, 3, 2, 5, 4, 3, 7, 8};
    Matrix* mat = matrix_from_array(arr,3, 3);
    Matrix* cofactor = matrix_cofactor(mat);

    fmt_printf("Cofactor of Matrix : \n");
    matrix_print(cofactor);

    matrix_deallocate(mat);
    matrix_deallocate(cofactor);
    return 0;
}
```
**Result:**
```
Cofactor of Matrix : 
|  12.00000  -4.00000  -1.00000 |
| -51.00000  -1.00000  20.00000 |
|  21.00000   2.00000 -13.00000 |
```

---

## Example 34 : calculate cholesky decomposition of a Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    size_t n = 3; 

    Matrix* matrix = matrix_from_array(data, n, n);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    Matrix* chol = matrix_cholesky_decomposition(matrix);
    if (!chol) {
        fmt_fprintf(stderr, "Cholesky decomposition failed.\n");
        matrix_deallocate(matrix); 
        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    fmt_printf("Cholesky Decomposition (Upper Triangular Matrix):\n");
    matrix_print(chol);

    matrix_deallocate(matrix);
    matrix_deallocate(chol);

    return 0;
}
```
**Result:**
```
Original Matrix:
|   4.00000  12.00000 -16.00000 |
|  12.00000  37.00000 -43.00000 |
| -16.00000 -43.00000  98.00000 |
Cholesky Decomposition (Upper Triangular Matrix):
|  2.00000  6.00000 -8.00000 |
|  0.00000  1.00000  5.00000 |
|  0.00000  0.00000  3.00000 |
```

---

## Example 35 : LU decomposition 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double A_data[] = {4, 3, 2, 3, 2, 1, 2, 1, 3};
    size_t n = 3; 
    Matrix* A = matrix_from_array(A_data, n, n);

    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    Matrix *L, *U;
    bool success = matrix_lu_decomposition(A, &L, &U);
    if (!success) {
        fmt_fprintf(stderr, "LU decomposition failed.\n");
        matrix_deallocate(A);
        return -1;
    }

    fmt_printf("Original Matrix A:\n");
    matrix_print(A);

    fmt_printf("Lower Triangular Matrix L:\n");
    matrix_print(L);

    fmt_printf("Upper Triangular Matrix U:\n");
    matrix_print(U);

    matrix_deallocate(A);
    matrix_deallocate(L);
    matrix_deallocate(U);

    return 0;
}
```
**Result:**
```
Original Matrix A:
| 4.00000 3.00000 2.00000 |
| 3.00000 2.00000 1.00000 |
| 2.00000 1.00000 3.00000 |
Lower Triangular Matrix L:
| 1.00000 0.00000 0.00000 |
| 0.75000 1.00000 0.00000 |
| 0.50000 2.00000 1.00000 |
Upper Triangular Matrix U:
| 4.00000  3.00000  2.00000 |
| 0.00000 -0.25000 -0.50000 |
| 0.00000  0.00000  3.00000 |
```

---

## Example 36 : QR decomposition 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double A_data[] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    size_t n = 3;
    Matrix* A = matrix_from_array(A_data, n, n);
    
    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    Matrix *Q, *R;
    bool success = matrix_qr_decomposition(A, &Q, &R);
    if (!success) {
        fmt_fprintf(stderr, "QR decomposition failed.\n");
        matrix_deallocate(A); 
        return -1;
    }

    fmt_printf("Original Matrix A:\n");
    matrix_print(A);

    fmt_printf("Orthogonal Matrix Q:\n");
    matrix_print(Q);

    fmt_printf("Upper Triangular Matrix R:\n");
    matrix_print(R);

    matrix_deallocate(A);
    matrix_deallocate(Q);
    matrix_deallocate(R);

    return 0;
}
```
**Result:**
```
Original Matrix A:
| 4.00000  12.00000 -16.00000 |
| 12.00000  37.00000 -43.00000 |
| -16.00000 -43.00000  98.00000 |
Orthogonal Matrix Q:
|  0.19612  0.16948  0.96582 |
|  0.58835  0.76762 -0.25416 |
| -0.78446  0.61809  0.05083 |
Upper Triangular Matrix R:
| 20.39608   57.85426 -105.31436 |
| 0.00000    3.85806   24.85307 |
| 0.00000    0.00000    0.45750 |
```

---

## Example 37 : Pascal matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* pascal = matrix_pascal(5);

    fmt_printf("Pascal Matrix : \n");
    matrix_print(pascal);

    matrix_deallocate(pascal);
    return 0;
}
```
**Result:**
```
Pascal Matrix : 
| 1.00000  1.00000  1.00000  1.00000  1.00000 |
| 1.00000  2.00000  3.00000  4.00000  5.00000 |
| 1.00000  3.00000  6.00000 10.00000 15.00000 |
| 1.00000  4.00000 10.00000 20.00000 35.00000 |
| 1.00000  5.00000 15.00000 35.00000 70.00000 |
```

---

## Example 38 : different kind of Norm 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {1, -2, 3, -4, 5, -6, 7, -8, 9};
    Matrix* matrix = matrix_from_array(data, 3, 3);
    
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // Compute norms
    double frobeniusNorm = matrix_frobenius_norm(matrix);
    double l1Norm = matrix_l1_norm(matrix);
    double infinityNorm = matrix_infinity_norm(matrix);


    fmt_printf("Frobenius Norm: %lf\n", frobeniusNorm);
    fmt_printf("L1 Norm: %lf\n", l1Norm);
    fmt_printf("Infinity Norm: %lf\n", infinityNorm);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Frobenius Norm: 16.881943
L1 Norm: 18.000000
Infinity Norm: 24.000000
```

---

## Example 39 : Matrix is positive definite or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* posDefMatrix = matrix_create(2, 2);
    matrix_set(posDefMatrix, 0, 0, 2);  
    matrix_set(posDefMatrix, 0, 1, -1);
    matrix_set(posDefMatrix, 1, 0, -1);
    matrix_set(posDefMatrix, 1, 1, 2);

    // Create a non-positive definite matrix (symmetric but not positive definite)
    Matrix* nonPosDefMatrix = matrix_create(2, 2);
    matrix_set(nonPosDefMatrix, 0, 0, 0);  
    matrix_set(nonPosDefMatrix, 0, 1, 1);
    matrix_set(nonPosDefMatrix, 1, 0, 1);
    matrix_set(nonPosDefMatrix, 1, 1, 0);

    if (matrix_is_positive_definite(posDefMatrix)) {
        fmt_printf("The matrix is positive definite.\n");
    } 
    else {
        fmt_printf("The matrix is not positive definite.\n");
    }

    if (matrix_is_positive_definite(nonPosDefMatrix)) {
        fmt_printf("The matrix is positive definite.\n");
    } 
    else {
        fmt_printf("The matrix is not positive definite.\n");
    }

    matrix_deallocate(posDefMatrix);
    matrix_deallocate(nonPosDefMatrix);

    return 0;
}
```
**Result:**
```
The matrix is positive definite.
The matrix is not positive definite.
```

---

## Example 40 : generate projection of a Matrix 

```c 
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double matrix[] = {
        1, 2,
        3, 4,
        5, 6
    };
    size_t rows = 3;
    size_t cols = 2;
    Matrix* A = matrix_from_array(matrix, rows, cols);

    if (!A) {
        fmt_printf("Failed to create matrix A.\n");
        return -1;
    }

    fmt_printf("Matrix A:\n");
    matrix_print(A);

    // Compute the projection matrix P for the column space of A
    Matrix* projection = matrix_projection(A);
    if (!projection) {
        fmt_printf("Failed to compute the projection matrix.\n");
        matrix_deallocate(A); 
        return -1;
    }

    fmt_printf("\nProjection Matrix P for the column space of A:\n");
    matrix_print(projection);

    matrix_deallocate(A);
    matrix_deallocate(projection);

    return 0;
}
``` 
**Result:**
```
Matrix A:
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |

Projection Matrix P for the column space of A:
|  0.83333  0.33333 -0.16667 |
|  0.33333  0.33333  0.33333 |
| -0.16667  0.33333  0.83333 |
```

---

## Example 41 : create vandermond matrix from give matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {4, 3, 1};
    Matrix* matrix = matrix_from_array(data, 1, 3);
    Matrix* vander = matrix_vandermonde(matrix, 3);

    if (!matrix || !vander) {
        fmt_fprintf(stderr, "vander creation or generate matrix from array failed\n");
        return -1;
    }

    fmt_printf("Vandermond Matrix : \n");
    matrix_print(vander);

    matrix_deallocate(matrix);
    matrix_deallocate(vander);

    return 0;   
}
```
**Result:**
```
Vandermond Matrix : 
|  1.00000  4.00000 16.00000 |
|  1.00000  3.00000  9.00000 |
|  1.00000  1.00000  1.00000 |
```

---

## Example 42 : create companion matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {1, 2, 3};
    Matrix* matrix = matrix_from_array(data, 1, 3);
    Matrix* companion = matrix_companion(matrix, 3);

    fmt_printf("Companion Matrix : \n");
    matrix_print(companion);

    matrix_deallocate(companion);
    matrix_deallocate(matrix);
    return 0;   
}
```
**Result:**
```
Companion Matrix : 
|  0.00000 -0.66667 |
|  1.00000 -0.33333 |
```

---

## Example 43 : How to apply a function to matrix_map also fill and matrix with prefer value 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

double sine_of_element(double x) {
    return sin(x);
}

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // Fill the matrix with the value PI / 4
    if (!matrix_fill(matrix, 3.141596 / 4)) {
        fmt_fprintf(stderr, "Failed to fill matrix.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("Matrix after filling with PI / 4:\n");
    matrix_print(matrix);

    // Apply the sine function to each element of the matrix
    Matrix* resultMatrix = matrix_map(matrix, sine_of_element);
    if (!resultMatrix) {
        fmt_fprintf(stderr, "Failed to apply function to matrix.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nMatrix after applying the sine function:\n");
    matrix_print(resultMatrix);

    matrix_deallocate(matrix);
    matrix_deallocate(resultMatrix);

    return 0;
}
```
**Result:**
```
Matrix after filling with PI / 4:
| 0.78540 0.78540 0.78540 |
| 0.78540 0.78540 0.78540 |
| 0.78540 0.78540 0.78540 |

Matrix after applying the sine function:
| 0.70711 0.70711 0.70711 |
| 0.70711 0.70711 0.70711 |
| 0.70711 0.70711 0.70711 |
```

---

## Example 44 : how to use matrix_row_addition and matrix_col_addition 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"


int main() {
    double values[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    Matrix* matrix = matrix_from_array(values, 3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    // Add the first row to the second row
    if (matrix_row_addition(matrix, 1, 0, 1.0)) {
        fmt_printf("\nMatrix after adding first row to the second row:\n");
        matrix_print(matrix);
    } 
    else {
        fmt_fprintf(stderr, "Failed to add rows.\n");
    }

    // Add the first column to the third column, scaling the first column by 2
    if (matrix_col_addition(matrix, 2, 0, 2.0)) {
        fmt_printf("\nMatrix after adding first column (scaled by 2) to the third column:\n");
        matrix_print(matrix);
    } 
    else {
        fmt_fprintf(stderr, "Failed to add columns.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

Matrix after adding first row to the second row:
| 1.00000 2.00000 3.00000 |
| 5.00000 7.00000 9.00000 |
| 7.00000 8.00000 9.00000 |

Matrix after adding first column (scaled by 2) to the third column:
|  1.00000  2.00000  5.00000 |
|  5.00000  7.00000 19.00000 |
|  7.00000  8.00000 23.00000 |
```

---

## Example 45 : Apply function for specefic row and col 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

double square(double x) {
    return x * x;
}

int main() {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix* matrix = matrix_from_array(values, 3, 3); 

    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    // Apply 'square' function to the second row (index 1)
    if (!matrix_apply_to_row(matrix, 1, square)) {
        fmt_fprintf(stderr, "Failed to apply function to row.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nMatrix after applying 'square' function to the second row:\n");
    matrix_print(matrix);

    // Apply 'square' function to the first column (index 0)
    if (!matrix_apply_to_col(matrix, 0, square)) {
        fmt_fprintf(stderr, "Failed to apply function to column.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nMatrix after applying 'square' function to the first column:\n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

Matrix after applying 'square' function to the second row:
|  1.00000  2.00000  3.00000 |
| 16.00000 25.00000 36.00000 |
|  7.00000  8.00000  9.00000 |

Matrix after applying 'square' function to the first column:
|   1.00000   2.00000   3.00000 |
| 256.00000  25.00000  36.00000 |
|  49.00000   8.00000   9.00000 |
```

---

## Example 46 : finding minimum and maximum element of a Matrix 

```c
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
```
**Result:**
```
Given Matrix:
| -2.50000  3.10000  5.00000 |
|  7.20000 -8.60000  1.00000 |
|  4.50000 -9.10000  2.30000 |

Minimum element in the matrix: -9.100000
Maximum element in the matrix: 7.200000
```

---

## Example 47 : create leslie matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double f[] = {0.1, 2.0, 1.0, 0.1}; // Fecundity coefficients
    double s[] = {0.2, 0.8, 0.7};      // Survival coefficients
    size_t f_size = sizeof(f) / sizeof(f[0]);
    size_t s_size = sizeof(s) / sizeof(s[0]);

    Matrix* fl = matrix_from_array(f, 1, 3);
    Matrix* sl = matrix_from_array(s, 1, 3);
    
    if (!fl || !sl) {
        fmt_fprintf(stderr, "Error: can not create Matrix from array fl or sl or both of them");
        return -1;
    }

    Matrix* leslieMatrix = matrix_leslie(fl, f_size, sl, s_size);
    if (leslieMatrix) {
        fmt_printf("Leslie Matrix:\n");
        matrix_print(leslieMatrix);
        matrix_deallocate(leslieMatrix);

    }

    matrix_deallocate(fl);
    matrix_deallocate(sl);
    return 0;
}
```
**Result:**
```
Leslie Matrix:
| 0.10000 2.00000 1.00000 0.00000 |
| 0.20000 0.00000 0.00000 0.00000 |
| 0.00000 0.80000 0.00000 0.00000 |
| 0.00000 0.00000 0.70000 0.00000 |
```

---

## Example 48 : create inverse hilbert Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* invHilbert = matrix_inverse_hilbert(4);

    if (!invHilbert) {
        fmt_fprintf(stderr, "Error: can not create inverse hilbert matrix.\n");
        return -1;
    }
    
    fmt_printf("Inverse Hilbert Matrix : \n");
    matrix_print(invHilbert);

    matrix_deallocate(invHilbert);
    return 0;
}
```
**Result:**
```
Inverse Hilbert Matrix : 
|    16.00000  -120.00000   240.00000  -140.00000 |
|  -120.00000  1200.00000 -2700.00000  1680.00000 |
|   240.00000 -2700.00000  6480.00000 -4200.00000 |
|  -140.00000  1680.00000 -4200.00000  2800.00000 |
```

---

## Example 49 : get row or col of Matrix as Matrix object 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix* matrix = matrix_from_array(values, 3, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix : \n");
    matrix_print(matrix);

    Matrix* row = matrix_get_row(matrix, 1);
    if (!row) {
        fmt_fprintf(stderr, "Row Matrix is null and invalid.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("row of matrix : \n");
    matrix_print(row);

    Matrix* col = matrix_get_col(matrix, 2);
    if (!col) {
        fmt_fprintf(stderr, "Col Matrix is null and invalid.\n");
        matrix_deallocate(row);
        matrix_deallocate(matrix);
        return -1;
    };

    fmt_printf("col of matrix : \n");
    matrix_print(col);

    matrix_deallocate(col);
    matrix_deallocate(row);
    matrix_deallocate(matrix);

    return 0;
}
```
**Result:**
```
Original Matrix : 
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
row of matrix :
| 4.00000 5.00000 6.00000 |
col of matrix :
| 3.00000 |
| 6.00000 |
| 9.00000 |
```

---

## Example 50 : Matrix to double Array 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix* matrix = matrix_from_array(values, 3, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix : \n");
    matrix_print(matrix);

    double* data = matrix_to_array(matrix);
    fmt_printf("\n");

    for (size_t i = 0; i < (matrix->rows * matrix->cols); i++) {
        fmt_printf("%lf ", data[i]);
    }

    free(data);
    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original Matrix : 
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

1.000000 2.000000 3.000000 4.000000 5.000000 6.000000 7.000000 8.000000 9.000000
```

---

## Example 51 : get size of Matrix 

```c
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
```
**Result:**
```
Matrix A Size is 4
| 1.00000 0.00000 |
| 1.00000 0.00000 |
```

---

## Example 52 : create random Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_random(3, 3, 1, 10);

    if (!matrix) {
        fmt_printf("Error in creation matrix object");
        return -1;        
    }

    fmt_printf("Matrix random is : \n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix random is : 
| 7.00000 3.00000 7.00000 |
| 1.00000 1.00000 7.00000 |
| 9.00000 5.00000 5.00000 |
```

---

## Example 53 : create walsh matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_walsh(8);

    if (!matrix) {
        fmt_printf("Error in creation matrix object");
        return -1;        
    }

    fmt_printf("Matrix walsh is : \n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix walsh is : 
|  1.00000  1.00000  1.00000  1.00000  1.00000  1.00000  1.00000  1.00000 |
|  1.00000 -1.00000  1.00000 -1.00000  1.00000 -1.00000  1.00000 -1.00000 |
|  1.00000  1.00000 -1.00000 -1.00000  1.00000  1.00000 -1.00000 -1.00000 |
|  1.00000 -1.00000 -1.00000  1.00000  1.00000 -1.00000 -1.00000  1.00000 |
|  1.00000  1.00000  1.00000  1.00000 -1.00000 -1.00000 -1.00000 -1.00000 |
|  1.00000 -1.00000  1.00000 -1.00000 -1.00000  1.00000 -1.00000  1.00000 |
|  1.00000  1.00000 -1.00000 -1.00000 -1.00000 -1.00000  1.00000  1.00000 |
|  1.00000 -1.00000 -1.00000  1.00000 -1.00000  1.00000  1.00000 -1.00000 |
```

---

## License

This project is open-source and available under [ISC License].