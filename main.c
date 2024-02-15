#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create a 4x4 identity matrix
    Matrix* identityMatrix = matrix_create_identity(4);

    double det = matrix_determinant(identityMatrix);
    fmt_printf("Determinant of the 4x4 identity matrix is: %lf\n", det);

    matrix_deallocate(identityMatrix);
    return 0;
}
