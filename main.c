#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* arr1 = array_create(sizeof(int), 3);
    Array* arr2 = array_create(sizeof(int), 3);
    int arr1Values[] = {1, 2, 3};
    int arr2Values[] = {1, 2, 3};

    for (size_t i = 0; i < 3; ++i) {
        array_set(arr1, i, &arr1Values[i]);
        array_set(arr2, i, &arr2Values[i]);
    }

    if (array_is_equal(arr1, arr2)) {
        fmt_printf("Arrays are equal.\n");
    }
    else {
        fmt_printf("Arrays are not equal.\n");
    }

    array_deallocate(arr1);
    array_deallocate(arr2);

    return 0;
}