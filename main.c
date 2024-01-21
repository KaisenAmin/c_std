#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    return *(int*)a - *(int*)b;
}

void int_deallocator(void* data) {
    free(data);
}

void print_key(const KeyType key) {
    fmt_printf("%d", *(const int*)key);
}

void print_value(const ValueType value) {
    fmt_printf("%d", *(const int*)value);
}

int main() {
    Map* originalMap = map_create(compare_ints, int_deallocator, int_deallocator);
    int key = 10;
    int value = 20;

    map_insert(originalMap, &key, &value);
    Map* copiedMap = map_copy(originalMap);

    map_print(copiedMap, print_key, print_value);
    fmt_printf("Size of the copied map: %zu\n", map_size(copiedMap));

    map_deallocate(originalMap);
    map_deallocate(copiedMap);
    return 0;
}