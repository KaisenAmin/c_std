# Map Library in C

Author: amin tahmasebi
Release Date: 2023
License: ISC License


## Overview
This C library provides a generic implementation of a map, also known as an associative array. The map is implemented as a Red-Black Tree, which is a self-balancing binary search tree. This ensures efficient operations such as insertion, deletion, and lookup.

## Features
- **Generic Key and Value Types:** Can store any type of key-value pairs.
- **Red-Black Tree Implementation:** Guarantees O(log n) time complexity for basic operations.
- **Iterators:** Support for forward and backward traversal.
- **Flexible Memory Management:** Customizable deallocation functions for keys and values.
- **Operations:** Insertion, deletion, lookup, size, swap, and more.

## Usage
Include the `map.h` header file in your C program to use the map functionality. The `map.c` file contains the implementation and should be compiled with your program.

## Function Explanations

- `map_create`: Initializes a new map with the provided comparison and deallocation functions.
- `map_deallocate`: Frees all memory associated with the map.
- `map_begin`: Returns an iterator to the first element of the map.
- `map_end`: Returns an iterator to the end of the map (after the last element).
- `map_rbegin`: Returns a reverse iterator to the last element of the map.
- `map_rend`: Returns a reverse iterator to the position before the first element of the map.
- `map_cbegin`: Returns a constant iterator to the first element of the map.
- `map_cend`: Returns a constant iterator to the end of the map.
- `map_crbegin`: Returns a constant reverse iterator to the last element of the map.
- `map_crend`: Returns a constant reverse iterator to the position before the first element of the map.
- `map_empty`: Checks if the map is empty.
- `map_size`: Returns the number of elements in the map.
- `map_max_size`: Returns the maximum possible number of elements in the map.
- `map_at`: Accesses the value associated with a key in the map.
- `map_insert`: Inserts a key-value pair into the map.
- `map_erase`: Removes the element with the specified key from the map.
- `map_swap`: Exchanges the contents of two maps.
- `map_clear`: Removes all elements from the map.
- `map_emplace`: Inserts a new element into the map if the key does not exist.
- `map_emplace_hint`: Inserts a new element into the map with a hint on the position.
- `map_key_comp`: Returns the comparison function used by the map.
- `map_find`: Finds an element with a specific key.
- `map_count`: Returns the number of elements matching a specific key.
- `map_lower_bound`: Returns an iterator to the first element not less than a given key.
- `map_upper_bound`: Returns an iterator to the first element greater than a given key.
- `map_equal_range`: Returns a range of elements matching a specific key.
- `map_node_get_key`: Retrieves the key of a map node.
- `map_node_get_value`: Retrieves the value of a map node.
- `map_iterator_increment`: Advances the iterator to the next element.
- `map_iterator_decrement`: Moves the iterator to the previous element.
- `map_print`: Prints all elements of the map using provided print functions.
- `map_copy`: Creates a copy of the given map.

---

## Example 1 : how to create map and insert and get value by  `map_create, map_insert, map_at`
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
    // int foundValue = *(int*)map_at(myMap, &lookupKey);
    if (foundValue) {
        fmt_printf("Found value: %d\n", *foundValue);
    }
    else {
        fmt_printf("Key not found.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 2 : Map is Genreic accept different kind of key and value type 

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
    char* foundValue = (char*)map_at(myMap, &lookupKey);

    if (foundValue) {
        fmt_printf("Found value for %d: %s\n", lookupKey, foundValue);
    }
    else {
        fmt_printf("Key not found.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 3 : how to get size of Map wit `map_size`
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

### Example 4 : swap two Map object 
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

    // After swap, map3a should have the contents of map3b and vice versa
    fmt_printf("Value in map3a for key 2: %d\n", *(int*)map_at(map3a, &key3b));
    fmt_printf("Value in map3b for key 1: %d\n", *(int*)map_at(map3b, &key3a));

    map_deallocate(map3a);
    map_deallocate(map3b);
    return 0;
}
```

### Example 5 : erase Key and value in Map 
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

    // Insert elements
    for (int i = 0; i < 5; ++i) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(myMap, key, value);
    }

    // Search for a key
    int searchKey = 3;
    int* foundValue = (int*)map_at(myMap, &searchKey);
    if (foundValue) {
        fmt_printf("Found value: %d\n", *foundValue);
    }
    else {
        fmt_printf("Key not found.\n");
    }
    // Delete a key
    int deleteKey = 2;
    if (map_erase(myMap, &deleteKey)) {
        fmt_printf("Key %d erased.\n", deleteKey);
    }
    else { 
        fmt_printf("Key not found for deletion.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 6 : decrement and increment iterator

```c
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>
#include <string.h>

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
        MapNode* node = it.node;
        fmt_printf("%s: %s\n", (char*)node->key, (char*)node->value);
    }
    map_swap(map1, map2);

    fmt_printf("Map1 after swap:\n");
    for (MapIterator it = map_begin(map1); it.node != map_end(map1).node; map_iterator_increment(&it)) {
        MapNode* node = it.node;
        fmt_printf("%s: %s\n", (char*)node->key, (char*)node->value);
    }

    fmt_printf("Before do-while decrement iterator\n");
    MapIterator it = map_rbegin(map2);

    // Iterate backwards through the map
    do {
        MapNode* node = it.node;
        fmt_printf("%s: %s\n", (char*)node->key, (char*)node->value);        
        map_iterator_decrement(&it);// Move to the previous element

    } while (it.node != NULL);

    map_deallocate(map1);
    map_deallocate(map2);
    return 0;
}
```

### Example 7 : lower and upper bound with `map_lower_bound` and `map_upper_bound`

```c
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

    // Insert elements
    double keys[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    for (int i = 0; i < 5; ++i) {
        double* key = malloc(sizeof(double));
        double* value = malloc(sizeof(double));
        *key = keys[i];
        *value = keys[i] * 10;
        
        map_insert(myMap, key, value);
    }

    // Find lower and upper bounds
    double queryKey = 3.0;
    MapIterator lower = map_lower_bound(myMap, &queryKey);
    MapIterator upper = map_upper_bound(myMap, &queryKey);

    fmt_printf("Lower bound for %f: %f\n", queryKey, *((double*)(lower.node)->key));
    fmt_printf("Upper bound for %f: %f\n", queryKey, *((double*)(upper.node)->key));

    map_deallocate(myMap);
    return 0;
}
```

### Example 8 : `map_node_get_key` , `and map_node_get_value` 

```c
#include "map/map.h"
#include <stdio.h>
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

    for (int i = 0; i < 5; ++i)
    {
        double* key = malloc(sizeof(double));
        double* value = malloc(sizeof(double));
        *key = keys[i];
        *value = keys[i] * 10;
        map_insert(myMap, key, value);
    }

    // Iterate over the map and print keys and values
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

### Example 9 : Use String Obj in Map 
```c
#include "map/map.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

// Compare function for String keys
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
            *strPtr = NULL;  // Set the pointer to NULL after deallocating
        }
    }
}

int main() {
    // Create a map with String keys and String values
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
    }
    else {
        fmt_printf("Key 'key2' not found for removal.\n");
    }

    if (map_erase(myMap, &key2)) { 
        fmt_printf("Removed 'key2' from the map.\n");
    }
    else { 
        fmt_printf("Key 'key2' not found for removal.\n");
    }
    // Search for a key
    String* searchKey = string_create("key1");
    String** foundValue = map_at(myMap, &searchKey);

    if (foundValue) {
        fmt_printf("Found value for 'key1': %s\n", string_c_str(*foundValue));
    }
    else { 
        fmt_printf("Key 'key1' not found.\n");
    }

    string_deallocate(searchKey);
    map_deallocate(myMap);
    return 0;
}
```

### Example 10: Merging Two Maps

**C Implementation:**

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
    for (MapIterator it = map_begin(src); it.node != map_end(src).node; map_iterator_increment(&it)) {
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

    // Populate map1
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

**C++ Implementation:**

```cpp
#include <iostream>
#include <map>

int main() {
    std::map<int, int> map1, map2;

    for (int i = 0; i < 3; ++i) { 
        map1[i] = i * 100;
    }
    for (int i = 2; i < 5; ++i) { 
        map2[i] = i * 200;
    }
    map1.insert(map2.begin(), map2.end());

    for (const auto& pair : map1) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
```

### Example 11: Filtering a Map Based on a Condition

**C Implementation:**

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

**C++ Implementation:**

```cpp
#include <iostream>
#include <map>

bool filter_even_keys(const std::pair<const int, int>& pair) {
    return pair.first % 2 == 0;
}

int main() {
    std::map<int, int> myMap;

    for (int i = 0; i < 5; ++i) { 
        myMap[i] = i * 100;
    }
    for (auto it = myMap.begin(); it != myMap.end(); ) {
        if (!filter_even_keys(*it)) {
            it = myMap.erase(it);
        }
        else {
            ++it;
        }
    }
    for (const auto& pair : myMap) { 
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
```

## Example 12 : `map_print` 

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

## Example 13 : `map_copy`

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