#include "matrix.h"
#include "../fmt/fmt.h"
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdint.h>
#include <time.h>

#define EPSILON 1e-9

// Function to check if a floating-point number is effectively zero
static bool is_effectively_zero(double value) {
    return fabs(value) < EPSILON;
}

// function to generate the Walsh matrix recursively
static void generateWalshMatrixRecursively(double* data, int order, int dim, int startRow, int startCol, int val) {
    if (order == 1) {
        data[startRow * dim + startCol] = val;
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
}

static inline int min_number(int a, int b) {
    return (a < b) ? a : b;
}

// Function to calculate binomial coefficient
static double binomial_coefficient(int n, int k) {
    double *C = (double*) malloc(sizeof(double) * (k + 1));
    if (!C) {
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
    return value;
}

// Function to calculate factorial
static int64_t factorial(int n) {
    int64_t result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

// Function to calculate binomial coefficient
static int64_t binomial_factorial(int n, int k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    return factorial(n) / (factorial(k) * factorial(n - k));
}

// function to calculate the dot product of two vectors
double dot_product(const double* v1, const double* v2, size_t length) {
    double sum = 0.0;
    for (size_t i = 0; i < length; ++i) {
        sum += v1[i] * v2[i];
    }
    return sum;
}

// function to subtract vector projection of u onto v from u (u = u - proj_v(u))
void subtract_projection(double* u, const double* v, size_t length) {
    double dot_uv = dot_product(u, v, length);
    double dot_vv = dot_product(v, v, length);
    double scale = dot_uv / dot_vv;
    for (size_t i = 0; i < length; ++i) {
        u[i] -= scale * v[i];
    }
}

// function to normalize a vector
void normalize_vector(double* v, size_t length) {
    double norm = sqrt(dot_product(v, v, length));
    for (size_t i = 0; i < length; ++i) {
        v[i] /= norm;
    }
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
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid row indices or matrix is null in matrix_swap_rows.\n");
        #endif
        return; 
    }

    for (size_t i = 0; i < mat->cols; i++) {
        double temp = mat->data[row1 * mat->cols + i];
        mat->data[row1 * mat->cols + i] = mat->data[row2 * mat->cols + i];
        mat->data[row2 * mat->cols + i] = temp;
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Rows %d and %d swapped successfully in matrix_swap_rows.\n", row1, row2);
    #endif
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
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid column indices or matrix is null in matrix_swap_cols.\n");
        #endif
        return; 
    }

    for (size_t i = 0; i < mat->rows; i++) {
        double temp = mat->data[i * mat->cols + col1];
        mat->data[i * mat->cols + col1] = mat->data[i * mat->cols + col2];
        mat->data[i * mat->cols + col2] = temp;
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Columns %d and %d swapped successfully in matrix_swap_cols.\n", col1, col2);
    #endif
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
    for (size_t col = 0; col < matrix->cols; col++) {
        matrix->data[row * matrix->cols + col] /= scalar;
    }
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
    for (size_t col = 0; col < matrix->cols; col++) {
        matrix->data[targetRow * matrix->cols + col] -= scalar * matrix->data[subtractRow * matrix->cols + col];
    }
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
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: rows or cols value is zero in matrix_create.\n");
        #endif 
        return NULL;
    }

    Matrix* matrix = (Matrix*) malloc(sizeof(Matrix));
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation failed for matrix object in matrix_create.\n");
        #endif
        return NULL;
    }

    size_t totalSize = rows * cols * sizeof(double);
    matrix->data = (double*) malloc(totalSize);

    if (!matrix->data) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation faild for matrix data in matrix_create.\n");
        #endif
        return NULL;
    }

    matrix->rows = rows;
    matrix->cols = cols;

    for (size_t index = 0; index < rows * cols; index++) {
        matrix->data[index] = 0.0;
    }

    #ifdef MATRIX_LOGGING_ENABLE  
        fmt_fprintf(stdout, "Success: Matrix creation and initalization is successfull in matrix_create.\n");
    #endif
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
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix1 object is null and invalid in matrix_add.\n");
        #endif 
        return NULL;
    }
    else if (!matrix2) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix2 object is null and invalid in matrix_add.\n");
        #endif 
        return NULL;
    }
    else if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: The two Matrix are not of the same order in matrix_add.\n");
        #endif 
        return NULL;
    }

    Matrix* addition = matrix_create(matrix1->rows, matrix1->cols);
    if (!addition) {
        fmt_fprintf(stderr, "Error: object creation failed for addition to matrix in matrix_add.\n");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows; i++) {
        for (size_t j = 0; j < matrix1->cols; j++) {
            size_t index = i * matrix1->cols + j;
            addition->data[index] = matrix1->data[index] + matrix2->data[index];
        }
    }
    
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
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix1 object is null and invalid in matrix_subtract.\n");
        #endif 
        return NULL;
    }
    else if (!matrix2) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix2 object is null and invalid in matrix_subtract.\n");
        #endif 
        return NULL;
    }
    else if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: The two Matrix are not of the same order in matrix_subtract.\n");
        #endif 
        return NULL;
    }

    Matrix* subtraction = matrix_create(matrix1->rows, matrix1->cols);
    if (!subtraction) {
        fmt_fprintf(stderr, "Error: object creation failed for subtraction to matrix in matrix_subtract.\n");
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows; i++) {
        for (size_t j = 0; j < matrix1->cols; j++) {
            size_t index = i * matrix1->cols + j;
            subtraction->data[index] = matrix1->data[index] - matrix2->data[index];
        }
    }
    
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
    if (!matrix1) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix1 object is null and invalid in matrix_multiply.\n");
        #endif 
        return NULL;
    }
    else if (!matrix2) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix2 object is null and invalid in matrix_multiply.\n");
        #endif 
        return NULL;
    }
    else if (matrix1->cols != matrix2->rows) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Number of columns in matrix1 does not match the number of rows in matrix2 in matrix_multiply.\n");
        #endif 
        return NULL;
    }

    Matrix* product = matrix_create(matrix1->rows, matrix2->cols);
    if (!product) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Object creation failed for product matrix in matrix_multiply.\n");
        #endif 
        return NULL;
    }

    for (size_t i = 0; i < matrix1->rows; i++) {
        for (size_t j = 0; j < matrix2->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < matrix1->cols; k++) {
                sum += matrix1->data[i * matrix1->cols + k] * matrix2->data[k * matrix2->cols + j];
            }
            product->data[i * product->cols + j] = sum;
        }
    }
    
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stdout, "Info : this matrix object as param is null -- no need for deallocation in matrix_deallocate.\n");
        #endif 
        return;
    }

    free(matrix->data);
    free(matrix);

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success : matrix object deallocated successfully in matrix_deallocate.\n");
    #endif 
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_set.\n");
        #endif
        return false;
    }
    else if (rows >= matrix->rows || cols >= matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: rows or cols or both of them are biffger than matrix rows and cols in matrix_set.\n");
        #endif
        return false;
    }

    size_t row = matrix->cols * rows;
    matrix->data[row + cols] = value;

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success : set new value in matrix object in matrix_set.\n");
    #endif 
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stdout, "Info : this matrix object as param is null and invalid in matrix_print.\n");
        #endif 
        return;
    }

    // Find the maximum width needed for any element
    int max_width = 0;
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        int width = snprintf(NULL, 0, "%.5lf", matrix->data[i]);
        if (width > max_width) {
            max_width = width;
        }
    }

    for (size_t row = 0; row < matrix->rows; ++row) {
        fmt_printf("| ");
        for (size_t col = 0; col < matrix->cols; ++col) {
            size_t index = row * matrix->cols + col;
            fmt_printf("%*.*lf ", max_width, 5, matrix->data[index]);
        }
        fmt_printf("|\n");
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stdout, "Info : this matrix object as param is null and invalid in matrix_get.\n");
        #endif 
        exit(-1);
    }
    else if (row >= matrix->rows || col >= matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: rows or cols or both of them are bigger than matrix rows and cols in matrix_get.\n");
        #endif
        exit(-1);
    }

    size_t index = row * matrix->cols + col;
    return matrix->data[index];
}

static bool matrix_check_diagonal(const Matrix* mat, size_t i, size_t j) {
    double res = matrix_get(mat, i, j);
    while (++i < mat->rows && ++j < mat->cols) {
        if (matrix_get(mat, i, j) != res)
            return false;
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_scalar_multiply.\n");
        #endif
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            size_t index = i * matrix->cols + j;
            matrix->data[index] *= scalar;
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Matrix scalar multiplication completed successfully in matrix_scalar_multiply.\n");
    #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_is_square.\n");
        #endif
        return false;
    }
    if (matrix->rows == matrix->cols) {
        return true;
    }

    return false;
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
    Matrix* matrix = matrix_create(n, n);
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for identity matrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i == j) {
                matrix_set(matrix, i, j, 1.0);
            }
            else {
                matrix_set(matrix, i, j, 0.0);
            }
        }
    }

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
    if (!matrix1) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix1 object is null and invalid in matrix_is_equal.\n");
        #endif 
        return NULL;
    }
    else if (!matrix2) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix2 object is null and invalid in matrix_is_equal.\n");
        #endif 
        return NULL;
    }
    else if ((matrix1->rows != matrix2->rows) || (matrix1->cols != matrix2->cols)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: The two Matrix are not of the same order in matrix_is_equal.\n");
        #endif 
        return NULL;
    }

    for (size_t index = 0; index < (matrix1->rows * matrix1->cols); index++) {
        if (matrix1->data[index] != matrix2->data[index]) {
            return false;
        }
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_is_identity.\n");
        #endif 
        return false;
    }
    else if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is not square and invalid in matrix_is_identity.\n");
        #endif 
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            size_t index = i * matrix->cols + j;
            if (i == j) {
                if (matrix->data[index] != 1.0) { 
                    return false;
                }
            }
            else if (matrix->data[index] != 0.0) {
                return false;
            }
        }
    }

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
    if (!matrix || !matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: The matrix is null or not square in matrix_is_idempotent.\n");
        #endif
        return false;
    }

    Matrix* square = matrix_multiply(matrix, matrix);
    if (!square) {
        #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stderr, "Error: Matrix multiplication failed in matrix_is_idempotent.\n");
        #endif
        return false;
    }

    bool isIdempotent = matrix_is_equal(square, matrix);
    matrix_deallocate(square);

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_is_row.\n");
        #endif 
        return false;
    }
    return matrix->rows == 1? true: false;
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_is_columnar.\n");
        #endif 
        return false;
    }
    return matrix->cols == 1? true: false;
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_get_main_diagonal_as_column.\n");
        #endif 
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_get_main_diagonal_as_column.\n");
        #endif 
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(matrix->rows, 1); 
    if (!diagonalMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation failed for diagonal matrix in matrix_get_main_diagonal_as_column.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        matrix_set(diagonalMatrix, i, 0, matrix->data[i * matrix->cols + i]); 
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_get_main_diagonal_as_column.\n");
        #endif 
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_get_main_diagonal_as_column.\n");
        #endif 
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(1, matrix->cols); 
    if (!diagonalMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation failed for diagonal matrix in matrix_get_main_diagonal_as_column.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < matrix->cols; i++) {
        matrix_set(diagonalMatrix, 0, i, matrix->data[i * matrix->cols + i]); 
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_get_minor_diagonal_as_row.\n");
        #endif 
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_get_minor_diagonal_as_row.\n");
        #endif 
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(1, matrix->cols); 
    if (!diagonalMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation failed for diagonal matrix in matrix_get_minor_diagonal_as_row.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < matrix->cols; i++) {
        matrix_set(diagonalMatrix, 0, i, matrix->data[i * matrix->cols + (matrix->cols - 1 - i)]); 
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_get_minor_diagonal_as_column.\n");
        #endif 
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_get_minor_diagonal_as_column.\n");
        #endif 
        return NULL;
    }

    Matrix* diagonalMatrix = matrix_create(matrix->rows, 1); 
    if (!diagonalMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation failed for diagonal matrix in matrix_get_minor_diagonal_as_column.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        matrix_set(diagonalMatrix, i, 0, matrix->data[i * matrix->cols + (matrix->cols - 1 - i)]); 
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_transpose.\n");
        #endif 
        return NULL;
    }

    Matrix* transposed = matrix_create(matrix->cols, matrix->rows);
    if (!transposed) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: Memory allocation failed for transposed matrix in matrix_transpose.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            matrix_set(transposed, j, i, matrix->data[i * matrix->cols + j]);
        }
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_symmetric.\n");
        #endif 
        return false;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_is_symmetric.\n");
        #endif 
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = i + 1; j < matrix->cols; j++) { // Start from j = i + 1 to skip the diagonal
            if (matrix->data[i * matrix->cols + j] != matrix->data[j * matrix->cols + i]) {
                return false;
            }
        }
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_upper_triangular.\n");
        #endif 
        return false;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_is_upper_triangular.\n");
        #endif 
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < i; j++) { // Check below the main diagonal
            if (matrix->data[i * matrix->cols + j] != 0) {
                return false; 
            }
        }
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_lower_triangular.\n");
        #endif 
        return false;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_is_lower_triangular.\n");
        #endif 
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = i + 1; j < matrix->cols; j++) { // Check above the main diagonal
            if (matrix->data[i * matrix->cols + j] != 0) {
                return false; 
            }
        }
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_skew_symmetric.\n");
        #endif 
        return false;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_is_skew_symmetric.\n");
        #endif 
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        // check diagonal elements are zero
        if (matrix->data[i * matrix->cols + i] != 0) {
            return false;
        }

        for (size_t j = i + 1; j < matrix->cols; j++) {
            // check aij = -aji for non diagonal elements
            if (matrix->data[i * matrix->cols + j] != -matrix->data[j * matrix->cols + i]) {
                return false;
            }
        }
    }
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
    if (matrix->rows != matrix->cols) {
        fmt_fprintf(stderr, "Error: Determinant can only be calculated for square matrices.\n");
        return 0;
    }

    if (matrix->rows == 1) {
        return matrix->data[0];
    } 
    else if (matrix->rows == 2) {
        return matrix->data[0] * matrix->data[3] - matrix->data[1] * matrix->data[2];
    } 
    else {
        double det = 0;
        for (int j1 = 0; j1 < (int)matrix->cols; j1++) {
            Matrix* submatrix = matrix_create(matrix->rows - 1, matrix->cols - 1);
            
            // Generate submatrix
            for (int i = 1; i < (int)matrix->rows; i++) {
                int j2 = 0;
                for (int j = 0; j < (int)matrix->cols; j++) {
                    if (j == j1) {
                        continue;
                    }
                    matrix_set(submatrix, i - 1, j2++, matrix->data[i * matrix->cols + j]);
                }
            }
            det += (j1 % 2 == 0 ? 1 : -1) * matrix->data[j1] * matrix_determinant(submatrix);
            matrix_deallocate(submatrix);
        }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_trace.\n");
        #endif
        return 0.0; // Indicate error or undefined
    }

    if (matrix->rows != matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_trace.\n");
        #endif
        return 0.0; // Indicate error or undefined
    }

    double trace = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        trace += matrix->data[i * matrix->cols + i];
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Trace calculated successfully in matrix_trace.\n");
    #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input matrix is null in matrix_create_submatrix.\n");
        #endif
        return NULL;
    }
    if (excludeRow >= matrix->rows || excludeCol >= matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: excludeRow or excludeCol out of bounds in matrix_create_submatrix.\n");
        #endif
        return NULL;
    }

    Matrix* submatrix = matrix_create(matrix->rows - 1, matrix->cols - 1);
    if (!submatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for submatrix in matrix_create_submatrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0, sub_i = 0; i < matrix->rows; i++) {
        if (i == excludeRow) {
            continue; // Skip the excluded row
        }

        for (size_t j = 0, sub_j = 0; j < matrix->cols; j++) {
            if (j == excludeCol) {
                continue; // Skip the excluded column
            }
            double value = matrix_get(matrix, i, j); // Assumes matrix_get function correctly handles bounds
            matrix_set(submatrix, sub_i, sub_j, value); // Assumes matrix_set function correctly handles bounds and errors
            sub_j++;
        }
        sub_i++;
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Submatrix created successfully in matrix_create_submatrix.\n");
    #endif

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
    if (!matrix || !matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid input for matrix_adjugate.\n");
        #endif
        return NULL;
    }

    Matrix* cofactorMatrix = matrix_create(matrix->rows, matrix->cols);
    if (!cofactorMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for cofactorMatrix in matrix_adjugate.\n");
        #endif
        return NULL;
    }

    // Calculate cofactor matrix
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            Matrix* submatrix = matrix_create_submatrix(matrix, i, j); 
            double minor = matrix_determinant(submatrix); 
            double cofactor = pow(-1, i + j) * minor;

            matrix_set(cofactorMatrix, i, j, cofactor);
            matrix_deallocate(submatrix);
        }
    }

    // Transpose the cofactor matrix to get the adjugate
    Matrix* adjugate = matrix_transpose(cofactorMatrix);
    matrix_deallocate(cofactorMatrix);

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_inverse.\n");
        #endif
        return NULL;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_inverse.\n");
        #endif
        return NULL;
    }

    double det = matrix_determinant(matrix);
    if (det == 0) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix is singular and cannot be inverted in matrix_inverse.\n");
        #endif
        return NULL;
    }

    // Calculate the adjugate matrix
    Matrix* inverse = matrix_adjugate(matrix); // This function needs to be implemented as discussed
    if (!inverse) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to calculate adjugate matrix in matrix_inverse.\n");
        #endif
        return NULL;
    }

    // Create the inverse matrix by dividing the adjugate by the determinant
    for (size_t i = 0; i < inverse->rows; i++) {
        for (size_t j = 0; j < inverse->cols; j++) {
            inverse->data[i * inverse->cols + j] /= det;
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Matrix inversion completed successfully in matrix_inverse.\n");
    #endif

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input matrix is null in matrix_copy.\n");
        #endif
        return NULL;
    }

    Matrix* copy = matrix_create(matrix->rows, matrix->cols);
    if (!copy) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for matrix copy in matrix_copy.\n");
        #endif
        return NULL;
    }

    // Copy the data from the original matrix to the new copy
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            copy->data[i * matrix->cols + j] = matrix->data[i * matrix->cols + j];
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Matrix copied successfully in matrix_copy.\n");
    #endif

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_power.\n");
        #endif
        return NULL;
    }

    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_power.\n");
        #endif
        return NULL;
    }

    if (power < 0) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Negative power is not supported in matrix_power.\n");
        #endif
        return NULL;
    }

    // Handle the power of 0 separately, which should return an identity matrix
    if (power == 0) {
        return matrix_create_identity(matrix->rows);
    }

    // Initialize result as a copy of the original matrix for power = 1
    Matrix* result = matrix_copy(matrix); 
    if (power == 1) {
        return result;
    }

    Matrix* temp = NULL;
    while (power > 1) {
        if (power % 2 == 0) {
            temp = matrix_multiply(result, result);
            matrix_deallocate(result);

            result = temp;
            power /= 2;
        } 
        else {
            temp = matrix_multiply(result, matrix);
            matrix_deallocate(result);

            result = temp;
            power--;
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Matrix raised to power successfully in matrix_power.\n");
    #endif

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input matrix is null in matrix_rank.\n");
        #endif
        return -1; 
    }

    Matrix* tempMatrix = matrix_copy(matrix);
    if (!tempMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to copy matrix in matrix_rank.\n");
        #endif
        return -1;
    }

    int rank = tempMatrix->cols;
    for (int row = 0; row < rank; row++) {
        // If the diagonal element is effectively zero, try to find a non-zero element below it
        if (is_effectively_zero(tempMatrix->data[row * tempMatrix->cols + row])) {
            bool reduce = true;
            for (int i = row + 1; i < (int)tempMatrix->rows; i++) {
                if (!is_effectively_zero(tempMatrix->data[i * tempMatrix->cols + row])) {
                    matrix_swap_rows(tempMatrix, row, i);
                    reduce = false;
                    break;
                }
            }

            // If all elements in the current column below the diagonal are zero, reduce the rank
            if (reduce) {
                rank--;
                for (int i = 0; i < (int)tempMatrix->rows; i++) {
                    tempMatrix->data[i * tempMatrix->cols + row] = tempMatrix->data[i * tempMatrix->cols + rank];
                }
                row--;
            }
        } 
        else {
            // Make all elements below the diagonal in the current column zero
            for (int i = row + 1; i < (int)tempMatrix->rows; i++) {
                double mult = tempMatrix->data[i * tempMatrix->cols + row] / tempMatrix->data[row * tempMatrix->cols + row];
                for (int j = row; j < (int)tempMatrix->cols; j++) {
                    tempMatrix->data[i * tempMatrix->cols + j] -= mult * tempMatrix->data[row * tempMatrix->cols + j];
                }
            }
        }
    }

    matrix_deallocate(tempMatrix);
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_diagonal.\n");
        #endif
        return false;
    }

    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_is_diagonal.\n");
        #endif
        return false;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            // Check if the current element is outside the main diagonal and not zero
            if (i != j && !is_effectively_zero(matrix->data[i * matrix->cols + j])) {
                #ifdef MATRIX_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Found a non-zero element outside the main diagonal in matrix_is_diagonal.\n");
                #endif
                return false;
            }
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "The matrix is diagonal in matrix_is_diagonal.\n");
    #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_orthogonal.\n");
        #endif
        return false;
    }
    if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix is not square in matrix_is_orthogonal.\n");
        #endif
        return false;
    }

    Matrix* transpose = matrix_transpose(matrix);
    if (!transpose) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to calculate transpose in matrix_is_orthogonal.\n");
        #endif
        return false;
    }

    Matrix* product = matrix_multiply(matrix, transpose);
    if (!product) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to multiply matrix by its transpose in matrix_is_orthogonal.\n");
        #endif
        matrix_deallocate(transpose);  
        return false;
    }

    // Check if the product is an identity matrix
    bool isOrthogonal = matrix_is_identity(product);

    matrix_deallocate(transpose);
    matrix_deallocate(product);

    #ifdef MATRIX_LOGGING_ENABLE
        if (isOrthogonal) {
            fmt_fprintf(stdout, "The matrix is orthogonal in matrix_is_orthogonal.\n");
        } 
        else {
            fmt_fprintf(stderr, "The matrix is not orthogonal in matrix_is_orthogonal.\n");
        }
    #endif

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
    if (!matrix1 || !matrix2) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both matrix objects are null in matrix_kronecker_product.\n");
        #endif
        return NULL;
    }

    size_t m = matrix1->rows, n = matrix1->cols, p = matrix2->rows, q = matrix2->cols;
    Matrix* product = matrix_create(m * p, n * q);
    if (!product) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for the result matrix in matrix_kronecker_product.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < p; ++k) {
                for (size_t l = 0; l < q; ++l) {
                    double a = matrix_get(matrix1, i, j);  
                    double b = matrix_get(matrix2, k, l);  

                    matrix_set(product, i * p + k, j * q + l, a * b);  
                }
            }
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Kronecker product computed successfully in matrix_kronecker_product.\n");
    #endif
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
    if (!firstRow || !lastCol || firstRow->rows != 1 || lastCol->cols != 1) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input matrices are null or invalid dimensions in matrix_hankel.\n");
        #endif
        return NULL;
    }

    size_t n = firstRow->cols; 
    if (lastCol->rows != n) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Incompatible dimensions between first row and last column in matrix_hankel.\n");
        #endif
        return NULL;
    }

    Matrix* hankel = matrix_create(n, n);
    if (!hankel) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for Hankel matrix in matrix_hankel.\n");
        #endif
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
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Hankel matrix generated successfully in matrix_hankel.\n");
    #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null in matrix_is_hankel.\n");
        #endif
        return false;
    }
    for (size_t i = 0; i < matrix->rows - 1; i++) {
        for (size_t j = 0; j < matrix->cols - 1; j++) {
            if (i + j >= matrix->rows - 1) {
                continue; // Skip checks not relevant for a Hankel matrix
            }
            double value = matrix_get(matrix, i, j);
            // Check next element in the anti-diagonal
            if (i + 1 < matrix->rows && j > 0) {
                double next = matrix_get(matrix, i + 1, j - 1);
                if (!is_effectively_zero(value - next)) {
                    #ifdef MATRIX_LOGGING_ENABLE
                        fmt_fprintf(stderr, "The matrix is not Hankel at element [%zu, %zu].\n", i + 1, j - 1);
                    #endif
                    return false;
                }
            }
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "The matrix is Hankel.\n");
    #endif
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
    if (!firstRow || !firstCol) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input matrices (firstRow or firstCol) are null in matrix_toeplitz.\n");
        #endif
        return NULL;
    }
    if (firstRow->rows != 1) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: FirstRow should be a row vector in matrix_toeplitz.\n");
        #endif
        return NULL;
    }
    if (firstCol->cols != 1) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: FirstCol should be a column vector in matrix_toeplitz.\n");
        #endif
        return NULL;
    }

    size_t rows = firstCol->rows; 
    size_t cols = firstRow->cols;
    Matrix* toeplitzMatrix = matrix_create(rows, cols);
    if (!toeplitzMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for Toeplitz matrix in matrix_toeplitz.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            if (j >= i) {
                // Set value from first row
                double value = matrix_get(firstRow, 0, j - i);
                matrix_set(toeplitzMatrix, i, j, value);
            } 
            else {
                // Set value from first column
                double value = matrix_get(firstCol, i - j, 0);
                matrix_set(toeplitzMatrix, i, j, value);
            }
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Toeplitz matrix created successfully in matrix_toeplitz.\n");
    #endif
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
    if (!data) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input data is null in matrix_from_array.\n");
        #endif
        return NULL;
    }
    if (rows == 0 || cols == 0) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Rows or cols cannot be zero in matrix_from_array.\n");
        #endif
        return NULL;
    }

    Matrix* matrix = matrix_create(rows, cols);
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for matrix in matrix_from_array.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix->data[i * cols + j] = data[i * cols + j];
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Matrix created successfully from array in matrix_from_array.\n");
    #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null in matrix_is_toeplitz.\n");
        #endif
        return false;
    }

    // Check all elements in the first row
    for (size_t i = 0; i < matrix->cols; i++) {
        if (!matrix_check_diagonal(matrix, 0, i))
            return false;
    }

    // Check all elements in the first column
    for (size_t i = 1; i < matrix->rows; i++) {
        if (!matrix_check_diagonal(matrix, i, 0))
            return false;
    }

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
    if (!firstRow || firstRow->rows != 1) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input must be a single-row matrix in matrix_circulant.\n");
        #endif
        return NULL;
    }

    size_t n = firstRow->cols; // The number of columns in the first row defines the size of the square matrix
    Matrix* circulantMatrix = matrix_create(n, n);
    if (!circulantMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for circulant matrix in matrix_circulant.\n");
        #endif
        return NULL;
    }

    // Populate the circulant matrix
    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {
            // Calculate the index for the element in the first row that should be copied to the current position.
            // This calculation effectively rotates the elements of the first row to fill the matrix.
            size_t index = (col + row) % n;
            double value = matrix_get(firstRow, 0, index);
            matrix_set(circulantMatrix, row, col, value);
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Circulant matrix created successfully from the first row in matrix_circulant.\n");
    #endif
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
    if (n == 0) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: size is zero and its invalid for creating hilbert matrix.\n");
        #endif 
        return NULL;
    }

    Matrix* hilbertMatrix = matrix_create(n, n);
    if (!hilbertMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for hilbert matrix in matrix_hilbert.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            double value = (double)1.0 / ((i + 1) + (j + 1) - 1.0);
            if (!matrix_set(hilbertMatrix, i, j, value)) {
                #ifdef MATRIX_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Error: Can not set valid in hilbert matrix in matrix_hilbert.\n");
                #endif
                return NULL;
            }
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Hilbert matrix created successfully from the first row in matrix_hilbert.\n");
    #endif
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
    Matrix* helmertMatrix = matrix_create(n, full ? n : n - 1);
    if (!helmertMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for Helmert matrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == 0) {
                // First row
                matrix_set(helmertMatrix, i, j, 1.0 / sqrt(n));
            } 
            else if (j < i) {
                // Below diagonal for subsequent rows
                double value = 1.0 / sqrt(i * (i + 1.0));
                matrix_set(helmertMatrix, full ? i : i - 1, j, value);
            } 
            else if (j == i) {
                // Diagonal elements for subsequent rows
                double value = -sqrt((double)i / (i + 1.0));
                matrix_set(helmertMatrix, full ? i : i - 1, j, value);
            }
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Helmert matrix created successfully from the first row in matrix_helmert.\n");
    #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null in matrix_cofator.\n");
        #endif 
        return NULL;
    }
    else if (!matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object should be square in matrix_cofactor.\n");
        #endif 
        return NULL;
    }

    size_t n = matrix->rows;
    Matrix* cofactorMatrix = matrix_create(n, n);
    if (!cofactorMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for cofactor matrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            Matrix* submatrix = matrix_create_submatrix(matrix, i, j);
            if (!submatrix) {
                matrix_deallocate(cofactorMatrix);
                return NULL;
            }

            double det = matrix_determinant(submatrix);
            matrix_deallocate(submatrix);

            // Calculate the cofactor
            double cofactor = ((i + j) % 2 == 0 ? 1 : -1) * det;
            matrix_set(cofactorMatrix, i, j, cofactor);
        }
    }

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
    if (!matrix || matrix->rows != matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input must be a square matrix.\n");
        #endif
        return NULL;
    }

    size_t n = matrix->rows;
    Matrix* chol = matrix_create(n, n);
    if (!chol) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Memory allocation failed for Cholesky decomposition.\n");
        #endif
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
                    #ifdef MATRIX_LOGGING_ENABLE
                        fmt_fprintf(stderr, "Matrix is not positive definite.\n");
                    #endif

                    matrix_deallocate(chol);
                    return NULL;
                }
                matrix_set(chol, i, j, sqrt(sum));
            } 
            else {
                matrix_set(chol, i, j, sum / matrix_get(chol, i, i));
            }
        }
    }

    // Zero out the lower triangular part
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < i; j++) {
            matrix_set(chol, i, j, 0.0);
        }
    }

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
    if (!matrix || !matrix_is_square(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix must be square for LU decomposition.\n");
        #endif
        return false;
    }

    size_t n = matrix->rows;
    *L = matrix_create(n, n);
    *U = matrix_create(n, n);

    if (!(*L) || !(*U)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for L or U in LU decomposition.\n");
        #endif
        if (*L) {
            matrix_deallocate(*L);
        }
        if (*U) {
            matrix_deallocate(*U);
        }
        return false;
    }

    for (size_t i = 0; i < n; i++) {
        // Upper Triangular
        for (size_t k = i; k < n; k++) {
            double sum = 0.0;
            for (size_t j = 0; j < i; j++) {
                sum += matrix_get(*L, i, j) * matrix_get(*U, j, k);
            }
            matrix_set(*U, i, k, matrix_get(matrix, i, k) - sum);
        }

        // Lower Triangular
        for (size_t k = i; k < n; k++) {
            if (i == k) {
                matrix_set(*L, i, i, 1.0); // Diagonals of L are set to 1
            } 
            else {
                double sum = 0.0;
                for (size_t j = 0; j < i; j++) {
                    sum += matrix_get(*L, k, j) * matrix_get(*U, j, i);
                }
                matrix_set(*L, k, i, (matrix_get(matrix, k, i) - sum) / matrix_get(*U, i, i));
            }
        }
    }

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
    if (!A || A->rows < A->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix must have more rows than columns for QR decomposition in matrix_qr_decompositoin.\n");
        #endif
        return false;
    }

    size_t m = A->rows;
    size_t n = A->cols;

    *Q = matrix_create(m, n);
    *R = matrix_create(n, n);
    if (!Q || !R) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for Q && R in matrix_qr_decompositon.\n");
        #endif 
        return false;
    }

    double* a_col = (double*)malloc(sizeof(double) * m); 
    double* q_col = (double*)malloc(sizeof(double) * m); 
    if (!a_col || !q_col) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed in matrix_decomposition.\n");
        #endif 
        return false;
    }

    for (size_t i = 0; i < n; ++i) {
        // Copy the ith column of A to a_col
        for (size_t j = 0; j < m; ++j) {
            a_col[j] = matrix_get(A, j, i);
        }

        // For each column, apply the Householder transformation
        for (size_t k = 0; k < i; ++k) {
            for (size_t j = 0; j < m; ++j) {
                q_col[j] = matrix_get(*Q, j, k);
            }
            subtract_projection(a_col, q_col, m); // a_col -= projection of a_col onto q_col
        }

        normalize_vector(a_col, m); // Normalize a_col to get the ith column of Q

        // Set the ith column of Q to normalized a_col
        for (size_t j = 0; j < m; ++j) {
            matrix_set(*Q, j, i, a_col[j]);
        }

        // Calculate R values
        for (size_t j = 0; j < n; ++j) {
            for (size_t i = 0; i <= j; ++i) {
                // Compute the dot product of the ith orthonormal vector and the jth original column of A
                double r_ij = 0.0;
                for (size_t k = 0; k < m; ++k) {
                    r_ij += matrix_get(*Q, k, i) * matrix_get(A, k, j); 
                }
                matrix_set(*R, i, j, r_ij); // Set the computed value in R
            }
        }
    }

    free(a_col);
    free(q_col);

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
    Matrix* pascalMatrix = matrix_create(n, n);
    if (!pascalMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_printf(stderr, "Error: Memory allocation failed for Pascal matrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j <= i; j++) {
            // Pascal matrix is symmetric, so we can fill both upper and lower triangular parts
            double value = binomial_coefficient(i + j, i);
            matrix_set(pascalMatrix, i, j, value);
            matrix_set(pascalMatrix, j, i, value);
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Pascal matrix created successfully.\n");
    #endif
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
    double sum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            double value = matrix_get(matrix, i, j);
            sum += value * value;
        }
    }
    return sqrt(sum);
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
    double maxSum = 0.0;
    for (size_t j = 0; j < matrix->cols; j++) {
        double columnSum = 0.0;
        for (size_t i = 0; i < matrix->rows; i++) {
            columnSum += fabs(matrix_get(matrix, i, j));
        }
        if (columnSum > maxSum) {
            maxSum = columnSum;
        }
    }
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
    double maxSum = 0.0;
    for (size_t i = 0; i < matrix->rows; i++) {
        double rowSum = 0.0;
        for (size_t j = 0; j < matrix->cols; j++) {
            rowSum += fabs(matrix_get(matrix, i, j));
        }
        if (rowSum > maxSum) {
            maxSum = rowSum;
        }
    }
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
    if (matrix->rows != matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_inverse_gauss_jordan.\n");
        #endif 
        return NULL;
    }

    // Create an augmented matrix [A|I]
    size_t n = matrix->rows;
    Matrix* augmented = matrix_create(n, 2 * n);
    if (!augmented) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed in matrix_inverse_gauss_jordan.\n");
        #endif 
        return NULL;
    }

    // Fill the augmented matrix with [A|I]
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            augmented->data[i * 2 * n + j] = matrix->data[i * n + j]; 
            augmented->data[i * 2 * n + j + n] = (i == j) ? 1.0 : 0.0; 
        }
    }

    // Perform Gauss-Jordan elimination
    for (size_t col = 0; col < n; col++) {
        if (augmented->data[col * 2 * n + col] == 0) {
            // Find a non-zero element in the same column
            size_t swapRow = col + 1;
            while (swapRow < n && augmented->data[swapRow * 2 * n + col] == 0) {
                swapRow++;
            }
            if (swapRow == n) { 
                #ifdef MATRIX_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Error: index out of bounds in swapping rwo in matrix_inverse_gauss_jordan.\n");
                #endif 
                matrix_deallocate(augmented);
                return NULL;
            }
            matrix_swap_rows(augmented, col, swapRow);
        }

        matrix_row_divide(augmented, col, augmented->data[col * 2 * n + col]);

        // Eliminate the column entries below and above the pivot
        for (size_t row = 0; row < n; row++) {
            if (row != col) {
                matrix_row_subtract(augmented, row, col, augmented->data[row * 2 * n + col]);
            }
        }
    }

    // Extract the inverse matrix from the augmented matrix
    Matrix* inverse = matrix_create(n, n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            inverse->data[i * n + j] = augmented->data[i * 2 * n + j + n];
        }
    }

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
    if (!matrix || matrix->rows != matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix must be square to check if it's positive definite.\n");
        #endif
        return false;
    }

    // Matrix must also be symmetric for it to be positive definite
    if (!matrix_is_symmetric(matrix)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix is not symmetric in positive_definite.\n");
        #endif
        return false;
    }
    Matrix* chol = matrix_cholesky_decomposition(matrix);
    bool isPositiveDefinite = chol != NULL;

    // Deallocate the Cholesky matrix if it was successfully created
    if (isPositiveDefinite) {
        matrix_deallocate(chol);
    }
    else {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: failed in getting chlesky_decompostion in positive_definite.\n");
        #endif
        return false;
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Input matrix is null in matrix_projection.\n");
        #endif
        return NULL;
    }

    // Calculate transpose of matrix
    Matrix* matrixTranspose = matrix_transpose(matrix);
    if (!matrixTranspose) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Transpose calculation failed in matrix_projection.\n");
        #endif
        return NULL;
    }

    // Calculate m^T * m
    Matrix* mta = matrix_multiply(matrixTranspose, matrix);
    if (!mta) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Multiplication matrix^Tmatrix failed in matrix_projection.\n");
        #endif
        matrix_deallocate(matrixTranspose);
        return NULL;
    }

    // Calculate inverse of m^T * m
    Matrix* mtaInv = matrix_inverse(mta);
    if (!mtaInv) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Inverse calculation failed in matrix_projection.\n");
        #endif
        matrix_deallocate(matrixTranspose); 
        matrix_deallocate(mta); 
        return NULL;
    }

    // Calculate m * (m^T * m)^-1
    Matrix* m_mta_inv = matrix_multiply(matrix, mtaInv);
    if (!m_mta_inv) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Multiplication A*(A^TA)^-1 failed in matrix_projection.\n");
        #endif
        matrix_deallocate(matrixTranspose); // Clean up
        matrix_deallocate(mta); // Clean up
        matrix_deallocate(mtaInv); // Clean up
        return NULL;
    }

    // Calculate final projection matrix: A * (A^T * A)^-1 * A^T
    Matrix* projection = matrix_multiply(m_mta_inv, matrixTranspose);
    if (!projection) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Final projection matrix calculation failed in matrix_projection.\n");
        #endif
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null and invalid in matrix_vandermonde.\n");
        #endif 
        return NULL;
    }
    
    Matrix* vandermonde = matrix_create(n, n);
    if (!vandermonde) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for vandermonde in matrix_vandermonde.\n");
        #endif 
        return NULL; // Memory allocation failed
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            vandermonde->data[i * n + j] = pow(matrix->data[i], j);
        }
    }

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
    if (!coefficients) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null and invalid in matrix_companion.\n");
        #endif 
        return NULL;
    }
    
    size_t n = degree - 1; // For a polynomial of degree n, the companion matrix is nxn
    Matrix* companion = matrix_create(n, n);
    if (!companion) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for companion matrix.\n");
        #endif 
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (j == n - 1) { // Last column
                companion->data[i * n + j] = -coefficients->data[n - 1 - i] / coefficients->data[degree - 1];
            } 
            else if (i == j + 1) { // Sub-diagonal
                companion->data[i * n + j] = 1;
            } 
            else { // Other positions
                companion->data[i * n + j] = 0;
            }
        }
    }

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
    // Check if the matrix pointer is NULL
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null in matrix_fill.\n");
        #endif
        return false; // Exit the function early if matrix is NULL
    }

    // Check if the matrix data pointer is NULL
    if (!matrix->data) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix data is null in matrix_fill.\n");
        #endif
        return false; 
    }

    // Iterate over each element in the matrix and set it to the specified value
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            matrix->data[i * matrix->cols + j] = value;
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Matrix filled with value %lf in matrix_fill.\n", value);
    #endif
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
    // Check for NULL pointer in input matrix or function
    if (!matrix || !func) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL argument provided to matrix_map.\n");
        #endif
        return NULL;
    }

    // Allocate memory for the new matrix
    Matrix* result = (Matrix*)malloc(sizeof(Matrix));
    if (!result) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed in matrix_map.\n");
        #endif
        return NULL;
    }

    // Allocate memory for the new matrix data
    result->data = (double*)malloc(matrix->rows * matrix->cols * sizeof(double));
    if (!result->data) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation for matrix data failed in matrix_map.\n");
        #endif
        free(result); // Clean up previously allocated matrix structure
        return NULL;
    }

    result->rows = matrix->rows;
    result->cols = matrix->cols;

    // Apply the function to each element of the input matrix
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            result->data[i * matrix->cols + j] = func(matrix->data[i * matrix->cols + j]);
        }
    }

    #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stdout, "Success: Function applied to matrix in matrix_map.\n");
    #endif

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
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid matrix in matrix_min_element.\n");
        #endif
        return DBL_MAX; // Return the maximum double value as an error indicator
    }

    double min = DBL_MAX;
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
            if (value < min) {
                min = value;
            }
        }
    }

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
    if (!matrix || !matrix->data || matrix->rows == 0 || matrix->cols == 0) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid matrix in matrix_max_element.\n");
        #endif
        return -DBL_MAX; // Return the minimum double value as an error indicator
    }

    double max = -DBL_MAX;
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            double value = matrix->data[i * matrix->cols + j];
            if (value > max) {
                max = value;
            }
        }
    }
    
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
    if (!matrix || !func || row >= matrix->rows) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid arguments in matrix_apply_to_row.\n");
        #endif
        return false;
    }

    for (size_t j = 0; j < matrix->cols; ++j) {
        matrix->data[row * matrix->cols + j] = func(matrix->data[row * matrix->cols + j]);
    }
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
    if (!matrix || !func || col >= matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid arguments in matrix_apply_to_col.\n");
        #endif
        return false;
    }

    for (size_t i = 0; i < matrix->rows; ++i) {
        matrix->data[i * matrix->cols + col] = func(matrix->data[i * matrix->cols + col]);
    }

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
    if (!matrix || targetRow >= matrix->rows || sourceRow >= matrix->rows) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid arguments in matrix_row_addition.\n");
        #endif
        return false;
    }

    for (size_t j = 0; j < matrix->cols; ++j) {
        matrix->data[targetRow * matrix->cols + j] += scale * matrix->data[sourceRow * matrix->cols + j];
    }

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
    if (!matrix || targetCol >= matrix->cols || sourceCol >= matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid arguments in matrix_col_addition.\n");
        #endif
        return false;
    }

    for (size_t i = 0; i < matrix->rows; ++i) {
        matrix->data[i * matrix->cols + targetCol] += scale * matrix->data[i * matrix->cols + sourceCol];
    }

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
    if (!f) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_leslie")
        #endif 
    }
    // Check if sizes are compatible
    else if (f_size != s_size + 1) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "The length of s must be one less than the length of f in matrix_leslie.\n");
        #endif
        return NULL;
    }

    // Create an N x N matrix, where N is the size of the fecundity array f
    Matrix* leslie = matrix_create(f_size, f_size);
    if (!leslie) {
        fmt_fprintf(stderr, "Failed to create Leslie matrix.\n");
        return NULL;
    }

    // Set the first row with fecundity coefficients
    for (size_t i = 0; i < f_size; ++i) {
        matrix_set(leslie, 0, i, f->data[i]);
    }

    // Set the sub-diagonal with survival coefficients
    for (size_t i = 1; i < f_size; ++i) {
        matrix_set(leslie, i, i - 1, s->data[i - 1]);
    }
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_fiedler.\n");
        #endif 
        return NULL;
    }

    size_t n = matrix->cols >= matrix->rows? matrix->cols: matrix->rows; 
    Matrix* fiedler = matrix_create(n, n);

    if (!fiedler) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to create Fiedler matrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            // Set F[i, j] to the absolute difference between a[i] and a[j]
            double value = fabs(matrix->data[i] - matrix->data[j]);
            matrix_set(fiedler, i, j, value);
        }
    }

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
    Matrix* invH = matrix_create(n, n);
    if (!invH) {
        #ifdef MATRIX_LOGGING_ENABLE
        fmt_fprintf(stderr, "Error: Memory allocation failed for inverse Hilbert matrix.\n");
        #endif
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            int s = i + j;
            int64_t sign = (s % 2 == 0) ? 1 : -1;
            int64_t numerator = sign * (i + j + 1) * binomial_factorial(n + i, n - j - 1) * binomial_factorial(n + j, n - i - 1) * binomial_factorial(s, i) * binomial_factorial(s, j);
            int64_t denominator = 1; // The denominator for Hilbert matrix inverse entries when n <= 14 are effectively 1
            double value = (double)numerator / denominator;
            matrix_set(invH, i, j, value);
        }
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_get_row.\n");
        #endif 
        return NULL;
    }
    else if (row >= matrix->rows) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: row is bigger than matrix->row or less than zero in matrix_get_row.\n");
        #endif 
        return NULL;
    }

    Matrix* r = matrix_create(1, matrix->cols);
    if (!r) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for row in matrix_get_row.\n");
        #endif 
        return NULL;
    }
    
    for (size_t j = 0; j < matrix->cols; j++) {
        matrix_set(r, 0, j, matrix_get(matrix, row, j));
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_get_col.\n");
        #endif 
        return NULL;
    }
    else if (col >= matrix->cols) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: row is bigger than matrix->row or less than zero in matrix_get_col.\n");
        #endif 
        return NULL;
    }

    Matrix* c = matrix_create(matrix->rows, 1);
    if (!c) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for row in matrix_get_col.\n");
        #endif 
        return NULL;
    }

    for (size_t i = 0; i < matrix->rows; i++) {
        matrix_set(c, i, 0, matrix_get(matrix, i, col));
    }

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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null and invalid in matrix_to_array.\n");
        #endif 
        return NULL;
    }

    size_t size = matrix->rows * matrix->cols * sizeof(double);
    double* data = (double*) malloc(size);
    
    if (!data) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed in matrix_to_array.\n");
        #endif 
        return NULL;
    }

    memcpy(data, matrix->data, size);

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
    va_list args;
    size_t totalRows = 0, totalCols = 0;

    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        Matrix* mat = va_arg(args, Matrix*);
        totalRows += mat->rows;
        totalCols += mat->cols;
    }
    va_end(args);

    Matrix* result = matrix_create(totalRows, totalCols);
    if (!result) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for matrix in matrix_block_diag")
        #endif 
        return NULL; 
    }

    // copy data into the right positions
    size_t currentRow = 0, currentCol = 0;
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        Matrix* mat = va_arg(args, Matrix*);
        for (size_t r = 0; r < mat->rows; ++r) {
            memcpy(result->data + (currentRow + r) * totalCols + currentCol, mat->data + r * mat->cols, mat->cols * sizeof(double));
        }
        currentRow += mat->rows;
        currentCol += mat->cols;
    }
    va_end(args);

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
    if (!matrix || !matrix->data) {
        #ifdef MATRIX_LOGGING_ENABLE 
            fmt_fprintf(stderr, "Error: Matrix object is null and invalid in matrix_is_sparse.\n");
        #endif 
        return false; 
    }

    size_t totalElements = matrix->rows * matrix->cols;
    size_t nonZeroCount = 0;
    
    for (size_t i = 0; i < totalElements; ++i) {
        if (matrix->data[i] != 0) {
            ++nonZeroCount;
        }
    }

    // Calculate the percentage of non-zero elements
    double nonZeroPercentage = (double)nonZeroCount / (double)totalElements;

    // Consider the matrix sparse if the non-zero percentage is less than 30%
    return nonZeroPercentage < 0.3;
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
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null and invalid in matrix_size.\n");
        #endif 
        return 0;
    }

    return matrix->rows * matrix->cols;
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
    Matrix* matrix = matrix_create(row, col);
    srand(time(NULL));

    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: creation of Matrix object failed in matrix_random.\n");
        #endif
        return NULL; 
    }

    for (size_t i = 0; i < matrix_size(matrix); i++) {
        matrix->data[i] = (rand() % end) + start;
    }
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
    // Ensure 'n' is a power of 2
    if (n & (n - 1)) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: 'n' is not a power of 2 in matrix_walsh.\n");
        #endif 
        return NULL;
    }

    Matrix* walshMatrix = matrix_create(n, n);
    if (!walshMatrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for walsh in matrix_walsh.\n");
        #endif 
        return NULL;
    }
    generateWalshMatrixRecursively(walshMatrix->data, n, n, 0, 0, 1);

    return walshMatrix;
}