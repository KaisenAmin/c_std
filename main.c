#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}

void int_deallocator(void* data) {
    free(data);
}

void map_filter(Map* map, bool (*filter_func)(KeyType key, ValueType value)) {
    MapIterator it = map_begin(map);
    while (it.node != map_end(map).node) {
        MapIterator current = it;
        map_iterator_increment(&it);

        if (!filter_func(map_node_get_key(current.node), map_node_get_value(current.node))) {
            map_erase(map, map_node_get_key(current.node));
        }
    }
}

bool filter_even_keys(KeyType key, ValueType value) {
    assert(value != NULL);
    return (*(int*)key) % 2 == 0;
}

int main() {
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);

    // Insert elements
    for (int i = 0; i < 5; ++i) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(myMap, key, value);
    }

    // Filter map to keep only even keys
    map_filter(myMap, filter_even_keys);

    // Print filtered map
    for (MapIterator it = map_begin(myMap); it.node != map_end(myMap).node; map_iterator_increment(&it)) {
        printf("%d: %d\n", *(int*)map_node_get_key(it.node), *(int*)map_node_get_value(it.node));
    }

    map_deallocate(myMap);

    return 0;
}
