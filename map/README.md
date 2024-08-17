# Map Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

## Overview

The Map library provides a generic implementation of a map (also known as an associative array) in C. This map is implemented as a Red-Black Tree, which is a type of self-balancing binary search tree. This ensures efficient operations like insertion, deletion, and lookup, all with a time complexity of O(log n).

## Features

- **Generic Key and Value Types:** The map can store any type of key-value pairs.
- **Red-Black Tree Implementation:** Guarantees O(log n) time complexity for basic operations.
- **Iterators:** The library provides iterators to traverse the map in both forward and reverse directions.
- **Flexible Memory Management:** Customizable deallocation functions for keys and values.
- **Comprehensive Operations:** Includes insertion, deletion, lookup, size, swap, and more.

## Usage

To use the Map library, include the `map.h` header file in your C program. The implementation resides in `map.c`, which should be compiled along with your program.

## Function Explanations

### Core Functions

- **`Map* map_create(CompareFuncMap comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue);`**
  - **Description:** Creates and initializes a new map with the provided comparison and deallocation functions.
  - **Parameters:**
    - `comp`: Function pointer for comparing keys.
    - `deallocKey`: Function pointer for deallocating keys.
    - `deallocValue`: Function pointer for deallocating values.
  - **Returns:** Pointer to the created map.

- **`void map_deallocate(Map* map);`**
  - **Description:** Deallocates all memory associated with the map, including all nodes and their keys and values.

- **`bool map_insert(Map* map, KeyType key, ValueType value);`**
  - **Description:** Inserts a key-value pair into the map. If the key already exists, its value is updated.
  - **Parameters:**
    - `map`: Pointer to the map.
    - `key`: Pointer to the key to insert.
    - `value`: Pointer to the value associated with the key.
  - **Returns:** `true` if the insertion was successful, `false` otherwise.

- **`ValueType map_at(const Map* map, KeyType key);`**
  - **Description:** Retrieves the value associated with a given key in the map.
  - **Parameters:**
    - `map`: Pointer to the map.
    - `key`: Pointer to the key to search for.
  - **Returns:** Pointer to the value if the key exists, `NULL` otherwise.

- **`bool map_erase(Map* map, KeyType key);`**
  - **Description:** Removes the element with the specified key from the map.
  - **Parameters:**
    - `map`: Pointer to the map.
    - `key`: Pointer to the key to remove.
  - **Returns:** `true` if the key was successfully removed, `false` otherwise.

- **`size_t map_size(const Map* map);`**
  - **Description:** Returns the number of elements currently stored in the map.

- **`bool map_empty(const Map* map);`**
  - **Description:** Checks if the map is empty.
  - **Returns:** `true` if the map is empty, `false` otherwise.

### Iterator Functions

- **`MapIterator map_begin(const Map* map);`**
  - **Description:** Returns an iterator to the first element in the map.

- **`MapIterator map_end(const Map* map);`**
  - **Description:** Returns an iterator to the position after the last element in the map.

- **`MapIterator map_rbegin(const Map* map);`**
  - **Description:** Returns a reverse iterator to the last element in the map.

- **`MapIterator map_rend(const Map* map);`**
  - **Description:** Returns a reverse iterator to the position before the first element in the map.

- **`void map_iterator_increment(MapIterator* it);`**
  - **Description:** Advances the iterator to the next element in the map.

- **`void map_iterator_decrement(MapIterator* it);`**
  - **Description:** Moves the iterator to the previous element in the map.

### Advanced Operations

- **`void map_swap(Map* map1, Map* map2);`**
  - **Description:** Exchanges the contents of two maps.

- **`void map_clear(Map* map);`**
  - **Description:** Removes all elements from the map, making it empty.

- **`Map* map_copy(const Map* src);`**
  - **Description:** Creates a copy of the given map.

### Example Programs

The following examples demonstrate how to use the Map library.

### Example 1: Create a Map, Insert Elements, and Retrieve Values

```c
#include "fmt/fmt.h"
#include "map/map.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;
    return (*ia - *ib);
}

void int_deallocator(void* data) {
    free(data);
}

int main() {
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);
    int key = 5, value = 10;
    int lookupKey = 5;

    map_insert(myMap, &key, &value);
    
    int *foundValue = map_at(myMap, &lookupKey);
    if (foundValue) {
        fmt_printf("Found value: %d\n", *foundValue);
    } else {
        fmt_printf("Key not found.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 2: Use Different Types for Keys and Values

```c
#include "fmt/fmt.h"
#include "map/map.h"
#include "string/string.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;
    return (*ia - *ib);
}

void string_deallocator(void* data) {
    free(data);
}

int main() {
    Map* myMap = map_create(compare_ints, free, string_deallocator);
    int* key1 = malloc(sizeof(int));
    *key1 = 1;

    char* value1 = string_strdup("Alice");
    int* key2 = malloc(sizeof(int));
    *key2 = 2;

    char* value2 = string_strdup("Bob");

    map_insert(myMap, key1, value1);
    map_insert(myMap, key2, value2);

    int lookupKey = 1;
    char* foundValue = map_at(myMap, &lookupKey);

    if (foundValue) {
        fmt_printf("Found value for %d: %s\n", lookupKey, foundValue);
    } else {
        fmt_printf("Key not found.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 3: Get the Size of the Map

```c
#include "fmt/fmt.h"
#include "map/map.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;
    return (*ia - *ib);
}

int main() {
    Map* map1 = map_create(compare_ints, NULL, free);
    int key1 = 1;
    char* value1 = string_strdup("Hello");

    map_insert(map1, &key1, value1);

    fmt_printf("Size: %zu\n", map_size(map1));
    fmt_printf("Value for key 1: %s\n", (char*)map_at(map1, &key1));

    map_deallocate(map1);
    return 0;
}
```

### Example 4: Swap Two Map Objects

```c
#include "fmt/fmt.h"
#include "map/map.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;
    return (*ia - *ib);
}

int main() {
    Map* map3a = map_create(compare_ints, NULL, NULL);
    int key3a = 1, value3a = 100;

    map_insert(map3a, &key3a, &value3a);

    Map* map3b = map_create(compare_ints, NULL, NULL);
    int key3b = 2, value3b = 200;

    map_insert(map3b, &key3b, &value3b);
    map_swap(map3a, map3b);

    fmt_printf("Value in map3a for key 2: %d\n", *(int*)map_at(map3a, &key3b));
    fmt_printf("Value in map3b for key 1: %d\n", *(int*)map_at(map3b, &key3a));

    map_deallocate(map3a);
    map_deallocate(map3b);
    return 0;
}
```

### Example 5

: Erase a Key-Value Pair

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}

void int_deallocator(void* data) {
    free(data);
}

int main() {
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);

    for (int i = 0; i < 5; ++i) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(myMap, key, value);
    }

    int searchKey = 3;
    int* foundValue = map_at(myMap, &searchKey);
    if (foundValue) {
        fmt_printf("Found value: %d\n", *foundValue);
    } else {
        fmt_printf("Key not found.\n");
    }

    int deleteKey = 2;
    if (map_erase(myMap, &deleteKey)) {
        fmt_printf("Key %d erased.\n", deleteKey);
    } else {
        fmt_printf("Key not found for deletion.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 6: Increment and Decrement Iterators

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int compare_strings(const KeyType a, const KeyType b) {
    return strcmp((const char*)a, (const char*)b);
}

void string_deallocator(void* data){
    free(data);
}

int main() {
    Map* map1 = map_create(compare_strings, string_deallocator, string_deallocator);
    map_insert(map1, string_strdup("apple"), string_strdup("red"));
    map_insert(map1, string_strdup("banana"), string_strdup("yellow"));

    Map* map2 = map_create(compare_strings, string_deallocator, string_deallocator);
    map_insert(map2, string_strdup("cherry"), string_strdup("red"));
    map_insert(map2, string_strdup("grape"), string_strdup("green"));

    fmt_printf("Size of Map1 before swap: %zu\n", map_size(map1));
    fmt_printf("Map1 before swap:\n");

    for (MapIterator it = map_begin(map1); it.node != map_end(map1).node; map_iterator_increment(&it)) {
        fmt_printf("%s: %s\n", (char*)map_node_get_key(it.node), (char*)map_node_get_value(it.node));
    }
    map_swap(map1, map2);

    fmt_printf("Map1 after swap:\n");
    for (MapIterator it = map_begin(map1); it.node != map_end(map1).node; map_iterator_increment(&it)) {
        fmt_printf("%s: %s\n", (char*)map_node_get_key(it.node), (char*)map_node_get_value(it.node));
    }

    fmt_printf("Before decrementing iterator:\n");
    MapIterator it = map_rbegin(map2);

    do {
        fmt_printf("%s: %s\n", (char*)map_node_get_key(it.node), (char*)map_node_get_value(it.node));        
        map_iterator_decrement(&it);

    } while (it.node != NULL);

    map_deallocate(map1);
    map_deallocate(map2);
    return 0;
}
```

### Example 7: Lower Bound and Upper Bound

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_doubles(const KeyType a, const KeyType b) {
    const double* da = (const double*)a;
    const double* db = (const double*)b;

    if (*da < *db) {
        return -1;
    } else if (*da > *db) { 
        return 1;
    } else { 
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

    double queryKey = 3.0;
    MapIterator lower = map_lower_bound(myMap, &queryKey);
    MapIterator upper = map_upper_bound(myMap, &queryKey);

    fmt_printf("Lower bound for %f: %f\n", queryKey, *((double*)(lower.node)->key));
    fmt_printf("Upper bound for %f: %f\n", queryKey, *((double*)(upper.node)->key));

    map_deallocate(myMap);
    return 0;
}
```

### Example 8: `map_node_get_key` and `map_node_get_value`

```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>

int compare_doubles(const KeyType a, const KeyType b) {
    const double* da = (const double*)a;
    const double* db = (const double*)b;

    if (*da < *db) {
        return -1;
    } else if (*da > *db) { 
        return 1;
    } else {
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
```

### Example 9: Use String Objects in a Map

```c
#include "map/map.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

int compare_strings(const KeyType a, const KeyType b) {
    String* strA = *(String**)a;
    String* strB = *(String**)b;
    return strcmp(string_c_str(strA), string_c_str(strB));
}

void string_deallocator(void* data) {
    if (data != NULL) {
        String** strPtr = (String**)data;
        
        if (*strPtr != NULL) {
            string_deallocate(*strPtr);
            *strPtr = NULL;
        }
    }
}

int main() {
    Map* myMap = map_create(compare_strings, string_deallocator, string_deallocator);
    String* key1 = string_create("key1");
    String* value1 = string_create("Hello");
    String* key2 = string_create("key2");
    String* value2 = string_create("World");

    map_insert(myMap, &key1, &value1);
    map_insert(myMap, &key2, &value2);

    fmt_printf("Map contents:\n");
    for (MapIterator it = map_begin(myMap); it.node != map_end(myMap).node; map_iterator_increment(&it)) {
        String* key = *(String**)map_node_get_key(it.node);
        String* value = *(String**)map_node_get_value(it.node);
        fmt_printf("%s: %s\n", string_c_str(key), string_c_str(value));
    }

    if (map_erase(myMap, &key2)) {
        fmt_printf("Removed 'key2' from the map.\n");
    } else {
        fmt_printf("Key 'key2' not found for removal.\n");
    }

    String* searchKey = string_create("key1");
    String** foundValue = map_at(myMap, &searchKey);

    if (foundValue) {
        fmt_printf("Found value for 'key1': %s\n", string_c_str(*foundValue));
    } else {
        fmt_printf("Key 'key1' not found.\n");
    }

    string_deallocate(searchKey);
    map_deallocate(myMap);
    return 0;
}
```

### Example 10: Merging Two Maps

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}

void int_deallocator(void* data) {
    free(data);
}

void map_merge(Map* dest, Map* src) {
    for (MapIterator it = map_begin(src);

 it.node != map_end(src).node; map_iterator_increment(&it)) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));

        *key = *(int*)map_node_get_key(it.node);
        *value = *(int*)map_node_get_value(it.node);
        map_insert(dest, key, value);
    }
}

int main() {
    Map* map1 = map_create(compare_ints, int_deallocator, int_deallocator);
    Map* map2 = map_create(compare_ints, int_deallocator, int_deallocator);

    for (int i = 0; i < 3; ++i) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(map1, key, value);
    }

    for (int i = 2; i < 5; ++i) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 200;
        map_insert(map2, key, value);
    }

    map_merge(map1, map2);

    for (MapIterator it = map_begin(map1); it.node != map_end(map1).node; map_iterator_increment(&it)) {
        fmt_printf("%d: %d\n", *(int*)map_node_get_key(it.node), *(int*)map_node_get_value(it.node));
    }

    map_deallocate(map1);
    map_deallocate(map2);
    return 0;
}
```

### Example 11: Filtering a Map Based on a Condition

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

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
    (void)value;
    return (*(int*)key) % 2 == 0;
}

int main() {
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);

    for (int i = 0; i < 5; ++i) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(myMap, key, value);
    }

    map_filter(myMap, filter_even_keys);

    for (MapIterator it = map_begin(myMap); it.node != map_end(myMap).node; map_iterator_increment(&it)) {
        fmt_printf("%d: %d\n", *(int*)map_node_get_key(it.node), *(int*)map_node_get_value(it.node));
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 12: `map_print`

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    return *(int*)a - *(int*)b;
}

void int_deallocator(void* data) {
    free(data);
}

void print_int(const KeyType key) {
    fmt_printf("%d", *(const int*)key);
}

void print_string(const ValueType value) {
    fmt_printf("%s", (const char*)value);
}

int main() {
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);
    char* value = string_strdup("Hello, World");
    int key = 5;

    map_insert(myMap, &key, value);
    map_print(myMap, print_int, print_string);

    map_deallocate(myMap);
    free(value);

    return 0;
}
```

### Example 13: `map_copy`

```c
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
```
