#include "matrix.h"
#include "../fmt/fmt.h"
#include <math.h>

#define EPSILON 1e-9

// Function to check if a floating-point number is effectively zero
static bool is_effectively_zero(double value) {
    return fabs(value) < EPSILON;
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

