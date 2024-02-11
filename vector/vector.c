/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Vector
*/

#include "vector.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

static MemoryPoolVector *memory_pool_create(size_t size);
static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size);
static void memory_pool_destroy(MemoryPoolVector *pool);

static MemoryPoolVector *memory_pool_create(size_t size) {
    if (size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory pool size cannot be zero.\n");
        #endif
        return NULL;
    }

    MemoryPoolVector *pool = malloc(sizeof(MemoryPoolVector));
    if (!pool) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to allocate memory for MemoryPoolVector structure.\n");
        #endif 
        return NULL;
    }

    pool->pool = malloc(size);
    if (!pool->pool) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to allocate memory for memory pool of size %zu.\n", size);
        #endif 
        free(pool);
        return NULL;
    }

    pool->poolSize = size;
    pool->used = 0;

    return pool;
}

static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size) {
    if (!pool) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory pool is not initialized.\n");
        #endif 
        return NULL;
    }
    if (size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Cannot allocate zero size.\n");
        #endif
        return NULL;
    }
    if (pool->used + size > pool->poolSize) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory pool out of space. Cannot allocate %zu bytes.\n", size);
        #endif
        return NULL; // Pool is out of memory
    }

    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;

    return mem;
}

static void memory_pool_destroy(MemoryPoolVector *pool) {
    if (!pool) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Attempted to destroy a non-initialized memory pool.\n");
        #endif
        return;
    }
    free(pool->pool); // Free the allocated pool memory
    free(pool); // Free the pool structure itself
}

Vector* vector_create(size_t itemSize) {
    Vector* vec = (Vector*)malloc(sizeof(Vector));

    if (!vec){
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Can not allocate memory for Vector structure");
        #endif 
        exit(-1);
    } 
        
    vec->size = 0;
    vec->capacitySize = 32; // Initial capacity
    vec->itemSize = itemSize;
    
    size_t initialPoolSize = 100000;
    vec->pool = memory_pool_create(initialPoolSize);
    if (!vec->pool) {
        free(vec);
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Can not allocate memory for Vector pool");
        #endif 
        exit(-1);
    }

    // Instead of malloc, use memory pool for initial allocation
    vec->items = memory_pool_allocate(vec->pool, vec->capacitySize * itemSize);
    if (!vec->items) {
        memory_pool_destroy(vec->pool);
        free(vec);
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Can not allocate memory for Vector items");
        #endif
        exit(-1);
    }
    return vec;
}

bool vector_is_equal(const Vector* vec1, const Vector* vec2) {
    if (vec1 == NULL || vec2 == NULL) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vector pointers are NULL in vector_is_equal.\n");
        #endif
        return false; // One or both vectors are NULL, so they cannot be equal
    }
    if (vec1->size != vec2->size) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vectors have different sizes in vector_is_equal.\n");
        #endif
        return false; // Vectors with different sizes cannot be equal
    }
    return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) == 0;
}

bool vector_is_less(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vector pointers are NULL in vector_is_less.\n");
        #endif
        return false;
    }

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    return cmp < 0 || (cmp == 0 && vec1->size < vec2->size);
}

bool vector_is_greater(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vector pointers are NULL in vector_is_greater.\n");
        #endif 
        return false;
    }

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    return cmp > 0 || (cmp == 0 && vec1->size > vec2->size);
}

bool vector_is_not_equal(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vector pointers are NULL in vector_is_not_equal.\n");
        #endif 
        return true;
    }
    if (vec1->size != vec2->size) { 
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vectors have different sizes in vector_is_not_equal.\n");
        #endif 
        return true;
    }
    return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) != 0;
}

bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vector pointers are NULL in vector_is_greater_or_equal.\n");
        #endif 
        return false; // Handle the error as per your application's needs
    }
    return !vector_is_less(vec1, vec2);
}

bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2) {
    if (!vec1 || !vec2) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vector pointers are NULL in vector_is_less_or_equal.\n");
        #endif 
        return false; // Handle the error as per your application's needs
    }
    return !vector_is_greater(vec1, vec2);
}

bool vector_is_empty(Vector *vec) {
    if (vec == NULL) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_is_empty_impl.\n");
        #endif 
        return true; // or handle the error as per your application's needs
    }
    return vec->size == 0;
}

void vector_erase(Vector *vec, size_t pos, size_t len) {
    if (vec == NULL) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_erase.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }

    if (pos >= vec->size) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Position is out of bounds in vector_erase.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }

    if (pos + len > vec->size) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Length is too large in vector_erase.\n");
        #endif
        return; // Handle the error as per your application's needs
    }
    char *base = (char *)vec->items;
    memmove(base + pos * vec->itemSize, 
            base + (pos + len) * vec->itemSize, 
            (vec->size - pos - len) * vec->itemSize);

    vec->size -= len;
}

void vector_insert(Vector *vec, size_t pos, void *item) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_insert.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    if (pos > vec->size) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Position is out of bounds in vector_insert.\n");
        #endif
        return; // Handle the error as per your application's needs
    }
    
    if (vec->size == vec->capacitySize) {
        // Allocate new space from the memory pool
        size_t newCapacity = vec->capacitySize * 2; // Double the capacity
        void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);

        if (!newItems) {
            #ifdef VECTOR_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Failed to allocate memory for vector_insert.\n");
            #endif 
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

bool vector_reserve(Vector *vec, size_t size) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_reserve.\n");
        #endif 
        return false; 
    }
    if (vec->capacitySize >= size) {
        return true; 
    }

    void *newItems = memory_pool_allocate(vec->pool, size * vec->itemSize);
    if (!newItems) {
        #ifdef VECTOR_LOGGING_ENABLE 
            fmt_fprintf(stderr, "Error: Failed to allocate memory for vector_reserve.\n");
        #endif 
        return false;
    }
    if (vec->size > 0) {
        memcpy(newItems, vec->items, vec->size * vec->itemSize);
    }

    vec->items = newItems;
    vec->capacitySize = size;
    return true;
}

void vector_resize(Vector *vec, size_t size) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_resize.\n");
        #endif 
        return; // Handle the error as per your application's needs
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
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_shrink_to_fit.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    if (vec->size == vec->capacitySize) {
        return; // No need to shrink if size equals capacity
    }

    if (vec->size == 0) {
        free(vec->items); // Assuming this memory is not part of the pool
        vec->items = NULL;
        vec->capacitySize = 0;

        return;
    }

    void *newItems = memory_pool_allocate(vec->pool, vec->size * vec->itemSize);
    if (!newItems) {
        #ifdef VECTOR_LOGGING_ENABLE 
            fmt_fprintf(stderr, "Error: Failed to allocate memory for vector_shrink_to_fit.\n");
        #endif 
        return; // Handle allocation failure, maybe by resizing the pool or another appropriate action
    }
    
    memcpy(newItems, vec->items, vec->size * vec->itemSize);
    vec->items = newItems;
    vec->capacitySize = vec->size;
}

void vector_swap(Vector *vec1, Vector *vec2) {
    if (!vec1 || !vec2) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both vectors are NULL in vector_swap.\n");
        #endif 
        return; // Handle the error as per your application's needs
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
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_assign.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    if (pos >= vec->size) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Position is out of bounds in vector_assign.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
}

void vector_emplace(Vector *vec, size_t pos, void *item, size_t itemSize) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_emplace.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    if (pos > vec->size || itemSize != vec->itemSize) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid position or item size in vector_emplace.\n");
        #endif 
        return; // Handle the error as per your application's needs
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

bool vector_emplace_back(Vector *vec, void *item, size_t itemSize) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_emplace_back.\n");
        #endif 
        return false; // Indicate failure
    }
    if (itemSize != vec->itemSize) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid item size in vector_emplace_back.\n");
        #endif 
        return false; // Indicate failure
    }
    if (vec->size >= vec->capacitySize) {
        if (!vector_reserve(vec, vec->capacitySize * 2)) {
            return false; // vector_reserve failed, indicate failure
        }
    }

    memcpy((char *)vec->items + vec->size * vec->itemSize, item, vec->itemSize);
    vec->size++;
    return true; // Indicate success
}

bool vector_push_back(Vector *vec, const void *item) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_push_back.\n");
        #endif 
        return false; // Indicate failure
    }

    if (vec->size >= vec->capacitySize) {
        size_t newCapacity = vec->capacitySize * 2; // Example growth strategy
        // Allocate new space from the memory pool
        void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);
        if (!newItems) {
            #ifdef VECTOR_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Failed to allocate memory in vector_push_back.\n");
            #endif 
            return false; // Indicate failure
        }

        memcpy(newItems, vec->items, vec->size * vec->itemSize); // Copy existing items to the new space
        vec->items = newItems;
        vec->capacitySize = newCapacity;
    }

    // Proceed with adding the new item
    memcpy((char *)vec->items + (vec->size * vec->itemSize), item, vec->itemSize);
    vec->size++;
    return true; // Indicate success
}

void vector_deallocate(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_deallocate.\n");
        #endif 
        return; // Handle the error as per your application's needs
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
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_at.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }

    if (pos < vec->size) {
        return (char *)vec->items + (pos * vec->itemSize); // Calculate the address of the item at position 'pos'
    }
    else {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Position is out of bounds in vector_at.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
}

void *vector_rbegin(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_rbegin.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_rbegin.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }

    return (void *)((char *)vec->items + (vec->size - 1) * vec->itemSize); // Last element
}

void *vector_rend(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_rend.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return (void *)((char *)vec->items - vec->itemSize); // One before the first element
}

const void *vector_cbegin(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_cbegin.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_cbegin.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return (const void *)vec->items;
}

const void *vector_cend(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_cend.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_end.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return (const void *)((char *)vec->items + (vec->size * vec->itemSize)); // One past the last element, as a read-only pointer
}

const void *vector_crbegin(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_crbegin.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_crbegin.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return (const void *)((char *)vec->items + (vec->size - 1) * vec->itemSize); // Last element, as a read-only pointer
}

const void *vector_crend(Vector *vec) {
    if (vec == NULL) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_crend.\n");
        #endif 
        return NULL;
    } 
    return (const void *)((char *)vec->items - vec->itemSize); // One before the first element, as a read-only pointer
}

void *vector_begin(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_crend.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return vec->items; // Pointer to the first element
}

void *vector_end(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_end.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_end.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return (char *)vec->items + (vec->size * vec->itemSize); // One past the last element
}

void *vector_pop_back(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_pop_back.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_pop_back.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }

    vec->size--;
    return (char *)vec->items + (vec->size * vec->itemSize);
}

void vector_clear(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_clear.\n");
        #endif 
        return; // Handle the error as per your application's needs
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
            #ifdef VECTOR_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Cannot reallocate the Vector in vector_clear.\n");
            #endif 
        }
    }
}

void *vector_front(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_front.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_front.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return vec->items; // The first element is at the beginning of the items array
}

void *vector_back(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_back.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    if (vec->size == 0) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is empty in vector_back.\n");
        #endif 
        return NULL; // Handle the error as per your application's needs
    }
    return (char *)vec->items + (vec->size - 1) * vec->itemSize; // The last element is at (size - 1) * itemSize offset from the beginning
}

void *vector_data(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_data.\n");
        #endif 
        return NULL;
    }
    return vec->items; // The underlying array
}

/**
 * @brief Returns the number of elements currently stored in the vector.
 * 
 * This function provides the current size of the vector, indicating how many elements are currently stored in it. 
 * The size can be used to determine the number of elements that are actually accessible within the vector. 
 * It is an essential attribute for iterating over the vector's elements or when making decisions based on the vector's 
 * occupancy.
 * 
 * @param vec A pointer to a constant Vector instance to query for its size. The vector should have been properly initialized 
 * before invoking this function.
 * @return The current number of elements in the vector as a `size_t` value. If the vector pointer is NULL, 
 * the function returns 0. In case `VECTOR_LOGGING_ENABLE` is defined, it logs an error message to `stderr` to indicate 
 * that the input vector pointer is NULL.
 * 
 * @note Error Handling: The function checks if the input vector pointer is NULL. If so, and `VECTOR_LOGGING_ENABLE` is defined,
 * it outputs an error message to `stderr`. This approach aids in identifying issues during development, 
 * but the specific error handling strategy might need to be adapted to fit the application's requirements.
 */
size_t vector_size(const Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_size.\n");
        #endif 
        return 0; 
    }
    return vec->size;
}

/**
 * @brief Retrieves the current capacity of the vector.
 * 
 * This function returns the current capacity of the vector, which is the number of elements it can hold before needing to 
 * allocate more memory. It is an important function for understanding the memory efficiency and performance characteristics 
 * of the vector, especially in scenarios where minimizing reallocations is critical.
 * 
 * @param vec A pointer to the Vector instance whose capacity is queried. The vector must be properly initialized before 
 * calling this function.
 * @return The capacity of the vector as a `size_t` value, representing the maximum number of elements it can currently 
 * hold without reallocating memory. If the vector is NULL, the function returns 0, and if `VECTOR_LOGGING_ENABLE` is defined, 
 * an error message is logged to `stderr`.
 * 
 * @note The function checks if the input vector pointer is NULL. If it is, and if `VECTOR_LOGGING_ENABLE` is defined, 
 * an error message is printed to `stderr`. This aids in debugging but may need to be adjusted or enhanced based on specific 
 * requirements for error reporting in your application.
 */
size_t vector_capacity(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_capacity.\n");
        #endif 
        return 0; // Handle the error as per your application's needs
    }
    return vec->capacitySize;
}

/**
 * @brief Calculates the maximum number of elements that the vector can hold based on its item size.
 * 
 * This function provides the maximum size that a vector can achieve, which is determined by the size of the items it holds. 
 * It is crucial to note that this function returns the item size, which might not accurately represent the 'maximum size' 
 * in terms of the number of elements. Typically, the maximum size would depend on the total amount of memory available and 
 * the size of each element, so the implementation might need a revision to reflect the true maximum capacity of the vector.
 * 
 * @param vec A pointer to the Vector instance whose maximum size is to be determined.
 * @return The size of the items in the vector as `size_t`. 
 * This is indicative of the item size rather than the maximum number of items the vector can hold. If the vector is NULL, 
 * the function returns 0, and an error message is logged to `stderr` if `VECTOR_LOGGING_ENABLE` is defined.
 * 
 * @note This function checks if the provided vector pointer is NULL. If so, and if `VECTOR_LOGGING_ENABLE` is defined, 
 * it logs an error message to `stderr`. This behavior is useful for debugging but might need adjustment based on specific 
 * application requirements for error handling.
 */
size_t vector_max_size(Vector *vec) {
    if (!vec) {
        #ifdef VECTOR_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in vector_max_size.\n");
        #endif 
        return 0; // Handle the error as per your application's needs
    }
    return vec->itemSize;
}

