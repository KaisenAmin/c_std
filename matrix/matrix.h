/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Matrix
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in matrix.c (file-level convention).
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* #define MATRIX_LOGGING_ENABLE */

#ifdef MATRIX_LOGGING_ENABLE
    #define MATRIX_LOG(fmt, ...) \
        do { fprintf(stderr, "[MATRIX LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define MATRIX_LOG(...) do { } while (0)
#endif



/* A row-major, owning double matrix. Allocate with `matrix_create`
 * (or any of the matrix-returning factories below), release with
 * `matrix_deallocate`. */
typedef struct Matrix {
    size_t  rows;
    size_t  cols;
    double* data;
} Matrix;

/* Scalar→scalar callback used by `matrix_map` / `matrix_apply_to_*`. */
typedef double (*MatrixFunc)(double);


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Matrix*  matrix_create                       (size_t rows, size_t cols);
Matrix*  matrix_copy                         (const Matrix* matrix);
Matrix*  matrix_from_array                   (const double* data, size_t rows, size_t cols);
Matrix*  matrix_random                       (size_t row, size_t col, size_t start, size_t end);


/* ------------------------------------------------------------------ */
/* Destruction / debug                                                */
/* ------------------------------------------------------------------ */

void     matrix_deallocate                   (Matrix* matrix);
void     matrix_print                        (Matrix* matrix);


/* ------------------------------------------------------------------ */
/* Special / named matrices                                           */
/* ------------------------------------------------------------------ */

Matrix*  matrix_create_identity              (size_t n);
Matrix*  matrix_hilbert                      (size_t n);
Matrix*  matrix_inverse_hilbert              (size_t n);
Matrix*  matrix_pascal                       (size_t n);
Matrix*  matrix_helmert                      (size_t n, bool full);
Matrix*  matrix_walsh                        (size_t n);
Matrix*  matrix_vandermonde                  (const Matrix* matrix, size_t n);
Matrix*  matrix_companion                    (const Matrix* coefficients, size_t n);
Matrix*  matrix_circulant                    (const Matrix* firstRow);
Matrix*  matrix_hankel                       (const Matrix* firstRow, const Matrix* lastCol);
Matrix*  matrix_toeplitz                     (const Matrix* firstRow, const Matrix* firstCol);
Matrix*  matrix_leslie                       (Matrix* f, size_t f_size, Matrix* s, size_t s_size);
Matrix*  matrix_fiedler                      (const Matrix* matrix);
Matrix*  matrix_block_diag                   (size_t count, ...);


/* ------------------------------------------------------------------ */
/* Element access / mutation                                          */
/* ------------------------------------------------------------------ */

double   matrix_get                          (const Matrix* matrix, size_t row, size_t col);
bool     matrix_set                          (Matrix* matrix, size_t rows, size_t cols, double value);
bool     matrix_fill                         (Matrix* matrix, double value);
double*  matrix_to_array                     (const Matrix* matrix);
size_t   matrix_size                         (const Matrix* matrix);


/* ------------------------------------------------------------------ */
/* Row / column operations                                            */
/* ------------------------------------------------------------------ */

Matrix*  matrix_get_row                      (const Matrix* matrix, size_t row);
Matrix*  matrix_get_col                      (const Matrix* matrix, size_t col);
void     matrix_swap_rows                    (Matrix* mat, size_t row1, size_t row2);
void     matrix_swap_cols                    (Matrix* mat, size_t col1, size_t col2);
void     matrix_row_divide                   (Matrix* matrix, size_t row, double scalar);
void     matrix_row_subtract                 (Matrix* matrix, size_t targetRow, size_t subtractRow, double scalar);
bool     matrix_row_addition                 (Matrix* matrix, size_t targetRow, size_t sourceRow, double scale);
bool     matrix_col_addition                 (Matrix* matrix, size_t targetCol, size_t sourceCol, double scale);
bool     matrix_apply_to_row                 (Matrix* matrix, size_t row, MatrixFunc func);
bool     matrix_apply_to_col                 (Matrix* matrix, size_t col, MatrixFunc func);


/* ------------------------------------------------------------------ */
/* Arithmetic                                                         */
/* ------------------------------------------------------------------ */

Matrix*  matrix_add                          (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_subtract                     (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_multiply                     (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_kronecker_product            (const Matrix* A, const Matrix* B);
Matrix*  matrix_hadamard_product             (const Matrix* A, const Matrix* B);
Matrix*  matrix_power                        (const Matrix* matrix, int power);
Matrix*  matrix_map                          (const Matrix* matrix, MatrixFunc func);
bool     matrix_scalar_multiply              (Matrix* matrix, double scalar);


/* ------------------------------------------------------------------ */
/* Structural transforms                                              */
/* ------------------------------------------------------------------ */

Matrix*  matrix_transpose                    (const Matrix* matrix);
Matrix*  matrix_inverse                      (const Matrix* matrix);
Matrix*  matrix_inverse_gauss_jordan         (const Matrix* matrix);
Matrix*  matrix_adjugate                     (const Matrix* matrix);
Matrix*  matrix_cofactor                     (const Matrix* matrix);
Matrix*  matrix_create_submatrix             (const Matrix* matrix, size_t excludeRow, size_t excludeCol);
Matrix*  matrix_projection                   (const Matrix* matrix);


/* ------------------------------------------------------------------ */
/* Diagonals                                                          */
/* ------------------------------------------------------------------ */

Matrix*  matrix_get_main_diagonal_as_column  (const Matrix* matrix);
Matrix*  matrix_get_main_diagonal_as_row     (const Matrix* matrix);
Matrix*  matrix_get_minor_diagonal_as_row    (const Matrix* matrix);
Matrix*  matrix_get_minor_diagonal_as_column (const Matrix* matrix);


/* ------------------------------------------------------------------ */
/* Decompositions                                                     */
/* ------------------------------------------------------------------ */

bool     matrix_lu_decomposition             (const Matrix* matrix, Matrix** L, Matrix** U);
bool     matrix_qr_decomposition             (const Matrix* A, Matrix** Q, Matrix** R);
Matrix*  matrix_cholesky_decomposition       (const Matrix* matrix);
Matrix*  matrix_solve                        (const Matrix* A, const Matrix* b);


/* ------------------------------------------------------------------ */
/* Scalars / norms                                                    */
/* ------------------------------------------------------------------ */

double   matrix_determinant                  (const Matrix* matrix);
double   matrix_trace                        (const Matrix* matrix);
double   matrix_frobenius_norm               (const Matrix* matrix);
double   matrix_l1_norm                      (const Matrix* matrix);
double   matrix_infinity_norm                (const Matrix* matrix);
double   matrix_min_element                  (const Matrix* matrix);
double   matrix_max_element                  (const Matrix* matrix);
int      matrix_rank                         (const Matrix* matrix);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool     matrix_is_equal                     (const Matrix* matrix1, const Matrix* matrix2);
bool     matrix_is_square                    (const Matrix* matrix);
bool     matrix_is_row                       (const Matrix* matrix);
bool     matrix_is_columnar                  (const Matrix* matrix);
bool     matrix_is_identity                  (const Matrix* matrix);
bool     matrix_is_idempotent                (const Matrix* matrix);
bool     matrix_is_symmetric                 (const Matrix* matrix);
bool     matrix_is_skew_symmetric            (const Matrix* matrix);
bool     matrix_is_diagonal                  (const Matrix* matrix);
bool     matrix_is_upper_triangular          (const Matrix* matrix);
bool     matrix_is_lower_triangular          (const Matrix* matrix);
bool     matrix_is_orthogonal                (const Matrix* matrix);
bool     matrix_is_hankel                    (const Matrix* matrix);
bool     matrix_is_toeplitz                  (const Matrix* matrix);
bool     matrix_is_positive_definite         (const Matrix* matrix);
bool     matrix_is_sparse                    (const Matrix* matrix);


#ifdef __cplusplus
}
#endif

#endif 
