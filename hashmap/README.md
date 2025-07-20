# HashMap Library in C

## Overview

The HashMap library provides a generic implementation of an unordered associative container (similar to `std::unordered_map` in C++). This HashMap is implemented as a hash table with separate chaining for collision resolution, providing average O(1) time complexity for insertion, deletion, and lookup operations.

## Features

- **Generic Key and Value Types:** The hashmap can store any type of key-value pairs.
- **Hash Table Implementation:** Provides average O(1) time complexity for basic operations.
- **Separate Chaining:** Uses linked lists to handle hash collisions.
- **Dynamic Resizing:** Automatically rehashes when load factor exceeds threshold.
- **Iterators:** Provides iterators to traverse the hashmap.
- **Flexible Memory Management:** Customizable deallocation functions for keys and values.
- **Comprehensive Operations:** Includes insertion, deletion, lookup, size, swap, and more.

## Usage

To use the HashMap library, include the `hashmap.h` header file in your C program. The implementation resides in `hashmap.c`, which should be compiled along with your program.

## Data Structures

### `HashMapNode`
The `HashMapNode` structure represents an individual node in the hash table chain. Each node contains a key-value pair and a pointer to the next node in the chain.

- **Fields:**
  - `KeyType key`: A pointer to the key stored in the node.
  - `ValueType value`: A pointer to the value associated with the key.
  - `struct HashMapNode* next`: A pointer to the next node in the chain.
  - `bool is_occupied`: Indicates whether the node is occupied (true) or empty (false).

### `HashMapEntry`
The `HashMapEntry` structure encapsulates a key-value pair. It represents an element of the hashmap, storing the key and its corresponding value.

- **Fields:**
  - `KeyType key`: The key associated with the value.
  - `ValueType value`: The value associated with the key.

### `HashMapIterator`
The `HashMapIterator` structure provides a way to traverse the `HashMap`. The iterator maintains references to the hashmap and current position.

- **Fields:**
  - `HashMap* map`: A pointer to the hashmap being iterated.
  - `size_t bucket_index`: The current bucket index.
  - `HashMapNode* current_node`: A pointer to the current node in the iteration.

### `HashMap`
The `HashMap` structure is the main container that manages the hash table.

- **Fields:**
  - `HashMapNode* buckets`: Array of bucket heads.
  - `size_t bucket_count`: Number of buckets in the hash table.
  - `size_t size`: Number of elements in the hashmap.
  - `size_t max_load_factor_numerator`: Numerator for max load factor calculation.
  - `size_t max_load_factor_denominator`: Denominator for max load factor calculation.
  - `HashFunc hash_func`: Function pointer to the hash function.
  - `CompareFuncHashMap compare_func`: Function pointer to the comparison function.
  - `ValueDeallocFunc dealloc_key`: Function pointer to key deallocation function.
  - `ValueDeallocFunc dealloc_value`: Function pointer to value deallocation function.

## Function Explanations

### Constructor and Destructor Functions

#### `HashMap* hashmap_create(HashFunc hash_func, CompareFuncHashMap compare_func, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value)`
- **Purpose**: Creates a new HashMap with default bucket count.
- **Parameters:**
  - `hash_func`: Function pointer for hashing keys.
  - `compare_func`: Function pointer for comparing keys.
  - `dealloc_key`: Function pointer for deallocating keys (can be NULL).
  - `dealloc_value`: Function pointer for deallocating values (can be NULL).
- **Returns**: A pointer to the newly created HashMap, or NULL if allocation fails.

#### `HashMap* hashmap_create_with_buckets(HashFunc hash_func, CompareFuncHashMap compare_func, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value, size_t initial_bucket_count)`
- **Purpose**: Creates a new HashMap with a specified initial bucket count.
- **Parameters:**
  - `hash_func`: Function pointer for hashing keys.
  - `compare_func`: Function pointer for comparing keys.
  - `dealloc_key`: Function pointer for deallocating keys (can be NULL).
  - `dealloc_value`: Function pointer for deallocating values (can be NULL).
  - `initial_bucket_count`: Initial number of buckets.
- **Returns**: A pointer to the newly created HashMap, or NULL if allocation fails.

#### `void hashmap_deallocate(HashMap* map)`
- **Purpose**: Deallocates all memory associated with the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap to be deallocated.
- **Returns**: None.

### Capacity Functions

#### `bool hashmap_empty(const HashMap* map)`
- **Purpose**: Checks if the HashMap is empty.
- **Parameters:**
  - `map`: A pointer to the HashMap to check.
- **Returns**: true if the HashMap is empty, false otherwise.

#### `size_t hashmap_size(const HashMap* map)`
- **Purpose**: Returns the number of elements in the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: The number of elements in the HashMap.

#### `size_t hashmap_bucket_count(const HashMap* map)`
- **Purpose**: Returns the number of buckets in the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: The number of buckets in the HashMap.

#### `size_t hashmap_max_bucket_count(const HashMap* map)`
- **Purpose**: Returns the maximum number of buckets the HashMap can have.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: The maximum number of buckets.

#### `size_t hashmap_bucket_size(const HashMap* map, size_t bucket_index)`
- **Purpose**: Returns the number of elements in a specific bucket.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `bucket_index`: The index of the bucket to check.
- **Returns**: The number of elements in the specified bucket.

### Element Access Functions

#### `ValueType hashmap_at(const HashMap* map, KeyType key)`
- **Purpose**: Returns the value associated with the specified key.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to look up.
- **Returns**: The value associated with the key, or NULL if the key is not found.

#### `ValueType hashmap_operator_bracket(HashMap* map, KeyType key)`
- **Purpose**: Returns the value associated with the specified key, creating a new entry if the key doesn't exist.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to look up.
- **Returns**: The value associated with the key, or NULL if the key is not found and insertion failed.

### Iterator Functions

#### `HashMapIterator hashmap_begin(const HashMap* map)`
- **Purpose**: Returns an iterator pointing to the first element in the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: An iterator pointing to the first element.

#### `HashMapIterator hashmap_end(const HashMap* map)`
- **Purpose**: Returns an iterator pointing to the position after the last element.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: An iterator pointing to the end position.

#### `HashMapIterator hashmap_cbegin(const HashMap* map)`
- **Purpose**: Returns a constant iterator pointing to the first element in the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: A constant iterator pointing to the first element.

#### `HashMapIterator hashmap_cend(const HashMap* map)`
- **Purpose**: Returns a constant iterator pointing to the position after the last element.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: A constant iterator pointing to the end position.

### Iterator Operations

#### `void hashmap_iterator_increment(HashMapIterator* it)`
- **Purpose**: Advances the iterator to the next element.
- **Parameters:**
  - `it`: A pointer to the iterator to increment.
- **Returns**: None.

#### `void hashmap_iterator_decrement(HashMapIterator* it)`
- **Purpose**: Moves the iterator to the previous element (simplified implementation).
- **Parameters:**
  - `it`: A pointer to the iterator to decrement.
- **Returns**: None.

#### `bool hashmap_iterator_equal(const HashMapIterator* it1, const HashMapIterator* it2)`
- **Purpose**: Compares two iterators for equality.
- **Parameters:**
  - `it1`: A pointer to the first iterator.
  - `it2`: A pointer to the second iterator.
- **Returns**: true if the iterators are equal, false otherwise.

#### `HashMapEntry hashmap_iterator_dereference(const HashMapIterator* it)`
- **Purpose**: Returns the key-value pair at the iterator's position.
- **Parameters:**
  - `it`: A pointer to the iterator.
- **Returns**: A HashMapEntry containing the key-value pair.

### Element Lookup Functions

#### `HashMapIterator hashmap_find(const HashMap* map, KeyType key)`
- **Purpose**: Finds an element with the specified key.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to search for.
- **Returns**: An iterator pointing to the found element, or end iterator if not found.

#### `size_t hashmap_count(const HashMap* map, KeyType key)`
- **Purpose**: Returns the number of elements with the specified key (0 or 1 for HashMap).
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to count.
- **Returns**: The number of elements with the specified key.

#### `bool hashmap_contains(const HashMap* map, KeyType key)`
- **Purpose**: Checks if the HashMap contains an element with the specified key.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to check for.
- **Returns**: true if the key is found, false otherwise.

### Modifier Functions

#### `bool hashmap_insert(HashMap* map, KeyType key, ValueType value)`
- **Purpose**: Inserts a key-value pair into the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to insert.
  - `value`: The value to associate with the key.
- **Returns**: true if insertion was successful, false otherwise.

#### `bool hashmap_emplace(HashMap* map, KeyType key, ValueType value)`
- **Purpose**: Inserts a key-value pair into the HashMap (same as insert).
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to insert.
  - `value`: The value to associate with the key.
- **Returns**: true if insertion was successful, false otherwise.

#### `bool hashmap_erase(HashMap* map, KeyType key)`
- **Purpose**: Removes the element with the specified key.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key of the element to remove.
- **Returns**: true if the element was removed, false if the key was not found.

#### `void hashmap_clear(HashMap* map)`
- **Purpose**: Removes all elements from the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: None.

### Bucket Interface Functions

#### `size_t hashmap_bucket(const HashMap* map, KeyType key)`
- **Purpose**: Returns the bucket index for the specified key.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `key`: The key to get the bucket for.
- **Returns**: The bucket index for the key.

#### `HashMapIterator hashmap_begin_bucket(const HashMap* map, size_t bucket_index)`
- **Purpose**: Returns an iterator pointing to the first element in a specific bucket.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `bucket_index`: The index of the bucket.
- **Returns**: An iterator pointing to the first element in the bucket.

#### `HashMapIterator hashmap_end_bucket(const HashMap* map, size_t bucket_index)`
- **Purpose**: Returns an iterator pointing to the position after the last element in a specific bucket.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `bucket_index`: The index of the bucket.
- **Returns**: An iterator pointing to the end of the bucket.

### Hash Policy Functions

#### `float hashmap_load_factor(const HashMap* map)`
- **Purpose**: Returns the current load factor of the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: The current load factor (size / bucket_count).

#### `float hashmap_max_load_factor(const HashMap* map)`
- **Purpose**: Returns the maximum load factor of the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: The maximum load factor.

#### `void hashmap_max_load_factor_set(HashMap* map, float max_load_factor)`
- **Purpose**: Sets the maximum load factor for the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `max_load_factor`: The new maximum load factor.
- **Returns**: None.

#### `void hashmap_rehash(HashMap* map, size_t bucket_count)`
- **Purpose**: Rehashes the HashMap with a new bucket count.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `bucket_count`: The new number of buckets.
- **Returns**: None.

#### `void hashmap_reserve(HashMap* map, size_t count)`
- **Purpose**: Reserves space for at least the specified number of elements.
- **Parameters:**
  - `map`: A pointer to the HashMap.
  - `count`: The number of elements to reserve space for.
- **Returns**: None.

### Observer Functions

#### `HashFunc hashmap_hash_function(const HashMap* map)`
- **Purpose**: Returns the hash function used by the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: A pointer to the hash function.

#### `CompareFuncHashMap hashmap_key_eq(const HashMap* map)`
- **Purpose**: Returns the comparison function used by the HashMap.
- **Parameters:**
  - `map`: A pointer to the HashMap.
- **Returns**: A pointer to the comparison function.

### Non-member Functions

#### `bool hashmap_equal(const HashMap* map1, const HashMap* map2)`
- **Purpose**: Compares two HashMaps for equality.
- **Parameters:**
  - `map1`: A pointer to the first HashMap.
  - `map2`: A pointer to the second HashMap.
- **Returns**: true if the HashMaps are equal, false otherwise.

#### `bool hashmap_not_equal(const HashMap* map1, const HashMap* map2)`
- **Purpose**: Compares two HashMaps for inequality.
- **Parameters:**
  - `map1`: A pointer to the first HashMap.
  - `map2`: A pointer to the second HashMap.
- **Returns**: true if the HashMaps are not equal, false otherwise.

#### `void hashmap_swap(HashMap* map1, HashMap* map2)`
- **Purpose**: Swaps the contents of two HashMaps.
- **Parameters:**
  - `map1`: A pointer to the first HashMap.
  - `map2`: A pointer to the second HashMap.
- **Returns**: None.

### Utility Functions

#### `void hashmap_print(const HashMap* map, void (*print_key)(const KeyType), void (*print_value)(const ValueType))`
- **Purpose**: Prints the contents of the HashMap using custom print functions.
- **Parameters:**
  - `map`: A pointer to the HashMap to print.
  - `print_key`: Function pointer for printing keys.
  - `print_value`: Function pointer for printing values.
- **Returns**: None.

#### `HashMap* hashmap_copy(const HashMap* src)`
- **Purpose**: Creates a deep copy of the HashMap.
- **Parameters:**
  - `src`: A pointer to the source HashMap.
- **Returns**: A pointer to the copied HashMap, or NULL if allocation fails.

### Default Hash Functions

#### `size_t hashmap_hash_int(const KeyType key)`
- **Purpose**: Default hash function for integer keys.
- **Parameters:**
  - `key`: A pointer to an integer key.
- **Returns**: The hash value for the integer key.

#### `size_t hashmap_hash_string(const KeyType key)`
- **Purpose**: Default hash function for string keys (djb2 algorithm).
- **Parameters:**
  - `key`: A pointer to a string key.
- **Returns**: The hash value for the string key.

#### `size_t hashmap_hash_pointer(const KeyType key)`
- **Purpose**: Default hash function for pointer keys.
- **Parameters:**
  - `key`: A pointer key.
- **Returns**: The hash value for the pointer key.

### Default Comparison Functions

#### `int hashmap_compare_int(const KeyType key1, const KeyType key2)`
- **Purpose**: Default comparison function for integer keys.
- **Parameters:**
  - `key1`: A pointer to the first integer key.
  - `key2`: A pointer to the second integer key.
- **Returns**: Negative value if key1 < key2, positive if key1 > key2, 0 if equal.

#### `int hashmap_compare_string(const KeyType key1, const KeyType key2)`
- **Purpose**: Default comparison function for string keys.
- **Parameters:**
  - `key1`: A pointer to the first string key.
  - `key2`: A pointer to the second string key.
- **Returns**: Negative value if key1 < key2, positive if key1 > key2, 0 if equal.

#### `int hashmap_compare_pointer(const KeyType key1, const KeyType key2)`
- **Purpose**: Default comparison function for pointer keys.
- **Parameters:**
  - `key1`: The first pointer key.
  - `key2`: The second pointer key.
- **Returns**: Negative value if key1 < key2, positive if key1 > key2, 0 if equal.

## Example Usage

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap/hashmap.h"
#include "fmt/fmt.h"


void print_separator(const char* title) {
    int len = strlen(title) + 4;
    fmt_printf("\n");
    for (int i = 0; i < len; i++) fmt_printf("=");
    fmt_printf("\n");
    fmt_printf("= %s =\n", title);
    for (int i = 0; i < len; i++) fmt_printf("=");
    fmt_printf("\n");
}

// Custom print functions
void print_string_key(const KeyType key) {
    fmt_printf("\"%s\"", (const char*)key);
}

void print_int_value(const ValueType value) {
    fmt_printf("%d", *(int*)value);
}

void print_float_value(const ValueType value) {
    fmt_printf("%.2f", *(float*)value);
}

void demo_basic_hashmap_operations() {
    print_separator("Basic HashMap Operations Demo");
    
    // Create a hashmap with string keys and integer values
    HashMap* map = hashmap_create(hashmap_hash_string, hashmap_compare_string, 
                                 free, free);
    
    if (!map) {
        fmt_printf("Failed to create hashmap\n");
        return;
    }
    
    fmt_printf("Created hashmap with %zu buckets\n", hashmap_bucket_count(map));
    fmt_printf("Initial size: %zu\n", hashmap_size(map));
    fmt_printf("Is empty: %s\n", hashmap_empty(map) ? "true" : "false");
    fmt_printf("\n");
    
    // Insert some key-value pairs
    char* key1 = strdup("apple");
    int* value1 = malloc(sizeof(int));
    *value1 = 10;
    hashmap_insert(map, key1, value1);
    
    char* key2 = strdup("banana");
    int* value2 = malloc(sizeof(int));
    *value2 = 20;
    hashmap_insert(map, key2, value2);
    
    char* key3 = strdup("cherry");
    int* value3 = malloc(sizeof(int));
    *value3 = 30;
    hashmap_insert(map, key3, value3);
    
    char* key4 = strdup("date");
    int* value4 = malloc(sizeof(int));
    *value4 = 40;
    hashmap_insert(map, key4, value4);
    
    fmt_printf("After inserting 4 elements:\n");
    fmt_printf("Size: %zu\n", hashmap_size(map));
    fmt_printf("Load factor: %.3f\n", hashmap_load_factor(map));
    fmt_printf("Is empty: %s\n", hashmap_empty(map) ? "true" : "false");
    fmt_printf("\n");
    
    // Print the hashmap
    fmt_printf("HashMap contents:\n");
    hashmap_print(map, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Look up values
    fmt_printf("Looking up values:\n");
    int* found_value = (int*)hashmap_at(map, "banana");
    if (found_value) {
        fmt_printf("Value for 'banana': %d\n", *found_value);
    } else {
        fmt_printf("'banana' not found\n");
    }
    
    found_value = (int*)hashmap_at(map, "grape");
    if (found_value) {
        fmt_printf("Value for 'grape': %d\n", *found_value);
    } else {
        fmt_printf("'grape' not found\n");
    }
    
    // Check if keys exist
    fmt_printf("\nChecking key existence:\n");
    fmt_printf("Contains 'apple': %s\n", hashmap_contains(map, "apple") ? "true" : "false");
    fmt_printf("Contains 'grape': %s\n", hashmap_contains(map, "grape") ? "true" : "false");
    fmt_printf("Count of 'cherry': %zu\n", hashmap_count(map, "cherry"));
    fmt_printf("\n");
    
    // Clean up
    hashmap_deallocate(map);
}

void demo_hashmap_iteration() {
    print_separator("HashMap Iteration Demo");
    
    // Create a hashmap with string keys and float values
    HashMap* map = hashmap_create(hashmap_hash_string, hashmap_compare_string, 
                                 free, free);
    
    if (!map) {
        fmt_printf("Failed to create hashmap\n");
        return;
    }
    
    // Insert some data
    char* fruits[] = {"apple", "banana", "cherry", "date", "elderberry"};
    float prices[] = {1.99f, 0.99f, 3.49f, 2.99f, 4.99f};
    
    for (int i = 0; i < 5; i++) {
        char* key = strdup(fruits[i]);
        float* value = malloc(sizeof(float));
        *value = prices[i];
        hashmap_insert(map, key, value);
    }
    
    fmt_printf("HashMap with %zu elements:\n", hashmap_size(map));
    
    // Iterate through the hashmap
    fmt_printf("Iterating through hashmap:\n");
    HashMapIterator it = hashmap_begin(map);
    HashMapIterator end = hashmap_end(map);
    
    int count = 0;
    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry entry = hashmap_iterator_dereference(&it);
        fmt_printf("  %d: ", ++count);
        print_string_key(entry.key);
        fmt_printf(" -> $");
        print_float_value(entry.value);
        fmt_printf("\n");
        hashmap_iterator_increment(&it);
    }
    fmt_printf("\n");
    
    // Demonstrate bucket interface
    fmt_printf("Bucket information:\n");
    fmt_printf("Total buckets: %zu\n", hashmap_bucket_count(map));
    
    for (size_t i = 0; i < hashmap_bucket_count(map); i++) {
        size_t bucket_size = hashmap_bucket_size(map, i);
        if (bucket_size > 0) {
            fmt_printf("Bucket %zu: %zu elements\n", i, bucket_size);
        }
    }
    fmt_printf("\n");
    
    // Clean up
    hashmap_deallocate(map);
}

void demo_hashmap_modifications() {
    print_separator("HashMap Modifications Demo");
    
    // Create a hashmap
    HashMap* map = hashmap_create(hashmap_hash_string, hashmap_compare_string, 
                                 free, free);
    
    if (!map) {
        fmt_printf("Failed to create hashmap\n");
        return;
    }
    
    // Insert initial data
    char* keys[] = {"red", "green", "blue", "yellow", "purple"};
    int values[] = {100, 200, 300, 400, 500};
    
    for (int i = 0; i < 5; i++) {
        char* key = strdup(keys[i]);
        int* value = malloc(sizeof(int));
        *value = values[i];
        hashmap_insert(map, key, value);
    }
    
    fmt_printf("Initial hashmap:\n");
    hashmap_print(map, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Update an existing value
    fmt_printf("Updating 'green' from 200 to 250:\n");
    char* update_key = strdup("green");
    int* update_value = malloc(sizeof(int));
    *update_value = 250;
    hashmap_insert(map, update_key, update_value);
    
    hashmap_print(map, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Remove an element
    fmt_printf("Removing 'blue':\n");
    if (hashmap_erase(map, "blue")) {
        fmt_printf("Successfully removed 'blue'\n");
    } else {
        fmt_printf("Failed to remove 'blue'\n");
    }
    
    hashmap_print(map, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Try to remove a non-existent element
    fmt_printf("Trying to remove 'orange':\n");
    if (hashmap_erase(map, "orange")) {
        fmt_printf("Successfully removed 'orange'\n");
    } else {
        fmt_printf("Failed to remove 'orange' (not found)\n");
    }
    
    fmt_printf("Final hashmap size: %zu\n", hashmap_size(map));
    fmt_printf("\n");
    
    // Clean up
    hashmap_deallocate(map);
}

void demo_hashmap_rehashing() {
    print_separator("HashMap Rehashing Demo");
    
    // Create a hashmap with small initial bucket count
    HashMap* map = hashmap_create_with_buckets(hashmap_hash_string, hashmap_compare_string, 
                                              free, free, 4);
    
    if (!map) {
        fmt_printf("Failed to create hashmap\n");
        return;
    }
    
    fmt_printf("Initial hashmap:\n");
    fmt_printf("Bucket count: %zu\n", hashmap_bucket_count(map));
    fmt_printf("Max load factor: %.3f\n", hashmap_max_load_factor(map));
    fmt_printf("\n");
    
    // Insert elements to trigger rehashing
    char* colors[] = {"red", "green", "blue", "yellow", "purple", "orange", 
                      "pink", "brown", "black", "white", "gray", "cyan"};
    
    for (int i = 0; i < 12; i++) {
        char* key = strdup(colors[i]);
        int* value = malloc(sizeof(int));
        *value = (i + 1) * 100;
        
        fmt_printf("Inserting '%s' -> %d\n", colors[i], *value);
        fmt_printf("  Current size: %zu, Load factor: %.3f\n", 
               hashmap_size(map), hashmap_load_factor(map));
        
        hashmap_insert(map, key, value);
        
        fmt_printf("  After insertion - Size: %zu, Buckets: %zu, Load factor: %.3f\n", 
               hashmap_size(map), hashmap_bucket_count(map), hashmap_load_factor(map));
        fmt_printf("\n");
    }
    
    fmt_printf("Final hashmap:\n");
    fmt_printf("Size: %zu\n", hashmap_size(map));
    fmt_printf("Bucket count: %zu\n", hashmap_bucket_count(map));
    fmt_printf("Load factor: %.3f\n", hashmap_load_factor(map));
    fmt_printf("\n");
    
    // Clean up
    hashmap_deallocate(map);
}

void demo_hashmap_with_integers() {
    print_separator("HashMap with Integer Keys Demo");
    
    // Create a hashmap with integer keys and string values
    HashMap* map = hashmap_create(hashmap_hash_int, hashmap_compare_int, 
                                 free, free);
    
    if (!map) {
        fmt_printf("Failed to create hashmap\n");
        return;
    }
    
    // Insert some data
    int keys[] = {1, 2, 3, 4, 5, 10, 20, 30, 40, 50};
    char* values[] = {"one", "two", "three", "four", "five",
                      "ten", "twenty", "thirty", "forty", "fifty"};
    
    for (int i = 0; i < 10; i++) {
        int* key = malloc(sizeof(int));
        *key = keys[i];
        char* value = strdup(values[i]);
        hashmap_insert(map, key, value);
    }
    
    fmt_printf("HashMap with integer keys:\n");
    fmt_printf("Size: %zu\n", hashmap_size(map));
    fmt_printf("Bucket count: %zu\n", hashmap_bucket_count(map));
    fmt_printf("\n");
    
    // Look up some values
    fmt_printf("Looking up values:\n");
    int search_keys[] = {1, 5, 15, 30, 100};
    
    for (int i = 0; i < 5; i++) {
        char* found_value = (char*)hashmap_at(map, &search_keys[i]);
        if (found_value) {
            fmt_printf("Key %d -> Value: %s\n", search_keys[i], found_value);
        } else {
            fmt_printf("Key %d -> Not found\n", search_keys[i]);
        }
    }
    fmt_printf("\n");
    
    // Clean up
    hashmap_deallocate(map);
}

void demo_hashmap_copy_and_swap() {
    print_separator("HashMap Copy and Swap Demo");
    
    // Create first hashmap
    HashMap* map1 = hashmap_create(hashmap_hash_string, hashmap_compare_string, 
                                  free, free);
    
    if (!map1) {
        fmt_printf("Failed to create first hashmap\n");
        return;
    }
    
    // Insert data into first map
    char* keys1[] = {"a", "b", "c"};
    int values1[] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        char* key = strdup(keys1[i]);
        int* value = malloc(sizeof(int));
        *value = values1[i];
        hashmap_insert(map1, key, value);
    }
    
    fmt_printf("First hashmap:\n");
    hashmap_print(map1, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Create second hashmap
    HashMap* map2 = hashmap_create(hashmap_hash_string, hashmap_compare_string, 
                                  free, free);
    
    if (!map2) {
        fmt_printf("Failed to create second hashmap\n");
        hashmap_deallocate(map1);
        return;
    }
    
    // Insert different data into second map
    char* keys2[] = {"x", "y", "z"};
    int values2[] = {10, 20, 30};
    
    for (int i = 0; i < 3; i++) {
        char* key = strdup(keys2[i]);
        int* value = malloc(sizeof(int));
        *value = values2[i];
        hashmap_insert(map2, key, value);
    }
    
    fmt_printf("Second hashmap:\n");
    hashmap_print(map2, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Copy first map
    HashMap* map1_copy = hashmap_copy(map1);
    if (map1_copy) {
        fmt_printf("Copy of first hashmap:\n");
        hashmap_print(map1_copy, print_string_key, print_int_value);
        fmt_printf("\n");
    }
    
    // Compare maps
    fmt_printf("map1 == map1_copy: %s\n", hashmap_equal(map1, map1_copy) ? "true" : "false");
    fmt_printf("map1 == map2: %s\n", hashmap_equal(map1, map2) ? "true" : "false");
    fmt_printf("map1 != map2: %s\n", hashmap_not_equal(map1, map2) ? "true" : "false");
    fmt_printf("\n");
    
    // Swap maps
    fmt_printf("Swapping maps...\n");
    hashmap_swap(map1, map2);
    
    fmt_printf("After swap - First hashmap:\n");
    hashmap_print(map1, print_string_key, print_int_value);
    fmt_printf("\n");
    
    fmt_printf("After swap - Second hashmap:\n");
    hashmap_print(map2, print_string_key, print_int_value);
    fmt_printf("\n");
    
    // Clean up
    hashmap_deallocate(map1);
    hashmap_deallocate(map2);
    if (map1_copy) hashmap_deallocate(map1_copy);
}

int main() {
    fmt_printf("HashMap Library Demo\n");
    fmt_printf("===================\n\n");
    
    // Run all demos
    demo_basic_hashmap_operations();
    demo_hashmap_iteration();
    demo_hashmap_modifications();
    demo_hashmap_rehashing();
    demo_hashmap_with_integers();
    demo_hashmap_copy_and_swap();
    
    fmt_printf("Demo completed successfully!\n");
    return 0;
} 
```

## Performance Characteristics

- **Average Time Complexity:**
  - Insertion: O(1)
  - Deletion: O(1)
  - Lookup: O(1)
  - Iteration: O(n)

- **Worst Case Time Complexity:**
  - Insertion: O(n) (when all elements hash to the same bucket)
  - Deletion: O(n) (when all elements hash to the same bucket)
  - Lookup: O(n) (when all elements hash to the same bucket)

- **Space Complexity:** O(n)

## Memory Management

The HashMap library provides flexible memory management through function pointers:

- **Key Deallocation:** If a `dealloc_key` function is provided, it will be called when keys are removed or the hashmap is deallocated.
- **Value Deallocation:** If a `dealloc_value` function is provided, it will be called when values are removed or the hashmap is deallocated.
- **Automatic Cleanup:** The `hashmap_deallocate` function automatically calls the deallocation functions for all remaining keys and values.

## Thread Safety

The HashMap library is not thread-safe. If you need to use it in a multi-threaded environment, you must provide your own synchronization mechanisms.

## Error Handling

The library includes comprehensive error checking and logging:

- **NULL Pointer Checks:** All functions check for NULL pointers and log appropriate error messages.
- **Memory Allocation Errors:** Functions that allocate memory check for allocation failures and return NULL or false.
- **Bounds Checking:** Functions that access buckets check for valid bucket indices.
- **Logging:** The library includes a logging system that can be enabled/disabled via the `HASHMAP_LOGGING_ENABLE` macro.

## Compilation

To compile the HashMap library with your project:

```bash
# Include the header
#include "hashmap/hashmap.h"

# Compile with the source file
gcc -c hashmap.c -o hashmap.o
gcc your_program.c hashmap.o -o your_program
```

Or using CMake:

```cmake
# Add the hashmap directory to your CMakeLists.txt
add_subdirectory(hashmap)

# Link against the hashmap library
target_link_libraries(your_target hashmap)
```

## Dependencies

The HashMap library has minimal dependencies:
- Standard C library (`stdlib.h`, `string.h`)
- Standard I/O library (`stdio.h`) for logging
- Math library (`math.h`) for some utility functions

## License

This library is released under the ISC License, which is a simple, permissive license that allows for commercial use, modification, distribution, and private use. 