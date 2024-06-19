#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_even(const void *n) {
    int num = *(int *)n;
    return num % 2 == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int result[10] = {0};

    fmt_printf("Source array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    size_t count = algorithm_remove_copy_if(arr, 10, sizeof(int), result, is_even);

    fmt_printf("Result array:\n");
    for (size_t i = 0; i < count; ++i) {
        fmt_printf("%d ", result[i]);
    }
    fmt_printf("\n");

    return 0;
}