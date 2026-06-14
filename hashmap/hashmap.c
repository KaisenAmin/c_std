/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class HashMap
 *
 * Separate-chaining hash map, modeled on std::unordered_map. `buckets`
 * is an array of bucket_count head POINTERS (NULL == empty bucket); each
 * entry is a node carved from a per-map slab POOL and pushed to the front
 * of its bucket's `next` chain. There is no per-bucket sentinel and no
 * malloc per node: 8 bytes per bucket (not a 32-byte node) and slab-pooled
 * entries, so memory tracks the data like std::unordered_map and teardown
 * is O(slabs) instead of one free() per element.
 *
 * Insert is amortised O(1); a `size / bucket_count >= max_load_factor`
 * triggers `hashmap_rehash` to the next prime ≥ 2 × bucket_count.
 *
 * All public functions are NULL-safe (return a documented sentinel —
 * NULL, false, 0, etc. — rather than crashing).
 */

#include <stdlib.h>
#include <stdint.h>   
#include <string.h>
#include <math.h>
#include "hashmap.h"



// Default bucket count
#define DEFAULT_BUCKET_COUNT 16
#define DEFAULT_MAX_LOAD_FACTOR 0.75f


#define HMAP_FIRST_SLAB_CAP 8
#define HMAP_MAX_SLAB_CAP   (65536u / sizeof(HashMapNode))


struct HashMapSlab {
    struct HashMapSlab* next;
    size_t              used;
    size_t              cap;   
};


static HashMapNode* hmap_pool_alloc(HashMap* map) {
    HashMapNode* node;

    if (map->freeList) {
        node = map->freeList;
        map->freeList = node->next;
    }
    else {
        HashMapSlab* slab = map->slabs;

        if (!slab || slab->used == slab->cap) {
            size_t cap = map->slabCap;
            slab = (HashMapSlab*)malloc(sizeof(HashMapSlab) + cap * sizeof(HashMapNode));
            if (!slab) {
                HASHMAP_LOG("[hmap_pool_alloc] : Error: slab malloc failed");
                return NULL;
            }
            slab->used = 0;
            slab->cap  = cap;
            slab->next = map->slabs;
            map->slabs = slab;

            /* Grow the next slab geometrically, capped at ~64 KB. */
            if (map->slabCap < HMAP_MAX_SLAB_CAP) {
                map->slabCap *= 2;
                if (map->slabCap > HMAP_MAX_SLAB_CAP) {
                    map->slabCap = HMAP_MAX_SLAB_CAP;
                }
            }
        }
        node = (HashMapNode*)((unsigned char*)slab + sizeof(HashMapSlab) + slab->used * sizeof(HashMapNode));
        slab->used++;
    }
    return node;
}


/* Return a node's slot to the free-list for reuse (used on erase). */
static void hmap_pool_free(HashMap* map, HashMapNode* node) {
    node->next = map->freeList;
    map->freeList = node;
}


/* Release every slab (used on clear / deallocate). O(number of slabs). */
static void hmap_pool_destroy(HashMap* map) {
    HashMapSlab* s = map->slabs;

    while (s) {
        HashMapSlab* next = s->next;
        free(s);
        s = next;
    }
    map->slabs = NULL;
    map->freeList = NULL;
}


// Helper function to find the next prime number
static size_t hmap_next_prime(size_t n) {
    HASHMAP_LOG("[hmap_next_prime] : Searching next prime >= %zu", n);

    if (n <= 2) {
        HASHMAP_LOG("[hmap_next_prime] : Returning 2 (n <= 2)");
        return 2;
    }
    if (n % 2 == 0) {
        n++;
    }

    while (true) {
        bool is_prime = true;
        for (size_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            HASHMAP_LOG("[hmap_next_prime] : Returning prime %zu", n);
            return n;
        }
        n += 2;
    }
}


// Helper function to create a new node (carved from the per-map pool)
static HashMapNode* hmap_create_node(HashMap* map, KeyType key, ValueType value) {
    HashMapNode* node = hmap_pool_alloc(map);
    if (!node) {
        HASHMAP_LOG("[hmap_create_node] : Error: Cannot allocate memory for node");
        return NULL;
    }

    node->key = key;
    node->value = value;
    node->next = NULL;
    node->is_occupied = true;

    HASHMAP_LOG("[hmap_create_node] : Created node with key: %p, value: %p", (void*)key, (void*)value);
    return node;
}


static void hmap_free_node(HashMap* map, HashMapNode* node) {
    if (!node) {
        HASHMAP_LOG("[hmap_free_node] : NULL node, nothing to free");
        return;
    }

    if (map->dealloc_key && node->key) {
        map->dealloc_key(node->key);
    }
    if (map->dealloc_value && node->value) {
        map->dealloc_value(node->value);
    }

    hmap_pool_free(map, node);
    HASHMAP_LOG("[hmap_free_node] : Node returned to pool");
}


// Walk every live entry to run the optional key/value deallocators, then
// release all node memory in one O(slabs) sweep. Only walks when a
// deallocator is set
static void hmap_teardown_nodes(HashMap* map) {
    if (map->dealloc_key || map->dealloc_value) {
        for (size_t i = 0; i < map->bucket_count; i++) {
            HashMapNode* current = map->buckets[i];
            while (current) {
                if (map->dealloc_key && current->key) {
                    map->dealloc_key(current->key);
                }
                if (map->dealloc_value && current->value) {
                    map->dealloc_value(current->value);
                }
                current = current->next;
            }
        }
    }
    hmap_pool_destroy(map);
}


// Find a node matching `key` in the chain whose head is `head` (NULL = empty).
static HashMapNode* hmap_find_node_in_bucket(HashMapNode* head, KeyType key, CompareFuncHashMap compare_func) {
    HashMapNode* current = head;
    size_t steps = 0;
    while (current) {
        if (current->is_occupied && compare_func(current->key, key) == 0) {
            HASHMAP_LOG("[hmap_find_node_in_bucket] : Found node %p after %zu step(s)", (void*)current, steps);
            return current;
        }
        current = current->next;
        steps++;
    }

    HASHMAP_LOG("[hmap_find_node_in_bucket] : Key %p not found (walked %zu node(s))", (void*)key, steps);
    return NULL;
}


// Find the next non-empty bucket at or after start_index (bucket_count = end).
static size_t hmap_find_next_occupied_bucket(const HashMap* map, size_t start_index) {
    for (size_t i = start_index; i < map->bucket_count; i++) {
        if (map->buckets[i] != NULL) {
            HASHMAP_LOG("[hmap_find_next_occupied_bucket] : Found occupied bucket at %zu", i);
            return i;
        }
    }

    HASHMAP_LOG("[hmap_find_next_occupied_bucket] : No occupied bucket found, returning end (%zu)", map->bucket_count);
    return map->bucket_count; // End iterator
}


/**
 * @brief Default hash function for `int*` keys.
 *
 * Reinterprets the int value as a `size_t` — fine for non-adversarial
 * inputs, but trivially collidable if attacker-controlled. Suitable
 * for application code that owns its keyspace.
 *
 * @param key Pointer to an `int`. NULL → 0.
 * @return Hash value.
 */
size_t hashmap_hash_int(const KeyType key) {
    if (!key) {
        HASHMAP_LOG("[hashmap_hash_int] : NULL key, returning 0");
        return 0;
    }
    int value = *(int*)key;

    HASHMAP_LOG("[hashmap_hash_int] : int %d -> hash %zu", value, (size_t)value);
    return (size_t)value;
}

/**
 * @brief Default hash function for null-terminated C strings (djb2).
 *
 * Implements djb2: `hash = hash * 33 + c` starting from 5381. Decent
 * distribution for typical text input.
 *
 * @param key Pointer to a NUL-terminated string. NULL → 0.
 * @return Hash value.
 */
size_t hashmap_hash_string(const KeyType key) {
    if (!key) {
        HASHMAP_LOG("[hashmap_hash_string] : NULL key, returning 0");
        return 0;
    }
    const char* str = (const char*)key;
    size_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    HASHMAP_LOG("[hashmap_hash_string] : \"%s\" -> hash %zu", (const char*)key, hash);
    return hash;
}

/**
 * @brief Default hash function for raw pointer keys (identity).
 *
 * Returns the pointer reinterpreted as `size_t` — uses the address
 * itself as the hash. Useful when keys are object identities.
 *
 * @param key Pointer to use as the key. NULL → 0.
 * @return The pointer value as a `size_t`.
 */
size_t hashmap_hash_pointer(const KeyType key) {
    if (!key) {
        HASHMAP_LOG("[hashmap_hash_pointer] : NULL key, returning 0");
        return 0;
    }

    HASHMAP_LOG("[hashmap_hash_pointer] : pointer %p -> hash %zu", (void*)key, (size_t)key);
    return (size_t)key;
}

/**
 * @brief Default comparator for `int*` keys.
 *
 * Three-way comparison (-1 / 0 / +1) over `*(int*)key1` vs
 * `*(int*)key2`. Both NULL compare equal; only one NULL compares less.
 */
int hashmap_compare_int(const KeyType key1, const KeyType key2) {
    if (!key1 && !key2) {
        HASHMAP_LOG("[hashmap_compare_int] : both NULL -> 0");
        return 0;
    }
    if (!key1) {
        HASHMAP_LOG("[hashmap_compare_int] : key1 NULL -> -1");
        return -1;
    }
    if (!key2) {
        HASHMAP_LOG("[hashmap_compare_int] : key2 NULL -> 1");
        return 1;
    }

    int val1 = *(int*)key1;
    int val2 = *(int*)key2;

    if (val1 < val2) {
        HASHMAP_LOG("[hashmap_compare_int] : %d < %d -> -1", val1, val2);
        return -1;
    }
    if (val1 > val2) {
        HASHMAP_LOG("[hashmap_compare_int] : %d > %d -> 1", val1, val2);
        return 1;
    }

    HASHMAP_LOG("[hashmap_compare_int] : %d == %d -> 0", val1, val2);
    return 0;
}


/**
 * @brief Default comparator for C-string keys.
 *
 * Thin wrapper over `strcmp`. Both NULL compare equal; only one NULL
 * compares less.
 */
int hashmap_compare_string(const KeyType key1, const KeyType key2) {
    if (!key1 && !key2) {
        HASHMAP_LOG("[hashmap_compare_string] : both NULL -> 0");
        return 0;
    }
    if (!key1) {
        HASHMAP_LOG("[hashmap_compare_string] : key1 NULL -> -1");
        return -1;
    }
    if (!key2) {
        HASHMAP_LOG("[hashmap_compare_string] : key2 NULL -> 1");
        return 1;
    }

    int r = strcmp((const char*)key1, (const char*)key2);
    HASHMAP_LOG("[hashmap_compare_string] : strcmp(\"%s\", \"%s\") -> %d", (const char*)key1, (const char*)key2, r);
    return r;
}


/**
 * @brief Default comparator for raw pointer keys.
 *
 * Three-way comparison over the address values themselves.
 */
int hashmap_compare_pointer(const KeyType key1, const KeyType key2) {
    if (!key1 && !key2) {
        HASHMAP_LOG("[hashmap_compare_pointer] : both NULL -> 0");
        return 0;
    }
    if (!key1) {
        HASHMAP_LOG("[hashmap_compare_pointer] : key1 NULL -> -1");
        return -1;
    }
    if (!key2) {
        HASHMAP_LOG("[hashmap_compare_pointer] : key2 NULL -> 1");
        return 1;
    }

    if (key1 < key2) {
        HASHMAP_LOG("[hashmap_compare_pointer] : %p < %p -> -1", (void*)key1, (void*)key2);
        return -1;
    }
    if (key1 > key2) {
        HASHMAP_LOG("[hashmap_compare_pointer] : %p > %p -> 1", (void*)key1, (void*)key2);
        return 1;
    }

    HASHMAP_LOG("[hashmap_compare_pointer] : %p == %p -> 0", (void*)key1, (void*)key2);
    return 0;
}


/**
 * @brief Create a HashMap with the default initial bucket count
 *        (`DEFAULT_BUCKET_COUNT` = 16, rounded up to the next prime).
 *
 * @param hash_func     Required. Function used to hash a key.
 * @param compare_func  Required. 3-way comparator returning 0 on equal.
 * @param dealloc_key   Optional. Called on each key during clear / erase /
 *                      deallocate. Pass `NULL` if keys are not owned.
 * @param dealloc_value Optional. Same for values.
 * @return Newly-allocated HashMap, or NULL on bad input / OOM.
 */
HashMap* hashmap_create(HashFunc hash_func, CompareFuncHashMap compare_func, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value) {
    return hashmap_create_with_buckets(hash_func, compare_func, dealloc_key, dealloc_value, DEFAULT_BUCKET_COUNT);
}


/**
 * @brief Create a HashMap with a caller-specified initial bucket count.
 *
 * The actual bucket count is `hmap_next_prime(max(initial_bucket_count, 1))` —
 * primes give the modulo-based hashing much better collision behaviour
 * than power-of-two sizes.
 *
 * @param hash_func           Required.
 * @param compare_func        Required.
 * @param dealloc_key         Optional. See `hashmap_create`.
 * @param dealloc_value       Optional. See `hashmap_create`.
 * @param initial_bucket_count Lower bound on the bucket count; the value
 *                            actually used is rounded up to the next prime.
 * @return Newly-allocated HashMap, or NULL on bad input / OOM.
 */
HashMap* hashmap_create_with_buckets(HashFunc hash_func, CompareFuncHashMap compare_func, ValueDeallocFunc dealloc_key, ValueDeallocFunc dealloc_value,
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
    size_t bucket_count = hmap_next_prime(initial_bucket_count > 0 ? initial_bucket_count : 1);

    map->buckets = (HashMapNode**)calloc(bucket_count, sizeof(HashMapNode*));
    if (!map->buckets) {
        HASHMAP_LOG("[hashmap_create_with_buckets] : Error: Cannot allocate memory for buckets");
        free(map);
        return NULL;
    }

    map->bucket_count = bucket_count;
    map->size = 0;
    map->max_load_factor_numerator = 3;
    map->max_load_factor_denominator = 4; // 0.75
    map->hash_func = hash_func;
    map->compare_func = compare_func;
    map->dealloc_key = dealloc_key;
    map->dealloc_value = dealloc_value;

    /* Node pool: small first slab (cheap for many-small-maps), doubling up to
     * a ~64 KB cap so a huge map still amortises to very few mallocs. */
    map->slabs = NULL;
    map->freeList = NULL;
    map->slabCap = HMAP_FIRST_SLAB_CAP;

    HASHMAP_LOG("[hashmap_create_with_buckets] : Created HashMap with %zu buckets", bucket_count);
    return map;
}


/**
 * @brief Release every node, the bucket array, and the map struct itself.
 *
 * Calls the configured `dealloc_key` / `dealloc_value` on every entry
 * before freeing. NULL is a safe no-op.
 */
void hashmap_deallocate(HashMap* map) {
    if (!map) {
        return;
    }

    hmap_teardown_nodes(map);   /* dealloc live entries (if callbacks) + free all slabs */
    free(map->buckets);
    free(map);

    HASHMAP_LOG("[hashmap_deallocate] : Deallocated HashMap");
}


/**
 * @brief Is the map empty?
 * @return `true` if the map is NULL or has 0 entries.
 */
bool hashmap_empty(const HashMap* map) {
    if (!map) {
        return true;
    }

    return map->size == 0;
}


/**
 * @brief Number of entries currently stored.
 * @return Entry count, or 0 if @p map is NULL.
 */
size_t hashmap_size(const HashMap* map) {
    if (!map) {
        return 0;
    }

    return map->size;
}


/**
 * @brief Current bucket-array length (always a prime).
 * @return Bucket count, or 0 if @p map is NULL.
 */
size_t hashmap_bucket_count(const HashMap* map) {
    if (!map) {
        return 0;
    }

    return map->bucket_count;
}


/**
 * @brief Theoretical maximum number of buckets the map could allocate.
 * @return `SIZE_MAX / sizeof(HashMapNode)`, or 0 if @p map is NULL.
 */
size_t hashmap_max_bucket_count(const HashMap* map) {
    if (!map) {
        return 0;
    }

    return SIZE_MAX / sizeof(HashMapNode);
}


/**
 * @brief Number of entries in a single bucket.
 *
 * Useful for measuring hash distribution. The bucket head is a
 * sentinel — only the chain (`.next`) entries are counted.
 *
 * @param map           HashMap.
 * @param bucket_index  0-based bucket index, must be < bucket_count.
 * @return Entries in that bucket, or 0 on bad arg / OOR.
 */
size_t hashmap_bucket_size(const HashMap* map, size_t bucket_index) {
    if (!map || bucket_index >= map->bucket_count) {
        return 0;
    }

    size_t count = 0;
    HashMapNode* current = map->buckets[bucket_index];

    while (current) {
        if (current->is_occupied) {
            count++;
        }
        current = current->next;
    }

    return count;
}


/**
 * @brief Look up the value associated with @p key.
 *
 * Read-only: the map is unchanged. Returns NULL if the key is missing
 * or any argument is NULL. The lib does NOT distinguish between
 * "missing key" and "key present with NULL value" — use
 * `hashmap_contains` if you need to tell them apart.
 *
 * @return Borrowed value pointer owned by the map.
 */
ValueType hashmap_at(const HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_at] : Error: NULL map or key");
        return NULL;
    }

    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* node = hmap_find_node_in_bucket(map->buckets[bucket_index], key, map->compare_func);

    if (node) {
        HASHMAP_LOG("[hashmap_at] : Found value for key %p: %p", (void*)key, (void*)node->value);
        return node->value;
    }

    HASHMAP_LOG("[hashmap_at] : Key %p not found", (void*)key);
    return NULL;
}


/**
 * @brief Lookup-or-insert: returns the current value for @p key, or
 *        creates a new entry with value `NULL` and returns NULL.
 *
 * Analogous to C++'s `unordered_map::operator[]`. Note that NULL is
 * both the "newly-inserted" sentinel and a legitimate value — use
 * `hashmap_contains` first if that ambiguity matters.
 *
 * @return Borrowed value pointer, or NULL.
 */
ValueType hashmap_operator_bracket(HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_operator_bracket] : Error: NULL map or key");
        return NULL;
    }

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


/**
 * @brief Iterator pointing at the first entry in the map.
 *
 * Walk via `hashmap_iterator_increment` until the iterator equals
 * `hashmap_end`. Iteration order is not stable across rehashes.
 */
HashMapIterator hashmap_begin(const HashMap* map) {
    HashMapIterator it = {NULL, 0, NULL};
    if (!map) {
        return it;
    }

    it.map = (HashMap*)map;
    it.bucket_index = hmap_find_next_occupied_bucket(map, 0);

    if (it.bucket_index < map->bucket_count) {
        it.current_node = map->buckets[it.bucket_index];
    }

    return it;
}


/**
 * @brief Past-the-end iterator. Compares equal only to itself / an
 *        iterator that has walked off the end.
 */
HashMapIterator hashmap_end(const HashMap* map) {
    HashMapIterator it = {NULL, 0, NULL};
    if (!map) {
        HASHMAP_LOG("[hashmap_end] : NULL map -> sentinel iterator");
        return it;
    }

    it.map = (HashMap*)map;
    it.bucket_index = map->bucket_count;
    it.current_node = NULL;

    HASHMAP_LOG("[hashmap_end] : Past-the-end iterator at bucket_index=%zu", map->bucket_count);
    return it;
}


/**
 * @brief Const variant of `hashmap_begin`. Currently identical — kept
 *        for STL-style API symmetry.
 */
HashMapIterator hashmap_cbegin(const HashMap* map) {
    HASHMAP_LOG("[hashmap_cbegin] : Delegating to hashmap_begin");
    return hashmap_begin(map);
}


/**
 * @brief Const variant of `hashmap_end`.
 */
HashMapIterator hashmap_cend(const HashMap* map) {
    HASHMAP_LOG("[hashmap_cend] : Delegating to hashmap_end");
    return hashmap_end(map);
}


/**
 * @brief Advance @p it to the next entry.
 *
 * Walks the current bucket's chain first, then probes forward for the
 * next non-empty bucket. Becomes the end iterator when no further
 * entry exists. NULL @p it is a safe no-op.
 */
void hashmap_iterator_increment(HashMapIterator* it) {
    if (!it || !it->map) {
        return;
    }
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
    it->bucket_index = hmap_find_next_occupied_bucket(it->map, it->bucket_index + 1);

    if (it->bucket_index < it->map->bucket_count) {
        it->current_node = it->map->buckets[it->bucket_index];
        HASHMAP_LOG("[hashmap_iterator_increment] : Moved to bucket %zu", it->bucket_index);
    }
    else {
        it->current_node = NULL; // End iterator
        HASHMAP_LOG("[hashmap_iterator_increment] : Reached end");
    }
}


/**
 * @brief Step the iterator backwards.
 *
 * Walks the bucket chain to find the predecessor of the current node;
 * if the current node is the head of its bucket, scans earlier buckets
 * for the LAST node of the closest occupied bucket. From the past-the-
 * end iterator (`current_node == NULL`), moves to the last entry in
 * the map.
 *
 * Decrementing the begin iterator is a no-op (per STL convention,
 * stepping below begin is undefined; we leave the iterator unchanged
 * rather than corrupt it).
 *
 * Complexity: O(bucket_size + bucket_count) worst case — finding the
 * predecessor in a singly-linked chain requires walking the chain from
 * the head, and finding the previous occupied bucket may require
 * scanning earlier buckets.
 */
void hashmap_iterator_decrement(HashMapIterator* it) {
    if (!it || !it->map) {
        return;
    }

    HashMap* map = it->map;

    if (it->current_node == NULL) {
        if (it->bucket_index < map->bucket_count) {
            HashMapNode* node = map->buckets[it->bucket_index];
            if (node) {
                while (node->next) {
                    node = node->next;
                }
                it->current_node = node;
                HASHMAP_LOG("[hashmap_iterator_decrement] : end-of-bucket -> last node in bucket %zu", it->bucket_index);
                return;
            }
        }

        size_t b = it->bucket_index;
        while (b > 0) {
            b--;
            HashMapNode* node = map->buckets[b];
            if (node) {
                while (node->next) {
                    node = node->next;
                }
                it->bucket_index = b;
                it->current_node = node;
                HASHMAP_LOG("[hashmap_iterator_decrement] : end -> last entry in bucket %zu", b);
                return;
            }
        }

        HASHMAP_LOG("[hashmap_iterator_decrement] : Map empty, no decrement.");
        return;
    }

    HashMapNode* head = map->buckets[it->bucket_index];
    if (head == it->current_node) {

        size_t b = it->bucket_index;
        while (b > 0) {
            b--;
            HashMapNode* node = map->buckets[b];
            if (node) {
                while (node->next) {
                    node = node->next;
                }
                it->bucket_index = b;
                it->current_node = node;
                HASHMAP_LOG("[hashmap_iterator_decrement] : crossed to last node of bucket %zu", b);
                return;
            }
        }

        HASHMAP_LOG("[hashmap_iterator_decrement] : At begin(), no decrement.");
        return;
    }


    HashMapNode* prev = head;
    while (prev && prev->next != it->current_node) {
        prev = prev->next;
    }
    if (prev) {
        it->current_node = prev;
        HASHMAP_LOG("[hashmap_iterator_decrement] : Moved to predecessor in bucket %zu", it->bucket_index);
    }
}


/**
 * @brief Iterator equality: same map, same bucket, same node pointer.
 *
 * @return `false` if either argument is NULL.
 */
bool hashmap_iterator_equal(const HashMapIterator* it1, const HashMapIterator* it2) {
    if (!it1 || !it2) {
        HASHMAP_LOG("[hashmap_iterator_equal] : NULL operand -> false");
        return false;
    }

    bool eq = it1->map == it2->map &&
              it1->bucket_index == it2->bucket_index &&
              it1->current_node == it2->current_node;
    HASHMAP_LOG("[hashmap_iterator_equal] : (%p,%zu,%p) vs (%p,%zu,%p) -> %s",
                (void*)it1->map, it1->bucket_index, (void*)it1->current_node,
                (void*)it2->map, it2->bucket_index, (void*)it2->current_node,
                eq ? "true" : "false");
    return eq;
}


/**
 * @brief Dereference: return the entry the iterator points at.
 *
 * The returned `HashMapEntry` is a value copy of the (borrowed) key
 * and value pointers — modifying it does not change the map. An
 * end / NULL iterator yields `{NULL, NULL}`.
 */
HashMapEntry hashmap_iterator_dereference(const HashMapIterator* it) {
    HashMapEntry entry = {NULL, NULL};
    if (!it || !it->current_node) {
        HASHMAP_LOG("[hashmap_iterator_dereference] : NULL or end iterator -> {NULL, NULL}");
        return entry;
    }

    entry.key = it->current_node->key;
    entry.value = it->current_node->value;

    HASHMAP_LOG("[hashmap_iterator_dereference] : -> {key=%p, value=%p}", (void*)entry.key, (void*)entry.value);
    return entry;
}


/**
 * @brief Find an entry by key.
 *
 * @return Iterator pointing at the entry, or `hashmap_end(map)` if
 *         the key is absent / any argument is NULL.
 */
HashMapIterator hashmap_find(const HashMap* map, KeyType key) {
    HashMapIterator it = hashmap_end(map);
    if (!map || !key) {
        return it;
    }

    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* node = hmap_find_node_in_bucket(map->buckets[bucket_index], key, map->compare_func);

    if (node) {
        it.map = (HashMap*)map;
        it.bucket_index = bucket_index;
        it.current_node = node;

        HASHMAP_LOG("[hashmap_find] : Found key %p in bucket %zu", (void*)key, bucket_index);
    }

    return it;
}


/**
 * @brief Multiplicity of @p key.
 *
 * Since this is a uniquely-keyed map, the return value is 0 or 1.
 */
size_t hashmap_count(const HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_count] : NULL map or key -> 0");
        return 0;
    }

    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* node = hmap_find_node_in_bucket(map->buckets[bucket_index], key, map->compare_func);

    HASHMAP_LOG("[hashmap_count] : Key %p in bucket %zu -> %zu", (void*)key, bucket_index, node ? (size_t)1 : (size_t)0);
    return node ? 1 : 0;
}


/**
 * @brief Convenience: `hashmap_count(map, key) > 0`.
 */
bool hashmap_contains(const HashMap* map, KeyType key) {
    bool has = hashmap_count(map, key) > 0;
    HASHMAP_LOG("[hashmap_contains] : Key %p -> %s", (void*)key, has ? "true" : "false");
    return has;
}

/**
 * @brief Insert or update.
 *
 * If @p key already exists: the existing value is replaced (the old
 * value is freed via `dealloc_value` if configured) and the function
 * returns `true`. If absent: a new node is created and the function
 * returns `true`. A failed allocation returns `false`.
 *
 * Insert can trigger a rehash when `size / bucket_count >=
 * max_load_factor`. After the rehash, this same call inserts into
 * the freshly-sized table.
 *
 * @note The map stores the supplied @p key and @p value pointers
 *       AS-IS — it does not copy them. They must outlive the map (or
 *       the map's deallocators must take ownership and free them).
 */
bool hashmap_insert(HashMap* map, KeyType key, ValueType value) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_insert] : Error: NULL map or key");
        return false;
    }

    // Check if key already exists
    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* existing_node = hmap_find_node_in_bucket(map->buckets[bucket_index], key, map->compare_func);

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
        size_t new_bucket_count = hmap_next_prime(map->bucket_count * 2);
        hashmap_rehash(map, new_bucket_count);
        bucket_index = map->hash_func(key) % map->bucket_count;
    }

    HashMapNode* new_node = hmap_create_node(map, key, value);
    if (!new_node) {
        HASHMAP_LOG("[hashmap_insert] : Error: Failed to create new node");
        return false;
    }

    new_node->next = map->buckets[bucket_index];
    map->buckets[bucket_index] = new_node;
    map->size++;

    HASHMAP_LOG("[hashmap_insert] : Inserted key %p in bucket %zu", (void*)key, bucket_index);
    return true;
}


/**
 * @brief Alias for `hashmap_insert` — kept for STL-style symmetry.
 *
 * In the absence of move-semantics, "emplace" and "insert" behave
 * identically in this implementation.
 */
bool hashmap_emplace(HashMap* map, KeyType key, ValueType value) {
    HASHMAP_LOG("[hashmap_emplace] : Delegating to hashmap_insert (key=%p, value=%p)", (void*)key, (void*)value);
    return hashmap_insert(map, key, value);
}


/**
 * @brief Remove the entry with the given @p key.
 *
 * If the key exists: the matching node is unlinked, its key and value
 * are freed via the configured deallocators, and the function returns
 * `true`. If the key is missing or any argument is NULL, returns
 * `false`.
 */
bool hashmap_erase(HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_erase] : Error: NULL map or key");
        return false;
    }

    size_t bucket_index = map->hash_func(key) % map->bucket_count;
    HashMapNode* current = map->buckets[bucket_index];
    HashMapNode* prev = NULL;

    while (current) {
        if (current->is_occupied && map->compare_func(current->key, key) == 0) {
            // Unlink node from chain (update the bucket head if it was first)
            if (prev) {
                prev->next = current->next;
            }
            else {
                map->buckets[bucket_index] = current->next;
            }

            hmap_free_node(map, current);
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


/**
 * @brief Remove every entry without releasing the bucket array.
 *
 * Iterates every bucket chain, calling the configured deallocators
 * on each node. After `clear`, `size == 0` and the map is reusable.
 */
void hashmap_clear(HashMap* map) {
    if (!map) {
        return;
    }

    hmap_teardown_nodes(map);   /* dealloc live entries (if callbacks) + free all slabs */
    memset(map->buckets, 0, map->bucket_count * sizeof(*map->buckets));  /* all heads -> NULL */

    map->size = 0;
    HASHMAP_LOG("[hashmap_clear] : Cleared all elements from HashMap");
}


/**
 * @brief Bucket index a key would land in.
 *
 * Useful when working with `hashmap_begin_bucket` /
 * `hashmap_end_bucket` to iterate a single chain.
 *
 * @return Bucket index, or 0 on bad arg.
 */
size_t hashmap_bucket(const HashMap* map, KeyType key) {
    if (!map || !key) {
        HASHMAP_LOG("[hashmap_bucket] : NULL map or key -> 0");
        return 0;
    }

    size_t idx = map->hash_func(key) % map->bucket_count;
    HASHMAP_LOG("[hashmap_bucket] : Key %p -> bucket %zu", (void*)key, idx);
    return idx;
}


/**
 * @brief Iterator pointing at the first entry in @p bucket_index.
 *
 * On bad arg / OOR returns `hashmap_end`.
 */
HashMapIterator hashmap_begin_bucket(const HashMap* map, size_t bucket_index) {
    HashMapIterator it = hashmap_end(map);
    if (!map || bucket_index >= map->bucket_count) {
        HASHMAP_LOG("[hashmap_begin_bucket] : Bad arg (map=%p, bucket_index=%zu) -> end()", (void*)map, bucket_index);
        return it;
    }

    it.map = (HashMap*)map;
    it.bucket_index = bucket_index;
    it.current_node = map->buckets[bucket_index];

    HASHMAP_LOG("[hashmap_begin_bucket] : Iterator to bucket %zu head (node=%p)", bucket_index, (void*)it.current_node);
    return it;
}


/**
 * @brief Past-the-end iterator for a single bucket — same shape as
 *        `hashmap_end` but with a non-zero bucket_index.
 */
HashMapIterator hashmap_end_bucket(const HashMap* map, size_t bucket_index) {
    HashMapIterator it = hashmap_end(map);
    if (!map || bucket_index >= map->bucket_count) {
        HASHMAP_LOG("[hashmap_end_bucket] : Bad arg (map=%p, bucket_index=%zu) -> end()", (void*)map, bucket_index);
        return it;
    }

    it.map = (HashMap*)map;
    it.bucket_index = bucket_index;
    it.current_node = NULL;

    HASHMAP_LOG("[hashmap_end_bucket] : End-of-bucket iterator at bucket %zu", bucket_index);
    return it;
}


/**
 * @brief Current load factor `size / bucket_count`.
 * @return 0.0 on NULL map / 0 buckets.
 */
float hashmap_load_factor(const HashMap* map) {
    if (!map || map->bucket_count == 0) {
        HASHMAP_LOG("[hashmap_load_factor] : NULL map or zero buckets -> 0.0");
        return 0.0f;
    }

    float lf = (float)map->size / map->bucket_count;
    HASHMAP_LOG("[hashmap_load_factor] : size=%zu / buckets=%zu -> %.3f", map->size, map->bucket_count, lf);
    return lf;
}


/**
 * @brief Configured maximum load factor (default 0.75).
 *
 * When `load_factor >= max_load_factor`, the next `hashmap_insert`
 * triggers a rehash.
 *
 * @return 0.0 on NULL map.
 */
float hashmap_max_load_factor(const HashMap* map) {
    if (!map) {
        HASHMAP_LOG("[hashmap_max_load_factor] : NULL map -> 0.0");
        return 0.0f;
    }

    float lf = (float)map->max_load_factor_numerator / map->max_load_factor_denominator;
    HASHMAP_LOG("[hashmap_max_load_factor] : %zu/%zu -> %.3f", map->max_load_factor_numerator, map->max_load_factor_denominator, lf);
    return lf;
}


/**
 * @brief Change the rehash threshold.
 *
 * The value is stored as a numerator / denominator pair (millis), so
 * non-trivial floats are preserved without FP drift.
 *
 * @param map              HashMap.
 * @param max_load_factor  New threshold; values <= 0 are rejected.
 */
void hashmap_max_load_factor_set(HashMap* map, float max_load_factor) {
    if (!map || max_load_factor <= 0.0f) {
        return;
    }

    map->max_load_factor_numerator = (size_t)(max_load_factor * 1000);
    map->max_load_factor_denominator = 1000;

    HASHMAP_LOG("[hashmap_max_load_factor_set] : Set max load factor to %.3f", max_load_factor);
}


/**
 * @brief Resize the bucket array to @p bucket_count slots.
 *
 * Splices existing nodes (no malloc/free of value memory) into a
 * freshly-allocated bucket array, then frees the old array. Safe to
 * call with any positive count; for "just bigger" semantics use
 * `hashmap_reserve` which derives the count from a target entry
 * count and the current max-load-factor.
 *
 * After rehash, `map->size` is unchanged.
 */
void hashmap_rehash(HashMap* map, size_t bucket_count) {
    if (!map || bucket_count == 0) {
        return;
    }

    HASHMAP_LOG("[hashmap_rehash] : Rehashing from %zu to %zu buckets", map->bucket_count, bucket_count);

    HashMapNode** new_buckets = (HashMapNode**)calloc(bucket_count, sizeof(HashMapNode*));
    if (!new_buckets) {
        HASHMAP_LOG("[hashmap_rehash] : Error: Failed to allocate new buckets");
        return;
    }

    // Splice existing nodes into the new bucket array (no node alloc/free)
    HashMapNode** old_buckets = map->buckets;
    size_t old_bucket_count = map->bucket_count;

    for (size_t i = 0; i < old_bucket_count; i++) {
        HashMapNode* current = old_buckets[i];
        while (current) {
            HashMapNode* next = current->next;
            size_t new_index = map->hash_func(current->key) % bucket_count;
            // Push to front of new chain
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;
            current = next;
        }
    }

    map->buckets = new_buckets;
    map->bucket_count = bucket_count;
    // map->size is unchanged: we moved every node, didn't add/remove any.

    free(old_buckets);
    HASHMAP_LOG("[hashmap_rehash] : Rehash completed, size: %zu", map->size);
}


/**
 * @brief Resize the bucket array so it can hold @p count entries
 *        without exceeding the configured max load factor.
 *
 * If the existing array is already large enough, this is a no-op.
 * Otherwise it rehashes to the next prime ≥ `count / max_load_factor`.
 */
void hashmap_reserve(HashMap* map, size_t count) {
    if (!map) {
        HASHMAP_LOG("[hashmap_reserve] : NULL map, ignoring");
        return;
    }

    HASHMAP_LOG("[hashmap_reserve] : Requested capacity %zu (current buckets %zu)", count, map->bucket_count);
    size_t required_buckets = (size_t)(count / hashmap_max_load_factor(map));
    if (required_buckets > map->bucket_count) {
        HASHMAP_LOG("[hashmap_reserve] : Need to rehash to at least %zu buckets", required_buckets);
        hashmap_rehash(map, hmap_next_prime(required_buckets));
    }
    else {
        HASHMAP_LOG("[hashmap_reserve] : Existing bucket array is sufficient, no-op");
    }
}


/**
 * @brief Configured hash function.
 * @return Function pointer, or NULL if @p map is NULL.
 */
HashFunc hashmap_hash_function(const HashMap* map) {
    if (!map) {
        HASHMAP_LOG("[hashmap_hash_function] : NULL map -> NULL");
        return NULL;
    }

    HASHMAP_LOG("[hashmap_hash_function] : Returning hash function %p", (void*)(uintptr_t)map->hash_func);
    return map->hash_func;
}


/**
 * @brief Configured key-equality comparator.
 * @return Function pointer, or NULL if @p map is NULL.
 */
CompareFuncHashMap hashmap_key_eq(const HashMap* map) {
    if (!map) {
        HASHMAP_LOG("[hashmap_key_eq] : NULL map -> NULL");
        return NULL;
    }

    HASHMAP_LOG("[hashmap_key_eq] : Returning compare function %p", (void*)(uintptr_t)map->compare_func);
    return map->compare_func;
}


/**
 * @brief Set-equality: same number of entries AND every key in @p map1
 *        is present in @p map2.
 *
 * @note Values are NOT compared — only key set. Two maps with the same
 *       keys but different values are considered equal by this
 *       function. If you need value-aware equality, walk one map and
 *       compare values via `hashmap_at(map2, key)` yourself.
 */
bool hashmap_equal(const HashMap* map1, const HashMap* map2) {
    HASHMAP_LOG("[hashmap_equal] : Comparing maps %p and %p", (void*)map1, (void*)map2);

    if (!map1 || !map2) {
        bool same = (map1 == map2);
        HASHMAP_LOG("[hashmap_equal] : NULL operand -> %s", same ? "true" : "false");
        return same;
    }
    if (map1->size != map2->size) {
        HASHMAP_LOG("[hashmap_equal] : Size mismatch (%zu vs %zu) -> false", map1->size, map2->size);
        return false;
    }

    HashMapIterator it1 = hashmap_begin(map1);
    HashMapIterator end1 = hashmap_end(map1);

    while (!hashmap_iterator_equal(&it1, &end1)) {
        HashMapEntry entry1 = hashmap_iterator_dereference(&it1);
        if (hashmap_count(map2, entry1.key) == 0) {
            HASHMAP_LOG("[hashmap_equal] : Key %p missing in map2 -> false", (void*)entry1.key);
            return false;
        }
        hashmap_iterator_increment(&it1);
    }

    HASHMAP_LOG("[hashmap_equal] : All keys present in both -> true");
    return true;
}


/**
 * @brief Logical negation of `hashmap_equal`.
 */
bool hashmap_not_equal(const HashMap* map1, const HashMap* map2) {
    bool result = !hashmap_equal(map1, map2);
    HASHMAP_LOG("[hashmap_not_equal] : -> %s", result ? "true" : "false");
    return result;
}


/**
 * @brief Swap the contents of two maps (struct-by-struct value copy).
 *
 * O(1) — internal pointers are exchanged; no node-level work.
 * NULL on either side is a safe no-op.
 */
void hashmap_swap(HashMap* map1, HashMap* map2) {
    if (!map1 || !map2) {
        return;
    }

    HashMap temp = *map1;
    *map1 = *map2;
    *map2 = temp;

    HASHMAP_LOG("[hashmap_swap] : Swapped two HashMaps");
}


/**
 * @brief Print a map's contents in a `(key -> value)` format.
 *
 * If a print callback is NULL, the raw pointer value is printed in
 * `%p` form. Useful for debugging; not intended for production logs.
 *
 * @param map         HashMap (may be NULL → prints "HashMap: NULL").
 * @param print_key   Optional callback to render keys.
 * @param print_value Optional callback to render values.
 */
void hashmap_print(const HashMap* map, void (*print_key)(const KeyType), void (*print_value)(const ValueType)) {
    HASHMAP_LOG("[hashmap_print] : Printing map %p", (void*)map);

    if (!map) {
        HASHMAP_LOG("[hashmap_print] : NULL map");
        printf("HashMap: NULL\n");
        return;
    }

    printf("HashMap (size: %zu, buckets: %zu, load factor: %.3f):\n", map->size, map->bucket_count, hashmap_load_factor(map));

    HashMapIterator it = hashmap_begin(map);
    HashMapIterator end = hashmap_end(map);

    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry entry = hashmap_iterator_dereference(&it);
        printf("  ");
        if (print_key) {
            print_key(entry.key);
        }
        else {
            printf("%p", (void*)entry.key);
        }

        printf(" -> ");
        if (print_value) {
            print_value(entry.value);
        }
        else {
            printf("%p", (void*)entry.value);
        }

        printf("\n");
        hashmap_iterator_increment(&it);
    }
}


/**
 * @brief Shallow copy: clone the bucket layout and re-insert every
 *        (key, value) pair — without copying the underlying key/value
 *        memory.
 *
 * The new map starts with NULL deallocators so a later
 * `hashmap_deallocate` won't double-free what the source still owns.
 * If you need ownership semantics on the copy, write
 * `dest->dealloc_key` / `dest->dealloc_value` after this call.
 *
 * @return Newly-allocated copy, or NULL on bad input / OOM.
 */
HashMap* hashmap_copy(const HashMap* src) {
    HASHMAP_LOG("[hashmap_copy] : Copying from src=%p", (void*)src);

    if (!src) {
        HASHMAP_LOG("[hashmap_copy] : NULL src -> NULL");
        return NULL;
    }

    // The copy is SHALLOW: it shares the same key/value pointers with src.
    HashMap* dest = hashmap_create_with_buckets(src->hash_func, src->compare_func, NULL, NULL, src->bucket_count);
    if (!dest) {
        HASHMAP_LOG("[hashmap_copy] : Failed to allocate destination map");
        return NULL;
    }

    HashMapIterator it = hashmap_begin(src);
    HashMapIterator end = hashmap_end(src);
    size_t copied = 0;

    while (!hashmap_iterator_equal(&it, &end)) {
        HashMapEntry entry = hashmap_iterator_dereference(&it);
        hashmap_insert(dest, entry.key, entry.value);
        copied++;
        hashmap_iterator_increment(&it);
    }

    HASHMAP_LOG("[hashmap_copy] : Copied %zu entr%s -> dest=%p", copied, copied == 1 ? "y" : "ies", (void*)dest);
    return dest;
}
