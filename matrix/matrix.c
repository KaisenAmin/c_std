#include "matrix.h"
#include "../fmt/fmt.h"



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

double matrix_get(Matrix* matrix, size_t row, size_t col) {
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

double matrix_determinant(Matrix* matrix) {
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
                    if (j == j1) continue;
                    matrix_set(submatrix, i - 1, j2++, matrix->data[i * matrix->cols + j]);
                }
            }
            det += (j1 % 2 == 0 ? 1 : -1) * matrix->data[j1] * matrix_determinant(submatrix);
            matrix_deallocate(submatrix);
        }
        return det;
    }
}