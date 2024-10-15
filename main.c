#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    size_t size1, size2;

    Tuple* tuple1 = tuple_create(1);
    int intValue1 = 42;
    tuple_set(tuple1, 0, &intValue1, sizeof(int));

    Tuple* tuple2 = tuple_create(1);
    int intValue2 = 100;
    tuple_set(tuple2, 0, &intValue2, sizeof(int));

    fmt_printf("Before swap:\n");
    fmt_printf("Tuple1 first element: %d\n", *(int*)tuple_get(tuple1, 0, &size1));
    fmt_printf("Tuple2 first element: %d\n", *(int*)tuple_get(tuple2, 0, &size2));

    tuple_swap(tuple1, tuple2);

    fmt_printf("After swap:\n");
    fmt_printf("Tuple1 first element: %d\n", *(int*)tuple_get(tuple1, 0, &size1));
    fmt_printf("Tuple2 first element: %d\n", *(int*)tuple_get(tuple2, 0, &size2));

    tuple_deallocate(tuple1);
    tuple_deallocate(tuple2);
    
    return 0;
}