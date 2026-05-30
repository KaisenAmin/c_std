# Map Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

## Overview

The Map library provides a generic implementation of a map (also known as an associative array). This map is implemented as a Red-Black Tree, which is a type of self-balancing binary search tree. This ensures efficient operations like insertion, deletion, and lookup, all with a time complexity of O(log n).

## Features

- **Generic Key and Value Types:** The map can store any type of key-value pairs.
- **Red-Black Tree Implementation:** Guarantees O(log n) time complexity for basic operations.
- **Iterators:** The library provides iterators to traverse the map in both forward and reverse directions.
- **Flexible Memory Management:** Customizable deallocation functions for keys and values.
- **Comprehensive Operations:** Includes insertion, deletion, lookup, size, swap, and more.

## Usage

To use the Map library, include the `map.h` header file in your C program. The implementation resides in `map.c`, which should be compiled along with your program.

### Map Structures 

### `MapNode`
The `MapNode` structure represents an individual node in the Red-Black Tree that underlies the `Map`. Each node contains a key-value pair and pointers to its left child, right child, and parent. The `color` field is used in the Red-Black Tree balancing mechanism, where nodes are either red or black to maintain the tree's properties.

- **Fields:**
  - `void* key`: A pointer to the key stored in the node.
  - `void* value`: A pointer to the value associated with the key.
  - `struct MapNode* left`: A pointer to the node's left child.
  - `struct MapNode* right`: A pointer to the node's right child.
  - `struct MapNode* parent`: A pointer to the node's parent.
  - `int color`: The color of the node, used for maintaining the Red-Black Tree's balancing properties. The value is typically either `RED` or `BLACK`.

---

### `MapEntry`
The `MapEntry` structure is a simple structure that encapsulates a key-value pair. It represents an element of the map, storing the key and its corresponding value. This structure is typically used when manipulating the elements in the map.

- **Fields:**
  - `KeyType key`: The key associated with the value.
  - `ValueType value`: The value associated with the key.

---

### `MapIterator`
The `MapIterator` structure provides a way to traverse the `Map`. The iterator maintains a reference to a node in the Red-Black Tree and is used to iterate over the key-value pairs stored in the map. It can be advanced or reversed to navigate through the elements of the map in order.

- **Fields:**
  - `MapNode* node`: A pointer to the current node in the map that the iterator points to.

---

### `MapIteratorPair`
The `MapIteratorPair` structure is a utility structure that holds two iterators. It is often used when performing operations like `equal_range()`, which returns a range of elements between two iterators, or when working with lower and upper bounds in a map.

- **Fields:**
  - `MapIterator first`: The first iterator in the pair.
  - `MapIterator second`: The second iterator in the pair.

---

## Function Explanations

### `CompareFuncMap map_key_comp(const Map* map)`
**Purpose**: Returns the comparison function used to order elements in the map.
**Parameters**:
- `map`: A pointer to the `Map` whose comparison function is requested.
**Return Value**: A pointer to the comparison function, or `NULL` if `map` is `NULL`.
**Usage Case**: Use when you need to retrieve the ordering function to perform manual key comparisons outside the map.

---

### `MapIteratorPair map_equal_range(const Map* map, KeyType key)`
**Purpose**: Returns a pair of iterators [lower_bound, upper_bound] for the given key.
**Parameters**:
- `map`: A pointer to the `Map` in which to search.
- `key`: The key to compare the elements against.
**Return Value**: A `MapIteratorPair` where `first` is the lower bound and `second` is the upper bound; returns an empty pair if `map` or `key` is `NULL`.
**Usage Case**: Use when you need both the lower and upper bound for a key in a single call, for example to determine the exact range a key occupies.

---

### `KeyType map_node_get_key(MapNode* node)`
**Purpose**: Retrieves the key stored in a map node.
**Parameters**:
- `node`: A pointer to the `MapNode` from which to retrieve the key.
**Return Value**: The key stored in the node, or `NULL` if `node` is `NULL`.
**Usage Case**: Use when iterating over map nodes and you need to read the key at the current position.

---

### `ValueType map_node_get_value(MapNode* node)`
**Purpose**: Retrieves the value stored in a map node.
**Parameters**:
- `node`: A pointer to the `MapNode` from which to retrieve the value.
**Return Value**: The value stored in the node, or `NULL` if `node` is `NULL`.
**Usage Case**: Use when iterating over map nodes and you need to read the value at the current position.

---

### `Map* map_create(CompareFuncMap comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue)`
**Purpose**: Creates and initializes a new empty map backed by a Red-Black Tree.
**Parameters**:
- `comp`: A function pointer for comparing keys; required to determine key ordering.
- `deallocKey`: A function pointer to deallocate keys on node deletion; may be `NULL` when the map does not own key memory.
- `deallocValue`: A function pointer to deallocate values on node deletion; may be `NULL` when the map does not own value memory.
**Return Value**: A pointer to the newly created map, or `NULL` if `comp` is `NULL` or memory allocation fails.
**Usage Case**: Use at the start of a program to create a new map with a custom comparator and optional memory management callbacks.

---

### `Map* map_copy(const Map* src)`
**Purpose**: Creates a shallow copy of the map where nodes are independent but key/value pointers are shared.
**Parameters**:
- `src`: A pointer to the source `Map` to copy.
**Return Value**: A pointer to the new copy, or `NULL` if `src` is `NULL` or allocation fails.
**Usage Case**: Use when you need a structurally independent map that shares the same key/value data without risking double-free (the copy's dealloc functions are set to `NULL`).

---

### `void map_deallocate(Map* map)`
**Purpose**: Frees all nodes (calling registered dealloc functions) and the map struct itself.
**Parameters**:
- `map`: A pointer to the `Map` to be deallocated.
**Return Value**: `None`.
**Usage Case**: Use when the map is no longer needed to release all associated memory; the pointer must not be used afterward.

---

### `void map_clear(Map* map)`
**Purpose**: Removes all elements from the map and resets the size to zero while leaving the map struct valid.
**Parameters**:
- `map`: A pointer to the `Map` to be cleared.
**Return Value**: `None`.
**Usage Case**: Use when you want to empty a map for reuse without destroying the map struct itself.

---

### `void map_swap(Map* map1, Map* map2)`
**Purpose**: Swaps the root, size, comparator, and dealloc functions of two maps.
**Parameters**:
- `map1`: A pointer to the first `Map`.
- `map2`: A pointer to the second `Map`.
**Return Value**: `None`.
**Usage Case**: Use to exchange the full contents of two maps in O(1) time without copying individual elements.

---

### `void map_iterator_increment(MapIterator* it)`
**Purpose**: Advances the iterator to the next element in in-order (ascending key) traversal.
**Parameters**:
- `it`: A pointer to the `MapIterator` to be incremented.
**Return Value**: `None`.
**Usage Case**: Use inside a forward iteration loop to move to the next key-value pair; `it->node` becomes `NULL` when past the last element.

---

### `void map_iterator_decrement(MapIterator* it)`
**Purpose**: Moves the iterator to the previous element in in-order traversal.
**Parameters**:
- `it`: A pointer to the `MapIterator` to be decremented.
**Return Value**: `None`.
**Usage Case**: Use inside a reverse iteration loop to move to the preceding key-value pair.

---

### `void map_print(const Map* map, void (*printKey)(const KeyType), void (*printValue)(const ValueType))`
**Purpose**: Traverses the map in ascending key order and prints every key-value pair using the caller-supplied print callbacks.
**Parameters**:
- `map`: A pointer to the `Map` to be printed.
- `printKey`: A function pointer for printing a key.
- `printValue`: A function pointer for printing a value.
**Return Value**: `None`.
**Usage Case**: Use for debugging or logging the full contents of a map with type-specific formatting.

---

### `size_t map_size(const Map* map)`
**Purpose**: Returns the number of key-value pairs in the map.
**Parameters**:
- `map`: A pointer to the `Map` whose size is to be determined.
**Return Value**: The number of elements in the map, or `0` if `map` is `NULL`.
**Usage Case**: Use to check how many entries exist in the map before iterating or making capacity decisions.

---

### `size_t map_max_size(const Map* map)`
**Purpose**: Returns the theoretical maximum number of elements the map could hold based on system memory limits.
**Parameters**:
- `map`: A pointer to the `Map` whose maximum size is to be determined.
**Return Value**: The theoretical maximum element count, or `0` if `map` is `NULL`.
**Usage Case**: Use when you need an upper bound on map capacity for validation or pre-condition checks.

---

### `size_t map_count(const Map* map, KeyType key)`
**Purpose**: Returns 1 if `key` exists in the map, 0 otherwise, since the map does not allow duplicate keys.
**Parameters**:
- `map`: A pointer to the `Map` to be searched.
- `key`: The key to check for in the map.
**Return Value**: `1` if the key is found, `0` if not found or `map` is `NULL`.
**Usage Case**: Use to test for key existence when you do not need the associated value.

---

### `bool map_empty(const Map* map)`
**Purpose**: Returns `true` if the map has no elements or is `NULL`, `false` otherwise.
**Parameters**:
- `map`: A pointer to the `Map` to check.
**Return Value**: `true` if the map is empty or `NULL`; `false` if it contains at least one element.
**Usage Case**: Use before iterating or performing operations that require a non-empty map.

---

### `ValueType map_at(const Map* map, KeyType key)`
**Purpose**: Returns the value associated with `key`, or `NULL` if not found.
**Parameters**:
- `map`: A pointer to the `Map` to search.
- `key`: The key to look up.
**Return Value**: The value associated with the key, or `NULL` if the key is not found or `map` is `NULL`.
**Usage Case**: Use for direct key-to-value lookup when you expect the key to be present.

---

### `MapIterator map_begin(const Map* map)`
**Purpose**: Returns an iterator to the smallest (first in-order) element in the map.
**Parameters**:
- `map`: A pointer to the `Map` whose first element is requested.
**Return Value**: A `MapIterator` pointing to the first element, or a `NULL`-node iterator if the map is empty or `NULL`.
**Usage Case**: Use as the starting point of a forward iteration loop over the map.

---

### `MapIterator map_end(const Map* map)`
**Purpose**: Returns a sentinel iterator representing the position past the last element (node == NULL).
**Parameters**:
- `map`: A pointer to the `Map` whose end iterator is requested.
**Return Value**: A `MapIterator` with `node == NULL` serving as the past-the-end sentinel.
**Usage Case**: Use as the termination condition for forward iteration loops.

---

### `MapIterator map_rbegin(const Map* map)`
**Purpose**: Returns an iterator to the largest (last in-order) element for reverse traversal.
**Parameters**:
- `map`: A pointer to the `Map` whose last element in reverse is requested.
**Return Value**: A `MapIterator` pointing to the last element, or a `NULL`-node iterator if the map is empty or `NULL`.
**Usage Case**: Use as the starting point of a reverse iteration loop over the map.

---

### `MapIterator map_rend(const Map* map)`
**Purpose**: Returns a sentinel iterator representing the position before the first element (node == NULL) for reverse iteration.
**Parameters**:
- `map`: A pointer to the `Map` whose reverse end iterator is requested.
**Return Value**: A `MapIterator` with `node == NULL` serving as the before-the-beginning sentinel.
**Usage Case**: Use as the termination condition for reverse iteration loops.

---

### `MapIterator map_cbegin(const Map* map)`
**Purpose**: Same as `map_begin` but signals read-only intent; the iterator type is identical.
**Parameters**:
- `map`: A pointer to the `Map` whose first element is requested for read-only access.
**Return Value**: A `MapIterator` pointing to the first element, or a `NULL`-node iterator if the map is empty or `NULL`.
**Usage Case**: Use in const contexts where you want to document that the iteration is read-only.

---

### `MapIterator map_cend(const Map* map)`
**Purpose**: Same as `map_end` but signals read-only intent.
**Parameters**:
- `map`: A pointer to the `Map` whose constant end iterator is requested.
**Return Value**: A `MapIterator` with `node == NULL` serving as the past-the-end sentinel for read-only traversal.
**Usage Case**: Use as the termination condition for read-only forward iteration loops.

---

### `MapIterator map_crbegin(const Map* map)`
**Purpose**: Same as `map_rbegin` but signals read-only intent.
**Parameters**:
- `map`: A pointer to the `Map` whose last element in reverse is requested for read-only access.
**Return Value**: A `MapIterator` pointing to the last element, or a `NULL`-node iterator if the map is empty or `NULL`.
**Usage Case**: Use as the starting point of a read-only reverse iteration loop.

---

### `MapIterator map_crend(const Map* map)`
**Purpose**: Same as `map_rend` but signals read-only intent.
**Parameters**:
- `map`: A pointer to the `Map` whose reverse end iterator is requested for read-only access.
**Return Value**: A `MapIterator` with `node == NULL` serving as the before-the-beginning sentinel for read-only reverse traversal.
**Usage Case**: Use as the termination condition for read-only reverse iteration loops.

---

### `MapIterator map_find(const Map* map, KeyType key)`
**Purpose**: Searches for `key` and returns an iterator to it, or `map_end` if not found.
**Parameters**:
- `map`: A pointer to the `Map` to search.
- `key`: The key to search for.
**Return Value**: A `MapIterator` pointing to the matching element, or the end iterator if the key is not found.
**Usage Case**: Use when you need both to test for key existence and to obtain an iterator for subsequent operations.

---

### `MapIterator map_lower_bound(const Map* map, KeyType key)`
**Purpose**: Returns an iterator to the first element whose key is >= `key`, or `map_end` if none.
**Parameters**:
- `map`: A pointer to the `Map` to search.
- `key`: The key to use as the lower threshold.
**Return Value**: A `MapIterator` pointing to the first element with key >= `key`, or the end iterator if no such element exists.
**Usage Case**: Use for range queries where you need the first element at or above a given key threshold.

---

### `MapIterator map_upper_bound(const Map* map, KeyType key)`
**Purpose**: Returns an iterator to the first element whose key is strictly > `key`, or `map_end` if none.
**Parameters**:
- `map`: A pointer to the `Map` to search.
- `key`: The key to use as the upper threshold.
**Return Value**: A `MapIterator` pointing to the first element with key strictly > `key`, or the end iterator if no such element exists.
**Usage Case**: Use for range queries where you need the first element strictly above a given key value.

---

### `bool map_insert(Map* map, KeyType key, ValueType value)`
**Purpose**: Inserts a new key-value pair; if the key already exists, the existing value is replaced (calling `deallocValue` on the old one if set).
**Parameters**:
- `map`: A pointer to the `Map` where the key-value pair will be inserted.
- `key`: The key to insert.
- `value`: The value associated with the key.
**Return Value**: `true` on success; `false` if `map` or `key` is `NULL`, or if memory allocation fails.
**Usage Case**: Use as the primary way to add or update key-value pairs in the map.

---

### `bool map_emplace(Map* map, KeyType key, ValueType value)`
**Purpose**: Inserts a key-value pair only if the key does not already exist in the map.
**Parameters**:
- `map`: A pointer to the `Map` where the key-value pair should be inserted.
- `key`: The key to insert.
- `value`: The value to associate with the key.
**Return Value**: `true` on successful insertion; `false` if the key already exists or if `map` or `key` is `NULL`.
**Usage Case**: Use when you want insert-only semantics and must not overwrite an existing value.

---

### `bool map_emplace_hint(Map* map, MapIterator hint, KeyType key, ValueType value)`
**Purpose**: Like `map_emplace` but accepts a hint iterator that may speed up insertion when the hint is close to the correct position.
**Parameters**:
- `map`: A pointer to the `Map` where the key-value pair should be inserted.
- `hint`: A `MapIterator` providing a positional hint for faster insertion.
- `key`: The key to insert.
- `value`: The value to associate with the key.
**Return Value**: `true` on successful insertion; `false` if the key already exists or if `map` or `key` is `NULL`.
**Usage Case**: Use when you already have an iterator near the expected insertion point and want to optimize performance.

---

### `bool map_erase(Map* map, KeyType key)`
**Purpose**: Removes the node with `key` from the tree, calls the configured dealloc functions, rebalances the Red-Black Tree, and decrements the size.
**Parameters**:
- `map`: A pointer to the `Map` from which the element should be erased.
- `key`: The key of the element to remove.
**Return Value**: `true` on success; `false` if the key is not found or `map` is `NULL`.
**Usage Case**: Use to delete a specific key-value pair from the map by key.

---

### Examples

### Example 1: Create a Map, Insert Elements, and Retrieve Values

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "map/map.h"


int compare_ints(const KeyType a, const KeyType b) {
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;
    return (*ia - *ib);
}


void int_deallocator(void* data) {
    free(data);
}


int main() {
    Map* myMap = map_create(compare_ints, NULL, NULL);
    int key = 5, value = 10;
    int lookupKey = 5;

    map_insert(myMap, &key, &value);
    
    int *foundValue = (int*)map_at(myMap, &lookupKey);
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
**Result**
```
Found value: 10
```

---

### Example 2: Use Different Types for Keys and Values

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "map/map.h"
#include "string/std_string.h"


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
    int* key1 = (int*) malloc(sizeof(int));
    *key1 = 1;

    char* value1 = string_strdup("Alice");
    int* key2 = (int*) malloc(sizeof(int));
    *key2 = 2;

    char* value2 = string_strdup("Bob");

    map_insert(myMap, key1, value1);
    map_insert(myMap, key2, value2);

    int lookupKey = 1;
    char* foundValue = (char*) map_at(myMap, &lookupKey);

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
**Result**
```
Found value for 1: Alice
```

---

### Example 3: Get the Size of the Map

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "map/map.h"
#include "string/std_string.h"


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
**Result**
```
Size: 1
Value for key 1: Hello
```

---

### Example 4: Swap Two Map Objects

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "map/map.h"


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
**Result**
```
Value in map3a for key 2: 200
Value in map3b for key 1: 100
```


---

### Example 5 : Erase a Key-Value Pair

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"


int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}


void int_deallocator(void* data) {
    free(data);
}


int main() {
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);

    for (int i = 0; i < 5; ++i) {
        int* key = (int*) malloc(sizeof(int));
        int* value = (int*) malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(myMap, key, value);
    }

    int searchKey = 3;
    int* foundValue = (int*) map_at(myMap, &searchKey);
    if (foundValue) {
        fmt_printf("Found value: %d\n", *foundValue);
    } 
    else {
        fmt_printf("Key not found.\n");
    }

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
**Result**
```
Found value: 300
Key 2 erased.
```

---

### Example 6: Increment and Decrement Iterators

```c
#include <stdlib.h>
#include <string.h>
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/std_string.h"


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
**Result**
```
Size of Map1 before swap: 2
Map1 before swap:
apple: red
banana: yellow
Map1 after swap:
cherry: red
grape: green
Before decrementing iterator:
banana: yellow
apple: red
```

---

### Example 7: Lower Bound and Upper Bound

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"


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
        double* key = (double*) malloc(sizeof(double));
        double* value = (double*) malloc(sizeof(double));
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
**Result**
```
Lower bound for 3.000000: 3.300000
Upper bound for 3.000000: 3.300000
```

---

### Example 8: `map_node_get_key` and `map_node_get_value`

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"


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
        double* key = (double*) malloc(sizeof(double));
        double* value = (double*) malloc(sizeof(double));
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
**Result**
```
1.100000: 11.000000
2.200000: 22.000000
3.300000: 33.000000
4.400000: 44.000000
5.500000: 55.000000
```

---

### Example 9: Use String Objects in a Map

```c
#include <stdlib.h>
#include <string.h>
#include "map/map.h"
#include "string/std_string.h"
#include "fmt/fmt.h"


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
    } 
    else {
        fmt_printf("Key 'key2' not found for removal.\n");
    }

    String* searchKey = string_create("key1");
    String** foundValue = (String**) map_at(myMap, &searchKey);

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
**Result**
```
Map contents:
key1: Hello
key2: World
Removed 'key2' from the map.
Found value for 'key1': Hello
```

---

### Example 10: Merging Two Maps

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"


int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}


void int_deallocator(void* data) {
    free(data);
}

void map_merge(Map* dest, Map* src) {
    for (MapIterator it = map_begin(src); it.node != map_end(src).node; map_iterator_increment(&it)) {
        int* key = (int*) malloc(sizeof(int));
        int* value = (int*) malloc(sizeof(int));

        *key = *(int*)map_node_get_key(it.node);
        *value = *(int*)map_node_get_value(it.node);
        map_insert(dest, key, value);
    }
}


int main() {
    Map* map1 = map_create(compare_ints, int_deallocator, int_deallocator);
    Map* map2 = map_create(compare_ints, int_deallocator, int_deallocator);

    for (int i = 0; i < 3; ++i) {
        int* key = (int*) malloc(sizeof(int));
        int* value = (int*) malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(map1, key, value);
    }

    for (int i = 2; i < 5; ++i) {
        int* key = (int*)  malloc(sizeof(int));
        int* value = (int*) malloc(sizeof(int));
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
**Result**
```
0: 0
1: 100
2: 400
3: 600
4: 800
```

---

### Example 11: Filtering a Map Based on a Condition

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"


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
        int* key = (int*) malloc(sizeof(int));
        int* value = (int*) malloc(sizeof(int));
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
**Result**
```
0: 0
2: 200
4: 400
```

---

### Example 12: `map_print`

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/std_string.h"


int compare_ints(const KeyType a, const KeyType b) {
    return *(int*)a - *(int*)b;
}


void print_int(const KeyType key) {
    fmt_printf("%d", *(const int*)key);
}


void print_string(const ValueType value) {
    fmt_printf("%s", (const char*)value);
}


int main() {
    Map* myMap = map_create(compare_ints, NULL, free);
    char* value = string_strdup("Hello, World");
    int key = 5;

    map_insert(myMap, &key, value);
    map_print(myMap, print_int, print_string);

    map_deallocate(myMap);   
    return 0;
}
```
**Result**
```
5: Hello, World
```

---

### Example 13: `map_copy`

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"


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
    Map* originalMap = map_create(compare_ints, NULL, NULL);
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
**Result**
```
10: 20
Size of the copied map: 1
```
---

## License

This project is open-source and available under [ISC License].