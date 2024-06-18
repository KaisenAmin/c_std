#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    Tuple* tuple1 = tuple_create(2);
    Tuple* tuple2 = tuple_create(2);
    Tuple* emptyTuple = tuple_create(0);

    int value1 = 100;
    int value2 = 200;
    tuple_set(tuple1, 0, &value1, sizeof(int));
    tuple_set(tuple1, 1, &value2, sizeof(int));
    tuple_set(tuple2, 0, &value2, sizeof(int));
    tuple_set(tuple2, 1, &value1, sizeof(int));

    fmt_printf("Tuple1 is less than Tuple2: %s\n", tuple_is_less(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is greater than Tuple2: %s\n", tuple_is_greater(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is equal to Tuple2: %s\n", tuple_is_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is not equal to Tuple2: %s\n", tuple_is_not_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is greater or equal to Tuple2: %s\n", tuple_is_greater_or_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is less or equal to Tuple2: %s\n", tuple_is_less_or_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is empty: %s\n", tuple_is_empty(tuple1) ? "true" : "false");
    fmt_printf("Empty Tuple is empty: %s\n", tuple_is_empty(emptyTuple) ? "true" : "false");

    tuple_deallocate(tuple1);
    tuple_deallocate(tuple2);

    return 0;
}