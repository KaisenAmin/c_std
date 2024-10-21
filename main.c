#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    unsigned long arr[10];
    unsigned long start_val = 1000;
    
    algorithm_iota(arr, arr + 10, &start_val, sizeof(unsigned long), sizeof(unsigned long));

    fmt_printf("Unsigned long array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%lu\n", arr[i]);
    }

    return 0;
}