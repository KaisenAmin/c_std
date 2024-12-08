/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Vector
*/

#include <stdlib.h>
#include <string.h>
#include "vector.h"


static MemoryPoolVector *memory_pool_create(size_t size);
static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size);
static void memory_pool_destroy(MemoryPoolVector *pool);

static MemoryPoolVector *memory_pool_create(size_t size) {
    VECTOR_LOG("[memory_pool_create]: Entering with pool size: %zu", size);
    
    if (size == 0) {
        VECTOR_LOG("[memory_pool_create]: Error: Memory pool size cannot be zero.");
        return NULL;
    }

    MemoryPoolVector *pool = (MemoryPoolVector*) malloc(sizeof(MemoryPoolVector));
    if (!pool) {
        VECTOR_LOG("[memory_pool_create]: Error: Failed to allocate memory for MemoryPoolVector structure.");
        return NULL;
    }

    pool->pool = malloc(size);
    if (!pool->pool) {
        VECTOR_LOG("[memory_pool_create]: Error: Failed to allocate memory for memory pool of size %zu.", size);
        free(pool);
        return NULL;
    }

    pool->poolSize = size;
    pool->used = 0;

    VECTOR_LOG("[memory_pool_create]: Successfully created memory pool at %p with size %zu.", (void*)pool, pool->poolSize);
    return pool;
}

static void *memory_pool_allocate(MemoryPoolVector *pool, size_t size) {
    VECTOR_LOG("[memory_pool_allocate]: Entering with pool: %p, size: %zu", (void*)pool, size);
    
    if (!pool) {
        VECTOR_LOG("[memory_pool_allocate]: Error: Memory pool is not initialized.");
        return NULL;
    }
    if (size == 0) {
        VECTOR_LOG("[memory_pool_allocate]: Error: Cannot allocate zero size.");
        return NULL;
    }
    if (pool->used + size > pool->poolSize) {
        VECTOR_LOG("[memory_pool_allocate]: Error: Memory pool out of space. Cannot allocate %zu bytes.", size);
        return NULL; 
    }

    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;

    VECTOR_LOG("[memory_pool_allocate]: Successfully allocated %zu bytes. New pool used size: %zu.", size, pool->used);
    return mem;
}

static void memory_pool_destroy(MemoryPoolVector *pool) {
    VECTOR_LOG("[memory_pool_destroy]: Entering with pool: %p", (void*)pool);
    
    if (!pool) {
        VECTOR_LOG("[memory_pool_destroy]: Error: Attempted to destroy a non-initialized memory pool.");
        return;
    }
    
    free(pool->pool); 
    free(pool); 
    
    VECTOR_LOG("[memory_pool_destroy]: Successfully destroyed the memory pool.");
}

/**
 * @brief This function allocates memory for a new vector, initializes its internal structure, and 
 * sets its initial capacity. The vector is created with a specified item size, which 
 * determines the size of the elements it can store. A memory pool is also created for 
 * efficient allocation of vector items.
 * 
 * @param itemSize The size of the elements that the vector will store. This size must be 
 * provided when creating the vector.
 * 
 * @return A pointer to the newly created Vector instance. If memory allocation fails at any 
 * step, the function will log an error message (if `VECTOR_LOGGING_ENABLE` is defined) 
 * and terminate the program with an error code.
 * 
 * @warning Ensure that the item size is correctly specified. Incorrect item size may lead 
 * to undefined behavior when storing and accessing elements.
 * 
 * @note The initial capacity of the vector is set to 32 elements, and the memory pool is 
 * created with a size of 100,000 bytes.
 */
Vector* vector_create(size_t itemSize) {
    VECTOR_LOG("[vector_create]: Entering with itemSize: %zu", itemSize);
    
    Vector* vec = (Vector*)malloc(sizeof(Vector));
    if (!vec) {
        VECTOR_LOG("[vector_create]: Error: Cannot allocate memory for Vector structure.");
        exit(-1); // Handle allocation failure
    }
    
    vec->size = 0;
    vec->capacitySize = 32; // Initial capacity
    vec->itemSize = itemSize;
    
    size_t initialPoolSize = 100000;
    vec->pool = memory_pool_create(initialPoolSize);
    if (!vec->pool) {
        free(vec);
        VECTOR_LOG("[vector_create]: Error: Cannot allocate memory for Vector pool.");
        exit(-1);
    }

    vec->items = memory_pool_allocate(vec->pool, vec->capacitySize * itemSize);
    if (!vec->items) {
        memory_pool_destroy(vec->pool);
        free(vec);
        VECTOR_LOG("[vector_create]: Error: Cannot allocate memory for Vector items.");
        exit(-1);
    }

    VECTOR_LOG("[vector_create]: Successfully created vector at %p with capacity %zu.", (void*)vec, vec->capacitySize);
    return vec;
}

/**
 * @brief This function compares two vectors to determine if they are equal in size and content. 
 * The comparison is performed element by element, and the function returns `true` if both 
 * vectors have the same size and identical elements, and `false` otherwise.
 * 
 * @param vec1 A pointer to the first Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @return `true` if `vec1` and `vec2` are equal in size and content, `false` if they are not 
 * or if one or both vectors are NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message 
 * is logged to `stderr` if either vector is NULL or if their sizes differ.
 * 
 * @note The comparison checks the sizes of the vectors first. If they are not equal, the function 
 * immediately returns `false`. If the sizes are equal, the function performs a byte-by-byte 
 * comparison of the vector contents.
 */
bool vector_is_equal(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_equal]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (vec1 == NULL || vec2 == NULL) {
        VECTOR_LOG("[vector_is_equal]: Error: One or both vectors are NULL.");
        return false; 
    }
    if (vec1->size != vec2->size) {
        VECTOR_LOG("[vector_is_equal]: Vectors have different sizes. vec1 size: %zu, vec2 size: %zu", vec1->size, vec2->size);
        return false; 
    }

    bool result = memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) == 0;
    VECTOR_LOG("[vector_is_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief This function compares two vectors element by element to determine if the first vector (`vec1`) 
 * is lexicographically less than the second vector (`vec2`). The comparison is based on the 
 * elements stored in the vectors, and the function returns `true` if `vec1` is less than `vec2`, 
 * and `false` otherwise.
 * 
 * @param vec1 A pointer to the first Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @return `true` if `vec1` is lexicographically less than `vec2`, `false` if it is not, or if one 
 * or both vectors are NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged 
 * to `stderr` if either vector is NULL.
 * 
 * @note The comparison first checks the common prefix of both vectors. If they are equal, the 
 * vector with fewer elements is considered to be less.
 */
bool vector_is_less(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_less]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vec1 || !vec2) {
        VECTOR_LOG("[vector_is_less]: Error: One or both vectors are NULL.");
        return false;
    }

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    bool result = cmp < 0 || (cmp == 0 && vec1->size < vec2->size);
    VECTOR_LOG("[vector_is_less]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief This function compares two vectors element by element to determine if the first vector (`vec1`) 
 * is lexicographically greater than the second vector (`vec2`). The comparison is based on the 
 * elements stored in the vectors, and the function returns `true` if `vec1` is greater than `vec2`, 
 * and `false` otherwise.
 * 
 * @param vec1 A pointer to the first Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @return `true` if `vec1` is lexicographically greater than `vec2`, `false` if it is not, or if one 
 * or both vectors are NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged 
 * to `stderr` if either vector is NULL.
 * 
 * @note The comparison first checks the common prefix of both vectors. If they are equal, the 
 * vector with more elements is considered to be greater.
 */
bool vector_is_greater(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_greater]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vec1 || !vec2) {
        VECTOR_LOG("[vector_is_greater]: Error: One or both vectors are NULL.");
        return false;
    }

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    bool result = cmp > 0 || (cmp == 0 && vec1->size > vec2->size);
    VECTOR_LOG("[vector_is_greater]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief This function compares two vectors to determine if they are not equal. It returns `true` 
 * if the vectors differ in size or content, and `false` if they are equal.
 * 
 * @param vec1 A pointer to the first Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @return `true` if `vec1` and `vec2` are not equal, `false` if they are equal. If one or both 
 * vectors are NULL, the function returns `true`. If `VECTOR_LOGGING_ENABLE` is defined, an 
 * error message is logged to `stderr` if either vector is NULL or if their sizes differ.
 * 
 * @note The comparison first checks the sizes of the vectors. If they are not the same, the vectors 
 * are considered not equal. Otherwise, a byte-by-byte comparison of the contents is performed.
 */
bool vector_is_not_equal(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_not_equal]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vec1 || !vec2) {
        VECTOR_LOG("[vector_is_not_equal]: Error: One or both vectors are NULL.");
        return true;
    }
    if (vec1->size != vec2->size) {
        VECTOR_LOG("[vector_is_not_equal]: Vectors have different sizes. vec1 size: %zu, vec2 size: %zu", vec1->size, vec2->size);
        return true;
    }

    bool result = memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) != 0;
    VECTOR_LOG("[vector_is_not_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}


/**
 * @brief This function compares two vectors lexicographically to check if the first vector (`vec1`) 
 * is greater than or equal to the second vector (`vec2`). The comparison is based on the 
 * elements stored in the vectors, and the function returns `true` if `vec1` is greater than 
 * or equal to `vec2`, and `false` otherwise.
 * 
 * @param vec1 A pointer to the first Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @return `true` if `vec1` is greater than or equal to `vec2`, `false` if it is not, or if one 
 * or both vectors are NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged 
 * to `stderr` if either vector is NULL.
 * 
 * @note Ensure that both vectors are properly initialized before calling this function.
 */
bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_greater_or_equal]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vec1 || !vec2) {
        VECTOR_LOG("[vector_is_greater_or_equal]: Error: One or both vectors are NULL.");
        return false;
    }

    bool result = !vector_is_less(vec1, vec2);
    VECTOR_LOG("[vector_is_greater_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief This function compares two vectors lexicographically to check if the first vector (`vec1`) 
 * is less than or equal to the second vector (`vec2`). The comparison is based on the elements 
 * stored in the vectors, and the function returns `true` if `vec1` is less than or equal to 
 * `vec2`, and `false` otherwise.
 * 
 * @param vec1 A pointer to the first Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance in the comparison. The vector must have 
 * been initialized before calling this function.
 * 
 * @return `true` if `vec1` is less than or equal to `vec2`, `false` if it is not, or if one or 
 * both vectors are NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged 
 * to `stderr` if either vector is NULL.
 * 
 * @note Ensure that both vectors are properly initialized before calling this function.
 */
bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_less_or_equal]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vec1 || !vec2) {
        VECTOR_LOG("[vector_is_less_or_equal]: Error: One or both vectors are NULL.");
        return false;
    }

    bool result = !vector_is_greater(vec1, vec2);
    VECTOR_LOG("[vector_is_less_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief This function checks if the vector contains any elements. It returns `true` if the vector 
 * is empty (i.e., its size is 0) and `false` otherwise.
 * 
 * @param vec A pointer to the Vector instance to be checked. The vector must hqave been 
 * initialized before calling this function.
 * 
 * @return `true` if the vector is empty, `false` if it is not or if the vector is NULL. If 
 * `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr` if the vector is NULL.
 * 
 * @note This function is useful for checking whether a vector contains any elements before 
 * performing operations that require the vector to be non-empty.
 */
bool vector_is_empty(Vector *vec) {
    VECTOR_LOG("[vector_is_empty]: Entering with vector: %p", (void*)vec);

    if (vec == NULL) {
        VECTOR_LOG("[vector_is_empty]: Error: Vector is NULL.");
        return true; 
    }

    bool isEmpty = (vec->size == 0);
    VECTOR_LOG("[vector_is_empty]: Vector is %s", isEmpty ? "empty" : "not empty");

    return isEmpty;
}

/**
 * @brief This function removes a specified number of elements starting from a given position in 
 * the vector. The elements that follow the removed range are shifted to close the gap, 
 * effectively reducing the size of the vector.
 * 
 * @param vec A pointer to the Vector instance from which elements should be removed. 
 * The vector must have been initialized before calling this function.
 * 
 * @param pos The starting position of the range to be removed. The position is zero-based, 
 * meaning that 0 refers to the first element, 1 to the second, and so on.
 * 
 * @param len The number of elements to remove from the vector, starting from `pos`.
 * 
 * @return void
 * 
 * @note Ensure that the starting position and length are within the bounds of the vector's 
 * size. If the starting position is out of bounds or the length exceeds the available elements 
 * from the starting position, the function will return without making any changes. If 
 * `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
 */
void vector_erase(Vector *vec, size_t pos, size_t len) {
    VECTOR_LOG("[vector_erase]: Entering with vector: %p, position: %zu, length: %zu", (void*)vec, pos, len);

    if (vec == NULL) {
        VECTOR_LOG("[vector_erase]: Error: Vector is NULL.");
        return;
    }
    if (pos >= vec->size) {
        VECTOR_LOG("[vector_erase]: Error: Position is out of bounds. Vector size: %zu", vec->size);
        return;
    }
    if (pos + len > vec->size) {
        VECTOR_LOG("[vector_erase]: Error: Length exceeds vector size. Vector size: %zu", vec->size);
        return;
    }

    char *base = (char *)vec->items;
    memmove(base + pos * vec->itemSize, base + (pos + len) * vec->itemSize, (vec->size - pos - len) * vec->itemSize);

    vec->size -= len;
    VECTOR_LOG("[vector_erase]: Successfully erased %zu elements from position %zu. New size: %zu", len, pos, vec->size);
}


/**
 * @brief This function inserts a new element into the vector at the specified position. The elements 
 * following the insertion point are shifted to make space for the new element. If necessary, 
 * the vector's capacity is expanded to accommodate the new element.
 * 
 * @param vec A pointer to the Vector instance where the new element should be inserted. 
 * The vector must have been initialized before calling this function.
 * 
 * @param pos The position at which the new element should be inserted. The position is 
 * zero-based, meaning that 0 refers to the first element, 1 to the second, and so on.
 * 
 * @param item A pointer to the item to be inserted into the vector. The item is copied 
 * into the vector's storage.
 * 
 * @return void
 * 
 * @note Ensure that the position is within the bounds of the vector's size to avoid 
 * out-of-bounds errors. If the position is out of bounds or the vector is NULL, the function 
 * returns without making any changes. If `VECTOR_LOGGING_ENABLE` is defined, an error 
 * message is logged to `stderr`.
 */
void vector_insert(Vector *vec, size_t pos, void *item) {
    VECTOR_LOG("[vector_insert]: Entering with vector: %p, position: %zu, item: %p", (void*)vec, pos, (void*)item);

    if (!vec) {
        VECTOR_LOG("[vector_insert]: Error: Vector is NULL.");
        return;
    }
    if (pos > vec->size) {
        VECTOR_LOG("[vector_insert]: Error: Position is out of bounds. Vector size: %zu", vec->size);
        return;
    }

    if (vec->size == vec->capacitySize) {
        VECTOR_LOG("[vector_insert]: Resizing vector to accommodate new element.");

        size_t newCapacity = vec->capacitySize * 2; // Double the capacity
        void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);

        if (!newItems) {
            VECTOR_LOG("[vector_insert]: Error: Failed to allocate memory for resizing.");
            return;
        }

        memcpy(newItems, vec->items, pos * vec->itemSize); // Copy elements before insertion position
        memcpy((char *)newItems + (pos + 1) * vec->itemSize, (char *)vec->items + pos * vec->itemSize, 
                (vec->size - pos) * vec->itemSize); // Copy elements after insertion position

        vec->items = newItems;
        vec->capacitySize = newCapacity;
        VECTOR_LOG("[vector_insert]: Resized vector to new capacity: %zu", newCapacity);
    } 
    else {
        char *base = (char *)vec->items;
        memmove(base + (pos + 1) * vec->itemSize, base + pos * vec->itemSize, (vec->size - pos) * vec->itemSize);
    }

    memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
    vec->size++;
    VECTOR_LOG("[vector_insert]: Inserted new element at position %zu. New size: %zu", pos, vec->size);
}

/**
 * @brief This function ensures that the vector has enough capacity to hold at least the specified 
 * number of elements. If the current capacity is already greater than or equal to the 
 * requested size, no action is taken. If the capacity needs to be increased, memory 
 * is allocated, and the existing elements are copied to the new storage.
 * 
 * @param vec A pointer to the Vector instance whose capacity should be increased. 
 * The vector must have been initialized before calling this function.
 * 
 * @param size The minimum capacity that the vector should have after the operation.
 * 
 * @return `true` if the capacity was successfully increased or if no increase was necessary, 
 * `false` if the operation failed due to memory allocation issues or if the vector is NULL. 
 * If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr` in case 
 * of failure.
 * 
 * @note After calling this function, the vector's capacity will be at least the requested 
 * size. However, the actual size of the vector (i.e., the number of elements it currently 
 * holds) will not change.
 * 
 */
bool vector_reserve(Vector *vec, size_t size) {
    VECTOR_LOG("[vector_reserve]: Entering with vector: %p, size: %zu", (void*)vec, size);

    if (!vec) {
        VECTOR_LOG("[vector_reserve]: Error: Vector is NULL.");
        return false;
    }
    if (vec->capacitySize >= size) {
        VECTOR_LOG("[vector_reserve]: Current capacity (%zu) is already sufficient.", vec->capacitySize);
        return true;
    }

    VECTOR_LOG("[vector_reserve]: Resizing to new capacity: %zu", size);
    void *newItems = memory_pool_allocate(vec->pool, size * vec->itemSize);
    if (!newItems) {
        VECTOR_LOG("[vector_reserve]: Error: Failed to allocate memory.");
        return false;
    }

    if (vec->size > 0) {
        memcpy(newItems, vec->items, vec->size * vec->itemSize);
        VECTOR_LOG("[vector_reserve]: Copied existing items to new memory.");
    }

    vec->items = newItems;
    vec->capacitySize = size;
    VECTOR_LOG("[vector_reserve]: New capacity set to %zu", size);

    return true;
}

/**
 * @brief This function adjusts the size of the vector to the specified value. If the new size 
 * is greater than the current size, the vector is expanded, and the new elements are 
 * initialized to zero. If the new size is smaller than the current size, the vector 
 * is truncated, and the extra elements are removed. If necessary, the function increases 
 * the capacity of the vector to accommodate the new size.
 * 
 * @param vec A pointer to the Vector instance to be resized. The vector must have been 
 * initialized before calling this function.
 * 
 * @param size The new size of the vector, representing the number of elements it should contain.
 * 
 * @return void
 * 
 * @note If the vector is resized to a larger size, new elements will be zero-initialized. 
 * Resizing the vector to a smaller size will result in the loss of the elements beyond 
 * the new size.
 */
void vector_resize(Vector *vec, size_t size) {
    VECTOR_LOG("[vector_resize]: Entering with vector: %p, new size: %zu", (void*)vec, size);

    if (!vec) {
        VECTOR_LOG("[vector_resize]: Error: Vector is NULL.");
        return;
    }

    if (size > vec->capacitySize) {
        VECTOR_LOG("[vector_resize]: Current capacity (%zu) is less than new size. Resizing...", vec->capacitySize);
        vector_reserve(vec, size);
    }

    if (vec->size < size) {
        VECTOR_LOG("[vector_resize]: Expanding vector and zero-initializing new elements.");
        memset((char *)vec->items + vec->size * vec->itemSize, 0, (size - vec->size) * vec->itemSize);
    }

    vec->size = size;
    VECTOR_LOG("[vector_resize]: Vector size set to %zu", size);
}


/**
 * @brief This function resizes the vector's capacity to match its current size, thereby freeing 
 * any unused memory. If the vector's capacity is already equal to its size, the function 
 * does nothing. This operation can help reduce memory usage when the vector's size has 
 * significantly decreased.
 * 
 * @param vec A pointer to the Vector instance whose capacity should be reduced. 
 * The vector must have been initialized before calling this function.
 * 
 * @return void
 * 
 * @note After shrinking, the vector's capacity will be equal to its size. If new elements 
 * are added to the vector, it may need to reallocate memory, which can be a costly operation.
 * 
 * @note If the vector is empty, this function deallocates the memory used by the vector's 
 * items and sets its capacity to zero.
 */
void vector_shrink_to_fit(Vector *vec) {
    VECTOR_LOG("[vector_shrink_to_fit]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_shrink_to_fit]: Error: Vector is NULL.");
        return;
    }

    if (vec->size == vec->capacitySize) {
        VECTOR_LOG("[vector_shrink_to_fit]: No need to shrink. Size (%zu) is equal to capacity.", vec->size);
        return;
    }

    if (vec->size == 0) {
        VECTOR_LOG("[vector_shrink_to_fit]: Vector is empty. Deallocating memory.");
        free(vec->items);
        vec->items = NULL;
        vec->capacitySize = 0;
        return;
    }

    VECTOR_LOG("[vector_shrink_to_fit]: Shrinking vector to fit size: %zu", vec->size);
    void *newItems = memory_pool_allocate(vec->pool, vec->size * vec->itemSize);
    if (!newItems) {
        VECTOR_LOG("[vector_shrink_to_fit]: Error: Failed to allocate memory for shrink.");
        return;
    }

    memcpy(newItems, vec->items, vec->size * vec->itemSize);
    vec->items = newItems;
    vec->capacitySize = vec->size;

    VECTOR_LOG("[vector_shrink_to_fit]: Shrink successful. New capacity: %zu", vec->capacitySize);
}

/**
 * @brief This function exchanges the contents of `vec1` and `vec2`, including their size, capacity, 
 * and item size. This can be useful for efficiently exchanging data between two vectors 
 * without copying the individual elements.
 * 
 * @param vec1 A pointer to the first Vector instance to be swapped. The vector must have 
 * been initialized before calling this function.
 * 
 * @param vec2 A pointer to the second Vector instance to be swapped. The vector must have 
 * been initialized before calling this function.
 * 
 * @return void
 * 
 * @note After the swap, the vectors will have exchanged their contents, and all pointers 
 * to the elements in the original vectors will now reference elements in the swapped vectors.
 */
void vector_swap(Vector *vec1, Vector *vec2) {
    VECTOR_LOG("[vector_swap]: Entering with vec1: %p, vec2: %p", (void*)vec1, (void*)vec2);

    if (!vec1 || !vec2) {
        VECTOR_LOG("[vector_swap]: Error: One or both vectors are NULL.");
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

    VECTOR_LOG("[vector_swap]: Swapped vectors. vec1: %p, vec2: %p", (void*)vec1, (void*)vec2);
}

/**
 * @brief This function replaces the element at the given position in the vector with a new value. 
 * The function ensures that the specified position is within the bounds of the vector's 
 * current size.
 * 
 * @param vec A pointer to the Vector instance where the assignment should occur. 
 * The vector must have been initialized before calling this function.
 * 
 * @param pos The position of the element to be replaced. The position is zero-based, 
 * meaning that 0 refers to the first element, 1 to the second, and so on.
 * 
 * @param item A pointer to the new item that will replace the existing element at the 
 * specified position.
 * 
 * @return void
 * 
 * @note Ensure that the position is within the bounds of the vector's size to avoid 
 * out-of-bounds errors. If the position is out of bounds or the vector is NULL, the 
 * function returns without making any changes. If `VECTOR_LOGGING_ENABLE` is defined, 
 * an error message is logged to `stderr`.
 */
void vector_assign(Vector *vec, size_t pos, void *item) {
    VECTOR_LOG("[vector_assign]: Entering with vector: %p, position: %zu, item: %p", (void*)vec, pos, (void*)item);

    if (!vec) {
        VECTOR_LOG("[vector_assign]: Error: Vector is NULL.");
        return;
    }
    if (pos >= vec->size) {
        VECTOR_LOG("[vector_assign]: Error: Position %zu is out of bounds.", pos);
        return;
    }

    memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
    VECTOR_LOG("[vector_assign]: Assigned new item at position %zu.", pos);
}

/**
 * @brief This function inserts a new element into the vector at the specified position by copying 
 * the given item into the vector's storage. If necessary, the vector's capacity is expanded 
 * to accommodate the new element. Elements after the specified position are shifted to 
 * make room for the new element.
 * 
 * @param vec A pointer to the Vector instance where the new element should be inserted. 
 * The vector must have been initialized before calling this function.
 * 
 * @param pos The position at which the new element should be inserted. The position is 
 * zero-based, meaning that 0 refers to the first element, 1 to the second, and so on. 
 * The position must not exceed the current size of the vector.
 * 
 * @param item A pointer to the item to be inserted into the vector. The item is copied 
 * into the vector's storage.
 * 
 * @param itemSize The size of the item to be inserted. This must match the size of the 
 * elements stored in the vector (`vec->itemSize`).
 * 
 * @return void
 * 
 * @note Ensure that the position is within the bounds of the vector's size and that 
 * the item size matches the vector's element size. If the position is out of bounds or 
 * the item size does not match, the function returns without making any changes. If 
 * `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
 */
void vector_emplace(Vector *vec, size_t pos, void *item, size_t itemSize) {
    VECTOR_LOG("[vector_emplace]: Entering with vector: %p, position: %zu, item: %p, itemSize: %zu", (void*)vec, pos, (void*)item, itemSize);

    if (!vec) {
        VECTOR_LOG("[vector_emplace]: Error: Vector is NULL.");
        return;
    }
    if (pos > vec->size || itemSize != vec->itemSize) {
        VECTOR_LOG("[vector_emplace]: Error: Invalid position or item size.");
        return;
    }
    if (vec->size == vec->capacitySize) {
        VECTOR_LOG("[vector_emplace]: Resizing vector to new capacity.");
        vector_reserve(vec, vec->capacitySize * 2);
    }

    char *base = (char *)vec->items;
    memmove(base + (pos + 1) * vec->itemSize, 
            base + pos * vec->itemSize, 
            (vec->size - pos) * vec->itemSize);

    memcpy(base + pos * vec->itemSize, item, vec->itemSize);
    vec->size++;

    VECTOR_LOG("[vector_emplace]: Inserted new item at position %zu. New size: %zu", pos, vec->size);
}

/**
 * @brief This function adds a new element to the end of the vector by copying the given item 
 * into the vector's storage. It ensures that there is enough capacity in the vector 
 * before adding the new element. If the vector's capacity is exceeded, it attempts 
 * to reserve more space.
 * 
 * @param vec A pointer to the Vector instance to which the element should be added. 
 * The vector must have been initialized before calling this function.
 * 
 * @param item A pointer to the item to be added to the vector. The item is copied 
 * into the vector's storage.
 * 
 * @param itemSize The size of the item to be added. This must match the size of the 
 * elements stored in the vector (`vec->itemSize`).
 * 
 * @return `true` if the element was successfully added to the vector, `false` otherwise. 
 * If the vector is NULL, the item size does not match, or memory allocation fails, 
 * the function returns `false`. If `VECTOR_LOGGING_ENABLE` is defined, an error message 
 * is logged to `stderr`.
 * 
 * @note If the vector needs to be resized to accommodate the new element, the returned 
 * pointers from previous calls to functions like `vector_data()` or `vector_at()` may 
 * become invalid.
 */
bool vector_emplace_back(Vector *vec, void *item, size_t itemSize) {
    VECTOR_LOG("[vector_emplace_back]: Entering with vector: %p, item: %p, itemSize: %zu", (void*)vec, (void*)item, itemSize);

    if (!vec) {
        VECTOR_LOG("[vector_emplace_back]: Error: Vector is NULL.");
        return false; // Indicate failure
    }
    if (itemSize != vec->itemSize) {
        VECTOR_LOG("[vector_emplace_back]: Error: Invalid item size. Expected: %zu, got: %zu", vec->itemSize, itemSize);
        return false; // Indicate failure
    }
    if (vec->size >= vec->capacitySize) {
        VECTOR_LOG("[vector_emplace_back]: Vector capacity exceeded. Resizing...");
        if (!vector_reserve(vec, vec->capacitySize * 2)) {
            VECTOR_LOG("[vector_emplace_back]: Error: Resizing vector failed.");
            return false; // vector_reserve failed, indicate failure
        }
    }

    memcpy((char *)vec->items + vec->size * vec->itemSize, item, vec->itemSize);
    vec->size++;

    VECTOR_LOG("[vector_emplace_back]: Item emplaced successfully. New size: %zu", vec->size);
    return true; // Indicate success
}


/**
 * @brief This function adds a new element to the end of the vector by copying the given item 
 * into the vector's storage. It ensures that there is enough capacity in the vector 
 * before adding the new element. If the vector's capacity is exceeded, it attempts 
 * to allocate more space from the memory pool.
 * 
 * @param vec A pointer to the Vector instance to which the element should be added. 
 * The vector must have been initialized before calling this function.
 * 
 * @param item A pointer to the item to be added to the vector. The item is copied 
 * into the vector's storage.
 * 
 * @return `true` if the element was successfully added to the vector, `false` otherwise. 
 * If the vector is NULL or memory allocation fails, the function returns `false`. If 
 * `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
 * 
 * @note If the vector needs to be resized to accommodate the new element, the returned 
 * pointers from previous calls to functions like `vector_data()` or `vector_at()` may 
 * become invalid.
 */
bool vector_push_back(Vector *vec, const void *item) {
    VECTOR_LOG("[vector_push_back]: Entering with vector: %p, item: %p", (void*)vec, (void*)item);

    if (!vec) {
        VECTOR_LOG("[vector_push_back]: Error: Vector is NULL.");
        return false; // Indicate failure
    }

    if (vec->size >= vec->capacitySize) {
        VECTOR_LOG("[vector_push_back]: Vector capacity exceeded. Resizing...");
        size_t newCapacity = vec->capacitySize * 2; // Example growth strategy
        void *newItems = memory_pool_allocate(vec->pool, newCapacity * vec->itemSize);

        if (!newItems) {
            VECTOR_LOG("[vector_push_back]: Error: Memory allocation failed for new capacity: %zu", newCapacity);
            return false; // Indicate failure
        }

        memcpy(newItems, vec->items, vec->size * vec->itemSize); // Copy existing items
        vec->items = newItems;
        vec->capacitySize = newCapacity;

        VECTOR_LOG("[vector_push_back]: Resized vector successfully. New capacity: %zu", newCapacity);
    }

    memcpy((char *)vec->items + (vec->size * vec->itemSize), item, vec->itemSize);
    vec->size++;

    VECTOR_LOG("[vector_push_back]: Item pushed successfully. New size: %zu", vec->size);
    return true; // Indicate success
}


/**
 * @brief This function releases all the memory allocated for the vector, including the memory pool 
 * and the vector structure itself. After calling this function, the vector pointer should 
 * not be used as it is no longer valid.
 * 
 * @param vec A pointer to the Vector instance to be deallocated. The vector must have been 
 * initialized before calling this function.
 * 
 * @return void
 * 
 * @note Ensure that the vector is not NULL before calling this function. If the vector 
 * is NULL, the function returns immediately without performing any operations. If 
 * `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
 */
void vector_deallocate(Vector *vec) {
    VECTOR_LOG("[vector_deallocate]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_deallocate]: Error: Vector is NULL.");
        return; // Handle the error as per your application's needs
    }

    if (vec->pool != NULL) {
        VECTOR_LOG("[vector_deallocate]: Destroying memory pool at: %p", (void*)vec->pool);
        memory_pool_destroy(vec->pool);
        vec->pool = NULL;
    }

    if (vec->items != NULL) {
        VECTOR_LOG("[vector_deallocate]: Nullifying items pointer: %p", (void*)vec->items);
        vec->items = NULL;  
    }

    VECTOR_LOG("[vector_deallocate]: Freeing vector: %p", (void*)vec);
    free(vec);

    VECTOR_LOG("[vector_deallocate]: Exiting.");
}


/**
 * @brief Retrieves a pointer to the element at the specified position in the vector.
 * 
 * This function provides direct access to the element at the specified position 
 * in the vector. If the position is within bounds, it returns a pointer to the 
 * element. If the position is out of bounds or the vector is NULL, it returns NULL.
 * 
 * @param vec A pointer to the Vector structure from which to retrieve the element. 
 *            The vector must be properly initialized before calling this function.
 * @param pos The position of the element to access within the vector.
 * 
 * @return A pointer to the element at the specified position. If the vector is 
 *         NULL or the position is out of bounds, the function returns NULL.
 * 
 * @note Ensure that the position is less than the vector's size to avoid 
 *       accessing out-of-bounds memory.
 */

void *vector_at(const Vector *vec, size_t pos) {
    VECTOR_LOG("[vector_at]: Entering with vector: %p, position: %zu", (void*)vec, pos);

    if (!vec) {
        VECTOR_LOG("[vector_at]: Error: Vector is NULL.");
        return NULL;
    }
    if (pos < vec->size) {
        void *item = (char *)vec->items + (pos * vec->itemSize);
        VECTOR_LOG("[vector_at]: Returning item at position %zu: %p", pos, item);
        return item;
    } 
    else {
        VECTOR_LOG("[vector_at]: Error: Position %zu is out of bounds.", pos);
        return NULL;
    }
}

/**
 * @brief This function provides access to the last element stored in the vector by returning 
 * a pointer to it. It is useful for reverse iteration over the vector, starting from the 
 * last element. The returned pointer allows you to modify or read the value of the last 
 * element directly.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the last element. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A pointer to the last element in the vector. If the vector is NULL or empty, 
 * the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message 
 * is logged to `stderr`.
 * 
 * @note The returned pointer remains valid as long as the vector is not resized or 
 * deallocated. Modifying the vector may invalidate the pointer.
 */
void *vector_rbegin(Vector *vec) {
    VECTOR_LOG("[vector_rbegin]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_rbegin]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_rbegin]: Error: Vector is empty.");
        return NULL;
    }

    void *rbegin = (char *)vec->items + (vec->size - 1) * vec->itemSize;
    VECTOR_LOG("[vector_rbegin]: Returning reverse begin pointer: %p",(void*)rbegin);

    return rbegin;
}


/**
 * @brief This function provides a pointer to the position just before the first element of the vector. 
 * It is used as a sentinel value for reverse iteration, indicating the end of the reverse 
 * iteration loop. The pointer can be used to check if the reverse iteration has reached the 
 * start of the vector.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the sentinel pointer. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A pointer to one position before the first element in the vector. If the vector 
 * is NULL, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error 
 * message is logged to `stderr`.
 * 
 * @note Ensure that the vector is not NULL before calling this function to avoid receiving 
 * a NULL pointer.
 */
void *vector_rend(Vector *vec) {
    VECTOR_LOG("[vector_rend]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_rend]: Error: Vector is NULL.");
        return NULL;
    }

    void *rend = (char *)vec->items - vec->itemSize;
    VECTOR_LOG("[vector_rend]: Returning reverse end pointer (one before first element): %p", (void*)rend);

    return rend;
}


/**
 * @brief This function provides a read-only pointer to the first element stored in the vector. 
 * It is useful for iterating over the vector in a read-only context, starting from the 
 * first element. The returned pointer cannot be used to modify the contents of the vector.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the first element. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A constant pointer to the first element in the vector. If the vector is NULL 
 * or empty, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an 
 * error message is logged to `stderr`.
 * 
 * @note The returned pointer is intended for read-only access. It remains valid as long 
 * as the vector is not resized or deallocated. Modifying the vector may invalidate the 
 * pointer.
 */
const void *vector_cbegin(Vector *vec) {
    VECTOR_LOG("[vector_cbegin]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_cbegin]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_cbegin]: Error: Vector is empty.");
        return NULL;
    }

    VECTOR_LOG("[vector_cbegin]: Returning constant begin pointer: %p", (const void*)vec->items);
    VECTOR_LOG("[vector_cbegin]: Exiting.");
    return (const void *)vec->items;
}


/**
 * @brief This function provides a read-only pointer to the memory location immediately following 
 * the last element in the vector. It is useful for marking the end of the vector in a 
 * read-only iteration loop. The returned pointer cannot be used to modify the contents 
 * of the vector.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the end pointer. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A constant pointer to one past the last element in the vector. If the vector 
 * is NULL or empty, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, 
 * an error message is logged to `stderr`.
 * 
 * @note The returned pointer is intended for read-only access. It remains valid as long 
 * as the vector is not resized or deallocated. Modifying the vector may invalidate the 
 * pointer.
 */
const void *vector_cend(Vector *vec) {
    VECTOR_LOG("[vector_cend]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_cend]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_cend]: Error: Vector is empty.");
        return NULL;
    }

    const void *end_ptr = (const void *)((char *)vec->items + (vec->size * vec->itemSize));
    VECTOR_LOG("[vector_cend]: Returning constant end pointer: %p", end_ptr);
    VECTOR_LOG("[vector_cend]: Exiting.");

    return end_ptr;
}

/**
 * @brief This function provides a read-only pointer to the last element stored in the vector. 
 * It is useful for reverse iteration over the vector, starting from the last element. 
 * The returned pointer cannot be used to modify the contents of the vector.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the last element. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A constant pointer to the last element in the vector. If the vector is NULL 
 * or empty, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an 
 * error message is logged to `stderr`.
 * 
 * @note Ensure that the vector is not NULL and not empty before calling this function 
 * to avoid receiving a NULL pointer.
 */
const void *vector_crbegin(Vector *vec) {
    VECTOR_LOG("[vector_crbegin]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_crbegin]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_crbegin]: Error: Vector is empty.");
        return NULL;
    }

    const void *rbegin_ptr = (const void *)((char *)vec->items + (vec->size - 1) * vec->itemSize);
    VECTOR_LOG("[vector_crbegin]: Returning constant reverse begin pointer: %p", rbegin_ptr);
    VECTOR_LOG("[vector_crbegin]: Exiting.");

    return rbegin_ptr;
}

/**
 * @brief This function provides a read-only pointer to the position just before the first element 
 * of the vector. It is used as a sentinel value for reverse iteration, indicating the end 
 * of the reverse iteration loop. The returned pointer cannot be used to modify the contents 
 * of the vector.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the sentinel pointer. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A constant pointer to one position before the first element in the vector. If the 
 * vector is NULL, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an 
 * error message is logged to `stderr`.
 * 
 * @warning The pointer returned by this function is not meant to be dereferenced. It is 
 * only used as a marker in reverse iteration loops.
 * 
 * @note Ensure that the vector is not NULL before calling this function to avoid receiving 
 * a NULL pointer.
 */
const void *vector_crend(Vector *vec) {
    VECTOR_LOG("[vector_crend]: Entering with vector: %p", (void*)vec);

    if (vec == NULL) {
        VECTOR_LOG("[vector_crend]: Error: Vector is NULL.");
        return NULL;
    }

    const void *crend_ptr = (const void *)((char *)vec->items - vec->itemSize); // One before the first element
    VECTOR_LOG("[vector_crend]: Returning crend pointer: %p", crend_ptr);
    VECTOR_LOG("[vector_crend]: Exiting.");

    return crend_ptr;
}


/**
 * @brief This function provides access to the first element stored in the vector by returning 
 * a pointer to the beginning of the internal array. This is useful for iterating over the 
 * vector starting from the first element.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the first element. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A pointer to the first element in the vector. If the vector is NULL, the function 
 * returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to 
 * `stderr`.
 * 
 * @warning Ensure that the vector is not NULL before calling this function to avoid receiving 
 * a NULL pointer.
 */
void *vector_begin(Vector *vec) {
    VECTOR_LOG("[vector_begin]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_begin]: Error: Vector is NULL.");
        return NULL;
    }

    VECTOR_LOG("[vector_begin]: Returning begin pointer: %p", (void*)vec->items);
    VECTOR_LOG("[vector_begin]: Exiting.");

    return vec->items;
}


/**
 * @brief This function provides a pointer to the memory location immediately following the 
 * last element in the vector. This is useful for iterating over the vector in a loop 
 * where the end of the vector needs to be marked.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the end pointer. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A pointer to one past the last element in the vector. If the vector is NULL 
 * or empty, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an 
 * error message is logged to `stderr`.
 * 
 * @note The pointer returned by this function is meant for use as a sentinel value in 
 * iteration. It does not point to a valid element and should not be dereferenced.
 */
void *vector_end(Vector *vec) {
    VECTOR_LOG("[vector_end]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_end]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_end]: Error: Vector is empty.");
        return NULL;
    }

    void *end_ptr = (char *)vec->items + (vec->size * vec->itemSize); // One past the last element
    VECTOR_LOG("[vector_end]: Returning end pointer: %p", (void*)end_ptr);

    VECTOR_LOG("[vector_end]: Exiting.");
    return end_ptr;
}


/**
 * @brief This function decreases the size of the vector by one, effectively removing the last 
 * element. It returns a pointer to the removed element, allowing you to access its value 
 * before it is discarded. This is useful when you need to retrieve and remove the last 
 * element of the vector in one operation.
 * 
 * @param vec A pointer to the Vector instance from which the last element should be 
 * removed. The vector must have been initialized before calling this function.
 * 
 * @return A pointer to the last element in the vector before it was removed. If the 
 * vector is NULL or empty, the function returns NULL. If `VECTOR_LOGGING_ENABLE` 
 * is defined, an error message is logged to `stderr`.
 * 
 * @note The pointer returned by this function is only valid as long as the vector 
 * is not resized or deallocated. After the vector is modified, the pointer may become 
 * invalid.
 */
void *vector_pop_back(Vector *vec) {
    VECTOR_LOG("[vector_pop_back]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_pop_back]: Error: Vector is NULL.");
        return NULL; 
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_pop_back]: Error: Vector is empty.");
        return NULL;
    }

    vec->size--;
    void *popped_element = (char *)vec->items + (vec->size * vec->itemSize);
    VECTOR_LOG("[vector_pop_back]: Popped element: %p, new size: %zu", popped_element, vec->size);

    VECTOR_LOG("[vector_pop_back]: Exiting.");
    return popped_element;
}

/**
 * @brief This function resets the vector by setting its size to zero, effectively removing all 
 * elements. Optionally, it can also reduce the capacity of the vector to a smaller size 
 * to free up memory. This is useful when you want to reuse the vector without retaining 
 * the current elements.
 * 
 * @param vec A pointer to the Vector instance that should be cleared. The vector must 
 * have been initialized before calling this function.
 * 
 * @return void
 *  
 * @note Ensure that the vector is not NULL before calling this function. If the 
 * vector is NULL, the function returns immediately without performing any operations. 
 * If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
 */
void vector_clear(Vector *vec) {
    VECTOR_LOG("[vector_clear]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_clear]: Error: Vector is NULL.");
        return; // Handle the error as per your application's needs
    }

    vec->size = 0;
    VECTOR_LOG("[vector_clear]: Vector size set to 0.");

    // Optionally reduce capacity. Choose an appropriate size for your use case.
    size_t reducedCapacity = 4; // Or some other small size
    if (vec->capacitySize > reducedCapacity) {
        VECTOR_LOG("[vector_clear]: Reducing capacity to %zu.", reducedCapacity);
        
        void *newItems = memory_pool_allocate(vec->pool, reducedCapacity * vec->itemSize);
        if (newItems != NULL || reducedCapacity == 0) {
            vec->items = newItems;
            vec->capacitySize = reducedCapacity;
            VECTOR_LOG("[vector_clear]: Capacity reduced, new capacity: %zu.", reducedCapacity);
        } 
        else {
            VECTOR_LOG("[vector_clear]: Error: Cannot reallocate the vector.");
        }
    }

    VECTOR_LOG("[vector_clear]: Exiting.");
}

/**
 * @brief This function provides access to the first element stored in the vector. It returns a pointer 
 * to the first element, allowing you to modify or read the value directly. This is useful when 
 * you need quick access to the first element of the vector.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the first element. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A pointer to the first element in the vector. If the vector is NULL or empty, 
 * the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message 
 * is logged to `stderr`.
 * 
 * @note Ensure that the vector is not empty before calling this function to avoid receiving 
 * a NULL pointer. If the vector is empty, consider checking the size of the vector with 
 * `vector_size()` before calling `vector_front()`.
 */
void *vector_front(Vector *vec) {
    VECTOR_LOG("[vector_front]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_front]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_front]: Error: Vector is empty.");
        return NULL;
    }

    VECTOR_LOG("[vector_front]: Returning front element: %p", (void*)(vec->items));
    return vec->items; 
}

/**
 * @brief This function provides access to the last element stored in the vector. It returns a pointer 
 * to the last element, allowing you to modify or read the value directly. This function is 
 * useful when you need quick access to the most recently added element.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the last element. 
 * The vector must have been initialized before calling this function.
 * 
 * @return A pointer to the last element in the vector. If the vector is NULL or empty, 
 * the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error message 
 * is logged to `stderr`.
 *
 * @note Ensure that the vector is not empty before calling this function to avoid receiving 
 * a NULL pointer.
 */
void *vector_back(Vector *vec) {
    VECTOR_LOG("[vector_back]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_back]: Error: Vector is NULL.");
        return NULL;
    }
    if (vec->size == 0) {
        VECTOR_LOG("[vector_back]: Error: Vector is empty.");
        return NULL;
    }

    void *back_element = (char *)vec->items + (vec->size - 1) * vec->itemSize;
    VECTOR_LOG("[vector_back]: Returning back element: %p", (void*)back_element);

    return back_element; 
}

/**
 * @brief This function provides direct access to the internal data of the vector. It returns a pointer 
 * to the beginning of the array where the elements are stored. This can be useful when 
 * interacting with APIs or functions that require a standard C array.
 * 
 * @param vec A pointer to the Vector instance from which to retrieve the data. The vector 
 * must have been initialized before calling this function.
 * 
 * @return A pointer to the underlying array of the vector's elements. If the vector is 
 * NULL, the function returns NULL. If `VECTOR_LOGGING_ENABLE` is defined, an error 
 * message is logged to `stderr`.
 * 
 * @note The returned pointer is valid until the vector is resized or deallocated. If the 
 * vector is resized, reallocated, or deallocated, the pointer may become invalid.
 */
void *vector_data(Vector *vec) {
    VECTOR_LOG("[vector_data]: Entering with vector: %p", (void*)vec);
    
    if (!vec) {
        VECTOR_LOG("[vector_data]: Error: Vector is NULL.");
        return NULL;
    }

    VECTOR_LOG("[vector_data]: Returning underlying data pointer: %p", (void*)(vec->items));
    return vec->items;
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
    VECTOR_LOG("[vector_size]: Entering with vector: %p", (void*)vec);
    
    if (!vec) {
        VECTOR_LOG("[vector_size]: Error: Vector is NULL.");
        return 0; 
    }

    VECTOR_LOG("[vector_size]: Vector size: %zu", vec->size);
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
    VECTOR_LOG("[vector_capacity]: Entering with vector: %p", (void*)vec);
    
    if (!vec) {
        VECTOR_LOG("[vector_capacity]: Error: Vector is NULL.");
        return 0;
    }
    
    VECTOR_LOG("[vector_capacity]: Vector capacity: %zu", vec->capacitySize);
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
    VECTOR_LOG("[vector_max_size]: Entering vector_max_size with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_max_size]: Error: Vector is NULL in vector_max_size.");
        return 0; 
    }

    VECTOR_LOG("[vector_max_size]: Vector itemSize is %zu", vec->itemSize);
    VECTOR_LOG("[vector_max_size]: Exiting vector_max_size, returning itemSize: %zu", vec->itemSize);

    return vec->itemSize;
}

