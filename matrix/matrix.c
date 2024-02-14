#include "matrix.h"
#include "../fmt/fmt.h"
#include <stdlib.h>


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