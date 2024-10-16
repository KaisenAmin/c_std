#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double f[] = {0.1, 2.0, 1.0, 0.1}; // Fecundity coefficients
    double s[] = {0.2, 0.8, 0.7};      // Survival coefficients
    size_t f_size = sizeof(f) / sizeof(f[0]);
    size_t s_size = sizeof(s) / sizeof(s[0]);

    Matrix* fl = matrix_from_array(f, 1, 3);
    Matrix* sl = matrix_from_array(s, 1, 3);
    
    if (!fl || !sl) {
        fmt_fprintf(stderr, "Error: can not create Matrix from array fl or sl or both of them");
        return -1;
    }

    Matrix* leslieMatrix = matrix_leslie(fl, f_size, sl, s_size);
    if (leslieMatrix) {
        fmt_printf("Leslie Matrix:\n");
        matrix_print(leslieMatrix);
        matrix_deallocate(leslieMatrix);

    }

    matrix_deallocate(fl);
    matrix_deallocate(sl);
    return 0;
}