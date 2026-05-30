# HashMap Library in C

**Author:** amin tahmasebi
**Release Date:** 2025
**License:** ISC License

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

### `HashMap* hashmap_create(HashFunc hash_func, CompareFuncHashMap compare_func, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value)`
**Purpose**: Allocates a new HashMap with a default bucket count.
**Parameters**:
- `hash_func`: Required hash function pointer for hashing keys.
- `compare_func`: Required comparison function pointer for comparing keys.
- `dealloc_key`: Deallocation function for keys; may be NULL when the map does not own key memory.
- `dealloc_value`: Deallocation function for values; may be NULL when the map does not own value memory.
**Return Value**: Pointer to the newly created HashMap, or NULL on OOM.
**Usage Case**: Use whenever you need a new empty HashMap with automatic memory sizing.

---

### `HashMap* hashmap_create_with_buckets(HashFunc hash_func, CompareFuncHashMap compare_func, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value, size_t initial_bucket_count)`
**Purpose**: Like `hashmap_create` but pre-allocates exactly `initial_bucket_count` buckets.
**Parameters**:
- `hash_func`: Required hash function pointer for hashing keys.
- `compare_func`: Required comparison function pointer for comparing keys.
- `dealloc_key`: Deallocation function for keys; may be NULL.
- `dealloc_value`: Deallocation function for values; may be NULL.
- `initial_bucket_count`: Number of buckets to allocate upfront.
**Return Value**: Pointer to the newly created HashMap, or NULL on OOM.
**Usage Case**: Use when the expected element count is known in advance to avoid early rehashes.

---

### `HashMap* hashmap_copy(const HashMap* src)`
**Purpose**: Creates a shallow, non-owning copy of `src`.
**Parameters**:
- `src`: Pointer to the source HashMap to copy.
**Return Value**: Pointer to the new HashMap sharing key/value pointers with `src`, or NULL on OOM.
**Usage Case**: Use when you need an independent bucket structure over the same data without transferring ownership; the copy is created with `dealloc_key = NULL` and `dealloc_value = NULL` to prevent double-free.

---

### `void hashmap_deallocate(HashMap* map)`
**Purpose**: Frees all bucket nodes (calling registered dealloc functions for keys and values), then frees the HashMap struct itself.
**Parameters**:
- `map`: Pointer to the HashMap to deallocate; safe to call with NULL.
**Return Value**: None.
**Usage Case**: Call when you are completely done with a HashMap to release all associated memory.

---

### `bool hashmap_empty(const HashMap* map)`
**Purpose**: Returns `true` if the map contains no elements or is NULL.
**Parameters**:
- `map`: Pointer to the HashMap to check.
**Return Value**: `true` if the map has zero entries, `false` otherwise.
**Usage Case**: Use as a quick guard before iterating or accessing elements.

---

### `size_t hashmap_size(const HashMap* map)`
**Purpose**: Returns the number of key-value pairs currently stored.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Number of stored entries, or 0 if `map` is NULL.
**Usage Case**: Use to check how many elements are in the map before loops or capacity decisions.

---

### `size_t hashmap_bucket_count(const HashMap* map)`
**Purpose**: Returns the number of buckets in the hash table.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Current bucket count, or 0 if `map` is NULL.
**Usage Case**: Use together with `hashmap_size` to compute the actual load factor or audit resizing behavior.

---

### `size_t hashmap_max_bucket_count(const HashMap* map)`
**Purpose**: Returns the theoretical maximum number of buckets (system-dependent upper limit).
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Maximum bucket count the implementation supports, or 0 if `map` is NULL.
**Usage Case**: Use to validate that a requested reserve or rehash count is within system limits.

---

### `size_t hashmap_bucket_size(const HashMap* map, size_t bucket_index)`
**Purpose**: Returns the number of elements in bucket `bucket_index` (the length of the collision chain).
**Parameters**:
- `map`: Pointer to the HashMap.
- `bucket_index`: Index of the bucket to inspect.
**Return Value**: Number of entries in that bucket, or 0 for an invalid index or NULL map.
**Usage Case**: Use to profile hash quality — high chain lengths signal a poor hash function or too few buckets.

---

### `ValueType hashmap_at(const HashMap* map, KeyType key)`
**Purpose**: Returns the value for `key`, or NULL if not found.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to look up.
**Return Value**: Associated value pointer, or NULL if the key is absent.
**Usage Case**: Use for pure reads when you do not need insertion-on-miss semantics.

---

### `ValueType hashmap_operator_bracket(HashMap* map, KeyType key)`
**Purpose**: Mirrors C++ `operator[]`: returns the existing value for `key`, or inserts a new entry with value NULL and returns NULL.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to look up or insert.
**Return Value**: Existing value pointer, or NULL if the key was newly inserted.
**Usage Case**: Use when "first-time-seen" insertion semantics are needed, such as initializing counts or accumulators.

---

### `HashMapIterator hashmap_begin(const HashMap* map)`
**Purpose**: Returns an iterator pointing to the first occupied element, or an end iterator if the map is empty.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Iterator positioned at the first element.
**Usage Case**: Use to start a forward traversal of all entries.

---

### `HashMapIterator hashmap_end(const HashMap* map)`
**Purpose**: Returns a sentinel end iterator.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Sentinel end iterator to compare against during forward iteration.
**Usage Case**: Use as the loop termination condition when iterating with `hashmap_iterator_increment`.

---

### `HashMapIterator hashmap_cbegin(const HashMap* map)`
**Purpose**: Same as `hashmap_begin` but signals read-only intent.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Iterator positioned at the first element, signaling const access.
**Usage Case**: Use in contexts where you want to document that the iteration will not modify the map.

---

### `HashMapIterator hashmap_cend(const HashMap* map)`
**Purpose**: Same as `hashmap_end` but signals read-only intent.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Sentinel end iterator signaling const access.
**Usage Case**: Use as the loop termination condition paired with `hashmap_cbegin`.

---

### `void hashmap_iterator_increment(HashMapIterator* it)`
**Purpose**: Advances `it` to the next occupied element, traversing across buckets as needed.
**Parameters**:
- `it`: Pointer to the iterator to advance.
**Return Value**: None.
**Usage Case**: Use inside iteration loops to move from one entry to the next; after the last element `it` becomes the end iterator.

---

### `void hashmap_iterator_decrement(HashMapIterator* it)`
**Purpose**: Moves `it` one step backward.
**Parameters**:
- `it`: Pointer to the iterator to move backward.
**Return Value**: None.
**Usage Case**: Provided for completeness; note that HashMap iteration order is unspecified so backward traversal has limited practical use.

---

### `bool hashmap_iterator_equal(const HashMapIterator* it1, const HashMapIterator* it2)`
**Purpose**: Returns `true` if both iterators point to the same position (same bucket and node).
**Parameters**:
- `it1`: Pointer to the first iterator.
- `it2`: Pointer to the second iterator.
**Return Value**: `true` if both iterators are at the same position, `false` otherwise.
**Usage Case**: Use to detect end-of-iteration by comparing against the end iterator.

---

### `HashMapEntry hashmap_iterator_dereference(const HashMapIterator* it)`
**Purpose**: Returns a `HashMapEntry` struct containing the `key` and `value` at the current iterator position.
**Parameters**:
- `it`: Pointer to the iterator to dereference.
**Return Value**: `HashMapEntry` with `.key` and `.value` fields for the current element.
**Usage Case**: Use inside an iteration loop to read the key and value of the current entry.

---

### `HashMapIterator hashmap_find(const HashMap* map, KeyType key)`
**Purpose**: Searches for `key` and returns an iterator to it.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to search for.
**Return Value**: Iterator to the found element, or the end iterator if not found.
**Usage Case**: Prefer over `hashmap_at` when you also need to test membership or want to use the iterator for further operations.

---

### `size_t hashmap_count(const HashMap* map, KeyType key)`
**Purpose**: Returns 1 if `key` exists, 0 otherwise (HashMap keys are unique).
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to count.
**Return Value**: 1 if the key is present, 0 if absent.
**Usage Case**: Use for a simple existence check when you only need a boolean-like result.

---

### `bool hashmap_contains(const HashMap* map, KeyType key)`
**Purpose**: Returns `true` if `key` is present, `false` otherwise.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to check for.
**Return Value**: `true` if the key exists, `false` otherwise.
**Usage Case**: Use as the most readable existence test; equivalent to `hashmap_count(map, key) == 1`.

---

### `bool hashmap_insert(HashMap* map, KeyType key, ValueType value)`
**Purpose**: Inserts a new key-value pair; if `key` already exists the existing entry is unchanged.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to insert.
- `value`: Value to associate with the key.
**Return Value**: `true` on success, `false` on allocation failure.
**Usage Case**: Use for standard insertion; triggers a rehash automatically if the load factor would be exceeded.

---

### `bool hashmap_emplace(HashMap* map, KeyType key, ValueType value)`
**Purpose**: Identical to `hashmap_insert`; provided for API parity with C++ `emplace`.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to insert.
- `value`: Value to associate with the key.
**Return Value**: `true` on success, `false` on allocation failure.
**Usage Case**: Use interchangeably with `hashmap_insert` when porting C++ code that uses `emplace`.

---

### `bool hashmap_erase(HashMap* map, KeyType key)`
**Purpose**: Removes the entry for `key`, calling the registered dealloc functions.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key of the entry to remove.
**Return Value**: `true` if found and removed, `false` if the key was not present.
**Usage Case**: Use to delete a single entry by key, with automatic memory cleanup via the registered deallocators.

---

### `void hashmap_clear(HashMap* map)`
**Purpose**: Removes all entries (calling dealloc functions) but preserves the bucket array so the map can be reused without reallocation.
**Parameters**:
- `map`: Pointer to the HashMap to clear.
**Return Value**: None.
**Usage Case**: Use when you want to empty the map and refill it, avoiding the cost of reallocation.

---

### `void hashmap_swap(HashMap* map1, HashMap* map2)`
**Purpose**: Swaps all internal state (buckets, size, load-factor settings, function pointers) between two HashMaps in O(1).
**Parameters**:
- `map1`: Pointer to the first HashMap.
- `map2`: Pointer to the second HashMap.
**Return Value**: None.
**Usage Case**: Use to exchange two maps cheaply without copying elements.

---

### `size_t hashmap_bucket(const HashMap* map, KeyType key)`
**Purpose**: Returns the bucket index that `key` would hash into.
**Parameters**:
- `map`: Pointer to the HashMap.
- `key`: Key to locate.
**Return Value**: Zero-based bucket index for the key.
**Usage Case**: Use to predict placement for debugging hash distribution or to iterate a specific bucket.

---

### `HashMapIterator hashmap_begin_bucket(const HashMap* map, size_t bucket_index)`
**Purpose**: Returns an iterator to the first element in `bucket_index`, or end if the bucket is empty.
**Parameters**:
- `map`: Pointer to the HashMap.
- `bucket_index`: Index of the bucket to iterate.
**Return Value**: Iterator positioned at the first element of the bucket.
**Usage Case**: Use together with `hashmap_end_bucket` to iterate only the entries in a single collision chain.

---

### `HashMapIterator hashmap_end_bucket(const HashMap* map, size_t bucket_index)`
**Purpose**: Returns the end sentinel for iteration within `bucket_index`.
**Parameters**:
- `map`: Pointer to the HashMap.
- `bucket_index`: Index of the bucket being iterated.
**Return Value**: Sentinel end iterator for the specified bucket.
**Usage Case**: Use as the loop termination condition when iterating a single bucket with `hashmap_begin_bucket`.

---

### `float hashmap_load_factor(const HashMap* map)`
**Purpose**: Returns the current load factor (`size / bucket_count`).
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Current load factor as a float.
**Usage Case**: Use to monitor how full the table is and decide whether to call `hashmap_rehash` or `hashmap_reserve`.

---

### `float hashmap_max_load_factor(const HashMap* map)`
**Purpose**: Returns the configured maximum load factor.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: Maximum load factor as a float; default is 0.75.
**Usage Case**: Use to inspect the current threshold before deciding whether to lower it for better performance.

---

### `void hashmap_max_load_factor_set(HashMap* map, float max_load_factor)`
**Purpose**: Sets the maximum load factor; the next insert that would exceed this threshold triggers a rehash.
**Parameters**:
- `map`: Pointer to the HashMap.
- `max_load_factor`: New maximum load factor value.
**Return Value**: None.
**Usage Case**: Lower the value (e.g., 0.4) to trade memory for fewer collisions on lookup-heavy workloads.

---

### `void hashmap_rehash(HashMap* map, size_t bucket_count)`
**Purpose**: Reallocates the bucket array to `bucket_count` buckets and redistributes all existing entries.
**Parameters**:
- `map`: Pointer to the HashMap.
- `bucket_count`: Desired new bucket count.
**Return Value**: None.
**Usage Case**: Use to manually resize the table after a large batch delete or when profiling reveals poor distribution.

---

### `void hashmap_reserve(HashMap* map, size_t count)`
**Purpose**: Ensures the map has enough buckets to hold at least `count` elements without triggering a rehash.
**Parameters**:
- `map`: Pointer to the HashMap.
- `count`: Anticipated number of elements.
**Return Value**: None.
**Usage Case**: Call before a bulk insert to avoid repeated rehashing; equivalent to `hashmap_rehash(map, ceil(count / max_load_factor))`.

---

### `HashFunc hashmap_hash_function(const HashMap* map)`
**Purpose**: Returns the hash function pointer configured at creation.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: The `HashFunc` pointer used by this map.
**Usage Case**: Use in tests or debugging to verify that a map was constructed with the expected hasher.

---

### `CompareFuncHashMap hashmap_key_eq(const HashMap* map)`
**Purpose**: Returns the key comparison function pointer.
**Parameters**:
- `map`: Pointer to the HashMap.
**Return Value**: The `CompareFuncHashMap` pointer used by this map.
**Usage Case**: Use in tests or debugging to verify the configured comparator.

---

### `bool hashmap_equal(const HashMap* map1, const HashMap* map2)`
**Purpose**: Returns `true` if both maps contain the same set of key-value pairs (deep element-wise comparison).
**Parameters**:
- `map1`: Pointer to the first HashMap.
- `map2`: Pointer to the second HashMap.
**Return Value**: `true` if the maps are element-wise equal, `false` otherwise.
**Usage Case**: Use in tests to assert that two maps have identical contents.

---

### `bool hashmap_not_equal(const HashMap* map1, const HashMap* map2)`
**Purpose**: Returns `true` if the maps differ in any key-value pair.
**Parameters**:
- `map1`: Pointer to the first HashMap.
- `map2`: Pointer to the second HashMap.
**Return Value**: `true` if the maps differ, `false` if they are equal.
**Usage Case**: Equivalent to `!hashmap_equal(map1, map2)`; use for readable inequality assertions.

---

### `void hashmap_print(const HashMap* map, void (*print_key)(const KeyType), void (*print_value)(const ValueType))`
**Purpose**: Iterates the map and calls `print_key` and `print_value` for each entry.
**Parameters**:
- `map`: Pointer to the HashMap to print.
- `print_key`: Callback function to print a single key.
- `print_value`: Callback function to print a single value.
**Return Value**: None.
**Usage Case**: Use for quick debugging when you need to dump the full contents of a map to stdout.

---

### `size_t hashmap_hash_int(const KeyType key)`
**Purpose**: Built-in hash for `int*` keys; dereferences the pointer and mixes the integer bits.
**Parameters**:
- `key`: Pointer to an `int` key.
**Return Value**: Hash value derived from the integer's bit pattern.
**Usage Case**: Pass as `hash_func` to `hashmap_create` when keys are heap- or stack-allocated integers.

---

### `size_t hashmap_hash_string(const KeyType key)`
**Purpose**: Built-in djb2 hash for `const char*` keys.
**Parameters**:
- `key`: Pointer to a null-terminated string key.
**Return Value**: djb2 hash value of the string.
**Usage Case**: Pass as `hash_func` to `hashmap_create` when keys are C strings.

---

### `size_t hashmap_hash_pointer(const KeyType key)`
**Purpose**: Built-in hash that treats the address itself as the key (object-identity hashing).
**Parameters**:
- `key`: Pointer whose address is used as the key.
**Return Value**: Hash value derived from the raw pointer address.
**Usage Case**: Pass as `hash_func` when you want object-identity semantics (two pointers are equal only if they point to the same address).

---

### `int hashmap_compare_int(const KeyType key1, const KeyType key2)`
**Purpose**: Compares two `int*` keys.
**Parameters**:
- `key1`: Pointer to the first integer key.
- `key2`: Pointer to the second integer key.
**Return Value**: Negative if `*key1 < *key2`, 0 if equal, positive if `*key1 > *key2`.
**Usage Case**: Pass as `compare_func` to `hashmap_create` when keys are integers.

---

### `int hashmap_compare_string(const KeyType key1, const KeyType key2)`
**Purpose**: Compares two `const char*` keys lexicographically (delegates to `strcmp`).
**Parameters**:
- `key1`: Pointer to the first string key.
- `key2`: Pointer to the second string key.
**Return Value**: Negative if `key1 < key2`, 0 if equal, positive if `key1 > key2` lexicographically.
**Usage Case**: Pass as `compare_func` to `hashmap_create` when keys are C strings.

---

### `int hashmap_compare_pointer(const KeyType key1, const KeyType key2)`
**Purpose**: Compares two pointer keys by raw address.
**Parameters**:
- `key1`: First pointer key.
- `key2`: Second pointer key.
**Return Value**: Negative if `key1 < key2`, 0 if equal, positive if `key1 > key2` by address.
**Usage Case**: Pass as `compare_func` paired with `hashmap_hash_pointer` for object-identity maps.

---

### Example 1: `hashmap_find` returns an iterator

`hashmap_find` is the lookup of choice when you want to know *where*
in the map a key lives — not just its value. It returns a
`HashMapIterator` that you compare against `hashmap_end(map)` to tell
hit from miss.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"

int main(void) {
    HashMap* m = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);

    hashmap_insert(m, strdup("alice"), strdup("admin"));
    hashmap_insert(m, strdup("bob"),   strdup("user"));

    HashMapIterator it  = hashmap_find(m, (void*)"alice");
    HashMapIterator end = hashmap_end(m);

    if (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry e = hashmap_iterator_dereference(&it);
        fmt_printf("found: %s -> %s\n", (char*)e.key, (char*)e.value);
    }

    HashMapIterator miss = hashmap_find(m, (void*)"carol");
    fmt_printf("'carol' missing: %s\n", hashmap_iterator_equal(&miss, &end) ? "yes" : "no");

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
found: alice -> admin
'carol' missing: yes
```

---

### Example 2: `operator[]` (lookup-or-insert)

`hashmap_operator_bracket` mirrors C++'s `map[key]` — if the key
exists, return its value; if not, create a new entry with value
`NULL` and return `NULL`. Use it when you want "first time we see
this key" semantics.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"

int main(void) {
    HashMap* m = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    int* v = malloc(sizeof(int)); *v = 42;
    hashmap_insert(m, strdup("answer"), v);

    int* hit = (int*)hashmap_operator_bracket(m, (void*)"answer");
    void* miss = hashmap_operator_bracket(m, strdup("new_slot"));

    fmt_printf("answer = %d (existing)\n", *hit);
    fmt_printf("new_slot inserted with NULL value: %s\n", miss == NULL ? "yes" : "no");
    fmt_printf("size after operator[] auto-insert: %zu\n", hashmap_size(m));

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
answer = 42 (existing)
new_slot inserted with NULL value: yes
size after operator[] auto-insert: 2
```

---

### Example 3: `hashmap_clear` empties and reuses the map

`clear` drops every entry but keeps the bucket array allocated, so the
next round of inserts doesn't pay an allocation tax. Use
`hashmap_deallocate` only when you're done with the map for good.

```c
#include <stdlib.h>
#include <string.h>
#include "fmt/fmt.h"
#include "hashmap/hashmap.h"

int main(void) {
    HashMap* m = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    for (int i = 0; i < 5; ++i) {
        char k[8]; snprintf(k, sizeof(k), "k%d", i);
        int* v = malloc(sizeof(int)); *v = i * 10;
        
        hashmap_insert(m, strdup(k), v);
    }
    fmt_printf("filled: size = %zu, buckets = %zu\n", hashmap_size(m), hashmap_bucket_count(m));

    size_t buckets_before = hashmap_bucket_count(m);
    hashmap_clear(m);

    fmt_printf("after clear: size = %zu, buckets = %zu (preserved: %s)\n", hashmap_size(m), hashmap_bucket_count(m),
           hashmap_bucket_count(m) == buckets_before ? "yes" : "no");

    int* one = malloc(sizeof(int)); 
    *one = 1;
    hashmap_insert(m, strdup("reused"), one);
    fmt_printf("after reuse: size = %zu\n", hashmap_size(m));

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
filled: size = 5, buckets = 17
after clear: size = 0, buckets = 17 (preserved: yes)
after reuse: size = 1
```

---

### Example 4: Tune the max load factor

Lower max-load-factor = more memory, fewer collisions. Default is
0.75. The next `insert` past the threshold triggers a rehash.

```c
#include <stdlib.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"


int main(void) {
    HashMap* m = hashmap_create(hashmap_hash_int, hashmap_compare_int, free, free);

    fmt_printf("default max_load_factor: %.3f\n", hashmap_max_load_factor(m));
    fmt_printf("default bucket_count:    %zu\n",  hashmap_bucket_count(m));

    hashmap_max_load_factor_set(m, 0.4f);
    fmt_printf("after set 0.4: max_load_factor = %.3f\n", hashmap_max_load_factor(m));

    for (int i = 0; i < 12; ++i) {
        int* k = malloc(sizeof(int)); 
        *k = i;

        int* v = malloc(sizeof(int)); 
        *v = i;

        hashmap_insert(m, k, v);
    }

    fmt_printf("after 12 inserts: size = %zu, buckets = %zu, load = %.3f\n", hashmap_size(m), hashmap_bucket_count(m), hashmap_load_factor(m));

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
default max_load_factor: 0.750
default bucket_count:    17
after set 0.4: max_load_factor = 0.400
after 12 inserts: size = 12, buckets = 37, load = 0.324
```

---

### Example 5: `hashmap_reserve` for bulk inserts

When you know roughly how many entries you'll add, `reserve` pre-grows
the bucket array so the inserts don't trigger a rehash mid-loop.

```c
#include <stdlib.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"

int main(void) {
    HashMap* m = hashmap_create(hashmap_hash_int, hashmap_compare_int, free, free);
    fmt_printf("before reserve: buckets = %zu\n", hashmap_bucket_count(m));

    hashmap_reserve(m, 1000);          // plan to insert up to 1000 entries
    fmt_printf("after  reserve: buckets = %zu (no insert yet)\n", hashmap_bucket_count(m));

    size_t buckets_during = hashmap_bucket_count(m);
    for (int i = 0; i < 500; ++i) {
        int* k = malloc(sizeof(int)); 
        *k = i;

        int* v = malloc(sizeof(int)); 
        *v = i * i;

        hashmap_insert(m, k, v);
    }

    fmt_printf("after  bulk insert (500): buckets = %zu (unchanged: %s)\n", hashmap_bucket_count(m),
           hashmap_bucket_count(m) == buckets_during ? "yes" : "no");

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
before reserve: buckets = 17
after  reserve: buckets = 1361 (no insert yet)
after  bulk insert (500): buckets = 1361 (unchanged: yes)
```

---

### Example 6: Word-frequency counter

The classic textbook use case. Read with `hashmap_at`; if the count
already exists bump it in place, otherwise insert a fresh `1`.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"


int main(void) {
    HashMap* freq = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* words[] = {
        "the", "quick", "brown", "fox", "jumps", "over", "the", "lazy",
        "dog", "the", "fox", "is", "quick"
    };
    size_t n = sizeof(words) / sizeof(words[0]);

    for (size_t i = 0; i < n; ++i) {
        int* count = (int*)hashmap_at(freq, (void*)words[i]);

        if (count) {
            (*count)++;  // bump existing entry in place
        } 
        else {
            int* one = malloc(sizeof(int)); *one = 1;
            hashmap_insert(freq, strdup(words[i]), one);
        }
    }

    fmt_printf("counted %zu words -> %zu unique\n", n, hashmap_size(freq));
    HashMapIterator it = hashmap_begin(freq), end = hashmap_end(freq);
    
    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry e = hashmap_iterator_dereference(&it);
        fmt_printf("  %-8s : %d\n", (char*)e.key, *(int*)e.value);
        hashmap_iterator_increment(&it);
    }

    hashmap_deallocate(freq);
    return 0;
}
```

**Result**
```
counted 13 words -> 9 unique
  fox      : 2
  is       : 1
  the      : 3
  jumps    : 1
  brown    : 1
  over     : 1
  lazy     : 1
  dog      : 1
  quick    : 2
```
> Note: HashMap iteration order is unspecified; word lines may appear in any order.

---

### Example 7: Inspect bucket distribution

For tuning your hash quality on a specific dataset. `hashmap_bucket_size`
gives you the chain length for each bucket, and `hashmap_bucket(key)`
tells you which bucket a key would land in.

```c
#include <stdlib.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"


int main(void) {
    HashMap* m = hashmap_create_with_buckets(hashmap_hash_int, hashmap_compare_int, free, free, 7);

    for (int i = 0; i < 25; ++i) {
        int* k = malloc(sizeof(int)); 
        *k = i;

        int* v = malloc(sizeof(int)); 
        *v = i;

        hashmap_insert(m, k, v);
    }

    fmt_printf("size=%zu, buckets=%zu, load=%.3f\n", hashmap_size(m), hashmap_bucket_count(m), hashmap_load_factor(m));
    
    for (size_t b = 0; b < hashmap_bucket_count(m); ++b) {
        size_t sz = hashmap_bucket_size(m, b);
        if (sz) fmt_printf("  bucket[%zu] : %zu entries\n", b, sz);
    }
    
    int key42 = 42;
    fmt_printf("key 42 would hash to bucket %zu\n", hashmap_bucket(m, &key42));

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
size=25, buckets=37, load=0.676
  bucket[0] : 1 entries
  bucket[1] : 1 entries
  bucket[2] : 1 entries
  ...
  bucket[24] : 1 entries
key 42 would hash to bucket 5
```
> Note: Exact bucket indices depend on the hash function and rehash threshold.

---

### Example 8: Observers — verify the configured hash + compare functions

Useful in tests / debugging to confirm a map was constructed with the
expected hash & comparator.

```c
#include "fmt/fmt.h"
#include "hashmap/hashmap.h"

int main(void) {
    HashMap* m = hashmap_create(hashmap_hash_string, hashmap_compare_string, NULL, NULL);
    
    fmt_printf("uses hashmap_hash_string?    %s\n", hashmap_hash_function(m) == hashmap_hash_string ? "yes" : "no");
    fmt_printf("uses hashmap_compare_string? %s\n", hashmap_key_eq(m) == hashmap_compare_string ? "yes" : "no");

    hashmap_deallocate(m);
    return 0;
}
```

**Result**
```
uses hashmap_hash_string?    yes
uses hashmap_compare_string? yes
```

---

### Example 9: Pointer keys (object-identity cache)

When your keys are *objects*, not values, use `hashmap_hash_pointer` +
`hashmap_compare_pointer`. The map hashes by raw address. Pass
`dealloc_key = NULL` because the addresses aren't heap-owned.

```c
#include <stdlib.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"


int main(void) {
    HashMap* seen = hashmap_create(hashmap_hash_pointer, hashmap_compare_pointer, NULL, free);

    int a = 1, b = 2, c = 3;
    int* notes[] = { malloc(sizeof(int)), malloc(sizeof(int)), malloc(sizeof(int)) };
    *notes[0] = 100; *notes[1] = 200; *notes[2] = 300;

    hashmap_insert(seen, &a, notes[0]);
    hashmap_insert(seen, &b, notes[1]);
    hashmap_insert(seen, &c, notes[2]);

    int* n_b = (int*)hashmap_at(seen, &b);
    fmt_printf("note for &b: %d\n", *n_b);
    fmt_printf("seen &a? %s\n", hashmap_contains(seen, &a) ? "yes" : "no");

    int d = 4;
    fmt_printf("seen &d? %s\n", hashmap_contains(seen, &d) ? "yes" : "no");

    hashmap_deallocate(seen);
    return 0;
}
```

**Result**
```
note for &b: 200
seen &a? yes
seen &d? no
```

---

### Example 10: Filter to a new (non-owning) map

Iterate one map and conditionally copy entries to another. The
destination uses NULL deallocators so it doesn't free what the source
still owns.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "fmt/fmt.h"

int main(void) {
    HashMap* src = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* names[]  = {"alice", "bob", "carol", "dave", "eve" };
    int scores[] = {92, 73, 85, 61, 99 };

    for (int i = 0; i < 5; ++i) {
        int* s = malloc(sizeof(int)); *s = scores[i];

        hashmap_insert(src, strdup(names[i]), s);
    }

    // Destination borrows src's key/value pointers — no double-free.
    HashMap* honors = hashmap_create(hashmap_hash_string, hashmap_compare_string, NULL, NULL);
    HashMapIterator it = hashmap_begin(src), end = hashmap_end(src);

    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry e = hashmap_iterator_dereference(&it);
        if (*(int*)e.value >= 90) {
            hashmap_insert(honors, e.key, e.value);
        }
        hashmap_iterator_increment(&it);
    }

    fmt_printf("source: %zu entries, honors (>= 90): %zu entries\n", hashmap_size(src), hashmap_size(honors));
    HashMapIterator hi = hashmap_begin(honors), he = hashmap_end(honors);

    while (!hashmap_iterator_equal(&hi, &he)) {
        HashMapEntry e = hashmap_iterator_dereference(&hi);
        fmt_printf("  %s -> %d\n", (char*)e.key, *(int*)e.value);
        hashmap_iterator_increment(&hi);
    }

    hashmap_deallocate(honors);   // non-owning — frees nodes, not data
    hashmap_deallocate(src);      // owns + frees the data

    return 0;
}
```

**Result**
```
source: 5 entries, honors (>= 90): 2 entries
  alice -> 92
  eve -> 99
```
> Note: HashMap iteration order is unspecified; honor entries may appear in any order.

---

### Example 11: Collect keys into a `Vector` and sort

Because bucket order is unspecified, the safe way to print a map in a
stable order is to harvest the keys into a `Vector`, sort them, then
look each value back up. Here the `Vector` borrows the map's key
pointers, so only the map frees them.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

static int cmp_cstr(const void* a, const void* b) {
    return strcmp(*(const char* const*)a, *(const char* const*)b);
}

int main(void) {
    HashMap* prices = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* fruit[] = { "banana", "apple", "cherry", "date" };
    int cents[] = { 60, 95, 30, 250 };

    for (int i = 0; i < 4; ++i) {
        int* v = malloc(sizeof(int)); *v = cents[i];

        hashmap_insert(prices, strdup(fruit[i]), v);
    }

    /* Collect the keys into a Vector, then sort for deterministic output. */
    Vector* keys = vector_create(sizeof(char*));
    HashMapIterator it = hashmap_begin(prices), end = hashmap_end(prices);
    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry e = hashmap_iterator_dereference(&it);
        char* k = (char*)e.key;

        vector_push_back(keys, &k);
        hashmap_iterator_increment(&it);
    }
    qsort(vector_data(keys), vector_size(keys), sizeof(char*), cmp_cstr);

    fmt_printf("%zu fruits, sorted by name:\n", vector_size(keys));
    for (size_t i = 0; i < vector_size(keys); ++i) {
        char* k = *(char**)vector_at(keys, i);
        fmt_printf("  %-8s %d cents\n", k, *(int*)hashmap_at(prices, k));
    }

    vector_deallocate(keys);     /* borrowed key pointers — frees only the vector */
    hashmap_deallocate(prices);  /* owns and frees the keys/values */

    return 0;
}
```

**Result**
```
4 fruits, sorted by name:
  apple    95 cents
  banana   60 cents
  cherry   30 cents
  date     250 cents
```

---

### Example 12: Tally by first letter with the `String` builder

The key is a *canonical* form derived from each word — its upper-cased
first letter, produced via `string_to_upper`. Counts are bumped in
place, and results are printed by looking up known initials so the
output never depends on bucket order.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main(void) {
    HashMap* tally = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* words[] = {
        "apple", "avocado", "banana", "blueberry", "cherry", "Apricot", "Banana"
    };
    size_t n = sizeof(words) / sizeof(words[0]);

    for (size_t i = 0; i < n; ++i) {
        String* w = string_create(words[i]);
        char* up = string_to_upper(w);           /* "APPLE", "BANANA", ... */
        char initial[2] = { up[0], '\0' };        /* canonical key: first letter */

        int* c = (int*)hashmap_at(tally, initial);
        if (c) {
            (*c)++;
        } 
        else {
            int* one = malloc(sizeof(int)); *one = 1;
            hashmap_insert(tally, strdup(initial), one);
        }

        free(up);
        string_deallocate(w);
    }

    /* Print deterministically by looking up known initials. */
    const char* letters[] = { "A", "B", "C" };
    fmt_printf("words=%zu, distinct initials=%zu\n", n, hashmap_size(tally));
    for (int i = 0; i < 3; ++i) {
        int* c = (int*)hashmap_at(tally, (void*)letters[i]);
        fmt_printf("  %s: %d\n", letters[i], c ? *c : 0);
    }

    hashmap_deallocate(tally);
    return 0;
}
```

**Result**
```
words=7, distinct initials=3
  A: 3
  B: 3
  C: 1
```

---

### Example 13: Serialize a hashmap to JSON

A `<string, int>` map maps naturally onto a JSON object. We build a
`JsonElement` of type `JSON_OBJECT`, one entry per map pair, then
`json_format` it. The formatter sorts keys alphabetically, so the
serialized text is fully deterministic even though bucket order is not.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "json/json.h"
#include "fmt/fmt.h"

int main(void) {
    /* A hashmap of <string, int>. */
    HashMap* inv = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* part[] = { "bolt", "nut", "washer" };
    int qty[] = { 120, 80, 200 };

    for (int i = 0; i < 3; ++i) {
        int* v = malloc(sizeof(int)); *v = qty[i];
        hashmap_insert(inv, strdup(part[i]), v);
    }

    /* Turn the whole map into a JSON object.*/
    JsonElement* obj = json_create(JSON_OBJECT);
    HashMapIterator it = hashmap_begin(inv), end = hashmap_end(inv);

    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry e = hashmap_iterator_dereference(&it);
        JsonElement* num = json_create(JSON_NUMBER);

        num->value.number_val = (double)(*(int*)e.value);
        json_set_element(obj, (const char*)e.key, num);
        hashmap_iterator_increment(&it);
    }

    char* text = json_format(obj);
    fmt_printf("%s\n", text);

    free(text);
    json_deallocate(obj);
    hashmap_deallocate(inv);

    return 0;
}
```

**Result**
```
{
  "bolt": 120,
  "nut": 80,
  "washer": 200
}
```

---

### Example 14: Order-independent stats via a `Vector`

Sums, minimums and maximums don't care about traversal order, so they
are safe to compute directly while iterating. For the median we copy
the values into a `Vector`, sort it, and index the middle — all
deterministic.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    HashMap* scores = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* who[] = { "ada", "linus", "grace", "ken", "dennis" };

    int pts[] = { 88, 73, 95, 60, 81 };
    for (int i = 0; i < 5; ++i) {
        int* v = malloc(sizeof(int)); *v = pts[i];
        hashmap_insert(scores, strdup(who[i]), v);
    }

    /* Collect values into a Vector to compute order-independent stats. */
    Vector* vals = vector_create(sizeof(int));
    long sum = 0;
    HashMapIterator it = hashmap_begin(scores), end = hashmap_end(scores);

    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry e = hashmap_iterator_dereference(&it);
        int v = *(int*)e.value;
        sum += v;
        vector_push_back(vals, &v);
        hashmap_iterator_increment(&it);
    }
    qsort(vector_data(vals), vector_size(vals), sizeof(int), cmp_int);

    int lo = *(int*)vector_front(vals);
    int hi = *(int*)vector_back(vals);
    fmt_printf("n=%zu sum=%ld min=%d max=%d\n", vector_size(vals), sum, lo, hi);
    fmt_printf("median=%d\n", *(int*)vector_at(vals, vector_size(vals) / 2));

    vector_deallocate(vals);
    hashmap_deallocate(scores);

    return 0;
}
```

**Result**
```
n=5 sum=397 min=60 max=95
median=81
```

---

### Example 15: Composite string keys with the `String` builder

When a single field isn't unique, build a composite key. Here
`"city,country"` is assembled with the `String` builder and used for
both insert and lookup, so two `Springfield` cities stay distinct. The
same helper guarantees the lookup key is byte-for-byte identical to the
stored one.

```c
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

/* Build a canonical "city,country" key with the String builder, then use it
   both to insert and to look up — guaranteeing the two keys match exactly. */
static char* make_key(const char* city, const char* country) {
    String* s = string_create(city);
    string_append(s, ",");
    string_append(s, country);
    char* out = strdup(string_c_str(s));

    string_deallocate(s);
    return out;
}

int main(void) {
    HashMap* pop = hashmap_create(hashmap_hash_string, hashmap_compare_string, free, free);
    const char* cities[]    = { "Paris",  "Springfield", "Springfield" };
    const char* countries[] = { "FR",     "US",          "CA"          };
    long people[]           = { 2102000,  170000,        12000         };

    for (int i = 0; i < 3; ++i) {
        long* v = malloc(sizeof(long)); *v = people[i];
        hashmap_insert(pop, make_key(cities[i], countries[i]), v);
    }

    /* Same-name cities stay distinct because the key includes the country. */
    char* k = make_key("Springfield", "US");
    long* v = (long*)hashmap_at(pop, k);

    fmt_printf("distinct entries: %zu\n", hashmap_size(pop));
    fmt_printf("Springfield,US -> %ld\n", v ? *v : -1);
    fmt_printf("has Springfield,CA? %s\n", hashmap_contains(pop, "Springfield,CA") ? "yes" : "no");
    fmt_printf("has Springfield,GB? %s\n", hashmap_contains(pop, "Springfield,GB") ? "yes" : "no");

    free(k);
    hashmap_deallocate(pop);

    return 0;
}
```

**Result**
```
distinct entries: 3
Springfield,US -> 170000
has Springfield,CA? yes
has Springfield,GB? no
```

## License

This project is open-source and available under [ISC License].