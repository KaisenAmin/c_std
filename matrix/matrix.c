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
double binomial_coefficient(int n, int k) {
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

// Function to swap two rows of a matrix
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

void matrix_row_divide(Matrix* matrix, size_t row, double scalar) {
    for (size_t col = 0; col < matrix->cols; col++) {
        matrix->data[row * matrix->cols + col] /= scalar;
    }
}

void matrix_row_subtract(Matrix* matrix, size_t targetRow, size_t subtractRow, double scalar) {
    for (size_t col = 0; col < matrix->cols; col++) {
        matrix->data[targetRow * matrix->cols + col] -= scalar * matrix->data[subtractRow * matrix->cols + col];
    }
}

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

void matrix_print(Matrix* matrix) {
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stdout, "Info : this matrix object as param is null and invalid in matrix_print.\n");
        #endif 
        return;
    }

    int max_width = 1;  // Minimum width for "0 "
    for (size_t i = 0; i < matrix->rows * matrix->cols; ++i) {
        if (matrix->data[i] != 0) {
            int width = snprintf(NULL, 0, "%.5lf", matrix->data[i]);
            if (width > max_width) {
                max_width = width;
            }
        }
    }

    for (size_t row = 0; row < matrix->rows; ++row) {
        fmt_printf("|");
        for (size_t col = 0; col < matrix->cols; ++col) {
            size_t index = row * matrix->cols + col;
            if (col == 0) { // First column
                if (matrix->data[index] == 0) {
                    fmt_printf(" 0"); 
                } 
                else {
                    fmt_printf("%.5lf", matrix->data[index]); 
                }
            } 
            else { 
                if (matrix->data[index] == 0) {
                    fmt_printf("%*s", max_width, "0"); // Align "0" within the calculated width
                } 
                else {
                    fmt_printf("%*.5lf", max_width, matrix->data[index]);
                }
            }
            fmt_printf(" "); // Space between columns
        }
        fmt_printf("|\n");
    }
}

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

// Checks if a matrix is idempotent: A^2 = A
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

bool matrix_is_row(const Matrix* matrix) {
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_is_row.\n");
        #endif 
        return false;
    }
    return matrix->rows == 1? true: false;
}

bool matrix_is_columnar(const Matrix* matrix) {
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE  
            fmt_fprintf(stderr, "Error: matrix object is null and invalid in matrix_is_columnar.\n");
        #endif 
        return false;
    }
    return matrix->cols == 1? true: false;
}

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

// Calculate the trace of a square matrix
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

// Function to compute the Kronecker product of two matrices
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

// Function to generate a Hankel matrix from its first row and last column matrices
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

// Function to check matrix is Hankle or not 
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

// Function to create a Toeplitz matrix given its first row and column
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

// Function to create Matrix from given array of double 
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

// Function to create circulant matrix from a given matrix first_row .
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

// Function to create hilbert Matrix from give size 
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

// Function to create a Helmert matrix
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

// Function to create a Pascal matrix
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

// Function to compute the Frobenius norm of a matrix
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

// Function to compute the L1 norm of a matrix (maximum column sum)
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

// Function to compute the Infinity norm of a matrix (maximum row sum)
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

// Calculate the projection matrix onto the column space of A
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

// Generate a companion matrix from polynomial coefficients
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

// function to apply an other function to each element of matrix 
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

// function to find minimum element in the matrix 
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

// function to find the maximum element in the matrix
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

// Apply a function to each element of a specified row
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

// Apply a function to each element of a specified column
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

// Add one row to another after optionally scaling it
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

// Add one column to another after optionally scaling it
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

// Function to create a Leslie matrix
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

// Function to create a Fiedler matrix from a given array
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

// Function to create the inverse of a Hilbert matrix
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

// function to return specefic row or a matrix 
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

// function to return specefic col of a matrix 
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

// function to create a block diagonal matrix from provide Matrix params 
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

// Function to determine if a matrix is sparse
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

size_t matrix_size(const Matrix *matrix) {
    if (!matrix) {
        #ifdef MATRIX_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Matrix object is null and invalid in matrix_size.\n");
        #endif 
        return 0;
    }

    return matrix->rows * matrix->cols;
}

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

// Function to create a Walsh Matrix of size 'n'
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