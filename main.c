#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double dataA[] = {1, 0, 1, 0}; // 2x2 matrix
    Matrix* A = matrix_from_array(dataA, 2, 2);
    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    double dataB[] = {3, 4, 5, 6, 7, 8}; // 2x3 matrix
    Matrix* B = matrix_from_array(dataB, 2, 3);
    if (!B) {
        fmt_fprintf(stderr, "Failed to create matrix B.\n");
        matrix_deallocate(A);
        return -1;
    }

    double dataC[] = {7}; // 1x1 matrix
    Matrix* C = matrix_from_array(dataC, 1, 1);
    if (!C) {
        fmt_fprintf(stderr, "Failed to create matrix C.\n");
        matrix_deallocate(A);
        matrix_deallocate(B);
        return -1;
    }

    // Combine A, B, and C into a block diagonal matrix
    Matrix* blockDiagonal = matrix_block_diag(3, A, B, C);
    if (!blockDiagonal) {
        fmt_fprintf(stderr, "Failed to create block diagonal matrix.\n");
        matrix_deallocate(A);
        matrix_deallocate(B);
        matrix_deallocate(C);
        return -1;
    }

    fmt_printf("Block Diagonal Matrix:\n");
    matrix_print(blockDiagonal);

    matrix_deallocate(A);
    matrix_deallocate(B);
    matrix_deallocate(C);
    matrix_deallocate(blockDiagonal);

    return 0;
}
