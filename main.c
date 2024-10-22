#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* arr = array_create(sizeof(int), 5);

    for (size_t i = 0; i < array_size(arr); ++i) {
        int value = (int)(i * 10); // Example values: 0, 10, 20, 30, 40
        array_set(arr, i, &value);
    }

    array_clear(arr);
    fmt_printf("Array Size After Clear: %zu\n", array_size(arr));
    
    array_deallocate(arr);
    return 0;
}