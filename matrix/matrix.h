#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// #define MATRIX_LOGGING_ENABLE 

// structure for Matrix 
typedef struct Matrix {
    size_t rows;
    size_t cols;
    double* data;
} Matrix;

Matrix* matrix_create(size_t rows, size_t cols);
Matrix* matrix_add(const Matrix* matrix1, const Matrix* matrix2);
Matrix* matrix_subtract(const Matrix* matrix1, const Matrix* matrix2);
Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2);
Matrix* matrix_create_identity(size_t n);
Matrix* matrix_get_main_diagonal_as_column(const Matrix* matrix);
Matrix* matrix_get_main_diagonal_as_row(const Matrix* matrix);
Matrix* matrix_get_minor_diagonal_as_row(const Matrix* matrix);
Matrix* matrix_get_minor_diagonal_as_column(const Matrix* matrix);
Matrix* matrix_transpose(const Matrix* matrix);
Matrix* matrix_adjugate(const Matrix* matrix);
Matrix* matrix_inverse(const Matrix* matrix);
Matrix* matrix_create_submatrix(const Matrix* matrix, size_t excludeRow, size_t excludeCol);
Matrix* matrix_power(const Matrix* matrix, int power);
Matrix* matrix_copy(const Matrix* matrix);
Matrix* matrix_kronecker_product(const Matrix* A, const Matrix* B);
Matrix* matrix_hankel(const Matrix* firstRow, const Matrix* lastCol);

void matrix_deallocate(Matrix* matrix);
void matrix_print(Matrix* matrix);
void matrix_swap_rows(Matrix* mat, size_t row1, size_t row2);
void matrix_swap_cols(Matrix* mat, size_t col1, size_t col2);

bool matrix_scalar_multiply(Matrix* matrix, double scalar);
bool matrix_set(Matrix* matrix, size_t rows, size_t cols, double value);
bool matrix_is_square(const Matrix* matrix);
bool matrix_is_equal(const Matrix* matrix1, const Matrix* matrix2);
bool matrix_is_identity(const Matrix* matrix);
bool matrix_is_idempotent(const Matrix* matrix);
bool matrix_is_row(const Matrix* matrix);
bool matrix_is_columnar(const Matrix* matrix);
bool matrix_is_symmetric(const Matrix* matrix);
bool matrix_is_upper_triangular(const Matrix* matrix);
bool matrix_is_lower_triangular(const Matrix* matrix);
bool matrix_is_skew_symmetric(const Matrix* matrix);
bool matrix_is_diagonal(const Matrix* matrix);
bool matrix_is_orthogonal(const Matrix* matrix);
bool matrix_is_hankel(const Matrix* matrix);

double matrix_get(const Matrix* matrix, size_t row, size_t col);
double matrix_determinant(const Matrix* matrix);
double matrix_trace(const Matrix* matrix);

int matrix_rank(const Matrix* matrix);

#endif 