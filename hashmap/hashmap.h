/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class HashMap
*/

#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif 

#define HASHMAP_LOGGING_ENABLE

#ifdef HASHMAP_LOGGING_ENABLE 
    #define HASHMAP_LOG(fmt, ...) \
        do { fprintf(stderr, "[HASHMAP LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define HASHMAP_LOG(fmt, ...) do { } while (0)
#endif

// Forward declarations
typedef struct HashMap HashMap;
typedef struct HashMapNode HashMapNode;
typedef struct HashMapIterator HashMapIterator;

// Generic key and value types
typedef void* KeyType;
typedef void* ValueType;

// Function pointer types for hash, comparison, and deallocation
typedef size_t (*HashFunc)(const KeyType);
typedef int (*CompareFuncHashMap)(const KeyType, const KeyType);
typedef void (*ValueDeallocFunc)(void*);

// HashMap node structure
struct HashMapNode {
    KeyType key;
    ValueType value;
    struct HashMapNode* next;  // For chaining
    bool is_occupied;          // To distinguish between empty and deleted slots
};

// HashMap entry structure
typedef struct HashMapEntry {
    KeyType key;
    ValueType value;
} HashMapEntry;

// HashMap iterator structure
typedef struct HashMapIterator {
    HashMap* map;
    size_t bucket_index;
    HashMapNode* current_node;
} HashMapIterator;

// HashMap structure
struct HashMap {
    HashMapNode* buckets;
    size_t bucket_count;
    size_t size;
    size_t max_load_factor_numerator;
    size_t max_load_factor_denominator;
    HashFunc hash_func;
    CompareFuncHashMap compare_func;
    ValueDeallocFunc dealloc_key;
    ValueDeallocFunc dealloc_value;
};

// Constructor and destructor
HashMap* hashmap_create(HashFunc hash_func, CompareFuncHashMap compare_func, 
                       ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value);
HashMap* hashmap_create_with_buckets(HashFunc hash_func, CompareFuncHashMap compare_func,
                                   ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value,
                                   size_t initial_bucket_count);
void hashmap_deallocate(HashMap* map);

// Capacity
bool hashmap_empty(const HashMap* map);
size_t hashmap_size(const HashMap* map);
size_t hashmap_bucket_count(const HashMap* map);
size_t hashmap_max_bucket_count(const HashMap* map);
size_t hashmap_bucket_size(const HashMap* map, size_t bucket_index);

// Element access
ValueType hashmap_at(const HashMap* map, KeyType key);
ValueType hashmap_operator_bracket(HashMap* map, KeyType key);

// Iterators
HashMapIterator hashmap_begin(const HashMap* map);
HashMapIterator hashmap_end(const HashMap* map);
HashMapIterator hashmap_cbegin(const HashMap* map);
HashMapIterator hashmap_cend(const HashMap* map);

// Iterator operations
void hashmap_iterator_increment(HashMapIterator* it);
void hashmap_iterator_decrement(HashMapIterator* it);
bool hashmap_iterator_equal(const HashMapIterator* it1, const HashMapIterator* it2);
HashMapEntry hashmap_iterator_dereference(const HashMapIterator* it);

// Element lookup
HashMapIterator hashmap_find(const HashMap* map, KeyType key);
size_t hashmap_count(const HashMap* map, KeyType key);
bool hashmap_contains(const HashMap* map, KeyType key);

// Modifiers
bool hashmap_insert(HashMap* map, KeyType key, ValueType value);
bool hashmap_emplace(HashMap* map, KeyType key, ValueType value);
bool hashmap_erase(HashMap* map, KeyType key);
void hashmap_clear(HashMap* map);

// Bucket interface
size_t hashmap_bucket(const HashMap* map, KeyType key);
HashMapIterator hashmap_begin_bucket(const HashMap* map, size_t bucket_index);
HashMapIterator hashmap_end_bucket(const HashMap* map, size_t bucket_index);

// Hash policy
float hashmap_load_factor(const HashMap* map);
float hashmap_max_load_factor(const HashMap* map);
void hashmap_max_load_factor_set(HashMap* map, float max_load_factor);
void hashmap_rehash(HashMap* map, size_t bucket_count);
void hashmap_reserve(HashMap* map, size_t count);

// Observers
HashFunc hashmap_hash_function(const HashMap* map);
CompareFuncHashMap hashmap_key_eq(const HashMap* map);

// Non-member functions
bool hashmap_equal(const HashMap* map1, const HashMap* map2);
bool hashmap_not_equal(const HashMap* map1, const HashMap* map2);
void hashmap_swap(HashMap* map1, HashMap* map2);

// Utility functions
void hashmap_print(const HashMap* map, void (*print_key)(const KeyType), void (*print_value)(const ValueType));
HashMap* hashmap_copy(const HashMap* src);

// Default hash functions for common types
size_t hashmap_hash_int(const KeyType key);
size_t hashmap_hash_string(const KeyType key);
size_t hashmap_hash_pointer(const KeyType key);

// Default comparison functions
int hashmap_compare_int(const KeyType key1, const KeyType key2);
int hashmap_compare_string(const KeyType key1, const KeyType key2);
int hashmap_compare_pointer(const KeyType key1, const KeyType key2);

#ifdef __cplusplus 
}
#endif 

#endif 