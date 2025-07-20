/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class HashMap
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashmap.h"

// Default bucket count
#define DEFAULT_BUCKET_COUNT 16
#define DEFAULT_MAX_LOAD_FACTOR 0.75f

// Helper function to find the next prime number
static size_t next_prime(size_t n) {
    if (n <= 2) return 2;
    if (n % 2 == 0) n++;
    
    while (true) {
        bool is_prime = true;
        for (size_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) return n;
        n += 2;
    }
}

// Helper function to create a new node
static HashMapNode* create_node(KeyType key, ValueType value) {
    HashMapNode* node = (HashMapNode*)malloc(sizeof(HashMapNode));
    if (!node) {
        HASHMAP_LOG("[create_node] : Error: Cannot allocate memory for node");
        return NULL;
    }
    
    node->key = key;
    node->value = value;
    node->next = NULL;
    node->is_occupied = true;
    
    HASHMAP_LOG("[create_node] : Created node with key: %p, value: %p", (void*)key, (void*)value);
    return node;
}

// Helper function to free a node
static void free_node(HashMapNode* node, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value) {
    if (!node) return;
    
    if (dealloc_key && node->key) {
        dealloc_key(node->key);
    }
    if (dealloc_value && node->value) {
        dealloc_value(node->value);
    }
    
    free(node);
}

// Helper function to find a node in a bucket
static HashMapNode* find_node_in_bucket(HashMapNode* bucket, KeyType key, CompareFuncHashMap compare_func) {
    HashMapNode* current = bucket;
    while (current) {
        if (current->is_occupied && compare_func(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Helper function to find the next occupied bucket
static size_t find_next_occupied_bucket(const HashMap* map, size_t start_index) {
    for (size_t i = start_index; i < map->bucket_count; i++) {
        if (map->buckets[i].is_occupied) {
            return i;
        }
    }
    return map->bucket_count; // End iterator
}

// Default hash functions
size_t hashmap_hash_int(const KeyType key) {
    if (!key) return 0;
    int value = *(int*)key;
    return (size_t)value;
}

size_t hashmap_hash_string(const KeyType key) {
    if (!key) return 0;
    const char* str = (const char*)key;
    size_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

size_t hashmap_hash_pointer(const KeyType key) {
    if (!key) return 0;
    return (size_t)key;
}

// Default comparison functions
int hashmap_compare_int(const KeyType key1, const KeyType key2) {
    if (!key1 && !key2) return 0;
    if (!key1) return -1;
    if (!key2) return 1;
    
    int val1 = *(int*)key1;
    int val2 = *(int*)key2;
    
    if (val1 < val2) return -1;
    if (val1 > val2) return 1;
    return 0;
}

int hashmap_compare_string(const KeyType key1, const KeyType key2) {
    if (!key1 && !key2) return 0;
    if (!key1) return -1;
    if (!key2) return 1;
    
    return strcmp((const char*)key1, (const char*)key2);
}

int hashmap_compare_pointer(const KeyType key1, const KeyType key2) {
    if (!key1 && !key2) return 0;
    if (!key1) return -1;
    if (!key2) return 1;
    
    if (key1 < key2) return -1;
    if (key1 > key2) return 1;
    return 0;
}

// Constructor functions
HashMap* hashmap_create(HashFunc hash_func, CompareFuncHashMap compare_func, 
                       ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value) {
    return hashmap_create_with_buckets(hash_func, compare_func, dealloc_key, dealloc_value, DEFAULT_BUCKET_COUNT);
}

HashMap* hashmap_create_with_buckets(HashFunc hash_func, CompareFuncHashMap compare_func,
                                   ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value,
                                   size_t initial_bucket_count) {
    if (!hash_func || !compare_func) {
        HASHMAP_LOG("[hashmap_create_with_buckets] : Error: Hash function or compare function is NULL");
        return NULL;
    }
    
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) {
        HASHMAP_LOG("[hashmap_create_with_buckets] : Error: Cannot allocate memory for HashMap");
        return NULL;
    }
    
    // Ensure bucket count is at least 1 and is prime
    size_t bucket_count = next_prime(initial_bucket_count > 0 ? initial_bucket_count : 1);
    
    map->buckets = (HashMapNode*)calloc(bucket_count, sizeof(HashMapNode));
    if (!map->buckets) {
        HASHMAP_LOG("[hashmap_create_with_buckets] : Error: Cannot allocate memory for buckets");
        free(map);
        return NULL;
    }
    
    // Initialize all buckets as empty
    for (size_t i = 0; i < bucket_count; i++) {
        map->buckets[i].is_occupied = false;
        map->buckets[i].next = NULL;
    }
    
    map->bucket_count = bucket_count;
    map->size = 0;
    map->max_load_factor_numerator = 3;
    map->max_load_factor_denominator = 4; // 0.75
    map->hash_func = hash_func;
    map->compare_func = compare_func;
    map->dealloc_key = dealloc_key;
    map->dealloc_value = dealloc_value;
    
    HASHMAP_LOG("[hashmap_create_with_buckets] : Created HashMap with %zu buckets", bucket_count);
    return map;
}

void hashmap_deallocate(HashMap* map) {
    if (!map) return;
    
    hashmap_clear(map);
    free(map->buckets);
    free(map);
    
    HASHMAP_LOG("[hashmap_deallocate] : Deallocated HashMap");
}

// Capacity functions
bool hashmap_empty(const HashMap* map) {
    if (!map) return true;
    return map->size == 0;
}

size_t hashmap_size(const HashMap* map) {
    if (!map) return 0;
    return map->size;
}

size_t hashmap_bucket_count(const HashMap* map) {
    if (!map) return 0;
    return map->bucket_count;
}

size_t hashmap_max_bucket_count(const HashMap* map) {
    if (!map) return 0;
    return SIZE_MAX / sizeof(HashMapNode);
}

size_t hashmap_bucket_size(const HashMap* map, size_t bucket_index) {
    if (!map || bucket_index >= map->bucket_count) return 0;
    
    size_t count = 0;
    HashMapNode* current = &map->buckets[bucket_index];
    
    while (current) {
        if (current->is_occupied) count++;
        current = current->next;
    }
    
    return count;
}

// Element access functions
ValueType hashmap_at(const HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_at] : Error: NULL map or key");
        return NULL;
    }
    
    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* node = find_node_in_bucket(&map->buckets[bucket_index], key, map->compare_func);
    
    if (node) {
        HASHMAP_LOG("[hashmap_at] : Found value for key %p: %p", (void*)key, (void*)node->value);
        return node->value;
    }
    
    HASHMAP_LOG("[hashmap_at] : Key %p not found", (void*)key);
    return NULL;
}

ValueType hashmap_operator_bracket(HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_operator_bracket] : Error: NULL map or key");
        return NULL;
    }
    
    // First try to find existing key
    ValueType value = hashmap_at(map, key);
    if (value) {
        return value;
    }
    
    // If key doesn't exist, create a new entry with NULL value
    if (hashmap_insert(map, key, NULL)) {
        return hashmap_at(map, key);
    }
    
    return NULL;
}

// Iterator functions
HashMapIterator hashmap_begin(const HashMap* map) {
    HashMapIterator it = {NULL, 0, NULL};
    if (!map) return it;
    
    it.map = (HashMap*)map;
    it.bucket_index = find_next_occupied_bucket(map, 0);
    
    if (it.bucket_index < map->bucket_count) {
        it.current_node = &map->buckets[it.bucket_index];
    }
    
    return it;
}

HashMapIterator hashmap_end(const HashMap* map) {
    HashMapIterator it = {NULL, 0, NULL};
    if (!map) return it;
    
    it.map = (HashMap*)map;
    it.bucket_index = map->bucket_count;
    it.current_node = NULL;
    
    return it;
}

HashMapIterator hashmap_cbegin(const HashMap* map) {
    return hashmap_begin(map);
}

HashMapIterator hashmap_cend(const HashMap* map) {
    return hashmap_end(map);
}

void hashmap_iterator_increment(HashMapIterator* it) {
    if (!it || !it->map) return;
    
    if (!it->current_node) {
        HASHMAP_LOG("[hashmap_iterator_increment] : Iterator is at end");
        return;
    }
    
    // Move to next node in current bucket
    if (it->current_node->next) {
        it->current_node = it->current_node->next;
        HASHMAP_LOG("[hashmap_iterator_increment] : Moved to next node in bucket");
        return;
    }
    
    // Move to next occupied bucket
    it->bucket_index = find_next_occupied_bucket(it->map, it->bucket_index + 1);
    
    if (it->bucket_index < it->map->bucket_count) {
        it->current_node = &it->map->buckets[it->bucket_index];
        HASHMAP_LOG("[hashmap_iterator_increment] : Moved to bucket %zu", it->bucket_index);
    } else {
        it->current_node = NULL; // End iterator
        HASHMAP_LOG("[hashmap_iterator_increment] : Reached end");
    }
}

void hashmap_iterator_decrement(HashMapIterator* it) {
    if (!it || !it->map) return;
    
    // This is a simplified implementation
    // For a full implementation, we'd need to maintain a reverse iterator
    HASHMAP_LOG("[hashmap_iterator_decrement] : Decrement not fully implemented");
}

bool hashmap_iterator_equal(const HashMapIterator* it1, const HashMapIterator* it2) {
    if (!it1 || !it2) return false;
    return it1->map == it2->map && 
           it1->bucket_index == it2->bucket_index && 
           it1->current_node == it2->current_node;
}

HashMapEntry hashmap_iterator_dereference(const HashMapIterator* it) {
    HashMapEntry entry = {NULL, NULL};
    if (!it || !it->current_node) return entry;
    
    entry.key = it->current_node->key;
    entry.value = it->current_node->value;
    return entry;
}

// Element lookup functions
HashMapIterator hashmap_find(const HashMap* map, KeyType key) {
    HashMapIterator it = hashmap_end(map);
    if (!map || !key) return it;
    
    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* node = find_node_in_bucket(&map->buckets[bucket_index], key, map->compare_func);
    
    if (node) {
        it.map = (HashMap*)map;
        it.bucket_index = bucket_index;
        it.current_node = node;
        HASHMAP_LOG("[hashmap_find] : Found key %p in bucket %zu", (void*)key, bucket_index);
    }
    
    return it;
}

size_t hashmap_count(const HashMap* map, KeyType key) {
    if (!map || !key) return 0;
    
    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* node = find_node_in_bucket(&map->buckets[bucket_index], key, map->compare_func);
    
    return node ? 1 : 0;
}

bool hashmap_contains(const HashMap* map, KeyType key) {
    return hashmap_count(map, key) > 0;
}

// Modifier functions
bool hashmap_insert(HashMap* map, KeyType key, ValueType value) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_insert] : Error: NULL map or key");
        return false;
    }
    
    // Check if key already exists
    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* existing_node = find_node_in_bucket(&map->buckets[bucket_index], key, map->compare_func);
    
    if (existing_node) {
        // Update existing value
        if (map->dealloc_value && existing_node->value) {
            map->dealloc_value(existing_node->value);
        }
        existing_node->value = value;
        HASHMAP_LOG("[hashmap_insert] : Updated existing key %p", (void*)key);
        return true;
    }
    
    // Check if we need to rehash
    float current_load_factor = (float)map->size / map->bucket_count;
    float max_load_factor = (float)map->max_load_factor_numerator / map->max_load_factor_denominator;
    
    if (current_load_factor >= max_load_factor) {
        size_t new_bucket_count = next_prime(map->bucket_count * 2);
        hashmap_rehash(map, new_bucket_count);
        bucket_index = map->hash_func(key) % map->bucket_count;
    }
    
    // Create new node
    HashMapNode* new_node = create_node(key, value);
    if (!new_node) {
        HASHMAP_LOG("[hashmap_insert] : Error: Failed to create new node");
        return false;
    }
    
    // Insert at beginning of bucket chain
    new_node->next = map->buckets[bucket_index].next;
    map->buckets[bucket_index].next = new_node;
    map->size++;
    
    HASHMAP_LOG("[hashmap_insert] : Inserted key %p in bucket %zu", (void*)key, bucket_index);
    return true;
}

bool hashmap_emplace(HashMap* map, KeyType key, ValueType value) {
    return hashmap_insert(map, key, value);
}

bool hashmap_erase(HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_erase] : Error: NULL map or key");
        return false;
    }
    
    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* current = &map->buckets[bucket_index];
    HashMapNode* prev = NULL;
    
    while (current) {
        if (current->is_occupied && map->compare_func(current->key, key) == 0) {
            // Remove node from chain
            if (prev) {
                prev->next = current->next;
            } else {
                map->buckets[bucket_index].next = current->next;
            }
            
            // Free the node
            free_node(current, map->dealloc_key, map->dealloc_value);
            map->size--;
            
            HASHMAP_LOG("[hashmap_erase] : Erased key %p from bucket %zu", (void*)key, bucket_index);
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    HASHMAP_LOG("[hashmap_erase] : Key %p not found for erasure", (void*)key);
    return false;
}

void hashmap_clear(HashMap* map) {
    if (!map) return;
    
    for (size_t i = 0; i < map->bucket_count; i++) {
        HashMapNode* current = map->buckets[i].next;
        while (current) {
            HashMapNode* next = current->next;
            free_node(current, map->dealloc_key, map->dealloc_value);
            current = next;
        }
        map->buckets[i].next = NULL;
    }
    
    map->size = 0;
    HASHMAP_LOG("[hashmap_clear] : Cleared all elements from HashMap");
}

// Bucket interface functions
size_t hashmap_bucket(const HashMap* map, KeyType key) {
    if (!map || !key) return 0;
    return map->hash_func(key) % map->bucket_count;
}

HashMapIterator hashmap_begin_bucket(const HashMap* map, size_t bucket_index) {
    HashMapIterator it = hashmap_end(map);
    if (!map || bucket_index >= map->bucket_count) return it;
    
    it.map = (HashMap*)map;
    it.bucket_index = bucket_index;
    it.current_node = map->buckets[bucket_index].next;
    
    return it;
}

HashMapIterator hashmap_end_bucket(const HashMap* map, size_t bucket_index) {
    HashMapIterator it = hashmap_end(map);
    if (!map || bucket_index >= map->bucket_count) return it;
    
    it.map = (HashMap*)map;
    it.bucket_index = bucket_index;
    it.current_node = NULL;
    
    return it;
}

// Hash policy functions
float hashmap_load_factor(const HashMap* map) {
    if (!map || map->bucket_count == 0) return 0.0f;
    return (float)map->size / map->bucket_count;
}

float hashmap_max_load_factor(const HashMap* map) {
    if (!map) return 0.0f;
    return (float)map->max_load_factor_numerator / map->max_load_factor_denominator;
}

void hashmap_max_load_factor_set(HashMap* map, float max_load_factor) {
    if (!map || max_load_factor <= 0.0f) return;
    
    map->max_load_factor_numerator = (size_t)(max_load_factor * 1000);
    map->max_load_factor_denominator = 1000;
    
    HASHMAP_LOG("[hashmap_max_load_factor_set] : Set max load factor to %.3f", max_load_factor);
}

void hashmap_rehash(HashMap* map, size_t bucket_count) {
    if (!map || bucket_count == 0) return;
    
    HASHMAP_LOG("[hashmap_rehash] : Rehashing from %zu to %zu buckets", map->bucket_count, bucket_count);
    
    // Save old data
    HashMapNode* old_buckets = map->buckets;
    size_t old_bucket_count = map->bucket_count;
    
    // Create new buckets
    map->buckets = (HashMapNode*)calloc(bucket_count, sizeof(HashMapNode));
    if (!map->buckets) {
        HASHMAP_LOG("[hashmap_rehash] : Error: Failed to allocate new buckets");
        return;
    }
    
    // Initialize new buckets
    for (size_t i = 0; i < bucket_count; i++) {
        map->buckets[i].is_occupied = false;
        map->buckets[i].next = NULL;
    }
    
    map->bucket_count = bucket_count;
    map->size = 0;
    
    // Reinsert all elements
    for (size_t i = 0; i < old_bucket_count; i++) {
        HashMapNode* current = old_buckets[i].next;
        while (current) {
            HashMapNode* next = current->next;
            hashmap_insert(map, current->key, current->value);
            free(current); // Don't call free_node here as we're reusing the key/value
            current = next;
        }
    }
    
    free(old_buckets);
    HASHMAP_LOG("[hashmap_rehash] : Rehash completed, size: %zu", map->size);
}

void hashmap_reserve(HashMap* map, size_t count) {
    if (!map) return;
    
    size_t required_buckets = (size_t)(count / hashmap_max_load_factor(map));
    if (required_buckets > map->bucket_count) {
        hashmap_rehash(map, next_prime(required_buckets));
    }
}

// Observer functions
HashFunc hashmap_hash_function(const HashMap* map) {
    return map ? map->hash_func : NULL;
}

CompareFuncHashMap hashmap_key_eq(const HashMap* map) {
    return map ? map->compare_func : NULL;
}

// Non-member functions
bool hashmap_equal(const HashMap* map1, const HashMap* map2) {
    if (!map1 || !map2) return map1 == map2;
    if (map1->size != map2->size) return false;
    
    HashMapIterator it1 = hashmap_begin(map1);
    HashMapIterator end1 = hashmap_end(map1);
    
    while (!hashmap_iterator_equal(&it1, &end1)) {
        HashMapEntry entry1 = hashmap_iterator_dereference(&it1);
        if (hashmap_count(map2, entry1.key) == 0) return false;
        hashmap_iterator_increment(&it1);
    }
    
    return true;
}

bool hashmap_not_equal(const HashMap* map1, const HashMap* map2) {
    return !hashmap_equal(map1, map2);
}

void hashmap_swap(HashMap* map1, HashMap* map2) {
    if (!map1 || !map2) return;
    
    HashMap temp = *map1;
    *map1 = *map2;
    *map2 = temp;
    
    HASHMAP_LOG("[hashmap_swap] : Swapped two HashMaps");
}

// Utility functions
void hashmap_print(const HashMap* map, void (*print_key)(const KeyType), void (*print_value)(const ValueType)) {
    if (!map) {
        printf("HashMap: NULL\n");
        return;
    }
    
    printf("HashMap (size: %zu, buckets: %zu, load factor: %.3f):\n", 
           map->size, map->bucket_count, hashmap_load_factor(map));
    
    HashMapIterator it = hashmap_begin(map);
    HashMapIterator end = hashmap_end(map);
    
    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry entry = hashmap_iterator_dereference(&it);
        printf("  ");
        if (print_key) print_key(entry.key);
        else printf("%p", (void*)entry.key);
        printf(" -> ");
        if (print_value) print_value(entry.value);
        else printf("%p", (void*)entry.value);
        printf("\n");
        hashmap_iterator_increment(&it);
    }
}

HashMap* hashmap_copy(const HashMap* src) {
    if (!src) return NULL;
    
    HashMap* dest = hashmap_create_with_buckets(src->hash_func, src->compare_func,
                                              src->dealloc_key, src->dealloc_value,
                                              src->bucket_count);
    if (!dest) return NULL;
    
    HashMapIterator it = hashmap_begin(src);
    HashMapIterator end = hashmap_end(src);
    
    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry entry = hashmap_iterator_dereference(&it);
        hashmap_insert(dest, entry.key, entry.value);
        hashmap_iterator_increment(&it);
    }
    
    return dest;
} 