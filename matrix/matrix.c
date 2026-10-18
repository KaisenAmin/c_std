/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Matrix
*/

#include <math.h>
#include <string.h>
#include <float.h>
#include <stdint.h>
#include <limits.h>   /* LLONG_MAX */
#include <time.h>
#include "matrix.h"

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>   /* _InterlockedExchangeAdd64 for mat_rng_next */
#endif

#define EPSILON 1e-9

// Function to check if a floating-point number is effectively zero
static bool is_effectively_zero(double value) {
    MATRIX_LOG("[is_effectively_zero] Info: Checking if value %lf is effectively zero.\n", value);
    bool result = fabs(value) < EPSILON;

    if (result) {
        MATRIX_LOG("[is_effectively_zero] Success: Value %lf is considered effectively zero.\n", value);
    } 
    else {
        MATRIX_LOG("[is_effectively_zero] Info: Value %lf is not effectively zero.\n", value);
    }

    return result;
}

// function to generate the Walsh matrix recursively
static void generateWalshMatrixRecursively(double* data, size_t order, size_t dim, size_t startRow, size_t startCol, int val) {
    MATRIX_LOG("[generateWalshMatrixRecursively] Info: Generating Walsh matrix with order %zu at position (%zu, %zu) with initial value %d.\n", order, startRow, startCol, val);

    if (order == 1) {
        data[startRow * dim + startCol] = val;
        MATRIX_LOG("[generateWalshMatrixRecursively] Success: Base case reached, set value %d at (%zu, %zu).\n", val, startRow, startCol);
        return;
    }

    size_t halfOrder = order / 2;
    // Top-left quadrant
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow, startCol, val);
    // Top-right quadrant
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow, startCol + halfOrder, val);
    // Bottom-left quadrant
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow + halfOrder, startCol, val);
    // Bottom-right quadrant (invert the values)
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow + halfOrder, startCol + halfOrder, -val);

    MATRIX_LOG("[generateWalshMatrixRecursively] Success: Walsh matrix generated recursively with order %zu.\n", order);
}

/* Binomial coefficient C(n, k) in double precision, computed multiplicatively:
   each partial product is itself a binomial coefficient, so the result is
   exact while it stays below 2^53. (The old int64 factorials overflowed at
   21!, which broke matrix_inverse_hilbert for n >= 11.) */
static double mat_binomial(size_t n, size_t k) {
    if (k > n) {
        return 0.0;
    }
    if (k > n - k) {
        k = n - k;
    }
    double result = 1.0;
    for (size_t i = 1; i <= k; ++i) {
        result = result * (double)(n - k + i) / (double)i;
    }
    return result;
}


/* Overflow-checked size arithmetic for result dimensions computed from
   caller-supplied counts: a wrapped product or sum made matrix_create
   allocate a small block that the fill loops then wrote far past. */
static bool mat_size_mul(size_t a, size_t b, size_t* out) {
    if (b != 0 && a > SIZE_MAX / b) {
        return false;
    }
    *out = a * b;
    return true;
}

static bool mat_size_add(size_t a, size_t b, size_t* out) {
    if (a > SIZE_MAX - b) {
        return false;
    }
    *out = a + b;
    return true;
}


/* 64-bit generator for matrix_random: a splitmix64 stream over an atomic
   counter, offset by the clock, so concurrent calls never share a value and
   libc's rand()/srand() state is left alone. Not for cryptographic use. */
static uint64_t mat_rng_next(void) {
    static uint64_t counter;
#if defined(_MSC_VER) && !defined(__clang__)
    uint64_t c = (uint64_t)_InterlockedExchangeAdd64((volatile long long*)&counter, (long long)0x9E3779B97F4A7C15ULL);
#else
    uint64_t c = __atomic_fetch_add(&counter, 0x9E3779B97F4A7C15ULL, __ATOMIC_RELAXED);
#endif
    uint64_t z = c + ((uint64_t)time(NULL) * 0xD1B54A32D192ED03ULL) + (uint64_t)(uintptr_t)&counter;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

/**
 * @brief Swaps two rows of a matrix.
 *
 * This function swaps the elements of two rows in the given matrix.
 * If the matrix pointer is NULL or the row indices are out of bounds, the function does nothing.
 *
 * @param mat The matrix whose rows are to be swapped.
 * @param row1 The index of the first row to swap.
 * @param row2 The index of the second row to swap.
 */
void matrix_swap_rows(Matrix* mat, size_t row1, size_t row2) {
    if (!mat || row1 >= mat->rows || row2 >= mat->rows) {
        MATRIX_LOG("[matrix_swap_rows] Error: Invalid row indices or matrix is null.\n");
        return;
    }

    for (size_t i = 0; i < mat->cols; i++) {
        double temp = mat->data[row1 * mat->cols + i];
        mat->data[row1 * mat->cols + i] = mat->data[row2 * mat->cols + i];
        mat->data[row2 * mat->cols + i] = temp;
    }

    MATRIX_LOG("[matrix_swap_rows] Success: Rows %zu and %zu swapped successfully.\n", row1, row2);
}

/**
 * @brief Swaps two columns in a matrix.
 *
 * This function exchanges the positions of two columns in a matrix.
 * If the matrix is NULL or the column indices are out of bounds, the function does nothing.
 *
 * @param mat The matrix in which to swap the columns.
 * @param col1 The index of the first column to swap.
 * @param col2 The index of the second column to swap.
 */
void matrix_swap_cols(Matrix* mat, size_t col1, size_t col2) {
    if (!mat || col1 >= mat->cols || col2 >= mat->cols) {
        MATRIX_LOG("[matrix_swap_cols] Error: Invalid column indices or matrix is null.\n");
        return;
    }

    for (size_t i = 0; i < mat->rows; i++) {
        double temp = mat->data[i * mat->cols + col1];
        mat->data[i * mat->cols + col1] = mat->data[i * mat->cols + col2];
        mat->data[i * mat->cols + col2] = temp;
    }

    MATRIX_LOG("[matrix_swap_cols] Success: Columns %zu and %zu swapped successfully.\n", col1, col2);
}

/**
 * @brief Divides all elements in a row by a scalar.
 *
 * This function divides each element in a specified row of the matrix by a given scalar value.
 *
 * @param matrix The matrix whose row will be divided.
 * @param row The index of the row to divide.
 * @param scalar The scalar value by which to divide each element in the row.
 */
void matrix_row_divide(Matrix* matrix, size_t row, double scalar) {
    if (!matrix || row >= matrix->rows) {
        MATRIX_LOG("[matrix_row_divide] Error: Invalid row index or matrix is null.\n");
        return;
    }
    // Guard against division by zero — would otherwise produce +/-inf or NaN
    // and silently corrupt downstream operations.
    if (scalar == 0.0) {
        MATRIX_LOG("[matrix_row_divide] Error: division by zero requested.\n");
        return;
    }
    for (size_t col = 0; col < matrix->cols; col++) {
        matrix->data[row * matrix->cols + col] /= scalar;
    }

    MATRIX_LOG("[matrix_row_divide] Success: Row %zu divided by %lf successfully.\n", row, scalar);
}

/**
 * @brief Subtracts a scaled row from another row.
 *
 * This function subtracts a scaled version of one row from another row in the matrix.
 *
 * @param matrix The matrix in which the operation will be performed.
 * @param targetRow The index of the row that will be modified.
 * @param subtractRow The index of the row to subtract from the target row.
 * @param scalar The scalar value by which to multiply the subtractRow before subtracting it from the targetRow.
 */
void matrix_row_subtract(Matrix* matrix, size_t targetRow, size_t subtractRow, double scalar) {
    if (!matrix || targetRow >= matrix->rows || subtractRow >= matrix->rows) {
        MATRIX_LOG("[matrix_row_subtract] Error: Invalid row indices or matrix is null.\n");
        return;
    }
    for (size_t col = 0; col < matrix->cols; col++) {
        matrix->data[targetRow * matrix->cols + col] -= scalar * matrix->data[subtractRow * matrix->cols + col];
    }

    MATRIX_LOG("[matrix_row_subtract] Success: Row %zu modified by subtracting scaled Row %zu (scale = %lf).\n", targetRow, subtractRow, scalar);
}

/**
 * @brief Creates a matrix with the specified number of rows and columns, initialized to zero.
 *
 * This function allocates memory for a matrix structure and its data array. 
 * The matrix is initialized with all elements set to zero.
 * If the number of rows or columns is zero, or if memory allocation fails, 
 * the function returns NULL.
 *
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * 
 * @return A pointer to the created matrix, or NULL if an error occurred.
 */
Matrix* matrix_create(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        MATRIX_LOG("[matrix_create] Error: Number of rows or columns is zero.\n");
        return NULL;
    }

    Matrix* matrix = (Matrix*) malloc(sizeof(Matrix));
    if (!matrix) {
        MATRIX_LOG("[matrix_create] Error: Memory allocation failed for matrix object.\n");
        return NULL;
    }

    /* Reject element counts whose byte size would overflow size_t: a wrapped
       size would give an undersized buffer that the zero-fill below overruns. */
    if (cols > SIZE_MAX / rows || rows * cols > SIZE_MAX / sizeof(double)) {
        MATRIX_LOG("[matrix_create] Error: %zu x %zu elements overflow the addressable size.\n", rows, cols);
        free(matrix);
        return NULL;
    }

    size_t totalSize = rows * cols * sizeof(double);
    matrix->data = (double*) malloc(totalSize);
    if (!matrix->data) {
        MATRIX_LOG("[matrix_create] Error: Memory allocation failed for matrix data.\n");
        free(matrix);  // Clean up the allocated matrix
        return NULL;
    }

    matrix->rows = rows;
    matrix->cols = cols;

    for (size_t index = 0; index < rows * cols; index++) {
        matrix->data[index] = 0.0;
    }

    MATRIX_LOG("[matrix_create] Success: Matrix creation and initialization successful.\n");
    return matrix;
}


/**
 * @brief Adds two matrices of the same dimensions.
 *
 * This function performs element-wise addition of two matrices.
 * The matrices must have the same dimensions. If the dimensions do not match, 
 * or if any of the matrices is NULL, the function returns NULL.
 *
 * @param matrix1 The first matrix to add.
 * @param matrix2 The second matrix to add.
 * 
 * @return A new matrix containing the sum of the two matrices, or NULL if an error occurred.
 */
Matrix* matrix_add(const Matrix* matrix1, const Matrix* matrix2) {
    if (!matrix1) {
        MATRIX_LOG("[matrix_add] Error: matrix1 object is null.\n");
        return NULL;
    }
    if (!matrix2) {
        MATRIX_LOG("[matrix_add] Error: matrix2 object is null.\n");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_add] Error: The two matrices are not of the same order.\n");
        return NULL;
    }

    Matrix* addition = matrix_create(matrix1->rows, matrix1->cols);
    if (!addition) {
        MATRIX_LOG("[matrix_add] Error: Failed to create result matrix.\n");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows; i++) {
        for (size_t j = 0; j < matrix1->cols; j++) {
            size_t index = i * matrix1->cols + j;
            addition->data[index] = matrix1->data[index] + matrix2->data[index];
        }
    }

    MATRIX_LOG("[matrix_add] Success: Matrices added successfully.\n");
    return addition;
}

/**
 * @brief Subtracts the second matrix from the first matrix.
 *
 * This function performs element-wise subtraction of two matrices.
 * The matrices must have the same dimensions. If the dimensions do not match, 
 * or if any of the matrices is NULL, the function returns NULL.
 *
 * @param matrix1 The matrix from which to subtract.
 * @param matrix2 The matrix to subtract.
 * 
 * @return A new matrix containing the result of the subtraction, or NULL if an error occurred.
 */
Matrix* matrix_subtract(const Matrix* matrix1, const Matrix* matrix2) {
    if (!matrix1) {
        MATRIX_LOG("[matrix_subtract] Error: matrix1 object is null.\n");
        return NULL;
    }
    if (!matrix2) {
        MATRIX_LOG("[matrix_subtract] Error: matrix2 object is null.\n");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_subtract] Error: The two matrices are not of the same order.\n");
        return NULL;
    }

    Matrix* subtraction = matrix_create(matrix1->rows, matrix1->cols);
    if (!subtraction) {
        MATRIX_LOG("[matrix_subtract] Error: Failed to create result matrix.\n");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows; i++) {
        for (size_t j = 0; j < matrix1->cols; j++) {
            size_t index = i * matrix1->cols + j;
            subtraction->data[index] = matrix1->data[index] - matrix2->data[index];
        }
    }

    MATRIX_LOG("[matrix_subtract] Success: Matrices subtracted successfully.\n");
    return subtraction;
}

/**
 * @brief Multiplies two matrices and returns the result.
 *
 * This function performs matrix multiplication between two matrices, `matrix1` and `matrix2`.
 * The number of columns in `matrix1` must match the number of rows in `matrix2`. 
 * If the matrices are not compatible for multiplication or if any matrix is NULL, 
 * the function returns NULL.
 *
 * @param matrix1 The first matrix (left operand) in the multiplication.
 * @param matrix2 The second matrix (right operand) in the multiplication.
 * 
 * @return A new matrix representing the product of `matrix1` and `matrix2`, or NULL if an error occurred.
 */
Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_multiply] Entering function");

    if (!matrix1) {
        MATRIX_LOG("[matrix_multiply] Error: matrix1 object is null.");
        return NULL;
    }
    if (!matrix2) {
        MATRIX_LOG("[matrix_multiply] Error: matrix2 object is null.");
        return NULL;
    }
    if (matrix1->cols != matrix2->rows) {
        MATRIX_LOG("[matrix_multiply] Error: matrix1's columns (%zu) do not match matrix2's rows (%zu).", matrix1->cols, matrix2->rows);
        return NULL;
    }

    Matrix* product = matrix_create(matrix1->rows, matrix2->cols);
    if (!product) {
        MATRIX_LOG("[matrix_multiply] Error: Memory allocation failed for product matrix.");
        return NULL;
    }


    const size_t m = matrix1->rows;
    const size_t n = matrix2->cols;
    const size_t p = matrix1->cols;            /* == matrix2->rows */
    const double* a = matrix1->data;
    const double* b = matrix2->data;
    double* c = product->data;

    for (size_t i = 0; i < m; i++) {
        const double* arow = a + i * p;
        double* crow = c + i * n;
        for (size_t k = 0; k < p; k++) {
            const double aik = arow[k];
            const double* brow = b + k * n;
            for (size_t j = 0; j < n; j++) {
                crow[j] += aik * brow[j];
            }
        }
    }

    MATRIX_LOG("[matrix_multiply] Success: Matrix multiplication completed.");
    return product;
}

/**
 * @brief Deallocates the memory associated with a matrix.
 *
 * This function frees the memory used by the matrix, including its data array. 
 * If the matrix pointer is NULL, the function does nothing.
 *
 * @param matrix The matrix to be deallocated.
 */
void matrix_deallocate(Matrix* matrix) {
    MATRIX_LOG("[matrix_deallocate] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_deallocate] Info: Matrix object is null, no need for deallocation.");
        return;
    }

    free(matrix->data);
    free(matrix);

    MATRIX_LOG("[matrix_deallocate] Success: Matrix deallocated successfully.");
}

/**
 * @brief Sets the value of a specific element in a matrix.
 *
 * This function sets the value of the matrix element at the specified row and column.
 * If the matrix pointer is NULL, or if the row or column indices are out of bounds, the function returns false.
 *
 * @param matrix The matrix in which to set the value.
 * @param rows The row index where the value will be set.
 * @param cols The column index where the value will be set.
 * @param value The value to set at the specified position.
 * 
 * @return true if the value was set successfully, false otherwise.
 */
bool matrix_set(Matrix* matrix, size_t rows, size_t cols, double value) {
    MATRIX_LOG("[matrix_set] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_set] Error: Matrix object is null.");
        return false;
    }
    if (rows >= matrix->rows || cols >= matrix->cols) {
        MATRIX_LOG("[matrix_set] Error: Row (%zu) or column (%zu) out of bounds.", rows, cols);
        return false;
    }

    size_t index = rows * matrix->cols + cols;
    matrix->data[index] = value;

    MATRIX_LOG("[matrix_set] Success: Set value at (%zu, %zu) = %lf", rows, cols, value);
    return true;
}

/**
 * @brief Prints the matrix to the console in a formatted manner.
 *
 * This function prints the matrix elements to the console, formatting the output so that
 * all elements are aligned in columns. If the matrix pointer is NULL, the function does nothing.
 *
 * @param matrix The matrix to be printed.
 */
void matrix_print(Matrix* matrix) {
    MATRIX_LOG("[matrix_print] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_print] Info: Matrix object is null, nothing to print.");
        return;
    }

    int max_width = 0;
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        int width = snprintf(NULL, 0, "%.5lf", matrix->data[i]);
        if (width > max_width) {
            max_width = width;
        }
    }

    for (size_t row = 0; row < matrix->rows; row++) {
        printf("| ");
        for (size_t col = 0; col < matrix->cols; col++) {
            size_t index = row * matrix->cols + col;
            printf("%*.*lf ", max_width, 5, matrix->data[index]);
        }
        printf("|\n");
    }

    MATRIX_LOG("[matrix_print] Success: Matrix printed successfully.");
}

/**
 * @brief Gets the value of a specific element in a matrix.
 *
 * This function retrieves the value of the matrix element at the specified row and column.
 * If the matrix pointer is NULL, or if the row or column indices are out of bounds, the function exits the program.
 *
 * @param matrix The matrix from which to get the value.
 * @param row The row index of the element to retrieve.
 * @param col The column index of the element to retrieve.
 * 
 * @return The value at the specified position.
 */
double matrix_get(const Matrix* matrix, size_t row, size_t col) {
    MATRIX_LOG("[matrix_get] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_get] Error: Matrix object is null.");
        // Project convention: libraries never abort. Return 0.0 on invalid
        // input — callers that care about correctness should validate first.
        return 0.0;
    }
    if (row >= matrix->rows || col >= matrix->cols) {
        MATRIX_LOG("[matrix_get] Error: Row or column out of bounds. Requested row = %zu, col = %zu.", row, col);
        return 0.0;
    }

    size_t index = row * matrix->cols + col;
    double value = matrix->data[index];

    MATRIX_LOG("[matrix_get] Value at (%zu, %zu) = %lf", row, col, value);
    return value;
}

static bool mat_check_diagonal(const Matrix* mat, size_t i, size_t j) {
    MATRIX_LOG("[mat_check_diagonal] Entering function");

    double res = matrix_get(mat, i, j);
    MATRIX_LOG("[mat_check_diagonal] Starting value on diagonal at (%zu, %zu) = %lf", i, j, res);

    while (++i < mat->rows && ++j < mat->cols) {
        double next_value = matrix_get(mat, i, j);
        if (next_value != res) {
            MATRIX_LOG("[mat_check_diagonal] Mismatch found at (%zu, %zu), value = %lf", i, j, next_value);
            return false;
        }
    }

    MATRIX_LOG("[mat_check_diagonal] Diagonal check successful.");
    return true;
}

/**
 * @brief Multiplies every element of the matrix by a scalar value.
 *
 * This function scales each element in the matrix by the given scalar.
 *
 * @param matrix The matrix to be scaled.
 * @param scalar The scalar value to multiply each element by.
 * 
 * @return true if the operation is successful, false if the matrix is NULL.
 */
bool matrix_scalar_multiply(Matrix* matrix, double scalar) {
    MATRIX_LOG("[matrix_scalar_multiply] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_scalar_multiply] Error: Matrix object is null.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            size_t index = i * matrix->cols + j;
            matrix->data[index] *= scalar;
            MATRIX_LOG("[matrix_scalar_multiply] Scaled value at (%zu, %zu) = %lf", i, j, matrix->data[index]);
        }
    }

    MATRIX_LOG("[matrix_scalar_multiply] Success: Matrix scalar multiplication completed.");
    return true;
}

/**
 * @brief Computes the element-wise absolute value of a matrix.
 *
 * The C analogue of NumPy's `numpy.abs(M)` (a.k.a. `numpy.absolute`): every
 * element of the returned matrix is `|x|`. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix containing the element-wise absolute values, or `NULL`
 *         if `matrix` is NULL or memory allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_abs(const Matrix* matrix) {
    MATRIX_LOG("[matrix_abs] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_abs] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_abs] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = fabs(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_abs] Success: element-wise absolute value computed.");
    return result;
}

/**
 * @brief Computes the element-wise negation of a matrix.
 *
 * The C analogue of NumPy's `numpy.negative(M)`: every element of the returned
 * matrix is `-x`. The input matrix is not modified. This is equivalent to
 * `matrix_scalar_multiply` by `-1.0`, but returns a new matrix rather than
 * mutating in place.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix containing the element-wise negated values, or `NULL`
 *         if `matrix` is NULL or memory allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_negate(const Matrix* matrix) {
    MATRIX_LOG("[matrix_negate] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_negate] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_negate] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = -matrix->data[i];
    }

    MATRIX_LOG("[matrix_negate] Success: element-wise negation computed.");
    return result;
}

/**
 * @brief Adds a scalar to every element of a matrix.
 *
 * The C analogue of NumPy's `numpy.add(M, c)` (i.e. `M + c`): every element of
 * the returned matrix is `x + scalar`. The input matrix is not modified. This
 * is the additive counterpart of `matrix_scalar_multiply`; note that, unlike
 * that in-place function, this one returns a new matrix.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param scalar The value to add to every element.
 *
 * @return A new matrix with `scalar` added to each element, or `NULL` if
 *         `matrix` is NULL or memory allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_scalar_add(const Matrix* matrix, double scalar) {
    MATRIX_LOG("[matrix_scalar_add] Entering function with scalar = %lf", scalar);

    if (!matrix) {
        MATRIX_LOG("[matrix_scalar_add] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_scalar_add] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = matrix->data[i] + scalar;
    }

    MATRIX_LOG("[matrix_scalar_add] Success: scalar added to every element.");
    return result;
}

/**
 * @brief Computes the element-wise (Hadamard) division of two matrices.
 *
 * The C analogue of NumPy's `numpy.divide(A, B)` (i.e. `A / B`): every element
 * of the returned matrix is `a / b`. The two matrices must have identical
 * dimensions (this function does not broadcast). Neither input is modified.
 *
 * @param matrix1 The numerator matrix. Must not be NULL.
 * @param matrix2 The denominator matrix. Must not be NULL and must have the
 *                same dimensions as `matrix1`.
 *
 * @return A new matrix containing the element-wise quotient, or `NULL` if
 *         either matrix is NULL, the dimensions differ, or memory allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 *
 * @note Division by zero is not treated as an error: as in NumPy's default
 *       mode, it follows IEEE-754 and yields `+inf`, `-inf`, or `nan`
 *       (`0.0 / 0.0`). NumPy additionally emits a runtime warning; this
 *       function does not.
 */
Matrix* matrix_divide(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_divide] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_divide] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_divide] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        MATRIX_LOG("[matrix_divide] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = matrix1->data[i] / matrix2->data[i];
    }

    MATRIX_LOG("[matrix_divide] Success: element-wise division completed.");
    return result;
}

/**
 * @brief Computes the element-wise square root of a matrix.
 *
 * The C analogue of NumPy's `numpy.sqrt(M)`: every element of the returned
 * matrix is `sqrt(x)`. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix containing the element-wise square roots, or `NULL` if
 *         `matrix` is NULL or memory allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note For a negative element the result is `nan`, mirroring NumPy's real
 *       `np.sqrt` (which returns `nan` and emits a warning rather than
 *       producing a complex result).
 */
Matrix* matrix_sqrt(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sqrt] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_sqrt] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_sqrt] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = sqrt(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_sqrt] Success: element-wise square root computed.");
    return result;
}

/**
 * @brief Computes the element-wise sign of a matrix.
 *
 * The C analogue of NumPy's `numpy.sign(M)`: each element becomes `-1` if it is
 * negative, `0` if it is zero (including `-0.0`), or `+1` if it is positive. The
 * input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise signs, or `NULL` if `matrix` is NULL or
 *         memory allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 *
 * @note A `nan` element yields `nan`, matching NumPy (`np.sign(nan) == nan`).
 */
Matrix* matrix_sign(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sign] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_sign] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_sign] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        double x = matrix->data[i];
        result->data[i] = isnan(x) ? x : (double)((x > 0.0) - (x < 0.0));
    }

    MATRIX_LOG("[matrix_sign] Success: element-wise sign computed.");
    return result;
}

/**
 * @brief Computes the element-wise reciprocal (`1/x`) of a matrix.
 *
 * The C analogue of NumPy's `numpy.reciprocal(M)`: each element becomes
 * `1.0 / x`. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise reciprocals, or `NULL` if `matrix` is
 *         NULL or memory allocation fails. The caller owns the result and must
 *         release it with `matrix_deallocate`.
 *
 * @note Reciprocal of zero is not an error: as in NumPy's default mode it
 *       follows IEEE-754 and yields `+inf` (`1/0.0`) or `-inf` (`1/-0.0`).
 *       NumPy additionally emits a runtime warning; this function does not.
 */
Matrix* matrix_reciprocal(const Matrix* matrix) {
    MATRIX_LOG("[matrix_reciprocal] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_reciprocal] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_reciprocal] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = 1.0 / matrix->data[i];
    }

    MATRIX_LOG("[matrix_reciprocal] Success: element-wise reciprocal computed.");
    return result;
}

/**
 * @brief Clamps every element of a matrix to the interval `[min_val, max_val]`.
 *
 * The C analogue of NumPy's `numpy.clip(M, min_val, max_val)`: each element `x`
 * becomes `min_val` if `x < min_val`, `max_val` if `x > max_val`, or `x`
 * otherwise. The input matrix is not modified.
 *
 * The computation mirrors NumPy exactly as `minimum(max_val, maximum(x, min_val))`
 * with NaN propagation, so:
 *  - a `nan` element yields `nan`;
 *  - if `min_val > max_val`, every element collapses to `max_val` (the same
 *    quirk NumPy exhibits, e.g. `np.clip(a, 8, 1)` returns all 1s).
 *
 * @param matrix  The source matrix. Must not be NULL.
 * @param min_val The lower bound of the clamp interval.
 * @param max_val The upper bound of the clamp interval.
 *
 * @return A new clamped matrix, or `NULL` if `matrix` is NULL or memory
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_clip(const Matrix* matrix, double min_val, double max_val) {
    MATRIX_LOG("[matrix_clip] Entering function with min = %lf, max = %lf", min_val, max_val);

    if (!matrix) {
        MATRIX_LOG("[matrix_clip] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_clip] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        double x = matrix->data[i];
        /* maximum(x, min_val), NaN-propagating like numpy */
        double m = (isnan(x) || isnan(min_val)) ? NAN : (x > min_val ? x : min_val);
        /* minimum(max_val, m), NaN-propagating like numpy */
        result->data[i] = (isnan(m) || isnan(max_val)) ? NAN : (max_val < m ? max_val : m);
    }

    MATRIX_LOG("[matrix_clip] Success: matrix clamped to [min, max].");
    return result;
}

/**
 * @brief Rounds every element of a matrix to the nearest integer.
 *
 * The C analogue of NumPy's `numpy.round(M)` (with the default `decimals=0`):
 * each element is rounded to the nearest integer value. The input matrix is not
 * modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of rounded values, or `NULL` if `matrix` is NULL or
 *         memory allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 *
 * @note Like NumPy, ties are rounded to the nearest **even** integer
 *       (banker's rounding): `0.5 -> 0`, `1.5 -> 2`, `2.5 -> 2`, `-2.5 -> -2`.
 *       This uses C's `nearbyint` under the default round-to-nearest mode, and
 *       therefore differs from C's `round`, which rounds halves away from zero.
 */
Matrix* matrix_round(const Matrix* matrix) {
    MATRIX_LOG("[matrix_round] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_round] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_round] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = nearbyint(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_round] Success: element-wise round-half-to-even computed.");
    return result;
}

/**
 * @brief Computes the element-wise floor of a matrix.
 *
 * The C analogue of NumPy's `numpy.floor(M)`: each element becomes the largest
 * integer value not greater than it (e.g. `2.7 -> 2`, `-2.3 -> -3`). The input
 * matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of floored values, or `NULL` if `matrix` is NULL or
 *         memory allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 */
Matrix* matrix_floor(const Matrix* matrix) {
    MATRIX_LOG("[matrix_floor] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_floor] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_floor] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = floor(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_floor] Success: element-wise floor computed.");
    return result;
}

/**
 * @brief Computes the element-wise ceiling of a matrix.
 *
 * The C analogue of NumPy's `numpy.ceil(M)`: each element becomes the smallest
 * integer value not less than it (e.g. `2.3 -> 3`, `-2.7 -> -2`). The input
 * matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of ceiled values, or `NULL` if `matrix` is NULL or
 *         memory allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 */
Matrix* matrix_ceil(const Matrix* matrix) {
    MATRIX_LOG("[matrix_ceil] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_ceil] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_ceil] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = ceil(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_ceil] Success: element-wise ceiling computed.");
    return result;
}

/**
 * @brief Computes the element-wise square of a matrix.
 *
 * The C analogue of NumPy's `numpy.square(M)`: every element becomes `x * x`.
 * The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise squares, or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_square(const Matrix* matrix) {
    MATRIX_LOG("[matrix_square] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_square] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_square] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = matrix->data[i] * matrix->data[i];
    }

    MATRIX_LOG("[matrix_square] Success: element-wise square computed.");
    return result;
}

/**
 * @brief Computes the element-wise exponential of a matrix.
 *
 * The C analogue of NumPy's `numpy.exp(M)`: every element becomes `e^x`. The
 * input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise exponentials, or `NULL` if `matrix` is
 *         NULL or allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 */
Matrix* matrix_exp(const Matrix* matrix) {
    MATRIX_LOG("[matrix_exp] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_exp] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_exp] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = exp(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_exp] Success: element-wise exponential computed.");
    return result;
}

/**
 * @brief Computes the element-wise natural logarithm of a matrix.
 *
 * The C analogue of NumPy's `numpy.log(M)`: every element becomes `ln(x)`. The
 * input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise natural logs, or `NULL` if `matrix` is
 *         NULL or allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 *
 * @note Like NumPy, `log(0)` is `-inf` and `log(x)` for `x < 0` is `nan`.
 */
Matrix* matrix_log(const Matrix* matrix) {
    MATRIX_LOG("[matrix_log] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_log] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_log] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = log(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_log] Success: element-wise natural log computed.");
    return result;
}

/**
 * @brief Computes the element-wise base-10 logarithm of a matrix.
 *
 * The C analogue of NumPy's `numpy.log10(M)`: every element becomes `log10(x)`.
 * The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise base-10 logs, or `NULL` if `matrix` is
 *         NULL or allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 *
 * @note Like NumPy, `log10(0)` is `-inf` and `log10(x)` for `x < 0` is `nan`.
 */
Matrix* matrix_log10(const Matrix* matrix) {
    MATRIX_LOG("[matrix_log10] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_log10] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_log10] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = log10(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_log10] Success: element-wise base-10 log computed.");
    return result;
}

/**
 * @brief Computes the element-wise sine of a matrix.
 *
 * The C analogue of NumPy's `numpy.sin(M)`: every element becomes `sin(x)` (with
 * `x` in radians). The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise sines, or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_sin(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sin] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_sin] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_sin] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = sin(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_sin] Success: element-wise sine computed.");
    return result;
}

/**
 * @brief Computes the element-wise cosine of a matrix.
 *
 * The C analogue of NumPy's `numpy.cos(M)`: every element becomes `cos(x)` (with
 * `x` in radians). The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise cosines, or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_cos(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cos] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_cos] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_cos] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = cos(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_cos] Success: element-wise cosine computed.");
    return result;
}

/**
 * @brief Computes the element-wise tangent of a matrix.
 *
 * The C analogue of NumPy's `numpy.tan(M)`: every element becomes `tan(x)` (with
 * `x` in radians). The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise tangents, or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_tan(const Matrix* matrix) {
    MATRIX_LOG("[matrix_tan] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_tan] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_tan] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = tan(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_tan] Success: element-wise tangent computed.");
    return result;
}

/**
 * @brief Truncates every element of a matrix toward zero.
 *
 * The C analogue of NumPy's `numpy.trunc(M)`: every element is rounded toward
 * zero (e.g. `2.7 -> 2`, `-2.7 -> -2`). Unlike `matrix_floor`/`matrix_ceil`,
 * which round toward `-inf`/`+inf`, this discards the fractional part. The input
 * matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of truncated values, or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_trunc(const Matrix* matrix) {
    MATRIX_LOG("[matrix_trunc] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_trunc] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_trunc] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = trunc(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_trunc] Success: element-wise truncation computed.");
    return result;
}

/**
 * @brief Computes the element-wise cube root of a matrix.
 *
 * The C analogue of NumPy's `numpy.cbrt(M)`: every element becomes its real cube
 * root. Unlike `matrix_sqrt`, negative inputs are valid (e.g. `cbrt(-27) = -3`).
 * The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise cube roots, or `NULL` if `matrix` is NULL
 *         or allocation fails. The caller owns the result and must release it
 *         with `matrix_deallocate`.
 */
Matrix* matrix_cbrt(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cbrt] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_cbrt] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_cbrt] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = cbrt(matrix->data[i]);
    }

    MATRIX_LOG("[matrix_cbrt] Success: element-wise cube root computed.");
    return result;
}

/**
 * @brief Computes the element-wise base-2 logarithm of a matrix.
 *
 * The C analogue of NumPy's `numpy.log2(M)`: every element becomes `log2(x)`.
 * The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise base-2 logs, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 *
 * @note Like NumPy, `log2(0)` is `-inf` and `log2(x)` for `x < 0` is `nan`.
 */
Matrix* matrix_log2(const Matrix* matrix) {
    MATRIX_LOG("[matrix_log2] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_log2] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = log2(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_log2] Success: element-wise base-2 log computed.");
    return result;
}

/**
 * @brief Computes the element-wise base-2 exponential of a matrix.
 *
 * The C analogue of NumPy's `numpy.exp2(M)`: every element becomes `2^x`. The
 * input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise `2^x`, or `NULL` if `matrix` is NULL or
 *         allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_exp2(const Matrix* matrix) {
    MATRIX_LOG("[matrix_exp2] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_exp2] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = exp2(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_exp2] Success: element-wise base-2 exponential computed.");
    return result;
}

/**
 * @brief Computes the element-wise `e^x - 1` of a matrix.
 *
 * The C analogue of NumPy's `numpy.expm1(M)`: every element becomes `e^x - 1`,
 * computed accurately even for small `x`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise `e^x - 1`, or `NULL` if `matrix` is NULL
 *         or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_expm1(const Matrix* matrix) {
    MATRIX_LOG("[matrix_expm1] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_expm1] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = expm1(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_expm1] Success: element-wise e^x - 1 computed.");
    return result;
}

/**
 * @brief Computes the element-wise `ln(1 + x)` of a matrix.
 *
 * The C analogue of NumPy's `numpy.log1p(M)`: every element becomes `ln(1 + x)`,
 * computed accurately even for small `x`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise `ln(1 + x)`, or `NULL` if `matrix` is NULL
 *         or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_log1p(const Matrix* matrix) {
    MATRIX_LOG("[matrix_log1p] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_log1p] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = log1p(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_log1p] Success: element-wise ln(1 + x) computed.");
    return result;
}

/**
 * @brief Computes the element-wise inverse sine of a matrix.
 *
 * The C analogue of NumPy's `numpy.arcsin(M)`: every element becomes `asin(x)`
 * (in radians). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise arcsines, or `NULL` if `matrix` is NULL or
 *         allocation fails. Free it with `matrix_deallocate`.
 *
 * @note An element outside `[-1, 1]` yields `nan`, matching NumPy.
 */
Matrix* matrix_arcsin(const Matrix* matrix) {
    MATRIX_LOG("[matrix_arcsin] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_arcsin] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = asin(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_arcsin] Success: element-wise inverse sine computed.");
    return result;
}

/**
 * @brief Computes the element-wise inverse cosine of a matrix.
 *
 * The C analogue of NumPy's `numpy.arccos(M)`: every element becomes `acos(x)`
 * (in radians). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise arccosines, or `NULL` if `matrix` is NULL
 *         or allocation fails. Free it with `matrix_deallocate`.
 *
 * @note An element outside `[-1, 1]` yields `nan`, matching NumPy.
 */
Matrix* matrix_arccos(const Matrix* matrix) {
    MATRIX_LOG("[matrix_arccos] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_arccos] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = acos(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_arccos] Success: element-wise inverse cosine computed.");
    return result;
}

/**
 * @brief Computes the element-wise inverse tangent of a matrix.
 *
 * The C analogue of NumPy's `numpy.arctan(M)`: every element becomes `atan(x)`
 * (in radians). For the quadrant-aware two-argument form use `matrix_arctan2`.
 * The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise arctangents, or `NULL` if `matrix` is NULL
 *         or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_arctan(const Matrix* matrix) {
    MATRIX_LOG("[matrix_arctan] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_arctan] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = atan(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_arctan] Success: element-wise inverse tangent computed.");
    return result;
}

/**
 * @brief Computes the element-wise hyperbolic sine of a matrix.
 *
 * The C analogue of NumPy's `numpy.sinh(M)`: every element becomes `sinh(x)`.
 * The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise hyperbolic sines, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_sinh(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sinh] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_sinh] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = sinh(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_sinh] Success: element-wise hyperbolic sine computed.");
    return result;
}

/**
 * @brief Computes the element-wise hyperbolic cosine of a matrix.
 *
 * The C analogue of NumPy's `numpy.cosh(M)`: every element becomes `cosh(x)`.
 * The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise hyperbolic cosines, or `NULL` if `matrix`
 *         is NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_cosh(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cosh] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_cosh] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = cosh(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_cosh] Success: element-wise hyperbolic cosine computed.");
    return result;
}

/**
 * @brief Computes the element-wise hyperbolic tangent of a matrix.
 *
 * The C analogue of NumPy's `numpy.tanh(M)`: every element becomes `tanh(x)`.
 * This is the classic neural-network activation, mapping values into `(-1, 1)`.
 * The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix of element-wise hyperbolic tangents, or `NULL` if `matrix`
 *         is NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_tanh(const Matrix* matrix) {
    MATRIX_LOG("[matrix_tanh] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_tanh] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = tanh(matrix->data[i]);
    }
    MATRIX_LOG("[matrix_tanh] Success: element-wise hyperbolic tangent computed.");
    return result;
}

/**
 * @brief Converts a matrix of degrees to radians, element-wise.
 *
 * The C analogue of NumPy's `numpy.deg2rad(M)`: every element is multiplied by
 * `pi / 180`. The input is not modified.
 *
 * @param matrix The source matrix (in degrees). Must not be NULL.
 *
 * @return A new matrix in radians, or `NULL` if `matrix` is NULL or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_deg2rad(const Matrix* matrix) {
    MATRIX_LOG("[matrix_deg2rad] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_deg2rad] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = matrix->data[i] * (3.14159265358979323846 / 180.0);
    }
    MATRIX_LOG("[matrix_deg2rad] Success: degrees converted to radians.");
    return result;
}

/**
 * @brief Converts a matrix of radians to degrees, element-wise.
 *
 * The C analogue of NumPy's `numpy.rad2deg(M)`: every element is multiplied by
 * `180 / pi`. The input is not modified.
 *
 * @param matrix The source matrix (in radians). Must not be NULL.
 *
 * @return A new matrix in degrees, or `NULL` if `matrix` is NULL or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_rad2deg(const Matrix* matrix) {
    MATRIX_LOG("[matrix_rad2deg] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_rad2deg] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = matrix->data[i] * (180.0 / 3.14159265358979323846);
    }
    MATRIX_LOG("[matrix_rad2deg] Success: radians converted to degrees.");
    return result;
}

/**
 * @brief Builds a 1/0 mask of the negatively-signed positions in a matrix.
 *
 * The C analogue of NumPy's `numpy.signbit(M)`: each element becomes `1.0` where
 * the corresponding input has its sign bit set (i.e. negative values, **including
 * `-0.0`**), otherwise `0.0`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_signbit(const Matrix* matrix) {
    MATRIX_LOG("[matrix_signbit] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_signbit] Error: Matrix object is null.");
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = signbit(matrix->data[i]) ? 1.0 : 0.0;
    }
    MATRIX_LOG("[matrix_signbit] Success: sign-bit mask computed.");
    return result;
}

/**
 * @brief Element-wise quadrant-aware arctangent of two matrices.
 *
 * The C analogue of NumPy's `numpy.arctan2(y, x)`: each element is
 * `atan2(y, x)`, the angle (in radians, in `(-pi, pi]`) of the point `(x, y)`,
 * using the signs of both arguments to pick the correct quadrant. The two
 * matrices must have identical dimensions; neither input is modified.
 *
 * @param y The numerator (sine-component) matrix. Must not be NULL.
 * @param x The denominator (cosine-component) matrix, same dimensions as `y`.
 *          Must not be NULL.
 *
 * @return A new matrix of element-wise angles, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_arctan2(const Matrix* y, const Matrix* x) {
    MATRIX_LOG("[matrix_arctan2] Entering function");
    if (!y || !x || !y->data || !x->data) {
        MATRIX_LOG("[matrix_arctan2] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((y->rows != x->rows) || (y->cols != x->cols)) {
        MATRIX_LOG("[matrix_arctan2] Error: The two matrices are not of the same order.");
        return NULL;
    }
    Matrix* result = matrix_create(y->rows, y->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < y->rows * y->cols; ++i) {
        result->data[i] = atan2(y->data[i], x->data[i]);
    }
    MATRIX_LOG("[matrix_arctan2] Success: element-wise arctan2 computed.");
    return result;
}

/**
 * @brief Element-wise Euclidean hypotenuse of two matrices.
 *
 * The C analogue of NumPy's `numpy.hypot(x, y)`: each element is
 * `sqrt(x*x + y*y)`, computed without undue overflow. The two matrices must have
 * identical dimensions; neither input is modified.
 *
 * @param x The first matrix. Must not be NULL.
 * @param y The second matrix, same dimensions as `x`. Must not be NULL.
 *
 * @return A new matrix of element-wise hypotenuses, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_hypot(const Matrix* x, const Matrix* y) {
    MATRIX_LOG("[matrix_hypot] Entering function");
    if (!x || !y || !x->data || !y->data) {
        MATRIX_LOG("[matrix_hypot] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((x->rows != y->rows) || (x->cols != y->cols)) {
        MATRIX_LOG("[matrix_hypot] Error: The two matrices are not of the same order.");
        return NULL;
    }
    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < x->rows * x->cols; ++i) {
        result->data[i] = hypot(x->data[i], y->data[i]);
    }
    MATRIX_LOG("[matrix_hypot] Success: element-wise hypotenuse computed.");
    return result;
}

/**
 * @brief Element-wise copy of magnitude from `x` and sign from `y`.
 *
 * The C analogue of NumPy's `numpy.copysign(x, y)`: each element takes the
 * magnitude of the corresponding element of `x` and the sign of the
 * corresponding element of `y`. The two matrices must have identical dimensions;
 * neither input is modified.
 *
 * @param x The magnitude source matrix. Must not be NULL.
 * @param y The sign source matrix, same dimensions as `x`. Must not be NULL.
 *
 * @return A new matrix of sign-adjusted values, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_copysign(const Matrix* x, const Matrix* y) {
    MATRIX_LOG("[matrix_copysign] Entering function");
    if (!x || !y || !x->data || !y->data) {
        MATRIX_LOG("[matrix_copysign] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((x->rows != y->rows) || (x->cols != y->cols)) {
        MATRIX_LOG("[matrix_copysign] Error: The two matrices are not of the same order.");
        return NULL;
    }
    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < x->rows * x->cols; ++i) {
        result->data[i] = copysign(x->data[i], y->data[i]);
    }
    MATRIX_LOG("[matrix_copysign] Success: element-wise copysign computed.");
    return result;
}

/**
 * @brief Element-wise C-style floating-point remainder of two matrices.
 *
 * The C analogue of NumPy's `numpy.fmod(x, y)`: each element is the remainder
 * `x - n*y` where `n = trunc(x/y)`, so the result has the sign of the **dividend**
 * `x`. This differs from `matrix_mod` (NumPy's `mod`/`remainder`), whose result
 * takes the sign of the **divisor**. The two matrices must have identical
 * dimensions; neither input is modified.
 *
 * @param x The dividend matrix. Must not be NULL.
 * @param y The divisor matrix, same dimensions as `x`. Must not be NULL.
 *
 * @return A new matrix of element-wise remainders, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 *
 * @note For example `fmod(-3, 5) = -3`, whereas `matrix_mod(-3, 5) = 2`.
 */
Matrix* matrix_fmod(const Matrix* x, const Matrix* y) {
    MATRIX_LOG("[matrix_fmod] Entering function");
    if (!x || !y || !x->data || !y->data) {
        MATRIX_LOG("[matrix_fmod] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((x->rows != y->rows) || (x->cols != y->cols)) {
        MATRIX_LOG("[matrix_fmod] Error: The two matrices are not of the same order.");
        return NULL;
    }
    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < x->rows * x->cols; ++i) {
        result->data[i] = fmod(x->data[i], y->data[i]);
    }
    MATRIX_LOG("[matrix_fmod] Success: element-wise fmod computed.");
    return result;
}

/**
 * @brief Element-wise power of two matrices.
 *
 * The C analogue of NumPy's `numpy.power(A, B)` (i.e. `A ** B`): every element of
 * the result is `a ^ b`. The two matrices must have identical dimensions (no
 * broadcasting); neither input is modified.
 *
 * @param matrix1 The base matrix. Must not be NULL.
 * @param matrix2 The exponent matrix, same dimensions as `matrix1`. Must not be
 *                NULL.
 *
 * @return A new matrix of element-wise powers, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note This is distinct from `matrix_power`, which raises a square matrix to an
 *       integer power. Like C's `pow`, `pow(x, y)` for negative `x` and a
 *       non-integer `y` yields `nan`.
 */
Matrix* matrix_pow(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_pow] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_pow] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_pow] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        MATRIX_LOG("[matrix_pow] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = pow(matrix1->data[i], matrix2->data[i]);
    }

    MATRIX_LOG("[matrix_pow] Success: element-wise power computed.");
    return result;
}

/**
 * @brief Element-wise modulo of two matrices.
 *
 * The C analogue of NumPy's `numpy.mod(A, B)` (equivalently `numpy.remainder`):
 * every element of the result is `a mod b`. The two matrices must have identical
 * dimensions (no broadcasting); neither input is modified.
 *
 * Unlike C's `fmod` (whose result takes the sign of the dividend), this follows
 * NumPy/Python semantics where the result takes the sign of the **divisor**,
 * computed as `a - b * floor(a / b)`. For example `mod(-3, 5) = 2`, whereas
 * `fmod(-3, 5) = -3`.
 *
 * @param matrix1 The dividend matrix. Must not be NULL.
 * @param matrix2 The divisor matrix, same dimensions as `matrix1`. Must not be
 *                NULL.
 *
 * @return A new matrix of element-wise remainders, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note Modulo by zero is not an error: as in NumPy it yields `nan`.
 */
Matrix* matrix_mod(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_mod] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_mod] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_mod] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        MATRIX_LOG("[matrix_mod] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        double a = matrix1->data[i];
        double b = matrix2->data[i];
        if (isinf(b) && isfinite(a)) {
            /* a - b*floor(a/b) is inf*0 = NaN here; NumPy gives a when it
               has the divisor's sign (or is 0), else the divisor */
            result->data[i] = (a == 0.0 || (a > 0.0) == (b > 0.0)) ? a : b;
        }
        else {
            result->data[i] = a - b * floor(a / b);   /* sign of divisor, like NumPy */
        }
    }

    MATRIX_LOG("[matrix_mod] Success: element-wise modulo computed.");
    return result;
}

/**
 * @brief Builds a 1/0 mask of the NaN positions in a matrix.
 *
 * The C analogue of NumPy's `numpy.isnan(M)`: each element becomes `1.0` where
 * the corresponding input is `NaN`, otherwise `0.0`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_isnan(const Matrix* matrix) {
    MATRIX_LOG("[matrix_isnan] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_isnan] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = isnan(matrix->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_isnan] Success: NaN mask computed.");
    return result;
}

/**
 * @brief Builds a 1/0 mask of the infinite positions in a matrix.
 *
 * The C analogue of NumPy's `numpy.isinf(M)`: each element becomes `1.0` where
 * the corresponding input is `+inf` or `-inf`, otherwise `0.0`. The input is not
 * modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_isinf(const Matrix* matrix) {
    MATRIX_LOG("[matrix_isinf] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_isinf] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = isinf(matrix->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_isinf] Success: infinity mask computed.");
    return result;
}

/**
 * @brief Builds a 1/0 mask of the finite positions in a matrix.
 *
 * The C analogue of NumPy's `numpy.isfinite(M)`: each element becomes `1.0`
 * where the corresponding input is finite (neither `NaN` nor `±inf`), otherwise
 * `0.0`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_isfinite(const Matrix* matrix) {
    MATRIX_LOG("[matrix_isfinite] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_isfinite] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = isfinite(matrix->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_isfinite] Success: finite mask computed.");
    return result;
}

/**
 * @brief Element-wise logical NOT of a matrix.
 *
 * The C analogue of NumPy's `numpy.logical_not(M)`: each element becomes `1.0`
 * where the input is zero (false), otherwise `0.0`. Any non-zero value
 * (including `NaN` and the infinities) is treated as true. The input is not
 * modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_logical_not(const Matrix* matrix) {
    MATRIX_LOG("[matrix_logical_not] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_logical_not] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = (matrix->data[i] == 0.0) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_logical_not] Success: logical NOT computed.");
    return result;
}

/**
 * @brief Element-wise "greater than" comparison of two matrices.
 *
 * The C analogue of NumPy's `numpy.greater(A, B)` (`A > B`): each element becomes
 * `1.0` where `a > b`, otherwise `0.0`. The two matrices must have identical
 * dimensions; neither input is modified.
 *
 * @param matrix1 The left matrix. Must not be NULL.
 * @param matrix2 The right matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if either matrix is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 *
 * @note A comparison involving `NaN` is false (yields `0.0`), matching NumPy.
 */
Matrix* matrix_greater(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_greater] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_greater] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_greater] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = (matrix1->data[i] > matrix2->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_greater] Success: element-wise > computed.");
    return result;
}

/**
 * @brief Element-wise "greater than or equal" comparison of two matrices.
 *
 * The C analogue of NumPy's `numpy.greater_equal(A, B)` (`A >= B`): each element
 * becomes `1.0` where `a >= b`, otherwise `0.0`. The two matrices must have
 * identical dimensions; neither input is modified.
 *
 * @param matrix1 The left matrix. Must not be NULL.
 * @param matrix2 The right matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if either matrix is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_greater_equal(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_greater_equal] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_greater_equal] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_greater_equal] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = (matrix1->data[i] >= matrix2->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_greater_equal] Success: element-wise >= computed.");
    return result;
}

/**
 * @brief Element-wise "less than" comparison of two matrices.
 *
 * The C analogue of NumPy's `numpy.less(A, B)` (`A < B`): each element becomes
 * `1.0` where `a < b`, otherwise `0.0`. The two matrices must have identical
 * dimensions; neither input is modified.
 *
 * @param matrix1 The left matrix. Must not be NULL.
 * @param matrix2 The right matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if either matrix is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_less(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_less] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_less] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_less] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = (matrix1->data[i] < matrix2->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_less] Success: element-wise < computed.");
    return result;
}

/**
 * @brief Element-wise "less than or equal" comparison of two matrices.
 *
 * The C analogue of NumPy's `numpy.less_equal(A, B)` (`A <= B`): each element
 * becomes `1.0` where `a <= b`, otherwise `0.0`. The two matrices must have
 * identical dimensions; neither input is modified.
 *
 * @param matrix1 The left matrix. Must not be NULL.
 * @param matrix2 The right matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if either matrix is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_less_equal(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_less_equal] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_less_equal] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_less_equal] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = (matrix1->data[i] <= matrix2->data[i]) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_less_equal] Success: element-wise <= computed.");
    return result;
}

/**
 * @brief Element-wise logical AND of two matrices.
 *
 * The C analogue of NumPy's `numpy.logical_and(A, B)`: each element becomes `1.0`
 * where both inputs are non-zero (true), otherwise `0.0`. Any non-zero value
 * (including `NaN`) is treated as true. The two matrices must have identical
 * dimensions; neither input is modified.
 *
 * @param matrix1 The first matrix. Must not be NULL.
 * @param matrix2 The second matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if either matrix is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_logical_and(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_logical_and] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_logical_and] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_logical_and] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = (matrix1->data[i] != 0.0 && matrix2->data[i] != 0.0) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_logical_and] Success: logical AND computed.");
    return result;
}

/**
 * @brief Element-wise logical OR of two matrices.
 *
 * The C analogue of NumPy's `numpy.logical_or(A, B)`: each element becomes `1.0`
 * where either input is non-zero (true), otherwise `0.0`. Any non-zero value
 * (including `NaN`) is treated as true. The two matrices must have identical
 * dimensions; neither input is modified.
 *
 * @param matrix1 The first matrix. Must not be NULL.
 * @param matrix2 The second matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new 1/0 mask matrix, or `NULL` if either matrix is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_logical_or(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_logical_or] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_logical_or] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_logical_or] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        result->data[i] = (matrix1->data[i] != 0.0 || matrix2->data[i] != 0.0) ? 1.0 : 0.0;
    }

    MATRIX_LOG("[matrix_logical_or] Success: logical OR computed.");
    return result;
}

/**
 * @brief Element-wise conditional select between two matrices.
 *
 * The C analogue of NumPy's `numpy.where(cond, a, b)`: each element of the result
 * is taken from `a` where the corresponding element of `cond` is non-zero (true),
 * and from `b` otherwise. All three matrices must have identical dimensions; none
 * is modified.
 *
 * @param cond The condition matrix (non-zero = true). Must not be NULL.
 * @param a    The values chosen where `cond` is true. Must not be NULL.
 * @param b    The values chosen where `cond` is false. Must not be NULL.
 *
 * @return A new matrix of selected values, or `NULL` if any input is NULL, the
 *         dimensions differ, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 *
 * @note As in NumPy, any non-zero condition (including `NaN`) selects from `a`.
 */
Matrix* matrix_where(const Matrix* cond, const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_where] Entering function");

    if (!cond || !a || !b) {
        MATRIX_LOG("[matrix_where] Error: One or more matrix objects are null.");
        return NULL;
    }
    if ((cond->rows != a->rows) || (cond->cols != a->cols) ||
        (a->rows != b->rows) || (a->cols != b->cols)) {
        MATRIX_LOG("[matrix_where] Error: The three matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(cond->rows, cond->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < cond->rows * cond->cols; ++i) {
        result->data[i] = (cond->data[i] != 0.0) ? a->data[i] : b->data[i];
    }

    MATRIX_LOG("[matrix_where] Success: conditional select computed.");
    return result;
}

/**
 * @brief Element-wise maximum of two matrices.
 *
 * The C analogue of NumPy's `numpy.maximum(A, B)`: each element of the result
 * is the larger of the corresponding elements of `matrix1` and `matrix2`. The
 * two matrices must have identical dimensions (no broadcasting); neither input
 * is modified.
 *
 * @param matrix1 First matrix. Must not be NULL.
 * @param matrix2 Second matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new matrix of element-wise maxima, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note Like NumPy's `maximum` (and unlike C's `fmax`), `nan` propagates: if
 *       either operand is `nan`, the result is `nan`. This is distinct from the
 *       reduction `matrix_max_element`, which collapses the whole matrix.
 */
Matrix* matrix_maximum(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_maximum] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_maximum] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_maximum] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        MATRIX_LOG("[matrix_maximum] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        double a = matrix1->data[i];
        double b = matrix2->data[i];
        result->data[i] = (isnan(a) || isnan(b)) ? NAN : (a > b ? a : b);
    }

    MATRIX_LOG("[matrix_maximum] Success: element-wise maximum computed.");
    return result;
}

/**
 * @brief Element-wise minimum of two matrices.
 *
 * The C analogue of NumPy's `numpy.minimum(A, B)`: each element of the result
 * is the smaller of the corresponding elements of `matrix1` and `matrix2`. The
 * two matrices must have identical dimensions (no broadcasting); neither input
 * is modified.
 *
 * @param matrix1 First matrix. Must not be NULL.
 * @param matrix2 Second matrix, same dimensions as `matrix1`. Must not be NULL.
 *
 * @return A new matrix of element-wise minima, or `NULL` if either matrix is
 *         NULL, the dimensions differ, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note Like NumPy's `minimum` (and unlike C's `fmin`), `nan` propagates: if
 *       either operand is `nan`, the result is `nan`. This is distinct from the
 *       reduction `matrix_min_element`, which collapses the whole matrix.
 */
Matrix* matrix_minimum(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_minimum] Entering function");

    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_minimum] Error: One or both matrix objects are null.");
        return NULL;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_minimum] Error: The two matrices are not of the same order.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix1->rows, matrix1->cols);
    if (!result) {
        MATRIX_LOG("[matrix_minimum] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows * matrix1->cols; ++i) {
        double a = matrix1->data[i];
        double b = matrix2->data[i];
        result->data[i] = (isnan(a) || isnan(b)) ? NAN : (a < b ? a : b);
    }

    MATRIX_LOG("[matrix_minimum] Success: element-wise minimum computed.");
    return result;
}

/**
 * @brief Checks if a matrix is square.
 *
 * A square matrix has the same number of rows and columns.
 *
 * @param matrix The matrix to be checked.
 * @return true if the matrix is square, false otherwise. Returns false if the matrix is NULL.
 */
bool matrix_is_square(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_square] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_square] Error: Matrix object is null.");
        return false;
    }

    bool isSquare = matrix->rows == matrix->cols;
    MATRIX_LOG("[matrix_is_square] Matrix is %ssquare.", isSquare ? "" : "not ");

    return isSquare;
}

/**
 * @brief Creates an identity matrix of size n x n.
 *
 * An identity matrix is a square matrix with 1's on the main diagonal and 0's elsewhere.
 *
 * @param n The size of the identity matrix to be created.
 * @return A pointer to the newly created identity matrix, or NULL if memory allocation fails.
 */
Matrix* matrix_create_identity(size_t n) {
    MATRIX_LOG("[matrix_create_identity] Entering function");

    Matrix* matrix = matrix_create(n, n);
    if (!matrix) {
        MATRIX_LOG("[matrix_create_identity] Error: Memory allocation failed for identity matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            double value = (i == j) ? 1.0 : 0.0;
            matrix_set(matrix, i, j, value);
            MATRIX_LOG("[matrix_create_identity] Set value at (%zu, %zu) = %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_create_identity] Success: Identity matrix created.");
    return matrix;
}

/**
 * @brief Creates a new `rows` x `cols` matrix filled with ones.
 *
 * The C analogue of NumPy's `numpy.ones(shape)`: every element of the
 * returned matrix is `1.0`. Because this library stores a single 2-D matrix
 * of `double`, NumPy's `dtype` / `order` / `device` / `like` parameters do
 * not apply — the element type is always `double` and storage is always
 * row-major. A 1-D NumPy shape such as `np.ones(5)` maps to a row vector
 * `matrix_ones(1, 5)` (or a column vector `matrix_ones(5, 1)`).
 *
 * @param rows The number of rows. Must be greater than 0.
 * @param cols The number of columns. Must be greater than 0.
 *
 * @return A pointer to the newly created matrix of ones, or `NULL` if either
 *         dimension is 0 or memory allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_ones(size_t rows, size_t cols) {
    MATRIX_LOG("[matrix_ones] Entering function with rows = %zu, cols = %zu", rows, cols);

    Matrix* matrix = matrix_create(rows, cols);
    if (!matrix) {
        MATRIX_LOG("[matrix_ones] Error: Matrix creation failed (zero dimension or out of memory).");
        return NULL;
    }

    matrix_fill(matrix, 1.0);

    MATRIX_LOG("[matrix_ones] Success: ones matrix created.");
    return matrix;
}

/**
 * @brief Creates a new `rows` x `cols` matrix filled with zeros.
 *
 * The C analogue of NumPy's `numpy.zeros(shape)`: every element of the
 * returned matrix is `0.0`. Because this library stores a single 2-D matrix
 * of `double`, NumPy's `dtype` / `order` / `device` / `like` parameters do
 * not apply — the element type is always `double` and storage is always
 * row-major. A 1-D NumPy shape such as `np.zeros(5)` maps to a row vector
 * `matrix_zeros(1, 5)` (or a column vector `matrix_zeros(5, 1)`).
 *
 * @param rows The number of rows. Must be greater than 0.
 * @param cols The number of columns. Must be greater than 0.
 *
 * @return A pointer to the newly created matrix of zeros, or `NULL` if either
 *         dimension is 0 or memory allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note `matrix_create` already zero-initializes its storage; the explicit
 *       `matrix_fill(matrix, 0.0)` keeps the zeros guarantee self-contained
 *       and mirrors `matrix_ones`, so the contract holds regardless of how
 *       `matrix_create` initializes memory.
 */
Matrix* matrix_zeros(size_t rows, size_t cols) {
    MATRIX_LOG("[matrix_zeros] Entering function with rows = %zu, cols = %zu", rows, cols);

    Matrix* matrix = matrix_create(rows, cols);
    if (!matrix) {
        MATRIX_LOG("[matrix_zeros] Error: Matrix creation failed (zero dimension or out of memory).");
        return NULL;
    }

    matrix_fill(matrix, 0.0);

    MATRIX_LOG("[matrix_zeros] Success: zeros matrix created.");
    return matrix;
}

/**
 * @brief Creates a 2-D matrix with ones on a diagonal and zeros elsewhere.
 *
 * The C analogue of NumPy's `numpy.eye(N, M, k)`: returns a `rows x cols`
 * matrix whose elements are `1.0` on the `k`-th diagonal and `0.0` everywhere
 * else. `k = 0` is the main diagonal, `k > 0` selects a super-diagonal (above
 * the main), and `k < 0` selects a sub-diagonal (below). Unlike
 * `matrix_create_identity`, the matrix may be rectangular and the diagonal may
 * be offset.
 *
 * @param rows The number of rows. Must be greater than 0.
 * @param cols The number of columns. Must be greater than 0.
 * @param k    The diagonal offset (0 = main, >0 = above, <0 = below).
 *
 * @return A new `rows x cols` matrix, or `NULL` if either dimension is 0 or
 *         memory allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`. An out-of-range `k` simply yields an
 *         all-zero matrix, matching NumPy.
 */
Matrix* matrix_eye(size_t rows, size_t cols, int k) {
    MATRIX_LOG("[matrix_eye] Entering function with rows = %zu, cols = %zu, k = %d", rows, cols, k);

    Matrix* matrix = matrix_create(rows, cols);
    if (!matrix) {
        MATRIX_LOG("[matrix_eye] Error: Matrix creation failed (zero dimension or out of memory).");
        return NULL;
    }

    /* matrix_create zero-fills; place 1.0 where (col - row) == k. */
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if ((ptrdiff_t)j - (ptrdiff_t)i == (ptrdiff_t)k) {
                matrix->data[i * cols + j] = 1.0;
            }
        }
    }

    MATRIX_LOG("[matrix_eye] Success: eye matrix created.");
    return matrix;
}

/**
 * @brief Builds a square diagonal matrix from a vector.
 *
 * The C analogue of the constructor form of NumPy's `numpy.diag(v)`: given a
 * 1-D vector (a `1 x n` row or `n x 1` column) of length `n`, returns an
 * `n x n` matrix with the vector's values on the main diagonal and `0.0`
 * everywhere else. The input is not modified.
 *
 * @param vector The source vector (`1 x n` or `n x 1`). Must not be NULL.
 *
 * @return A new `n x n` diagonal matrix, or `NULL` if `vector` is NULL, is not a
 *         row/column vector, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 *
 * @note This implements only the constructor direction (vector -> diagonal
 *       matrix). To extract a diagonal use `matrix_get_main_diagonal_as_column`.
 */
Matrix* matrix_diag(const Matrix* vector) {
    MATRIX_LOG("[matrix_diag] Entering function");

    if (!vector || !vector->data) {
        MATRIX_LOG("[matrix_diag] Error: Invalid vector provided.");
        return NULL;
    }
    if (vector->rows != 1 && vector->cols != 1) {
        MATRIX_LOG("[matrix_diag] Error: input must be a row or column vector.");
        return NULL;
    }

    size_t n = vector->rows * vector->cols;
    Matrix* result = matrix_create(n, n);   /* zero-filled */
    if (!result) {
        MATRIX_LOG("[matrix_diag] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        result->data[i * n + i] = vector->data[i];
    }

    MATRIX_LOG("[matrix_diag] Success: diagonal matrix created.");
    return result;
}

/**
 * @brief Checks if two matrices are equal.
 *
 * Two matrices are considered equal if they have the same dimensions and their corresponding elements are identical.
 *
 * @param matrix1 The first matrix to be compared.
 * @param matrix2 The second matrix to be compared.
 * 
 * @return true if the matrices are equal, false otherwise. 
 * Returns false if either matrix is NULL or if their dimensions differ.
 */
bool matrix_is_equal(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_is_equal] Entering function");

    if (!matrix1) {
        MATRIX_LOG("[matrix_is_equal] Error: Matrix1 object is null.");
        return false;
    }
    if (!matrix2) {
        MATRIX_LOG("[matrix_is_equal] Error: Matrix2 object is null.");
        return false;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_is_equal] Error: Matrices are not of the same dimensions.");
        return false;
    }

    for (size_t index = 0; index < (matrix1->rows * matrix1->cols); index++) {
        if (matrix1->data[index] != matrix2->data[index]) {
            MATRIX_LOG("[matrix_is_equal] Matrices differ at index %zu.", index);
            return false;
        }
    }

    MATRIX_LOG("[matrix_is_equal] Matrices are equal.");
    return true;
}

/**
 * @brief Checks if a matrix is an identity matrix.
 *
 * An identity matrix is a square matrix with 1's on the main diagonal and 0's elsewhere.
 *
 * @param matrix The matrix to be checked.
 * @return true if the matrix is an identity matrix, false otherwise. Returns false if the matrix is NULL or not square.
 */
bool matrix_is_identity(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_identity] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_identity] Error: Matrix object is null.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_identity] Error: Matrix is not square.");
        return false;
    }

    // Use a tolerance check rather than strict equality — products like
    // A * A^-1 land very close to identity but rarely exactly there in
    // floating-point arithmetic.
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            size_t index = i * matrix->cols + j;
            double v = matrix->data[index];
            if (i == j) {
                if (!is_effectively_zero(v - 1.0)) {
                    MATRIX_LOG("[matrix_is_identity] Element at (%zu, %zu) is not 1 (got %g).", i, j, v);
                    return false;
                }
            }
            else if (!is_effectively_zero(v)) {
                MATRIX_LOG("[matrix_is_identity] Element at (%zu, %zu) is not 0 (got %g).", i, j, v);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_identity] Matrix is an identity matrix.");
    return true;
}

/**
 * @brief Checks if a matrix is idempotent (i.e., A^2 = A).
 *
 * An idempotent matrix is a matrix that, when multiplied by itself, yields the same matrix.
 * A*A is compared with A within a tolerance (1e-9 absolute and relative), not bitwise.
 *
 * @param matrix The matrix to be checked.
 * @return true if the matrix is idempotent, false otherwise. Returns false if the matrix is NULL, not square, or if the matrix multiplication fails.
 */
bool matrix_is_idempotent(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_idempotent] Entering function");

    if (!matrix || !matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_idempotent] Error: Matrix is null or not square.");
        return false;
    }

    Matrix* square = matrix_multiply(matrix, matrix);
    if (!square) {
        MATRIX_LOG("[matrix_is_idempotent] Error: Matrix multiplication failed.");
        return false;
    }

    /* A*A computed in floating point is rarely bit-identical to A (a
       projection matrix came out "not idempotent"): compare with the same
       absolute / relative tolerance as matrix_allclose's defaults. */
    bool isIdempotent = matrix_allclose(square, matrix, 1e-9, 1e-9);
    matrix_deallocate(square);

    if (isIdempotent) {
        MATRIX_LOG("[matrix_is_idempotent] Matrix is idempotent.");
    } 
    else {
        MATRIX_LOG("[matrix_is_idempotent] Matrix is not idempotent.");
    }

    return isIdempotent;
}

/**
 * @brief Checks if a matrix is a row vector.
 *
 * A row vector is a matrix with only one row and one or more columns.
 *
 * @param matrix The matrix to be checked.
 * @return true if the matrix is a row vector, false otherwise. Returns false if the matrix is NULL.
 */
bool matrix_is_row(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_row] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_is_row] Error: Matrix object is null.");
        return false;
    }

    bool isRow = matrix->rows == 1;
    if (isRow) {
        MATRIX_LOG("[matrix_is_row] Matrix is a row vector.");
    } 
    else {
        MATRIX_LOG("[matrix_is_row] Matrix is not a row vector.");
    }

    return isRow;
}

/**
 * @brief Checks if a matrix is a column vector.
 *
 * A column vector is a matrix with only one column and one or more rows.
 *
 * @param matrix The matrix to be checked.
 * @return true if the matrix is a column vector, false otherwise. Returns false if the matrix is NULL.
 */
bool matrix_is_columnar(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_columnar] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_columnar] Error: Matrix object is null.");
        return false;
    }

    bool isColumnar = matrix->cols == 1;
    if (isColumnar) {
        MATRIX_LOG("[matrix_is_columnar] Matrix is a column vector.");
    } 
    else {
        MATRIX_LOG("[matrix_is_columnar] Matrix is not a column vector.");
    }

    return isColumnar;
}


/**
 * @brief Extracts the main diagonal of a square matrix and returns it as a column matrix.
 *
 * This function creates a new Nx1 matrix (where N is the number of rows in the input matrix),
 * and fills it with the elements from the main diagonal of the input square matrix.
 *
 * @param matrix The square matrix from which the main diagonal will be extracted.
 * 
 * @return A new Nx1 matrix containing the main diagonal elements as a column.
 * Returns NULL if the input matrix is NULL, not square, or if memory allocation fails.
 */
Matrix* matrix_get_main_diagonal_as_column(const Matrix* matrix) {
    MATRIX_LOG("[matrix_get_main_diagonal_as_column] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_get_main_diagonal_as_column] Error: Matrix object is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_get_main_diagonal_as_column] Error: Matrix is not square.");
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(matrix->rows, 1); 
    if (!diagonalMatrix) {
        MATRIX_LOG("[matrix_get_main_diagonal_as_column] Error: Memory allocation failed for diagonal matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        double value = matrix->data[i * matrix->cols + i];
        matrix_set(diagonalMatrix, i, 0, value);
        MATRIX_LOG("[matrix_get_main_diagonal_as_column] Set diagonalMatrix(%zu, 0) = %lf", i, value);
    }

    MATRIX_LOG("[matrix_get_main_diagonal_as_column] Success: Main diagonal extracted as column.");
    return diagonalMatrix;
}

/**
 * @brief Extracts the main diagonal of a square matrix and returns it as a row matrix.
 *
 * This function creates a new 1xN matrix (where N is the number of columns in the input matrix),
 * and fills it with the elements from the main diagonal of the input square matrix.
 *
 * @param matrix The square matrix from which the main diagonal will be extracted.
 * 
 * @return A new 1xN matrix containing the main diagonal elements as a row. 
 * Returns NULL if the input matrix is NULL, not square, or if memory allocation fails.
 */
Matrix* matrix_get_main_diagonal_as_row(const Matrix* matrix) {
    MATRIX_LOG("[matrix_get_main_diagonal_as_row] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_get_main_diagonal_as_row] Error: Matrix object is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_get_main_diagonal_as_row] Error: Matrix is not square.");
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(1, matrix->cols); 
    if (!diagonalMatrix) {
        MATRIX_LOG("[matrix_get_main_diagonal_as_row] Error: Memory allocation failed for diagonal matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->cols; i++) {
        double value = matrix->data[i * matrix->cols + i];
        matrix_set(diagonalMatrix, 0, i, value);
        MATRIX_LOG("[matrix_get_main_diagonal_as_row] Set diagonalMatrix(0, %zu) = %lf", i, value);
    }

    MATRIX_LOG("[matrix_get_main_diagonal_as_row] Success: Main diagonal extracted as row.");
    return diagonalMatrix;
}

/**
 * @brief Extracts the minor diagonal of a square matrix and returns it as a row matrix.
 *
 * This function creates a new 1xN matrix (where N is the number of columns in the input matrix),
 * and fills it with the elements from the minor diagonal (anti-diagonal) of the input square matrix.
 *
 * @param matrix The square matrix from which the minor diagonal will be extracted.
 * 
 * @return A new 1xN matrix containing the minor diagonal elements as a row.
 * Returns NULL if the input matrix is NULL, not square, or if memory allocation fails.
 */
Matrix* matrix_get_minor_diagonal_as_row(const Matrix* matrix) {
    MATRIX_LOG("[matrix_get_minor_diagonal_as_row] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_get_minor_diagonal_as_row] Error: Matrix object is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_get_minor_diagonal_as_row] Error: Matrix is not square.");
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(1, matrix->cols); 
    if (!diagonalMatrix) {
        MATRIX_LOG("[matrix_get_minor_diagonal_as_row] Error: Memory allocation failed for diagonal matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->cols; i++) {
        double value = matrix->data[i * matrix->cols + (matrix->cols - 1 - i)];
        matrix_set(diagonalMatrix, 0, i, value);
        MATRIX_LOG("[matrix_get_minor_diagonal_as_row] Set diagonalMatrix(0, %zu) = %lf", i, value);
    }

    MATRIX_LOG("[matrix_get_minor_diagonal_as_row] Success: Minor diagonal extracted as row.");
    return diagonalMatrix;
}

/**
 * @brief Extracts the minor diagonal of a square matrix and returns it as a column matrix.
 *
 * This function creates a new Nx1 matrix (where N is the number of rows in the input matrix),
 * and fills it with the elements from the minor diagonal (anti-diagonal) of the input square matrix.
 *
 * @param matrix The square matrix from which the minor diagonal will be extracted.
 * 
 * @return A new Nx1 matrix containing the minor diagonal elements as a column.
 * Returns NULL if the input matrix is NULL, not square, or if memory allocation fails.
 */
Matrix* matrix_get_minor_diagonal_as_column(const Matrix* matrix) {
    MATRIX_LOG("[matrix_get_minor_diagonal_as_column] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_get_minor_diagonal_as_column] Error: Matrix object is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_get_minor_diagonal_as_column] Error: Matrix is not square.");
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(matrix->rows, 1); 
    if (!diagonalMatrix) {
        MATRIX_LOG("[matrix_get_minor_diagonal_as_column] Error: Memory allocation failed for diagonal matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        double value = matrix->data[i * matrix->cols + (matrix->cols - 1 - i)];
        matrix_set(diagonalMatrix, i, 0, value);
        MATRIX_LOG("[matrix_get_minor_diagonal_as_column] Set diagonalMatrix(%zu, 0) = %lf", i, value);
    }

    MATRIX_LOG("[matrix_get_minor_diagonal_as_column] Success: Minor diagonal extracted as column.");
    return diagonalMatrix;
}

/**
 * @brief Transposes a given matrix.
 *
 * This function creates a new matrix with rows and columns swapped from the input matrix.
 * The elements are rearranged such that the element at position (i, j) in the original matrix 
 * is moved to position (j, i) in the transposed matrix.
 *
 * @param matrix The matrix to be transposed.
 * @return A new transposed matrix. Returns NULL if the input matrix is NULL or if memory allocation fails.
 */
Matrix* matrix_transpose(const Matrix* matrix) {
    MATRIX_LOG("[matrix_transpose] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_transpose] Error: Matrix object is null.");
        return NULL;
    }

    Matrix* transposed = matrix_create(matrix->cols, matrix->rows);
    if (!transposed) {
        MATRIX_LOG("[matrix_transpose] Error: Memory allocation failed for transposed matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            double value = matrix->data[i * matrix->cols + j];
            matrix_set(transposed, j, i, value);
            MATRIX_LOG("[matrix_transpose] Set transposed(%zu, %zu) = %lf", j, i, value);
        }
    }

    MATRIX_LOG("[matrix_transpose] Success: Transpose completed.");
    return transposed;
}

/**
 * @brief Reinterprets a matrix with a new shape, preserving the element order.
 *
 * The C analogue of NumPy's `numpy.reshape(M, (new_rows, new_cols))` in the
 * default row-major (C) order: the elements are read in row-major order and
 * laid back out into a `new_rows x new_cols` matrix. The total number of
 * elements must be unchanged. The input matrix is not modified.
 *
 * @param matrix   The source matrix. Must not be NULL.
 * @param new_rows The number of rows in the result. Must be greater than 0.
 * @param new_cols The number of columns in the result. Must be greater than 0.
 *
 * @return A new `new_rows x new_cols` matrix containing the same elements in the
 *         same row-major order, or `NULL` if `matrix` is NULL, either new
 *         dimension is 0, the element count differs
 *         (`new_rows * new_cols != rows * cols`), or allocation fails. The
 *         caller owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_reshape(const Matrix* matrix, size_t new_rows, size_t new_cols) {
    MATRIX_LOG("[matrix_reshape] Entering function with new shape = %zu x %zu", new_rows, new_cols);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_reshape] Error: Invalid matrix provided.");
        return NULL;
    }
    if (new_rows == 0 || new_cols == 0) {
        MATRIX_LOG("[matrix_reshape] Error: New dimensions must be non-zero.");
        return NULL;
    }
    /* The product must not wrap: 2^63+1 x 2 "equals" 1 x 2 modulo 2^64. */
    if (new_cols > SIZE_MAX / new_rows || new_rows * new_cols != matrix->rows * matrix->cols) {
        MATRIX_LOG("[matrix_reshape] Error: Element count mismatch (%zu vs %zu).",
                   new_rows * new_cols, matrix->rows * matrix->cols);
        return NULL;
    }

    Matrix* result = matrix_create(new_rows, new_cols);
    if (!result) {
        MATRIX_LOG("[matrix_reshape] Error: Failed to create result matrix.");
        return NULL;
    }

    /* Storage is row-major and contiguous, so a flat copy reshapes in C order. */
    memcpy(result->data, matrix->data, matrix->rows * matrix->cols * sizeof(double));

    MATRIX_LOG("[matrix_reshape] Success: matrix reshaped.");
    return result;
}

/**
 * @brief Reverses the row order of a matrix (flip up-down).
 *
 * The C analogue of NumPy's `numpy.flipud(M)`: the rows are reversed so the
 * first row becomes the last and vice versa, while the order within each row is
 * unchanged. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix with reversed row order, or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_flipud(const Matrix* matrix) {
    MATRIX_LOG("[matrix_flipud] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_flipud] Error: Invalid matrix provided.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_flipud] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; ++i) {
        size_t src = matrix->rows - 1 - i;
        for (size_t j = 0; j < matrix->cols; ++j) {
            result->data[i * matrix->cols + j] = matrix->data[src * matrix->cols + j];
        }
    }

    MATRIX_LOG("[matrix_flipud] Success: row order reversed.");
    return result;
}

/**
 * @brief Reverses the column order of a matrix (flip left-right).
 *
 * The C analogue of NumPy's `numpy.fliplr(M)`: the columns are reversed so the
 * first column becomes the last and vice versa, while the order of the rows is
 * unchanged. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix with reversed column order, or `NULL` if `matrix` is NULL
 *         or allocation fails. The caller owns the result and must release it
 *         with `matrix_deallocate`.
 */
Matrix* matrix_fliplr(const Matrix* matrix) {
    MATRIX_LOG("[matrix_fliplr] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_fliplr] Error: Invalid matrix provided.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_fliplr] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            result->data[i * matrix->cols + j] =
                matrix->data[i * matrix->cols + (matrix->cols - 1 - j)];
        }
    }

    MATRIX_LOG("[matrix_fliplr] Success: column order reversed.");
    return result;
}

/**
 * @brief Rotates a matrix by 90 degrees, `k` times, counterclockwise.
 *
 * The C analogue of NumPy's `numpy.rot90(M, k)`: rotates the matrix in the
 * plane by `90 * k` degrees counterclockwise. `k` is taken modulo 4, so any
 * integer (including negative values, which rotate clockwise) is valid. For odd
 * `k` the result has swapped dimensions (`rows x cols` becomes `cols x rows`);
 * for even `k` the shape is preserved. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param k      Number of 90-degree counterclockwise rotations (any int).
 *
 * @return A new rotated matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_rot90(const Matrix* matrix, int k) {
    MATRIX_LOG("[matrix_rot90] Entering function with k = %d", k);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_rot90] Error: Invalid matrix provided.");
        return NULL;
    }

    int kk = ((k % 4) + 4) % 4;   /* normalise to 0..3 */
    size_t m = matrix->rows;
    size_t n = matrix->cols;
    Matrix* result = NULL;

    if (kk == 0) {
        MATRIX_LOG("[matrix_rot90] k mod 4 == 0; returning a copy.");
        return matrix_copy(matrix);
    }
    else if (kk == 1) {
        /* 90 CCW: result is n x m; result[i][j] = M[j][n-1-i] */
        result = matrix_create(n, m);
        if (!result) {
            MATRIX_LOG("[matrix_rot90] Error: Failed to create result matrix.");
            return NULL;
        }
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                result->data[i * m + j] = matrix->data[j * n + (n - 1 - i)];
            }
        }
    }
    else if (kk == 2) {
        /* 180: result is m x n; result[i][j] = M[m-1-i][n-1-j] */
        result = matrix_create(m, n);
        if (!result) {
            MATRIX_LOG("[matrix_rot90] Error: Failed to create result matrix.");
            return NULL;
        }
        for (size_t i = 0; i < m; ++i) {
            for (size_t j = 0; j < n; ++j) {
                result->data[i * n + j] = matrix->data[(m - 1 - i) * n + (n - 1 - j)];
            }
        }
    }
    else {  /* kk == 3 */
        /* 270 CCW (== 90 CW): result is n x m; result[i][j] = M[m-1-j][i] */
        result = matrix_create(n, m);
        if (!result) {
            MATRIX_LOG("[matrix_rot90] Error: Failed to create result matrix.");
            return NULL;
        }
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                result->data[i * m + j] = matrix->data[(m - 1 - j) * n + i];
            }
        }
    }

    MATRIX_LOG("[matrix_rot90] Success: matrix rotated.");
    return result;
}

/**
 * @brief Cyclically shifts all elements of a matrix (flattened).
 *
 * The C analogue of NumPy's `numpy.roll(M, shift)` with the default
 * `axis=None`: the matrix is flattened in row-major order, rotated cyclically by
 * `shift` positions, and reshaped back to the original dimensions. A positive
 * `shift` moves elements toward higher (later) flat indices; a negative `shift`
 * moves them toward lower indices. `shift` is taken modulo the element count, so
 * any integer is valid. The input matrix is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param shift  The number of positions to roll (any `int`).
 *
 * @return A new matrix (same shape) with the elements cyclically shifted, or
 *         `NULL` if `matrix` is NULL/invalid or allocation fails. The caller
 *         owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_roll(const Matrix* matrix, int shift) {
    MATRIX_LOG("[matrix_roll] Entering function with shift = %d", shift);

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_roll] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_roll] Error: Failed to create result matrix.");
        return NULL;
    }

    long long nn = (long long)n;
    size_t s = (size_t)(((long long)shift % nn + nn) % nn);   /* normalised to [0, n) */
    for (size_t i = 0; i < n; ++i) {
        size_t src = (i + n - s) % n;   /* result[i] = M[(i - shift) mod n] */
        result->data[i] = matrix->data[src];
    }

    MATRIX_LOG("[matrix_roll] Success: elements rolled.");
    return result;
}

/**
 * @brief Cyclically shifts a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.roll(M, shift, axis=...)`. With `axis == 0`
 * the rows are rolled (each row moves down by `shift`, wrapping around); with
 * `axis == 1` the columns are rolled (each column moves right by `shift`). A
 * negative `shift` rolls the other way, and `shift` is taken modulo the axis
 * length. The result has the same shape as the input, which is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param shift  The number of positions to roll along the axis (any `int`).
 * @param axis   `0` to roll rows, `1` to roll columns.
 *
 * @return A new matrix (same shape) rolled along the chosen axis, or `NULL` if
 *         `matrix` is NULL, `axis` is neither 0 nor 1, or allocation fails. The
 *         caller owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_roll_axis(const Matrix* matrix, int shift, int axis) {
    MATRIX_LOG("[matrix_roll_axis] Entering function with shift = %d, axis = %d", shift, axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_roll_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_roll_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t rows = matrix->rows;
    size_t cols = matrix->cols;
    Matrix* result = matrix_create(rows, cols);
    if (!result) {
        MATRIX_LOG("[matrix_roll_axis] Error: Failed to create result matrix.");
        return NULL;
    }

    if (axis == 0) {
        long long rr = (long long)rows;
        size_t s = (size_t)(((long long)shift % rr + rr) % rr);
        for (size_t i = 0; i < rows; ++i) {
            size_t src = (i + rows - s) % rows;
            for (size_t j = 0; j < cols; ++j) {
                result->data[i * cols + j] = matrix->data[src * cols + j];
            }
        }
    }
    else {
        long long cc = (long long)cols;
        size_t s = (size_t)(((long long)shift % cc + cc) % cc);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                size_t src = (j + cols - s) % cols;
                result->data[i * cols + j] = matrix->data[i * cols + src];
            }
        }
    }

    MATRIX_LOG("[matrix_roll_axis] Success: axis rolled.");
    return result;
}

/**
 * @brief Returns the lower-triangular part of a matrix.
 *
 * The C analogue of NumPy's `numpy.tril(M, k)`: copies the elements on and below
 * the `k`-th diagonal and sets everything above it to `0.0`. `k = 0` is the main
 * diagonal, `k > 0` keeps additional super-diagonals, and `k < 0` drops
 * sub-diagonals. Works for rectangular matrices. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param k      The diagonal offset above which elements are zeroed.
 *
 * @return A new matrix holding the lower-triangular part, or `NULL` if `matrix`
 *         is NULL or allocation fails. The caller owns the result and must
 *         release it with `matrix_deallocate`.
 */
Matrix* matrix_tril(const Matrix* matrix, int k) {
    MATRIX_LOG("[matrix_tril] Entering function with k = %d", k);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_tril] Error: Invalid matrix provided.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);   /* zero-filled */
    if (!result) {
        MATRIX_LOG("[matrix_tril] Error: Failed to create result matrix.");
        return NULL;
    }

    /* Keep element (i, j) when j <= i + k (on or below the k-th diagonal). */
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            if ((ptrdiff_t)j <= (ptrdiff_t)i + (ptrdiff_t)k) {
                result->data[i * matrix->cols + j] = matrix->data[i * matrix->cols + j];
            }
        }
    }

    MATRIX_LOG("[matrix_tril] Success: lower-triangular part extracted.");
    return result;
}

/**
 * @brief Returns the upper-triangular part of a matrix.
 *
 * The C analogue of NumPy's `numpy.triu(M, k)`: copies the elements on and above
 * the `k`-th diagonal and sets everything below it to `0.0`. `k = 0` is the main
 * diagonal, `k > 0` drops super-diagonals, and `k < 0` keeps additional
 * sub-diagonals. Works for rectangular matrices. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param k      The diagonal offset below which elements are zeroed.
 *
 * @return A new matrix holding the upper-triangular part, or `NULL` if `matrix`
 *         is NULL or allocation fails. The caller owns the result and must
 *         release it with `matrix_deallocate`.
 */
Matrix* matrix_triu(const Matrix* matrix, int k) {
    MATRIX_LOG("[matrix_triu] Entering function with k = %d", k);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_triu] Error: Invalid matrix provided.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);   /* zero-filled */
    if (!result) {
        MATRIX_LOG("[matrix_triu] Error: Failed to create result matrix.");
        return NULL;
    }

    /* Keep element (i, j) when j >= i + k (on or above the k-th diagonal). */
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            if ((ptrdiff_t)j >= (ptrdiff_t)i + (ptrdiff_t)k) {
                result->data[i * matrix->cols + j] = matrix->data[i * matrix->cols + j];
            }
        }
    }

    MATRIX_LOG("[matrix_triu] Success: upper-triangular part extracted.");
    return result;
}

/**
 * @brief Concatenates two matrices horizontally (side by side).
 *
 * The C analogue of NumPy's `numpy.hstack((A, B))` for 2-D arrays: places `B` to
 * the right of `A`. The two matrices must have the same number of rows; the
 * result has that many rows and `cols(A) + cols(B)` columns. Neither input is
 * modified.
 *
 * @param matrix1 The left matrix. Must not be NULL.
 * @param matrix2 The right matrix, with the same number of rows. Must not be NULL.
 *
 * @return A new `rows x (cols1 + cols2)` matrix, or `NULL` if either matrix is
 *         NULL, the row counts differ, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_hstack(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_hstack] Entering function");

    if (!matrix1 || !matrix2 || !matrix1->data || !matrix2->data) {
        MATRIX_LOG("[matrix_hstack] Error: One or both matrix objects are null.");
        return NULL;
    }
    if (matrix1->rows != matrix2->rows) {
        MATRIX_LOG("[matrix_hstack] Error: Row counts differ (%zu vs %zu).",
                   matrix1->rows, matrix2->rows);
        return NULL;
    }

    size_t rows = matrix1->rows;
    size_t c1 = matrix1->cols;
    size_t c2 = matrix2->cols;
    Matrix* result = matrix_create(rows, c1 + c2);
    if (!result) {
        MATRIX_LOG("[matrix_hstack] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < rows; ++i) {
        memcpy(&result->data[i * (c1 + c2)],      &matrix1->data[i * c1], c1 * sizeof(double));
        memcpy(&result->data[i * (c1 + c2) + c1], &matrix2->data[i * c2], c2 * sizeof(double));
    }

    MATRIX_LOG("[matrix_hstack] Success: matrices stacked horizontally.");
    return result;
}

/**
 * @brief Concatenates two matrices vertically (one above the other).
 *
 * The C analogue of NumPy's `numpy.vstack((A, B))` for 2-D arrays: places `B`
 * below `A`. The two matrices must have the same number of columns; the result
 * has `rows(A) + rows(B)` rows and that many columns. Neither input is modified.
 *
 * @param matrix1 The top matrix. Must not be NULL.
 * @param matrix2 The bottom matrix, with the same number of columns. Must not be NULL.
 *
 * @return A new `(rows1 + rows2) x cols` matrix, or `NULL` if either matrix is
 *         NULL, the column counts differ, or allocation fails. The caller owns
 *         the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_vstack(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_vstack] Entering function");

    if (!matrix1 || !matrix2 || !matrix1->data || !matrix2->data) {
        MATRIX_LOG("[matrix_vstack] Error: One or both matrix objects are null.");
        return NULL;
    }
    if (matrix1->cols != matrix2->cols) {
        MATRIX_LOG("[matrix_vstack] Error: Column counts differ (%zu vs %zu).",
                   matrix1->cols, matrix2->cols);
        return NULL;
    }

    size_t cols = matrix1->cols;
    size_t r1 = matrix1->rows;
    size_t r2 = matrix2->rows;
    Matrix* result = matrix_create(r1 + r2, cols);
    if (!result) {
        MATRIX_LOG("[matrix_vstack] Error: Failed to create result matrix.");
        return NULL;
    }

    /* Row-major contiguous storage: copy all of matrix1, then all of matrix2. */
    memcpy(result->data,                 matrix1->data, r1 * cols * sizeof(double));
    memcpy(&result->data[r1 * cols],     matrix2->data, r2 * cols * sizeof(double));

    MATRIX_LOG("[matrix_vstack] Success: matrices stacked vertically.");
    return result;
}

/* Ascending comparator for `double`s; NaN sorts to the end, matching NumPy. */
static int mat_cmp_asc(const void* pa, const void* pb) {
    double a = *(const double*)pa;
    double b = *(const double*)pb;
    if (isnan(a)) {
        return isnan(b) ? 0 : 1;
    }
    if (isnan(b)) {
        return -1;
    }
    return (a > b) - (a < b);
}

/* (value, original index) pair used to build a stable argsort. */
typedef struct {
    double value;
    size_t index;
} mat_vi;

/* Orders by value ascending (NaN last), breaking ties by original index so the
 * argsort is stable (first occurrence wins), like NumPy's `kind='stable'`. */
static int mat_cmp_vi(const void* pa, const void* pb) {
    const mat_vi* a = (const mat_vi*)pa;
    const mat_vi* b = (const mat_vi*)pb;
    if (isnan(a->value)) {
        if (!isnan(b->value)) {
            return 1;
        }
    }
    else if (isnan(b->value)) {
        return -1;
    }
    else if (a->value < b->value) {
        return -1;
    }
    else if (a->value > b->value) {
        return 1;
    }
    return (a->index > b->index) - (a->index < b->index);
}

/**
 * @brief Tiles (repeats) a matrix in a grid.
 *
 * The C analogue of NumPy's `numpy.tile(M, (reps_row, reps_col))`: stacks
 * `reps_row` copies of `matrix` vertically and `reps_col` copies horizontally,
 * producing a `(rows*reps_row) x (cols*reps_col)` matrix. The input is not
 * modified.
 *
 * @param matrix   The source matrix. Must not be NULL.
 * @param reps_row Number of vertical repetitions. Must be greater than 0.
 * @param reps_col Number of horizontal repetitions. Must be greater than 0.
 *
 * @return A new tiled matrix, or `NULL` if `matrix` is NULL, either repetition
 *         count is 0, or allocation fails. The caller owns the result and must
 *         release it with `matrix_deallocate`.
 */
Matrix* matrix_tile(const Matrix* matrix, size_t reps_row, size_t reps_col) {
    MATRIX_LOG("[matrix_tile] Entering function with reps = (%zu, %zu)", reps_row, reps_col);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_tile] Error: Invalid matrix provided.");
        return NULL;
    }
    if (reps_row == 0 || reps_col == 0) {
        MATRIX_LOG("[matrix_tile] Error: Repetition counts must be greater than 0.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    size_t newC, newR;
    if (!mat_size_mul(C, reps_col, &newC) || !mat_size_mul(R, reps_row, &newR)) {
        MATRIX_LOG("[matrix_tile] Error: result size overflows.");
        return NULL;
    }
    Matrix* result = matrix_create(newR, newC);
    if (!result) {
        MATRIX_LOG("[matrix_tile] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < newR; ++i) {
        for (size_t j = 0; j < newC; ++j) {
            result->data[i * newC + j] = matrix->data[(i % R) * C + (j % C)];
        }
    }

    MATRIX_LOG("[matrix_tile] Success: matrix tiled.");
    return result;
}

/**
 * @brief Repeats each element of a matrix (flattened).
 *
 * The C analogue of NumPy's `numpy.repeat(M, repeats)` with the default
 * `axis=None`: the matrix is flattened in row-major order and each element is
 * repeated `repeats` times consecutively, returning a `1 x (rows*cols*repeats)`
 * row vector. For per-row / per-column repetition use `matrix_repeat_axis`.
 *
 * @param matrix  The source matrix. Must not be NULL.
 * @param repeats Number of times to repeat each element. Must be greater than 0.
 *
 * @return A new `1 x (rows*cols*repeats)` row vector, or `NULL` if `matrix` is
 *         NULL/invalid, `repeats` is 0, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_repeat(const Matrix* matrix, size_t repeats) {
    MATRIX_LOG("[matrix_repeat] Entering function with repeats = %zu", repeats);

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_repeat] Error: Invalid matrix provided.");
        return NULL;
    }
    if (repeats == 0) {
        MATRIX_LOG("[matrix_repeat] Error: repeats must be greater than 0.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    size_t total;
    if (!mat_size_mul(n, repeats, &total)) {
        MATRIX_LOG("[matrix_repeat] Error: result size overflows.");
        return NULL;
    }
    Matrix* result = matrix_create(1, total);
    if (!result) {
        MATRIX_LOG("[matrix_repeat] Error: Failed to create result vector.");
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t r = 0; r < repeats; ++r) {
            result->data[i * repeats + r] = matrix->data[i];
        }
    }

    MATRIX_LOG("[matrix_repeat] Success: elements repeated.");
    return result;
}

/**
 * @brief Repeats rows or columns of a matrix.
 *
 * The C analogue of NumPy's `numpy.repeat(M, repeats, axis=...)`:
 *  - `axis == 0` repeats each row `repeats` times, returning a
 *    `(rows*repeats) x cols` matrix;
 *  - `axis == 1` repeats each column `repeats` times, returning a
 *    `rows x (cols*repeats)` matrix.
 *
 * @param matrix  The source matrix. Must not be NULL.
 * @param repeats Number of repetitions. Must be greater than 0.
 * @param axis    `0` to repeat rows, `1` to repeat columns.
 *
 * @return A new matrix with rows/columns repeated, or `NULL` if `matrix` is
 *         NULL, `repeats` is 0, `axis` is neither 0 nor 1, or allocation fails.
 *         The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_repeat_axis(const Matrix* matrix, size_t repeats, int axis) {
    MATRIX_LOG("[matrix_repeat_axis] Entering function with repeats = %zu, axis = %d", repeats, axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_repeat_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (repeats == 0) {
        MATRIX_LOG("[matrix_repeat_axis] Error: repeats must be greater than 0.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_repeat_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    size_t scaled;
    if (!mat_size_mul(axis == 0 ? R : C, repeats, &scaled)) {
        MATRIX_LOG("[matrix_repeat_axis] Error: result size overflows.");
        return NULL;
    }

    if (axis == 0) {
        Matrix* result = matrix_create(scaled, C);
        if (!result) {
            MATRIX_LOG("[matrix_repeat_axis] Error: Failed to create result matrix.");
            return NULL;
        }
        for (size_t i = 0; i < R; ++i) {
            for (size_t r = 0; r < repeats; ++r) {
                memcpy(&result->data[(i * repeats + r) * C], &matrix->data[i * C], C * sizeof(double));
            }
        }
        MATRIX_LOG("[matrix_repeat_axis] Success: rows repeated.");
        return result;
    }

    size_t newC = scaled;
    Matrix* result = matrix_create(R, newC);
    if (!result) {
        MATRIX_LOG("[matrix_repeat_axis] Error: Failed to create result matrix.");
        return NULL;
    }
    for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) {
            for (size_t r = 0; r < repeats; ++r) {
                result->data[i * newC + (j * repeats + r)] = matrix->data[i * C + j];
            }
        }
    }

    MATRIX_LOG("[matrix_repeat_axis] Success: columns repeated.");
    return result;
}

/**
 * @brief Pads a matrix with a constant-valued border.
 *
 * The C analogue of NumPy's `numpy.pad(M, pad, mode='constant',
 * constant_values=value)`: surrounds the matrix with a border of width `pad`
 * filled with `value` on all four sides, producing a
 * `(rows + 2*pad) x (cols + 2*pad)` matrix. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param pad    Border width added on every side (0 yields a plain copy).
 * @param value  The constant value used to fill the border.
 *
 * @return A new padded matrix, or `NULL` if `matrix` is NULL or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_pad(const Matrix* matrix, size_t pad, double value) {
    MATRIX_LOG("[matrix_pad] Entering function with pad = %zu", pad);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_pad] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    size_t pad2, newR, newC;
    if (!mat_size_mul(pad, 2, &pad2) || !mat_size_add(R, pad2, &newR) || !mat_size_add(C, pad2, &newC)) {
        MATRIX_LOG("[matrix_pad] Error: result size overflows.");
        return NULL;
    }
    Matrix* result = matrix_create(newR, newC);
    if (!result) {
        MATRIX_LOG("[matrix_pad] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < newR * newC; ++i) {
        result->data[i] = value;
    }
    for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) {
            result->data[(i + pad) * newC + (j + pad)] = matrix->data[i * C + j];
        }
    }

    MATRIX_LOG("[matrix_pad] Success: matrix padded.");
    return result;
}

/**
 * @brief Returns a copy of a matrix sorted along one axis.
 *
 * The C analogue of NumPy's `numpy.sort(M, axis=...)`: each row (`axis == 1`) or
 * each column (`axis == 0`) is sorted independently in ascending order. NumPy's
 * default is `axis=-1`, which for a 2-D array means `axis == 1` here. The input
 * is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param axis   `0` to sort each column, `1` to sort each row.
 *
 * @return A new matrix (same shape) sorted along the chosen axis, or `NULL` if
 *         `matrix` is NULL, `axis` is neither 0 nor 1, or allocation fails. The
 *         caller owns the result and must release it with `matrix_deallocate`.
 *
 * @note `NaN` values are ordered after all finite values, matching NumPy.
 */
Matrix* matrix_sort(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_sort] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_sort] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_sort] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_copy(matrix);
    if (!result) {
        MATRIX_LOG("[matrix_sort] Error: Failed to copy matrix.");
        return NULL;
    }

    if (axis == 1) {
        /* each row occupies a contiguous run, so it can be sorted in place */
        for (size_t i = 0; i < R; ++i) {
            qsort(&result->data[i * C], C, sizeof(double), mat_cmp_asc);
        }
    }
    else {
        double* col = (double*)malloc(R * sizeof(double));
        if (!col) {
            MATRIX_LOG("[matrix_sort] Error: Column buffer allocation failed.");
            matrix_deallocate(result);
            return NULL;
        }
        for (size_t j = 0; j < C; ++j) {
            for (size_t i = 0; i < R; ++i) {
                col[i] = result->data[i * C + j];
            }
            qsort(col, R, sizeof(double), mat_cmp_asc);
            for (size_t i = 0; i < R; ++i) {
                result->data[i * C + j] = col[i];
            }
        }
        free(col);
    }

    MATRIX_LOG("[matrix_sort] Success: matrix sorted.");
    return result;
}

/**
 * @brief Returns the indices that would sort a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.argsort(M, axis=...)`: for each row
 * (`axis == 1`) or column (`axis == 0`), returns the per-axis indices that would
 * place that row/column in ascending order. The result has the same shape as the
 * input, with the integer indices stored as `double` values. NumPy's default is
 * `axis=-1`, which for a 2-D array means `axis == 1` here. The input is not
 * modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param axis   `0` for column-wise indices, `1` for row-wise indices.
 *
 * @return A new matrix (same shape) of sorting indices, or `NULL` if `matrix` is
 *         NULL, `axis` is neither 0 nor 1, or allocation fails. The caller owns
 *         the result and must release it with `matrix_deallocate`.
 *
 * @note The sort is stable: ties keep their original order (first occurrence
 *       first), matching NumPy's `kind='stable'`. `NaN` is ordered last.
 */
Matrix* matrix_argsort(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_argsort] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_argsort] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_argsort] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        MATRIX_LOG("[matrix_argsort] Error: Failed to create result matrix.");
        return NULL;
    }

    size_t len = (axis == 1) ? C : R;
    mat_vi* buf = (mat_vi*)malloc(len * sizeof(mat_vi));
    if (!buf) {
        MATRIX_LOG("[matrix_argsort] Error: Index buffer allocation failed.");
        matrix_deallocate(result);
        return NULL;
    }

    if (axis == 1) {
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                buf[j].value = matrix->data[i * C + j];
                buf[j].index = j;
            }
            qsort(buf, C, sizeof(mat_vi), mat_cmp_vi);
            for (size_t j = 0; j < C; ++j) {
                result->data[i * C + j] = (double)buf[j].index;
            }
        }
    }
    else {
        for (size_t j = 0; j < C; ++j) {
            for (size_t i = 0; i < R; ++i) {
                buf[i].value = matrix->data[i * C + j];
                buf[i].index = i;
            }
            qsort(buf, R, sizeof(mat_vi), mat_cmp_vi);
            for (size_t i = 0; i < R; ++i) {
                result->data[i * C + j] = (double)buf[i].index;
            }
        }
    }

    free(buf);
    MATRIX_LOG("[matrix_argsort] Success: sorting indices computed.");
    return result;
}

/**
 * @brief Computes the variance of all elements of a matrix.
 *
 * The C analogue of NumPy's `numpy.var(M)`: the population variance with
 * `ddof = 0`, i.e. `mean((x - mean(x))^2)`.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return The population variance, or `0.0` if `matrix` is NULL/invalid.
 *
 * @note Uses `ddof = 0` (divide by `N`), matching NumPy's default. For the
 *       sample variance divide the result by `(N-1)/N`.
 */
double matrix_var(const Matrix* matrix) {
    MATRIX_LOG("[matrix_var] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_var] Error: Invalid matrix provided.");
        return 0.0;
    }

    size_t n = matrix->rows * matrix->cols;
    double mean = matrix_mean(matrix);
    double sumsq = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double diff = matrix->data[i] - mean;
        sumsq += diff * diff;
    }

    double var = sumsq / (double)n;
    MATRIX_LOG("[matrix_var] var = %f", var);
    return var;
}

/**
 * @brief Computes the standard deviation of all elements of a matrix.
 *
 * The C analogue of NumPy's `numpy.std(M)`: the population standard deviation
 * with `ddof = 0`, i.e. `sqrt(matrix_var(M))`.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return The population standard deviation, or `0.0` if `matrix` is
 *         NULL/invalid.
 */
double matrix_std(const Matrix* matrix) {
    MATRIX_LOG("[matrix_std] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_std] Error: Invalid matrix provided.");
        return 0.0;
    }

    double std = sqrt(matrix_var(matrix));
    MATRIX_LOG("[matrix_std] std = %f", std);
    return std;
}

/**
 * @brief Computes the median of all elements of a matrix.
 *
 * The C analogue of NumPy's `numpy.median(M)`: the matrix is flattened, sorted,
 * and the middle value is returned (the average of the two middle values when
 * the element count is even). The input is not modified.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return The median of all elements, or `0.0` if `matrix` is NULL/invalid or a
 *         temporary allocation fails.
 */
double matrix_median(const Matrix* matrix) {
    MATRIX_LOG("[matrix_median] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_median] Error: Invalid matrix provided.");
        return 0.0;
    }

    size_t n = matrix->rows * matrix->cols;
    double* tmp = (double*)malloc(n * sizeof(double));
    if (!tmp) {
        MATRIX_LOG("[matrix_median] Error: Temporary buffer allocation failed.");
        return 0.0;
    }
    memcpy(tmp, matrix->data, n * sizeof(double));
    qsort(tmp, n, sizeof(double), mat_cmp_asc);

    double median;
    if (isnan(tmp[n - 1])) {
        median = tmp[n - 1];   /* NaN sorts last: a NaN anywhere -> NaN (NumPy) */
    }
    else if (n % 2 == 1) {
        median = tmp[n / 2];
    }
    else {
        median = (tmp[n / 2 - 1] + tmp[n / 2]) / 2.0;
    }

    free(tmp);
    MATRIX_LOG("[matrix_median] median = %f", median);
    return median;
}

/**
 * @brief Computes the first discrete difference along one axis.
 *
 * The C analogue of NumPy's `numpy.diff(M, axis=...)` (first order, `n = 1`):
 * each element becomes the difference between it and the next element along the
 * axis. `axis = 1` differences along each row (result loses one column);
 * `axis = 0` differences along each column (result loses one row). NumPy's
 * default `axis=-1` corresponds to `axis = 1` here. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param axis   `0` to difference down columns, `1` along rows.
 *
 * @return A new matrix one shorter along the chosen axis, or `NULL` if `matrix`
 *         is NULL, `axis` is neither 0 nor 1, the axis length is less than 2
 *         (the result would be empty), or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_diff(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_diff] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_diff] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_diff] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;

    if (axis == 1) {
        if (C < 2) {
            MATRIX_LOG("[matrix_diff] Error: need at least 2 columns for axis 1.");
            return NULL;
        }
        Matrix* result = matrix_create(R, C - 1);
        if (!result) {
            return NULL;
        }
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C - 1; ++j) {
                result->data[i * (C - 1) + j] =
                    matrix->data[i * C + (j + 1)] - matrix->data[i * C + j];
            }
        }
        MATRIX_LOG("[matrix_diff] Success: row differences computed.");
        return result;
    }

    if (R < 2) {
        MATRIX_LOG("[matrix_diff] Error: need at least 2 rows for axis 0.");
        return NULL;
    }
    Matrix* result = matrix_create(R - 1, C);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < R - 1; ++i) {
        for (size_t j = 0; j < C; ++j) {
            result->data[i * C + j] =
                matrix->data[(i + 1) * C + j] - matrix->data[i * C + j];
        }
    }
    MATRIX_LOG("[matrix_diff] Success: column differences computed.");
    return result;
}

/**
 * @brief Tests whether two matrices are equal within a tolerance.
 *
 * The C analogue of NumPy's `numpy.allclose(A, B, rtol, atol)`: returns `true`
 * when every pair of corresponding elements satisfies
 * `|a - b| <= atol + rtol * |b|`. The two matrices must have identical
 * dimensions (no broadcasting). NumPy's defaults are `rtol = 1e-5`,
 * `atol = 1e-8`.
 *
 * @param matrix1 First matrix. Must not be NULL.
 * @param matrix2 Second matrix, same dimensions as `matrix1`. Must not be NULL.
 * @param rtol    Relative tolerance.
 * @param atol    Absolute tolerance.
 *
 * @return `true` if the matrices are element-wise close; `false` if either is
 *         NULL, the dimensions differ, or any pair is not within tolerance.
 *
 * @note Matching NumPy's default (`equal_nan=False`), any `NaN` makes the
 *       result `false`. Equal infinities of the same sign compare as close.
 */
bool matrix_allclose(const Matrix* matrix1, const Matrix* matrix2, double rtol, double atol) {
    MATRIX_LOG("[matrix_allclose] Entering function");

    if (!matrix1 || !matrix2 || !matrix1->data || !matrix2->data) {
        MATRIX_LOG("[matrix_allclose] Error: One or both matrix objects are null.");
        return false;
    }
    if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        MATRIX_LOG("[matrix_allclose] Error: The two matrices are not of the same order.");
        return false;
    }

    size_t n = matrix1->rows * matrix1->cols;
    for (size_t i = 0; i < n; ++i) {
        double a = matrix1->data[i];
        double b = matrix2->data[i];
        if (a == b) {
            continue;   /* exact equality, incl. equal infinities of same sign */
        }
        if (fabs(a - b) <= atol + rtol * fabs(b)) {
            continue;   /* within tolerance (NaN falls through to false) */
        }
        MATRIX_LOG("[matrix_allclose] Not close at index %zu.", i);
        return false;
    }

    MATRIX_LOG("[matrix_allclose] Success: matrices are element-wise close.");
    return true;
}

/**
 * @brief Computes the outer product of two vectors.
 *
 * The C analogue of NumPy's `numpy.outer(a, b)`: both inputs are flattened to
 * 1-D, and the result is an `len(a) x len(b)` matrix where
 * `result[i][j] = a[i] * b[j]`. Neither input is modified.
 *
 * @param a The first vector (any shape; flattened). Must not be NULL.
 * @param b The second vector (any shape; flattened). Must not be NULL.
 *
 * @return A new `len(a) x len(b)` matrix, or `NULL` if either input is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_outer(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_outer] Entering function");

    if (!a || !b || !a->data || !b->data) {
        MATRIX_LOG("[matrix_outer] Error: One or both vectors are null.");
        return NULL;
    }

    size_t M = a->rows * a->cols;
    size_t N = b->rows * b->cols;
    if (M == 0 || N == 0) {
        MATRIX_LOG("[matrix_outer] Error: Empty input vector.");
        return NULL;
    }

    Matrix* result = matrix_create(M, N);
    if (!result) {
        MATRIX_LOG("[matrix_outer] Error: Failed to create result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            result->data[i * N + j] = a->data[i] * b->data[j];
        }
    }

    MATRIX_LOG("[matrix_outer] Success: outer product computed.");
    return result;
}

/**
 * @brief Computes the cross product of two 3-element vectors.
 *
 * The C analogue of NumPy's `numpy.cross(a, b)` for 3-D vectors: returns the
 * vector `a x b = (a1*b2 - a2*b1, a2*b0 - a0*b2, a0*b1 - a1*b0)` as a `1 x 3`
 * row vector. Both inputs must contain exactly 3 elements (any shape, flattened).
 * Neither input is modified.
 *
 * @param a The first 3-element vector. Must not be NULL.
 * @param b The second 3-element vector. Must not be NULL.
 *
 * @return A new `1 x 3` matrix holding the cross product, or `NULL` if either
 *         input is NULL, does not have exactly 3 elements, or allocation fails.
 *         The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_cross(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_cross] Entering function");

    if (!a || !b || !a->data || !b->data) {
        MATRIX_LOG("[matrix_cross] Error: One or both vectors are null.");
        return NULL;
    }
    if (a->rows * a->cols != 3 || b->rows * b->cols != 3) {
        MATRIX_LOG("[matrix_cross] Error: Both vectors must have exactly 3 elements.");
        return NULL;
    }

    Matrix* result = matrix_create(1, 3);
    if (!result) {
        MATRIX_LOG("[matrix_cross] Error: Failed to create result vector.");
        return NULL;
    }

    double a0 = a->data[0], a1 = a->data[1], a2 = a->data[2];
    double b0 = b->data[0], b1 = b->data[1], b2 = b->data[2];
    result->data[0] = a1 * b2 - a2 * b1;
    result->data[1] = a2 * b0 - a0 * b2;
    result->data[2] = a0 * b1 - a1 * b0;

    MATRIX_LOG("[matrix_cross] Success: cross product computed.");
    return result;
}

/**
 * @brief Estimates the covariance matrix of a set of observations.
 *
 * The C analogue of NumPy's `numpy.cov(M)` with the defaults `rowvar=True` and
 * `bias=False`: each **row** of `matrix` is a variable and each **column** an
 * observation. For an `m x N` input it returns the `m x m` covariance matrix,
 * where `cov[i][k] = sum_j (x_ij - mean_i)(x_kj - mean_k) / (N - 1)`. The input
 * is not modified.
 *
 * @param matrix The observation matrix (rows = variables, cols = observations).
 *               Must not be NULL and must have at least 2 columns.
 *
 * @return A new `m x m` covariance matrix, or `NULL` if `matrix` is NULL, has
 *         fewer than 2 observations, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note Uses `ddof = 1` (divide by `N-1`), matching NumPy's default
 *       (`bias=False`).
 */
Matrix* matrix_cov(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cov] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_cov] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t m = matrix->rows;   /* variables   */
    size_t N = matrix->cols;   /* observations */
    if (N < 2) {
        MATRIX_LOG("[matrix_cov] Error: need at least 2 observations (columns).");
        return NULL;
    }

    Matrix* result = matrix_create(m, m);
    if (!result) {
        MATRIX_LOG("[matrix_cov] Error: Failed to create result matrix.");
        return NULL;
    }

    double* means = (double*)malloc(m * sizeof(double));
    if (!means) {
        MATRIX_LOG("[matrix_cov] Error: Means buffer allocation failed.");
        matrix_deallocate(result);
        return NULL;
    }
    for (size_t i = 0; i < m; ++i) {
        double s = 0.0;
        for (size_t j = 0; j < N; ++j) {
            s += matrix->data[i * N + j];
        }
        means[i] = s / (double)N;
    }

    for (size_t i = 0; i < m; ++i) {
        for (size_t k = 0; k < m; ++k) {
            double s = 0.0;
            for (size_t j = 0; j < N; ++j) {
                s += (matrix->data[i * N + j] - means[i]) *
                     (matrix->data[k * N + j] - means[k]);
            }
            result->data[i * m + k] = s / (double)(N - 1);
        }
    }

    free(means);
    MATRIX_LOG("[matrix_cov] Success: covariance matrix computed.");
    return result;
}

/**
 * @brief Computes the Pearson correlation-coefficient matrix.
 *
 * The C analogue of NumPy's `numpy.corrcoef(M)` with the default `rowvar=True`:
 * each row of `matrix` is a variable. Returns the `m x m` matrix whose entries
 * are `corr[i][k] = cov[i][k] / sqrt(cov[i][i] * cov[k][k])`, i.e. the
 * covariance normalised by the standard deviations. The diagonal is `1`, and the
 * result is clamped to `[-1, 1]` (as NumPy does to absorb rounding). The input
 * is not modified.
 *
 * @param matrix The observation matrix (rows = variables, cols = observations).
 *               Must not be NULL and must have at least 2 columns.
 *
 * @return A new `m x m` correlation matrix, or `NULL` if `matrix` is NULL, has
 *         fewer than 2 observations, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 *
 * @note A variable with zero variance yields `nan` correlations, matching NumPy.
 */
Matrix* matrix_corrcoef(const Matrix* matrix) {
    MATRIX_LOG("[matrix_corrcoef] Entering function");

    Matrix* cov = matrix_cov(matrix);
    if (!cov) {
        MATRIX_LOG("[matrix_corrcoef] Error: covariance computation failed.");
        return NULL;
    }

    size_t m = cov->rows;
    Matrix* result = matrix_create(m, m);
    if (!result) {
        MATRIX_LOG("[matrix_corrcoef] Error: Failed to create result matrix.");
        matrix_deallocate(cov);
        return NULL;
    }

    for (size_t i = 0; i < m; ++i) {
        for (size_t k = 0; k < m; ++k) {
            double denom = sqrt(cov->data[i * m + i] * cov->data[k * m + k]);
            double c = cov->data[i * m + k] / denom;
            if (c > 1.0) {
                c = 1.0;
            }
            else if (c < -1.0) {
                c = -1.0;
            }
            result->data[i * m + k] = c;
        }
    }

    matrix_deallocate(cov);
    MATRIX_LOG("[matrix_corrcoef] Success: correlation matrix computed.");
    return result;
}

/**
 * @brief Computes the full discrete linear convolution of two 1-D sequences.
 *
 * The C analogue of NumPy's `numpy.convolve(a, v)` with the default
 * `mode='full'`: both inputs are flattened to 1-D and convolved, producing a
 * `1 x (len(a) + len(v) - 1)` row vector where
 * `out[n] = sum_k a[k] * v[n-k]`. Neither input is modified.
 *
 * @param a The first sequence (any shape; flattened). Must not be NULL.
 * @param v The second sequence (any shape; flattened). Must not be NULL.
 *
 * @return A new `1 x (M + N - 1)` row vector holding the convolution, or `NULL`
 *         if either input is NULL/empty or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_convolve(const Matrix* a, const Matrix* v) {
    MATRIX_LOG("[matrix_convolve] Entering function");

    if (!a || !v || !a->data || !v->data) {
        MATRIX_LOG("[matrix_convolve] Error: One or both sequences are null.");
        return NULL;
    }

    size_t M = a->rows * a->cols;
    size_t N = v->rows * v->cols;
    if (M == 0 || N == 0) {
        MATRIX_LOG("[matrix_convolve] Error: Empty input sequence.");
        return NULL;
    }

    size_t L = M + N - 1;
    Matrix* result = matrix_create(1, L);
    if (!result) {
        MATRIX_LOG("[matrix_convolve] Error: Failed to create result vector.");
        return NULL;
    }

    for (size_t n = 0; n < L; ++n) {
        double sum = 0.0;
        size_t kstart = (n >= N - 1) ? (n - (N - 1)) : 0;
        size_t kend = (n < M) ? n : (M - 1);
        for (size_t k = kstart; k <= kend; ++k) {
            sum += a->data[k] * v->data[n - k];
        }
        result->data[n] = sum;
    }

    MATRIX_LOG("[matrix_convolve] Success: convolution computed.");
    return result;
}

/*
 * Cyclic Jacobi eigenvalue algorithm for a real symmetric matrix.
 * `A` is n x n symmetric; on success `eigvals` (length n) holds the eigenvalues
 * and `V` (n x n) holds the eigenvectors as columns, both unsorted. The original
 * matrix is not modified. Returns false on allocation failure or non-convergence.
 */
static bool mat_jacobi(const Matrix* A, double* eigvals, Matrix* V) {
    size_t n = A->rows;
    double* a = (double*)malloc(n * n * sizeof(double));
    if (!a) {
        return false;
    }
    memcpy(a, A->data, n * n * sizeof(double));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            V->data[i * n + j] = (i == j) ? 1.0 : 0.0;
        }
    }

    /* Convergence is judged relative to the matrix's own size: the old
       absolute threshold (off <= 1e-30) declared any matrix with entries of
       about 1e-15 converged before a single rotation. Sums are taken over
       entries scaled by the largest magnitude so they cannot overflow; the
       Frobenius norm is invariant under the rotations, so it is fixed. */
    double scale = 0.0;
    for (size_t i = 0; i < n * n; ++i) {
        if (fabs(a[i]) > scale) {
            scale = fabs(a[i]);
        }
    }
    double fro2 = 0.0;
    if (scale > 0.0) {
        for (size_t i = 0; i < n * n; ++i) {
            double x = a[i] / scale;
            fro2 += x * x;
        }
    }
    const double tol2 = (4.0 * DBL_EPSILON) * (4.0 * DBL_EPSILON) * fro2;

    const int max_sweeps = 100;
    bool converged = false;
    for (int sweep = 0; sweep < max_sweeps && !converged; ++sweep) {
        double off = 0.0;
        if (scale > 0.0) {
            for (size_t p = 0; p < n; ++p) {
                for (size_t q = p + 1; q < n; ++q) {
                    double x = a[p * n + q] / scale;
                    off += x * x;
                }
            }
        }
        if (off <= tol2) {
            converged = true;
            break;
        }

        for (size_t p = 0; p < n; ++p) {
            for (size_t q = p + 1; q < n; ++q) {
                double apq = a[p * n + q];
                if (apq == 0.0) {
                    continue;
                }
                double app = a[p * n + p];
                double aqq = a[q * n + q];
                /* An entry far below the eps-level of its diagonal pair no
                   longer changes the result; dropping it guarantees the
                   sweeps finish. */
                if (fabs(apq) <= 0.25 * DBL_EPSILON * (fabs(app) + fabs(aqq))) {
                    a[p * n + q] = 0.0;
                    a[q * n + p] = 0.0;
                    continue;
                }
                double phi = 0.5 * atan2(2.0 * apq, app - aqq);
                double c = cos(phi);
                double s = sin(phi);

                /* A := A * J  (rotate columns p, q) */
                for (size_t k = 0; k < n; ++k) {
                    double akp = a[k * n + p];
                    double akq = a[k * n + q];
                    a[k * n + p] = c * akp + s * akq;
                    a[k * n + q] = -s * akp + c * akq;
                }
                /* A := J^T * A  (rotate rows p, q) */
                for (size_t k = 0; k < n; ++k) {
                    double apk = a[p * n + k];
                    double aqk = a[q * n + k];
                    a[p * n + k] = c * apk + s * aqk;
                    a[q * n + k] = -s * apk + c * aqk;
                }
                /* V := V * J  (accumulate eigenvectors) */
                for (size_t k = 0; k < n; ++k) {
                    double vkp = V->data[k * n + p];
                    double vkq = V->data[k * n + q];
                    V->data[k * n + p] = c * vkp + s * vkq;
                    V->data[k * n + q] = -s * vkp + c * vkq;
                }
            }
        }
    }

    for (size_t i = 0; i < n; ++i) {
        eigvals[i] = a[i * n + i];
    }
    free(a);
    return converged;
}

/* Swaps columns `i` and `j` of an n-column matrix stored row-major. */
static void mat_swap_cols(Matrix* M, size_t i, size_t j) {
    size_t cols = M->cols;
    for (size_t r = 0; r < M->rows; ++r) {
        double tmp = M->data[r * cols + i];
        M->data[r * cols + i] = M->data[r * cols + j];
        M->data[r * cols + j] = tmp;
    }
}

/**
 * @brief Computes the eigenvalues and eigenvectors of a symmetric matrix.
 *
 * The C analogue of NumPy's `numpy.linalg.eig` for a real **symmetric** matrix
 * (equivalently `numpy.linalg.eigh`), computed with the cyclic Jacobi algorithm.
 * Because a real-valued matrix library cannot represent the complex eigenvalues
 * that a general matrix may have, the input must be symmetric (which guarantees
 * real eigenvalues and orthonormal eigenvectors).
 *
 * @param matrix       The symmetric matrix to decompose. Must not be NULL.
 * @param eigenvalues  Out-parameter: receives an `n x 1` column of eigenvalues
 *                     in ascending order (like `numpy.linalg.eigh`).
 * @param eigenvectors Out-parameter: receives an `n x n` matrix whose columns
 *                     are the corresponding orthonormal eigenvectors.
 *
 * @return `true` on success; `false` if `matrix` is NULL, not square, not
 *         symmetric, the out-pointers are NULL, allocation fails, or the
 *         iteration does not converge. On success the caller owns
 *         `*eigenvalues` and `*eigenvectors` and must release each with
 *         `matrix_deallocate`.
 *
 * @note Eigenvectors are unique only up to sign; a column may have the opposite
 *       sign to NumPy's. The eigenvalues and the relation `A·v = λ·v` are
 *       unaffected.
 */
bool matrix_eig(const Matrix* matrix, Matrix** eigenvalues, Matrix** eigenvectors) {
    MATRIX_LOG("[matrix_eig] Entering function");

    if (!matrix || !eigenvalues || !eigenvectors) {
        MATRIX_LOG("[matrix_eig] Error: NULL argument provided.");
        return false;
    }
    if (!matrix_is_symmetric(matrix)) {
        MATRIX_LOG("[matrix_eig] Error: matrix must be square and symmetric.");
        return false;
    }

    size_t n = matrix->rows;
    double* vals = (double*)malloc(n * sizeof(double));
    Matrix* V = matrix_create(n, n);
    if (!vals || !V) {
        free(vals);
        if (V) {
            matrix_deallocate(V);
        }
        return false;
    }

    if (!mat_jacobi(matrix, vals, V)) {
        MATRIX_LOG("[matrix_eig] Error: Jacobi iteration failed.");
        free(vals);
        matrix_deallocate(V);
        return false;
    }

    /* sort eigenvalues ascending, reordering eigenvector columns to match */
    for (size_t i = 0; i < n; ++i) {
        size_t best = i;
        for (size_t j = i + 1; j < n; ++j) {
            if (vals[j] < vals[best]) {
                best = j;
            }
        }
        if (best != i) {
            double tmp = vals[i];
            vals[i] = vals[best];
            vals[best] = tmp;
            mat_swap_cols(V, i, best);
        }
    }

    Matrix* W = matrix_create(n, 1);
    if (!W) {
        free(vals);
        matrix_deallocate(V);
        return false;
    }
    for (size_t i = 0; i < n; ++i) {
        W->data[i] = vals[i];
    }
    free(vals);

    *eigenvalues = W;
    *eigenvectors = V;
    MATRIX_LOG("[matrix_eig] Success: eigen-decomposition computed.");
    return true;
}

/**
 * @brief Computes the (thin) singular value decomposition of a matrix.
 *
 * The C analogue of NumPy's `numpy.linalg.svd(M, full_matrices=False)`: factors
 * an `m x n` matrix (with `m >= n`) as `A = U · diag(S) · Vᵀ`, where `U` is
 * `m x n` with orthonormal columns, `S` is the `n x 1` column of singular values
 * in descending order, and `V` is the `n x n` orthogonal matrix of right
 * singular vectors. Computed with one-sided Jacobi rotations on `A` itself, which
 * keeps small singular values accurate (forming `AᵀA` would not).
 *
 * @param matrix The matrix to decompose. Must not be NULL and must have at least
 *               as many rows as columns (`m >= n`).
 * @param U      Out-parameter: receives the `m x n` left singular vectors.
 * @param S      Out-parameter: receives the `n x 1` singular values (descending).
 * @param V      Out-parameter: receives the `n x n` right singular vectors.
 *
 * @return `true` on success; `false` if `matrix` is NULL, has fewer rows than
 *         columns, an out-pointer is NULL, allocation fails, or the iteration
 *         does not converge. On success the caller owns `*U`, `*S`, `*V` and
 *         must release each with `matrix_deallocate`.
 *
 * @note This is the economy SVD (NumPy's `full_matrices=False`). Singular values
 *       are unique and match NumPy; the singular vectors are unique only up to
 *       sign, so columns of `U`/`V` may differ in sign from NumPy.
 */
bool matrix_svd(const Matrix* matrix, Matrix** U, Matrix** S, Matrix** V) {
    MATRIX_LOG("[matrix_svd] Entering function");

    if (!matrix || !U || !S || !V) {
        MATRIX_LOG("[matrix_svd] Error: NULL argument provided.");
        return false;
    }
    if (matrix->rows < matrix->cols) {
        MATRIX_LOG("[matrix_svd] Error: requires rows >= cols (thin SVD).");
        return false;
    }

    size_t m = matrix->rows;
    size_t n = matrix->cols;

    /* One-sided Jacobi (Hestenes): rotate pairs of columns of a copy of A
       until all columns are mutually orthogonal; the column norms are then
       the singular values. Working on A itself keeps full accuracy for small
       singular values, which forming AᵀA would square away (a zero singular
       value came out as ~1e-8·σmax, wrecking pinv, cond and rank). */
    Matrix* Umat = matrix_copy(matrix);
    Matrix* Vmat = matrix_create_identity(n);
    Matrix* Smat = matrix_create(n, 1);
    bool* good = (bool*)malloc(n * sizeof(bool));
    if (!Umat || !Vmat || !Smat || !good) {
        if (Umat) {
            matrix_deallocate(Umat);
        }
        if (Vmat) {
            matrix_deallocate(Vmat);
        }
        if (Smat) {
            matrix_deallocate(Smat);
        }
        free(good);
        return false;
    }
    double* u = Umat->data;
    double* v = Vmat->data;

    const int max_sweeps = 100;
    bool converged = (n < 2);
    for (int sweep = 0; sweep < max_sweeps && !converged; ++sweep) {
        bool rotated = false;
        for (size_t p = 0; p + 1 < n; ++p) {
            for (size_t q = p + 1; q < n; ++q) {
                double alpha = 0.0, beta = 0.0, gamma = 0.0;
                for (size_t r = 0; r < m; ++r) {
                    double up = u[r * n + p];
                    double uq = u[r * n + q];
                    alpha += up * up;
                    beta  += uq * uq;
                    gamma += up * uq;
                }
                if (gamma == 0.0 || fabs(gamma) <= DBL_EPSILON * sqrt(alpha) * sqrt(beta)) {
                    continue;   /* columns p and q are already orthogonal */
                }
                double zeta = (beta - alpha) / (2.0 * gamma);
                double t = (fabs(zeta) > 1e150) ? 0.5 / zeta
                         : ((zeta >= 0.0) ? 1.0 : -1.0) / (fabs(zeta) + sqrt(1.0 + zeta * zeta));
                if (t == 0.0) {
                    continue;
                }
                double c = 1.0 / sqrt(1.0 + t * t);
                double s = c * t;
                rotated = true;
                for (size_t r = 0; r < m; ++r) {
                    double up = u[r * n + p];
                    double uq = u[r * n + q];
                    u[r * n + p] = c * up - s * uq;
                    u[r * n + q] = s * up + c * uq;
                }
                for (size_t r = 0; r < n; ++r) {
                    double vp = v[r * n + p];
                    double vq = v[r * n + q];
                    v[r * n + p] = c * vp - s * vq;
                    v[r * n + q] = s * vp + c * vq;
                }
            }
        }
        if (!rotated) {
            converged = true;
        }
    }
    if (!converged) {
        MATRIX_LOG("[matrix_svd] Error: Jacobi iteration did not converge.");
        matrix_deallocate(Umat);
        matrix_deallocate(Vmat);
        matrix_deallocate(Smat);
        free(good);
        return false;
    }

    /* singular values = column norms */
    double* sigma = Smat->data;
    for (size_t i = 0; i < n; ++i) {
        double ss = 0.0;
        for (size_t r = 0; r < m; ++r) {
            ss += u[r * n + i] * u[r * n + i];
        }
        sigma[i] = sqrt(ss);
    }

    /* sort singular values descending, reordering U and V columns */
    for (size_t i = 0; i < n; ++i) {
        size_t best = i;
        for (size_t j = i + 1; j < n; ++j) {
            if (sigma[j] > sigma[best]) {
                best = j;
            }
        }
        if (best != i) {
            double tmp = sigma[i];
            sigma[i] = sigma[best];
            sigma[best] = tmp;
            mat_swap_cols(Umat, i, best);
            mat_swap_cols(Vmat, i, best);
        }
    }

    /* Normalize the columns of U. A column whose singular value is at the
       rounding-noise level carries no direction, so it is replaced by a unit
       vector orthogonal to the others; U keeps orthonormal columns. */
    double tolU = sigma[0] * (double)m * DBL_EPSILON;
    for (size_t i = 0; i < n; ++i) {
        good[i] = sigma[i] > tolU && sigma[i] > 0.0;
        if (good[i]) {
            for (size_t r = 0; r < m; ++r) {
                u[r * n + i] /= sigma[i];
            }
        }
    }
    for (size_t i = 0; i < n; ++i) {
        if (good[i]) {
            continue;
        }
        for (size_t k = 0; k < m && !good[i]; ++k) {
            for (size_t r = 0; r < m; ++r) {
                u[r * n + i] = (r == k) ? 1.0 : 0.0;
            }
            for (int pass = 0; pass < 2; ++pass) {   /* Gram-Schmidt, twice */
                for (size_t j = 0; j < n; ++j) {
                    if (!good[j]) {
                        continue;
                    }
                    double dot = 0.0;
                    for (size_t r = 0; r < m; ++r) {
                        dot += u[r * n + j] * u[r * n + i];
                    }
                    for (size_t r = 0; r < m; ++r) {
                        u[r * n + i] -= dot * u[r * n + j];
                    }
                }
            }
            double norm = 0.0;
            for (size_t r = 0; r < m; ++r) {
                norm += u[r * n + i] * u[r * n + i];
            }
            norm = sqrt(norm);
            if (norm > 0.5) {
                for (size_t r = 0; r < m; ++r) {
                    u[r * n + i] /= norm;
                }
                good[i] = true;
            }
        }
    }
    free(good);

    *U = Umat;
    *S = Smat;
    *V = Vmat;
    MATRIX_LOG("[matrix_svd] Success: SVD computed.");
    return true;
}

/**
 * @brief Computes the Moore-Penrose pseudo-inverse of a matrix.
 *
 * The C analogue of NumPy's `numpy.linalg.pinv(M)`, computed from the SVD as
 * `A⁺ = V · Σ⁺ · Uᵀ`, where `Σ⁺` inverts the non-negligible singular values.
 * Works for any shape: for an `m x n` input the result is `n x m`. (For `m < n`
 * the pseudo-inverse is obtained via `pinv(A) = pinv(Aᵀ)ᵀ`.)
 *
 * @param matrix The matrix to pseudo-invert. Must not be NULL.
 *
 * @return A new `n x m` pseudo-inverse, or `NULL` if `matrix` is NULL, the SVD
 *         fails, or allocation fails. The caller owns the result and must
 *         release it with `matrix_deallocate`.
 *
 * @note Singular values at or below `rcond · σ_max` (with `rcond` the larger of
 *       `1e-15` and `max(m, n)·eps`) are treated as zero, so the pseudo-inverse
 *       is robust to rank deficiency.
 */
Matrix* matrix_pinv(const Matrix* matrix) {
    MATRIX_LOG("[matrix_pinv] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_pinv] Error: Invalid matrix provided.");
        return NULL;
    }

    /* For wide matrices, invert the transpose and transpose the result. */
    if (matrix->rows < matrix->cols) {
        Matrix* At = matrix_transpose(matrix);
        if (!At) {
            return NULL;
        }
        Matrix* pt = matrix_pinv(At);
        matrix_deallocate(At);
        if (!pt) {
            return NULL;
        }
        Matrix* result = matrix_transpose(pt);
        matrix_deallocate(pt);
        return result;
    }

    size_t m = matrix->rows;
    size_t n = matrix->cols;

    Matrix* U = NULL;
    Matrix* S = NULL;
    Matrix* V = NULL;
    if (!matrix_svd(matrix, &U, &S, &V)) {
        MATRIX_LOG("[matrix_pinv] Error: SVD failed.");
        return NULL;
    }

    /* descending, so first is largest. Cutoff: NumPy's rcond = 1e-15, or
       max(m, n)·eps (NumPy 2's default) when that is larger. */
    double smax = S->data[0];
    double rcond = (double)(m > n ? m : n) * DBL_EPSILON;
    if (rcond < 1e-15) {
        rcond = 1e-15;
    }
    double tol = smax * rcond;

    Matrix* result = matrix_create(n, m);
    if (!result) {
        matrix_deallocate(U);
        matrix_deallocate(S);
        matrix_deallocate(V);
        return NULL;
    }

    /* result[i][j] = sum_l V[i][l] * (1/sigma_l) * U[j][l]  (sigma_l > tol) */
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            double acc = 0.0;
            for (size_t l = 0; l < n; ++l) {
                if (S->data[l] > tol) {
                    acc += V->data[i * n + l] * (1.0 / S->data[l]) * U->data[j * n + l];
                }
            }
            result->data[i * m + j] = acc;
        }
    }

    matrix_deallocate(U);
    matrix_deallocate(S);
    matrix_deallocate(V);
    MATRIX_LOG("[matrix_pinv] Success: pseudo-inverse computed.");
    return result;
}

/**
 * @brief Computes the 2-norm condition number of a matrix.
 *
 * The C analogue of NumPy's `numpy.linalg.cond(M)` with the default 2-norm: the
 * ratio of the largest to the smallest singular value, `σ_max / σ_min`. Works for
 * any shape (the singular values of `A` and `Aᵀ` are identical, so a wide matrix
 * is transposed first).
 *
 * @param matrix The matrix to inspect. Must not be NULL.
 *
 * @return The condition number (`+inf` for a singular matrix), or `NaN` if
 *         `matrix` is NULL or the SVD fails.
 */
double matrix_cond(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cond] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_cond] Error: Invalid matrix provided.");
        return NAN;
    }

    const Matrix* target = matrix;
    Matrix* transposed = NULL;
    if (matrix->rows < matrix->cols) {
        transposed = matrix_transpose(matrix);
        if (!transposed) {
            return NAN;
        }
        target = transposed;
    }

    Matrix* U = NULL;
    Matrix* S = NULL;
    Matrix* V = NULL;
    if (!matrix_svd(target, &U, &S, &V)) {
        if (transposed) {
            matrix_deallocate(transposed);
        }
        return NAN;
    }

    double smax = S->data[0];
    double smin = S->data[0];
    for (size_t i = 1; i < S->rows; ++i) {
        if (S->data[i] > smax) {
            smax = S->data[i];
        }
        if (S->data[i] < smin) {
            smin = S->data[i];
        }
    }

    matrix_deallocate(U);
    matrix_deallocate(S);
    matrix_deallocate(V);
    if (transposed) {
        matrix_deallocate(transposed);
    }

    double cond = smax / smin;   /* smin == 0 -> +inf, like NumPy */
    MATRIX_LOG("[matrix_cond] cond = %f", cond);
    return cond;
}

/**
 * @brief Solves the linear least-squares problem `min ||A·x - b||`.
 *
 * The C analogue of NumPy's `numpy.linalg.lstsq(A, b)[0]` for an overdetermined
 * or square system: finds the `x` minimising the Euclidean norm of the residual
 * `A·x - b`, using the QR decomposition (`A = QR`, then back-substitute
 * `R·x = Qᵀ·b`). `A` must have at least as many rows as columns and full column
 * rank. `b` may have multiple right-hand-side columns.
 *
 * @param A The `m x n` coefficient matrix (`m >= n`). Must not be NULL.
 * @param b The `m x k` right-hand side(s). Must not be NULL and must have the
 *          same number of rows as `A`.
 *
 * @return A new `n x k` least-squares solution, or `NULL` if either argument is
 *         NULL, the row counts differ, `A` has fewer rows than columns, `A` is
 *         rank-deficient, the QR decomposition fails, or allocation fails. The caller owns the result
 *         and must release it with `matrix_deallocate`.
 *
 * @note For the minimum-norm solution of an underdetermined or rank-deficient
 *       system, use `matrix_pinv` (`x = pinv(A)·b`).
 */
Matrix* matrix_lstsq(const Matrix* A, const Matrix* b) {
    MATRIX_LOG("[matrix_lstsq] Entering function");

    if (!A || !b || !A->data || !b->data) {
        MATRIX_LOG("[matrix_lstsq] Error: NULL argument provided.");
        return NULL;
    }
    if (A->rows != b->rows) {
        MATRIX_LOG("[matrix_lstsq] Error: row counts of A and b differ.");
        return NULL;
    }
    if (A->rows < A->cols) {
        MATRIX_LOG("[matrix_lstsq] Error: requires rows >= cols (overdetermined or square).");
        return NULL;
    }

    Matrix* Q = NULL;
    Matrix* R = NULL;
    if (!matrix_qr_decomposition(A, &Q, &R)) {
        MATRIX_LOG("[matrix_lstsq] Error: QR decomposition failed.");
        return NULL;
    }

    /* Back-substitution needs every R(i,i) to be clearly non-zero; for a
       rank-deficient A it would divide by ~0 and return inf/NaN. */
    {
        size_t nn = A->cols;
        double rmax = 0.0;
        for (size_t i = 0; i < nn; ++i) {
            if (fabs(R->data[i * nn + i]) > rmax) {
                rmax = fabs(R->data[i * nn + i]);
            }
        }
        double rtol = rmax * (double)A->rows * DBL_EPSILON;
        for (size_t i = 0; i < nn; ++i) {
            if (!(fabs(R->data[i * nn + i]) > rtol)) {
                MATRIX_LOG("[matrix_lstsq] Error: A is rank-deficient; use matrix_pinv.");
                matrix_deallocate(Q);
                matrix_deallocate(R);
                return NULL;
            }
        }
    }

    Matrix* Qt = matrix_transpose(Q);
    Matrix* C = Qt ? matrix_multiply(Qt, b) : NULL;   /* Qᵀb : n x k */
    if (Qt) {
        matrix_deallocate(Qt);
    }
    if (!C) {
        matrix_deallocate(Q);
        matrix_deallocate(R);
        return NULL;
    }

    size_t n = A->cols;
    size_t k = b->cols;
    Matrix* X = matrix_create(n, k);
    if (!X) {
        matrix_deallocate(Q);
        matrix_deallocate(R);
        matrix_deallocate(C);
        return NULL;
    }

    /* Back-substitution for the upper-triangular system R·X = C. */
    for (size_t col = 0; col < k; ++col) {
        for (size_t ii = 0; ii < n; ++ii) {
            size_t i = n - 1 - ii;
            double s = C->data[i * k + col];
            for (size_t j = i + 1; j < n; ++j) {
                s -= R->data[i * n + j] * X->data[j * k + col];
            }
            X->data[i * k + col] = s / R->data[i * n + i];
        }
    }

    matrix_deallocate(Q);
    matrix_deallocate(R);
    matrix_deallocate(C);
    MATRIX_LOG("[matrix_lstsq] Success: least-squares solution computed.");
    return X;
}

/**
 * @brief Checks if a matrix is symmetric.
 *
 * A matrix is symmetric if it is equal to its transpose, meaning the element at position 
 * (i, j) is equal to the element at position (j, i) for all i, j. This function verifies 
 * the symmetry by comparing the elements above the diagonal with those below the diagonal.
 *
 * @param matrix The matrix to check.
 * @return true if the matrix is symmetric, false otherwise.
 */
bool matrix_is_symmetric(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_symmetric] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_symmetric] Error: Matrix object is null.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_symmetric] Error: Matrix is not square.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = i + 1; j < matrix->cols; j++) { 
            if (matrix->data[i * matrix->cols + j] != matrix->data[j * matrix->cols + i]) {
                MATRIX_LOG("[matrix_is_symmetric] Matrix is not symmetric at (%zu, %zu).", i, j);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_symmetric] The matrix is symmetric.");
    return true;
}


/**
 * @brief Checks if a matrix is upper triangular.
 *
 * A matrix is upper triangular if all the elements below the main diagonal are zero. 
 * This function checks each element below the main diagonal and returns false if 
 * any of these elements are non-zero.
 *
 * @param matrix The matrix to check.
 * @return true if the matrix is upper triangular, false otherwise.
 */
bool matrix_is_upper_triangular(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_upper_triangular] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_upper_triangular] Error: Matrix object is null.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_upper_triangular] Error: Matrix is not square.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < i; j++) { // Check below the diagonal
            if (matrix->data[i * matrix->cols + j] != 0) {
                MATRIX_LOG("[matrix_is_upper_triangular] Non-zero element found below the diagonal at (%zu, %zu).", i, j);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_upper_triangular] The matrix is upper triangular.");
    return true;
}

/**
 * @brief Checks if a matrix is lower triangular.
 *
 * A matrix is lower triangular if all the elements above the main diagonal are zero. 
 * This function checks each element above the main diagonal and returns false if 
 * any of these elements are non-zero.
 *
 * @param matrix The matrix to check.
 * @return true if the matrix is lower triangular, false otherwise.
 */
bool matrix_is_lower_triangular(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_lower_triangular] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_lower_triangular] Error: Matrix object is null.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_lower_triangular] Error: Matrix is not square.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = i + 1; j < matrix->cols; j++) {
            if (matrix->data[i * matrix->cols + j] != 0) {
                MATRIX_LOG("[matrix_is_lower_triangular] Non-zero element found above the diagonal at (%zu, %zu).", i, j);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_lower_triangular] The matrix is lower triangular.");
    return true;
}

/**
 * @brief Checks if a matrix is skew-symmetric.
 *
 * A matrix is skew-symmetric if it is equal to the negative of its transpose, meaning 
 * the element at position (i, j) is equal to the negative of the element at position (j, i) 
 * for all i, j, and the diagonal elements are all zero. This function verifies the 
 * skew-symmetry by checking these conditions.
 *
 * @param matrix The matrix to check.
 * @return true if the matrix is skew-symmetric, false otherwise.
 */
bool matrix_is_skew_symmetric(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_skew_symmetric] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_skew_symmetric] Error: Matrix object is null.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_skew_symmetric] Error: Matrix is not square.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        if (matrix->data[i * matrix->cols + i] != 0) {
            MATRIX_LOG("[matrix_is_skew_symmetric] Diagonal element at (%zu, %zu) is not zero.", i, i);
            return false;
        }

        for (size_t j = i + 1; j < matrix->cols; j++) {
            if (matrix->data[i * matrix->cols + j] != -matrix->data[j * matrix->cols + i]) {
                MATRIX_LOG("[matrix_is_skew_symmetric] Matrix is not skew-symmetric at element (%zu, %zu).", i, j);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_skew_symmetric] The matrix is skew-symmetric.");
    return true;
}

/**
 * @brief Calculates the determinant of a square matrix.
 *
 * For 1x1, 2x2 and 3x3 matrices it uses the direct formulas; larger matrices are
 * reduced by Gaussian elimination with partial pivoting (LU), O(n^3), and the
 * determinant is the signed product of the pivots.
 *
 * @param matrix The input square matrix.
 * @return The determinant of the matrix. Returns 0 if the matrix is not square or an error occurs.
 */
double matrix_determinant(const Matrix* matrix) {
    MATRIX_LOG("[matrix_determinant] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_determinant] Error: Matrix is NULL.");
        return 0.0;
    }
    if (matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_determinant] Error: Determinant can only be calculated for square matrices.");
        return 0.0;
    }
    if (matrix->rows == 1) {
        MATRIX_LOG("[matrix_determinant] Determinant of 1x1 matrix is %lf.", matrix->data[0]);
        return matrix->data[0];
    } 
    else if (matrix->rows == 2) {
        double det = matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
        MATRIX_LOG("[matrix_determinant] Determinant of 2x2 matrix is %lf.", det);
        return det;
    }
    else if (matrix->rows == 3) {
        /* Closed-form expansion: exact for small integer matrices (a singular
           integer matrix gives exactly 0, not rounding noise). */
        const double* m = matrix->data;
        double det = m[0] * (m[4] * m[8] - m[5] * m[7])
                   - m[1] * (m[3] * m[8] - m[5] * m[6])
                   + m[2] * (m[3] * m[7] - m[4] * m[6]);
        MATRIX_LOG("[matrix_determinant] Determinant of 3x3 matrix is %lf.", det);
        return det;
    }
    else {
        /* Gaussian elimination with partial pivoting, O(n^3). (Laplace
           expansion was O(n!) and took seconds already at n = 11.) */
        size_t n = matrix->rows;
        double* a = (double*)malloc(n * n * sizeof(double));
        if (!a) {
            MATRIX_LOG("[matrix_determinant] Error: Memory allocation failed.");
            return 0.0;
        }
        memcpy(a, matrix->data, n * n * sizeof(double));

        double det = 1.0;
        for (size_t k = 0; k < n; k++) {
            size_t piv = k;
            double best = fabs(a[k * n + k]);
            for (size_t i = k + 1; i < n; i++) {
                if (fabs(a[i * n + k]) > best) {
                    best = fabs(a[i * n + k]);
                    piv = i;
                }
            }
            if (best == 0.0) {
                free(a);
                MATRIX_LOG("[matrix_determinant] Matrix is singular, determinant is 0.");
                return 0.0;
            }
            if (piv != k) {
                for (size_t j = 0; j < n; j++) {
                    double t = a[k * n + j];
                    a[k * n + j] = a[piv * n + j];
                    a[piv * n + j] = t;
                }
                det = -det;
            }
            double pk = a[k * n + k];
            det *= pk;
            for (size_t i = k + 1; i < n; i++) {
                double f = a[i * n + k] / pk;
                if (f != 0.0) {
                    for (size_t j = k + 1; j < n; j++) {
                        a[i * n + j] -= f * a[k * n + j];
                    }
                }
            }
        }
        free(a);

        MATRIX_LOG("[matrix_determinant] Determinant of matrix is %lf.", det);
        return det;
    }
}

/**
 * @brief Calculates the trace of a square matrix.
 *
 * The trace of a matrix is defined as the sum of the elements on its main diagonal.
 * This function calculates the trace by iterating over the diagonal elements of the matrix.
 *
 * @param matrix The input square matrix.
 * @return The trace of the matrix. Returns 0.0 if the matrix is not square or if an error occurs.
 */
double matrix_trace(const Matrix* matrix) {
    MATRIX_LOG("[matrix_trace] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_trace] Error: Matrix object is null.");
        return 0.0;
    }
    if (matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_trace] Error: Matrix is not square.");
        return 0.0;
    }

    double trace = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        trace += matrix->data[i * matrix->cols + i];
        MATRIX_LOG("[matrix_trace] Added diagonal element (%zu, %zu): %lf", i, i, matrix->data[i * matrix->cols + i]);
    }

    MATRIX_LOG("[matrix_trace] Success: Trace calculated as %lf.", trace);
    return trace;
}

/**
 * @brief Creates a submatrix by excluding a specified row and column.
 *
 * This function generates a submatrix from the input matrix by removing the specified 
 * row and column. The resulting submatrix has one fewer row and one fewer column 
 * than the original matrix.
 *
 * @param matrix The input matrix.
 * @param excludeRow The index of the row to exclude.
 * @param excludeCol The index of the column to exclude.
 * 
 * @return A pointer to the newly created submatrix, or NULL if an error occurs 
 * (e.g., invalid indices or memory allocation failure).
 */
Matrix* matrix_create_submatrix(const Matrix* matrix, size_t excludeRow, size_t excludeCol) {
    MATRIX_LOG("[matrix_create_submatrix] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_create_submatrix] Error: Input matrix is null.");
        return NULL;
    }
    if (excludeRow >= matrix->rows || excludeCol >= matrix->cols) {
        MATRIX_LOG("[matrix_create_submatrix] Error: excludeRow or excludeCol out of bounds.");
        return NULL;
    }

    Matrix* submatrix = matrix_create(matrix->rows - 1, matrix->cols - 1);
    if (!submatrix) {
        MATRIX_LOG("[matrix_create_submatrix] Error: Memory allocation failed for submatrix.");
        return NULL;
    }

    for (size_t i = 0, sub_i = 0; i < matrix->rows; i++) {
        if (i == excludeRow) {
            continue;
        }
        for (size_t j = 0, sub_j = 0; j < matrix->cols; j++) {
            if (j == excludeCol) {
                continue;
            }
            double value = matrix_get(matrix, i, j);
            matrix_set(submatrix, sub_i, sub_j, value);
            MATRIX_LOG("[matrix_create_submatrix] Set submatrix(%zu, %zu) = %lf", sub_i, sub_j, value);
            sub_j++;
        }
        sub_i++;
    }

    MATRIX_LOG("[matrix_create_submatrix] Success: Submatrix created successfully.");
    return submatrix;
}

/**
 * @brief Computes the adjugate (also known as adjoint) of a square matrix.
 *
 * The adjugate of a matrix is the transpose of its cofactor matrix. This function
 * first computes the cofactor matrix by calculating the determinant of submatrices,
 * then transposes the cofactor matrix to produce the adjugate matrix.
 *
 * @param matrix The input square matrix.
 * @return A pointer to the newly created adjugate matrix, or NULL if an error occurs 
 * (e.g., if the input is not square or memory allocation fails).
 */
Matrix* matrix_adjugate(const Matrix* matrix) {
    MATRIX_LOG("[matrix_adjugate] Entering function");

    if (!matrix || !matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_adjugate] Error: Invalid input matrix (null or not square).");
        return NULL;
    }

    /* adj([[a]]) = [[1]]: the only minor is the empty determinant, 1. The loop
       below cannot express it (a 1x1 matrix has no 0x0 submatrix), which made
       inverse([[a]]) come out as [0]. */
    if (matrix->rows == 1) {
        Matrix* one = matrix_create(1, 1);
        if (one) {
            one->data[0] = 1.0;
        }
        return one;
    }

    Matrix* cofactorMatrix = matrix_create(matrix->rows, matrix->cols);
    if (!cofactorMatrix) {
        MATRIX_LOG("[matrix_adjugate] Error: Memory allocation failed for cofactorMatrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            Matrix* submatrix = matrix_create_submatrix(matrix, i, j);
            if (!submatrix) {
                MATRIX_LOG("[matrix_adjugate] Error: Failed to create submatrix.");
                matrix_deallocate(cofactorMatrix);
                return NULL;
            }
            double minor = matrix_determinant(submatrix);
            double cofactor = ((i + j) % 2 == 0 ? 1.0 : -1.0) * minor;

            matrix_set(cofactorMatrix, i, j, cofactor);
            MATRIX_LOG("[matrix_adjugate] Set cofactorMatrix(%zu, %zu) = %lf", i, j, cofactor);
            matrix_deallocate(submatrix);
        }
    }

    Matrix* adjugate = matrix_transpose(cofactorMatrix);
    matrix_deallocate(cofactorMatrix);

    MATRIX_LOG("[matrix_adjugate] Success: Adjugate matrix created.");
    return adjugate;
}

/**
 * @brief Computes the inverse of a square matrix.
 *
 * Uses Gauss-Jordan elimination with partial pivoting, O(n^3). The matrix is
 * treated as singular when a pivot is no larger than `n * DBL_EPSILON` times its
 * largest entry, so the test does not depend on the matrix's scale.
 *
 * @param matrix The input square matrix.
 * @return A pointer to the newly created inverse matrix, or NULL if the matrix is singular or if an error occurs.
 */
Matrix* matrix_inverse(const Matrix* matrix) {
    MATRIX_LOG("[matrix_inverse] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_inverse] Error: Input matrix is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_inverse] Error: Input matrix is not square.");
        return NULL;
    }

    /* Gauss-Jordan elimination with partial pivoting, O(n^3). The old
       determinant/adjugate route was O(n * n!), returned [0] for every 1x1
       matrix, and called anything with |det| < 1e-9 singular, which rejected
       perfectly invertible small-scale matrices such as 0.01 * I(5). A
       matrix is now singular when a pivot is at the rounding-noise level of
       its own entries (n * eps * max|a_ij|). */
    size_t n = matrix->rows;
    double* a = (double*)malloc(n * n * sizeof(double));
    Matrix* inverse = matrix_create_identity(n);
    if (!a || !inverse) {
        MATRIX_LOG("[matrix_inverse] Error: Memory allocation failed.");
        free(a);
        if (inverse) {
            matrix_deallocate(inverse);
        }
        return NULL;
    }
    memcpy(a, matrix->data, n * n * sizeof(double));
    double* b = inverse->data;

    double scale = 0.0;
    for (size_t i = 0; i < n * n; i++) {
        if (fabs(a[i]) > scale) {
            scale = fabs(a[i]);
        }
    }
    double tol = (double)n * DBL_EPSILON * scale;

    for (size_t k = 0; k < n; k++) {
        size_t piv = k;
        double best = fabs(a[k * n + k]);
        for (size_t i = k + 1; i < n; i++) {
            if (fabs(a[i * n + k]) > best) {
                best = fabs(a[i * n + k]);
                piv = i;
            }
        }
        if (!(best > tol)) {   /* also catches NaN */
            MATRIX_LOG("[matrix_inverse] Error: Matrix is singular and cannot be inverted.");
            free(a);
            matrix_deallocate(inverse);
            return NULL;
        }
        if (piv != k) {
            for (size_t j = 0; j < n; j++) {
                double t = a[k * n + j];
                a[k * n + j] = a[piv * n + j];
                a[piv * n + j] = t;
                t = b[k * n + j];
                b[k * n + j] = b[piv * n + j];
                b[piv * n + j] = t;
            }
        }
        double pk = a[k * n + k];
        for (size_t j = 0; j < n; j++) {
            a[k * n + j] /= pk;
            b[k * n + j] /= pk;
        }
        for (size_t i = 0; i < n; i++) {
            if (i == k) {
                continue;
            }
            double f = a[i * n + k];
            if (f != 0.0) {
                for (size_t j = 0; j < n; j++) {
                    a[i * n + j] -= f * a[k * n + j];
                    b[i * n + j] -= f * b[k * n + j];
                }
            }
        }
    }
    free(a);

    MATRIX_LOG("[matrix_inverse] Success: Matrix inversion completed successfully.");
    return inverse;
}

/**
 * @brief Creates a deep copy of the given matrix.
 *
 * This function allocates a new matrix and copies all the data from the input matrix to the new matrix.
 * If the input matrix is NULL, the function returns NULL.
 *
 * @param matrix The matrix to copy.
 * @return A pointer to the newly created matrix that is a copy of the input matrix, or NULL if an error occurs.
 */
Matrix* matrix_copy(const Matrix* matrix) {
    MATRIX_LOG("[matrix_copy] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_copy] Error: Input matrix is null.");
        return NULL;
    }

    Matrix* copy = matrix_create(matrix->rows, matrix->cols);
    if (!copy) {
        MATRIX_LOG("[matrix_copy] Error: Memory allocation failed for matrix copy.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            copy->data[i * matrix->cols + j] = matrix->data[i * matrix->cols + j];
        }
    }

    MATRIX_LOG("[matrix_copy] Success: Matrix copied successfully.");
    return copy;
}

/**
 * @brief Raises a square matrix to a given non-negative integer power.
 *
 * This function computes the matrix raised to the specified power using an efficient 
 * exponentiation method. If the power is 0, the function returns the identity matrix.
 *
 * @param matrix The input matrix, which must be square.
 * @param power The non-negative integer power to which the matrix is raised.
 * 
 * @return A pointer to the resulting matrix, or NULL if an error occurs 
 * (e.g., the matrix is not square or the power is negative).
 */
Matrix* matrix_power(const Matrix* matrix, int power) {
    MATRIX_LOG("[matrix_power] Entering function with power = %d", power);

    if (!matrix) {
        MATRIX_LOG("[matrix_power] Error: Matrix object is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_power] Error: Matrix is not square.");
        return NULL;
    }
    if (power == 0) {
        MATRIX_LOG("[matrix_power] Power is zero, returning the identity matrix.");
        return matrix_create_identity(matrix->rows);
    }

    // The previous exponentiation-by-squaring was incorrect: it conflated
    // "result" with "running base", so matrix_power(A, 3) returned A^4 and
    // similar off-by-one errors for other exponents.
    //
    // Correct binary exp-by-squaring needs a separate `base` that gets
    // squared each iteration, and a `result` that accumulates base-products
    // whenever the corresponding bit of `power` is set:
    //
    //     result = I
    //     base   = A
    //     while power > 0:
    //         if (power & 1): result *= base
    //         power >>= 1
    //         if (power):     base *= base
    Matrix* result = matrix_create_identity(matrix->rows);
    if (!result) {
        MATRIX_LOG("[matrix_power] Error: Failed to allocate identity result.");
        return NULL;
    }
    /* A^-n = (A^-1)^n, as documented (NULL when A is singular). */
    Matrix* base = (power < 0) ? matrix_inverse(matrix) : matrix_copy(matrix);
    if (!base) {
        MATRIX_LOG("[matrix_power] Error: Failed to build the base (copy, or inverse for a negative power).");
        matrix_deallocate(result);
        return NULL;
    }

    unsigned int p = (power < 0) ? (unsigned int)(-(long long)power) : (unsigned int)power;
    while (p > 0) {
        if (p & 1u) {
            Matrix* t = matrix_multiply(result, base);
            if (!t) {
                MATRIX_LOG("[matrix_power] Error: Matrix multiplication failed.");
                matrix_deallocate(result);
                matrix_deallocate(base);
                return NULL;
            }
            matrix_deallocate(result);
            result = t;
        }
        p >>= 1;
        if (p) {
            Matrix* t = matrix_multiply(base, base);
            if (!t) {
                MATRIX_LOG("[matrix_power] Error: base square failed.");
                matrix_deallocate(result);
                matrix_deallocate(base);
                return NULL;
            }
            matrix_deallocate(base);
            base = t;
        }
    }
    matrix_deallocate(base);

    MATRIX_LOG("[matrix_power] Success: Matrix raised to power successfully.");
    return result;
}

/**
 * @brief Computes the rank of a matrix.
 *
 * The rank of a matrix is the maximum number of linearly independent row or column vectors in the matrix.
 * Like `numpy.linalg.matrix_rank`, it counts the singular values greater than
 * `σmax · max(rows, cols) · DBL_EPSILON`.
 *
 * @param matrix The input matrix.
 * @return The rank of the matrix, or -1 if an error occurs (e.g., the input matrix is NULL).
 */
int matrix_rank(const Matrix* matrix) {
    MATRIX_LOG("[matrix_rank] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_rank] Error: Input matrix is null.");
        return -1;
    }

    /* NumPy's definition: the number of singular values above
       σmax · max(m, n) · eps. (The old elimination indexed the diagonal past
       the last row of a wide matrix and could report rank > min(m, n).) */
    const Matrix* target = matrix;
    Matrix* transposed = NULL;
    if (matrix->rows < matrix->cols) {
        transposed = matrix_transpose(matrix);
        if (!transposed) {
            return -1;
        }
        target = transposed;
    }

    Matrix* U = NULL;
    Matrix* S = NULL;
    Matrix* V = NULL;
    bool ok = matrix_svd(target, &U, &S, &V);
    if (transposed) {
        matrix_deallocate(transposed);
    }
    if (!ok) {
        MATRIX_LOG("[matrix_rank] Error: SVD failed.");
        return -1;
    }

    size_t big = matrix->rows > matrix->cols ? matrix->rows : matrix->cols;
    double tol = S->data[0] * (double)big * DBL_EPSILON;
    int rank = 0;
    for (size_t i = 0; i < S->rows; i++) {
        if (S->data[i] > tol) {
            rank++;
        }
    }

    matrix_deallocate(U);
    matrix_deallocate(S);
    matrix_deallocate(V);
    MATRIX_LOG("[matrix_rank] Rank calculation completed. Final rank = %d", rank);
    return rank;
}


/**
 * @brief Checks if a matrix is diagonal.
 *
 * A matrix is considered diagonal if all its off-diagonal elements are zero. 
 * This function checks whether the input matrix meets this criterion.
 *
 * @param matrix The matrix to check.
 * @return `true` if the matrix is diagonal, `false` otherwise.
 */
bool matrix_is_diagonal(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_diagonal] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_is_diagonal] Error: Matrix object is null.");
        return false;
    }

    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_diagonal] Error: Matrix is not square.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            if (i != j && !is_effectively_zero(matrix->data[i * matrix->cols + j])) {
                MATRIX_LOG("[matrix_is_diagonal] Non-zero element found at (%zu, %zu) outside the main diagonal.", i, j);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_diagonal] The matrix is diagonal.");
    return true;
}

/**
 * @brief Checks if a matrix is orthogonal.
 *
 * A matrix is orthogonal if its transpose is equal to its inverse, or equivalently, if the matrix multiplied 
 * by its transpose yields the identity matrix. This function checks whether the input matrix is orthogonal.
 *
 * @param matrix The matrix to check.
 * @return `true` if the matrix is orthogonal, `false` otherwise.
 */
bool matrix_is_orthogonal(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_orthogonal] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_orthogonal] Error: Matrix object is null.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_is_orthogonal] Error: Matrix is not square.");
        return false;
    }

    Matrix* transpose = matrix_transpose(matrix);
    if (!transpose) {
        MATRIX_LOG("[matrix_is_orthogonal] Error: Failed to compute the transpose.");
        return false;
    }

    Matrix* product = matrix_multiply(matrix, transpose);
    if (!product) {
        MATRIX_LOG("[matrix_is_orthogonal] Error: Failed to multiply matrix by its transpose.");
        matrix_deallocate(transpose);
        return false;
    }

    bool isOrthogonal = matrix_is_identity(product);
    matrix_deallocate(transpose);
    matrix_deallocate(product);

    if (isOrthogonal) {
        MATRIX_LOG("[matrix_is_orthogonal] The matrix is orthogonal.");
    } 
    else {
        MATRIX_LOG("[matrix_is_orthogonal] The matrix is not orthogonal.");
    }

    return isOrthogonal;
}


/**
 * @brief Computes the Kronecker product of two matrices.
 *
 * The Kronecker product is a block matrix formed by multiplying each element of the first matrix 
 * by the entire second matrix. If matrix1 is of size m x n and matrix2 is of size p x q, 
 * the resulting matrix will be of size (m * p) x (n * q).
 *
 * @param matrix1 The first input matrix.
 * @param matrix2 The second input matrix.
 * 
 * @return A pointer to the resulting matrix, or NULL if an error occurs (invalid input or memory allocation failure).
 */
Matrix* matrix_kronecker_product(const Matrix* matrix1, const Matrix* matrix2) {
    MATRIX_LOG("[matrix_kronecker_product] Entering function");
    if (!matrix1 || !matrix2) {
        MATRIX_LOG("[matrix_kronecker_product] Error: One or both matrices are null.");
        return NULL;
    }

    size_t m = matrix1->rows, n = matrix1->cols, p = matrix2->rows, q = matrix2->cols;
    size_t mp, nq;
    if (!mat_size_mul(m, p, &mp) || !mat_size_mul(n, q, &nq)) {
        MATRIX_LOG("[matrix_kronecker_product] Error: result size overflows.");
        return NULL;
    }
    Matrix* product = matrix_create(mp, nq);
    if (!product) {
        MATRIX_LOG("[matrix_kronecker_product] Error: Memory allocation failed for the result matrix.");
        return NULL;
    }

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < p; ++k) {
                for (size_t l = 0; l < q; ++l) {
                    double a = matrix_get(matrix1, i, j);
                    double b = matrix_get(matrix2, k, l);

                    matrix_set(product, i * p + k, j * q + l, a * b);
                    MATRIX_LOG("[matrix_kronecker_product] Set product(%zu, %zu) = %lf", i * p + k, j * q + l, a * b);
                }
            }
        }
    }

    MATRIX_LOG("[matrix_kronecker_product] Success: Kronecker product computed successfully.");
    return product;
}

/**
 * @brief Generates a Hankel matrix from the first row and last column vectors.
 *
 * A Hankel matrix is a square matrix in which each ascending skew-diagonal from left to right is constant. 
 * This function generates such a matrix using the specified first row and last column vectors.
 *
 * @param firstRow A matrix containing the first row of the Hankel matrix (must be a row vector).
 * @param lastCol A matrix containing the last column of the Hankel matrix (must be a column vector).
 * 
 * @return A pointer to the newly created Hankel matrix, or NULL if an error occurs (invalid input or memory allocation failure).
 */
Matrix* matrix_hankel(const Matrix* firstRow, const Matrix* lastCol) {
    MATRIX_LOG("[matrix_hankel] Entering function");

    if (!firstRow || !lastCol || firstRow->rows != 1 || lastCol->cols != 1) {
        MATRIX_LOG("[matrix_hankel] Error: Invalid input matrices (must be a row vector and a column vector).");
        return NULL;
    }

    size_t n = firstRow->cols;
    if (lastCol->rows != n) {
        MATRIX_LOG("[matrix_hankel] Error: First row and last column dimensions are incompatible.");
        return NULL;
    }

    Matrix* hankel = matrix_create(n, n);
    if (!hankel) {
        MATRIX_LOG("[matrix_hankel] Error: Memory allocation failed for Hankel matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            double value;
            if (i + j < n) {
                value = matrix_get(firstRow, 0, i + j);
            } 
            else {
                value = matrix_get(lastCol, i + j - n + 1, 0);
            }

            matrix_set(hankel, i, j, value);
            MATRIX_LOG("[matrix_hankel] Set hankel(%zu, %zu) = %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_hankel] Success: Hankel matrix generated successfully.");
    return hankel;
}

/**
 * @brief Checks if a matrix is a Hankel matrix.
 *
 * A Hankel matrix is a matrix in which each ascending anti-diagonal from left to right is constant.
 * This function verifies whether the input matrix satisfies this property.
 *
 * @param matrix The input matrix to check.
 * @return `true` if the matrix is a Hankel matrix, `false` otherwise.
 */ 
bool matrix_is_hankel(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_hankel] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_is_hankel] Error: Matrix object is null.");
        return false;
    }

    /* Every element must equal its up-right neighbour: a[i][j] == a[i-1][j+1].
       That covers each anti-diagonal completely (the old loop skipped all
       anti-diagonals from the main one down). */
    for (size_t i = 1; i < matrix->rows; i++) {
        for (size_t j = 0; j + 1 < matrix->cols; j++) {
            double value = matrix->data[i * matrix->cols + j];
            double up_right = matrix->data[(i - 1) * matrix->cols + j + 1];
            if (!is_effectively_zero(value - up_right)) {
                MATRIX_LOG("[matrix_is_hankel] Error: Matrix is not Hankel at element [%zu, %zu].", i, j);
                return false;
            }
        }
    }

    MATRIX_LOG("[matrix_is_hankel] Matrix is Hankel.");
    return true;
}

/**
 * @brief Creates a Toeplitz matrix given its first row and first column.
 *
 * A Toeplitz matrix is a matrix in which each descending diagonal from left to right is constant.
 * This function generates such a matrix using the specified first row and first column vectors.
 *
 * @param firstRow A matrix containing the first row of the Toeplitz matrix (must be a row vector).
 * @param firstCol A matrix containing the first column of the Toeplitz matrix (must be a column vector).
 * 
 * @return A pointer to the newly created Toeplitz matrix, or NULL if an error occurs 
 * (invalid input or memory allocation failure).
 */
Matrix* matrix_toeplitz(const Matrix* firstRow, const Matrix* firstCol) {
    MATRIX_LOG("[matrix_toeplitz] Entering function");

    if (!firstRow || !firstCol) {
        MATRIX_LOG("[matrix_toeplitz] Error: Input matrices (firstRow or firstCol) are null.");
        return NULL;
    }
    if (firstRow->rows != 1) {
        MATRIX_LOG("[matrix_toeplitz] Error: FirstRow must be a row vector.");
        return NULL;
    }
    if (firstCol->cols != 1) {
        MATRIX_LOG("[matrix_toeplitz] Error: FirstCol must be a column vector.");
        return NULL;
    }

    size_t rows = firstCol->rows;
    size_t cols = firstRow->cols;
    Matrix* toeplitzMatrix = matrix_create(rows, cols);
    if (!toeplitzMatrix) {
        MATRIX_LOG("[matrix_toeplitz] Error: Memory allocation failed for Toeplitz matrix.");
        return NULL;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            double value;
            if (j >= i) {
                value = matrix_get(firstRow, 0, j - i);
            } 
            else {
                value = matrix_get(firstCol, i - j, 0);
            }
            matrix_set(toeplitzMatrix, i, j, value);
            MATRIX_LOG("[matrix_toeplitz] Set toeplitz(%zu, %zu) = %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_toeplitz] Successfully created Toeplitz matrix.");
    return toeplitzMatrix;
}

/**
 * @brief Creates a matrix from a given array of doubles.
 *
 * This function takes a 1D array of doubles and arranges it into a matrix of the specified 
 * number of rows and columns. The array is expected to be in row-major order.
 *
 * @param data The input array of doubles.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * 
 * @return A pointer to the newly created matrix, or NULL if an error occurs (invalid input or memory allocation failure).
 */
Matrix* matrix_from_array(const double* data, size_t rows, size_t cols) {
    MATRIX_LOG("[matrix_from_array] Entering function");

    if (!data) {
        MATRIX_LOG("[matrix_from_array] Error: Input data is null.");
        return NULL;
    }
    if (rows == 0 || cols == 0) {
        MATRIX_LOG("[matrix_from_array] Error: Rows or columns cannot be zero.");
        return NULL;
    }

    Matrix* matrix = matrix_create(rows, cols);
    if (!matrix) {
        MATRIX_LOG("[matrix_from_array] Error: Memory allocation failed for matrix.");
        return NULL;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix->data[i * cols + j] = data[i * cols + j];
            MATRIX_LOG("[matrix_from_array] Set matrix(%zu, %zu) = %lf", i, j, data[i * cols + j]);
        }
    }

    MATRIX_LOG("[matrix_from_array] Successfully created matrix from array.");
    return matrix;
}

/**
 * @brief Checks if a matrix is a Toeplitz matrix.
 *
 * A Toeplitz matrix is a matrix in which each descending diagonal from left to right is constant.
 * This function checks whether the input matrix satisfies this property.
 *
 * @param matrix The input matrix to check.
 * @return `true` if the matrix is Toeplitz, `false` otherwise.
 */
bool matrix_is_toeplitz(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_toeplitz] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_is_toeplitz] Error: Matrix object is null.");
        return false;
    }

    // Check all elements in the first row
    for (size_t i = 0; i < matrix->cols; i++) {
        if (!mat_check_diagonal(matrix, 0, i)) {
            MATRIX_LOG("[matrix_is_toeplitz] Error: Diagonal check failed starting from (0, %zu)", i);
            return false;
        }
    }

    // Check all elements in the first column
    for (size_t i = 1; i < matrix->rows; i++) {
        if (!mat_check_diagonal(matrix, i, 0)) {
            MATRIX_LOG("[matrix_is_toeplitz] Error: Diagonal check failed starting from (%zu, 0)", i);
            return false;
        }
    }

    MATRIX_LOG("[matrix_is_toeplitz] Matrix is Toeplitz.");
    return true;
}

/**
 * @brief Creates a circulant matrix from the first row of a given matrix.
 *
 * A circulant matrix is a special type of Toeplitz matrix where each row vector is a right cyclic 
 * shift of the row above it. This function generates such a matrix from the first row of the input matrix.
 *
 * @param firstRow A matrix containing the first row of the circulant matrix (must be a single-row matrix).
 * @return A pointer to the newly created circulant matrix, or NULL if an error occurs (e.g., invalid input or memory allocation failure).
 */
Matrix* matrix_circulant(const Matrix* firstRow) {
    MATRIX_LOG("[matrix_circulant] Entering function");

    if (!firstRow || firstRow->rows != 1) {
        MATRIX_LOG("[matrix_circulant] Error: Input must be a single-row matrix.");
        return NULL;
    }

    size_t n = firstRow->cols;
    Matrix* circulantMatrix = matrix_create(n, n);
    if (!circulantMatrix) {
        MATRIX_LOG("[matrix_circulant] Error: Memory allocation failed for circulant matrix.");
        return NULL;
    }

    // Populate the circulant matrix
    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {
            /* each row is the previous one shifted RIGHT by one, as
               documented (the old (col + row) % n shifted left) */
            size_t index = (col + n - row) % n;
            double value = matrix_get(firstRow, 0, index);
            matrix_set(circulantMatrix, row, col, value);
            MATRIX_LOG("[matrix_circulant] Set circulant(%zu, %zu) = %lf", row, col, value);
        }
    }

    MATRIX_LOG("[matrix_circulant] Successfully created circulant matrix.");
    return circulantMatrix;
}

/**
 * @brief Creates a Hilbert matrix of the given size.
 *
 * A Hilbert matrix is a square matrix with entries being the unit fractions:
 * H(i, j) = 1 / (i + j - 1), where i and j are the row and column indices, respectively.
 *
 * @param n The size of the Hilbert matrix (must be greater than 0).
 * @return A pointer to the newly created Hilbert matrix, or NULL if an error occurs.
 */
Matrix* matrix_hilbert(size_t n) {
    MATRIX_LOG("[matrix_hilbert] Entering function with n = %zu", n);

    if (n == 0) {
        MATRIX_LOG("[matrix_hilbert] Error: Size must be greater than 0.");
        return NULL;
    }

    Matrix* hilbertMatrix = matrix_create(n, n);
    if (!hilbertMatrix) {
        MATRIX_LOG("[matrix_hilbert] Error: Memory allocation failed for Hilbert matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            double value = 1.0 / ((i + 1) + (j + 1) - 1.0);
            if (!matrix_set(hilbertMatrix, i, j, value)) {
                MATRIX_LOG("[matrix_hilbert] Error: Failed to set value at (%zu, %zu)", i, j);
                matrix_deallocate(hilbertMatrix);
                return NULL;
            }
            MATRIX_LOG("[matrix_hilbert] Set hilbert(%zu, %zu) = %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_hilbert] Successfully created Hilbert matrix.");
    return hilbertMatrix;
}


/**
 * @brief Creates a Helmert matrix of the specified size.
 *
 * A Helmert matrix is an orthogonal matrix used in statistical analysis. This function generates
 * either the full n x n Helmert matrix or the reduced (n-1) x n version, which drops the first
 * (constant) row, exactly as scipy.linalg.helmert does.
 *
 * @param n The size of the Helmert matrix.
 * @param full Boolean value indicating whether to generate the full Helmert matrix (true) or the reduced version (false).
 * 
 * @return A pointer to the newly created Helmert matrix, or NULL if an error occurs.
 */
Matrix* matrix_helmert(size_t n, bool full) {
    MATRIX_LOG("[matrix_helmert] Entering function with n = %zu, full = %s", n, full ? "true" : "false");

    if (n == 0 || (!full && n < 2)) {
        MATRIX_LOG("[matrix_helmert] Error: n must be >= 1 (>= 2 for the reduced form).");
        return NULL;
    }

    /* As scipy.linalg.helmert: the full matrix is n x n; the reduced one drops
       the first row (the constant 1/sqrt(n) row) and is (n-1) x n. */
    size_t rows = full ? n : n - 1;
    Matrix* helmertMatrix = matrix_create(rows, n);
    if (!helmertMatrix) {
        MATRIX_LOG("[matrix_helmert] Error: Memory allocation failed for Helmert matrix.");
        return NULL;
    }

    for (size_t r = 0; r < rows; ++r) {
        size_t i = full ? r : r + 1;   /* row index in the full Helmert matrix */
        for (size_t j = 0; j < n; ++j) {
            double value = 0.0;
            if (i == 0) {
                value = 1.0 / sqrt((double)n);
            }
            else if (j < i) {
                value = 1.0 / sqrt(i * (i + 1.0));
            }
            else if (j == i) {
                value = -sqrt((double)i / (i + 1.0));
            }
            helmertMatrix->data[r * n + j] = value;
            MATRIX_LOG("[matrix_helmert] Set helmert(%zu, %zu) = %lf", r, j, value);
        }
    }

    MATRIX_LOG("[matrix_helmert] Successfully created Helmert matrix.");
    return helmertMatrix;
}

/**
 * @brief Computes the cofactor matrix of a given square matrix.
 *
 * This function calculates the cofactor matrix for a given square matrix. 
 * Each element in the cofactor matrix is determined by the determinant of 
 * the submatrix that remains after removing the corresponding row and column, 
 * multiplied by (-1)^(i+j) to account for the sign.
 *
 * @param matrix The input square matrix.
 * @return A new matrix representing the cofactor matrix, or NULL if an error occurs.
 */
Matrix* matrix_cofactor(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cofactor] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_cofactor] Error: Matrix object is null.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_cofactor] Error: Matrix must be square.");
        return NULL;
    }

    size_t n = matrix->rows;
    Matrix* cofactorMatrix = matrix_create(n, n);
    if (!cofactorMatrix) {
        MATRIX_LOG("[matrix_cofactor] Error: Memory allocation failed for cofactor matrix.");
        return NULL;
    }
    if (n == 1) {
        /* The cofactor of a 1x1 matrix is the empty determinant, 1. */
        cofactorMatrix->data[0] = 1.0;
        return cofactorMatrix;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            MATRIX_LOG("[matrix_cofactor] Creating submatrix by excluding row %zu and column %zu", i, j);
            Matrix* submatrix = matrix_create_submatrix(matrix, i, j);
            if (!submatrix) {
                MATRIX_LOG("[matrix_cofactor] Error: Failed to create submatrix.");
                matrix_deallocate(cofactorMatrix);
                return NULL;
            }

            // Calculate determinant of the submatrix
            double det = matrix_determinant(submatrix);
            matrix_deallocate(submatrix);
            MATRIX_LOG("[matrix_cofactor] Determinant of submatrix = %lf", det);

            double cofactor = ((i + j) % 2 == 0 ? 1 : -1) * det;
            matrix_set(cofactorMatrix, i, j, cofactor);
            MATRIX_LOG("[matrix_cofactor] Set cofactor(%zu, %zu) = %lf", i, j, cofactor);
        }
    }

    MATRIX_LOG("[matrix_cofactor] Successfully created cofactor matrix.");
    return cofactorMatrix;
}

/**
 * @brief Performs the Cholesky decomposition of a positive definite matrix.
 *
 * This function decomposes a positive definite matrix into a lower triangular 
 * matrix and its transpose. If the matrix is not positive definite, the function 
 * returns NULL.
 *
 * @param matrix The input square matrix (must be positive definite).
 * 
 * @return A new matrix representing the Cholesky factor, or NULL if the matrix 
 * is not positive definite or an error occurs.
 */
Matrix* matrix_cholesky_decomposition(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cholesky_decomposition] Entering function");

    if (!matrix || matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_cholesky_decomposition] Error: Input must be a square matrix.");
        return NULL;
    }

    size_t n = matrix->rows;
    Matrix* chol = matrix_create(n, n);
    if (!chol) {
        MATRIX_LOG("[matrix_cholesky_decomposition] Error: Memory allocation failed for Cholesky matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = i; j < n; j++) {
            double sum = matrix_get(matrix, i, j);
            for (size_t k = 0; k < i; k++) {
                sum -= matrix_get(chol, k, i) * matrix_get(chol, k, j);
            }

            if (i == j) {
                if (sum <= 0.0) {
                    MATRIX_LOG("[matrix_cholesky_decomposition] Error: Matrix is not positive definite.");
                    matrix_deallocate(chol);
                    return NULL;
                }
                matrix_set(chol, i, j, sqrt(sum));
                MATRIX_LOG("[matrix_cholesky_decomposition] Set chol(%zu, %zu) = %lf", i, j, sqrt(sum));
            } 
            else {
                matrix_set(chol, i, j, sum / matrix_get(chol, i, i));
                MATRIX_LOG("[matrix_cholesky_decomposition] Set chol(%zu, %zu) = %lf", i, j, sum / matrix_get(chol, i, i));
            }
        }
    }

    // Zero out the lower triangular part
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < i; j++) {
            matrix_set(chol, i, j, 0.0);
            MATRIX_LOG("[matrix_cholesky_decomposition] Zeroed lower triangular chol(%zu, %zu)", i, j);
        }
    }

    MATRIX_LOG("[matrix_cholesky_decomposition] Exiting function.");
    return chol;
}

/**
 * @brief Performs LU decomposition of a square matrix.
 *
 * This function decomposes a given square matrix into two matrices: 
 * a lower triangular matrix `L` and an upper triangular matrix `U`, 
 * such that the original matrix `A` can be represented as `A = L * U`.
 *
 * @param matrix The input square matrix to decompose.
 * @param L Pointer to a pointer that will store the lower triangular matrix.
 * @param U Pointer to a pointer that will store the upper triangular matrix.
 * 
 * @return `true` if the decomposition is successful, `false` otherwise.
 */
bool matrix_lu_decomposition(const Matrix* matrix, Matrix** L, Matrix** U) {
    MATRIX_LOG("[matrix_lu_decomposition] Entering function");

    if (!matrix || !matrix->data || !L || !U || !matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_lu_decomposition] Error: NULL argument, or matrix is not square.");
        return false;
    }

    size_t n = matrix->rows;
    *L = matrix_create(n, n);
    *U = matrix_create(n, n);

    if (!(*L) || !(*U)) {
        MATRIX_LOG("[matrix_lu_decomposition] Error: Memory allocation failed for L or U.");
        if (*L) matrix_deallocate(*L);
        if (*U) matrix_deallocate(*U);
        *L = NULL;
        *U = NULL;
        return false;
    }

    /* Doolittle without pivoting divides by every pivot but the last; one at
       the rounding level of the matrix means this factorization does not
       exist (it used to return true with inf/NaN in L and U). */
    double scale = 0.0;
    for (size_t i = 0; i < n * n; i++) {
        if (fabs(matrix->data[i]) > scale) {
            scale = fabs(matrix->data[i]);
        }
    }
    double tol = (double)n * DBL_EPSILON * scale;

    for (size_t i = 0; i < n; i++) {
        for (size_t k = i; k < n; k++) {
            double sum = 0.0;
            for (size_t j = 0; j < i; j++) {
                sum += matrix_get(*L, i, j) * matrix_get(*U, j, k);
            }
            matrix_set(*U, i, k, matrix_get(matrix, i, k) - sum);
            MATRIX_LOG("[matrix_lu_decomposition] Set U(%zu, %zu) = %lf", i, k, matrix_get(matrix, i, k) - sum);
        }
        if (i + 1 < n && !(fabs(matrix_get(*U, i, i)) > tol)) {
            MATRIX_LOG("[matrix_lu_decomposition] Error: zero pivot at %zu; no LU factorization without pivoting.", i);
            matrix_deallocate(*L);
            matrix_deallocate(*U);
            *L = NULL;
            *U = NULL;
            return false;
        }

        // Lower Triangular
        for (size_t k = i; k < n; k++) {
            if (i == k) {
                matrix_set(*L, i, i, 1.0); 
                MATRIX_LOG("[matrix_lu_decomposition] Set L(%zu, %zu) = 1.0", i, i);
            } 
            else {
                double sum = 0.0;
                for (size_t j = 0; j < i; j++) {
                    sum += matrix_get(*L, k, j) * matrix_get(*U, j, i);
                }
                matrix_set(*L, k, i, (matrix_get(matrix, k, i) - sum) / matrix_get(*U, i, i));
                MATRIX_LOG("[matrix_lu_decomposition] Set L(%zu, %zu) = %lf", k, i, (matrix_get(matrix, k, i) - sum) / matrix_get(*U, i, i));
            }
        }
    }

    MATRIX_LOG("[matrix_lu_decomposition] Exiting function.");
    return true;
}

/**
 * @brief Performs QR decomposition of a matrix.
 *
 * This function decomposes a given matrix into two matrices: 
 * an orthogonal matrix `Q` and an upper triangular matrix `R`, 
 * such that the original matrix `A` can be represented as `A = Q * R`.
 * Computed with Householder reflections: `Q` (m x n) has orthonormal columns
 * even for rank-deficient input, and `R` (n x n) has a non-negative diagonal.
 * Note: The input matrix must have at least as many rows as columns (m >= n).
 *
 * @param A The input matrix to decompose.
 * @param Q Pointer to a pointer that will store the orthogonal matrix.
 * @param R Pointer to a pointer that will store the upper triangular matrix.
 * 
 * @return `true` if the decomposition is successful, `false` otherwise.
 */
bool matrix_qr_decomposition(const Matrix* A, Matrix** Q, Matrix** R) {
    MATRIX_LOG("[matrix_qr_decomposition] Entering function");

    if (!A || !A->data || !Q || !R || A->rows < A->cols) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: NULL argument, or matrix has fewer rows than columns.");
        return false;
    }

    size_t m = A->rows;
    size_t n = A->cols;

    /* Householder QR. (Classical Gram-Schmidt lost orthogonality on
       ill-conditioned input and produced NaN, while reporting success, as
       soon as a column was dependent.) The reflector vectors are kept in V
       (column k uses rows k..m-1) to build the thin Q afterwards. */
    double* a = (double*)malloc(m * n * sizeof(double));
    double* V = (double*)calloc(m * n, sizeof(double));
    double* tau = (double*)calloc(n, sizeof(double));
    *Q = matrix_create(m, n);
    *R = matrix_create(n, n);
    if (!a || !V || !tau || !*Q || !*R) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Memory allocation failed.");
        free(a);
        free(V);
        free(tau);
        if (*Q) {
            matrix_deallocate(*Q);
        }
        if (*R) {
            matrix_deallocate(*R);
        }
        *Q = NULL;
        *R = NULL;
        return false;
    }
    memcpy(a, A->data, m * n * sizeof(double));

    for (size_t k = 0; k < n; ++k) {
        double norm = 0.0;
        for (size_t i = k; i < m; ++i) {
            norm = hypot(norm, a[i * n + k]);
        }
        if (norm == 0.0) {
            tau[k] = 0.0;               /* zero column: no reflection needed */
            continue;
        }
        double x0 = a[k * n + k];
        double alpha = (x0 >= 0.0) ? -norm : norm;   /* avoids cancellation */
        double vnorm2 = 0.0;
        for (size_t i = k; i < m; ++i) {
            double vi = a[i * n + k] - ((i == k) ? alpha : 0.0);
            V[i * n + k] = vi;
            vnorm2 += vi * vi;
        }
        tau[k] = 2.0 / vnorm2;
        for (size_t j = k; j < n; ++j) {
            double s = 0.0;
            for (size_t i = k; i < m; ++i) {
                s += V[i * n + k] * a[i * n + j];
            }
            s *= tau[k];
            for (size_t i = k; i < m; ++i) {
                a[i * n + j] -= s * V[i * n + k];
            }
        }
    }

    /* R is the upper triangle of the reduced matrix. */
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            (*R)->data[i * n + j] = (j >= i) ? a[i * n + j] : 0.0;
        }
    }

    /* Thin Q = H0 · H1 · ... · H(n-1) applied to the first n columns of I. */
    double* q = (*Q)->data;
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            q[i * n + j] = (i == j) ? 1.0 : 0.0;
        }
    }
    for (size_t kk = n; kk-- > 0; ) {
        if (tau[kk] == 0.0) {
            continue;
        }
        for (size_t j = 0; j < n; ++j) {
            double s = 0.0;
            for (size_t i = kk; i < m; ++i) {
                s += V[i * n + kk] * q[i * n + j];
            }
            s *= tau[kk];
            for (size_t i = kk; i < m; ++i) {
                q[i * n + j] -= s * V[i * n + kk];
            }
        }
    }

    /* Make R's diagonal non-negative (flip the matching column of Q), the
       convention Gram-Schmidt produces; for full-rank A this factorization
       is unique. */
    for (size_t i = 0; i < n; ++i) {
        if ((*R)->data[i * n + i] < 0.0) {
            for (size_t j = i; j < n; ++j) {
                (*R)->data[i * n + j] = -(*R)->data[i * n + j];
            }
            for (size_t r = 0; r < m; ++r) {
                q[r * n + i] = -q[r * n + i];
            }
        }
    }

    free(a);
    free(V);
    free(tau);

    MATRIX_LOG("[matrix_qr_decomposition] Exiting function.");
    return true;
}

/**
 * @brief Creates a Pascal matrix of size n x n.
 *
 * This function generates a Pascal matrix, which is a symmetric matrix where each element is a binomial coefficient.
 * The matrix is filled in both the upper and lower triangular parts using the binomial coefficients.
 *
 * @param n The size of the Pascal matrix (number of rows and columns).
 * @return A pointer to the generated Pascal matrix. Returns `NULL` if memory allocation fails.
 */
Matrix* matrix_pascal(size_t n) {
    MATRIX_LOG("[matrix_pascal] Entering function with size %zu", n);

    Matrix* pascalMatrix = matrix_create(n, n);
    if (!pascalMatrix) {
        MATRIX_LOG("[matrix_pascal] Error: Memory allocation failed for Pascal matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j <= i; j++) {
            double value = mat_binomial(i + j, i);   /* exact below 2^53 */
            matrix_set(pascalMatrix, i, j, value);
            matrix_set(pascalMatrix, j, i, value);

            MATRIX_LOG("[matrix_pascal] Set Pascal matrix element (%zu, %zu) = %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_pascal] Successfully created Pascal matrix.");
    return pascalMatrix;
}

/**
 * @brief Computes the Frobenius norm of a matrix.
 *
 * The Frobenius norm is calculated as the square root of the sum of the absolute squares of all elements in the matrix.
 * It provides a measure of the magnitude of the matrix.
 *
 * @param matrix The input matrix for which the Frobenius norm is to be computed.
 * @return The Frobenius norm of the matrix.
 */
double matrix_frobenius_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_frobenius_norm] Entering function");
    if (!matrix || !matrix->data) {
        return 0.0;   /* documented result for NULL */
    }

    double sum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            double value = matrix_get(matrix, i, j);
            sum += value * value;

            MATRIX_LOG("[matrix_frobenius_norm] Adding value^2 for element (%zu, %zu): %lf", i, j, value * value);
        }
    }

    double frobeniusNorm = sqrt(sum);

    MATRIX_LOG("[matrix_frobenius_norm] Frobenius norm = %lf", frobeniusNorm);
    return frobeniusNorm;
}

/**
 * @brief Computes the L1 norm of a matrix.
 *
 * The L1 norm, also known as the maximum column sum norm, is calculated as the maximum of the sums of absolute values of each column.
 *
 * @param matrix The input matrix for which the L1 norm is to be computed.
 * @return The L1 norm of the matrix.
 */
double matrix_l1_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_l1_norm] Entering function");
    if (!matrix || !matrix->data) {
        return 0.0;   /* documented result for NULL */
    }

    double maxSum = 0.0;
    for (size_t j = 0; j < matrix->cols; j++) {
        double columnSum = 0.0;
        for (size_t i = 0; i < matrix->rows; i++) {
            columnSum += fabs(matrix_get(matrix, i, j));
        }
        MATRIX_LOG("[matrix_l1_norm] Column %zu sum = %f", j, columnSum);

        if (columnSum > maxSum) {
            maxSum = columnSum;
            MATRIX_LOG("[matrix_l1_norm] New max column sum = %f", maxSum);
        }
    }

    MATRIX_LOG("[matrix_l1_norm] Exiting function, L1 norm = %f", maxSum);
    return maxSum;
}

/**
 * @brief Computes the infinity norm of a matrix.
 *
 * The infinity norm, also known as the maximum row sum norm, is calculated as the maximum of the sums of absolute values of each row.
 *
 * @param matrix The input matrix for which the infinity norm is to be computed.
 * @return The infinity norm of the matrix.
 */
double matrix_infinity_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_infinity_norm] Entering function");
    if (!matrix || !matrix->data) {
        return 0.0;   /* documented result for NULL */
    }

    double maxSum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        double rowSum = 0.0;
        for (size_t j = 0; j < matrix->cols; j++) {
            rowSum += fabs(matrix_get(matrix, i, j));
        }
        MATRIX_LOG("[matrix_infinity_norm] Row %zu sum = %f", i, rowSum);

        if (rowSum > maxSum) {
            maxSum = rowSum;
            MATRIX_LOG("[matrix_infinity_norm] New max row sum = %f", maxSum);
        }
    }

    MATRIX_LOG("[matrix_infinity_norm] Exiting function, infinity norm = %f", maxSum);
    return maxSum;
}

/**
 * @brief Computes the inverse of a square matrix using the Gauss-Jordan elimination method.
 *
 * This function takes a square matrix and computes its inverse by performing Gauss-Jordan elimination. 
 * If the matrix is not square or is singular (non-invertible), the function returns `NULL`.
 *
 * @param matrix The input square matrix to be inverted.
 * @return A pointer to the inverse matrix. If the matrix is not invertible or an error occurs, `NULL` is returned.
 */
Matrix* matrix_inverse_gauss_jordan(const Matrix* matrix) {
    MATRIX_LOG("[matrix_inverse_gauss_jordan] Entering function");

    if (!matrix || !matrix->data || matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Matrix must be square for inversion.");
        return NULL;
    }

    size_t n = matrix->rows;
    MATRIX_LOG("[matrix_inverse_gauss_jordan] Creating augmented matrix of size %zux%zu", n, 2 * n);

    Matrix* augmented = matrix_create(n, 2 * n);
    if (!augmented) {
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Memory allocation failed for augmented matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            augmented->data[i * 2 * n + j] = matrix->data[i * n + j]; 
            augmented->data[i * 2 * n + j + n] = (i == j) ? 1.0 : 0.0; 
        }
    }

    /* Partial pivoting (largest magnitude in the column), and a pivot at the
       rounding level of the matrix's own entries means singular. Swapping
       only on an exact zero used tiny pivots as they were and lost all
       accuracy ([[1e-20,1],[1,1]] came out wrong by 1.0). */
    double scale = 0.0;
    for (size_t i = 0; i < n * n; i++) {
        if (fabs(matrix->data[i]) > scale) {
            scale = fabs(matrix->data[i]);
        }
    }
    double tol = (double)n * DBL_EPSILON * scale;

    // Perform Gauss-Jordan elimination
    for (size_t col = 0; col < n; col++) {
        size_t best = col;
        for (size_t r = col + 1; r < n; r++) {
            if (fabs(augmented->data[r * 2 * n + col]) > fabs(augmented->data[best * 2 * n + col])) {
                best = r;
            }
        }
        if (!(fabs(augmented->data[best * 2 * n + col]) > tol)) {
            MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Matrix is singular, cannot invert.");
            matrix_deallocate(augmented);
            return NULL;
        }
        if (best != col) {
            MATRIX_LOG("[matrix_inverse_gauss_jordan] Swapping row %zu with row %zu", col, best);
            matrix_swap_rows(augmented, col, best);
        }

        matrix_row_divide(augmented, col, augmented->data[col * 2 * n + col]);
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Divided row %zu by pivot element", col);

        for (size_t row = 0; row < n; row++) {
            if (row != col) {
                matrix_row_subtract(augmented, row, col, augmented->data[row * 2 * n + col]);
                MATRIX_LOG("[matrix_inverse_gauss_jordan] Eliminated column %zu in row %zu", col, row);
            }
        }
    }

    Matrix* inverse = matrix_create(n, n);
    if (!inverse) {
        MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Memory allocation failed for inverse matrix.");
        matrix_deallocate(augmented);
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            inverse->data[i * n + j] = augmented->data[i * 2 * n + j + n];
        }
    }

    MATRIX_LOG("[matrix_inverse_gauss_jordan] Successfully computed inverse matrix.");
    matrix_deallocate(augmented);
    return inverse;
}

/**
 * @brief Checks if a matrix is positive definite.
 *
 * This function verifies if a given matrix is positive definite. For a matrix to be positive definite, 
 * it must be symmetric and its Cholesky decomposition must exist. If the matrix is not square or not symmetric,
 * the function returns `false`.
 *
 * @param matrix The input matrix to check for positive definiteness.
 * 
 * @return `true` if the matrix is positive definite, otherwise `false`.
 */
bool matrix_is_positive_definite(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_positive_definite] Entering function");

    if (!matrix || matrix->rows != matrix->cols) {
        MATRIX_LOG("[matrix_is_positive_definite] Error: Matrix must be square to check if it's positive definite.");
        return false;
    }
    if (!matrix_is_symmetric(matrix)) {
        MATRIX_LOG("[matrix_is_positive_definite] Error: Matrix is not symmetric.");
        return false;
    }

    MATRIX_LOG("[matrix_is_positive_definite] Attempting Cholesky decomposition.");
    Matrix* chol = matrix_cholesky_decomposition(matrix);
    bool isPositiveDefinite = chol != NULL;

    if (isPositiveDefinite) {
        MATRIX_LOG("[matrix_is_positive_definite] Matrix is positive definite.");
        matrix_deallocate(chol); 
    }
    else {
        MATRIX_LOG("[matrix_is_positive_definite] Error: Cholesky decomposition failed.");
        return false;
    }

    MATRIX_LOG("[matrix_is_positive_definite] Exiting function.");
    return isPositiveDefinite;
}

/**
 * @brief Calculates the projection matrix onto the column space of a given matrix.
 *
 * This function computes the projection matrix P that projects any vector onto the column space of the input matrix A.
 *
 * @param matrix The input matrix A.
 * 
 * @return A pointer to the projection matrix. If any step in the calculation fails (memory allocation, matrix inversion),
 * `NULL` is returned.
 */
Matrix* matrix_projection(const Matrix* matrix) {
    MATRIX_LOG("[matrix_projection] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_projection] Error: Input matrix is null.");
        return NULL;
    }

    MATRIX_LOG("[matrix_projection] Calculating transpose.");
    Matrix* matrixTranspose = matrix_transpose(matrix);
    if (!matrixTranspose) {
        MATRIX_LOG("[matrix_projection] Error: Transpose calculation failed.");
        return NULL;
    }

    MATRIX_LOG("[matrix_projection] Calculating matrix^T * matrix.");
    Matrix* mta = matrix_multiply(matrixTranspose, matrix);
    if (!mta) {
        MATRIX_LOG("[matrix_projection] Error: Multiplication matrix^T * matrix failed.");
        matrix_deallocate(matrixTranspose);
        return NULL;
    }

    // Calculate inverse of m^T * m
    MATRIX_LOG("[matrix_projection] Calculating inverse of matrix^T * matrix.");
    Matrix* mtaInv = matrix_inverse(mta);
    if (!mtaInv) {
        MATRIX_LOG("[matrix_projection] Error: Inverse calculation failed.");
        matrix_deallocate(matrixTranspose); 
        matrix_deallocate(mta); 
        return NULL;
    }

    MATRIX_LOG("[matrix_projection] Calculating A * (A^T * A)^-1.");
    Matrix* m_mta_inv = matrix_multiply(matrix, mtaInv);

    if (!m_mta_inv) {
        MATRIX_LOG("[matrix_projection] Error: Multiplication A * (A^T * A)^-1 failed.");
        matrix_deallocate(matrixTranspose);
        matrix_deallocate(mta);
        matrix_deallocate(mtaInv);
        return NULL;
    }

    // Calculate final projection matrix: A * (A^T * A)^-1 * A^T
    MATRIX_LOG("[matrix_projection] Calculating final projection matrix.");
    Matrix* projection = matrix_multiply(m_mta_inv, matrixTranspose);

    if (!projection) {
        MATRIX_LOG("[matrix_projection] Error: Final projection matrix calculation failed.");
    } 
    else {
        MATRIX_LOG("[matrix_projection] Successfully calculated projection matrix.");
    }

    matrix_deallocate(matrixTranspose);
    matrix_deallocate(mta);
    matrix_deallocate(mtaInv);
    matrix_deallocate(m_mta_inv);

    return projection;
}

/**
 * @brief Generates a Vandermonde matrix from the given input matrix.
 *
 * A Vandermonde matrix is a matrix with terms of a geometric progression in each row. 
 * 
 * @param matrix The input matrix containing the initial values for the Vandermonde matrix.
 * @param n The size (rows and columns) of the resulting Vandermonde matrix.
 * 
 * @return A pointer to the generated Vandermonde matrix. If the input matrix is `NULL` or 
 * memory allocation fails, `NULL` is returned.
 */
Matrix* matrix_vandermonde(const Matrix* matrix, size_t n) {
    MATRIX_LOG("[matrix_vandermonde] Entering function with n = %zu", n);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_vandermonde] Error: Matrix object is null");
        return NULL;
    }
    if (n == 0 || n > matrix->rows * matrix->cols) {
        /* Each of the n rows uses one input element; reading past the input
           would use memory beyond the buffer. */
        MATRIX_LOG("[matrix_vandermonde] Error: n (%zu) must be between 1 and the number of input elements.", n);
        return NULL;
    }

    Matrix* vandermonde = matrix_create(n, n);
    if (!vandermonde) {
        MATRIX_LOG("[matrix_vandermonde] Error: Memory allocation failed for Vandermonde matrix");
        return NULL; 
    }

    MATRIX_LOG("[matrix_vandermonde] Successfully created Vandermonde matrix of size %zux%zu", n, n);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            vandermonde->data[i * n + j] = pow(matrix->data[i], (double)j);
            MATRIX_LOG("[matrix_vandermonde] Setting element at (row, col) = (%zu, %zu) to %lf", i, j, vandermonde->data[i * n + j]);
        }
    }

    MATRIX_LOG("[matrix_vandermonde] Successfully created Vandermonde matrix");
    return vandermonde;
}

/**
 * @brief Generates a companion matrix from the given polynomial coefficients.
 *
 * Follows scipy.linalg.companion: the coefficients are given highest degree
 * first, a[0]·x^(n-1) + a[1]·x^(n-2) + ... + a[n-1]; the first row of the result
 * is -a[1..n-1] / a[0] and the sub-diagonal is 1, so its eigenvalues are the
 * roots of the polynomial.
 *
 * @param coefficients A matrix holding at least `degree` coefficients, highest degree first.
 * @param degree The number of coefficients to use (polynomial degree + 1); at least 2.
 *
 * @return A pointer to the `(degree-1) x (degree-1)` companion matrix, or `NULL` if the coefficients are
 * `NULL`, `degree` is below 2 or above the number of coefficients, the leading coefficient is 0, or memory
 * allocation fails.
 */
Matrix* matrix_companion(const Matrix* coefficients, size_t degree) {
    MATRIX_LOG("[matrix_companion] Entering function with degree = %zu", degree);

    if (!coefficients || !coefficients->data) {
        MATRIX_LOG("[matrix_companion] Error: Coefficients matrix is null");
        return NULL;
    }
    if (degree < 2 || degree > coefficients->rows * coefficients->cols) {
        MATRIX_LOG("[matrix_companion] Error: degree must be between 2 and the number of coefficients.");
        return NULL;
    }
    double lead = coefficients->data[0];
    if (lead == 0.0 || !isfinite(lead)) {
        MATRIX_LOG("[matrix_companion] Error: the leading coefficient must be a non-zero number.");
        return NULL;
    }

    size_t n = degree - 1;
    Matrix* companion = matrix_create(n, n);   /* zero-filled */
    if (!companion) {
        MATRIX_LOG("[matrix_companion] Error: Memory allocation failed for companion matrix");
        return NULL;
    }

    MATRIX_LOG("[matrix_companion] Successfully created companion matrix of size %zux%zu", n, n);

    for (size_t j = 0; j < n; j++) {
        companion->data[j] = -coefficients->data[j + 1] / lead;
    }
    for (size_t i = 1; i < n; i++) {
        companion->data[i * n + (i - 1)] = 1.0;
    }

    MATRIX_LOG("[matrix_companion] Successfully created companion matrix");
    return companion;
}

/**
 * @brief Fills a matrix with a specified value.
 *
 * This function sets every element of the matrix to the provided value.
 *
 * @param matrix The matrix to be filled.
 * @param value The value to fill the matrix with.
 * 
 * @return `true` if the matrix was successfully filled, `false` if the matrix or its data pointer is `NULL`.
 */
bool matrix_fill(Matrix* matrix, double value) {
    MATRIX_LOG("[matrix_fill] Entering function with value = %lf", value);

    if (!matrix) {
        MATRIX_LOG("[matrix_fill] Error: Matrix object is null");
        return false; 
    }
    if (!matrix->data) {
        MATRIX_LOG("[matrix_fill] Error: Matrix data is null");
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            matrix->data[i * matrix->cols + j] = value;
            MATRIX_LOG("[matrix_fill] Setting element at (row, col) = (%zu, %zu) to %lf", i, j, value);
        }
    }

    MATRIX_LOG("[matrix_fill] Successfully filled matrix with value %lf", value);
    return true;
}

/**
 * @brief Applies a specified function to each element of a matrix.
 *
 * This function creates a new matrix where each element is the result of applying the provided function `func`
 * to the corresponding element of the input matrix.
 *
 * @param matrix The input matrix to which the function will be applied.
 * @param func The function to apply to each element of the matrix.
 * 
 * @return A new matrix where each element is the result of applying `func` to the input matrix's elements.
 * If the input matrix or function is `NULL`, or if memory allocation fails, the function returns `NULL`.
 */
Matrix* matrix_map(const Matrix* matrix, MatrixFunc func) {
    MATRIX_LOG("[matrix_map] Entering function");

    if (!matrix || !func) {
        MATRIX_LOG("[matrix_map] Error: NULL argument provided");
        return NULL;
    }

    Matrix* result = (Matrix*)malloc(sizeof(Matrix));
    if (!result) {
        MATRIX_LOG("[matrix_map] Error: Memory allocation failed for matrix structure");
        return NULL;
    }

    result->data = (double*)malloc(matrix->rows * matrix->cols * sizeof(double));
    if (!result->data) {
        MATRIX_LOG("[matrix_map] Error: Memory allocation for matrix data failed");
        free(result); 
        return NULL;
    }

    result->rows = matrix->rows;
    result->cols = matrix->cols;

    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            MATRIX_LOG("[matrix_map] Applying function to element at (row, col) = (%zu, %zu)", i, j);
            result->data[i * matrix->cols + j] = func(matrix->data[i * matrix->cols + j]);
        }
    }

    MATRIX_LOG("[matrix_map] Successfully applied function to matrix");
    return result;
}

/**
 * @brief Finds the minimum element in a matrix.
 *
 * This function iterates through all the elements of the matrix to find and return the minimum value.
 *
 * @param matrix The matrix in which to find the minimum element.
 * 
 * @return The minimum element in the matrix. If the matrix is `NULL`, empty, or invalid, 
 * it returns `DBL_MAX` as an error indicator.
 */
double matrix_min_element(const Matrix* matrix) {
    MATRIX_LOG("[matrix_min_element] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_min_element] Error: Invalid matrix provided");
        return DBL_MAX; 
    }

    /* Seeded with the first element (an all-+inf matrix used to report the
       DBL_MAX error sentinel); a NaN anywhere makes the result NaN, as in
       NumPy. */
    double min = matrix->data[0];
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
            if (isnan(value)) {
                return value;
            }
            if (value < min) {
                MATRIX_LOG("[matrix_min_element] New minimum found: %f at (row, col) = (%zu, %zu)", value, i, j);
                min = value;
            }
        }
    }

    MATRIX_LOG("[matrix_min_element] Minimum element found: %f", min);
    return min;
}


/**
 * @brief Finds the maximum element in a matrix.
 *
 * This function iterates through all the elements of the matrix to find and return the maximum value.
 *
 * @param matrix The matrix in which to find the maximum element.
 * 
 * @return The maximum element in the matrix. If the matrix is `NULL`, empty, or invalid, it returns `-DBL_MAX` as an error indicator.
 */
double matrix_max_element(const Matrix* matrix) {
    MATRIX_LOG("[matrix_max_element] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_max_element] Error: Invalid matrix provided");
        return -DBL_MAX; 
    }

    double max = matrix->data[0];   /* see matrix_min_element */
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
            if (isnan(value)) {
                return value;
            }
            if (value > max) {
                MATRIX_LOG("[matrix_max_element] New maximum found: %f at (row, col) = (%zu, %zu)", value, i, j);
                max = value;
            }
        }
    }

    MATRIX_LOG("[matrix_max_element] Maximum element found: %f", max);
    return max;
}

/**
 * @brief Sums every element of a matrix.
 *
 * The C analogue of NumPy's `numpy.sum(M)`: returns the sum of all elements.
 * For a per-axis reduction (column or row sums) use `matrix_sum_axis`.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return The sum of all elements, or `0.0` if `matrix` is NULL/invalid.
 *
 * @note If any element is `nan`, the result is `nan` (IEEE addition), exactly
 *       as in NumPy.
 */
double matrix_sum(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sum] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_sum] Error: Invalid matrix provided.");
        return 0.0;
    }

    double total = 0.0;
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        total += matrix->data[i];
    }

    MATRIX_LOG("[matrix_sum] Sum = %f", total);
    return total;
}

/**
 * @brief Multiplies every element of a matrix together.
 *
 * The C analogue of NumPy's `numpy.prod(M)`: returns the product of all
 * elements. For a per-axis reduction use `matrix_prod_axis`.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return The product of all elements, or `0.0` if `matrix` is NULL/invalid.
 *
 * @note If any element is `nan`, the result is `nan`, exactly as in NumPy.
 */
double matrix_prod(const Matrix* matrix) {
    MATRIX_LOG("[matrix_prod] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_prod] Error: Invalid matrix provided.");
        return 0.0;
    }

    double product = 1.0;
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        product *= matrix->data[i];
    }

    MATRIX_LOG("[matrix_prod] Product = %f", product);
    return product;
}

/**
 * @brief Computes the arithmetic mean of every element of a matrix.
 *
 * The C analogue of NumPy's `numpy.mean(M)`: returns the sum of all elements
 * divided by the element count. For a per-axis mean use `matrix_mean_axis`.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return The mean of all elements, or `0.0` if `matrix` is NULL/invalid.
 *
 * @note If any element is `nan`, the result is `nan`, exactly as in NumPy.
 */
double matrix_mean(const Matrix* matrix) {
    MATRIX_LOG("[matrix_mean] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_mean] Error: Invalid matrix provided.");
        return 0.0;
    }

    double mean = matrix_sum(matrix) / (double)(matrix->rows * matrix->cols);

    MATRIX_LOG("[matrix_mean] Mean = %f", mean);
    return mean;
}

/**
 * @brief Computes the peak-to-peak range (max - min) of a matrix.
 *
 * The C analogue of NumPy's `numpy.ptp(M)`: returns the difference between the
 * largest and smallest element.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 *
 * @return `max - min` over all elements, or `0.0` if `matrix` is NULL/invalid.
 */
double matrix_ptp(const Matrix* matrix) {
    MATRIX_LOG("[matrix_ptp] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_ptp] Error: Invalid matrix provided.");
        return 0.0;
    }

    double mn = matrix->data[0];
    double mx = matrix->data[0];
    for (size_t i = 1; i < matrix->rows * matrix->cols; ++i) {
        double v = matrix->data[i];
        if (v < mn) {
            mn = v;
        }
        if (v > mx) {
            mx = v;
        }
    }

    MATRIX_LOG("[matrix_ptp] ptp = %f", mx - mn);
    return mx - mn;
}

/**
 * @brief Returns the flattened index of the largest element.
 *
 * The C analogue of NumPy's `numpy.argmax(M)`: the matrix is treated as a flat,
 * row-major (C-order) array and the index of the maximum value is returned. On
 * ties, the index of the first occurrence is returned, exactly as in NumPy.
 *
 * @param matrix The matrix to search. Must not be NULL.
 *
 * @return The flattened (row-major) index of the maximum element, or
 *         `SIZE_MAX` if `matrix` is NULL/invalid. The row/column can be
 *         recovered as `row = index / cols`, `col = index % cols`.
 *
 * @note Tie-breaking matches NumPy for finite inputs. NumPy's special handling
 *       of `nan` (which it reports as the maximum) is not reproduced.
 */
size_t matrix_argmax(const Matrix* matrix) {
    MATRIX_LOG("[matrix_argmax] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_argmax] Error: Invalid matrix provided.");
        return SIZE_MAX;
    }

    size_t best = 0;
    double mx = matrix->data[0];
    for (size_t i = 1; i < matrix->rows * matrix->cols; ++i) {
        if (matrix->data[i] > mx) {
            mx = matrix->data[i];
            best = i;
        }
    }

    MATRIX_LOG("[matrix_argmax] argmax = %zu", best);
    return best;
}

/**
 * @brief Returns the flattened index of the smallest element.
 *
 * The C analogue of NumPy's `numpy.argmin(M)`: the matrix is treated as a flat,
 * row-major (C-order) array and the index of the minimum value is returned. On
 * ties, the index of the first occurrence is returned, exactly as in NumPy.
 *
 * @param matrix The matrix to search. Must not be NULL.
 *
 * @return The flattened (row-major) index of the minimum element, or
 *         `SIZE_MAX` if `matrix` is NULL/invalid. The row/column can be
 *         recovered as `row = index / cols`, `col = index % cols`.
 *
 * @note Tie-breaking matches NumPy for finite inputs.
 */
size_t matrix_argmin(const Matrix* matrix) {
    MATRIX_LOG("[matrix_argmin] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_argmin] Error: Invalid matrix provided.");
        return SIZE_MAX;
    }

    size_t best = 0;
    double mn = matrix->data[0];
    for (size_t i = 1; i < matrix->rows * matrix->cols; ++i) {
        if (matrix->data[i] < mn) {
            mn = matrix->data[i];
            best = i;
        }
    }

    MATRIX_LOG("[matrix_argmin] argmin = %zu", best);
    return best;
}

/**
 * @brief Sums a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.sum(M, axis=...)`:
 *  - `axis == 0` sums down each column, returning a `1 x cols` row vector
 *    (`np.sum(M, axis=0)`);
 *  - `axis == 1` sums across each row, returning a `rows x 1` column vector
 *    (`np.sum(M, axis=1)`).
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column sums, `1` for row sums.
 *
 * @return A new vector of partial sums, or `NULL` if `matrix` is NULL, `axis`
 *         is neither 0 nor 1, or allocation fails. The caller owns the result
 *         and must release it with `matrix_deallocate`.
 */
Matrix* matrix_sum_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_sum_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_sum_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_sum_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    Matrix* result = (axis == 0) ? matrix_create(1, matrix->cols)
                                  : matrix_create(matrix->rows, 1);
    if (!result) {
        MATRIX_LOG("[matrix_sum_axis] Error: Failed to create result vector.");
        return NULL;
    }

    /* matrix_create zero-fills, so we can accumulate directly. */
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double v = matrix->data[i * matrix->cols + j];
            if (axis == 0) {
                result->data[j] += v;   /* column sum */
            }
            else {
                result->data[i] += v;   /* row sum */
            }
        }
    }

    MATRIX_LOG("[matrix_sum_axis] Success: axis sum computed.");
    return result;
}

/**
 * @brief Multiplies a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.prod(M, axis=...)`:
 *  - `axis == 0` multiplies down each column, returning a `1 x cols` row vector;
 *  - `axis == 1` multiplies across each row, returning a `rows x 1` column vector.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column products, `1` for row products.
 *
 * @return A new vector of partial products, or `NULL` if `matrix` is NULL,
 *         `axis` is neither 0 nor 1, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_prod_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_prod_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_prod_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_prod_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    Matrix* result = (axis == 0) ? matrix_create(1, matrix->cols)
                                  : matrix_create(matrix->rows, 1);
    if (!result) {
        MATRIX_LOG("[matrix_prod_axis] Error: Failed to create result vector.");
        return NULL;
    }

    /* Initialise the accumulators to the multiplicative identity. */
    matrix_fill(result, 1.0);

    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double v = matrix->data[i * matrix->cols + j];
            if (axis == 0) {
                result->data[j] *= v;   /* column product */
            }
            else {
                result->data[i] *= v;   /* row product */
            }
        }
    }

    MATRIX_LOG("[matrix_prod_axis] Success: axis product computed.");
    return result;
}

/**
 * @brief Averages a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.mean(M, axis=...)`:
 *  - `axis == 0` averages down each column, returning a `1 x cols` row vector;
 *  - `axis == 1` averages across each row, returning a `rows x 1` column vector.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column means, `1` for row means.
 *
 * @return A new vector of partial means, or `NULL` if `matrix` is NULL, `axis`
 *         is neither 0 nor 1, or allocation fails. The caller owns the result
 *         and must release it with `matrix_deallocate`.
 */
Matrix* matrix_mean_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_mean_axis] Entering function with axis = %d", axis);

    Matrix* sums = matrix_sum_axis(matrix, axis);
    if (!sums) {
        MATRIX_LOG("[matrix_mean_axis] Error: axis sum failed.");
        return NULL;
    }

    /* axis 0 averages over `rows` values per column; axis 1 over `cols`. */
    double denom = (axis == 0) ? (double)matrix->rows : (double)matrix->cols;
    for (size_t i = 0; i < sums->rows * sums->cols; ++i) {
        sums->data[i] /= denom;
    }

    MATRIX_LOG("[matrix_mean_axis] Success: axis mean computed.");
    return sums;
}

/**
 * @brief Reduces a matrix to its per-axis minimum.
 *
 * The C analogue of NumPy's `numpy.min(M, axis=...)`:
 *  - `axis == 0` takes the minimum down each column, returning a `1 x cols` row
 *    vector;
 *  - `axis == 1` takes the minimum across each row, returning a `rows x 1`
 *    column vector.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column minima, `1` for row minima.
 *
 * @return A new vector of per-axis minima, or `NULL` if `matrix` is NULL, `axis`
 *         is neither 0 nor 1, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_min_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_min_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_min_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_min_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;

    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) {
            return NULL;
        }
        for (size_t j = 0; j < C; ++j) {
            double m = matrix->data[j];
            for (size_t i = 1; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (!isnan(m) && (isnan(v) || v < m)) {
                    m = v;
                }
            }
            result->data[j] = m;
        }
        MATRIX_LOG("[matrix_min_axis] Success: column minima computed.");
        return result;
    }

    Matrix* result = matrix_create(R, 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < R; ++i) {
        double m = matrix->data[i * C];
        for (size_t j = 1; j < C; ++j) {
            double v = matrix->data[i * C + j];
            if (!isnan(m) && (isnan(v) || v < m)) {
                m = v;
            }
        }
        result->data[i] = m;
    }
    MATRIX_LOG("[matrix_min_axis] Success: row minima computed.");
    return result;
}

/**
 * @brief Reduces a matrix to its per-axis maximum.
 *
 * The C analogue of NumPy's `numpy.max(M, axis=...)`:
 *  - `axis == 0` takes the maximum down each column, returning a `1 x cols` row
 *    vector;
 *  - `axis == 1` takes the maximum across each row, returning a `rows x 1`
 *    column vector.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column maxima, `1` for row maxima.
 *
 * @return A new vector of per-axis maxima, or `NULL` if `matrix` is NULL, `axis`
 *         is neither 0 nor 1, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_max_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_max_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_max_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_max_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;

    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) {
            return NULL;
        }
        for (size_t j = 0; j < C; ++j) {
            double m = matrix->data[j];
            for (size_t i = 1; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (!isnan(m) && (isnan(v) || v > m)) {
                    m = v;
                }
            }
            result->data[j] = m;
        }
        MATRIX_LOG("[matrix_max_axis] Success: column maxima computed.");
        return result;
    }

    Matrix* result = matrix_create(R, 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < R; ++i) {
        double m = matrix->data[i * C];
        for (size_t j = 1; j < C; ++j) {
            double v = matrix->data[i * C + j];
            if (!isnan(m) && (isnan(v) || v > m)) {
                m = v;
            }
        }
        result->data[i] = m;
    }
    MATRIX_LOG("[matrix_max_axis] Success: row maxima computed.");
    return result;
}

/**
 * @brief Returns the per-axis index of the maximum element.
 *
 * The C analogue of NumPy's `numpy.argmax(M, axis=...)`:
 *  - `axis == 0` returns, for each column, the **row** index of its maximum, as
 *    a `1 x cols` row vector;
 *  - `axis == 1` returns, for each row, the **column** index of its maximum, as
 *    a `rows x 1` column vector.
 *
 * Indices are stored as `double` values. On ties, the first occurrence wins,
 * exactly as in NumPy.
 *
 * @param matrix The matrix to search. Must not be NULL.
 * @param axis   `0` for column-wise (row) indices, `1` for row-wise (column)
 *               indices.
 *
 * @return A new vector of indices, or `NULL` if `matrix` is NULL, `axis` is
 *         neither 0 nor 1, or allocation fails. The caller owns the result and
 *         must release it with `matrix_deallocate`.
 */
Matrix* matrix_argmax_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_argmax_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_argmax_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_argmax_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;

    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) {
            return NULL;
        }
        for (size_t j = 0; j < C; ++j) {
            size_t best = 0;
            double m = matrix->data[j];
            for (size_t i = 1; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (!isnan(m) && (isnan(v) || v > m)) {
                    m = v;
                    best = i;
                }
            }
            result->data[j] = (double)best;
        }
        MATRIX_LOG("[matrix_argmax_axis] Success: column-wise argmax computed.");
        return result;
    }

    Matrix* result = matrix_create(R, 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < R; ++i) {
        size_t best = 0;
        double m = matrix->data[i * C];
        for (size_t j = 1; j < C; ++j) {
            double v = matrix->data[i * C + j];
            if (!isnan(m) && (isnan(v) || v > m)) {
                m = v;
                best = j;
            }
        }
        result->data[i] = (double)best;
    }
    MATRIX_LOG("[matrix_argmax_axis] Success: row-wise argmax computed.");
    return result;
}

/**
 * @brief Counts the number of non-zero elements in a matrix.
 *
 * The C analogue of NumPy's `numpy.count_nonzero(M)`: returns how many elements
 * are not equal to zero. As in NumPy, `-0.0` counts as zero while `nan` and the
 * infinities count as non-zero.
 *
 * @param matrix The matrix to inspect. Must not be NULL.
 *
 * @return The number of non-zero elements, or `0` if `matrix` is NULL/invalid.
 */
size_t matrix_count_nonzero(const Matrix* matrix) {
    MATRIX_LOG("[matrix_count_nonzero] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_count_nonzero] Error: Invalid matrix provided.");
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        if (matrix->data[i] != 0.0) {   /* nan/inf are non-zero; -0.0 is zero */
            ++count;
        }
    }

    MATRIX_LOG("[matrix_count_nonzero] count = %zu", count);
    return count;
}

/**
 * @brief Cumulative sum of all elements in row-major (C) order.
 *
 * The C analogue of NumPy's `numpy.cumsum(M)` with the default `axis=None`: the
 * matrix is flattened in row-major order and the running total is returned as a
 * `1 x (rows*cols)` row vector. For a per-axis cumulative sum use
 * `matrix_cumsum_axis`.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 *
 * @return A new `1 x (rows*cols)` row vector of running totals, or `NULL` if
 *         `matrix` is NULL/invalid or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cumsum(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cumsum] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_cumsum] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(1, n);
    if (!result) {
        MATRIX_LOG("[matrix_cumsum] Error: Failed to create result vector.");
        return NULL;
    }

    double running = 0.0;
    for (size_t i = 0; i < n; ++i) {
        running += matrix->data[i];
        result->data[i] = running;
    }

    MATRIX_LOG("[matrix_cumsum] Success: flattened cumulative sum computed.");
    return result;
}

/**
 * @brief Cumulative sum along one axis.
 *
 * The C analogue of NumPy's `numpy.cumsum(M, axis=...)`. The result has the same
 * shape as the input:
 *  - `axis == 0` accumulates down each column (each element becomes the sum of
 *    itself and every element above it in the column);
 *  - `axis == 1` accumulates across each row (the sum of itself and every
 *    element to its left in the row).
 *
 * @param matrix The matrix to scan. Must not be NULL.
 * @param axis   `0` to accumulate down columns, `1` across rows.
 *
 * @return A new matrix (same shape as `matrix`) of running totals, or `NULL` if
 *         `matrix` is NULL, `axis` is neither 0 nor 1, or allocation fails. The
 *         caller owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cumsum_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_cumsum_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_cumsum_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_cumsum_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        MATRIX_LOG("[matrix_cumsum_axis] Error: Failed to create result matrix.");
        return NULL;
    }

    if (axis == 1) {
        /* running total across each row, left to right */
        for (size_t i = 0; i < matrix->rows; ++i) {
            double running = 0.0;
            for (size_t j = 0; j < matrix->cols; ++j) {
                running += matrix->data[i * matrix->cols + j];
                result->data[i * matrix->cols + j] = running;
            }
        }
    }
    else {
        /* axis == 0: running total down each column, top to bottom */
        for (size_t j = 0; j < matrix->cols; ++j) {
            double running = 0.0;
            for (size_t i = 0; i < matrix->rows; ++i) {
                running += matrix->data[i * matrix->cols + j];
                result->data[i * matrix->cols + j] = running;
            }
        }
    }

    MATRIX_LOG("[matrix_cumsum_axis] Success: axis cumulative sum computed.");
    return result;
}

/**
 * @brief Tests whether any element of a matrix is non-zero.
 *
 * The C analogue of NumPy's `numpy.any(M)`: returns `true` if at least one
 * element is non-zero (true), otherwise `false`. Any non-zero value (including
 * `NaN`) counts as true.
 *
 * @param matrix The matrix to inspect. Must not be NULL.
 *
 * @return `true` if any element is non-zero; `false` if all are zero or `matrix`
 *         is NULL/invalid.
 */
bool matrix_any(const Matrix* matrix) {
    MATRIX_LOG("[matrix_any] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_any] Error: Invalid matrix provided.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        if (matrix->data[i] != 0.0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Tests whether all elements of a matrix are non-zero.
 *
 * The C analogue of NumPy's `numpy.all(M)`: returns `true` if every element is
 * non-zero (true), otherwise `false`. Any non-zero value (including `NaN`)
 * counts as true.
 *
 * @param matrix The matrix to inspect. Must not be NULL.
 *
 * @return `true` if every element is non-zero; `false` if any element is zero or
 *         `matrix` is NULL/invalid.
 */
bool matrix_all(const Matrix* matrix) {
    MATRIX_LOG("[matrix_all] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_all] Error: Invalid matrix provided.");
        return false;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        if (matrix->data[i] == 0.0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Cumulative product of all elements in row-major (C) order.
 *
 * The C analogue of NumPy's `numpy.cumprod(M)` with the default `axis=None`: the
 * matrix is flattened in row-major order and the running product is returned as
 * a `1 x (rows*cols)` row vector. For a per-axis cumulative product use
 * `matrix_cumprod_axis`.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 *
 * @return A new `1 x (rows*cols)` row vector of running products, or `NULL` if
 *         `matrix` is NULL/invalid or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cumprod(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cumprod] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_cumprod] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(1, n);
    if (!result) {
        return NULL;
    }

    double run = 1.0;
    for (size_t i = 0; i < n; ++i) {
        run *= matrix->data[i];
        result->data[i] = run;
    }

    MATRIX_LOG("[matrix_cumprod] Success: flattened cumulative product computed.");
    return result;
}

/**
 * @brief Cumulative product along one axis.
 *
 * The C analogue of NumPy's `numpy.cumprod(M, axis=...)`. The result has the same
 * shape as the input: `axis == 0` accumulates the product down each column,
 * `axis == 1` across each row.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 * @param axis   `0` to accumulate down columns, `1` across rows.
 *
 * @return A new matrix (same shape) of running products, or `NULL` if `matrix`
 *         is NULL, `axis` is neither 0 nor 1, or allocation fails. The caller
 *         owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cumprod_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_cumprod_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_cumprod_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_cumprod_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    if (axis == 1) {
        for (size_t i = 0; i < R; ++i) {
            double run = 1.0;
            for (size_t j = 0; j < C; ++j) {
                run *= matrix->data[i * C + j];
                result->data[i * C + j] = run;
            }
        }
    }
    else {
        for (size_t j = 0; j < C; ++j) {
            double run = 1.0;
            for (size_t i = 0; i < R; ++i) {
                run *= matrix->data[i * C + j];
                result->data[i * C + j] = run;
            }
        }
    }

    MATRIX_LOG("[matrix_cumprod_axis] Success: axis cumulative product computed.");
    return result;
}

/**
 * @brief Running maximum of all elements in row-major (C) order.
 *
 * The C analogue of NumPy's `numpy.maximum.accumulate(M.flatten())`: the matrix
 * is flattened in row-major order and the running (cumulative) maximum is
 * returned as a `1 x (rows*cols)` row vector. For a per-axis running maximum use
 * `matrix_cummax_axis`.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 *
 * @return A new `1 x (rows*cols)` row vector of running maxima, or `NULL` if
 *         `matrix` is NULL/invalid or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cummax(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cummax] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_cummax] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(1, n);
    if (!result) {
        return NULL;
    }

    double run = -INFINITY;
    for (size_t i = 0; i < n; ++i) {
        double v = matrix->data[i];
        if (v > run) {
            run = v;
        }
        result->data[i] = run;
    }

    MATRIX_LOG("[matrix_cummax] Success: flattened running maximum computed.");
    return result;
}

/**
 * @brief Running maximum along one axis.
 *
 * The C analogue of NumPy's `numpy.maximum.accumulate(M, axis=...)`. The result
 * has the same shape as the input: `axis == 0` runs the maximum down each
 * column, `axis == 1` across each row.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 * @param axis   `0` to accumulate down columns, `1` across rows.
 *
 * @return A new matrix (same shape) of running maxima, or `NULL` if `matrix` is
 *         NULL, `axis` is neither 0 nor 1, or allocation fails. The caller owns
 *         the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cummax_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_cummax_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_cummax_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_cummax_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    if (axis == 1) {
        for (size_t i = 0; i < R; ++i) {
            double run = -INFINITY;
            for (size_t j = 0; j < C; ++j) {
                double v = matrix->data[i * C + j];
                if (v > run) {
                    run = v;
                }
                result->data[i * C + j] = run;
            }
        }
    }
    else {
        for (size_t j = 0; j < C; ++j) {
            double run = -INFINITY;
            for (size_t i = 0; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (v > run) {
                    run = v;
                }
                result->data[i * C + j] = run;
            }
        }
    }

    MATRIX_LOG("[matrix_cummax_axis] Success: axis running maximum computed.");
    return result;
}

/**
 * @brief Running minimum of all elements in row-major (C) order.
 *
 * The C analogue of NumPy's `numpy.minimum.accumulate(M.flatten())`: the matrix
 * is flattened in row-major order and the running (cumulative) minimum is
 * returned as a `1 x (rows*cols)` row vector. For a per-axis running minimum use
 * `matrix_cummin_axis`.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 *
 * @return A new `1 x (rows*cols)` row vector of running minima, or `NULL` if
 *         `matrix` is NULL/invalid or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cummin(const Matrix* matrix) {
    MATRIX_LOG("[matrix_cummin] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_cummin] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(1, n);
    if (!result) {
        return NULL;
    }

    double run = INFINITY;
    for (size_t i = 0; i < n; ++i) {
        double v = matrix->data[i];
        if (v < run) {
            run = v;
        }
        result->data[i] = run;
    }

    MATRIX_LOG("[matrix_cummin] Success: flattened running minimum computed.");
    return result;
}

/**
 * @brief Running minimum along one axis.
 *
 * The C analogue of NumPy's `numpy.minimum.accumulate(M, axis=...)`. The result
 * has the same shape as the input: `axis == 0` runs the minimum down each
 * column, `axis == 1` across each row.
 *
 * @param matrix The matrix to scan. Must not be NULL.
 * @param axis   `0` to accumulate down columns, `1` across rows.
 *
 * @return A new matrix (same shape) of running minima, or `NULL` if `matrix` is
 *         NULL, `axis` is neither 0 nor 1, or allocation fails. The caller owns
 *         the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_cummin_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_cummin_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_cummin_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_cummin_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    if (axis == 1) {
        for (size_t i = 0; i < R; ++i) {
            double run = INFINITY;
            for (size_t j = 0; j < C; ++j) {
                double v = matrix->data[i * C + j];
                if (v < run) {
                    run = v;
                }
                result->data[i * C + j] = run;
            }
        }
    }
    else {
        for (size_t j = 0; j < C; ++j) {
            double run = INFINITY;
            for (size_t i = 0; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (v < run) {
                    run = v;
                }
                result->data[i * C + j] = run;
            }
        }
    }

    MATRIX_LOG("[matrix_cummin_axis] Success: axis running minimum computed.");
    return result;
}

/**
 * @brief Variance of a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.var(M, axis=...)` with the default `ddof = 0`
 * (population variance): `axis == 0` gives the variance of each column
 * (`1 x cols`); `axis == 1` gives the variance of each row (`rows x 1`).
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column variances, `1` for row variances.
 *
 * @return A new vector of per-axis variances, or `NULL` if `matrix` is NULL,
 *         `axis` is neither 0 nor 1, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_var_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_var_axis] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_var_axis] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_var_axis] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;

    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) {
            return NULL;
        }
        for (size_t j = 0; j < C; ++j) {
            double mean = 0.0;
            for (size_t i = 0; i < R; ++i) {
                mean += matrix->data[i * C + j];
            }
            mean /= (double)R;
            double s = 0.0;
            for (size_t i = 0; i < R; ++i) {
                double d = matrix->data[i * C + j] - mean;
                s += d * d;
            }
            result->data[j] = s / (double)R;
        }
        MATRIX_LOG("[matrix_var_axis] Success: column variances computed.");
        return result;
    }

    Matrix* result = matrix_create(R, 1);
    if (!result) {
        return NULL;
    }
    for (size_t i = 0; i < R; ++i) {
        double mean = 0.0;
        for (size_t j = 0; j < C; ++j) {
            mean += matrix->data[i * C + j];
        }
        mean /= (double)C;
        double s = 0.0;
        for (size_t j = 0; j < C; ++j) {
            double d = matrix->data[i * C + j] - mean;
            s += d * d;
        }
        result->data[i] = s / (double)C;
    }
    MATRIX_LOG("[matrix_var_axis] Success: row variances computed.");
    return result;
}

/**
 * @brief Standard deviation of a matrix along one axis.
 *
 * The C analogue of NumPy's `numpy.std(M, axis=...)` with the default `ddof = 0`:
 * the element-wise square root of `matrix_var_axis`. `axis == 0` gives the std of
 * each column (`1 x cols`); `axis == 1` gives the std of each row (`rows x 1`).
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column std deviations, `1` for row std deviations.
 *
 * @return A new vector of per-axis standard deviations, or `NULL` if `matrix` is
 *         NULL, `axis` is neither 0 nor 1, or allocation fails. The caller owns
 *         the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_std_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_std_axis] Entering function with axis = %d", axis);

    Matrix* var = matrix_var_axis(matrix, axis);
    if (!var) {
        MATRIX_LOG("[matrix_std_axis] Error: axis variance failed.");
        return NULL;
    }

    for (size_t i = 0; i < var->rows * var->cols; ++i) {
        var->data[i] = sqrt(var->data[i]);
    }

    MATRIX_LOG("[matrix_std_axis] Success: axis standard deviation computed.");
    return var;
}

/**
 * @brief Computes the weighted average of all elements of a matrix.
 *
 * The C analogue of NumPy's `numpy.average(M, weights=W)`: returns
 * `sum(M * W) / sum(W)`. If `weights` is NULL, returns the unweighted mean
 * (equivalent to `matrix_mean`).
 *
 * @param matrix  The matrix to reduce. Must not be NULL.
 * @param weights The weight matrix, same dimensions as `matrix`, or NULL for an
 *                unweighted mean.
 *
 * @return The weighted average, or `0.0` if `matrix` is NULL/invalid or the
 *         weight dimensions do not match.
 *
 * @note As in NumPy, if the weights sum to zero the result follows IEEE-754
 *       (`±inf` or `nan`).
 */
double matrix_average(const Matrix* matrix, const Matrix* weights) {
    MATRIX_LOG("[matrix_average] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_average] Error: Invalid matrix provided.");
        return 0.0;
    }
    if (weights == NULL) {
        return matrix_mean(matrix);
    }
    if (!weights->data || weights->rows != matrix->rows || weights->cols != matrix->cols) {
        MATRIX_LOG("[matrix_average] Error: weights must match the matrix dimensions.");
        return 0.0;
    }

    size_t n = matrix->rows * matrix->cols;
    double num = 0.0;
    double den = 0.0;
    for (size_t i = 0; i < n; ++i) {
        num += matrix->data[i] * weights->data[i];
        den += weights->data[i];
    }

    double avg = num / den;
    MATRIX_LOG("[matrix_average] weighted average = %f", avg);
    return avg;
}

/**
 * @brief Extracts the k-th diagonal of a matrix as a row vector.
 *
 * The C analogue of NumPy's `numpy.diagonal(M, offset=k)`: returns the elements
 * `M[i][i+k]` along the `k`-th diagonal as a `1 x len` row vector. `k = 0` is the
 * main diagonal, `k > 0` selects a super-diagonal (above the main), and `k < 0`
 * a sub-diagonal (below). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param k      The diagonal offset (0 = main, >0 = above, <0 = below).
 *
 * @return A new `1 x len` row vector of the diagonal's elements, or `NULL` if
 *         `matrix` is NULL, the requested diagonal is empty (`k` out of range),
 *         or allocation fails. The caller owns the result and must release it
 *         with `matrix_deallocate`.
 */
Matrix* matrix_diagonal(const Matrix* matrix, int k) {
    MATRIX_LOG("[matrix_diagonal] Entering function with k = %d", k);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_diagonal] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t m = matrix->rows;
    size_t n = matrix->cols;
    ptrdiff_t kk = (ptrdiff_t)k;
    size_t rstart = (kk < 0) ? (size_t)(-kk) : 0;

    /* count the valid (r, r+k) positions */
    size_t len = 0;
    for (size_t r = rstart; r < m; ++r) {
        ptrdiff_t c = (ptrdiff_t)r + kk;
        if (c < 0 || (size_t)c >= n) {
            break;
        }
        ++len;
    }
    if (len == 0) {
        MATRIX_LOG("[matrix_diagonal] Error: requested diagonal is empty.");
        return NULL;
    }

    Matrix* result = matrix_create(1, len);
    if (!result) {
        return NULL;
    }
    size_t idx = 0;
    for (size_t r = rstart; r < m && idx < len; ++r) {
        ptrdiff_t c = (ptrdiff_t)r + kk;
        if (c < 0 || (size_t)c >= n) {
            break;
        }
        result->data[idx++] = matrix->data[r * n + (size_t)c];
    }

    MATRIX_LOG("[matrix_diagonal] Success: diagonal extracted.");
    return result;
}

/**
 * @brief Fills the main diagonal of a matrix in place with a constant value.
 *
 * The C analogue of NumPy's `numpy.fill_diagonal(M, value)`: sets `M[i][i]` to
 * `value` for `i` from 0 to `min(rows, cols) - 1`. This MUTATES the matrix.
 *
 * @param matrix The matrix to modify. Must not be NULL.
 * @param value  The value written to every main-diagonal entry.
 *
 * @return `true` on success; `false` if `matrix` is NULL.
 */
bool matrix_fill_diagonal(Matrix* matrix, double value) {
    MATRIX_LOG("[matrix_fill_diagonal] Entering function with value = %lf", value);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_fill_diagonal] Error: Invalid matrix provided.");
        return false;
    }

    size_t lim = (matrix->rows < matrix->cols) ? matrix->rows : matrix->cols;
    for (size_t i = 0; i < lim; ++i) {
        matrix->data[i * matrix->cols + i] = value;
    }

    MATRIX_LOG("[matrix_fill_diagonal] Success: main diagonal filled.");
    return true;
}

/**
 * @brief Builds coordinate grids from two 1-D vectors.
 *
 * The C analogue of NumPy's `numpy.meshgrid(x, y)` with the default `indexing='xy'`:
 * given a length-`nx` vector `x` and a length-`ny` vector `y` (each treated as a
 * flattened 1-D vector), produces two `ny x nx` matrices. `X` has `x` repeated
 * down every row, and `Y` has `y` repeated across every column.
 *
 * @param x The x-coordinate vector. Must not be NULL.
 * @param y The y-coordinate vector. Must not be NULL.
 * @param X Out-parameter: receives the `ny x nx` matrix of x-coordinates.
 * @param Y Out-parameter: receives the `ny x nx` matrix of y-coordinates.
 *
 * @return `true` on success; `false` if any argument is NULL, a vector is empty,
 *         or allocation fails. On success the caller owns `*X` and `*Y` and must
 *         release each with `matrix_deallocate`.
 */
bool matrix_meshgrid(const Matrix* x, const Matrix* y, Matrix** X, Matrix** Y) {
    MATRIX_LOG("[matrix_meshgrid] Entering function");

    if (!x || !y || !X || !Y || !x->data || !y->data) {
        MATRIX_LOG("[matrix_meshgrid] Error: NULL argument provided.");
        return false;
    }

    size_t nx = x->rows * x->cols;
    size_t ny = y->rows * y->cols;
    if (nx == 0 || ny == 0) {
        MATRIX_LOG("[matrix_meshgrid] Error: empty input vector.");
        return false;
    }

    Matrix* Xm = matrix_create(ny, nx);
    Matrix* Ym = matrix_create(ny, nx);
    if (!Xm || !Ym) {
        if (Xm) {
            matrix_deallocate(Xm);
        }
        if (Ym) {
            matrix_deallocate(Ym);
        }
        return false;
    }

    for (size_t i = 0; i < ny; ++i) {
        for (size_t j = 0; j < nx; ++j) {
            Xm->data[i * nx + j] = x->data[j];   /* each row is x */
            Ym->data[i * nx + j] = y->data[i];   /* each column is y */
        }
    }

    *X = Xm;
    *Y = Ym;
    MATRIX_LOG("[matrix_meshgrid] Success: coordinate grids created.");
    return true;
}

/**
 * @brief Returns the sorted unique values of a matrix.
 *
 * The C analogue of NumPy's `numpy.unique(M)`: the matrix is flattened, sorted in
 * ascending order, and duplicate values removed, returning a `1 x k` row vector
 * of the `k` distinct values. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new `1 x k` row vector of sorted unique values, or `NULL` if `matrix`
 *         is NULL/invalid or a temporary allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_unique(const Matrix* matrix) {
    MATRIX_LOG("[matrix_unique] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_unique] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    double* tmp = (double*)malloc(n * sizeof(double));
    if (!tmp) {
        return NULL;
    }
    memcpy(tmp, matrix->data, n * sizeof(double));
    qsort(tmp, n, sizeof(double), mat_cmp_asc);

    /* compact out adjacent duplicates; NaNs (sorted last) count as one value,
       as in NumPy (NaN != NaN kept every one of them) */
    size_t k = 0;
    for (size_t i = 0; i < n; ++i) {
        if (k == 0 || !(tmp[i] == tmp[k - 1] || (isnan(tmp[i]) && isnan(tmp[k - 1])))) {
            tmp[k++] = tmp[i];
        }
    }

    Matrix* result = matrix_create(1, k);
    if (!result) {
        free(tmp);
        return NULL;
    }
    memcpy(result->data, tmp, k * sizeof(double));
    free(tmp);

    MATRIX_LOG("[matrix_unique] Success: %zu unique values found.", k);
    return result;
}

/**
 * @brief Finds insertion indices into a sorted vector (binary search).
 *
 * The C analogue of NumPy's `numpy.searchsorted(a, v)` with the default
 * `side='left'`: for each element of `v`, returns the leftmost index at which it
 * could be inserted into the sorted vector `a` while keeping it sorted (i.e. the
 * number of elements of `a` strictly less than the value). Both inputs are
 * treated as flattened 1-D vectors; `a` must already be sorted ascending.
 *
 * @param a The sorted reference vector. Must not be NULL.
 * @param v The query values. Must not be NULL.
 *
 * @return A new matrix (same shape as `v`) of insertion indices stored as
 *         `double`, or `NULL` if either input is NULL/empty or allocation fails.
 *         The caller owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_searchsorted(const Matrix* a, const Matrix* v) {
    MATRIX_LOG("[matrix_searchsorted] Entering function");

    if (!a || !v || !a->data || !v->data) {
        MATRIX_LOG("[matrix_searchsorted] Error: NULL argument provided.");
        return NULL;
    }

    size_t na = a->rows * a->cols;
    size_t nv = v->rows * v->cols;
    if (na == 0 || nv == 0) {
        MATRIX_LOG("[matrix_searchsorted] Error: empty input vector.");
        return NULL;
    }

    Matrix* result = matrix_create(v->rows, v->cols);
    if (!result) {
        return NULL;
    }

    for (size_t t = 0; t < nv; ++t) {
        double key = v->data[t];
        size_t lo = 0;
        size_t hi = na;
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            /* NaN orders after every number (NumPy): a NaN key goes past
               all non-NaN entries instead of to index 0 */
            if (isnan(key) ? !isnan(a->data[mid]) : (a->data[mid] < key)) {
                lo = mid + 1;
            }
            else {
                hi = mid;
            }
        }
        result->data[t] = (double)lo;
    }

    MATRIX_LOG("[matrix_searchsorted] Success: insertion indices computed.");
    return result;
}

/**
 * @brief Splits a matrix into equal blocks along an axis.
 *
 * The C analogue of NumPy's `numpy.split(M, sections, axis)` (and `hsplit`/
 * `vsplit`): divides the matrix into `sections` equal pieces along the chosen
 * axis. `axis = 0` splits the rows (`vsplit`); `axis = 1` splits the columns
 * (`hsplit`). The axis length must be evenly divisible by `sections`.
 *
 * @param matrix    The source matrix. Must not be NULL.
 * @param sections  The number of equal pieces. Must be greater than 0 and divide
 *                  the axis length evenly.
 * @param axis      `0` to split rows, `1` to split columns.
 * @param out_count Out-parameter: receives the number of returned pieces
 *                  (`sections`), or 0 on failure. May be NULL.
 *
 * @return A newly allocated array of `sections` matrices, or `NULL` on any error
 *         (NULL matrix, zero sections, bad axis, non-divisible length, or
 *         allocation failure). The caller owns both the array and each matrix:
 *         release every element with `matrix_deallocate`, then `free` the array.
 */
Matrix** matrix_split(const Matrix* matrix, size_t sections, int axis, size_t* out_count) {
    MATRIX_LOG("[matrix_split] Entering function with sections = %zu, axis = %d", sections, axis);

    if (out_count) {
        *out_count = 0;
    }
    if (!matrix || !matrix->data || sections == 0) {
        MATRIX_LOG("[matrix_split] Error: Invalid matrix or sections.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_split] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    size_t along = (axis == 0) ? R : C;
    if (along % sections != 0) {
        MATRIX_LOG("[matrix_split] Error: axis length not divisible by sections.");
        return NULL;
    }
    size_t block = along / sections;

    Matrix** parts = (Matrix**)malloc(sections * sizeof(Matrix*));
    if (!parts) {
        return NULL;
    }
    for (size_t s = 0; s < sections; ++s) {
        parts[s] = NULL;
    }

    for (size_t s = 0; s < sections; ++s) {
        Matrix* p = (axis == 0) ? matrix_create(block, C) : matrix_create(R, block);
        if (!p) {
            for (size_t t = 0; t < sections; ++t) {
                if (parts[t]) {
                    matrix_deallocate(parts[t]);
                }
            }
            free(parts);
            return NULL;
        }
        if (axis == 0) {
            memcpy(p->data, &matrix->data[s * block * C], block * C * sizeof(double));
        }
        else {
            for (size_t i = 0; i < R; ++i) {
                for (size_t j = 0; j < block; ++j) {
                    p->data[i * block + j] = matrix->data[i * C + (s * block + j)];
                }
            }
        }
        parts[s] = p;
    }

    if (out_count) {
        *out_count = sections;
    }
    MATRIX_LOG("[matrix_split] Success: matrix split into %zu pieces.", sections);
    return parts;
}

/**
 * @brief Computes the p-th percentile of a matrix.
 *
 * The C analogue of NumPy's `numpy.percentile(M, p)` with the default
 * `method='linear'`: the matrix is flattened and sorted, and the value at rank
 * `p/100 * (n-1)` is returned, linearly interpolating between the two
 * surrounding samples. The input is not modified.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param p      The percentile in the range `[0, 100]`.
 *
 * @return The p-th percentile, or `0.0` if `matrix` is NULL/invalid or a
 *         temporary allocation fails.
 */
double matrix_percentile(const Matrix* matrix, double p) {
    MATRIX_LOG("[matrix_percentile] Entering function with p = %lf", p);

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_percentile] Error: Invalid matrix provided.");
        return 0.0;
    }
    if (!(p >= 0.0 && p <= 100.0)) {
        /* Like NumPy, p must lie in [0, 100]; a negative or NaN p used to
           convert a negative double to size_t (undefined behaviour) and
           index far outside the buffer. */
        MATRIX_LOG("[matrix_percentile] Error: p must be in [0, 100].");
        return NAN;
    }

    size_t n = matrix->rows * matrix->cols;
    double* tmp = (double*)malloc(n * sizeof(double));
    if (!tmp) {
        return 0.0;
    }
    memcpy(tmp, matrix->data, n * sizeof(double));
    qsort(tmp, n, sizeof(double), mat_cmp_asc);

    double rank = (p / 100.0) * (double)(n - 1);
    size_t lo = (size_t)floor(rank);
    size_t hi = (size_t)ceil(rank);
    double frac = rank - (double)lo;

    double result;
    if (isnan(tmp[n - 1])) {
        result = tmp[n - 1];   /* NaN sorts last: any NaN -> NaN, like NumPy */
    }
    else if (hi >= n) {
        result = tmp[n - 1];
    }
    else {
        result = tmp[lo] + frac * (tmp[hi] - tmp[lo]);
    }

    free(tmp);
    MATRIX_LOG("[matrix_percentile] percentile = %f", result);
    return result;
}

/**
 * @brief Standardises each column of a matrix to zero mean and unit variance.
 *
 * The C analogue of `scipy.stats.zscore(M, axis=0)`: for every column, subtracts
 * the column mean and divides by the column standard deviation (population,
 * `ddof = 0`), so each column has mean 0 and standard deviation 1. The input is
 * not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new standardised matrix (same shape), or `NULL` if `matrix` is NULL
 *         or allocation fails. The caller owns the result and must release it
 *         with `matrix_deallocate`.
 *
 * @note A column with zero variance yields `nan` (division by zero), matching
 *       SciPy's `zscore`.
 */
Matrix* matrix_standardize(const Matrix* matrix) {
    MATRIX_LOG("[matrix_standardize] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_standardize] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    for (size_t j = 0; j < C; ++j) {
        double mean = 0.0;
        for (size_t i = 0; i < R; ++i) {
            mean += matrix->data[i * C + j];
        }
        mean /= (double)R;

        double var = 0.0;
        for (size_t i = 0; i < R; ++i) {
            double d = matrix->data[i * C + j] - mean;
            var += d * d;
        }
        var /= (double)R;
        double sd = sqrt(var);

        for (size_t i = 0; i < R; ++i) {
            result->data[i * C + j] = (matrix->data[i * C + j] - mean) / sd;
        }
    }

    MATRIX_LOG("[matrix_standardize] Success: columns standardised.");
    return result;
}

/**
 * @brief Min-max scales each column of a matrix to the range [0, 1].
 *
 * The data-analysis "min-max" scaler (as in `sklearn.preprocessing.MinMaxScaler`):
 * for every column, maps the minimum to 0 and the maximum to 1 via
 * `(x - min) / (max - min)`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new scaled matrix (same shape), or `NULL` if `matrix` is NULL or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 *
 * @note A constant column (`max == min`) is mapped to all zeros, avoiding a
 *       division by zero (matching scikit-learn's behaviour).
 */
Matrix* matrix_normalize(const Matrix* matrix) {
    MATRIX_LOG("[matrix_normalize] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_normalize] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    for (size_t j = 0; j < C; ++j) {
        double mn = matrix->data[j];
        double mx = matrix->data[j];
        for (size_t i = 1; i < R; ++i) {
            double v = matrix->data[i * C + j];
            if (v < mn) {
                mn = v;
            }
            if (v > mx) {
                mx = v;
            }
        }
        double range = mx - mn;
        for (size_t i = 0; i < R; ++i) {
            result->data[i * C + j] = (range == 0.0) ? 0.0
                                                     : (matrix->data[i * C + j] - mn) / range;
        }
    }

    MATRIX_LOG("[matrix_normalize] Success: columns min-max scaled.");
    return result;
}

/**
 * @brief Computes the trailing moving average over a sliding window.
 *
 * The C analogue of pandas' `DataFrame.rolling(window).mean()`: each output
 * element is the mean of the `window` most recent values along the chosen axis.
 * `axis = 0` rolls down each column; `axis = 1` rolls across each row. The first
 * `window - 1` positions along the axis have too few preceding values and are set
 * to `NaN`. The result has the same shape as the input, which is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param window The window length. Must be greater than 0 and at most the axis
 *               length.
 * @param axis   `0` to roll down columns, `1` to roll across rows.
 *
 * @return A new matrix (same shape) of moving averages with `NaN` in the warm-up
 *         region, or `NULL` if `matrix` is NULL, `window` is 0 or larger than the
 *         axis, `axis` is neither 0 nor 1, or allocation fails. The caller owns
 *         the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_rolling_mean(const Matrix* matrix, size_t window, int axis) {
    MATRIX_LOG("[matrix_rolling_mean] Entering function with window = %zu, axis = %d", window, axis);

    if (!matrix || !matrix->data || window == 0) {
        MATRIX_LOG("[matrix_rolling_mean] Error: Invalid matrix or window.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_rolling_mean] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    size_t along = (axis == 0) ? R : C;
    if (window > along) {
        MATRIX_LOG("[matrix_rolling_mean] Error: window larger than axis length.");
        return NULL;
    }

    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    if (axis == 0) {
        for (size_t j = 0; j < C; ++j) {
            for (size_t i = 0; i < R; ++i) {
                if (i + 1 < window) {
                    result->data[i * C + j] = NAN;
                }
                else {
                    double s = 0.0;
                    for (size_t w = 0; w < window; ++w) {
                        s += matrix->data[(i - w) * C + j];
                    }
                    result->data[i * C + j] = s / (double)window;
                }
            }
        }
    }
    else {
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                if (j + 1 < window) {
                    result->data[i * C + j] = NAN;
                }
                else {
                    double s = 0.0;
                    for (size_t w = 0; w < window; ++w) {
                        s += matrix->data[i * C + (j - w)];
                    }
                    result->data[i * C + j] = s / (double)window;
                }
            }
        }
    }

    MATRIX_LOG("[matrix_rolling_mean] Success: rolling mean computed.");
    return result;
}

/* Linear-interpolated percentile of an already-sorted array (NumPy method). */
static double mat_pct_sorted(const double* a, size_t n, double p) {
    if (n > 0 && isnan(a[n - 1])) {
        return a[n - 1];   /* mat_cmp_asc sorts NaN last: any NaN -> NaN, like NumPy */
    }
    if (n == 1) {
        return a[0];
    }
    double rank = (p / 100.0) * (double)(n - 1);
    size_t lo = (size_t)floor(rank);
    size_t hi = (size_t)ceil(rank);
    double frac = rank - (double)lo;
    if (hi >= n) {
        return a[n - 1];
    }
    return a[lo] + frac * (a[hi] - a[lo]);
}

/**
 * @brief Shifts a matrix along an axis, filling vacated positions.
 *
 * The C analogue of pandas' `DataFrame.shift(periods, axis, fill_value)`: moves
 * the data by `periods` positions along the axis **without wrapping** (unlike
 * `matrix_roll`), and fills the vacated cells with `fill_value`. A positive
 * `periods` shifts forward (down for `axis = 0`, right for `axis = 1`); a
 * negative `periods` shifts the other way. The input is not modified.
 *
 * @param matrix     The source matrix. Must not be NULL.
 * @param periods    Number of positions to shift (any `int`).
 * @param axis       `0` to shift rows, `1` to shift columns.
 * @param fill_value The value placed in the vacated cells (use `NAN` to match
 *                   pandas' default).
 *
 * @return A new matrix (same shape) with the data shifted and gaps filled, or
 *         `NULL` if `matrix` is NULL, `axis` is neither 0 nor 1, or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_shift(const Matrix* matrix, int periods, int axis, double fill_value) {
    MATRIX_LOG("[matrix_shift] Entering function with periods = %d, axis = %d", periods, axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_shift] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_shift] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    if (axis == 0) {
        for (size_t i = 0; i < R; ++i) {
            ptrdiff_t src = (ptrdiff_t)i - periods;
            for (size_t j = 0; j < C; ++j) {
                result->data[i * C + j] = (src >= 0 && (size_t)src < R)
                                              ? matrix->data[(size_t)src * C + j]
                                              : fill_value;
            }
        }
    }
    else {
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                ptrdiff_t src = (ptrdiff_t)j - periods;
                result->data[i * C + j] = (src >= 0 && (size_t)src < C)
                                              ? matrix->data[i * C + (size_t)src]
                                              : fill_value;
            }
        }
    }

    MATRIX_LOG("[matrix_shift] Success: matrix shifted.");
    return result;
}

/**
 * @brief Computes the fractional change between adjacent elements along an axis.
 *
 * The C analogue of pandas' `DataFrame.pct_change()` (periods = 1): each element
 * becomes `(current - previous) / previous` along the axis. `axis = 0` compares
 * to the element in the row above; `axis = 1` to the element in the column to the
 * left. The first position along the axis has no predecessor and is set to
 * `NaN`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param axis   `0` to compare down columns, `1` across rows.
 *
 * @return A new matrix (same shape) of fractional changes, or `NULL` if `matrix`
 *         is NULL, `axis` is neither 0 nor 1, or allocation fails. The caller
 *         owns the result and must release it with `matrix_deallocate`.
 *
 * @note Division by a previous value of zero follows IEEE-754 (`±inf`/`nan`), as
 *       in pandas.
 */
Matrix* matrix_pct_change(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_pct_change] Entering function with axis = %d", axis);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_pct_change] Error: Invalid matrix provided.");
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        MATRIX_LOG("[matrix_pct_change] Error: axis must be 0 or 1.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) {
        return NULL;
    }

    if (axis == 0) {
        for (size_t j = 0; j < C; ++j) {
            result->data[j] = NAN;
            for (size_t i = 1; i < R; ++i) {
                double prev = matrix->data[(i - 1) * C + j];
                result->data[i * C + j] = (matrix->data[i * C + j] - prev) / prev;
            }
        }
    }
    else {
        for (size_t i = 0; i < R; ++i) {
            result->data[i * C] = NAN;
            for (size_t j = 1; j < C; ++j) {
                double prev = matrix->data[i * C + (j - 1)];
                result->data[i * C + j] = (matrix->data[i * C + j] - prev) / prev;
            }
        }
    }

    MATRIX_LOG("[matrix_pct_change] Success: percentage change computed.");
    return result;
}

/**
 * @brief Replaces every NaN in a matrix with a constant value.
 *
 * The C analogue of pandas' `DataFrame.fillna(value)`: each `NaN` element becomes
 * `value`; all other elements are copied unchanged. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param value  The value substituted for each `NaN`.
 *
 * @return A new matrix (same shape) with NaNs replaced, or `NULL` if `matrix` is
 *         NULL or allocation fails. The caller owns the result and must release
 *         it with `matrix_deallocate`.
 */
Matrix* matrix_fillna(const Matrix* matrix, double value) {
    MATRIX_LOG("[matrix_fillna] Entering function with value = %lf", value);

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_fillna] Error: Invalid matrix provided.");
        return NULL;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        result->data[i] = isnan(matrix->data[i]) ? value : matrix->data[i];
    }

    MATRIX_LOG("[matrix_fillna] Success: NaNs replaced.");
    return result;
}

/**
 * @brief Produces a per-column summary of descriptive statistics.
 *
 * The C analogue of pandas' `DataFrame.describe()`: for each column (variable),
 * computes eight statistics, returning an `8 x cols` matrix whose rows are, in
 * order: count, mean, standard deviation, minimum, 25th percentile, 50th
 * percentile (median), 75th percentile, and maximum.
 *
 * @param matrix The source matrix (rows = observations, cols = variables). Must
 *               not be NULL.
 *
 * @return A new `8 x cols` summary matrix, or `NULL` if `matrix` is NULL/invalid
 *         or a temporary allocation fails. The caller owns the result and must
 *         release it with `matrix_deallocate`.
 *
 * @note The standard deviation is the **sample** std (`ddof = 1`, divide by
 *       `n - 1`), matching pandas — distinct from `matrix_std`, which is the
 *       population std. The percentiles use linear interpolation. Assumes finite
 *       (non-NaN) data; the count is the number of rows.
 */
Matrix* matrix_describe(const Matrix* matrix) {
    MATRIX_LOG("[matrix_describe] Entering function");

    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        MATRIX_LOG("[matrix_describe] Error: Invalid matrix provided.");
        return NULL;
    }

    size_t R = matrix->rows;
    size_t C = matrix->cols;
    Matrix* result = matrix_create(8, C);
    if (!result) {
        return NULL;
    }

    double* col = (double*)malloc(R * sizeof(double));
    if (!col) {
        matrix_deallocate(result);
        return NULL;
    }

    for (size_t j = 0; j < C; ++j) {
        for (size_t i = 0; i < R; ++i) {
            col[i] = matrix->data[i * C + j];
        }

        double mean = 0.0;
        for (size_t i = 0; i < R; ++i) {
            mean += col[i];
        }
        mean /= (double)R;

        double ss = 0.0;
        for (size_t i = 0; i < R; ++i) {
            double d = col[i] - mean;
            ss += d * d;
        }
        double std = (R > 1) ? sqrt(ss / (double)(R - 1)) : 0.0;   /* sample std, ddof = 1 */

        qsort(col, R, sizeof(double), mat_cmp_asc);

        result->data[0 * C + j] = (double)R;                  /* count */
        result->data[1 * C + j] = mean;                       /* mean  */
        result->data[2 * C + j] = std;                        /* std   */
        result->data[3 * C + j] = col[0];                     /* min   */
        result->data[4 * C + j] = mat_pct_sorted(col, R, 25); /* 25%   */
        result->data[5 * C + j] = mat_pct_sorted(col, R, 50); /* 50%   */
        result->data[6 * C + j] = mat_pct_sorted(col, R, 75); /* 75%   */
        result->data[7 * C + j] = col[R - 1];                 /* max   */
    }

    free(col);
    MATRIX_LOG("[matrix_describe] Success: summary computed.");
    return result;
}

/**
 * @brief Computes the nuclear (trace) norm of a matrix.
 *
 * The C analogue of NumPy's `numpy.linalg.norm(M, 'nuc')`: the sum of the
 * singular values, computed from the SVD. Works for any shape (a wide matrix is
 * transposed first, since `A` and `Aᵀ` share singular values).
 *
 * @param matrix The matrix to measure. Must not be NULL.
 *
 * @return The sum of the singular values, or `NaN` if `matrix` is NULL or the
 *         SVD fails.
 */
double matrix_nuclear_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nuclear_norm] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_nuclear_norm] Error: Invalid matrix provided.");
        return NAN;
    }

    const Matrix* target = matrix;
    Matrix* transposed = NULL;
    if (matrix->rows < matrix->cols) {
        transposed = matrix_transpose(matrix);
        if (!transposed) {
            return NAN;
        }
        target = transposed;
    }

    Matrix* U = NULL;
    Matrix* S = NULL;
    Matrix* V = NULL;
    if (!matrix_svd(target, &U, &S, &V)) {
        if (transposed) {
            matrix_deallocate(transposed);
        }
        return NAN;
    }

    double sum = 0.0;
    for (size_t i = 0; i < S->rows * S->cols; ++i) {
        sum += S->data[i];
    }

    matrix_deallocate(U);
    matrix_deallocate(S);
    matrix_deallocate(V);
    if (transposed) {
        matrix_deallocate(transposed);
    }

    MATRIX_LOG("[matrix_nuclear_norm] nuclear norm = %f", sum);
    return sum;
}

/**
 * @brief Computes the spectral (2-)norm of a matrix.
 *
 * The C analogue of NumPy's `numpy.linalg.norm(M, 2)`: the largest singular
 * value, computed from the SVD. This is the true matrix 2-norm, complementing
 * the existing Frobenius / L1 / infinity norms. Works for any shape.
 *
 * @param matrix The matrix to measure. Must not be NULL.
 *
 * @return The largest singular value, or `NaN` if `matrix` is NULL or the SVD
 *         fails.
 */
double matrix_spectral_norm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_spectral_norm] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_spectral_norm] Error: Invalid matrix provided.");
        return NAN;
    }

    const Matrix* target = matrix;
    Matrix* transposed = NULL;
    if (matrix->rows < matrix->cols) {
        transposed = matrix_transpose(matrix);
        if (!transposed) {
            return NAN;
        }
        target = transposed;
    }

    Matrix* U = NULL;
    Matrix* S = NULL;
    Matrix* V = NULL;
    if (!matrix_svd(target, &U, &S, &V)) {
        if (transposed) {
            matrix_deallocate(transposed);
        }
        return NAN;
    }

    double smax = S->data[0];   /* descending order */

    matrix_deallocate(U);
    matrix_deallocate(S);
    matrix_deallocate(V);
    if (transposed) {
        matrix_deallocate(transposed);
    }

    MATRIX_LOG("[matrix_spectral_norm] spectral norm = %f", smax);
    return smax;
}

/**
 * @brief Evaluates a polynomial at one or more points.
 *
 * The C analogue of NumPy's `numpy.polyval(p, x)`: given the polynomial
 * coefficients `p = [p0, p1, ..., pn]` (highest degree first, so the polynomial
 * is `p0·xⁿ + p1·xⁿ⁻¹ + ... + pn`), evaluates it at every element of `x` using
 * Horner's method. The result has the same shape as `x`. Neither input is
 * modified.
 *
 * @param p The coefficient vector (any shape; flattened, highest degree first).
 *          Must not be NULL.
 * @param x The points at which to evaluate (any shape). Must not be NULL.
 *
 * @return A new matrix (same shape as `x`) of evaluated values, or `NULL` if
 *         either input is NULL/empty or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_polyval(const Matrix* p, const Matrix* x) {
    MATRIX_LOG("[matrix_polyval] Entering function");

    if (!p || !x || !p->data || !x->data) {
        MATRIX_LOG("[matrix_polyval] Error: NULL argument provided.");
        return NULL;
    }

    size_t np = p->rows * p->cols;
    size_t nx = x->rows * x->cols;
    if (np == 0 || nx == 0) {
        MATRIX_LOG("[matrix_polyval] Error: empty input.");
        return NULL;
    }

    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) {
        return NULL;
    }

    for (size_t t = 0; t < nx; ++t) {
        double xi = x->data[t];
        double acc = p->data[0];
        for (size_t k = 1; k < np; ++k) {
            acc = acc * xi + p->data[k];
        }
        result->data[t] = acc;
    }

    MATRIX_LOG("[matrix_polyval] Success: polynomial evaluated.");
    return result;
}

/**
 * @brief Computes the sign and natural log of the absolute determinant.
 *
 * The C analogue of NumPy's `numpy.linalg.slogdet(M)`: returns `sign` and
 * `logabsdet` such that `det(M) = sign · exp(logabsdet)`. Computed via Gaussian
 * elimination with partial pivoting and accumulation of `log|pivot|`, so it
 * avoids the overflow/underflow that a direct determinant can suffer for large
 * matrices.
 *
 * @param matrix    The square matrix. Must not be NULL.
 * @param sign      Out-parameter: receives `-1`, `0`, or `+1` (0 if singular).
 * @param logabsdet Out-parameter: receives `log|det|` (`-inf` if singular).
 *
 * @return `true` on success; `false` if `matrix` is NULL, not square, an
 *         out-pointer is NULL, or a temporary allocation fails.
 */
bool matrix_slogdet(const Matrix* matrix, double* sign, double* logabsdet) {
    MATRIX_LOG("[matrix_slogdet] Entering function");

    if (!matrix || !matrix->data || !sign || !logabsdet) {
        MATRIX_LOG("[matrix_slogdet] Error: NULL argument provided.");
        return false;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_slogdet] Error: matrix must be square.");
        return false;
    }

    size_t n = matrix->rows;
    double* a = (double*)malloc(n * n * sizeof(double));
    if (!a) {
        return false;
    }
    memcpy(a, matrix->data, n * n * sizeof(double));

    double sgn = 1.0;
    double logabs = 0.0;
    for (size_t k = 0; k < n; ++k) {
        /* partial pivot: largest |a[i][k]| for i >= k */
        size_t piv = k;
        double maxv = fabs(a[k * n + k]);
        for (size_t i = k + 1; i < n; ++i) {
            double v = fabs(a[i * n + k]);
            if (v > maxv) {
                maxv = v;
                piv = i;
            }
        }
        if (maxv == 0.0) {
            *sign = 0.0;
            *logabsdet = -INFINITY;
            free(a);
            return true;
        }
        if (piv != k) {
            for (size_t j = 0; j < n; ++j) {
                double tmp = a[k * n + j];
                a[k * n + j] = a[piv * n + j];
                a[piv * n + j] = tmp;
            }
            sgn = -sgn;
        }
        double pivot = a[k * n + k];
        if (pivot < 0.0) {
            sgn = -sgn;
        }
        logabs += log(fabs(pivot));
        for (size_t i = k + 1; i < n; ++i) {
            double f = a[i * n + k] / pivot;
            for (size_t j = k; j < n; ++j) {
                a[i * n + j] -= f * a[k * n + j];
            }
        }
    }

    *sign = sgn;
    *logabsdet = logabs;
    free(a);
    MATRIX_LOG("[matrix_slogdet] sign = %f, logabsdet = %f", sgn, logabs);
    return true;
}

/* ---- minimal complex helpers for the polynomial root finder ---- */
typedef struct {
    double re;
    double im;
} mat_cplx;

static mat_cplx mat_cadd(mat_cplx a, mat_cplx b) {
    mat_cplx r = { a.re + b.re, a.im + b.im };
    return r;
}
static mat_cplx mat_csub(mat_cplx a, mat_cplx b) {
    mat_cplx r = { a.re - b.re, a.im - b.im };
    return r;
}
static mat_cplx mat_cmul(mat_cplx a, mat_cplx b) {
    mat_cplx r = { a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re };
    return r;
}
static mat_cplx mat_cdiv(mat_cplx a, mat_cplx b) {
    double d = b.re * b.re + b.im * b.im;
    mat_cplx r = { (a.re * b.re + a.im * b.im) / d, (a.im * b.re - a.re * b.im) / d };
    return r;
}
static double mat_cabs(mat_cplx a) {
    return sqrt(a.re * a.re + a.im * a.im);
}

/**
 * @brief Computes the roots of a polynomial.
 *
 * The C analogue of NumPy's `numpy.roots(p)`: given coefficients
 * `p = [p0, p1, ..., pn]` (highest degree first), returns the `n` roots of the
 * polynomial `p0·xⁿ + ... + pn`. Because the roots may be complex, the result is
 * an `n x 2` matrix whose first column holds the real parts and second column
 * the imaginary parts. Found with the Durand-Kerner (Weierstrass) iteration,
 * which solves for all roots simultaneously. The input is not modified.
 *
 * @param coeffs The coefficient vector (any shape; flattened, highest degree
 *               first). Must not be NULL.
 *
 * @return A new `n x 2` matrix of `(real, imag)` root pairs, or `NULL` if
 *         `coeffs` is NULL, has degree below 1 (a constant has no roots), or
 *         allocation fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 *
 * @note Real roots come back with a tiny (≈0) imaginary part. The iteration is
 *       most reliable for polynomials with distinct roots.
 */
Matrix* matrix_roots(const Matrix* coeffs) {
    MATRIX_LOG("[matrix_roots] Entering function");

    if (!coeffs || !coeffs->data) {
        MATRIX_LOG("[matrix_roots] Error: Invalid coefficients provided.");
        return NULL;
    }

    size_t m = coeffs->rows * coeffs->cols;
    size_t start = 0;
    while (start < m && coeffs->data[start] == 0.0) {
        ++start;   /* strip leading zeros (they do not change the roots) */
    }
    size_t len = m - start;
    if (len < 2) {
        MATRIX_LOG("[matrix_roots] Error: polynomial degree must be at least 1.");
        return NULL;
    }
    size_t deg = len - 1;

    double lead = coeffs->data[start];
    double* c = (double*)malloc((deg + 1) * sizeof(double));
    mat_cplx* z = (mat_cplx*)malloc(deg * sizeof(mat_cplx));
    if (!c || !z) {
        free(c);
        free(z);
        return NULL;
    }
    for (size_t i = 0; i <= deg; ++i) {
        c[i] = coeffs->data[start + i] / lead;   /* monic */
    }

    /* initial guesses: distinct points (0.4 + 0.9i)^(k+1) */
    mat_cplx seed = { 0.4, 0.9 };
    mat_cplx cur = { 1.0, 0.0 };
    for (size_t i = 0; i < deg; ++i) {
        cur = mat_cmul(cur, seed);
        z[i] = cur;
    }

    for (int it = 0; it < 1000; ++it) {
        double maxcorr = 0.0;
        for (size_t i = 0; i < deg; ++i) {
            /* evaluate the monic polynomial at z[i] (Horner) */
            mat_cplx val = { c[0], 0.0 };
            for (size_t k = 1; k <= deg; ++k) {
                mat_cplx ck = { c[k], 0.0 };
                val = mat_cadd(mat_cmul(val, z[i]), ck);
            }
            /* denominator = product over j != i of (z[i] - z[j]) */
            mat_cplx den = { 1.0, 0.0 };
            for (size_t j = 0; j < deg; ++j) {
                if (j != i) {
                    den = mat_cmul(den, mat_csub(z[i], z[j]));
                }
            }
            mat_cplx delta = mat_cdiv(val, den);
            z[i] = mat_csub(z[i], delta);
            double corr = mat_cabs(delta);
            if (corr > maxcorr) {
                maxcorr = corr;
            }
        }
        if (maxcorr < 1e-14) {
            break;
        }
    }

    Matrix* result = matrix_create(deg, 2);
    if (!result) {
        free(c);
        free(z);
        return NULL;
    }
    for (size_t i = 0; i < deg; ++i) {
        result->data[i * 2 + 0] = z[i].re;
        result->data[i * 2 + 1] = z[i].im;
    }

    free(c);
    free(z);
    MATRIX_LOG("[matrix_roots] Success: %zu roots computed.", deg);
    return result;
}

/**
 * @brief Computes the matrix exponential e^A.
 *
 * The C analogue of SciPy's `scipy.linalg.expm(A)`, using the scaling-and-squaring
 * method with a [6/6] Padé approximation: scales `A` so its norm is small,
 * approximates `e^(A/2^s)` by a Padé rational `D⁻¹N`, then squares the result `s`
 * times. The input must be square and is not modified.
 *
 * @param matrix The square matrix. Must not be NULL.
 *
 * @return A new matrix holding `e^A`, or `NULL` if `matrix` is NULL, not square,
 *         contains inf/NaN, or an intermediate computation/allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_expm(const Matrix* matrix) {
    MATRIX_LOG("[matrix_expm] Entering function");

    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_expm] Error: Invalid matrix provided.");
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_expm] Error: matrix must be square.");
        return NULL;
    }

    size_t n = matrix->rows;

    /* choose scaling s so that ||A|| / 2^s <= 0.5 */
    double norm = matrix_infinity_norm(matrix);
    if (!isfinite(norm)) {
        /* inf/2 == inf: the scaling loop would never end (NaN would skip it
           and return garbage). */
        MATRIX_LOG("[matrix_expm] Error: matrix contains inf or NaN.");
        return NULL;
    }
    int s = 0;
    double scaled_norm = norm;
    while (scaled_norm > 0.5) {
        scaled_norm /= 2.0;
        ++s;
    }

    /* X = A / 2^s */
    double scale = 1.0;
    for (int i = 0; i < s; ++i) {
        scale *= 0.5;
    }
    Matrix* X = matrix_copy(matrix);
    if (!X) {
        return NULL;
    }
    for (size_t i = 0; i < n * n; ++i) {
        X->data[i] *= scale;
    }

    /* [6/6] Padé coefficients via the standard recurrence */
    const int q = 6;
    double c[7];
    c[0] = 1.0;
    for (int k = 0; k < q; ++k) {
        c[k + 1] = c[k] * (double)(q - k) / (double)((2 * q - k) * (k + 1));
    }

    Matrix* N = matrix_create(n, n);   /* numerator   */
    Matrix* D = matrix_create(n, n);   /* denominator */
    Matrix* Xpow = matrix_create_identity(n);

    if (!N || !D || !Xpow) {
        matrix_deallocate(X);
        if (N) { 
            matrix_deallocate(N); 
        }
        if (D) { 
            matrix_deallocate(D); 
        }
        if (Xpow) { 
            matrix_deallocate(Xpow); 
        }
        return NULL;
    }

    /* N = c0*I, D = c0*I  (k = 0 term) */
    for (size_t i = 0; i < n * n; ++i) {
        N->data[i] = c[0] * Xpow->data[i];
        D->data[i] = c[0] * Xpow->data[i];
    }
    /* accumulate k = 1..q: N += c_k X^k ; D += (-1)^k c_k X^k */
    bool ok = true;
    for (int k = 1; k <= q && ok; ++k) {
        Matrix* next = matrix_multiply(Xpow, X);   /* X^k */
        if (!next) {
            ok = false;
            break;
        }
        matrix_deallocate(Xpow);

        Xpow = next;
        double sgn = (k % 2 == 0) ? 1.0 : -1.0;
        for (size_t i = 0; i < n * n; ++i) {
            N->data[i] += c[k] * Xpow->data[i];
            D->data[i] += sgn * c[k] * Xpow->data[i];
        }
    }
    matrix_deallocate(Xpow);
    matrix_deallocate(X);

    Matrix* E = NULL;
    if (ok) {
        Matrix* Dinv = matrix_inverse(D);   /* solve D E = N */
        if (Dinv) {
            E = matrix_multiply(Dinv, N);
            matrix_deallocate(Dinv);
        }
    }
    matrix_deallocate(N);
    matrix_deallocate(D);

    if (!E) {
        MATRIX_LOG("[matrix_expm] Error: Padé inversion/multiply failed.");
        return NULL;
    }

    /* square s times: E = E^(2^s) */
    for (int i = 0; i < s; ++i) {
        Matrix* sq = matrix_multiply(E, E);
        matrix_deallocate(E);
        
        if (!sq) {
            MATRIX_LOG("[matrix_expm] Error: squaring step failed.");
            return NULL;
        }
        E = sq;
    }

    MATRIX_LOG("[matrix_expm] Success: matrix exponential computed.");
    return E;
}

/**
 * @brief Applies a function to each element of a specified row in a matrix.
 *
 * This function applies the provided function `func` to each element of the specified row in the matrix, 
 * modifying the row in place.
 *
 * @param matrix The matrix whose row will be modified.
 * @param row The index of the row to which the function will be applied.
 * @param func The function to apply to each element of the specified row.
 * 
 * @return `true` if the operation is successful, `false` if the matrix or function is `NULL`, 
 * or the row index is out of bounds.
 */
bool matrix_apply_to_row(Matrix* matrix, size_t row, MatrixFunc func) {
    MATRIX_LOG("[matrix_apply_to_row] Entering function with row = %zu", row);

    if (!matrix || !func || row >= matrix->rows) {
        MATRIX_LOG("[matrix_apply_to_row] Error: Invalid arguments. Matrix is NULL or row index out of bounds.");
        return false;
    }
    for (size_t j = 0; j < matrix->cols; ++j) {
        MATRIX_LOG("[matrix_apply_to_row] Applying function to element at (row, col) = (%zu, %zu)", row, j);
        matrix->data[row * matrix->cols + j] = func(matrix->data[row * matrix->cols + j]);
    }

    MATRIX_LOG("[matrix_apply_to_row] Successfully applied function to row %zu", row);
    return true;
}

/**
 * @brief Applies a function to each element of a specified column in a matrix.
 *
 * This function applies the provided function `func` to each element of the specified column in the matrix, 
 * modifying the column in place.
 *
 * @param matrix The matrix whose column will be modified.
 * @param col The index of the column to which the function will be applied.
 * @param func The function to apply to each element of the specified column.
 * 
 * @return `true` if the operation is successful, `false` if the matrix or function is `NULL`, 
 * or the column index is out of bounds.
 */
bool matrix_apply_to_col(Matrix* matrix, size_t col, MatrixFunc func) {
    MATRIX_LOG("[matrix_apply_to_col] Entering function with col = %zu", col);

    if (!matrix || !func || col >= matrix->cols) {
        MATRIX_LOG("[matrix_apply_to_col] Error: Invalid arguments. Matrix is NULL or column index out of bounds.");
        return false;
    }
    for (size_t i = 0; i < matrix->rows; ++i) {
        MATRIX_LOG("[matrix_apply_to_col] Applying function to element at (row, col) = (%zu, %zu)", i, col);
        matrix->data[i * matrix->cols + col] = func(matrix->data[i * matrix->cols + col]);
    }

    MATRIX_LOG("[matrix_apply_to_col] Successfully applied function to column %zu", col);
    return true;
}

/**
 * @brief Adds one row of a matrix to another, optionally scaling the source row before adding.
 *
 * This function adds the elements of the `sourceRow` to the corresponding elements of the `targetRow`
 * in the matrix, optionally scaling the source row by a specified factor before the addition.
 *
 * @param matrix The matrix in which the row addition will take place.
 * @param targetRow The index of the row that will be updated (the row to which the source row is added).
 * @param sourceRow The index of the row that will be added to the target row.
 * @param scale The scaling factor by which to multiply the source row before adding. Use 1.0 for no scaling.
 * 
 * @return `true` if the operation is successful, `false` if the matrix is `NULL` or the row indices are invalid.
 */
bool matrix_row_addition(Matrix* matrix, size_t targetRow, size_t sourceRow, double scale) {
    MATRIX_LOG("[matrix_row_addition] Entering function with targetRow = %zu, sourceRow = %zu, scale = %f", targetRow, sourceRow, scale);

    if (!matrix || targetRow >= matrix->rows || sourceRow >= matrix->rows) {
        MATRIX_LOG("[matrix_row_addition] Error: Invalid arguments. Matrix is NULL or row indices out of bounds.");
        return false;
    }
    for (size_t j = 0; j < matrix->cols; ++j) {
        MATRIX_LOG("[matrix_row_addition] Adding scaled sourceRow (%zu) element at (row, col) = (%zu, %zu) to targetRow (%zu)", sourceRow, sourceRow, j, targetRow);
        matrix->data[targetRow * matrix->cols + j] += scale * matrix->data[sourceRow * matrix->cols + j];
    }

    MATRIX_LOG("[matrix_row_addition] Successfully added row %zu to row %zu", sourceRow, targetRow);
    return true;
}

/**
 * @brief Adds one column of a matrix to another, optionally scaling the source column before adding.
 *
 * This function adds the elements of the `sourceCol` to the corresponding elements of the `targetCol`
 * in the matrix, optionally scaling the source column by a specified factor before the addition.
 *
 * @param matrix The matrix in which the column addition will take place.
 * @param targetCol The index of the column that will be updated (the column to which the source column is added).
 * @param sourceCol The index of the column that will be added to the target column.
 * @param scale The scaling factor by which to multiply the source column before adding. Use 1.0 for no scaling.
 * 
 * @return `true` if the operation is successful, `false` if the matrix is `NULL` or the column indices are invalid.
 */
bool matrix_col_addition(Matrix* matrix, size_t targetCol, size_t sourceCol, double scale) {
    MATRIX_LOG("[matrix_col_addition] Entering function with targetCol = %zu, sourceCol = %zu, scale = %f", targetCol, sourceCol, scale);

    if (!matrix || targetCol >= matrix->cols || sourceCol >= matrix->cols) {
        MATRIX_LOG("[matrix_col_addition] Error: Invalid arguments. Matrix is NULL or column indices out of bounds.");
        return false;
    }
    for (size_t i = 0; i < matrix->rows; ++i) {
        MATRIX_LOG("[matrix_col_addition] Adding scaled sourceCol (%zu) element at (row, col) = (%zu, %zu) to targetCol (%zu)", sourceCol, i, sourceCol, targetCol);
        matrix->data[i * matrix->cols + targetCol] += scale * matrix->data[i * matrix->cols + sourceCol];
    }

    MATRIX_LOG("[matrix_col_addition] Successfully added column %zu to column %zu", sourceCol, targetCol);
    return true;
}

/**
 * @brief Creates a Leslie matrix using fecundity and survival coefficients.
 *
 * A Leslie matrix is a type of matrix used in population ecology to model the dynamics of a population 
 * with different age classes. The first row contains fecundity coefficients, and the sub-diagonal contains 
 * survival coefficients.
 *
 * @param f A matrix representing the fecundity coefficients.
 * @param f_size The number of fecundity coefficients (the number of columns in the Leslie matrix).
 * @param s A matrix representing the survival coefficients.
 * @param s_size The number of survival coefficients (one less than the number of rows in the Leslie matrix).
 * 
 * @return A pointer to the newly created Leslie matrix, or `NULL` if the input parameters are invalid 
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_leslie(Matrix* f, size_t f_size, Matrix* s, size_t s_size) {
    MATRIX_LOG("[matrix_leslie] Entering function with f_size = %zu, s_size = %zu", f_size, s_size);

    if (!f || !f->data) {
        MATRIX_LOG("[matrix_leslie] Error: Matrix f is null");
        return NULL;
    }
    if (f_size != s_size + 1) {
        MATRIX_LOG("[matrix_leslie] Error: f_size (%zu) must be one more than s_size (%zu)", f_size, s_size);
        return NULL;
    }
    /* The sizes are caller-supplied: they must not exceed the matrices they
       describe, or the loops below read past the buffers. */
    if (f_size > f->rows * f->cols ||
        (s_size > 0 && (!s || !s->data || s_size > s->rows * s->cols))) {
        MATRIX_LOG("[matrix_leslie] Error: f_size/s_size exceed the given matrices (or s is NULL).");
        return NULL;
    }

    // Create an N x N matrix, where N is the size of the fecundity array f
    MATRIX_LOG("[matrix_leslie] Creating Leslie matrix of size %zux%zu", f_size, f_size);
    Matrix* leslie = matrix_create(f_size, f_size);

    if (!leslie) {
        MATRIX_LOG("[matrix_leslie] Error: Memory allocation failed for Leslie matrix");
        return NULL;
    }

    // Set the first row with fecundity coefficients
    for (size_t i = 0; i < f_size; ++i) {
        MATRIX_LOG("[matrix_leslie] Setting fecundity coefficient at position (0, %zu)", i);
        matrix_set(leslie, 0, i, f->data[i]);
    }
    for (size_t i = 1; i < f_size; ++i) {
        MATRIX_LOG("[matrix_leslie] Setting survival coefficient at position (%zu, %zu)", i, i - 1);
        matrix_set(leslie, i, i - 1, s->data[i - 1]);
    }

    MATRIX_LOG("[matrix_leslie] Successfully created Leslie matrix");

    return leslie;
}

/**
 * @brief Creates a Fiedler matrix from a given matrix.
 *
 * This function generates a Fiedler matrix, which is a symmetric matrix where each element 
 * at position (i, j) is the absolute difference between the elements at positions i and j of 
 * the input matrix's data array.
 *
 * @param matrix The input matrix whose elements will be used to generate the Fiedler matrix.
 * @return A pointer to the newly created Fiedler matrix, or `NULL` if the input matrix is `NULL`
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_fiedler(const Matrix* matrix) {
    MATRIX_LOG("[matrix_fiedler] Entering function");

    if (!matrix) {
        MATRIX_LOG("[matrix_fiedler] Error: Matrix object is null");
        return NULL;
    }

    size_t n = matrix->cols >= matrix->rows ? matrix->cols : matrix->rows;
    MATRIX_LOG("[matrix_fiedler] Creating Fiedler matrix of size %zux%zu", n, n);

    Matrix* fiedler = matrix_create(n, n);
    if (!fiedler) {
        MATRIX_LOG("[matrix_fiedler] Error: Memory allocation failed for Fiedler matrix");
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double value = fabs(matrix->data[i] - matrix->data[j]);
            MATRIX_LOG("[matrix_fiedler] Setting element at (%zu, %zu) to %f", i, j, value);
            matrix_set(fiedler, i, j, value);
        }
    }

    MATRIX_LOG("[matrix_fiedler] Successfully created Fiedler matrix");
    return fiedler;
}


/**
 * @brief Creates the inverse of a Hilbert matrix of size `n`.
 *
 * This function generates the inverse of a Hilbert matrix, which is a square matrix with 
 * elements defined by the formula for the inverse Hilbert matrix. The function uses binomial 
 * coefficients and factorials to calculate the values.
 *
 * @param n The size of the Hilbert matrix to be inverted.
 * 
 * @return A pointer to the newly created inverse Hilbert matrix, or `NULL` if memory allocation fails. 
 * The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_inverse_hilbert(size_t n) {
    MATRIX_LOG("[matrix_inverse_hilbert] Entering function with n = %zu", n);
    Matrix* invH = matrix_create(n, n);

    if (!invH) {
        MATRIX_LOG("[matrix_inverse_hilbert] Error: Memory allocation failed for inverse Hilbert matrix");
        return NULL;
    }

    /* (H^-1)_ij = (-1)^(i+j) (i+j+1) C(n+i, n-j-1) C(n+j, n-i-1) C(i+j, i)^2,
       evaluated in double precision: the entries grow past int64 for n >= 11
       (the old int64 version overflowed, which is undefined behaviour). They
       are exact integers while below 2^53. */
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            size_t s = i + j;
            double sign = (s % 2 == 0) ? 1.0 : -1.0;
            double cij = mat_binomial(s, i);
            double value = sign * (double)(s + 1) * mat_binomial(n + i, n - j - 1) * mat_binomial(n + j, n - i - 1) * cij * cij;

            MATRIX_LOG("[matrix_inverse_hilbert] Setting element at (%zu, %zu) to %f", i, j, value);
            matrix_set(invH, i, j, value);
        }
    }

    MATRIX_LOG("[matrix_inverse_hilbert] Successfully created inverse Hilbert matrix");
    return invH;
}


/**
 * @brief Extracts a specific row from a matrix and returns it as a new matrix.
 *
 * This function creates a new matrix containing the elements of the specified row from the input matrix.
 *
 * @param matrix The input matrix from which the row is to be extracted.
 * @param row The index of the row to extract (0-based).
 * 
 * @return A pointer to the newly created matrix containing the specified row, or `NULL` if the input matrix is `NULL`, the row index is out of bounds,
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_get_row(const Matrix* matrix, size_t row) {
    MATRIX_LOG("[matrix_get_row] Entering function with row = %zu", row);

    if (!matrix) {
        MATRIX_LOG("[matrix_get_row] Error: Matrix object is null or invalid");
        return NULL;
    }
    else if (row >= matrix->rows) {
        MATRIX_LOG("[matrix_get_row] Error: Row index (%zu) out of bounds, matrix has %zu rows", row, matrix->rows);
        return NULL;
    }

    MATRIX_LOG("[matrix_get_row] Creating row matrix with 1 row and %zu columns", matrix->cols);

    Matrix* r = matrix_create(1, matrix->cols);
    if (!r) {
        MATRIX_LOG("[matrix_get_row] Error: Memory allocation failed for row");
        return NULL;
    }
    
    for (size_t j = 0; j < matrix->cols; j++) {
        MATRIX_LOG("[matrix_get_row] Extracting element at (row, col) = (%zu, %zu)", row, j);
        matrix_set(r, 0, j, matrix_get(matrix, row, j));
    }
    MATRIX_LOG("[matrix_get_row] Successfully created row matrix");

    return r;
}

/**
 * @brief Extracts a specific column from a matrix and returns it as a new matrix.
 *
 * This function creates a new matrix containing the elements of the specified column from the input matrix.
 *
 * @param matrix The input matrix from which the column is to be extracted.
 * @param col The index of the column to extract (0-based).
 * 
 * @return A pointer to the newly created matrix containing the specified column, or `NULL` if the input matrix is `NULL`, the column index is out of bounds,
 * or memory allocation fails. The caller is responsible for freeing the allocated matrix.
 */
Matrix* matrix_get_col(const Matrix* matrix, size_t col) {
    MATRIX_LOG("[matrix_get_col] Entering function with col = %zu", col);

    if (!matrix) {
        MATRIX_LOG("[matrix_get_col] Error: Matrix object is null or invalid");
        return NULL;
    }
    else if (col >= matrix->cols) {
        MATRIX_LOG("[matrix_get_col] Error: Column index (%zu) out of bounds, matrix has %zu columns", col, matrix->cols);
        return NULL;
    }
    MATRIX_LOG("[matrix_get_col] Creating column matrix with %zu rows and 1 column", matrix->rows);

    Matrix* c = matrix_create(matrix->rows, 1);
    if (!c) {
        MATRIX_LOG("[matrix_get_col] Error: Memory allocation failed for column");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        MATRIX_LOG("[matrix_get_col] Extracting element at (row, col) = (%zu, %zu)", i, col);
        matrix_set(c, i, 0, matrix_get(matrix, i, col));
    }
    MATRIX_LOG("[matrix_get_col] Successfully created column matrix");

    return c;
}

/**
 * @brief Converts a matrix to a 1D array of doubles.
 *
 * This function copies the elements of the matrix into a newly allocated 1D array.
 * The array is a linear representation of the matrix data in row-major order.
 *
 * @param matrix The matrix to be converted to an array.
 * 
 * @return A pointer to the newly allocated array containing the matrix elements, or `NULL` if the matrix is `NULL` or memory allocation fails. 
 * The caller is responsible for freeing the allocated memory.
 */
double* matrix_to_array(const Matrix* matrix) {
    MATRIX_LOG("[matrix_to_array] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_to_array] Error: Matrix object is null or invalid");
        return NULL;
    }

    size_t size = matrix->rows * matrix->cols * sizeof(double);
    MATRIX_LOG("[matrix_to_array] Allocating array of size %zu bytes", size);

    double* data = (double*) malloc(size);
    if (!data) {
        MATRIX_LOG("[matrix_to_array] Error: Memory allocation failed");
        return NULL;
    }

    MATRIX_LOG("[matrix_to_array] Copying matrix data to array");
    memcpy(data, matrix->data, size);
    MATRIX_LOG("[matrix_to_array] Successfully converted matrix to array");

    return data;
}

/**
 * @brief Creates a block diagonal matrix from a variable number of matrices.
 *
 * This function constructs a block diagonal matrix by placing each provided matrix
 * along the diagonal of a larger matrix, leaving the off-diagonal blocks as zero matrices.
 *
 * @param count The number of matrices to include in the block diagonal matrix.
 * @param ... A variable number of `Matrix*` pointers, each representing a block to be placed on the diagonal.
 * 
 * @return A pointer to the newly created block diagonal matrix, or `NULL` if memory allocation fails.
 * The caller is responsible for freeing the allocated matrix using `matrix_deallocate`.
 */
Matrix* matrix_block_diag(size_t count, ...) {
    MATRIX_LOG("[matrix_block_diag] Entering function with count = %zu", count);

    va_list args;
    size_t totalRows = 0, totalCols = 0;

    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        Matrix* mat = va_arg(args, Matrix*);
        if (!mat || !mat->data ||
            !mat_size_add(totalRows, mat->rows, &totalRows) || !mat_size_add(totalCols, mat->cols, &totalCols)) {
            MATRIX_LOG("[matrix_block_diag] Error: block %zu is NULL or the total size overflows.", i + 1);
            va_end(args);
            return NULL;
        }

        MATRIX_LOG("[matrix_block_diag] Matrix %zu: %zux%zu", i + 1, mat->rows, mat->cols);
    }
    va_end(args);
    MATRIX_LOG("[matrix_block_diag] Total matrix size: %zux%zu", totalRows, totalCols);

    Matrix* result = matrix_create(totalRows, totalCols);
    if (!result) {
        MATRIX_LOG("[matrix_block_diag] Error: Memory allocation failed for block diagonal matrix");
        return NULL;
    }

    size_t currentRow = 0, currentCol = 0;
    va_start(args, count);

    for (size_t i = 0; i < count; ++i) {
        Matrix* mat = va_arg(args, Matrix*);
        for (size_t r = 0; r < mat->rows; ++r) {
            // Log data copying process for each matrix block
            MATRIX_LOG("[matrix_block_diag] Copying data for matrix %zu into block diagonal matrix at position (%zu, %zu)", i + 1, currentRow, currentCol);
            memcpy(result->data + (currentRow + r) * totalCols + currentCol, mat->data + r * mat->cols, mat->cols * sizeof(double));
        }
        currentRow += mat->rows;
        currentCol += mat->cols;
    }

    va_end(args);
    MATRIX_LOG("[matrix_block_diag] Successfully created block diagonal matrix");

    return result;
}

/**
 * @brief Determines if a matrix is sparse.
 *
 * A matrix is considered sparse if a significant portion of its elements are zero.
 * This function calculates the percentage of non-zero elements and considers 
 * the matrix sparse if less than 30% of the elements are non-zero.
 *
 * @param matrix The matrix to check for sparsity.
 * @return `true` if the matrix is sparse, otherwise `false`.
 */
bool matrix_is_sparse(const Matrix* matrix) {
    MATRIX_LOG("[matrix_is_sparse] Entering function");
    if (!matrix || !matrix->data) {
        MATRIX_LOG("[matrix_is_sparse] Error: Matrix object is null or invalid");
        return false;
    }

    size_t totalElements = matrix->rows * matrix->cols;
    size_t nonZeroCount = 0;

    MATRIX_LOG("[matrix_is_sparse] Matrix size: %zux%zu (%zu total elements)", matrix->rows, matrix->cols, totalElements);
    for (size_t i = 0; i < totalElements; ++i) {
        if (matrix->data[i] != 0) {
            ++nonZeroCount;
        }
    }
    MATRIX_LOG("[matrix_is_sparse] Non-zero elements count: %zu", nonZeroCount);

    double nonZeroPercentage = (double)nonZeroCount / (double)totalElements;
    MATRIX_LOG("[matrix_is_sparse] Non-zero percentage: %.2f%%", nonZeroPercentage * 100);

    bool isSparse = nonZeroPercentage < 0.3;
    MATRIX_LOG("[matrix_is_sparse] Matrix is %s", isSparse ? "sparse" : "not sparse");

    return isSparse;
}

/**
 * @brief Returns the total number of elements in a matrix.
 *
 * This function calculates the size of the matrix, defined as the product 
 * of its rows and columns.
 *
 * @param matrix The matrix whose size is to be determined.
 * @return The total number of elements in the matrix, or 0 if the matrix is NULL.
 */
size_t matrix_size(const Matrix *matrix) {
    MATRIX_LOG("[matrix_size] Entering function");
    if (!matrix) {
        MATRIX_LOG("[matrix_size] Error: Matrix object is null or invalid");
        return 0;
    }

    size_t size = matrix->rows * matrix->cols;
    MATRIX_LOG("[matrix_size] Matrix size: %zu", size);

    return size;
}

/**
 * @brief Creates a matrix with random integer values.
 *
 * This function generates a matrix of the specified size (row x col) and fills it 
 * with random integers within the specified range [start, end). The random values 
 * are generated using the standard C library's `rand()` function.
 *
 * @param row The number of rows in the matrix.
 * @param col The number of columns in the matrix.
 * @param start The minimum value (inclusive) of the random range.
 * @param end The maximum value (exclusive) of the random range.
 * 
 * @return A pointer to the newly created matrix filled with random values, 
 * or NULL if the matrix creation fails.
 */
Matrix* matrix_random(size_t row, size_t col, size_t start, size_t end) {
    MATRIX_LOG("[matrix_random] Entering function with row=%zu, col=%zu, start=%zu, end=%zu", row, col, start, end);

    Matrix* matrix = matrix_create(row, col);

    if (!matrix) {
        MATRIX_LOG("[matrix_random] Error: creation of Matrix object failed");
        return NULL;
    }
    MATRIX_LOG("[matrix_random] Matrix created with size %zux%zu", row, col);

    /* A private generator instead of srand(time(NULL)) + rand(): reseeding on
       every call made all calls within one second identical and clobbered the
       program's own rand() sequence, rand() never exceeds RAND_MAX (32767 on
       Windows), and rand() % range is biased. Rejection sampling keeps the
       draw uniform over the whole [start, end) range. */
    uint64_t range = (end > start) ? (uint64_t)(end - start) : 1;
    uint64_t limit = (UINT64_MAX / range) * range;
    for (size_t i = 0; i < matrix_size(matrix); i++) {
        uint64_t r;
        do {
            r = mat_rng_next();
        } while (r >= limit);
        matrix->data[i] = (double)(start + (size_t)(r % range));
    }
    MATRIX_LOG("[matrix_random] Matrix filled with random values in range [%zu, %zu)", start, end);

    return matrix;
}

/**
 * @brief Creates a Walsh matrix of size n.
 *
 * This function generates a Walsh matrix, which is a special kind of square 
 * matrix used in various applications such as signal processing and Hadamard 
 * transforms. The size of the matrix must be a power of 2.
 *
 * @param n The size of the Walsh matrix (must be a power of 2).
 * 
 * @return A pointer to the newly created Walsh matrix, or NULL if the size `n` 
 * is not a power of 2 or if the matrix creation fails.
 */
Matrix* matrix_walsh(size_t n) {
    MATRIX_LOG("[matrix_walsh] Entering function with n = %zu", n);
    if (n & (n - 1)) {
        MATRIX_LOG("[matrix_walsh] Error - 'n' (%zu) is not a power of 2", n);
        return NULL;
    }
    MATRIX_LOG("[matrix_walsh] Creating Walsh matrix of size %zux%zu", n, n);

    Matrix* walshMatrix = matrix_create(n, n);
    if (!walshMatrix) {
        MATRIX_LOG("[matrix_walsh] Error - Memory allocation failed for Walsh matrix of size %zux%zu", n, n);
        return NULL;
    }

    MATRIX_LOG("[matrix_walsh] Generating Walsh matrix recursively");
    generateWalshMatrixRecursively(walshMatrix->data, n, n, 0, 0, 1);
    MATRIX_LOG("[matrix_walsh] Successfully generated Walsh matrix");
    
    return walshMatrix;
}


/**
 * @brief Computes the element-wise (Hadamard) product of two matrices.
 *
 * Multiplies @p A and @p B entry by entry: `C[i][j] = A[i][j] * B[i][j]`.
 * Both matrices must have identical dimensions. This is distinct from
 * `matrix_multiply` (the dot-product matrix product) and
 * `matrix_kronecker_product`.
 *
 * @param A The first matrix.
 * @param B The second matrix (same dimensions as @p A).
 *
 * @return A new matrix holding the element-wise product, or NULL if either
 *         input is NULL, the dimensions differ, or allocation fails. The
 *         caller owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_hadamard_product(const Matrix* A, const Matrix* B) {
    MATRIX_LOG("[matrix_hadamard_product] Entering function.");
    if (!A || !B || !A->data || !B->data) {
        MATRIX_LOG("[matrix_hadamard_product] Error: NULL input matrix.");
        return NULL;
    }
    if (A->rows != B->rows || A->cols != B->cols) {
        MATRIX_LOG("[matrix_hadamard_product] Error: dimension mismatch (%zux%zu vs %zux%zu).",
                   A->rows, A->cols, B->rows, B->cols);
        return NULL;
    }

    Matrix* result = matrix_create(A->rows, A->cols);
    if (!result) {
        MATRIX_LOG("[matrix_hadamard_product] Error: allocation failed.");
        return NULL;
    }

    size_t total = A->rows * A->cols;
    for (size_t i = 0; i < total; ++i) {
        result->data[i] = A->data[i] * B->data[i];
    }

    MATRIX_LOG("[matrix_hadamard_product] Success.");
    return result;
}


/**
 * @brief Solves the linear system A x = b for x.
 *
 * Solves a square system using Gaussian elimination with partial pivoting,
 * followed by back substitution — the numerically sound, general-purpose
 * approach (no explicit inverse is formed). @p b may have more than one
 * column, in which case every column is solved at once (i.e. it solves
 * `A X = B` for an n x k right-hand side, like numpy's `linalg.solve`).
 *
 * @param A The coefficient matrix. Must be square (n x n) and non-singular.
 * @param b The right-hand side. Must have n rows and one or more columns.
 *
 * @return A new n x k matrix holding the solution x (X), or NULL if either
 *         input is NULL, @p A is not square, the dimensions are incompatible,
 *         @p A is singular (no unique solution), or allocation fails. The
 *         caller owns the result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_solve(const Matrix* A, const Matrix* b) {
    MATRIX_LOG("[matrix_solve] Entering function.");
    if (!A || !b || !A->data || !b->data) {
        MATRIX_LOG("[matrix_solve] Error: NULL input matrix.");
        return NULL;
    }
    if (A->rows != A->cols || A->rows == 0) {
        MATRIX_LOG("[matrix_solve] Error: A must be a non-empty square matrix.");
        return NULL;
    }
    size_t n = A->rows;
    if (b->rows != n || b->cols == 0) {
        MATRIX_LOG("[matrix_solve] Error: b must have %zu rows and >= 1 column.", n);
        return NULL;
    }
    size_t k = b->cols;

    /* Working copies: M (n x n) is reduced in place; X (n x k) tracks the
       same row operations applied to the right-hand side. */
    double* M = (double*)malloc(n * n * sizeof(double));
    double* X = (double*)malloc(n * k * sizeof(double));
    if (!M || !X) {
        MATRIX_LOG("[matrix_solve] Error: allocation failed.");
        free(M);
        free(X);
        
        return NULL;
    }
    memcpy(M, A->data, n * n * sizeof(double));
    memcpy(X, b->data, n * k * sizeof(double));

    /* Singular when a pivot is at the rounding level of A's own entries; the
       old absolute 1e-9 test rejected well-conditioned small-scale systems
       such as 1e-10 * I. */
    double scale = 0.0;
    for (size_t i = 0; i < n * n; ++i) {
        if (fabs(M[i]) > scale) {
            scale = fabs(M[i]);
        }
    }
    double tol = (double)n * DBL_EPSILON * scale;

    /* Forward elimination with partial pivoting. */
    for (size_t col = 0; col < n; ++col) {
        size_t pivot = col;
        double maxVal = fabs(M[col * n + col]);

        for (size_t r = col + 1; r < n; ++r) {
            double v = fabs(M[r * n + col]);
            if (v > maxVal) {
                maxVal = v;
                pivot = r;
            }
        }
        if (!(maxVal > tol)) {
            MATRIX_LOG("[matrix_solve] Error: A is singular (no unique solution).");
            free(M);
            free(X);
            return NULL;
        }
        if (pivot != col) {
            for (size_t c = 0; c < n; ++c) {
                double t = M[pivot * n + c];

                M[pivot * n + c] = M[col * n + c];
                M[col * n + c] = t;
            }
            for (size_t c = 0; c < k; ++c) {
                double t = X[pivot * k + c];

                X[pivot * k + c] = X[col * k + c];
                X[col * k + c] = t;
            }
        }

        double pivotVal = M[col * n + col];
        for (size_t r = col + 1; r < n; ++r) {
            double factor = M[r * n + col] / pivotVal;
            if (factor != 0.0) {
                for (size_t c = col; c < n; ++c) {
                    M[r * n + c] -= factor * M[col * n + c];
                }
                for (size_t c = 0; c < k; ++c) {
                    X[r * k + c] -= factor * X[col * k + c];
                }
            }
        }
    }

    /* Back substitution into the result matrix. */
    Matrix* result = matrix_create(n, k);
    if (!result) {
        MATRIX_LOG("[matrix_solve] Error: allocation failed for result.");
        free(M);
        free(X);

        return NULL;
    }
    for (size_t rhs = 0; rhs < k; ++rhs) {
        for (size_t i = n; i-- > 0; ) {
            double sum = X[i * k + rhs];
            for (size_t c = i + 1; c < n; ++c) {
                sum -= M[i * n + c] * result->data[c * k + rhs];
            }
            result->data[i * k + rhs] = sum / M[i * n + i];
        }
    }

    free(M);
    free(X);

    MATRIX_LOG("[matrix_solve] Success: solved %zu x %zu system with %zu RHS column(s).", n, n, k);
    return result;
}

/**
 * @brief Returns the per-axis index of the minimum element.
 *
 * The C analogue of NumPy's `numpy.argmin(M, axis=...)`: `axis == 0` returns, for
 * each column, the **row** index of its minimum (`1 x cols`); `axis == 1`
 * returns, for each row, the **column** index of its minimum (`rows x 1`). On
 * ties the first occurrence wins. Indices are stored as `double`.
 *
 * @param matrix The matrix to search. Must not be NULL.
 * @param axis   `0` for column-wise (row) indices, `1` for row-wise (column).
 *
 * @return A new vector of indices, or `NULL` if `matrix` is NULL, `axis` is
 *         neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_argmin_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_argmin_axis] Entering function with axis = %d", axis);
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) { return NULL; }
        for (size_t j = 0; j < C; ++j) {
            size_t best = 0;
            double m = matrix->data[j];
            for (size_t i = 1; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (!isnan(m) && (isnan(v) || v < m)) { m = v; best = i; }
            }
            result->data[j] = (double)best;
        }
        return result;
    }
    Matrix* result = matrix_create(R, 1);
    if (!result) { return NULL; }
    for (size_t i = 0; i < R; ++i) {
        size_t best = 0;
        double m = matrix->data[i * C];
        for (size_t j = 1; j < C; ++j) {
            double v = matrix->data[i * C + j];
            if (!isnan(m) && (isnan(v) || v < m)) { m = v; best = j; }
        }
        result->data[i] = (double)best;
    }
    return result;
}

/**
 * @brief Returns the per-axis peak-to-peak range (max - min).
 *
 * The C analogue of NumPy's `numpy.ptp(M, axis=...)`: `axis == 0` gives the
 * range of each column (`1 x cols`); `axis == 1` the range of each row
 * (`rows x 1`).
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column ranges, `1` for row ranges.
 *
 * @return A new vector of per-axis ranges, or `NULL` if `matrix` is NULL, `axis`
 *         is neither 0 nor 1, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_ptp_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_ptp_axis] Entering function with axis = %d", axis);
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) { return NULL; }
        for (size_t j = 0; j < C; ++j) {
            double mn = matrix->data[j], mx = matrix->data[j];
            for (size_t i = 1; i < R; ++i) {
                double v = matrix->data[i * C + j];
                if (!isnan(mn) && (isnan(v) || v < mn)) { mn = v; }   /* NaN sticks (NumPy) */
                if (!isnan(mx) && (isnan(v) || v > mx)) { mx = v; }
            }
            result->data[j] = mx - mn;
        }
        return result;
    }
    Matrix* result = matrix_create(R, 1);
    if (!result) { return NULL; }
    for (size_t i = 0; i < R; ++i) {
        double mn = matrix->data[i * C], mx = matrix->data[i * C];
        for (size_t j = 1; j < C; ++j) {
            double v = matrix->data[i * C + j];
            if (!isnan(mn) && (isnan(v) || v < mn)) { mn = v; }   /* NaN sticks (NumPy) */
            if (!isnan(mx) && (isnan(v) || v > mx)) { mx = v; }
        }
        result->data[i] = mx - mn;
    }
    return result;
}

/**
 * @brief Returns the per-axis median.
 *
 * The C analogue of NumPy's `numpy.median(M, axis=...)`: `axis == 0` gives the
 * median of each column (`1 x cols`); `axis == 1` the median of each row
 * (`rows x 1`). The input is not modified.
 *
 * @param matrix The matrix to reduce. Must not be NULL.
 * @param axis   `0` for column medians, `1` for row medians.
 *
 * @return A new vector of per-axis medians, or `NULL` if `matrix` is NULL,
 *         `axis` is neither 0 nor 1, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_median_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_median_axis] Entering function with axis = %d", axis);
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    size_t len = (axis == 0) ? R : C;
    double* buf = (double*)malloc(len * sizeof(double));
    if (!buf) { return NULL; }

    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) { free(buf); return NULL; }
        for (size_t j = 0; j < C; ++j) {
            for (size_t i = 0; i < R; ++i) { buf[i] = matrix->data[i * C + j]; }
            qsort(buf, R, sizeof(double), mat_cmp_asc);
            result->data[j] = mat_pct_sorted(buf, R, 50.0);
        }
        free(buf);
        return result;
    }
    Matrix* result = matrix_create(R, 1);
    if (!result) { free(buf); return NULL; }
    for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) { buf[j] = matrix->data[i * C + j]; }
        qsort(buf, C, sizeof(double), mat_cmp_asc);
        result->data[i] = mat_pct_sorted(buf, C, 50.0);
    }
    free(buf);
    return result;
}

/**
 * @brief Counts non-zero elements along an axis.
 *
 * The C analogue of NumPy's `numpy.count_nonzero(M, axis=...)`: `axis == 0`
 * counts down each column (`1 x cols`); `axis == 1` across each row (`rows x 1`).
 * As in NumPy, `-0.0` counts as zero while `nan`/`inf` count as non-zero.
 *
 * @param matrix The matrix to inspect. Must not be NULL.
 * @param axis   `0` for column counts, `1` for row counts.
 *
 * @return A new vector of counts (as `double`), or `NULL` if `matrix` is NULL,
 *         `axis` is neither 0 nor 1, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_count_nonzero_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_count_nonzero_axis] Entering function with axis = %d", axis);
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    if (axis == 0) {
        Matrix* result = matrix_create(1, C);
        if (!result) { return NULL; }
        for (size_t j = 0; j < C; ++j) {
            size_t cnt = 0;
            for (size_t i = 0; i < R; ++i) {
                if (matrix->data[i * C + j] != 0.0) { ++cnt; }
            }
            result->data[j] = (double)cnt;
        }
        return result;
    }
    Matrix* result = matrix_create(R, 1);
    if (!result) { return NULL; }
    for (size_t i = 0; i < R; ++i) {
        size_t cnt = 0;
        for (size_t j = 0; j < C; ++j) {
            if (matrix->data[i * C + j] != 0.0) { ++cnt; }
        }
        result->data[i] = (double)cnt;
    }
    return result;
}

/**
 * @brief Returns the (row, column) indices of the non-zero elements.
 *
 * The C analogue of NumPy's `numpy.nonzero(M)`: scans in row-major (C) order and
 * returns a `k x 2` matrix whose rows are the `(row, col)` index pairs of the `k`
 * non-zero elements. The input is not modified.
 *
 * @param matrix The matrix to inspect. Must not be NULL.
 *
 * @return A new `k x 2` matrix of index pairs, or `NULL` if `matrix` is NULL,
 *         contains no non-zero elements, or allocation fails. The caller owns the
 *         result and must release it with `matrix_deallocate`.
 */
Matrix* matrix_nonzero(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nonzero] Entering function");
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    size_t k = 0;
    for (size_t i = 0; i < R * C; ++i) {
        if (matrix->data[i] != 0.0) { ++k; }
    }
    if (k == 0) {
        MATRIX_LOG("[matrix_nonzero] No non-zero elements.");
        return NULL;
    }
    Matrix* result = matrix_create(k, 2);
    if (!result) { return NULL; }
    size_t row = 0;
    for (size_t i = 0; i < R; ++i) {
        for (size_t j = 0; j < C; ++j) {
            if (matrix->data[i * C + j] != 0.0) {
                result->data[row * 2 + 0] = (double)i;
                result->data[row * 2 + 1] = (double)j;
                ++row;
            }
        }
    }
    return result;
}

/**
 * @brief Reverses the order of elements along one axis.
 *
 * The C analogue of NumPy's `numpy.flip(M, axis=...)`: `axis == 0` reverses the
 * row order (like `matrix_flipud`); `axis == 1` reverses the column order (like
 * `matrix_fliplr`). The runtime-selectable axis generalises the fixed
 * `flipud`/`fliplr` pair. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param axis   `0` to reverse rows, `1` to reverse columns.
 *
 * @return A new flipped matrix (same shape), or `NULL` if `matrix` is NULL,
 *         `axis` is neither 0 nor 1, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_flip(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_flip] Entering function with axis = %d", axis);
    if (!matrix || !matrix->data) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    Matrix* result = matrix_create(R, C);
    if (!result) { return NULL; }
    if (axis == 0) {
        for (size_t i = 0; i < R; ++i) {
            size_t src = R - 1 - i;
            for (size_t j = 0; j < C; ++j) {
                result->data[i * C + j] = matrix->data[src * C + j];
            }
        }
    }
    else {
        for (size_t i = 0; i < R; ++i) {
            for (size_t j = 0; j < C; ++j) {
                result->data[i * C + j] = matrix->data[i * C + (C - 1 - j)];
            }
        }
    }
    return result;
}

/**
 * @brief Gathers elements by a list of flattened indices.
 *
 * The C analogue of NumPy's `numpy.take(M, indices)`: the source matrix is
 * flattened in row-major order and the elements at the given indices are
 * gathered, producing a result of the same shape as `indices`. Negative indices
 * count from the end (like NumPy/Python). Neither input is modified.
 *
 * @param matrix  The source matrix. Must not be NULL.
 * @param indices The flat indices to gather (any shape; integer-valued doubles).
 *                Must not be NULL.
 *
 * @return A new matrix (same shape as `indices`) of gathered values, or `NULL`
 *         if either input is NULL/empty, any index is out of range, or
 *         allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_take(const Matrix* matrix, const Matrix* indices) {
    MATRIX_LOG("[matrix_take] Entering function");
    if (!matrix || !indices || !matrix->data || !indices->data) {
        return NULL;
    }
    size_t n = matrix->rows * matrix->cols;
    size_t ni = indices->rows * indices->cols;
    if (n == 0 || ni == 0) {
        return NULL;
    }
    Matrix* result = matrix_create(indices->rows, indices->cols);
    if (!result) { return NULL; }
    for (size_t t = 0; t < ni; ++t) {
        double d = indices->data[t];
        /* range-check BEFORE converting: (ptrdiff_t)NaN or a huge value is
           undefined behaviour and produced a garbage index */
        if (!(d > -(double)n - 1.0 && d < (double)n)) {
            matrix_deallocate(result);
            return NULL;
        }
        ptrdiff_t idx = (ptrdiff_t)d;
        if (idx < 0) {
            idx += (ptrdiff_t)n;   /* negative indices wrap, like NumPy */
        }
        if (idx < 0 || (size_t)idx >= n) {
            MATRIX_LOG("[matrix_take] Error: index out of range.");
            matrix_deallocate(result);
            return NULL;
        }
        result->data[t] = matrix->data[(size_t)idx];
    }
    return result;
}

/**
 * @brief Removes a single row or column from a matrix.
 *
 * The C analogue of NumPy's `numpy.delete(M, index, axis=...)`: `axis == 0`
 * removes row `index` (result `(rows-1) x cols`); `axis == 1` removes column
 * `index` (result `rows x (cols-1)`). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param index  The 0-based row or column index to remove.
 * @param axis   `0` to remove a row, `1` to remove a column.
 *
 * @return A new matrix with the row/column removed, or `NULL` if `matrix` is
 *         NULL, `axis` is neither 0 nor 1, `index` is out of range, removing
 *         would leave an empty matrix, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_delete(const Matrix* matrix, size_t index, int axis) {
    MATRIX_LOG("[matrix_delete] Entering function with index = %zu, axis = %d", index, axis);
    if (!matrix || !matrix->data) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    if (axis == 0) {
        if (index >= R || R == 1) { return NULL; }
        Matrix* result = matrix_create(R - 1, C);
        if (!result) { return NULL; }
        size_t r = 0;
        for (size_t i = 0; i < R; ++i) {
            if (i == index) { continue; }
            memcpy(&result->data[r * C], &matrix->data[i * C], C * sizeof(double));
            ++r;
        }
        return result;
    }
    if (index >= C || C == 1) { return NULL; }
    Matrix* result = matrix_create(R, C - 1);
    if (!result) { return NULL; }
    for (size_t i = 0; i < R; ++i) {
        size_t c = 0;
        for (size_t j = 0; j < C; ++j) {
            if (j == index) { continue; }
            result->data[i * (C - 1) + c] = matrix->data[i * C + j];
            ++c;
        }
    }
    return result;
}

/**
 * @brief Inserts a row or column at a given position.
 *
 * The C analogue of NumPy's `numpy.insert(M, index, values, axis=...)`:
 * `axis == 0` inserts `values` (a length-`cols` vector) as a new row before row
 * `index` (result `(rows+1) x cols`); `axis == 1` inserts `values` (a
 * length-`rows` vector) as a new column before column `index` (result
 * `rows x (cols+1)`). `index` may equal the axis length to append at the end.
 * Neither input is modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param index  The 0-based position before which to insert (`0..axis-length`).
 * @param values The vector to insert (length must match the other dimension).
 *               Must not be NULL.
 * @param axis   `0` to insert a row, `1` to insert a column.
 *
 * @return A new matrix with the row/column inserted, or `NULL` if either input
 *         is NULL, `axis` is invalid, `index` is out of range, `values` has the
 *         wrong length, or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_insert(const Matrix* matrix, size_t index, const Matrix* values, int axis) {
    MATRIX_LOG("[matrix_insert] Entering function with index = %zu, axis = %d", index, axis);
    if (!matrix || !values || !matrix->data || !values->data) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    size_t R = matrix->rows, C = matrix->cols;
    size_t vlen = values->rows * values->cols;

    if (axis == 0) {
        if (index > R || vlen != C) { return NULL; }
        Matrix* result = matrix_create(R + 1, C);
        if (!result) { return NULL; }
        size_t r = 0;
        for (size_t i = 0; i <= R; ++i) {
            if (i == index) {
                memcpy(&result->data[r * C], values->data, C * sizeof(double));
                ++r;
            }
            if (i < R) {
                memcpy(&result->data[r * C], &matrix->data[i * C], C * sizeof(double));
                ++r;
            }
        }
        return result;
    }
    if (index > C || vlen != R) { return NULL; }
    Matrix* result = matrix_create(R, C + 1);
    if (!result) { return NULL; }
    for (size_t i = 0; i < R; ++i) {
        size_t c = 0;
        for (size_t j = 0; j <= C; ++j) {
            if (j == index) {
                result->data[i * (C + 1) + c] = values->data[i];
                ++c;
            }
            if (j < C) {
                result->data[i * (C + 1) + c] = matrix->data[i * C + j];
                ++c;
            }
        }
    }
    return result;
}

/**
 * @brief Appends rows or columns to the end of a matrix.
 *
 * The C analogue of NumPy's `numpy.append(M, values, axis=...)`: `axis == 0`
 * appends `values` (which must have `cols` columns) below `M`; `axis == 1`
 * appends `values` (which must have `rows` rows) to the right of `M`. This is
 * equivalent to `matrix_vstack` / `matrix_hstack`. Neither input is modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param values The matrix to append (matching dimensions on the other axis).
 *               Must not be NULL.
 * @param axis   `0` to append rows, `1` to append columns.
 *
 * @return A new combined matrix, or `NULL` if either input is NULL, `axis` is
 *         invalid, the dimensions are incompatible, or allocation fails. Free it
 *         with `matrix_deallocate`.
 */
Matrix* matrix_append(const Matrix* matrix, const Matrix* values, int axis) {
    MATRIX_LOG("[matrix_append] Entering function with axis = %d", axis);
    if (!matrix || !values) {
        return NULL;
    }
    if (axis == 0) {
        return matrix_vstack(matrix, values);
    }
    if (axis == 1) {
        return matrix_hstack(matrix, values);
    }
    return NULL;
}

/**
 * @brief Stacks two 1-D vectors as the columns of a 2-D matrix.
 *
 * The C analogue of NumPy's `numpy.column_stack((a, b))` for 1-D inputs: given
 * two length-`n` vectors, returns an `n x 2` matrix whose first column is `a` and
 * second column is `b`. Neither input is modified.
 *
 * @param a The first vector (any shape; flattened). Must not be NULL.
 * @param b The second vector, same length as `a`. Must not be NULL.
 *
 * @return A new `n x 2` matrix, or `NULL` if either input is NULL, the lengths
 *         differ, a vector is empty, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_column_stack(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_column_stack] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    size_t na = a->rows * a->cols;
    size_t nb = b->rows * b->cols;
    if (na == 0 || na != nb) {
        return NULL;
    }
    Matrix* result = matrix_create(na, 2);
    if (!result) { return NULL; }
    for (size_t i = 0; i < na; ++i) {
        result->data[i * 2 + 0] = a->data[i];
        result->data[i * 2 + 1] = b->data[i];
    }
    return result;
}

/**
 * @brief Returns the indices of the bins to which each value belongs.
 *
 * The C analogue of NumPy's `numpy.digitize(x, bins)` with the default
 * `right=False` and monotonically increasing `bins`: each result element is the
 * number of bin edges `<= x` — i.e. the index `i` such that
 * `bins[i-1] <= x < bins[i]` (with `0` for values below all edges and
 * `len(bins)` for values at or above the last edge). The result has the same
 * shape as `x`. Neither input is modified.
 *
 * @param x    The values to bin (any shape). Must not be NULL.
 * @param bins The sorted (ascending) bin edges (any shape; flattened). Must not
 *             be NULL.
 *
 * @return A new matrix (same shape as `x`) of bin indices (as `double`), or
 *         `NULL` if either input is NULL/empty or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_digitize(const Matrix* x, const Matrix* bins) {
    MATRIX_LOG("[matrix_digitize] Entering function");
    if (!x || !bins || !x->data || !bins->data) {
        return NULL;
    }
    size_t nx = x->rows * x->cols;
    size_t nb = bins->rows * bins->cols;
    if (nx == 0 || nb == 0) {
        return NULL;
    }
    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) { return NULL; }
    for (size_t t = 0; t < nx; ++t) {
        double v = x->data[t];
        /* upper bound: first index where bins[mid] > v -> count of bins <= v */
        size_t lo = 0, hi = nb;
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (isnan(v) || bins->data[mid] <= v) {   /* NaN: past every bin, like NumPy */
                lo = mid + 1;
            }
            else {
                hi = mid;
            }
        }
        result->data[t] = (double)lo;
    }
    return result;
}

/**
 * @brief Counts occurrences of each non-negative integer.
 *
 * The C analogue of NumPy's `numpy.bincount(x)`: treats the (flattened) elements
 * of `x` as non-negative integers and returns a `1 x (max+1)` row vector whose
 * `i`-th entry is the number of times `i` appears. The input is not modified.
 *
 * @param x The non-negative integer-valued matrix. Must not be NULL.
 *
 * @return A new `1 x (max+1)` row vector of counts, or `NULL` if `x` is NULL,
 *         empty, contains a negative value, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_bincount(const Matrix* x) {
    MATRIX_LOG("[matrix_bincount] Entering function");
    if (!x || !x->data || x->rows == 0 || x->cols == 0) {
        return NULL;
    }
    size_t n = x->rows * x->cols;
    size_t maxv = 0;
    for (size_t i = 0; i < n; ++i) {
        /* NaN fails every comparison, so test the accepted range positively:
           a NaN or infinite value must be rejected before the size_t cast
           (that cast is undefined behavior and used to yield a huge index). */
        if (!(x->data[i] >= 0.0)) {
            MATRIX_LOG("[matrix_bincount] Error: negative or NaN value not allowed.");
            return NULL;
        }
        if (!(x->data[i] < (double)SIZE_MAX)) {
            MATRIX_LOG("[matrix_bincount] Error: value too large (or infinite) to count.");
            return NULL;
        }
        size_t v = (size_t)x->data[i];
        if (v > maxv) { maxv = v; }
    }
    Matrix* result = matrix_create(1, maxv + 1);   /* zero-filled */
    if (!result) { return NULL; }
    for (size_t i = 0; i < n; ++i) {
        size_t v = (size_t)x->data[i];
        result->data[v] += 1.0;
    }
    return result;
}

/**
 * @brief Computes the histogram of a set of values.
 *
 * The C analogue of NumPy's `numpy.histogram(x, bins)`: divides the range
 * `[min(x), max(x)]` into `bins` equal-width buckets, counts how many values fall
 * in each, and reports both the counts and the bin edges. The rightmost edge is
 * inclusive (values equal to `max(x)` land in the last bin). The input is not
 * modified.
 *
 * @param x     The values to histogram (any shape; flattened). Must not be NULL.
 * @param bins  The number of equal-width bins. Must be greater than 0.
 * @param hist  Out-parameter: receives a `1 x bins` row vector of counts.
 * @param edges Out-parameter: receives a `1 x (bins+1)` row vector of bin edges.
 *
 * @return `true` on success; `false` if `x` is NULL/empty, `bins` is 0, an
 *         out-pointer is NULL, or allocation fails. On success the caller owns
 *         `*hist` and `*edges` and must release each with `matrix_deallocate`.
 */
bool matrix_histogram(const Matrix* x, size_t bins, Matrix** hist, Matrix** edges) {
    MATRIX_LOG("[matrix_histogram] Entering function with bins = %zu", bins);
    if (!x || !x->data || x->rows == 0 || x->cols == 0 || bins == 0 || !hist || !edges) {
        return false;
    }
    size_t n = x->rows * x->cols;
    for (size_t i = 0; i < n; ++i) {
        if (!isfinite(x->data[i])) {
            return false;   /* NumPy rejects a non-finite range; NaN used to be counted in bin 0 */
        }
    }
    double mn = x->data[0], mx = x->data[0];
    for (size_t i = 1; i < n; ++i) {
        double v = x->data[i];
        if (v < mn) { mn = v; }
        if (v > mx) { mx = v; }
    }
    /* degenerate range: NumPy uses [min-0.5, max+0.5] */
    if (mn == mx) {
        mn -= 0.5;
        mx += 0.5;
    }
    double width = (mx - mn) / (double)bins;

    Matrix* H = matrix_create(1, bins);            /* zero-filled */
    Matrix* E = matrix_create(1, bins + 1);
    if (!H || !E) {
        if (H) { matrix_deallocate(H); }
        if (E) { matrix_deallocate(E); }
        return false;
    }
    for (size_t k = 0; k <= bins; ++k) {
        E->data[k] = mn + (double)k * width;
    }
    for (size_t i = 0; i < n; ++i) {
        double pos = (x->data[i] - mn) / width;
        ptrdiff_t b = (ptrdiff_t)pos;
        if (b < 0) { b = 0; }
        if ((size_t)b >= bins) { b = (ptrdiff_t)bins - 1; }   /* incl. the right edge */
        H->data[b] += 1.0;
    }

    *hist = H;
    *edges = E;
    MATRIX_LOG("[matrix_histogram] Success: histogram computed.");
    return true;
}

/**
 * @brief Computes the normalized sinc of every element.
 *
 * The C analogue of NumPy's `numpy.sinc(x)`: the normalized cardinal sine
 * `sin(pi*x) / (pi*x)`, with the removable singularity filled exactly as
 * `sinc(0) = 1`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix (same shape) of sinc values, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_sinc(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sinc] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    const double pi = 3.14159265358979323846;
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (x == 0.0) {
            result->data[i] = 1.0;
        }
        else {
            double px = pi * x;
            result->data[i] = sin(px) / px;
        }
    }
    return result;
}

/**
 * @brief Rounds every element to a given number of decimal places.
 *
 * The C analogue of NumPy's `numpy.around(x, decimals)`: scales by
 * `10**decimals`, rounds to the nearest integer using round-half-to-even
 * (banker's rounding), then unscales — exactly matching NumPy (so e.g. a value
 * that lands on `.5` after scaling rounds to the nearest even). This differs
 * from `matrix_round`, which rounds halves away from zero at zero decimals.
 * Negative `decimals` round to the left of the decimal point (tens, hundreds).
 * The input is not modified.
 *
 * @param matrix   The source matrix. Must not be NULL.
 * @param decimals The number of decimal places to round to (may be negative).
 *
 * @return A new matrix (same shape) of rounded values, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_around(const Matrix* matrix, int decimals) {
    MATRIX_LOG("[matrix_around] Entering function with decimals = %d", decimals);
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    int ad = decimals < 0 ? -decimals : decimals;
    double scale = 1.0;
    for (int i = 0; i < ad; ++i) { scale *= 10.0; }
    double p = (decimals < 0) ? (1.0 / scale) : scale;   /* == 10**decimals */
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = rint(matrix->data[i] * p) / p;
    }
    return result;
}

/**
 * @brief Computes the relative exponential of every element.
 *
 * The C analogue of SciPy's `scipy.special.exprel(x)`: `(exp(x) - 1) / x`, with
 * the removable singularity filled exactly as `exprel(0) = 1`. Uses `expm1` so
 * the result stays accurate for `x` near zero. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix (same shape) of exprel values, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_exprel(const Matrix* matrix) {
    MATRIX_LOG("[matrix_exprel] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        result->data[i] = (x == 0.0) ? 1.0 : expm1(x) / x;
    }
    return result;
}

/* True if v converts to long long without undefined behaviour (finite and
   |v| < 2^63, so negating the result cannot overflow either). */
static bool mat_ll_ok(double v) {
    return isfinite(v) && fabs(v) < 9223372036854775808.0;
}

/* Euclidean GCD of two (already integer-valued) magnitudes. */
static long long mat_gcd_ll(long long a, long long b) {
    if (a < 0) { a = -a; }
    if (b < 0) { b = -b; }
    while (b != 0) {
        long long t = a % b;
        a = b;
        b = t;
    }
    return a;
}

/**
 * @brief Computes the element-wise greatest common divisor.
 *
 * The C analogue of NumPy's `numpy.gcd(a, b)`: each element is treated as an
 * integer and the result is the non-negative greatest common divisor of the two
 * magnitudes (`gcd(0, 0) == 0`). Both inputs must have the same shape and neither
 * is modified.
 *
 * @param a The first integer-valued matrix. Must not be NULL.
 * @param b The second integer-valued matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of GCDs, or `NULL` if either input is NULL,
 *         the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_gcd(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_gcd] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        if (!mat_ll_ok(a->data[i]) || !mat_ll_ok(b->data[i])) {
            result->data[i] = NAN;   /* NaN / inf / out of range: no integer */
            continue;
        }
        long long x = (long long)a->data[i];
        long long y = (long long)b->data[i];
        result->data[i] = (double)mat_gcd_ll(x, y);
    }
    return result;
}

/**
 * @brief Computes the element-wise least common multiple.
 *
 * The C analogue of NumPy's `numpy.lcm(a, b)`: each element is treated as an
 * integer and the result is the non-negative least common multiple of the two
 * magnitudes, with `lcm(0, x) == 0`. Computed as `|a / gcd(a, b) * b|` to avoid
 * intermediate overflow. Both inputs must have the same shape and neither is
 * modified.
 *
 * @param a The first integer-valued matrix. Must not be NULL.
 * @param b The second integer-valued matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of LCMs, or `NULL` if either input is NULL,
 *         the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_lcm(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_lcm] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        if (!mat_ll_ok(a->data[i]) || !mat_ll_ok(b->data[i])) {
            result->data[i] = NAN;
            continue;
        }
        long long x = (long long)a->data[i];
        long long y = (long long)b->data[i];
        long long g = mat_gcd_ll(x, y);
        if (g == 0) {
            result->data[i] = 0.0;
        }
        else {
            long long q  = x / g;
            long long aq = q < 0 ? -q : q;
            long long ay = y < 0 ? -y : y;
            if (ay != 0 && aq > LLONG_MAX / ay) {
                /* the exact value exceeds long long (it used to overflow, UB):
                   return the nearest double instead */
                result->data[i] = (double)aq * (double)ay;
            }
            else {
                result->data[i] = (double)(aq * ay);
            }
        }
    }
    return result;
}

/**
 * @brief Computes the element-wise log of summed exponentials.
 *
 * The C analogue of NumPy's `numpy.logaddexp(a, b)`: `log(exp(a) + exp(b))`
 * evaluated in the numerically stable form `max + log1p(exp(min - max))`, which
 * avoids overflow when the inputs are large. Both inputs must have the same
 * shape and neither is modified.
 *
 * @param a The first matrix. Must not be NULL.
 * @param b The second matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of results, or `NULL` if either input is
 *         NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_logaddexp(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_logaddexp] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = a->data[i];
        double y = b->data[i];
        if (x == y) {
            /* covers the +/-inf diagonal exactly (inf+log2=inf, -inf stays -inf) */
            result->data[i] = x + log(2.0);
        }
        else {
            double mx = (x > y) ? x : y;
            double mn = (x > y) ? y : x;
            result->data[i] = mx + log1p(exp(mn - mx));
        }
    }
    return result;
}

/**
 * @brief Element-wise NaN-ignoring maximum of two matrices.
 *
 * The C analogue of NumPy's `numpy.fmax(a, b)`: returns the larger of each pair,
 * but when exactly one operand is NaN it returns the other (non-NaN) operand;
 * the result is NaN only where both are NaN. This is unlike `matrix_maximum`,
 * which propagates NaN. Both inputs must have the same shape and neither is
 * modified.
 *
 * @param a The first matrix. Must not be NULL.
 * @param b The second matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of element-wise maxima, or `NULL` if either
 *         input is NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_fmax(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_fmax] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = fmax(a->data[i], b->data[i]);
    }
    return result;
}

/**
 * @brief Element-wise NaN-ignoring minimum of two matrices.
 *
 * The C analogue of NumPy's `numpy.fmin(a, b)`: returns the smaller of each
 * pair, but when exactly one operand is NaN it returns the other (non-NaN)
 * operand; the result is NaN only where both are NaN. This is unlike
 * `matrix_minimum`, which propagates NaN. Both inputs must have the same shape
 * and neither is modified.
 *
 * @param a The first matrix. Must not be NULL.
 * @param b The second matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of element-wise minima, or `NULL` if either
 *         input is NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_fmin(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_fmin] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = fmin(a->data[i], b->data[i]);
    }
    return result;
}

/**
 * @brief Splits every element into its fractional and integer parts.
 *
 * The C analogue of NumPy's `numpy.modf(x)`: each element `x` is decomposed so
 * that `frac + int == x`, with both parts carrying the sign of `x` (e.g.
 * `-2.5 -> frac -0.5, int -2.0`). The two results have the same shape as the
 * input, which is not modified.
 *
 * @param matrix    The source matrix. Must not be NULL.
 * @param frac_part Out-parameter receiving the fractional parts.
 * @param int_part  Out-parameter receiving the (truncated-toward-zero) integer
 *                  parts, as real values.
 *
 * @return `true` on success; `false` if `matrix` is NULL, an out-pointer is
 *         NULL, or allocation fails. On success the caller owns `*frac_part` and
 *         `*int_part` and must release each with `matrix_deallocate`.
 */
bool matrix_modf(const Matrix* matrix, Matrix** frac_part, Matrix** int_part) {
    MATRIX_LOG("[matrix_modf] Entering function");
    if (!matrix || !matrix->data || !frac_part || !int_part) {
        return false;
    }
    Matrix* fp = matrix_create(matrix->rows, matrix->cols);
    Matrix* ip = matrix_create(matrix->rows, matrix->cols);
    if (!fp || !ip) {
        if (fp) { matrix_deallocate(fp); }
        if (ip) { matrix_deallocate(ip); }
        return false;
    }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double ipart;
        double fpart = modf(matrix->data[i], &ipart);
        fp->data[i] = fpart;
        ip->data[i] = ipart;
    }
    *frac_part = fp;
    *int_part = ip;
    return true;
}

/**
 * @brief Computes the simultaneous element-wise quotient and remainder.
 *
 * The C analogue of NumPy's `numpy.divmod(a, b)`: for each pair it returns the
 * floor-division quotient `floor(a / b)` and the remainder `a - quotient * b`
 * (which carries the sign of the divisor, like `numpy.mod`). Both inputs must
 * have the same shape and neither is modified.
 *
 * @param a         The dividend matrix. Must not be NULL.
 * @param b         The divisor matrix, same shape as `a`. Must not be NULL.
 * @param quotient  Out-parameter receiving the floor-division quotients.
 * @param remainder Out-parameter receiving the remainders.
 *
 * @return `true` on success; `false` if either input is NULL, the shapes differ,
 *         an out-pointer is NULL, or allocation fails. On success the caller owns
 *         `*quotient` and `*remainder` and must release each with
 *         `matrix_deallocate`.
 */
bool matrix_divmod(const Matrix* a, const Matrix* b, Matrix** quotient, Matrix** remainder) {
    MATRIX_LOG("[matrix_divmod] Entering function");
    if (!a || !b || !a->data || !b->data || !quotient || !remainder) {
        return false;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return false;
    }
    Matrix* q = matrix_create(a->rows, a->cols);
    Matrix* r = matrix_create(a->rows, a->cols);
    if (!q || !r) {
        if (q) { matrix_deallocate(q); }
        if (r) { matrix_deallocate(r); }
        return false;
    }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        double quo = floor(a->data[i] / b->data[i]);
        q->data[i] = quo;
        r->data[i] = a->data[i] - quo * b->data[i];
    }
    *quotient = q;
    *remainder = r;
    return true;
}

/**
 * @brief Computes the element-wise floor of the division.
 *
 * The C analogue of NumPy's `numpy.floor_divide(a, b)`: `floor(a / b)`,
 * element-wise. The result follows NumPy's sign and division-by-zero semantics
 * exactly (e.g. `floor_divide(-7, 3) == -3`, and dividing a finite value by zero
 * yields `+inf`, `-inf`, or `nan` just as `floor(a / b)` does). Both inputs must
 * have the same shape and neither is modified.
 *
 * @param a The dividend matrix. Must not be NULL.
 * @param b The divisor matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of floor-divided values, or `NULL` if either
 *         input is NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_floor_divide(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_floor_divide] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = floor(a->data[i] / b->data[i]);
    }
    return result;
}

/**
 * @brief Rounds every element toward zero to the nearest integer.
 *
 * The C analogue of NumPy's `numpy.fix(x)`: truncates toward zero (floor for
 * positive values, ceil for negative ones). This is behaviourally identical to
 * `matrix_trunc`; it exists to mirror the NumPy name. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix (same shape) of truncated values, or `NULL` if `matrix`
 *         is NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_fix(const Matrix* matrix) {
    MATRIX_LOG("[matrix_fix] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = trunc(matrix->data[i]);
    }
    return result;
}

/**
 * @brief Rounds every element to the nearest integer, ties to even.
 *
 * The C analogue of NumPy's `numpy.rint(x)`: rounds to the nearest integer using
 * round-half-to-even (banker's rounding), so e.g. `0.5 -> 0`, `1.5 -> 2`,
 * `2.5 -> 2`. This differs from `matrix_round`, which rounds halves away from
 * zero. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix (same shape) of rounded values, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_rint(const Matrix* matrix) {
    MATRIX_LOG("[matrix_rint] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = rint(matrix->data[i]);
    }
    return result;
}

/**
 * @brief Replaces NaN and infinities with finite numbers.
 *
 * The C analogue of NumPy's `numpy.nan_to_num(x)` with default arguments: `NaN`
 * becomes `0.0`, `+inf` becomes the largest finite double (`DBL_MAX`), and
 * `-inf` becomes `-DBL_MAX`. Finite values pass through unchanged. The input is
 * not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new finite matrix (same shape), or `NULL` if `matrix` is NULL or
 *         allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_nan_to_num(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nan_to_num] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (isnan(x)) {
            result->data[i] = 0.0;
        }
        else if (isinf(x)) {
            result->data[i] = (x > 0.0) ? DBL_MAX : -DBL_MAX;
        }
        else {
            result->data[i] = x;
        }
    }
    return result;
}

/**
 * @brief Computes the element-wise power in floating point.
 *
 * The C analogue of NumPy's `numpy.float_power(a, b)`: `a ** b` computed in
 * double precision with the usual `pow` edge cases (e.g. a negative base with a
 * non-integer exponent yields `nan`). Because this library is already
 * double-valued, it is equivalent to `matrix_pow`; it exists to mirror the NumPy
 * name. Both inputs must have the same shape and neither is modified.
 *
 * @param a The base matrix. Must not be NULL.
 * @param b The exponent matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new matrix (same shape) of powers, or `NULL` if either input is
 *         NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_float_power(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_float_power] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = pow(a->data[i], b->data[i]);
    }
    return result;
}

/**
 * @brief Computes the element-wise logistic sigmoid.
 *
 * The C analogue of SciPy's `scipy.special.expit(x)`: `1 / (1 + exp(-x))`,
 * element-wise, mapping the real line onto `(0, 1)`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix (same shape) of sigmoid values, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_expit(const Matrix* matrix) {
    MATRIX_LOG("[matrix_expit] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = 1.0 / (1.0 + exp(-matrix->data[i]));
    }
    return result;
}

/**
 * @brief Computes the element-wise logit (inverse sigmoid).
 *
 * The C analogue of SciPy's `scipy.special.logit(p)`: `log(p / (1 - p))`,
 * element-wise — the inverse of `matrix_expit`. As in SciPy, `p == 0` yields
 * `-inf`, `p == 1` yields `+inf`, and `p` outside `[0, 1]` yields `nan`. The
 * input is not modified.
 *
 * @param matrix The source matrix of probabilities. Must not be NULL.
 *
 * @return A new matrix (same shape) of logit values, or `NULL` if `matrix` is
 *         NULL or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_logit(const Matrix* matrix) {
    MATRIX_LOG("[matrix_logit] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double p = matrix->data[i];
        result->data[i] = log(p / (1.0 - p));
    }
    return result;
}

/**
 * @brief Computes the element-wise `x * log(y)` with the `x == 0` convention.
 *
 * The C analogue of SciPy's `scipy.special.xlogy(x, y)`: `x * log(y)`, but
 * defined to be exactly `0` wherever `x == 0` (even if `y` is `0` or negative),
 * which avoids the `0 * (-inf) = nan` that the naive product would produce. Both
 * inputs must have the same shape and neither is modified.
 *
 * @param x The multiplier matrix. Must not be NULL.
 * @param y The matrix whose logarithm is taken, same shape as `x`. Must not be
 *          NULL.
 *
 * @return A new matrix (same shape) of results, or `NULL` if either input is
 *         NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_xlogy(const Matrix* x, const Matrix* y) {
    MATRIX_LOG("[matrix_xlogy] Entering function");
    if (!x || !y || !x->data || !y->data) {
        return NULL;
    }
    if (x->rows != y->rows || x->cols != y->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) { return NULL; }
    size_t n = x->rows * x->cols;
    for (size_t i = 0; i < n; ++i) {
        double xv = x->data[i];
        result->data[i] = (xv == 0.0) ? 0.0 : xv * log(y->data[i]);
    }
    return result;
}

/**
 * @brief Computes the element-wise Heaviside step function.
 *
 * The C analogue of NumPy's `numpy.heaviside(x, h0)`: `0` where `x < 0`, `1`
 * where `x > 0`, and the corresponding element of `h0` where `x == 0`
 * (`NaN` inputs propagate to `NaN`). The two inputs must have the same shape and
 * neither is modified.
 *
 * @param x  The step argument matrix. Must not be NULL.
 * @param h0 The values to use where `x == 0`, same shape as `x`. Must not be
 *           NULL.
 *
 * @return A new matrix (same shape) of step values, or `NULL` if either input is
 *         NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_heaviside(const Matrix* x, const Matrix* h0) {
    MATRIX_LOG("[matrix_heaviside] Entering function");
    if (!x || !h0 || !x->data || !h0->data) {
        return NULL;
    }
    if (x->rows != h0->rows || x->cols != h0->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(x->rows, x->cols);
    if (!result) { return NULL; }
    size_t n = x->rows * x->cols;
    for (size_t i = 0; i < n; ++i) {
        double v = x->data[i];
        if (v < 0.0) {
            result->data[i] = 0.0;
        }
        else if (v > 0.0) {
            result->data[i] = 1.0;
        }
        else if (v == 0.0) {
            result->data[i] = h0->data[i];
        }
        else {
            result->data[i] = v;   /* NaN propagates */
        }
    }
    return result;
}

/**
 * @brief Computes the element-wise phase angle from real and imaginary parts.
 *
 * The C analogue of NumPy's `numpy.angle(z)`: given the real and imaginary parts
 * of a complex value as two separate matrices, returns `atan2(imag, real)` — the
 * argument (phase) in radians, in the range `(-pi, pi]`. Both inputs must have
 * the same shape and neither is modified.
 *
 * @param real The real-part matrix. Must not be NULL.
 * @param imag The imaginary-part matrix, same shape as `real`. Must not be NULL.
 *
 * @return A new matrix (same shape) of phase angles, or `NULL` if either input
 *         is NULL, the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_angle(const Matrix* real, const Matrix* imag) {
    MATRIX_LOG("[matrix_angle] Entering function");
    if (!real || !imag || !real->data || !imag->data) {
        return NULL;
    }
    if (real->rows != imag->rows || real->cols != imag->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(real->rows, real->cols);
    if (!result) { return NULL; }
    size_t n = real->rows * real->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = atan2(imag->data[i], real->data[i]);
    }
    return result;
}

/**
 * @brief Computes the element-wise inequality mask.
 *
 * The C analogue of NumPy's `numpy.not_equal(a, b)`: returns `1.0` wherever the
 * corresponding elements differ and `0.0` where they are equal. Following IEEE
 * semantics, any comparison involving `NaN` is unequal, so positions where
 * either operand is `NaN` (including `NaN != NaN`) yield `1.0`. Both inputs must
 * have the same shape and neither is modified.
 *
 * @param a The first matrix. Must not be NULL.
 * @param b The second matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new 0/1 mask matrix (same shape), or `NULL` if either input is NULL,
 *         the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_not_equal(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_not_equal] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = (a->data[i] != b->data[i]) ? 1.0 : 0.0;
    }
    return result;
}

/**
 * @brief Computes the element-wise equality mask.
 *
 * The C analogue of NumPy's `numpy.equal(a, b)`: returns `1.0` wherever the
 * corresponding elements are equal and `0.0` where they differ. Following IEEE
 * semantics, any comparison involving `NaN` is unequal, so positions where
 * either operand is `NaN` (including `NaN == NaN`) yield `0.0`. This is the
 * element-wise complement of `matrix_not_equal`, and is distinct from
 * `matrix_is_equal`, which returns a single bool. Both inputs must have the same
 * shape and neither is modified.
 *
 * @param a The first matrix. Must not be NULL.
 * @param b The second matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new 0/1 mask matrix (same shape), or `NULL` if either input is NULL,
 *         the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_equal_elementwise(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_equal_elementwise] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        result->data[i] = (a->data[i] == b->data[i]) ? 1.0 : 0.0;
    }
    return result;
}

/**
 * @brief Computes the element-wise logical XOR mask.
 *
 * The C analogue of NumPy's `numpy.logical_xor(a, b)`: each element is treated
 * as a boolean (non-zero is true, so `NaN` is true) and the result is `1.0`
 * where exactly one operand is true and `0.0` otherwise. This completes the
 * boolean set alongside `matrix_logical_and`, `matrix_logical_or`, and
 * `matrix_logical_not`. Both inputs must have the same shape and neither is
 * modified.
 *
 * @param a The first matrix. Must not be NULL.
 * @param b The second matrix, same shape as `a`. Must not be NULL.
 *
 * @return A new 0/1 mask matrix (same shape), or `NULL` if either input is NULL,
 *         the shapes differ, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_logical_xor(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_logical_xor] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }
    Matrix* result = matrix_create(a->rows, a->cols);
    if (!result) { return NULL; }
    size_t n = a->rows * a->cols;
    for (size_t i = 0; i < n; ++i) {
        int ba = (a->data[i] != 0.0) ? 1 : 0;
        int bb = (b->data[i] != 0.0) ? 1 : 0;
        result->data[i] = (ba ^ bb) ? 1.0 : 0.0;
    }
    return result;
}

/**
 * @brief Tests element-wise for positive infinity.
 *
 * The C analogue of NumPy's `numpy.isposinf(x)`: returns `1.0` where an element
 * is `+inf` and `0.0` everywhere else (finite values, `-inf`, and `NaN` all map
 * to `0.0`). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 0/1 mask matrix (same shape), or `NULL` if `matrix` is NULL or
 *         allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_isposinf(const Matrix* matrix) {
    MATRIX_LOG("[matrix_isposinf] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        result->data[i] = (isinf(x) && x > 0.0) ? 1.0 : 0.0;
    }
    return result;
}

/**
 * @brief Tests element-wise for negative infinity.
 *
 * The C analogue of NumPy's `numpy.isneginf(x)`: returns `1.0` where an element
 * is `-inf` and `0.0` everywhere else (finite values, `+inf`, and `NaN` all map
 * to `0.0`). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new 0/1 mask matrix (same shape), or `NULL` if `matrix` is NULL or
 *         allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_isneginf(const Matrix* matrix) {
    MATRIX_LOG("[matrix_isneginf] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    if (!result) { return NULL; }
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        result->data[i] = (isinf(x) && x < 0.0) ? 1.0 : 0.0;
    }
    return result;
}

/**
 * @brief Tests whether two matrices are exactly equal.
 *
 * The C analogue of NumPy's `numpy.array_equal(a, b)` (with the default
 * `equal_nan=False`): returns `true` only if both matrices have the same shape
 * and every corresponding pair of elements is exactly equal. Because `NaN` is
 * never equal to itself, any matrix containing `NaN` is unequal even to itself.
 * This is behaviourally identical to `matrix_is_equal`; it exists to mirror the
 * NumPy name. Neither input is modified.
 *
 * @param a The first matrix.
 * @param b The second matrix.
 *
 * @return `true` if the matrices have identical shape and elements; `false`
 *         otherwise (including when either is NULL).
 */
bool matrix_array_equal(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_array_equal] Entering function");
    return matrix_is_equal(a, b);
}

/**
 * @brief Sums all elements, treating NaN as zero.
 *
 * The C analogue of NumPy's `numpy.nansum(x)`: adds every element but skips
 * `NaN`s (counting them as `0`). An empty or all-`NaN` matrix sums to `0.0`. The
 * input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The NaN-ignoring sum, or `0.0` if `matrix` is NULL/empty.
 */
double matrix_nansum(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nansum] Entering function");
    if (!matrix || !matrix->data) {
        return 0.0;
    }
    double sum = 0.0;
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (!isnan(x)) { sum += x; }
    }
    return sum;
}

/**
 * @brief Computes the mean over the non-NaN elements.
 *
 * The C analogue of NumPy's `numpy.nanmean(x)`: the sum of the non-`NaN`
 * elements divided by their count. If every element is `NaN` (or the matrix is
 * empty), the result is `NaN`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The NaN-ignoring mean, or `NaN` if there are no non-NaN elements.
 */
double matrix_nanmean(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nanmean] Entering function");
    if (!matrix || !matrix->data) {
        return NAN;
    }
    double sum = 0.0;
    size_t count = 0;
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (!isnan(x)) { 
            sum += x; 
            ++count; 
        }
    }
    return (count > 0) ? (sum / (double)count) : NAN;
}

/**
 * @brief Finds the minimum over the non-NaN elements.
 *
 * The C analogue of NumPy's `numpy.nanmin(x)`: the smallest element, ignoring
 * `NaN`s. If every element is `NaN` (or the matrix is empty), the result is
 * `NaN`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The NaN-ignoring minimum, or `NaN` if there are no non-NaN elements.
 */
double matrix_nanmin(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nanmin] Entering function");
    if (!matrix || !matrix->data) {
        return NAN;
    }
    double mn = NAN;
    int seen = 0;
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (!isnan(x)) {
            if (!seen || x < mn) { 
                mn = x; 
                seen = 1; 
            }
        }
    }
    return seen ? mn : NAN;
}

/**
 * @brief Finds the maximum over the non-NaN elements.
 *
 * The C analogue of NumPy's `numpy.nanmax(x)`: the largest element, ignoring
 * `NaN`s. If every element is `NaN` (or the matrix is empty), the result is
 * `NaN`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The NaN-ignoring maximum, or `NaN` if there are no non-NaN elements.
 */
double matrix_nanmax(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nanmax] Entering function");
    if (!matrix || !matrix->data) {
        return NAN;
    }
    double mx = NAN;
    int seen = 0;
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (!isnan(x)) {
            if (!seen || x > mx) { 
                mx = x; 
                seen = 1; 
            }
        }
    }
    return seen ? mx : NAN;
}

/**
 * @brief Multiplies all elements, treating NaN as one.
 *
 * The C analogue of NumPy's `numpy.nanprod(x)`: multiplies every element but
 * skips `NaN`s (counting them as `1`). An empty or all-`NaN` matrix yields
 * `1.0`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The NaN-ignoring product, or `1.0` if `matrix` is NULL/empty.
 */
double matrix_nanprod(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nanprod] Entering function");
    if (!matrix || !matrix->data) {
        return 1.0;
    }
    double prod = 1.0;
    size_t n = matrix->rows * matrix->cols;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (!isnan(x)) { 
            prod *= x; 
        }
    }
    return prod;
}

/**
 * @brief Builds a row vector of evenly spaced values over a half-open interval.
 *
 * The C analogue of NumPy's `numpy.arange(start, stop, step)`: a `1 x N` row
 * vector starting at `start` and incrementing by `step`, stopping before `stop`
 * (the endpoint is excluded). The length is `ceil((stop - start) / step)`, and
 * element `i` is `start + i * step`.
 *
 * @param start The first value.
 * @param stop  The exclusive upper (or lower, for negative step) bound.
 * @param step  The spacing between values. Must be non-zero.
 *
 * @return A new `1 x N` row vector, or `NULL` if `step` is `0` or the range is
 *         empty (no values fit). The caller owns the result and must release it
 *         with `matrix_deallocate`.
 */
Matrix* matrix_arange(double start, double stop, double step) {
    MATRIX_LOG("[matrix_arange] Entering function");
    if (step == 0.0) {
        MATRIX_LOG("[matrix_arange] Error: step must be non-zero.");
        return NULL;
    }
    double count_d = ceil((stop - start) / step);
    if (!(count_d > 0.0)) {
        MATRIX_LOG("[matrix_arange] Empty range.");
        return NULL;
    }

    size_t n = (size_t)count_d;
    Matrix* result = matrix_create(1, n);
    if (!result) { 
        return NULL; 
    }

    for (size_t i = 0; i < n; ++i) {
        result->data[i] = start + (double)i * step;
    }

    return result;
}

/**
 * @brief Builds a row vector of evenly spaced samples over a closed interval.
 *
 * The C analogue of NumPy's `numpy.linspace(start, stop, num)` with the default
 * `endpoint=True`: a `1 x num` row vector of `num` samples spaced evenly over the
 * closed interval `[start, stop]`. The spacing is `(stop - start) / (num - 1)`;
 * the first element is exactly `start` and the last is set exactly to `stop`. For
 * `num == 1` the single element is `start`.
 *
 * @param start The first value (inclusive).
 * @param stop  The last value (inclusive).
 * @param num   The number of samples to generate. Must be greater than 0.
 *
 * @return A new `1 x num` row vector, or `NULL` if `num` is `0` or allocation
 *         fails. The caller owns the result and must release it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_linspace(double start, double stop, size_t num) {
    MATRIX_LOG("[matrix_linspace] Entering function with num = %zu", num);
    if (num == 0) {
        MATRIX_LOG("[matrix_linspace] Error: num must be greater than 0.");
        return NULL;
    }

    Matrix* result = matrix_create(1, num);
    if (!result) { 
        return NULL; 
    }
    if (num == 1) {
        result->data[0] = start;
        return result;
    }

    double step = (stop - start) / (double)(num - 1);
    for (size_t i = 0; i < num; ++i) {
        result->data[i] = start + (double)i * step;
    }
    result->data[num - 1] = stop;   /* pin the endpoint exactly, like NumPy */

    return result;
}

/**
 * @brief Computes the differences between consecutive flattened elements.
 *
 * The C analogue of NumPy's `numpy.ediff1d(x)`: flattens the matrix in row-major
 * order and returns a `1 x (N-1)` row vector whose `i`-th entry is
 * `x[i+1] - x[i]`. The input is not modified.
 *
 * @param matrix The source matrix (must hold at least 2 elements). Must not be
 *               NULL.
 *
 * @return A new `1 x (N-1)` row vector of consecutive differences, or `NULL` if
 *         `matrix` is NULL, holds fewer than 2 elements, or allocation fails.
 *         Free it with `matrix_deallocate`.
 */
Matrix* matrix_ediff1d(const Matrix* matrix) {
    MATRIX_LOG("[matrix_ediff1d] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    if (n < 2) {
        return NULL;
    }

    Matrix* result = matrix_create(1, n - 1);
    if (!result) { 
        return NULL; 
    }

    for (size_t i = 0; i + 1 < n; ++i) {
        result->data[i] = matrix->data[i + 1] - matrix->data[i];
    }

    return result;
}

/**
 * @brief Returns the flattened indices of the non-zero elements.
 *
 * The C analogue of NumPy's `numpy.flatnonzero(x)`: scans the matrix in row-major
 * (C) order and returns a `1 x K` row vector of the flat indices of the `K`
 * non-zero elements. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new `1 x K` row vector of flat indices, or `NULL` if `matrix` is
 *         NULL, has no non-zero elements, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_flatnonzero(const Matrix* matrix) {
    MATRIX_LOG("[matrix_flatnonzero] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }
    size_t n = matrix->rows * matrix->cols;
    size_t k = 0;
    for (size_t i = 0; i < n; ++i) {
        if (matrix->data[i] != 0.0) { ++k; }
    }
    if (k == 0) {
        MATRIX_LOG("[matrix_flatnonzero] No non-zero elements.");
        return NULL;
    }

    Matrix* result = matrix_create(1, k);
    if (!result) { 
        return NULL; 
    }

    size_t j = 0;
    for (size_t i = 0; i < n; ++i) {
        if (matrix->data[i] != 0.0) { result->data[j++] = (double)i; }
    }
    return result;
}

/**
 * @brief Extracts the elements selected by a non-zero condition mask.
 *
 * The C analogue of NumPy's `numpy.extract(condition, arr)`: returns a `1 x K`
 * row vector of the elements of `arr` (in row-major order) at the positions where
 * the corresponding element of `condition` is non-zero. Both inputs must have the
 * same shape and neither is modified.
 *
 * @param condition The selection mask (non-zero selects). Must not be NULL.
 * @param arr       The values to pull from, same shape as `condition`. Must not
 *                  be NULL.
 *
 * @return A new `1 x K` row vector of selected values, or `NULL` if either input
 *         is NULL, the shapes differ, no element is selected, or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_extract(const Matrix* condition, const Matrix* arr) {
    MATRIX_LOG("[matrix_extract] Entering function");
    if (!condition || !arr || !condition->data || !arr->data) {
        return NULL;
    }
    if (condition->rows != arr->rows || condition->cols != arr->cols) {
        return NULL;
    }

    size_t n = arr->rows * arr->cols;
    size_t k = 0;
    for (size_t i = 0; i < n; ++i) {
        if (condition->data[i] != 0.0) { ++k; }
    }

    if (k == 0) {
        MATRIX_LOG("[matrix_extract] No element selected.");
        return NULL;
    }

    Matrix* result = matrix_create(1, k);
    if (!result) { 
        return NULL; 
    }

    size_t j = 0;
    for (size_t i = 0; i < n; ++i) {
        if (condition->data[i] != 0.0) { result->data[j++] = arr->data[i]; }
    }

    return result;
}

/**
 * @brief Trims leading and trailing zeros from a vector.
 *
 * The C analogue of NumPy's `numpy.trim_zeros(filt)` with the default
 * `trim='fb'`: flattens the matrix and removes the runs of exactly-zero elements
 * at the front and back, keeping the inner portion (including any interior
 * zeros). The input is not modified.
 *
 * @param matrix The source vector. Must not be NULL.
 *
 * @return A new `1 x K` row vector with leading/trailing zeros removed, or `NULL`
 *         if `matrix` is NULL/empty, every element is zero, or allocation fails.
 *         Free it with `matrix_deallocate`.
 */
Matrix* matrix_trim_zeros(const Matrix* matrix) {
    MATRIX_LOG("[matrix_trim_zeros] Entering function");
    if (!matrix || !matrix->data) {
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    if (n == 0) {
        return NULL;
    }

    size_t lo = 0;
    while (lo < n && matrix->data[lo] == 0.0) { 
        ++lo; 
    }

    if (lo == n) {
        MATRIX_LOG("[matrix_trim_zeros] All elements are zero.");
        return NULL;   /* nothing left after trimming */
    }

    size_t hi = n - 1;
    while (hi > lo && matrix->data[hi] == 0.0) { 
        --hi; 
    }

    size_t len = hi - lo + 1;
    Matrix* result = matrix_create(1, len);
    if (!result) { 
        return NULL; 
    }

    for (size_t i = 0; i < len; ++i) {
        result->data[i] = matrix->data[lo + i];
    }

    return result;
}

/**
 * @brief Returns the flat index of the maximum element, ignoring NaN.
 *
 * The C analogue of NumPy's `numpy.nanargmax(x)`: the row-major flat index of the
 * largest non-`NaN` element (the first occurrence on ties). The input is not
 * modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The flat index of the maximum non-NaN element, or `SIZE_MAX` if
 *         `matrix` is NULL/empty or every element is `NaN`.
 */
size_t matrix_nanargmax(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nanargmax] Entering function");
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return SIZE_MAX;
    }

    size_t n = matrix->rows * matrix->cols;
    size_t best = SIZE_MAX;
    double mx = 0.0;

    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];
        if (isnan(x)) { 
            continue; 
        }
        if (best == SIZE_MAX || x > mx) { 
            mx = x; 
            best = i; 
        }
    }

    return best;
}

/**
 * @brief Computes the cumulative sum, treating NaN as zero.
 *
 * The C analogue of NumPy's `numpy.nancumsum(x)`: flattens the matrix in
 * row-major order and returns a `1 x N` row vector of the running totals, where
 * each `NaN` contributes `0` (so the running total is held flat across `NaN`s
 * rather than becoming `NaN`). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new `1 x N` row vector of cumulative sums, or `NULL` if `matrix` is
 *         NULL/empty or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_nancumsum(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nancumsum] Entering function");
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(1, n);
    if (!result) { 
        return NULL; 
    }

    double running = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double x = matrix->data[i];

        if (!isnan(x)) { 
            running += x; 
        }
        result->data[i] = running;
    }

    return result;
}

/**
 * @brief Sums along an axis, treating NaN as zero.
 *
 * The C analogue of NumPy's `numpy.nansum(x, axis=...)`: `axis == 0` sums down
 * each column (`1 x cols`); `axis == 1` sums across each row (`rows x 1`). `NaN`s
 * are skipped, so an all-`NaN` slice sums to `0`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param axis   `0` for column sums, `1` for row sums.
 *
 * @return A new vector of per-axis NaN-ignoring sums, or `NULL` if `matrix` is
 *         NULL, `axis` is neither 0 nor 1, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_nansum_axis(const Matrix* matrix, int axis) {
    MATRIX_LOG("[matrix_nansum_axis] Entering function with axis = %d", axis);
    if (!matrix || !matrix->data) {
        return NULL;
    }
    if (axis != 0 && axis != 1) {
        return NULL;
    }
    Matrix* result = (axis == 0) ? matrix_create(1, matrix->cols)
                                 : matrix_create(matrix->rows, 1);
    if (!result) { 
        return NULL; 
    }

    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double v = matrix->data[i * matrix->cols + j];

            if (isnan(v)) { 
                continue; 
            }
            if (axis == 0) { 
                result->data[j] += v; 
            }
            else { 
                result->data[i] += v; 
            }
        }
    }
    return result;
}


/**
 * @brief Computes the geometric mean of all elements.
 *
 * The C analogue of SciPy's `scipy.stats.gmean(x)`: `exp(mean(log(x)))`, the
 * `n`-th root of the product of the `n` elements. Elements should be positive; a
 * zero drives the result to `0`, and a negative element yields `NaN` (via
 * `log`). The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The geometric mean, or `NaN` if `matrix` is NULL/empty.
 */
double matrix_gmean(const Matrix* matrix) {
    MATRIX_LOG("[matrix_gmean] Entering function");
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NAN;
    }

    size_t n = matrix->rows * matrix->cols;
    double sum_log = 0.0;
    for (size_t i = 0; i < n; ++i) {
        sum_log += log(matrix->data[i]);
    }

    return exp(sum_log / (double)n);
}


/**
 * @brief Computes the standard error of the mean.
 *
 * The C analogue of SciPy's `scipy.stats.sem(x)` with the default `ddof=1`: the
 * sample standard deviation divided by the square root of the count,
 * `sqrt(sum((x - mean)^2) / (n - 1)) / sqrt(n)`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The standard error of the mean, or `NaN` if `matrix` is NULL or holds
 *         fewer than 2 elements.
 */
double matrix_sem(const Matrix* matrix) {
    MATRIX_LOG("[matrix_sem] Entering function");
    if (!matrix || !matrix->data) {
        return NAN;
    }

    size_t n = matrix->rows * matrix->cols;
    if (n < 2) {
        return NAN;
    }

    double mean = 0.0;
    for (size_t i = 0; i < n; ++i) { 
        mean += matrix->data[i]; 
    }

    mean /= (double)n;
    double ss = 0.0;

    for (size_t i = 0; i < n; ++i) {
        double d = matrix->data[i] - mean;
        ss += d * d;
    }
    double sample_std = sqrt(ss / (double)(n - 1));

    return sample_std / sqrt((double)n);
}


/**
 * @brief Counts the number of distinct (non-NaN) values.
 *
 * The C analogue of pandas' `Series.nunique()` with the default `dropna=True`:
 * the number of distinct values among the elements, excluding `NaN`. The input is
 * not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return The count of distinct non-NaN values (`0` if `matrix` is NULL/empty or
 *         all elements are `NaN`).
 */
size_t matrix_nunique(const Matrix* matrix) {
    MATRIX_LOG("[matrix_nunique] Entering function");
    if (!matrix || !matrix->data) {
        return 0;
    }
    
    size_t n = matrix->rows * matrix->cols;
    if (n == 0) {
        return 0;
    }

    double* buf = (double*)malloc(n * sizeof(double));
    if (!buf) { 
        return 0; 
    }

    size_t m = 0;
    for (size_t i = 0; i < n; ++i) {
        if (!isnan(matrix->data[i])) {
            buf[m++] = matrix->data[i]; 
        }
    }

    if (m == 0) { 
        free(buf); 
        return 0; 
    }
    qsort(buf, m, sizeof(double), mat_cmp_asc);

    size_t distinct = 1;
    for (size_t i = 1; i < m; ++i) {
        if (buf[i] != buf[i - 1]) { 
            ++distinct; 
        }
    }

    free(buf);
    return distinct;
}

/**
 * @brief Builds a row vector spaced evenly on a logarithmic scale.
 *
 * The C analogue of NumPy's `numpy.logspace(start, stop, num, base)`:
 * `base ** linspace(start, stop, num)` — `num` samples where the exponents are
 * evenly spaced over the closed interval `[start, stop]`. The result is a
 * `1 x num` row vector.
 *
 * @param start The exponent of the first value (`base ** start`).
 * @param stop  The exponent of the last value (`base ** stop`).
 * @param num   The number of samples to generate. Must be greater than 0.
 * @param base  The base of the log scale (e.g. `10.0`).
 *
 * @return A new `1 x num` row vector, or `NULL` if `num` is `0` or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_logspace(double start, double stop, size_t num, double base) {
    MATRIX_LOG("[matrix_logspace] Entering function with num = %zu", num);
    if (num == 0) {
        return NULL;
    }

    Matrix* result = matrix_create(1, num);
    if (!result) { 
        return NULL; 
    }
    if (num == 1) {
        result->data[0] = pow(base, start);
        return result;
    }

    double step = (stop - start) / (double)(num - 1);
    for (size_t i = 0; i < num; ++i) {
        double exponent = (i == num - 1) ? stop : (start + (double)i * step);
        result->data[i] = pow(base, exponent);
    }

    return result;
}


/**
 * @brief Builds a row vector spaced evenly on a geometric progression.
 *
 * The C analogue of NumPy's `numpy.geomspace(start, stop, num)`: `num` samples
 * spaced evenly on a log scale (a geometric progression) from `start` to `stop`,
 * both endpoints inclusive. Equivalent to `exp(linspace(log(start), log(stop),
 * num))`, with the endpoints pinned exactly to `start` and `stop`. Both `start`
 * and `stop` must be positive.
 *
 * @param start The first value (inclusive); must be greater than 0.
 * @param stop  The last value (inclusive); must be greater than 0.
 * @param num   The number of samples to generate. Must be greater than 0.
 *
 * @return A new `1 x num` row vector, or `NULL` if `num` is `0`, `start` or
 *         `stop` is not positive, or allocation fails. Free it with
 *         `matrix_deallocate`.
 */
Matrix* matrix_geomspace(double start, double stop, size_t num) {
    MATRIX_LOG("[matrix_geomspace] Entering function with num = %zu", num);
    if (num == 0 || start <= 0.0 || stop <= 0.0) {
        return NULL;
    }

    Matrix* result = matrix_create(1, num);
    if (!result) { 
        return NULL; 
    }

    if (num == 1) {
        result->data[0] = start;
        return result;
    }

    double ls = log(start);
    double le = log(stop);
    double step = (le - ls) / (double)(num - 1);

    for (size_t i = 0; i < num; ++i) {
        result->data[i] = exp(ls + (double)i * step);
    }

    result->data[0] = start;          /* pin the endpoints exactly, like NumPy */
    result->data[num - 1] = stop;

    return result;
}


/**
 * @brief Resizes a matrix, cycling its elements to fill the new shape.
 *
 * The C analogue of NumPy's `numpy.resize(a, new_shape)`: returns a new
 * `rows x cols` matrix whose elements are taken from the row-major flattening of
 * `a`, repeating from the beginning as needed to fill (and truncating if the new
 * shape is smaller). Unlike `matrix_reshape`, the total size need not match. The
 * input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL and not empty.
 * @param rows   The number of rows in the result. Must be greater than 0.
 * @param cols   The number of columns in the result. Must be greater than 0.
 *
 * @return A new `rows x cols` matrix filled by cycling the source elements, or
 *         `NULL` if `matrix` is NULL/empty, a dimension is `0`, or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_resize(const Matrix* matrix, size_t rows, size_t cols) {
    MATRIX_LOG("[matrix_resize] Entering function with rows = %zu, cols = %zu", rows, cols);
    if (!matrix || !matrix->data) {
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    if (n == 0) {
        return NULL;
    }

    Matrix* result = matrix_create(rows, cols);
    if (!result) { 
        return NULL; 
    }

    size_t total = rows * cols;
    for (size_t i = 0; i < total; ++i) {
        result->data[i] = matrix->data[i % n];
    }

    return result;
}


/**
 * @brief Converts flat indices into (row, column) coordinate pairs.
 *
 * The C analogue of NumPy's `numpy.unravel_index(indices, shape)` for a 2-D
 * `(rows, cols)` shape: each (flattened) input index `k` is mapped to the pair
 * `(k / cols, k % cols)`. The result is a `K x 2` matrix whose rows are the
 * `(row, col)` pairs. Neither input is modified.
 *
 * @param indices The flat indices to convert (any shape; integer-valued, in
 *                `0 .. rows*cols-1`). Must not be NULL.
 * @param rows    The number of rows in the target shape.
 * @param cols    The number of columns in the target shape. Must be greater than
 *                0.
 *
 * @return A new `K x 2` matrix of `(row, col)` pairs, or `NULL` if `indices` is
 *         NULL/empty, `cols` is `0`, any index is negative or `>= rows*cols`, or
 *         allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_unravel_index(const Matrix* indices, size_t rows, size_t cols) {
    MATRIX_LOG("[matrix_unravel_index] Entering function with rows = %zu, cols = %zu", rows, cols);
    if (!indices || !indices->data || cols == 0) {
        return NULL;
    }

    size_t k = indices->rows * indices->cols;
    if (k == 0) {
        return NULL;
    }

    if (rows != 0 && cols > SIZE_MAX / rows) {
        return NULL;
    }
    size_t limit = rows * cols;
    Matrix* result = matrix_create(k, 2);

    if (!result) { 
        return NULL; 
    }
    for (size_t t = 0; t < k; ++t) {
        double dv = indices->data[t];

        /* NaN fails every comparison: test the accepted range positively
           before the size_t conversion */
        if (!(dv >= 0.0) || !(dv < (double)limit)) {
            MATRIX_LOG("[matrix_unravel_index] Error: index is negative, NaN or out of range.");
            matrix_deallocate(result);
            return NULL;
        }

        size_t flat = (size_t)dv;
        if (flat >= limit) {
            MATRIX_LOG("[matrix_unravel_index] Error: index out of range.");
            matrix_deallocate(result);
            return NULL;
        }

        result->data[t * 2 + 0] = (double)(flat / cols);
        result->data[t * 2 + 1] = (double)(flat % cols);
    }

    return result;
}


/**
 * @brief Flattens a matrix into a single row vector.
 *
 * The C analogue of NumPy's `numpy.ravel(x)`: returns a `1 x (rows*cols)` row
 * vector holding a row-major (C-order) copy of the elements. Unlike
 * `matrix_to_array` (which returns a raw `double*`), this returns an owning
 * `Matrix`. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new `1 x (rows*cols)` row vector, or `NULL` if `matrix` is
 *         NULL/empty or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_ravel(const Matrix* matrix) {
    MATRIX_LOG("[matrix_ravel] Entering function");
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result = matrix_create(1, n);
    if (!result) { 
        return NULL; 
    }
    memcpy(result->data, matrix->data, n * sizeof(double));

    return result;
}


/**
 * @brief Computes the inner product of two vectors.
 *
 * The C analogue of NumPy's `numpy.dot(a, b)` for one-dimensional inputs: the
 * scalar inner product `sum(a_i * b_i)` of two equal-length vectors, returned as
 * a `1 x 1` matrix. Both operands must be vectors (a single row or column); for
 * 2-D matrix multiplication use `matrix_multiply` instead. Neither input is
 * modified.
 *
 * @param a The first vector (`1 x N` or `N x 1`). Must not be NULL.
 * @param b The second vector, the same length as `a`. Must not be NULL.
 *
 * @return A new `1 x 1` matrix holding the inner product, or `NULL` if either
 *         input is NULL, is not a vector, the lengths differ, or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_dot(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_dot] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }

    int a_is_vec = (a->rows == 1 || a->cols == 1);
    int b_is_vec = (b->rows == 1 || b->cols == 1);
    if (!a_is_vec || !b_is_vec) {
        MATRIX_LOG("[matrix_dot] Error: both operands must be vectors.");
        return NULL;
    }

    size_t na = a->rows * a->cols;
    size_t nb = b->rows * b->cols;
    if (na == 0 || na != nb) {
        return NULL;
    }

    Matrix* result = matrix_create(1, 1);
    if (!result) { 
        return NULL; 
    }

    double sum = 0.0;
    for (size_t i = 0; i < na; ++i) {
        sum += a->data[i] * b->data[i];
    }
    result->data[0] = sum;

    return result;
}


/**
 * @brief Computes the inner product of two flattened matrices.
 *
 * The C analogue of NumPy's `numpy.vdot(a, b)`: flattens both operands in
 * row-major order (regardless of their shapes) and returns the scalar
 * `sum(a_i * b_i)` as a `1 x 1` matrix. The two matrices only need the same total
 * number of elements. (For real-valued data this matches `numpy.vdot`, which
 * conjugates its first argument only for complex input.) Neither input is
 * modified.
 *
 * @param a The first matrix (any shape). Must not be NULL.
 * @param b The second matrix, with the same total size as `a`. Must not be NULL.
 *
 * @return A new `1 x 1` matrix holding the flattened inner product, or `NULL` if
 *         either input is NULL/empty, the total sizes differ, or allocation
 *         fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_vdot(const Matrix* a, const Matrix* b) {
    MATRIX_LOG("[matrix_vdot] Entering function");
    if (!a || !b || !a->data || !b->data) {
        return NULL;
    }

    size_t na = a->rows * a->cols;
    size_t nb = b->rows * b->cols;
    if (na == 0 || na != nb) {
        return NULL;
    }

    Matrix* result = matrix_create(1, 1);
    if (!result) { 
        return NULL; 
    }

    double sum = 0.0;
    for (size_t i = 0; i < na; ++i) {
        sum += a->data[i] * b->data[i];
    }
    result->data[0] = sum;

    return result;
}


/**
 * @brief Sums the elements along a diagonal at a given offset.
 *
 * The C analogue of NumPy's `numpy.trace(x, offset=k)`: sums the elements
 * `x[i][i+k]` along the diagonal offset by `k` from the main diagonal — `k > 0`
 * selects a super-diagonal (above the main), `k < 0` a sub-diagonal (below). This
 * generalizes `matrix_trace` (which is the `k == 0` case) and works for
 * rectangular matrices. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 * @param k      The diagonal offset (0 is the main diagonal).
 *
 * @return The sum along the chosen diagonal (`0.0` if `matrix` is NULL or the
 *         offset selects no valid elements).
 */
double matrix_trace_offset(const Matrix* matrix, int k) {
    MATRIX_LOG("[matrix_trace_offset] Entering function with k = %d", k);
    if (!matrix || !matrix->data) {
        return 0.0;
    }

    double sum = 0.0;
    for (size_t i = 0; i < matrix->rows; ++i) {
        ptrdiff_t j = (ptrdiff_t)i + k;
        if (j >= 0 && (size_t)j < matrix->cols) {
            sum += matrix->data[i * matrix->cols + (size_t)j];
        }
    }

    return sum;
}


/**
 * @brief Ensures a vector is represented as an explicit row matrix.
 *
 * The C analogue of NumPy's `numpy.atleast_2d(x)`: a row or column vector is
 * returned as a `1 x N` row matrix (treating it as a 1-D array, as NumPy does);
 * a genuine 2-D matrix (more than one row and column) is returned unchanged. In
 * every case a new owning copy is produced. The input is not modified.
 *
 * @param matrix The source matrix. Must not be NULL.
 *
 * @return A new matrix — `1 x N` for a vector input, or a same-shape copy for a
 *         2-D input — or `NULL` if `matrix` is NULL/empty or allocation fails.
 *         Free it with `matrix_deallocate`.
 */
Matrix* matrix_atleast_2d(const Matrix* matrix) {
    MATRIX_LOG("[matrix_atleast_2d] Entering function");
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        return NULL;
    }

    size_t n = matrix->rows * matrix->cols;
    Matrix* result;
    if (matrix->rows > 1 && matrix->cols > 1) {
        result = matrix_create(matrix->rows, matrix->cols);   /* genuine 2-D: keep shape */
    }
    else {
        result = matrix_create(1, n);                         /* vector: promote to 1 x N */
    }
    if (!result) { 
        return NULL; 
    }
    memcpy(result->data, matrix->data, n * sizeof(double));

    return result;
}

/**
 * @brief Returns the DFT sample frequencies.
 *
 * The C analogue of NumPy's `numpy.fft.fftfreq(n, d)`: a `1 x n` row vector of
 * the frequency bins for a length-`n` DFT with sample spacing `d`, in the order
 * `[0, 1, ..., (n-1)/2, -(n/2), ..., -1] / (d*n)` (the non-negative frequencies
 * first, then the negative ones).
 *
 * @param n The window length (number of samples). Must be greater than 0.
 * @param d The sample spacing (e.g. the time step). Typically `1.0`.
 *
 * @return A new `1 x n` row vector of sample frequencies, or `NULL` if `n` is `0`
 *         or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_fftfreq(size_t n, double d) {
    MATRIX_LOG("[matrix_fftfreq] Entering function with n = %zu", n);
    if (n == 0) {
        return NULL;
    }

    Matrix* result = matrix_create(1, n);
    if (!result) { 
        return NULL; 
    }

    double val = 1.0 / ((double)n * d);
    size_t half = (n - 1) / 2;   /* last index of the non-negative block */
    for (size_t i = 0; i < n; ++i) {
        ptrdiff_t fi = (i <= half) ? (ptrdiff_t)i : ((ptrdiff_t)i - (ptrdiff_t)n);
        result->data[i] = (double)fi * val;
    }

    return result;
}


/**
 * @brief Returns the sample frequencies for a real FFT.
 *
 * The C analogue of NumPy's `numpy.fft.rfftfreq(n, d)`: a `1 x (n/2 + 1)` row
 * vector of the non-negative frequency bins `[0, 1, ..., n/2] / (d*n)` produced
 * by a real-input FFT of length `n` with sample spacing `d`.
 *
 * @param n The window length (number of samples). Must be greater than 0.
 * @param d The sample spacing (e.g. the time step). Typically `1.0`.
 *
 * @return A new `1 x (n/2 + 1)` row vector of sample frequencies, or `NULL` if
 *         `n` is `0` or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_rfftfreq(size_t n, double d) {
    MATRIX_LOG("[matrix_rfftfreq] Entering function with n = %zu", n);
    if (n == 0) {
        return NULL;
    }

    size_t m = n / 2 + 1;
    Matrix* result = matrix_create(1, m);
    if (!result) { 
        return NULL; 
    }

    double val = 1.0 / ((double)n * d);
    for (size_t i = 0; i < m; ++i) {
        result->data[i] = (double)i * val;
    }

    return result;
}


/**
 * @brief Builds a Hann (Hanning) window.
 *
 * The C analogue of NumPy's `numpy.hanning(M)`: a `1 x M` row vector of the Hann
 * window `0.5 - 0.5*cos(2*pi*n / (M-1))` for `n = 0 .. M-1`. As in NumPy, a
 * length-1 window is the single value `1.0`.
 *
 * @param M The number of points in the window. Must be greater than 0.
 *
 * @return A new `1 x M` row vector holding the window, or `NULL` if `M` is `0`
 *         or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_hanning(size_t M) {
    MATRIX_LOG("[matrix_hanning] Entering function with M = %zu", M);
    if (M == 0) {
        return NULL;
    }

    Matrix* result = matrix_create(1, M);
    if (!result) { 
        return NULL; 
    }
    if (M == 1) {
        result->data[0] = 1.0;
        return result;
    }

    const double two_pi = 6.28318530717958647692;
    for (size_t i = 0; i < M; ++i) {
        result->data[i] = 0.5 - 0.5 * cos(two_pi * (double)i / (double)(M - 1));
    }

    return result;
}


/**
 * @brief Builds a Hamming window.
 *
 * The C analogue of NumPy's `numpy.hamming(M)`: a `1 x M` row vector of the
 * Hamming window `0.54 - 0.46*cos(2*pi*n / (M-1))` for `n = 0 .. M-1`. As in
 * NumPy, a length-1 window is the single value `1.0`.
 *
 * @param M The number of points in the window. Must be greater than 0.
 *
 * @return A new `1 x M` row vector holding the window, or `NULL` if `M` is `0`
 *         or allocation fails. Free it with `matrix_deallocate`.
 */
Matrix* matrix_hamming(size_t M) {
    MATRIX_LOG("[matrix_hamming] Entering function with M = %zu", M);
    if (M == 0) {
        return NULL;
    }

    Matrix* result = matrix_create(1, M);
    if (!result) { 
        return NULL; 
    }
    if (M == 1) {
        result->data[0] = 1.0;
        return result;
    }

    const double two_pi = 6.28318530717958647692;
    for (size_t i = 0; i < M; ++i) {
        result->data[i] = 0.54 - 0.46 * cos(two_pi * (double)i / (double)(M - 1));
    }
    
    return result;
}
