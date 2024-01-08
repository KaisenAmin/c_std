/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Vector
*/

#include "vector.h"
#include <stdlib.h>
#include <string.h>

static MemoryPoolVector *memory_pool_create(size_t size);
static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size);
static void memory_pool_destroy(MemoryPoolVector *pool);

static MemoryPoolVector *memory_pool_create(size_t size) {
    MemoryPoolVector *pool = malloc(sizeof(MemoryPoolVector));
    if (pool) {
        pool->pool = malloc(size);
        if (!pool->pool) {
            free(pool);
            return NULL;
        }
        pool->poolSize = size;
        pool->used = 0;
    }
    return pool;
}

static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size) {
    if (pool->used + size > pool->poolSize) {
        return NULL; // Pool is out of memory
    }

    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;

    return mem;
}

static void memory_pool_destroy(MemoryPoolVector *pool) {
    if (pool) {
        free(pool->pool);
        free(pool);
    }
}

Vector* vector_create(size_t itemSize) {
    Vector* vec = (Vector*)malloc(sizeof(Vector));

    if (!vec){
        perror("can not allocate memory for vec->pool");
        exit(-1);
    } 
        
    vec->size = 0;
    vec->capacitySize = 32; // Initial capacity
    vec->itemSize = itemSize;
    
    size_t initialPoolSize = 10000;
    vec->pool = memory_pool_create(initialPoolSize);
    if (!vec->pool) {
        free(vec);
        perror("can not allocate memory for vec->pool");
        exit(-1);
    }

    // Instead of malloc, use memory pool for initial allocation
    vec->items = memory_pool_allocate(vec->pool, vec->capacitySize * itemSize);
    if (!vec->items) {
        memory_pool_destroy(vec->pool);
        free(vec);
        perror("can not allocate memory for vec->pool");
        exit(-1);
    }

    return vec;
}

bool vector_is_equal(const Vector* vec1, const Vector* vec2) {
    if (vec1 == NULL || vec2 == NULL || vec1->size != vec2->size) {
        return false;
    }
    return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) == 0;
}

bool vector_is_less(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        return false;
    }

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    return cmp < 0 || (cmp == 0 && vec1->size < vec2->size);
}

bool vector_is_greater(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) { 
        return false;
    }

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    return cmp > 0 || (cmp == 0 && vec1->size > vec2->size);
}

bool vector_is_not_equal(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        return true;
    }
    if (vec1->size != vec2->size) { 
        return true;
    }
    return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) != 0;
}

bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2) {
    return !vector_is_less(vec1, vec2);
}

bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2) {
    return !vector_is_greater(vec1, vec2);
}

bool vector_is_empty(Vector *vec) {
    if (vec == NULL) {
        fprintf(stderr, "Error: Vector is NULL in vector_is_empty_impl.\n");
        return true; // or handle the error as per your application's needs
    }
    return vec->size == 0;
}

void vector_erase(Vector *vec, size_t pos, size_t len) {
    if (vec == NULL || pos >= vec->size || pos + len > vec->size) { 
        return; // Vector is NULL, position is out of bounds, or len is too large
    }

    char *base = (char *)vec->items;
    memmove(base + pos * vec->itemSize, 
            base + (pos + len) * vec->itemSize, 
            (vec->size - pos - len) * vec->itemSize);

    vec->size -= len;
}

void vector_insert(Vector *vec, size_t pos, void *item) {
    if (vec == NULL || pos > vec->size) {
        return;
    }
    
    if (vec->size == vec->capacitySize) {
        // Allocate new space from the memory pool
        size_t newCapacity = vec->capacitySize * 2; // Double the capacity
        void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);

        if (!newItems) {
            return; // Handle allocation failure, maybe by resizing the pool
        }

        // Copy existing items to the new space
        memcpy(newItems, vec->items, pos * vec->itemSize); // Copy elements before insertion position
        memcpy((char *)newItems + (pos + 1) * vec->itemSize, 
               (char *)vec->items + pos * vec->itemSize, 
               (vec->size - pos) * vec->itemSize); // Copy elements after insertion position

        vec->items = newItems;
        vec->capacitySize = newCapacity;
    } 
    else {
        char *base = (char *)vec->items;  // Shift elements to make space for the new element
        memmove(base + (pos + 1) * vec->itemSize, 
                base + pos * vec->itemSize, 
                (vec->size - pos) * vec->itemSize);
    }

    // Insert the new element
    memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
    vec->size++;
}

void vector_reserve(Vector *vec, size_t size) {
    if (vec == NULL || vec->capacitySize >= size) {
        return; // No need to reserve if the capacity is already sufficient
    }

    void *newItems = memory_pool_allocate(vec->pool, size * vec->itemSize); // Allocate new space from the memory pool
    if (!newItems) { 
        return; // Handle allocation failure, maybe by resizing the pool
    }
    if (vec->size > 0) {
        memcpy(newItems, vec->items, vec->size * vec->itemSize); // Copy existing items to the new space if there are any
    }

    vec->items = newItems;
    vec->capacitySize = size;
}

void vector_resize(Vector *vec, size_t size) {
    if (vec == NULL) {
        return; // Vector is NULL
    }
    if (size > vec->capacitySize) { 
        vector_reserve(vec, size); // Resize capacity if new size exceeds current capacity
    }
    if (vec->size < size) { 
        memset((char *)vec->items + vec->size * vec->itemSize, 0, (size - vec->size) * vec->itemSize);  // Initialize new elements to 0 if size is increased
    }
    vec->size = size;
}

void vector_shrink_to_fit(Vector *vec) {
    if (vec == NULL || vec->size == vec->capacitySize) {
        return;
    }   
    if (vec->size == 0) {
        free(vec->items); // Assuming this memory is not part of the pool
        vec->items = NULL;
        vec->capacitySize = 0;

        return;
    }

    void *newItems = memory_pool_allocate(vec->pool, vec->size * vec->itemSize);
    if (newItems == NULL) { 
        return;
    }
    
    memcpy(newItems, vec->items, vec->size * vec->itemSize);
    vec->items = newItems;
    vec->capacitySize = vec->size;
}

void vector_swap(Vector *vec1, Vector *vec2) {
    if (vec1 == NULL || vec2 == NULL) {
        fprintf(stderr, "Error: One or both vectors are NULL in vector_swap_impl.\n");
        return;
    }

    void *tempItems = vec1->items;
    vec1->items = vec2->items;
    vec2->items = tempItems;

    size_t tempSize = vec1->size;
    vec1->size = vec2->size;
    vec2->size = tempSize;

    size_t tempCapacity = vec1->capacitySize;
    vec1->capacitySize = vec2->capacitySize;
    vec2->capacitySize = tempCapacity;

    size_t tempItemSize = vec1->itemSize;
    vec1->itemSize = vec2->itemSize;
    vec2->itemSize = tempItemSize;
}

void vector_assign(Vector *vec, size_t pos, void *item) {
    if (vec == NULL || pos >= vec->size) {
        return;
    }
    memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
}

void vector_emplace(Vector *vec, size_t pos, void *item, size_t itemSize) {
    if (vec == NULL || pos > vec->size || itemSize != vec->itemSize) {
        return;
    }
    if (vec->size == vec->capacitySize) {
        vector_reserve(vec, vec->capacitySize * 2); // Use the modified version
    }

    char *base = (char *)vec->items;
    memmove(base + (pos + 1) * vec->itemSize, 
            base + pos * vec->itemSize, 
            (vec->size - pos) * vec->itemSize);

    memcpy(base + pos * vec->itemSize, item, vec->itemSize);
    vec->size++;
}

void vector_emplace_back(Vector *vec, void *item, size_t itemSize) {
    if (vec == NULL || itemSize != vec->itemSize) {
        return;
    }
    if (vec->size >= vec->capacitySize) {
        vector_reserve(vec, vec->capacitySize * 2); // Use the modified version
    }

    memcpy((char *)vec->items + vec->size * vec->itemSize, item, vec->itemSize);
    vec->size++;
}

void vector_push_back(Vector *vec, void *item) {
    if (vec->size >= vec->capacitySize) {
        size_t newCapacity = vec->capacitySize * 2; // Example growth strategy
        // Allocate new space from the memory pool
        void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);
        if (!newItems) {
            return; // Handle allocation failure, maybe by resizing the pool
        }
        memcpy(newItems, vec->items, vec->size * vec->itemSize); // Copy existing items to the new space
        vec->items = newItems;
        vec->capacitySize = newCapacity;
    }
    // Proceed with adding the new item
    memcpy((char *)vec->items + (vec->size * vec->itemSize), item, vec->itemSize);
    vec->size++;
}

void vector_deallocate(Vector *vec) {
    if (vec == NULL) {
        return;
    }
    
    if (vec->pool != NULL) {
        memory_pool_destroy(vec->pool);
        vec->pool = NULL;
    }

    if (vec->items != NULL) {
        vec->items = NULL;   // The items are part of the pool, so no need to free them separately
    }
    free(vec);
}

void *vector_at(const Vector *vec, size_t pos) {
    if (pos < vec->size) {
        return (char *)vec->items + (pos * vec->itemSize); // Calculate the address of the item at position 'pos'
    }
    else { 
        return NULL; // Position is out of bounds
    }
}

void *vector_rbegin(Vector *vec) {
    if (vec == NULL || vec->size == 0) {
        return NULL;
    }
    return (void *)((char *)vec->items + (vec->size - 1) * vec->itemSize); // Last element
}

void *vector_rend(Vector *vec) {
    if (vec == NULL) {
        return NULL;
    }
    return (void *)((char *)vec->items - vec->itemSize); // One before the first element
}

const void *vector_cbegin(Vector *vec) {
    if (vec == NULL || vec->size == 0) {
        return NULL;
    }    
    return (const void *)vec->items;
}

const void *vector_cend(Vector *vec) {
    if (vec == NULL || vec->size == 0) { 
        return NULL;
    }
    return (const void *)((char *)vec->items + (vec->size * vec->itemSize)); // One past the last element, as a read-only pointer
}

const void *vector_crbegin(Vector *vec) {
    if (vec == NULL || vec->size == 0) {
        return NULL;
    }
    return (const void *)((char *)vec->items + (vec->size - 1) * vec->itemSize); // Last element, as a read-only pointer
}

const void *vector_crend(Vector *vec) {
    if (vec == NULL) {
        return NULL;
    } 
    return (const void *)((char *)vec->items - vec->itemSize); // One before the first element, as a read-only pointer
}

void *vector_begin(Vector *vec) {
    if (vec == NULL || vec->size == 0) {
        return NULL;
    }
    return vec->items; // Pointer to the first element
}

void *vector_end(Vector *vec) {
    if (vec == NULL || vec->size == 0) { 
        return NULL;
    }
    return (char *)vec->items + (vec->size * vec->itemSize); // One past the last element
}

void *vector_pop_back(Vector *vec) {
    if (vec == NULL || vec->size == 0) { 
        return NULL;
    }
    vec->size--;

    return (char *)vec->items + (vec->size * vec->itemSize);
}

void vector_clear(Vector *vec) {
    if (vec == NULL) {
        fprintf(stderr, "Error: Vector is NULL in vector_clear_impl.\n");
        return;
    }

    vec->size = 0;
    // Optionally reduce capacity. Choose an appropriate size for your use case.
    size_t reducedCapacity = 4; // Or some other small size
    if (vec->capacitySize > reducedCapacity) {
        void *newItems = memory_pool_allocate(vec->pool, reducedCapacity * vec->itemSize);
        if (newItems != NULL || reducedCapacity == 0) {
            vec->items = newItems;
            vec->capacitySize = reducedCapacity;
        } 
        else {
            fprintf(stderr, "Cannot reallocate the Vector\n");
        }
    }
}

void *vector_front(Vector *vec) {
    if (vec == NULL || vec->size == 0) { 
        return NULL;
    }
    return vec->items; // The first element is at the beginning of the items array
}

void *vector_back(Vector *vec) {
    if (vec == NULL || vec->size == 0) {
        return NULL;
    }
    return (char *)vec->items + (vec->size - 1) * vec->itemSize; // The last element is at (size - 1) * itemSize offset from the beginning
}

void *vector_data(Vector *vec) {
    if (vec == NULL) { 
        return NULL; 
    }
    return vec->items; // The underlying array
}

size_t vector_size(const Vector *vec) {
    return vec->size;
}

size_t vector_capacity(Vector *vec) {
    return vec->capacitySize;
}

size_t vector_max_size(Vector *vec) {
    return vec->itemSize;
}

