#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_doubles(const KeyType a, const KeyType b) {
    const double* da = (const double*)a;
    const double* db = (const double*)b;

    if (*da < *db) {
        return -1;
    } 
    else if (*da > *db) { 
        return 1;
    } 
    else {
        return 0;
    }
}

void double_deallocator(void* data) {
    free(data);
}

int main() {
    Map* myMap = map_create(compare_doubles, double_deallocator, double_deallocator);
    double keys[] = {1.1, 2.2, 3.3, 4.4, 5.5};

    for (int i = 0; i < 5; ++i) {
        double* key = malloc(sizeof(double));
        double* value = malloc(sizeof(double));
        *key = keys[i];
        *value = keys[i] * 10;
        map_insert(myMap, key, value);
    }

    for (MapIterator it = map_begin(myMap); it.node != map_end(myMap).node; map_iterator_increment(&it)) {
        double* key = (double*)map_node_get_key(it.node);
        double* value = (double*)map_node_get_value(it.node);
        
        if (key && value) {
            fmt_printf("%f: %f\n", *key, *value);
        }
    }

    map_deallocate(myMap);
    return 0;
}