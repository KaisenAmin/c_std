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

### `MapEntry`
The `MapEntry` structure is a simple structure that encapsulates a key-value pair. It represents an element of the map, storing the key and its corresponding value. This structure is typically used when manipulating the elements in the map.

- **Fields:**
  - `KeyType key`: The key associated with the value.
  - `ValueType value`: The value associated with the key.

### `MapIterator`
The `MapIterator` structure provides a way to traverse the `Map`. The iterator maintains a reference to a node in the Red-Black Tree and is used to iterate over the key-value pairs stored in the map. It can be advanced or reversed to navigate through the elements of the map in order.

- **Fields:**
  - `MapNode* node`: A pointer to the current node in the map that the iterator points to.

### `MapIteratorPair`
The `MapIteratorPair` structure is a utility structure that holds two iterators. It is often used when performing operations like `equal_range()`, which returns a range of elements between two iterators, or when working with lower and upper bounds in a map.

- **Fields:**
  - `MapIterator first`: The first iterator in the pair.
  - `MapIterator second`: The second iterator in the pair.


## Function Explanations


### `CompareFuncMap map_key_comp(const Map* map)`
This function returns the comparison function used by the map to order its elements. The comparison function is specified when the map is created and is essential for ensuring that the keys are ordered correctly within the Red-Black Tree structure.

- **Parameters:**
  - `map`: A pointer to the `Map` whose comparison function is requested.
- **Returns:** A pointer to the comparison function. If the `map` is `NULL`, it returns `NULL` and logs an error.
- **Usage Example:** This function is useful when you want to know or reuse the comparison function that was defined during the map's creation.


### `MapIteratorPair map_equal_range(const Map* map, KeyType key)`
This function returns a pair of iterators representing the range of elements in the map that match the given key. The first iterator points to the first element not less than the key (lower bound), and the second iterator points to the first element greater than the key (upper bound). This is particularly useful when searching for elements that match or are close to a specific key.

- **Parameters:**
  - `map`: A pointer to the `Map` in which to search.
  - `key`: The key to compare the elements against.
- **Returns:** A `MapIteratorPair` containing two iterators: the first pointing to the lower bound and the second pointing to the upper bound of the key. If the `map` or `key` is `NULL`, it returns an empty iterator pair and logs an error.
- **Usage Example:** This function can be used to find a range of elements that match or are close to a given key for range-based processing.



### `KeyType map_node_get_key(MapNode* node)`
This function retrieves the key stored in a given map node. It is a helper function for accessing the key of a node in the Red-Black Tree structure.

- **Parameters:**
  - `node`: A pointer to the `MapNode` from which to retrieve the key.
- **Returns:** The key stored in the node. If the `node` is `NULL`, it returns `NULL` and logs an error.
- **Usage Example:** This function is useful when working with iterators and accessing the key of a node during traversal of the map.


### `Map* map_create(CompareFuncMap comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue)`
This function creates and initializes a new map. It takes a comparison function used to order the keys in the map and optional deallocation functions for keys and values. The map is implemented as a Red-Black Tree, ensuring balanced insertion, deletion, and lookup operations, all with a time complexity of O(log n).

- **Parameters:**
  - `comp`: A function pointer for comparing keys. This function is required and determines the order of keys in the map.
  - `deallocKey`: A function pointer to deallocate keys when a node is deleted. If no deallocation is needed, this can be `NULL`.
  - `deallocValue`: A function pointer to deallocate values when a node is deleted. If no deallocation is needed, this can be `NULL`.
- **Returns:** A pointer to the newly created map. If memory allocation fails or the comparison function is `NULL`, the function logs an error and terminates the program.
- **Usage Example:** This is the primary function for creating a new map and must be called before any other operations on the map.


### `Map* map_copy(const Map* src)`
This function creates a deep copy of the given map. It duplicates all key-value pairs from the source map and inserts them into a newly created map. The copied map uses the same comparison and deallocation functions as the original.

- **Parameters:**
  - `src`: A pointer to the source `Map` to copy.
- **Returns:** A pointer to the newly created copy of the map. If the `src` map is `NULL` or memory allocation fails, the function returns `NULL` and logs an error.
- **Usage Example:** This function is used when you need to create a duplicate of an existing map without modifying the original.


### `void map_deallocate(Map* map)`
This function deallocates all memory associated with the map, including its nodes, keys, and values. It frees the memory used by the map and all of its contents. After calling this function, the `map` pointer should not be used anymore.

- **Parameters:**
  - `map`: A pointer to the `Map` to be deallocated.
- **Usage Example:** This function should be called to free the memory used by the map when it is no longer needed, preventing memory leaks.


### `void map_iterator_increment(MapIterator* it)`
This function advances the iterator to the next element in the map according to the in-order traversal of the Red-Black Tree. If the current node has a right child, the iterator moves to the leftmost node in the right subtree. Otherwise, it moves up the tree to find the first ancestor that is a left child of its parent.

- **Parameters:**
  - `it`: A pointer to the `MapIterator` to be incremented.
- **Usage Example:** Use this function to traverse the map forward.


### `void map_iterator_decrement(MapIterator* it)`
This function moves the iterator to the previous element in the map according to the in-order traversal of the Red-Black Tree. If the current node has a left child, the iterator moves to the rightmost node in the left subtree. Otherwise, it moves up the tree to find the first ancestor that is a right child of its parent.

- **Parameters:**
  - `it`: A pointer to the `MapIterator` to be decremented.
- **Usage Example:** Use this function to traverse the map backward.


### `void map_print(const Map* map, void (*printKey)(const KeyType), void (*printValue)(const ValueType))`
This function prints all key-value pairs stored in the map. It takes two function pointers—one for printing the key and another for printing the value. This allows users to customize how keys and values are printed based on their types.

- **Parameters:**
  - `map`: A pointer to the `Map` to be printed.
  - `printKey`: A function pointer to a custom function for printing the key.
  - `printValue`: A function pointer to a custom function for printing the value.
- **Usage Example:** Use this function to output all the contents of the map in a readable format.


### `void map_swap(Map* map1, Map* map2)`
This function swaps the contents of two maps. It exchanges the root nodes, sizes, comparison functions, and deallocation functions of the two maps. After calling this function, the two maps will have completely swapped their internal state.

- **Parameters:**
  - `map1`: A pointer to the first `Map`.
  - `map2`: A pointer to the second `Map`.
- **Usage Example:** Use this function when you need to exchange the contents of two maps without manually copying elements.


### `void map_clear(Map* map)`
This function removes all elements from the map and frees any associated memory, including the memory for keys and values (if applicable). After calling this function, the map will be empty but still valid for future operations.

- **Parameters:**
  - `map`: A pointer to the `Map` to be cleared.
- **Usage Example:** Use this function to clear the contents of the map without deallocating the map itself, keeping it ready for future use.


### `MapIterator map_begin(const Map* map)`
This function returns an iterator to the first element in the map, which is the leftmost node in the Red-Black Tree. If the map is empty, the returned iterator points to the end of the map.

- **Parameters:**
  - `map`: A pointer to the `Map` whose beginning iterator is requested.
- **Returns:** A `MapIterator` pointing to the first element in the map, or a default iterator if the map is empty or `NULL`.
- **Usage Example:** Use this function to start traversing the map from the first (smallest) element.


### `MapIterator map_end(const Map* map)`
This function returns an iterator representing the position after the last element in the map. It is typically used as a marker to denote the end of the map during iteration.

- **Parameters:**
  - `map`: A pointer to the `Map` whose end iterator is requested.
- **Returns:** A `MapIterator` representing the end of the map. If the map is `NULL`, a default iterator is returned.
- **Usage Example:** Use this function to mark the end of the map in forward iterations.


### `MapIterator map_rbegin(const Map* map)`
This function returns a reverse iterator to the last element in the map, which is the rightmost node in the Red-Black Tree. If the map is empty, it returns an end iterator.

- **Parameters:**
  - `map`: A pointer to the `Map` whose reverse begin iterator is requested.
- **Returns:** A `MapIterator` pointing to the last element in the map, or a default iterator if the map is empty or `NULL`.
- **Usage Example:** Use this function to start traversing the map from the last (largest) element in reverse order.


### `MapIterator map_rend(const Map* map)`
This function returns a reverse iterator representing the position before the first element in reverse order. It effectively points to the position before the first element in reverse traversal.

- **Parameters:**
  - `map`: A pointer to the `Map` whose reverse end iterator is requested.
- **Returns:** A `MapIterator` representing the end of the reverse sequence. If the map is `NULL`, a default iterator is returned.
- **Usage Example:** Use this function to mark the end of reverse iterations.


### `MapIterator map_cbegin(const Map* map)`
This function returns a constant iterator to the first element in the map, allowing read-only access to the elements. It behaves the same as `map_begin()` but ensures the map contents cannot be modified.

- **Parameters:**
  - `map`: A pointer to the `Map` whose constant begin iterator is requested.
- **Returns:** A `MapIterator` pointing to the first element, or a default iterator if the map is empty or `NULL`.
- **Usage Example:** Use this function when you need read-only access to the map elements in forward order.


### `MapIterator map_cend(const Map* map)`
This function returns a constant iterator representing the position after the last element in the map, ensuring read-only access to the elements.

- **Parameters:**
  - `map`: A pointer to the `Map` whose constant end iterator is requested.
- **Returns:** A `MapIterator` representing the end of the map. If the map is `NULL`, a default iterator is returned.
- **Usage Example:** Use this function to mark the end of read-only iterations.


### `MapIterator map_crbegin(const Map* map)`
This function returns a constant reverse iterator to the last element in the map, allowing read-only access to the elements in reverse order.

- **Parameters:**
  - `map`: A pointer to the `Map` whose constant reverse begin iterator is requested.
- **Returns:** A `MapIterator` pointing to the last element, or a default iterator if the map is empty or `NULL`.
- **Usage Example:** Use this function when you need read-only access to the map elements in reverse order.


### `MapIterator map_crend(const Map* map)`
This function returns a constant reverse iterator representing the position before the first element in reverse order, ensuring read-only access.

- **Parameters:**
  - `map`: A pointer to the `Map` whose constant reverse end iterator is requested.
- **Returns:** A `MapIterator` representing the end of the reverse sequence. If the map is `NULL`, a default iterator is returned.
- **Usage Example:** Use this function to mark the end of read-only reverse iterations.


### `MapIterator map_lower_bound(const Map* map, KeyType key)`
This function finds the first element in the map whose key is not less than the specified key. It returns an iterator pointing to that element. If no such element exists, the end iterator is returned.

- **Parameters:**
  - `map`: A pointer to the `Map` to search.
  - `key`: The key to compare against the map elements.
- **Returns:** A `MapIterator` pointing to the first element not less than the given key, or the end iterator if no such element is found.
- **Usage Example:** Use this function for range queries where you need to find elements greater than or equal to a specified key.


### `MapIterator map_upper_bound(const Map* map, KeyType key)`
This function finds the first element in the map whose key is greater than the specified key. It returns an iterator pointing to that element. If no such element exists, the end iterator is returned.

- **Parameters:**
  - `map`: A pointer to the `Map` to search.
  - `key`: The key to compare against the map elements.
- **Returns:** A `MapIterator` pointing to the first element greater than the given key, or the end iterator if no such element is found.
- **Usage Example:** Use this function for range queries where you need to find elements strictly greater than a specified key.


### `MapIterator map_find(const Map* map, KeyType key)`
This function searches for an element in the map by its key. If the key is found, the function returns an iterator pointing to the corresponding element. If the key is not found, the function returns an iterator representing the end of the map.

- **Parameters:**
  - `map`: A pointer to the `Map` to be searched.
  - `key`: The key of the element to find.
- **Returns:** A `MapIterator` pointing to the element if the key is found, or an iterator to the end of the map if the key is not found.
- **Usage Example:** Use this function to search for elements in the map by their keys.


### `size_t map_size(const Map* map)`
This function returns the number of key-value pairs currently stored in the map. If the `map` pointer is `NULL`, the function returns `0`.

- **Parameters:**
  - `map`: A pointer to the `Map` whose size is to be determined.
- **Returns:** The number of elements in the map, or `0` if the `map` pointer is `NULL`.
- **Usage Example:** Use this function to determine how many elements are currently stored in the map.


### `size_t map_max_size(const Map* map)`
This function returns the maximum number of elements that can be held in the map. The maximum size is determined based on the size of the `MapNode` structure. If the `map` pointer is `NULL`, the function returns `0`.

- **Parameters:**
  - `map`: A pointer to the `Map` whose maximum size is to be determined.
- **Returns:** The maximum number of elements that the map can hold, or `0` if the `map` pointer is `NULL`.
- **Usage Example:** This function can be used to check the theoretical limit of how many elements the map can store, based on system constraints.


### `size_t map_count(const Map* map, KeyType key)`
This function counts the number of elements in the map that match a specific key. Since maps do not allow duplicate keys, the function will return `0` if the key is not found or `1` if the key is found.

- **Parameters:**
  - `map`: A pointer to the `Map` to be searched.
  - `key`: The key to search for in the map.
- **Returns:** The number of elements matching the key (`0` or `1`), or `0` if a null pointer is provided.
- **Usage Example:** Use this function to determine if a specific key exists in the map.


### `ValueType map_at(const Map* map, KeyType key)`
This function retrieves the value associated with the specified key in the map. If the key is found, it returns the associated value. If the key is not found or if a null pointer is provided, it returns `NULL`.

- **Parameters:**
  - `map`: A pointer to the `Map` to be searched.
  - `key`: The key to search for in the map.
- **Returns:** The value associated with the key, or `NULL` if the key is not found or the `map` or `key` is `NULL`.
- **Usage Example:** Use this function to access the value of a specific key in the map.


### `ValueType map_node_get_value(MapNode* node)`
This function retrieves the value stored in a specific map node. If the node is `NULL`, the function returns `NULL`.

- **Parameters:**
  - `node`: A pointer to the `MapNode` from which to retrieve the value.
- **Returns:** The value stored in the node, or `NULL` if the node is `NULL`.
- **Usage Example:** This function is useful when working with map nodes and need to retrieve their values.


### `bool map_insert(Map* map, KeyType key, ValueType value)`
This function inserts a key-value pair into the map. If the key already exists, the associated value is replaced with the new value. If the key does not exist, a new node is created and inserted into the map. The function returns `true` on successful insertion or replacement.

- **Parameters:**
  - `map`: A pointer to the `Map` where the key-value pair will be inserted.
  - `key`: The key to be inserted into the map.
  - `value`: The value to be associated with the key.
- **Returns:** `true` if the key-value pair was successfully inserted or replaced, or `false` if the `map` or `key` is `NULL`, or if node creation fails.
- **Usage Example:** Use this function to add or update key-value pairs in the map.


### `bool map_erase(Map* map, KeyType key)`
This function removes the element with the specified key from the map. If the key is found, the node is deleted and the Red-Black Tree is rebalanced to maintain its properties. If the key is not found, the function returns `false`.

- **Parameters:**
  - `map`: A pointer to the `Map` from which the element should be erased.
  - `key`: The key of the element to be removed.
- **Returns:** `true` if the element was successfully erased, or `false` if the key was not found or the map was `NULL`.
- **Usage Example:** Use this function to remove elements from the map by their keys.


### `bool map_emplace(Map* map, KeyType key, ValueType value)`
This function attempts to insert a new key-value pair into the map. If the key already exists, the function returns `false` without modifying the map. If the insertion is successful, the function returns `true`, and the Red-Black Tree is rebalanced if necessary.

- **Parameters:**
  - `map`: A pointer to the `Map` where the key-value pair should be inserted.
  - `key`: The key to be inserted.
  - `value`: The value to be associated with the key.
- **Returns:** `true` if the key-value pair was successfully inserted, or `false` if the key already exists or if the `map` or `key` is `NULL`.
- **Usage Example:** Use this function to insert new key-value pairs only if the key does not already exist.


### `bool map_emplace_hint(Map* map, MapIterator hint, KeyType key, ValueType value)`
This function attempts to insert a new key-value pair into the map using a hint for the insertion position. If the key already exists, the function returns `false` without modifying the map. If the insertion is successful, the function returns `true`, and the Red-Black Tree is rebalanced if necessary.

- **Parameters:**
  - `map`: A pointer to the `Map` where the key-value pair should be inserted.
  - `hint`: A `MapIterator` providing a hint for the insertion position.
  - `key`: The key to be inserted.
  - `value`: The value to be associated with the key.
- **Returns:** `true` if the key-value pair was successfully inserted, or `false` if the key already exists or if the `map` or `key` is `NULL`.
- **Usage Example:** Use this function to insert key-value pairs with an insertion hint to optimize the operation.


### `bool map_empty(const Map* map)`
This function checks whether the map is empty. It returns `true` if the map contains no elements or if the `map` pointer is `NULL`.

- **Parameters:**
  - `map`: A pointer to the `Map` to be checked.
- **Returns:** `true` if the map is empty or `NULL`, or `false` otherwise.
- **Usage Example:** Use this function to determine if the map is empty before performing operations like insertion or deletion.


### Example Programs

The following examples demonstrate how to use the Map library.

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
#include <stdlib.h>
#include "fmt/fmt.h"
#include "map/map.h"
#include "string/string.h"


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
    } 
    else {
        fmt_printf("Key not found.\n");
    }

    map_deallocate(myMap);
    return 0;
}
```

### Example 3: Get the Size of the Map

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

### Example 5

: Erase a Key-Value Pair

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

### Example 6: Increment and Decrement Iterators

```c
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/string.h"


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
#include <stdlib.h>
#include <string.h>
#include "map/map.h"
#include "string/string.h"
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
#include <stdlib.h>
#include "map/map.h"
#include "fmt/fmt.h"
#include "string/string.h"


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
