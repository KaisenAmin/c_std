/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Matrix
*/

#include <math.h>
#include <string.h>
#include <float.h>
#include <stdint.h>
#include <time.h>
#include "matrix.h"

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
static void generateWalshMatrixRecursively(double* data, int order, int dim, int startRow, int startCol, int val) {
    MATRIX_LOG("[generateWalshMatrixRecursively] Info: Generating Walsh matrix with order %d at position (%d, %d) with initial value %d.\n", order, startRow, startCol, val);

    if (order == 1) {
        data[startRow * dim + startCol] = val;
        MATRIX_LOG("[generateWalshMatrixRecursively] Success: Base case reached, set value %d at (%d, %d).\n", val, startRow, startCol);
        return;
    }

    int halfOrder = order / 2;
    // Top-left quadrant
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow, startCol, val);
    // Top-right quadrant
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow, startCol + halfOrder, val);
    // Bottom-left quadrant
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow + halfOrder, startCol, val);
    // Bottom-right quadrant (invert the values)
    generateWalshMatrixRecursively(data, halfOrder, dim, startRow + halfOrder, startCol + halfOrder, -val);

    MATRIX_LOG("[generateWalshMatrixRecursively] Success: Walsh matrix generated recursively with order %d.\n", order);
}

static inline int min_number(int a, int b) {
    MATRIX_LOG("[min_number] Info: Calculating minimum between %d and %d.\n", a, b);
    int result = (a < b) ? a : b;
    MATRIX_LOG("[min_number] Success: The minimum value is %d.\n", result);
    return result;
}

// Function to calculate binomial coefficient
static double binomial_coefficient(int n, int k) {
    MATRIX_LOG("[binomial_coefficient] Info: Calculating binomial coefficient C(%d, %d).\n", n, k);

    double *C = (double*) malloc(sizeof(double) * (k + 1));
    if (!C) {
        MATRIX_LOG("[binomial_coefficient] Error: Memory allocation failed for binomial coefficient calculation.\n");
        return -1;
    }
    
    memset(C, 0, sizeof(double) * (k + 1));
    C[0] = 1;

    for (int i = 1; i <= n; i++) {
        for (int j = min_number(i, k); j > 0; j--) {
            C[j] = C[j] + C[j-1];
        }
    }

    double value = C[k];
    free(C);

    MATRIX_LOG("[binomial_coefficient] Success: Binomial coefficient C(%d, %d) = %lf.\n", n, k, value);
    return value;
}

// Function to calculate factorial
static int64_t factorial(int n) {
    MATRIX_LOG("[factorial] Info: Calculating factorial of %d.\n", n);
    
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }

    MATRIX_LOG("[factorial] Success: Factorial of %d is %lld.\n", n, result);
    return result;
}

// Function to calculate binomial coefficient
static int64_t binomial_factorial(int n, int k) {
    MATRIX_LOG("[binomial_factorial] Info: Calculating binomial coefficient using factorial method C(%d, %d).\n", n, k);

    if (k > n) {
        MATRIX_LOG("[binomial_factorial] Error: k is greater than n in C(%d, %d), returning 0.\n", n, k);
        return 0;
    }
    if (k == 0 || k == n) {
        MATRIX_LOG("[binomial_factorial] Info: Trivial case of C(%d, %d), returning 1.\n", n, k);
        return 1;
    }

    int64_t result = factorial(n) / (factorial(k) * factorial(n - k));
    MATRIX_LOG("[binomial_factorial] Success: Binomial coefficient C(%d, %d) = %lld.\n", n, k, result);

    return result;
}


// function to subtract vector projection of u onto v from u (u = u - proj_v(u))
static double dot_product(const double* v1, const double* v2, size_t length) {
    MATRIX_LOG("[dot_product] Info: Calculating dot product of two vectors of length %zu.\n", length);

    double sum = 0.0;
    for (size_t i = 0; i < length; ++i) {
        sum += v1[i] * v2[i];
    }

    MATRIX_LOG("[dot_product] Success: Dot product calculation completed. Result = %lf.\n", sum);
    return sum;
}

// function to normalize a vector
static void normalize_vector(double* v, size_t length) {
    MATRIX_LOG("[normalize_vector] Info: Starting vector normalization.\n");

    double norm = sqrt(dot_product(v, v, length));
    for (size_t i = 0; i < length; ++i) {
        v[i] /= norm;
    }

    MATRIX_LOG("[normalize_vector] Success: Vector normalization completed.\n");
}

void subtract_projection(double* u, const double* v, size_t length) {
    MATRIX_LOG("[subtract_projection] Info: Starting vector projection subtraction.\n");

    double dot_uv = dot_product(u, v, length);
    double dot_vv = dot_product(v, v, length);
    double scale = dot_uv / dot_vv;
    
    for (size_t i = 0; i < length; ++i) {
        u[i] -= scale * v[i];
    }

    MATRIX_LOG("[subtract_projection] Success: Vector projection subtraction completed.\n");
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

    for (size_t i = 0; i < matrix1->rows; i++) {
        for (size_t j = 0; j < matrix2->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < matrix1->cols; k++) {
                sum += matrix1->data[i * matrix1->cols + k] * matrix2->data[k * matrix2->cols + j];
            }
            product->data[i * product->cols + j] = sum;
            MATRIX_LOG("[matrix_multiply] Set value at (%zu, %zu) = %lf", i, j, sum);
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
        exit(-1);
    }
    if (row >= matrix->rows || col >= matrix->cols) {
        MATRIX_LOG("[matrix_get] Error: Row or column out of bounds. Requested row = %zu, col = %zu.", row, col);
        exit(-1);
    }

    size_t index = row * matrix->cols + col;
    double value = matrix->data[index];

    MATRIX_LOG("[matrix_get] Value at (%zu, %zu) = %lf", row, col, value);
    return value;
}

static bool matrix_check_diagonal(const Matrix* mat, size_t i, size_t j) {
    MATRIX_LOG("[matrix_check_diagonal] Entering function");

    double res = matrix_get(mat, i, j);
    MATRIX_LOG("[matrix_check_diagonal] Starting value on diagonal at (%zu, %zu) = %lf", i, j, res);

    while (++i < mat->rows && ++j < mat->cols) {
        double next_value = matrix_get(mat, i, j);
        if (next_value != res) {
            MATRIX_LOG("[matrix_check_diagonal] Mismatch found at (%zu, %zu), value = %lf", i, j, next_value);
            return false;
        }
    }

    MATRIX_LOG("[matrix_check_diagonal] Diagonal check successful.");
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

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            size_t index = i * matrix->cols + j;
            if (i == j) {
                if (matrix->data[index] != 1.0) {
                    MATRIX_LOG("[matrix_is_identity] Element at (%zu, %zu) is not 1.", i, j);
                    return false;
                }
            } 
            else if (matrix->data[index] != 0.0) {
                MATRIX_LOG("[matrix_is_identity] Element at (%zu, %zu) is not 0.", i, j);
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

    bool isIdempotent = matrix_is_equal(square, matrix);
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
 * This function computes the determinant of a square matrix using a recursive approach. 
 * For 1x1 and 2x2 matrices, it uses direct formulas. For larger matrices, it computes 
 * the determinant by expanding along the first row and using recursive calls on submatrices.
 *
 * @param matrix The input square matrix.
 * @return The determinant of the matrix. Returns 0 if the matrix is not square or an error occurs.
 */
double matrix_determinant(const Matrix* matrix) {
    MATRIX_LOG("[matrix_determinant] Entering function");

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
    else {
        double det = 0;
        for (int j1 = 0; j1 < (int)matrix->cols; j1++) {
            Matrix* submatrix = matrix_create(matrix->rows - 1, matrix->cols - 1);
            
            for (int i = 1; i < (int)matrix->rows; i++) {
                int j2 = 0;
                for (int j = 0; j < (int)matrix->cols; j++) {
                    if (j == j1) {
                        continue;
                    }
                    matrix_set(submatrix, i - 1, j2++, matrix->data[i * matrix->cols + j]);
                }
            }
            double cofactor = (j1 % 2 == 0 ? 1 : -1) * matrix->data[j1];
            double subDet = matrix_determinant(submatrix);
            det += cofactor * subDet;

            MATRIX_LOG("[matrix_determinant] Submatrix determinant at column %d is %lf.", j1, subDet);
            matrix_deallocate(submatrix);
        }

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

    Matrix* cofactorMatrix = matrix_create(matrix->rows, matrix->cols);
    if (!cofactorMatrix) {
        MATRIX_LOG("[matrix_adjugate] Error: Memory allocation failed for cofactorMatrix.");
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            Matrix* submatrix = matrix_create_submatrix(matrix, i, j);
            double minor = matrix_determinant(submatrix);
            double cofactor = pow(-1, i + j) * minor;

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
 * This function calculates the inverse of a square matrix by first computing its determinant.
 * If the determinant is non-zero, the function then calculates the adjugate matrix and divides
 * it by the determinant to obtain the inverse.
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

    double det = matrix_determinant(matrix);
    if (det == 0) {
        MATRIX_LOG("[matrix_inverse] Error: Matrix is singular (det = 0) and cannot be inverted.");
        return NULL;
    }

    Matrix* inverse = matrix_adjugate(matrix);
    if (!inverse) {
        MATRIX_LOG("[matrix_inverse] Error: Failed to calculate adjugate matrix.");
        return NULL;
    }

    for (size_t i = 0; i < inverse->rows; i++) {
        for (size_t j = 0; j < inverse->cols; j++) {
            inverse->data[i * inverse->cols + j] /= det;
            MATRIX_LOG("[matrix_inverse] Set inverse(%zu, %zu) = %lf", i, j, inverse->data[i * inverse->cols + j]);
        }
    }

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
    if (power < 0) {
        MATRIX_LOG("[matrix_power] Error: Negative power is not supported.");
        return NULL;
    }
    if (power == 0) {
        MATRIX_LOG("[matrix_power] Power is zero, returning the identity matrix.");
        return matrix_create_identity(matrix->rows);
    }

    // Initialize result as a copy of the original matrix for power = 1
    Matrix* result = matrix_copy(matrix);
    if (power == 1) {
        MATRIX_LOG("[matrix_power] Power is 1, returning a copy of the matrix.");
        return result;
    }

    Matrix* temp = NULL;
    while (power > 1) {
        if (power % 2 == 0) {
            temp = matrix_multiply(result, result);
            if (!temp) {
                MATRIX_LOG("[matrix_power] Error: Matrix multiplication failed.");
                matrix_deallocate(result);
                return NULL;
            }

            matrix_deallocate(result);
            result = temp;
            power /= 2;
        } 
        else {
            temp = matrix_multiply(result, matrix);
            if (!temp) {
                MATRIX_LOG("[matrix_power] Error: Matrix multiplication failed.");
                matrix_deallocate(result);
                return NULL;
            }
            matrix_deallocate(result);
            result = temp;
            power--;
        }
    }

    MATRIX_LOG("[matrix_power] Success: Matrix raised to power successfully.");
    return result;
}

/**
 * @brief Computes the rank of a matrix.
 *
 * The rank of a matrix is the maximum number of linearly independent row or column vectors in the matrix. 
 * This function uses a variant of Gaussian elimination to determine the rank.
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

    Matrix* tempMatrix = matrix_copy(matrix);
    if (!tempMatrix) {
        MATRIX_LOG("[matrix_rank] Error: Failed to copy matrix.");
        return -1;
    }

    int rank = tempMatrix->cols;
    MATRIX_LOG("[matrix_rank] Starting rank calculation with initial rank = %d", rank);

    for (int row = 0; row < rank; row++) {
        if (is_effectively_zero(tempMatrix->data[row * tempMatrix->cols + row])) {
            MATRIX_LOG("[matrix_rank] Diagonal element at (%d, %d) is effectively zero. Searching for a non-zero element.", row, row);
            bool reduce = true;
            for (int i = row + 1; i < (int)tempMatrix->rows; i++) {
                if (!is_effectively_zero(tempMatrix->data[i * tempMatrix->cols + row])) {
                    matrix_swap_rows(tempMatrix, row, i);
                    MATRIX_LOG("[matrix_rank] Swapped row %d with row %d.", row, i);
                    reduce = false;
                    break;
                }
            }

            if (reduce) {
                rank--;
                MATRIX_LOG("[matrix_rank] Reduced rank to %d.", rank);

                for (int i = 0; i < (int)tempMatrix->rows; i++) {
                    tempMatrix->data[i * tempMatrix->cols + row] = tempMatrix->data[i * tempMatrix->cols + rank];
                }
                row--;
            }
        } 
        else {
            for (int i = row + 1; i < (int)tempMatrix->rows; i++) {
                double mult = tempMatrix->data[i * tempMatrix->cols + row] / tempMatrix->data[row * tempMatrix->cols + row];
                MATRIX_LOG("[matrix_rank] Eliminating element at (%d, %d) using multiplier %lf", i, row, mult);

                for (int j = row; j < (int)tempMatrix->cols; j++) {
                    tempMatrix->data[i * tempMatrix->cols + j] -= mult * tempMatrix->data[row * tempMatrix->cols + j];
                }
            }
        }
    }

    matrix_deallocate(tempMatrix);
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
    Matrix* product = matrix_create(m * p, n * q);
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

    for (size_t i = 0; i < matrix->rows - 1; i++) {
        for (size_t j = 0; j < matrix->cols - 1; j++) {
            if (i + j >= matrix->rows - 1) {
                continue; // Skip checks not relevant for a Hankel matrix
            }

            double value = matrix_get(matrix, i, j);
            if (i + 1 < matrix->rows && j > 0) {
                double next = matrix_get(matrix, i + 1, j - 1);
                if (!is_effectively_zero(value - next)) {
                    MATRIX_LOG("[matrix_is_hankel] Error: Matrix is not Hankel at element [%zu, %zu].", i + 1, j - 1);
                    return false;
                }
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
        if (!matrix_check_diagonal(matrix, 0, i)) {
            MATRIX_LOG("[matrix_is_toeplitz] Error: Diagonal check failed starting from (0, %zu)", i);
            return false;
        }
    }

    // Check all elements in the first column
    for (size_t i = 1; i < matrix->rows; i++) {
        if (!matrix_check_diagonal(matrix, i, 0)) {
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
            size_t index = (col + row) % n;
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
 * either the full Helmert matrix or the reduced version by removing the last row. The entries in the 
 * Helmert matrix are calculated based on the size of the matrix and whether it is the full or reduced version.
 *
 * @param n The size of the Helmert matrix.
 * @param full Boolean value indicating whether to generate the full Helmert matrix (true) or the reduced version (false).
 * 
 * @return A pointer to the newly created Helmert matrix, or NULL if an error occurs.
 */
Matrix* matrix_helmert(size_t n, bool full) {
    MATRIX_LOG("[matrix_helmert] Entering function with n = %zu, full = %s", n, full ? "true" : "false");

    Matrix* helmertMatrix = matrix_create(n, full ? n : n - 1);
    if (!helmertMatrix) {
        MATRIX_LOG("[matrix_helmert] Error: Memory allocation failed for Helmert matrix.");
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == 0) {
                // First row
                matrix_set(helmertMatrix, i, j, 1.0 / sqrt(n));
                MATRIX_LOG("[matrix_helmert] Set helmert(%zu, %zu) = %lf", i, j, 1.0 / sqrt(n));
            } 
            else if (j < i) {
                // Below diagonal for subsequent rows
                double value = 1.0 / sqrt(i * (i + 1.0));
                matrix_set(helmertMatrix, full ? i : i - 1, j, value);
                MATRIX_LOG("[matrix_helmert] Set helmert(%zu, %zu) = %lf", full ? i : i - 1, j, value);
            } 
            else if (j == i) {
                // Diagonal elements for subsequent rows
                double value = -sqrt((double)i / (i + 1.0));
                matrix_set(helmertMatrix, full ? i : i - 1, j, value);
                MATRIX_LOG("[matrix_helmert] Set helmert(%zu, %zu) = %lf", full ? i : i - 1, j, value);
            }
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

    if (!matrix || !matrix_is_square(matrix)) {
        MATRIX_LOG("[matrix_lu_decomposition] Error: Matrix must be square for LU decomposition.");
        return false;
    }

    size_t n = matrix->rows;
    *L = matrix_create(n, n);
    *U = matrix_create(n, n);

    if (!(*L) || !(*U)) {
        MATRIX_LOG("[matrix_lu_decomposition] Error: Memory allocation failed for L or U.");
        if (*L) matrix_deallocate(*L);
        if (*U) matrix_deallocate(*U);
        return false;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t k = i; k < n; k++) {
            double sum = 0.0;
            for (size_t j = 0; j < i; j++) {
                sum += matrix_get(*L, i, j) * matrix_get(*U, j, k);
            }
            matrix_set(*U, i, k, matrix_get(matrix, i, k) - sum);
            MATRIX_LOG("[matrix_lu_decomposition] Set U(%zu, %zu) = %lf", i, k, matrix_get(matrix, i, k) - sum);
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
 * Note: The input matrix must have more rows than columns (m >= n).
 *
 * @param A The input matrix to decompose.
 * @param Q Pointer to a pointer that will store the orthogonal matrix.
 * @param R Pointer to a pointer that will store the upper triangular matrix.
 * 
 * @return `true` if the decomposition is successful, `false` otherwise.
 */
bool matrix_qr_decomposition(const Matrix* A, Matrix** Q, Matrix** R) {
    MATRIX_LOG("[matrix_qr_decomposition] Entering function");

    if (!A || A->rows < A->cols) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Matrix must have more rows than columns.");
        return false;
    }

    size_t m = A->rows;
    size_t n = A->cols;

    *Q = matrix_create(m, n);
    *R = matrix_create(n, n);
    if (!*Q || !*R) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Memory allocation failed for Q and R.");
        return false;
    }

    // Log memory allocation for temporary vectors
    MATRIX_LOG("[matrix_qr_decomposition] Allocating memory for temporary vectors.");
    double* a_col = (double*)malloc(sizeof(double) * m);
    double* q_col = (double*)malloc(sizeof(double) * m);
    if (!a_col || !q_col) {
        MATRIX_LOG("[matrix_qr_decomposition] Error: Memory allocation failed for column vectors.");
        return false;
    }

    for (size_t i = 0; i < n; ++i) {
        // Copy the ith column of A to a_col
        for (size_t j = 0; j < m; ++j) {
            a_col[j] = matrix_get(A, j, i);
        }

        for (size_t k = 0; k < i; ++k) {
            for (size_t j = 0; j < m; ++j) {
                q_col[j] = matrix_get(*Q, j, k);
            }
            subtract_projection(a_col, q_col, m);  // a_col -= projection of a_col onto q_col
        }

        normalize_vector(a_col, m);  

        for (size_t j = 0; j < m; ++j) {
            matrix_set(*Q, j, i, a_col[j]);
        }

        MATRIX_LOG("[matrix_qr_decomposition] Set column %zu of Q", i);
    }

    for (size_t j = 0; j < n; ++j) {
        for (size_t i = 0; i <= j; ++i) {
            double r_ij = 0.0;
            for (size_t k = 0; k < m; ++k) {
                r_ij += matrix_get(*Q, k, i) * matrix_get(A, k, j);
            }
            matrix_set(*R, i, j, r_ij);
            MATRIX_LOG("[matrix_qr_decomposition] Set R(%zu, %zu) = %lf", i, j, r_ij);
        }
    }

    free(a_col);
    free(q_col);

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
            double value = binomial_coefficient(i + j, i);
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

    if (matrix->rows != matrix->cols) {
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

    // Perform Gauss-Jordan elimination
    for (size_t col = 0; col < n; col++) {
        if (augmented->data[col * 2 * n + col] == 0) {
            MATRIX_LOG("[matrix_inverse_gauss_jordan] Pivot element is zero, searching for non-zero element to swap.");
            size_t swapRow = col + 1;
            while (swapRow < n && augmented->data[swapRow * 2 * n + col] == 0) {
                swapRow++;
            }
            if (swapRow == n) {
                MATRIX_LOG("[matrix_inverse_gauss_jordan] Error: Matrix is singular, cannot invert.");
                matrix_deallocate(augmented);
                return NULL;
            }
            MATRIX_LOG("[matrix_inverse_gauss_jordan] Swapping row %zu with row %zu", col, swapRow);
            matrix_swap_rows(augmented, col, swapRow);
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

    if (!matrix) {
        MATRIX_LOG("[matrix_vandermonde] Error: Matrix object is null");
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
            vandermonde->data[i * n + j] = pow(matrix->data[i], j);
            MATRIX_LOG("[matrix_vandermonde] Setting element at (row, col) = (%zu, %zu) to %lf", i, j, vandermonde->data[i * n + j]);
        }
    }

    MATRIX_LOG("[matrix_vandermonde] Successfully created Vandermonde matrix");
    return vandermonde;
}

/**
 * @brief Generates a companion matrix from the given polynomial coefficients.
 *
 * @param coefficients A matrix representing the polynomial coefficients, where the highest degree coefficient comes last.
 * @param degree The degree of the polynomial.
 * 
 * @return A pointer to the generated companion matrix. If the input coefficients are `NULL` or memory allocation fails,
 * `NULL` is returned.
 */
Matrix* matrix_companion(const Matrix* coefficients, size_t degree) {
    MATRIX_LOG("[matrix_companion] Entering function with degree = %zu", degree);

    if (!coefficients) {
        MATRIX_LOG("[matrix_companion] Error: Coefficients matrix is null");
        return NULL;
    }
    
    size_t n = degree - 1; 
    Matrix* companion = matrix_create(n, n);
    if (!companion) {
        MATRIX_LOG("[matrix_companion] Error: Memory allocation failed for companion matrix");
        return NULL;
    }

    MATRIX_LOG("[matrix_companion] Successfully created companion matrix of size %zux%zu", n, n);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (j == n - 1) { 
                companion->data[i * n + j] = -coefficients->data[n - 1 - i] / coefficients->data[degree - 1];
                MATRIX_LOG("[matrix_companion] Setting element at (row, col) = (%zu, %zu) to %lf (last column)", i, j, companion->data[i * n + j]);
            } 
            else if (i == j + 1) { 
                companion->data[i * n + j] = 1;
                MATRIX_LOG("[matrix_companion] Setting element at (row, col) = (%zu, %zu) to 1 (sub-diagonal)", i, j);
            } 
            else { 
                companion->data[i * n + j] = 0;
                MATRIX_LOG("[matrix_companion] Setting element at (row, col) = (%zu, %zu) to 0", i, j);
            }
        }
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

    double min = DBL_MAX;
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
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

    double max = -DBL_MAX;
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
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

    if (!f) {
        MATRIX_LOG("[matrix_leslie] Error: Matrix f is null");
        return NULL;
    }
    if (f_size != s_size + 1) {
        MATRIX_LOG("[matrix_leslie] Error: f_size (%zu) must be one more than s_size (%zu)", f_size, s_size);
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

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            int s = i + j;
            int64_t sign = (s % 2 == 0) ? 1 : -1;
            int64_t numerator = sign * (i + j + 1) * binomial_factorial(n + i, n - j - 1) * binomial_factorial(n + j, n - i - 1) * binomial_factorial(s, i) * binomial_factorial(s, j);
            int64_t denominator = 1; // The denominator for Hilbert matrix inverse entries when n <= 14 are effectively 1
            double value = (double)numerator / denominator;

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
        totalRows += mat->rows;
        totalCols += mat->cols;
        
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
    srand(time(NULL));

    if (!matrix) {
        MATRIX_LOG("[matrix_random] Error: creation of Matrix object failed");
        return NULL;
    }
    MATRIX_LOG("[matrix_random] Matrix created with size %zux%zu", row, col);

    for (size_t i = 0; i < matrix_size(matrix); i++) {
        matrix->data[i] = (rand() % end) + start;
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
