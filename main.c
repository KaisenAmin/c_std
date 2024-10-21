#include "algorithm/algorithm.h"
#include "fmt/fmt.h"


int main() {
    unsigned long arr_ulong[10];
    unsigned long start_ulong = 1000;
    
    algorithm_iota(algorithm_begin(arr_ulong), algorithm_end(arr_ulong, 10, sizeof(unsigned long)), &start_ulong, sizeof(unsigned long), TYPE_UNSIGNED_LONG);

    fmt_printf("Unsigned long array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%lu\n", arr_ulong[i]);
    }
    fmt_printf("\n");

    unsigned char arr_char[10];
    unsigned char start_char = 'A'; 
    
    algorithm_iota(arr_char, arr_char + 10, &start_char, sizeof(unsigned char), TYPE_UNSIGNED_CHAR);

    fmt_printf("Unsigned Char array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%c\n", arr_char[i]);
    }
    fmt_printf("\n");

    float arr_float[10];
    float start_float = 1.5f; 

    algorithm_iota(arr_float, arr_float + 10, &start_float, sizeof(float), TYPE_FLOAT);

    fmt_printf("Float array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%f\n", arr_float[i]);
    }

    return 0;
}
