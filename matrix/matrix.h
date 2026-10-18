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
    #define MATRIX_LOG(...) \
        do { fprintf(stderr, "[MATRIX LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
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
Matrix*  matrix_ones                         (size_t rows, size_t cols);
Matrix*  matrix_zeros                        (size_t rows, size_t cols);
Matrix*  matrix_eye                          (size_t rows, size_t cols, int k);
Matrix*  matrix_diag                         (const Matrix* vector);
bool     matrix_meshgrid                     (const Matrix* x, const Matrix* y, Matrix** X, Matrix** Y);
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
Matrix*  matrix_abs                          (const Matrix* matrix);
Matrix*  matrix_negate                       (const Matrix* matrix);
Matrix*  matrix_scalar_add                   (const Matrix* matrix, double scalar);
Matrix*  matrix_divide                       (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_pow                          (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_mod                          (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_arctan2                      (const Matrix* y, const Matrix* x);
Matrix*  matrix_hypot                        (const Matrix* x, const Matrix* y);
Matrix*  matrix_copysign                     (const Matrix* x, const Matrix* y);
Matrix*  matrix_fmod                         (const Matrix* x, const Matrix* y);
Matrix*  matrix_sqrt                         (const Matrix* matrix);
Matrix*  matrix_sign                         (const Matrix* matrix);
Matrix*  matrix_reciprocal                   (const Matrix* matrix);
Matrix*  matrix_clip                         (const Matrix* matrix, double min_val, double max_val);
Matrix*  matrix_round                        (const Matrix* matrix);
Matrix*  matrix_floor                        (const Matrix* matrix);
Matrix*  matrix_ceil                         (const Matrix* matrix);
Matrix*  matrix_square                       (const Matrix* matrix);
Matrix*  matrix_exp                          (const Matrix* matrix);
Matrix*  matrix_log                          (const Matrix* matrix);
Matrix*  matrix_log10                        (const Matrix* matrix);
Matrix*  matrix_sin                          (const Matrix* matrix);
Matrix*  matrix_cos                          (const Matrix* matrix);
Matrix*  matrix_tan                          (const Matrix* matrix);
Matrix*  matrix_trunc                        (const Matrix* matrix);
Matrix*  matrix_cbrt                         (const Matrix* matrix);
Matrix*  matrix_log2                         (const Matrix* matrix);
Matrix*  matrix_exp2                         (const Matrix* matrix);
Matrix*  matrix_expm1                        (const Matrix* matrix);
Matrix*  matrix_log1p                        (const Matrix* matrix);
Matrix*  matrix_arcsin                       (const Matrix* matrix);
Matrix*  matrix_arccos                       (const Matrix* matrix);
Matrix*  matrix_arctan                       (const Matrix* matrix);
Matrix*  matrix_sinh                         (const Matrix* matrix);
Matrix*  matrix_cosh                         (const Matrix* matrix);
Matrix*  matrix_tanh                         (const Matrix* matrix);
Matrix*  matrix_deg2rad                      (const Matrix* matrix);
Matrix*  matrix_rad2deg                      (const Matrix* matrix);
Matrix*  matrix_signbit                      (const Matrix* matrix);
Matrix*  matrix_isnan                        (const Matrix* matrix);
Matrix*  matrix_isinf                        (const Matrix* matrix);
Matrix*  matrix_isfinite                     (const Matrix* matrix);
Matrix*  matrix_logical_not                  (const Matrix* matrix);
Matrix*  matrix_maximum                      (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_minimum                      (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_greater                      (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_greater_equal                (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_less                         (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_less_equal                   (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_logical_and                  (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_logical_or                   (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_where                        (const Matrix* cond, const Matrix* a, const Matrix* b);
Matrix*  matrix_outer                        (const Matrix* a, const Matrix* b);
Matrix*  matrix_cross                        (const Matrix* a, const Matrix* b);
Matrix*  matrix_convolve                     (const Matrix* a, const Matrix* v);
Matrix*  matrix_sinc                         (const Matrix* matrix);
Matrix*  matrix_around                       (const Matrix* matrix, int decimals);
Matrix*  matrix_exprel                       (const Matrix* matrix);
Matrix*  matrix_gcd                          (const Matrix* a, const Matrix* b);
Matrix*  matrix_lcm                          (const Matrix* a, const Matrix* b);
Matrix*  matrix_logaddexp                    (const Matrix* a, const Matrix* b);
Matrix*  matrix_fmax                         (const Matrix* a, const Matrix* b);
Matrix*  matrix_fmin                         (const Matrix* a, const Matrix* b);
bool     matrix_modf                         (const Matrix* matrix, Matrix** frac_part, Matrix** int_part);
bool     matrix_divmod                       (const Matrix* a, const Matrix* b, Matrix** quotient, Matrix** remainder);
Matrix*  matrix_floor_divide                 (const Matrix* a, const Matrix* b);
Matrix*  matrix_fix                          (const Matrix* matrix);
Matrix*  matrix_rint                         (const Matrix* matrix);
Matrix*  matrix_nan_to_num                   (const Matrix* matrix);
Matrix*  matrix_float_power                  (const Matrix* a, const Matrix* b);
Matrix*  matrix_expit                        (const Matrix* matrix);
Matrix*  matrix_logit                        (const Matrix* matrix);
Matrix*  matrix_xlogy                        (const Matrix* x, const Matrix* y);
Matrix*  matrix_heaviside                    (const Matrix* x, const Matrix* h0);
Matrix*  matrix_angle                        (const Matrix* real, const Matrix* imag);
Matrix*  matrix_not_equal                    (const Matrix* a, const Matrix* b);
Matrix*  matrix_equal_elementwise            (const Matrix* a, const Matrix* b);
Matrix*  matrix_logical_xor                  (const Matrix* a, const Matrix* b);
Matrix*  matrix_isposinf                     (const Matrix* matrix);
Matrix*  matrix_isneginf                     (const Matrix* matrix);
bool     matrix_array_equal                  (const Matrix* a, const Matrix* b);
double   matrix_nansum                       (const Matrix* matrix);
double   matrix_nanmean                      (const Matrix* matrix);
double   matrix_nanmin                       (const Matrix* matrix);
double   matrix_nanmax                       (const Matrix* matrix);
double   matrix_nanprod                      (const Matrix* matrix);
Matrix*  matrix_arange                       (double start, double stop, double step);
Matrix*  matrix_linspace                     (double start, double stop, size_t num);
Matrix*  matrix_ediff1d                      (const Matrix* matrix);
Matrix*  matrix_flatnonzero                  (const Matrix* matrix);
Matrix*  matrix_extract                      (const Matrix* condition, const Matrix* arr);
Matrix*  matrix_trim_zeros                   (const Matrix* matrix);
size_t   matrix_nanargmax                    (const Matrix* matrix);
Matrix*  matrix_nancumsum                    (const Matrix* matrix);
Matrix*  matrix_nansum_axis                  (const Matrix* matrix, int axis);
double   matrix_gmean                        (const Matrix* matrix);
double   matrix_sem                          (const Matrix* matrix);
size_t   matrix_nunique                      (const Matrix* matrix);
Matrix*  matrix_logspace                     (double start, double stop, size_t num, double base);
Matrix*  matrix_geomspace                    (double start, double stop, size_t num);
Matrix*  matrix_resize                       (const Matrix* matrix, size_t rows, size_t cols);
Matrix*  matrix_unravel_index                (const Matrix* indices, size_t rows, size_t cols);
Matrix*  matrix_ravel                        (const Matrix* matrix);
Matrix*  matrix_dot                          (const Matrix* a, const Matrix* b);
Matrix*  matrix_vdot                         (const Matrix* a, const Matrix* b);
double   matrix_trace_offset                 (const Matrix* matrix, int k);
Matrix*  matrix_atleast_2d                   (const Matrix* matrix);
Matrix*  matrix_fftfreq                      (size_t n, double d);
Matrix*  matrix_rfftfreq                     (size_t n, double d);
Matrix*  matrix_hanning                      (size_t M);
Matrix*  matrix_hamming                      (size_t M);


/* ------------------------------------------------------------------ */
/* Structural transforms                                              */
/* ------------------------------------------------------------------ */

Matrix*  matrix_transpose                    (const Matrix* matrix);
Matrix*  matrix_reshape                      (const Matrix* matrix, size_t new_rows, size_t new_cols);
Matrix*  matrix_flipud                       (const Matrix* matrix);
Matrix*  matrix_fliplr                       (const Matrix* matrix);
Matrix*  matrix_rot90                        (const Matrix* matrix, int k);
Matrix*  matrix_roll                         (const Matrix* matrix, int shift);
Matrix*  matrix_roll_axis                    (const Matrix* matrix, int shift, int axis);
Matrix*  matrix_shift                        (const Matrix* matrix, int periods, int axis, double fill_value);
Matrix*  matrix_tril                         (const Matrix* matrix, int k);
Matrix*  matrix_triu                         (const Matrix* matrix, int k);
Matrix*  matrix_hstack                       (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_vstack                       (const Matrix* matrix1, const Matrix* matrix2);
Matrix*  matrix_tile                         (const Matrix* matrix, size_t reps_row, size_t reps_col);
Matrix*  matrix_repeat                       (const Matrix* matrix, size_t repeats);
Matrix*  matrix_repeat_axis                  (const Matrix* matrix, size_t repeats, int axis);
Matrix*  matrix_pad                          (const Matrix* matrix, size_t pad, double value);
Matrix*  matrix_sort                         (const Matrix* matrix, int axis);
Matrix*  matrix_argsort                      (const Matrix* matrix, int axis);
Matrix*  matrix_diff                         (const Matrix* matrix, int axis);
Matrix*  matrix_unique                       (const Matrix* matrix);
Matrix*  matrix_searchsorted                 (const Matrix* a, const Matrix* v);
Matrix** matrix_split                        (const Matrix* matrix, size_t sections, int axis, size_t* out_count);
Matrix*  matrix_nonzero                      (const Matrix* matrix);
Matrix*  matrix_flip                         (const Matrix* matrix, int axis);
Matrix*  matrix_take                         (const Matrix* matrix, const Matrix* indices);
Matrix*  matrix_delete                       (const Matrix* matrix, size_t index, int axis);
Matrix*  matrix_insert                       (const Matrix* matrix, size_t index, const Matrix* values, int axis);
Matrix*  matrix_append                       (const Matrix* matrix, const Matrix* values, int axis);
Matrix*  matrix_column_stack                 (const Matrix* a, const Matrix* b);
Matrix*  matrix_digitize                     (const Matrix* x, const Matrix* bins);
Matrix*  matrix_bincount                     (const Matrix* x);
bool     matrix_histogram                    (const Matrix* x, size_t bins, Matrix** hist, Matrix** edges);
Matrix*  matrix_inverse                      (const Matrix* matrix);
Matrix*  matrix_pinv                         (const Matrix* matrix);
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
Matrix*  matrix_diagonal                     (const Matrix* matrix, int k);
bool     matrix_fill_diagonal                (Matrix* matrix, double value);


/* ------------------------------------------------------------------ */
/* Decompositions                                                     */
/* ------------------------------------------------------------------ */

bool     matrix_lu_decomposition             (const Matrix* matrix, Matrix** L, Matrix** U);
bool     matrix_qr_decomposition             (const Matrix* A, Matrix** Q, Matrix** R);
Matrix*  matrix_cholesky_decomposition       (const Matrix* matrix);
Matrix*  matrix_solve                        (const Matrix* A, const Matrix* b);
bool     matrix_eig                          (const Matrix* matrix, Matrix** eigenvalues, Matrix** eigenvectors);
bool     matrix_svd                          (const Matrix* matrix, Matrix** U, Matrix** S, Matrix** V);
Matrix*  matrix_lstsq                        (const Matrix* A, const Matrix* b);
Matrix*  matrix_expm                         (const Matrix* matrix);
Matrix*  matrix_polyval                      (const Matrix* p, const Matrix* x);
Matrix*  matrix_roots                        (const Matrix* coeffs);


/* ------------------------------------------------------------------ */
/* Scalars / norms                                                    */
/* ------------------------------------------------------------------ */

double   matrix_determinant                  (const Matrix* matrix);
bool     matrix_slogdet                      (const Matrix* matrix, double* sign, double* logabsdet);
double   matrix_trace                        (const Matrix* matrix);
double   matrix_frobenius_norm               (const Matrix* matrix);
double   matrix_l1_norm                      (const Matrix* matrix);
double   matrix_infinity_norm                (const Matrix* matrix);
double   matrix_cond                         (const Matrix* matrix);
double   matrix_nuclear_norm                 (const Matrix* matrix);
double   matrix_spectral_norm                (const Matrix* matrix);
double   matrix_min_element                  (const Matrix* matrix);
double   matrix_max_element                  (const Matrix* matrix);
double   matrix_sum                          (const Matrix* matrix);
double   matrix_prod                         (const Matrix* matrix);
double   matrix_mean                         (const Matrix* matrix);
double   matrix_ptp                          (const Matrix* matrix);
size_t   matrix_argmax                       (const Matrix* matrix);
size_t   matrix_argmin                       (const Matrix* matrix);
Matrix*  matrix_sum_axis                     (const Matrix* matrix, int axis);
Matrix*  matrix_prod_axis                    (const Matrix* matrix, int axis);
Matrix*  matrix_mean_axis                    (const Matrix* matrix, int axis);
Matrix*  matrix_min_axis                     (const Matrix* matrix, int axis);
Matrix*  matrix_max_axis                     (const Matrix* matrix, int axis);
Matrix*  matrix_argmax_axis                  (const Matrix* matrix, int axis);
Matrix*  matrix_argmin_axis                  (const Matrix* matrix, int axis);
Matrix*  matrix_ptp_axis                     (const Matrix* matrix, int axis);
Matrix*  matrix_median_axis                  (const Matrix* matrix, int axis);
Matrix*  matrix_count_nonzero_axis           (const Matrix* matrix, int axis);
size_t   matrix_count_nonzero                (const Matrix* matrix);
bool     matrix_any                          (const Matrix* matrix);
bool     matrix_all                          (const Matrix* matrix);
Matrix*  matrix_cumsum                       (const Matrix* matrix);
Matrix*  matrix_cumsum_axis                  (const Matrix* matrix, int axis);
Matrix*  matrix_cumprod                      (const Matrix* matrix);
Matrix*  matrix_cumprod_axis                 (const Matrix* matrix, int axis);
Matrix*  matrix_cummax                       (const Matrix* matrix);
Matrix*  matrix_cummax_axis                  (const Matrix* matrix, int axis);
Matrix*  matrix_cummin                       (const Matrix* matrix);
Matrix*  matrix_cummin_axis                  (const Matrix* matrix, int axis);
double   matrix_var                          (const Matrix* matrix);
double   matrix_std                          (const Matrix* matrix);
double   matrix_median                       (const Matrix* matrix);
Matrix*  matrix_var_axis                     (const Matrix* matrix, int axis);
Matrix*  matrix_std_axis                     (const Matrix* matrix, int axis);
double   matrix_average                      (const Matrix* matrix, const Matrix* weights);
double   matrix_percentile                   (const Matrix* matrix, double p);
Matrix*  matrix_standardize                  (const Matrix* matrix);
Matrix*  matrix_normalize                    (const Matrix* matrix);
Matrix*  matrix_rolling_mean                 (const Matrix* matrix, size_t window, int axis);
Matrix*  matrix_pct_change                   (const Matrix* matrix, int axis);
Matrix*  matrix_fillna                       (const Matrix* matrix, double value);
Matrix*  matrix_describe                     (const Matrix* matrix);
Matrix*  matrix_cov                          (const Matrix* matrix);
Matrix*  matrix_corrcoef                     (const Matrix* matrix);
int      matrix_rank                         (const Matrix* matrix);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool     matrix_is_equal                     (const Matrix* matrix1, const Matrix* matrix2);
bool     matrix_allclose                     (const Matrix* matrix1, const Matrix* matrix2, double rtol, double atol);
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
