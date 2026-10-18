/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Vector
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "vector.h"


static bool vecbuf_realloc_to(Vector *vec, size_t newCapacity) {
    if (newCapacity == 0) {
        VECTOR_LOG("[vecbuf_realloc_to]: Releasing backing buffer (capacity 0).");
        free(vec->items);
        vec->items = NULL;
        vec->capacitySize = 0;
        return true;
    }
    if (newCapacity > ((size_t)-1) / vec->itemSize) {
        VECTOR_LOG("[vecbuf_realloc_to]: Error: capacity %zu * itemSize %zu overflows size_t.", newCapacity, vec->itemSize);
        return false;
    }

    void *newItems = realloc(vec->items, newCapacity * vec->itemSize);
    if (!newItems) {
        VECTOR_LOG("[vecbuf_realloc_to]: Error: realloc to %zu elements failed.", newCapacity);
        return false;
    }

    vec->items = newItems;
    vec->capacitySize = newCapacity;
    VECTOR_LOG("[vecbuf_realloc_to]: Buffer now holds %zu elements (%zu bytes).", newCapacity, newCapacity * vec->itemSize);
    return true;
}


/* Doubling growth policy, overflow-safe. Returns the next capacity that is at
 * least `minNeeded`; falls back to 8 from empty and clamps to the largest
 * addressable element count if doubling would wrap size_t. */
static size_t vecbuf_next_capacity(const Vector *vec, size_t minNeeded) {
    size_t cap  = vec->capacitySize;
    size_t next = cap ? cap * 2 : 8;

    if (cap && next / 2 != cap) {            /* size_t wrap on cap * 2 */
        next = ((size_t)-1) / vec->itemSize;
    }
    if (next < minNeeded) {
        next = minNeeded;
    }
    return next;
}


static bool vecbuf_ensure_one_more(Vector *vec) {
    if (vec->size < vec->capacitySize) {
        return true;
    }
    return vecbuf_realloc_to(vec, vecbuf_next_capacity(vec, vec->size + 1));
}

/* Byte offset of `item` inside the vector's buffer, or SIZE_MAX when it points
 * elsewhere. Lets the growth paths keep v.push_back(v[i]) / v.insert(p, v[i])
 * valid (as std::vector does) even though the realloc may move or free the
 * old buffer before the element is copied. */
static size_t vecbuf_alias_offset(const Vector *vec, const void *item) {
    uintptr_t p = (uintptr_t)item;
    uintptr_t b = (uintptr_t)vec->items;
    if (vec->items && p >= b && p < b + vec->capacitySize * vec->itemSize) {
        return (size_t)(p - b);
    }
    return (size_t)-1;
}


/**
 * @brief This function allocates memory for a new vector, initializes its internal structure, and
 * sets its initial capacity. The vector is created with a specified item size, which
 * determines the size of the elements it can store. Elements live in a single contiguous
 * block that is grown on demand with realloc (the std::vector storage model).
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
 * @note The initial capacity of the vector is set to 32 elements. Storage is a single
 * contiguous block grown by realloc (doubling), just like std::vector; there is no
 * separate arena, so an empty vector now costs one small allocation instead of 100 KB.
 */
Vector* vector_create(size_t itemSize) {
    VECTOR_LOG("[vector_create]: Entering with itemSize: %zu", itemSize);

    if (itemSize == 0) {
        VECTOR_LOG("[vector_create]: Error: itemSize must be greater than 0.");
        return NULL;
    }

    /* Preserve the documented initial capacity of 32 elements: small vectors
       then never reallocate (this is why many small vectors are ~2x faster
       than std::vector, which grows 1, 2, 4, 8, 16). */
    Vector* vec = vector_create_with_capacity(itemSize, 32);
    if (!vec) {
        VECTOR_LOG("[vector_create]: Error: Cannot allocate memory for the vector.");
        return NULL;
    }

    VECTOR_LOG("[vector_create]: Successfully created vector at %p with capacity %zu.", (void*)vec, vec->capacitySize);
    return vec;
}


/**
 * @brief Sets the element comparator used by vector_is_equal / is_less / ... .
 *
 * The comparisons are lexicographic over the elements. Without a comparator
 * each element's bytes are compared with memcmp, which orders int, negative
 * numbers and doubles incorrectly ({256} < {1} on little-endian); set e.g. an
 * int comparator to compare values. With a comparator, two elements are
 * equal when it returns 0. When only one of two vectors has a comparator,
 * that one is used.
 *
 * @param vec     The vector (NULL is a no-op).
 * @param compare qsort-style comparator, or NULL for byte comparison.
 */
void vector_set_compare(Vector* vec, VectorCompareFunc compare) {
    if (vec) {
        vec->compare = compare;
    }
}


/* Lexicographic three-way comparison of two vectors with the same itemSize. */
static int vecbuf_lex_compare(const Vector* vec1, const Vector* vec2) {
    VectorCompareFunc cmp = vec1->compare ? vec1->compare : vec2->compare;
    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    const char* a = (const char*)vec1->items;
    const char* b = (const char*)vec2->items;

    for (size_t i = 0; i < minSize; ++i) {
        int c = cmp ? cmp(a + i * vec1->itemSize, b + i * vec1->itemSize)
                    : memcmp(a + i * vec1->itemSize, b + i * vec1->itemSize, vec1->itemSize);
        if (c != 0) {
            return (c < 0) ? -1 : 1;
        }
    }
    if (vec1->size != vec2->size) {
        return (vec1->size < vec2->size) ? -1 : 1;
    }
    return 0;
}

/* Vectors of different element sizes are unordered (and never equal); the
   old code compared vec2 with vec1's itemSize and could read past its end. */
static bool vecbuf_comparable(const Vector* vec1, const Vector* vec2) {
    return vec1 && vec2 && vec1->itemSize == vec2->itemSize;
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
 * immediately returns `false`. If the sizes are equal, the elements are compared with the
 * comparator set by vector_set_compare, or byte by byte when there is none (then 0.0 and
 * -0.0, or structs that differ only in padding, are not equal).
 */
bool vector_is_equal(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_equal]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vecbuf_comparable(vec1, vec2)) {
        VECTOR_LOG("[vector_is_equal]: Error: One or both vectors are NULL or the element sizes differ.");
        return false;
    }
    if (vec1->size != vec2->size) {
        VECTOR_LOG("[vector_is_equal]: Vectors have different sizes. vec1 size: %zu, vec2 size: %zu", vec1->size, vec2->size);
        return false;
    }

    bool result;
    if (vec1->compare || vec2->compare) {
        result = vecbuf_lex_compare(vec1, vec2) == 0;
    }
    else {
        /* memcmp must not see the NULL buffer of a shrunk empty vector */
        result = vec1->size == 0 || memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) == 0;
    }
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
 * vector with fewer elements is considered to be less. Elements are ordered by the comparator
 * set with vector_set_compare; without one, by their bytes (memcmp), which is not the numeric
 * order of int / double elements.
 */
bool vector_is_less(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_less]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vecbuf_comparable(vec1, vec2)) {
        VECTOR_LOG("[vector_is_less]: Error: One or both vectors are NULL or the element sizes differ.");
        return false;
    }

    /* element by element (with the comparator if one is set); a single
       memcmp over the buffers ordered {256} before {1} and {-1} after {0} */
    bool result = vecbuf_lex_compare(vec1, vec2) < 0;
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
 * vector with more elements is considered to be greater. Elements are ordered as in
 * vector_is_less (comparator, or bytes without one).
 */
bool vector_is_greater(const Vector* vec1, const Vector* vec2) {
    VECTOR_LOG("[vector_is_greater]: Entering with vector1: %p, vector2: %p", (void*)vec1, (void*)vec2);

    if (!vecbuf_comparable(vec1, vec2)) {
        VECTOR_LOG("[vector_is_greater]: Error: One or both vectors are NULL or the element sizes differ.");
        return false;
    }

    bool result = vecbuf_lex_compare(vec1, vec2) > 0;
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

    bool result = !vector_is_equal(vec1, vec2);
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

    if (!vecbuf_comparable(vec1, vec2)) {
        VECTOR_LOG("[vector_is_greater_or_equal]: Error: One or both vectors are NULL or the element sizes differ.");
        return false;
    }

    bool result = vecbuf_lex_compare(vec1, vec2) >= 0;
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

    if (!vecbuf_comparable(vec1, vec2)) {
        VECTOR_LOG("[vector_is_less_or_equal]: Error: One or both vectors are NULL or the element sizes differ.");
        return false;
    }

    bool result = vecbuf_lex_compare(vec1, vec2) <= 0;
    VECTOR_LOG("[vector_is_less_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
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
    if (len > vec->size - pos) {   /* overflow-safe form of pos + len > size */
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

    if (!vec || !item) {
        VECTOR_LOG("[vector_insert]: Error: Vector or item is NULL.");
        return;
    }
    if (pos > vec->size) {
        VECTOR_LOG("[vector_insert]: Error: Position is out of bounds. Vector size: %zu", vec->size);
        return;
    }

    size_t alias = vecbuf_alias_offset(vec, item);
    if (vec->size >= vec->capacitySize) {
        VECTOR_LOG("[vector_insert]: Resizing vector to accommodate new element.");
        if (!vecbuf_ensure_one_more(vec)) {
            VECTOR_LOG("[vector_insert]: Error: Failed to allocate memory for resizing.");
            return;
        }
    }

    /* realloc preserved elements [0, size) in place; open a one-slot gap at
     * `pos` by shifting the tail right, then drop the new element in. */
    char *base = (char *)vec->items;
    memmove(base + (pos + 1) * vec->itemSize, base + pos * vec->itemSize, (vec->size - pos) * vec->itemSize);
    const void *src = item;
    if (alias != (size_t)-1) {
        /* item lives in this vector: follow it through the realloc and the shift */
        if (alias >= pos * vec->itemSize) {
            alias += vec->itemSize;
        }
        src = base + alias;
    }
    vecbuf_store(base + pos * vec->itemSize, src, vec->itemSize);
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
    /* Grow to EXACTLY the requested capacity (reserve never over-allocates and
     * never shrinks) */
    if (!vecbuf_realloc_to(vec, size)) {
        VECTOR_LOG("[vector_reserve]: Error: Failed to allocate memory.");
        return false;
    }

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
 * @return `true` on success, `false` if the vector is NULL or memory could not be allocated
 * (the vector is then unchanged).
 *
 * @note If the vector is resized to a larger size, new elements will be zero-initialized.
 * Resizing the vector to a smaller size will result in the loss of the elements beyond
 * the new size. Growing uses the same doubling policy as push_back, so a loop of
 * resize(size + 1) is amortized O(1) (it used to reallocate on every call).
 */
bool vector_resize(Vector *vec, size_t size) {
    VECTOR_LOG("[vector_resize]: Entering with vector: %p, new size: %zu", (void*)vec, size);

    if (!vec) {
        VECTOR_LOG("[vector_resize]: Error: Vector is NULL.");
        return false;
    }

    if (size > vec->capacitySize && vec->size == 0) {
        /* Nothing to keep: a zeroed buffer straight from calloc. For a large
           size the system hands out pages that are already zero, so there is
           no memset pass over the whole buffer (std::vector writes every
           element). Exactly `size` elements, like vector_resize from empty
           has always ended up with after the first growth. */
        if (size > ((size_t)-1) / vec->itemSize) {
            return false;
        }
        void *fresh = calloc(size, vec->itemSize);
        if (!fresh) {
            VECTOR_LOG("[vector_resize]: Error: allocation failed; aborting resize.");
            return false;
        }
        free(vec->items);
        vec->items = fresh;
        vec->capacitySize = size;
        vec->size = size;
        return true;
    }
    if (size > vec->capacitySize) {
        VECTOR_LOG("[vector_resize]: Current capacity (%zu) is less than new size. Resizing...", vec->capacitySize);
        if (!vecbuf_realloc_to(vec, vecbuf_next_capacity(vec, size)) && !vecbuf_realloc_to(vec, size)) {
            VECTOR_LOG("[vector_resize]: Error: allocation failed; aborting resize.");
            return false;
        }
    }

    if (vec->size < size) {
        VECTOR_LOG("[vector_resize]: Expanding vector and zero-initializing new elements.");
        memset((char *)vec->items + vec->size * vec->itemSize, 0, (size - vec->size) * vec->itemSize);
    }

    vec->size = size;
    VECTOR_LOG("[vector_resize]: Vector size set to %zu", size);
    return true;
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

    /* size == 0 releases the buffer entirely (items -> NULL, capacity -> 0);
     * otherwise realloc down to exactly `size` elements. */
    VECTOR_LOG("[vector_shrink_to_fit]: Shrinking vector to fit size: %zu", vec->size);
    if (!vecbuf_realloc_to(vec, vec->size)) {
        VECTOR_LOG("[vector_shrink_to_fit]: Error: Failed to allocate memory for shrink.");
        return;
    }

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

    Vector tmp = *vec1;
    *vec1 = *vec2;
    *vec2 = tmp;

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

    if (!vec || !item) {
        VECTOR_LOG("[vector_assign]: Error: Vector or item is NULL.");
        return;
    }
    if (pos >= vec->size) {
        VECTOR_LOG("[vector_assign]: Error: Position %zu is out of bounds.", pos);
        return;
    }

    vecbuf_store((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
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

    if (!vec || !item) {
        VECTOR_LOG("[vector_emplace]: Error: Vector or item is NULL.");
        return;
    }
    if (pos > vec->size || itemSize != vec->itemSize) {
        VECTOR_LOG("[vector_emplace]: Error: Invalid position or item size.");
        return;
    }
    size_t alias = vecbuf_alias_offset(vec, item);
    if (vec->size >= vec->capacitySize) {
        VECTOR_LOG("[vector_emplace]: Resizing vector to new capacity.");
        if (!vecbuf_ensure_one_more(vec)) {
            VECTOR_LOG("[vector_emplace]: Error: Failed to allocate memory for resizing.");
            return;
        }
    }

    char *base = (char *)vec->items;
    memmove(base + (pos + 1) * vec->itemSize, 
            base + pos * vec->itemSize, 
            (vec->size - pos) * vec->itemSize);

    const void *src = item;
    if (alias != (size_t)-1) {
        /* item lives in this vector: follow it through the realloc and the shift */
        if (alias >= pos * vec->itemSize) {
            alias += vec->itemSize;
        }
        src = base + alias;
    }
    vecbuf_store(base + pos * vec->itemSize, src, vec->itemSize);
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

    if (!vec || !item) {
        VECTOR_LOG("[vector_emplace_back]: Error: Vector or item is NULL.");
        return false;
    }
    if (itemSize != vec->itemSize) {
        VECTOR_LOG("[vector_emplace_back]: Error: Invalid item size. Expected: %zu, got: %zu", vec->itemSize, itemSize);
        return false;
    }
    size_t alias = vecbuf_alias_offset(vec, item);
    if (!vecbuf_ensure_one_more(vec)) {
        VECTOR_LOG("[vector_emplace_back]: Error: Resizing vector failed.");
        return false; 
    }

    const void *src = (alias != (size_t)-1) ? (const char *)vec->items + alias : item;
    vecbuf_store((char *)vec->items + vec->size * vec->itemSize, src, vec->itemSize);
    vec->size++;

    VECTOR_LOG("[vector_emplace_back]: Item emplaced successfully. New size: %zu", vec->size);
    return true; 
}


/**
 * @brief This function adds a new element to the end of the vector by copying the given item 
 * into the vector's storage. It ensures that there is enough capacity in the vector 
 * before adding the new element. If the vector's capacity is exceeded, it grows the
 * backing buffer with realloc (doubling), just like std::vector.
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
/* Out-of-line slow path for the inline vector_push_back() in vector.h. Reached
 * only when the vector is NULL or the buffer is full (the common in-place append
 * is handled inline at the call site). Performs the NULL check, the geometric
 * grow, and the element copy. */
bool vecbuf_push_back_grow(Vector *vec, const void *item) {
    VECTOR_LOG("[vecbuf_push_back_grow]: Entering with vector: %p, item: %p", (void*)vec, (void*)item);

    if (!vec || !item) {
        /* a NULL item used to be memcpy'd from (crash); stack / queue /
           priority_queue / deque all reject it too */
        VECTOR_LOG("[vecbuf_push_back_grow]: Error: Vector or item is NULL.");
        return false;
    }

    /* The element may live in this very buffer (v.push_back(v[0])): remember its
       offset, because the realloc below can move and free the old block. */
    size_t alias = vecbuf_alias_offset(vec, item);
    if (!vecbuf_ensure_one_more(vec)) {
        VECTOR_LOG("[vecbuf_push_back_grow]: Error: Memory allocation failed while growing.");
        return false;
    }

    const void *src = (alias != (size_t)-1) ? (const char *)vec->items + alias : item;
    vecbuf_store((char *)vec->items + (vec->size * vec->itemSize), src, vec->itemSize);
    vec->size++;

    VECTOR_LOG("[vecbuf_push_back_grow]: Item pushed successfully. New size: %zu", vec->size);
    return true;
}


/**
 * @brief This function releases all the memory allocated for the vector, including the
 * contiguous element buffer and the vector structure itself. After calling this function,
 * the vector pointer should not be used as it is no longer valid.
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
        return; 
    }

    VECTOR_LOG("[vector_deallocate]: Freeing items buffer: %p", (void*)vec->items);
    free(vec->items);  
    vec->items = NULL;

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

/* vector_at() is defined `static inline` in vector.h so the bounds-checked
 * element access inlines into the caller (no cross-TU call per access). */

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
 * is NULL or empty, the function returns NULL, the same value vector_rbegin returns for an
 * empty vector, so `for (it = rbegin; it != rend; --it)` runs zero times (rend used to be
 * items - itemSize there, and the loop dereferenced NULL). If `VECTOR_LOGGING_ENABLE` is
 * defined, an error message is logged to `stderr`.
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

    if (vec->size == 0 || !vec->items) {
        return NULL;   /* == vector_rbegin of an empty vector */
    }
    /* one element before the buffer: computed on the integer value, since
       pointer arithmetic outside the array is undefined behaviour */
    void *rend = (void *)((uintptr_t)vec->items - vec->itemSize);
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

    // For empty vector, cbegin == cend (both point at items). The standard
    // iteration loop `for (it = cbegin; it != cend; ...)` then runs zero times.
    VECTOR_LOG("[vector_cbegin]: Returning constant begin pointer: %p", (const void*)vec->items);
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
 * @return A constant pointer to one past the last element in the vector (equal to
 * vector_cbegin for an empty vector), or NULL if the vector is NULL. If
 * `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
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
    
    // For empty vector, this returns the same pointer as cbegin (items + 0).
    if (!vec->items) {
        return NULL;   /* shrunk empty vector: NULL + 0 is undefined in C */
    }
    const void *end_ptr = (const void *)((char *)vec->items + (vec->size * vec->itemSize));
    VECTOR_LOG("[vector_cend]: Returning constant end pointer: %p", end_ptr);
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
 * vector is NULL or empty, the function returns NULL (equal to vector_crbegin of an empty
 * vector). If `VECTOR_LOGGING_ENABLE` is defined, an error message is logged to `stderr`.
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

    if (vec->size == 0 || !vec->items) {
        return NULL;   /* == vector_crbegin of an empty vector */
    }
    const void *crend_ptr = (const void *)((uintptr_t)vec->items - vec->itemSize); // One before the first element
    VECTOR_LOG("[vector_crend]: Returning crend pointer: %p", crend_ptr);
    VECTOR_LOG("[vector_crend]: Exiting.");

    return crend_ptr;
}


/**
 * @brief Remove all elements; capacity is preserved.
 *
 * Matches std::vector::clear() — size is set to 0, capacity is left
 * intact. Reuses the existing buffer for subsequent push_backs.
 *
 * @param vec  Vector to clear. NULL is a safe no-op.
 *
 * @note If your vector stores pointers to heap-owned objects, free
 *       them before calling clear() — this function only zeroes the
 *       size counter; it does NOT call per-element destructors.
 *       Call vector_shrink_to_fit() afterwards if you want to
 *       release the backing storage too.
 */
void vector_clear(Vector *vec) {
    VECTOR_LOG("[vector_clear]: Entering with vector: %p", (void*)vec);

    if (!vec) {
        VECTOR_LOG("[vector_clear]: Error: Vector is NULL.");
        return;
    }

    // Reset size only; preserve capacity. Matches C++ std::vector::clear semantics
    // and avoids the cost of reallocating. Use vector_shrink_to_fit to release capacity.
    vec->size = 0;
    VECTOR_LOG("[vector_clear]: Vector size set to 0. Capacity preserved: %zu", vec->capacitySize);
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
    VECTOR_LOG("[vector_max_size]: Entering with vector: %p", (void*)vec);

    if (!vec || vec->itemSize == 0) {
        VECTOR_LOG("[vector_max_size]: Error: Vector is NULL or itemSize is 0.");
        return 0;
    }

    // Maximum number of elements addressable in size_t bytes.
    size_t result = ((size_t)-1) / vec->itemSize;
    VECTOR_LOG("[vector_max_size]: Returning max element count: %zu", result);
    return result;
}


/* ================================================================== */
/* Bulk operations, construction helpers and algorithms               */
/* ================================================================== */

/**
 * @brief Out-of-line growth used by the inline / typed fast paths: makes room
 *        for `extra` more elements (doubling, at least the exact need).
 *
 * @return true when `extra` more elements fit; false (vector unchanged) for a
 *         NULL vector, a size that would overflow, or when memory runs out.
 */
bool vecbuf_grow_for(Vector *vec, size_t extra) {
    if (!vec) {
        return false;
    }
    if (extra <= vec->capacitySize - vec->size) {
        return true;
    }
    if (extra > ((size_t)-1) / vec->itemSize - vec->size) {
        VECTOR_LOG("[vecbuf_grow_for]: Error: %zu more elements overflow size_t.", extra);
        return false;
    }
    size_t need = vec->size + extra;
    /* doubling first; if that much memory is not available, the exact need */
    return vecbuf_realloc_to(vec, vecbuf_next_capacity(vec, need)) || vecbuf_realloc_to(vec, need);
}


/* `item` copied aside when it lives in this vector's buffer, which the caller
 * is about to move (realloc) or overwrite. Small items go to `local`, larger
 * ones to the heap (*heap, to be freed). Returns the pointer to read from, or
 * NULL when that heap copy fails. */
static const void* vecbuf_stable_item(const Vector *vec, const void *item,
                                      unsigned char *local, size_t localSize, void **heap) {
    *heap = NULL;
    if (vecbuf_alias_offset(vec, item) == (size_t)-1) {
        return item;
    }
    if (vec->itemSize <= localSize) {
        memcpy(local, item, vec->itemSize);
        return local;
    }
    *heap = malloc(vec->itemSize);
    if (*heap) {
        memcpy(*heap, item, vec->itemSize);
    }
    return *heap;
}


/* Writes `count` copies of the `isz`-byte `item` at `dst` (item must not lie
 * in the destination range). An item whose bytes are all equal (0, -1, ...)
 * becomes one memset; otherwise the filled prefix is doubled with memcpy, in
 * chunks of at most 64 KiB so the source stays in cache. */
static void vecbuf_fill(char *dst, const void *item, size_t isz, size_t count) {
    if (count == 0) {
        return;
    }
    const unsigned char *b = (const unsigned char *)item;
    size_t k = 1;
    while (k < isz && b[k] == b[0]) {
        k++;
    }
    if (k == isz) {
        memset(dst, b[0], count * isz);
        return;
    }
    memcpy(dst, item, isz);
    size_t done = 1;
    size_t maxChunk = (65536 / isz) ? (65536 / isz) : 1;
    while (done < count) {
        size_t chunk = done < maxChunk ? done : maxChunk;
        if (chunk > count - done) {
            chunk = count - done;
        }
        memcpy(dst + done * isz, dst, chunk * isz);
        done += chunk;
    }
}


/* A new buffer for `count` elements whose old contents do not matter (they
 * are about to be replaced): allocated fresh instead of realloc'd, so nothing
 * is copied; the old buffer is freed only when that worked. */
static bool vecbuf_replace_buffer(Vector *vec, size_t count) {
    if (count > ((size_t)-1) / vec->itemSize) {
        return false;
    }
    void *fresh = malloc(count * vec->itemSize);
    if (!fresh) {
        return false;
    }
    free(vec->items);
    vec->items = fresh;
    vec->capacitySize = count;
    return true;
}


/**
 * @brief Creates an empty vector with room for `capacity` elements.
 *
 * Like vector_create, but the initial capacity is chosen by the caller:
 * `capacity == 0` allocates no element buffer at all (it is created by the
 * first insertion), which is what an empty std::vector costs.
 *
 * @param itemSize Size of one element in bytes (> 0).
 * @param capacity Number of elements to reserve.
 * @return The new vector, or NULL for itemSize 0 / out of memory.
 */
Vector* vector_create_with_capacity(size_t itemSize, size_t capacity) {
    if (itemSize == 0) {
        VECTOR_LOG("[vector_create_with_capacity]: Error: itemSize must be greater than 0.");
        return NULL;
    }
    Vector* vec = (Vector*)malloc(sizeof(Vector));
    if (!vec) {
        return NULL;
    }
    vec->items        = NULL;
    vec->size         = 0;
    vec->capacitySize = 0;
    vec->itemSize     = itemSize;
    vec->compare      = NULL;
    if (capacity && !vecbuf_realloc_to(vec, capacity)) {
        free(vec);
        return NULL;
    }
    return vec;
}


/**
 * @brief Creates a vector holding a copy of `count` elements from `data`
 *        (std::vector's range constructor).
 *
 * @return The new vector (capacity == count, or 32 for count 0), or NULL for
 *         itemSize 0, NULL data with count > 0, or out of memory.
 */
Vector* vector_from_array(size_t itemSize, const void* data, size_t count) {
    if (!data && count) {
        return NULL;
    }
    Vector* vec = vector_create_with_capacity(itemSize, count ? count : 32);
    if (!vec) {
        return NULL;
    }
    if (count) {
        memcpy(vec->items, data, count * itemSize);
        vec->size = count;
    }
    return vec;
}


/**
 * @brief Wraps an existing malloc'd buffer in a vector without copying it
 *        (std::vector cannot adopt a buffer).
 *
 * The vector takes ownership: it reallocs and frees the buffer from now on.
 *
 * @param buffer   Buffer from malloc / calloc / realloc holding `count`
 *                 elements, with room for `capacity` (NULL only when capacity is 0).
 * @param itemSize Size of one element (> 0).
 * @param count    Number of valid elements (<= capacity).
 * @param capacity Number of elements the buffer can hold.
 * @return The vector, or NULL for invalid arguments / out of memory; the
 *         buffer then still belongs to the caller.
 */
Vector* vector_from_buffer(void* buffer, size_t itemSize, size_t count, size_t capacity) {
    if (itemSize == 0 || count > capacity || (!buffer && capacity) ||
        (capacity && capacity > ((size_t)-1) / itemSize)) {
        return NULL;
    }
    Vector* vec = vector_create_with_capacity(itemSize, 0);
    if (!vec) {
        return NULL;
    }
    vec->items        = buffer;
    vec->size         = count;
    vec->capacitySize = capacity;
    return vec;
}


/**
 * @brief Deep copy: a new vector with the same elements, element size and
 *        comparator (std::vector's copy constructor; capacity == size).
 *
 * @return The copy, or NULL for a NULL source / out of memory.
 */
Vector* vector_copy(const Vector* src) {
    if (!src) {
        return NULL;
    }
    Vector* vec = vector_from_array(src->itemSize, src->items, src->size);
    if (vec) {
        vec->compare = src->compare;
    }
    return vec;
}


/**
 * @brief New vector with a copy of elements [pos, pos + count).
 *
 * @return The new vector (with the source's comparator), or NULL when the
 *         range does not lie inside the vector or memory runs out.
 */
Vector* vector_slice(const Vector* vec, size_t pos, size_t count) {
    if (!vec || pos > vec->size || count > vec->size - pos) {
        return NULL;
    }
    Vector* out = vector_from_array(vec->itemSize, count ? (const char*)vec->items + pos * vec->itemSize : NULL, count);
    if (out) {
        out->compare = vec->compare;
    }
    return out;
}


/**
 * @brief Hands the element buffer to the caller, who must free() it; the
 *        vector stays valid and becomes empty, with no buffer.
 *
 * A zero-copy way to pass the elements to an API that takes ownership of an
 * array (std::vector cannot release its storage).
 *
 * @param vec   The vector.
 * @param count Receives the number of elements in the returned buffer (may be NULL).
 * @return The buffer (NULL when the vector had none), or NULL for a NULL vector.
 */
void* vector_release(Vector* vec, size_t* count) {
    if (count) {
        *count = vec ? vec->size : 0;
    }
    if (!vec) {
        return NULL;
    }
    void* items = vec->items;
    vec->items        = NULL;
    vec->size         = 0;
    vec->capacitySize = 0;
    return items;
}


/**
 * @brief Like vector_resize, but new elements are copies of `*fill`
 *        (std::vector::resize(n, value)); `fill` may point into the vector.
 *        NULL `fill` zero-initialises them like vector_resize.
 *
 * @return true on success; false (vector unchanged) for NULL / out of memory.
 */
bool vector_resize_with(Vector* vec, size_t size, const void* fill) {
    if (!vec) {
        return false;
    }
    if (!fill || size <= vec->size) {
        return vector_resize(vec, size);
    }
    unsigned char local[64];
    void* heap;
    const void* item = vecbuf_stable_item(vec, fill, local, sizeof local, &heap);
    if (!item || !vecbuf_grow_for(vec, size - vec->size)) {
        free(heap);
        return false;
    }
    vecbuf_fill((char*)vec->items + vec->size * vec->itemSize, item, vec->itemSize, size - vec->size);
    vec->size = size;
    free(heap);
    return true;
}


/**
 * @brief Appends a new zero-filled element and returns a pointer to it, so a
 *        large element can be built in place instead of being copied in
 *        (the C form of emplace_back).
 *
 * @return The new last element (valid until the vector grows again), or NULL
 *         for a NULL vector / out of memory.
 */
void* vector_emplace_back_slot(Vector* vec) {
    if (!vecbuf_grow_for(vec, 1)) {
        return NULL;
    }
    void* slot = (char*)vec->items + vec->size * vec->itemSize;
    memset(slot, 0, vec->itemSize);
    vec->size++;
    return slot;
}


/**
 * @brief Inserts copies of `count` elements from `data` before position
 *        `pos` (std::vector::insert(pos, first, last)). `data` may point into
 *        this vector.
 *
 * @return true on success (also for count 0); false (vector unchanged) for
 *         NULL arguments, pos > size, or out of memory.
 */
bool vector_insert_range(Vector* vec, size_t pos, const void* data, size_t count) {
    if (!vec || pos > vec->size || (!data && count)) {
        return false;
    }
    if (count == 0) {
        return true;
    }
    size_t isz = vec->itemSize;
    void* temp = NULL;
    if (vecbuf_alias_offset(vec, data) != (size_t)-1) {
        /* the source moves with the shift (and the realloc): copy it aside */
        temp = malloc(count * isz);
        if (!temp) {
            return false;
        }
        memcpy(temp, data, count * isz);
        data = temp;
    }
    if (!vecbuf_grow_for(vec, count)) {
        free(temp);
        return false;
    }
    char* base = (char*)vec->items;
    memmove(base + (pos + count) * isz, base + pos * isz, (vec->size - pos) * isz);
    memcpy(base + pos * isz, data, count * isz);
    vec->size += count;
    free(temp);
    return true;
}


/**
 * @brief Inserts `count` copies of `*item` before position `pos`
 *        (std::vector::insert(pos, n, value)); `item` may point into the vector.
 *
 * @return true on success (also for count 0); false (vector unchanged) otherwise.
 */
bool vector_insert_fill(Vector* vec, size_t pos, size_t count, const void* item) {
    if (!vec || !item || pos > vec->size) {
        return false;
    }
    if (count == 0) {
        return true;
    }
    unsigned char local[64];
    void* heap;
    const void* src = vecbuf_stable_item(vec, item, local, sizeof local, &heap);
    if (!src || !vecbuf_grow_for(vec, count)) {
        free(heap);
        return false;
    }
    size_t isz = vec->itemSize;
    char* base = (char*)vec->items;
    memmove(base + (pos + count) * isz, base + pos * isz, (vec->size - pos) * isz);
    vecbuf_fill(base + pos * isz, src, isz, count);
    vec->size += count;
    free(heap);
    return true;
}


/**
 * @brief Replaces the contents with copies of `count` elements from `data`
 *        (std::vector::assign(first, last)); `data` may point into the vector.
 *
 * @return true on success; false (vector unchanged) for NULL arguments or out of memory.
 */
bool vector_assign_range(Vector* vec, const void* data, size_t count) {
    if (!vec || (!data && count)) {
        return false;
    }
    size_t isz = vec->itemSize;
    size_t alias = vecbuf_alias_offset(vec, data);
    if (count > vec->capacitySize) {
        /* source outside the buffer: no need to keep the old contents */
        bool ok = (alias == (size_t)-1) ? vecbuf_replace_buffer(vec, count)
                                        : (count <= ((size_t)-1) / isz && vecbuf_realloc_to(vec, count));
        if (!ok) {
            return false;
        }
    }
    const void* src = (alias != (size_t)-1) ? (const char*)vec->items + alias : data;
    if (count) {
        memmove(vec->items, src, count * isz);
    }
    vec->size = count;
    return true;
}


/**
 * @brief Replaces the contents with `count` copies of `*item`
 *        (std::vector::assign(n, value)); `item` may point into the vector.
 *
 * @return true on success; false (vector unchanged) for NULL arguments or out of memory.
 */
bool vector_assign_fill(Vector* vec, size_t count, const void* item) {
    if (!vec || !item) {
        return false;
    }
    unsigned char local[64];
    void* heap;
    const void* src = vecbuf_stable_item(vec, item, local, sizeof local, &heap);
    if (!src) {
        return false;
    }
    bool zero = true;
    for (size_t k = 0; k < vec->itemSize && zero; k++) {
        zero = ((const unsigned char*)src)[k] == 0;
    }
    if (count > vec->capacitySize) {
        if (zero) {
            /* all-zero value into a new buffer: calloc, no fill pass (see vector_resize) */
            void* fresh = (count <= ((size_t)-1) / vec->itemSize) ? calloc(count, vec->itemSize) : NULL;
            if (!fresh) {
                free(heap);
                return false;
            }
            free(vec->items);
            vec->items = fresh;
            vec->capacitySize = count;
            vec->size = count;
            free(heap);
            return true;
        }
        if (!vecbuf_replace_buffer(vec, count)) {
            free(heap);   /* (src was copied aside if it pointed into the old buffer) */
            return false;
        }
    }
    vecbuf_fill((char*)vec->items, src, vec->itemSize, count);
    vec->size = count;
    free(heap);
    return true;
}


/**
 * @brief Overwrites every element with a copy of `*item` (std::fill over the
 *        vector); `item` may be one of the vector's own elements.
 */
void vector_fill(Vector* vec, const void* item) {
    if (!vec || !item || vec->size == 0) {
        return;
    }
    unsigned char local[64];
    void* heap;
    const void* src = vecbuf_stable_item(vec, item, local, sizeof local, &heap);
    if (src) {
        vecbuf_fill((char*)vec->items, src, vec->itemSize, vec->size);
    }
    free(heap);
}


/**
 * @brief Appends copies of `count` elements from `data` with a single copy
 *        (std::vector::insert(end, first, last) / append_range). `data` may
 *        point into this vector (v.append(v.data(), v.size()) doubles it).
 *
 * @return true on success (also for count 0); false (vector unchanged) for
 *         NULL arguments or out of memory.
 */
bool vector_append(Vector* vec, const void* data, size_t count) {
    if (!vec || (!data && count)) {
        return false;
    }
    if (count == 0) {
        return true;
    }
    size_t alias = vecbuf_alias_offset(vec, data);
    if (!vecbuf_grow_for(vec, count)) {
        return false;
    }
    size_t isz = vec->itemSize;
    const void* src = (alias != (size_t)-1) ? (const char*)vec->items + alias : data;
    memmove((char*)vec->items + vec->size * isz, src, count * isz);
    vec->size += count;
    return true;
}


/**
 * @brief Adds `count` elements at the end WITHOUT initialising them and
 *        returns a pointer to the first one, to be filled in place.
 *
 * Filling a vector this way runs at the speed of a plain array loop: with
 * per-element push_back (here or with std::vector) every append stores and
 * re-reads the element count, which is about twice as slow. std::vector has no
 * equivalent (C++23 resize_and_overwrite exists only for std::string).
 *
 * @return The first new element (valid until the vector grows again), or
 *         NULL for a NULL vector / out of memory (vector unchanged). For
 *         count 0 the end of the elements is returned (NULL when the
 *         vector has no buffer yet).
 */
void* vector_append_uninit(Vector* vec, size_t count) {
    if (!vecbuf_grow_for(vec, count) || !vec->items) {
        return NULL;
    }
    void* first = (char*)vec->items + vec->size * vec->itemSize;
    vec->size += count;
    return first;
}


/**
 * @brief Appends all elements of `src` to `dst` (src may be dst itself).
 *
 * @return true on success; false for NULL vectors, different element sizes,
 *         or out of memory (dst unchanged).
 */
bool vector_append_vector(Vector* dst, const Vector* src) {
    if (!dst || !src || dst->itemSize != src->itemSize) {
        return false;
    }
    return vector_append(dst, src->items, src->size);
}


/**
 * @brief Removes the element at `pos` in O(1) by moving the last element
 *        into its place. The order of the elements is not kept (there is no
 *        std::vector equivalent; erase is O(n)).
 *
 * @return true when an element was removed; false for NULL / pos >= size.
 */
bool vector_swap_remove(Vector* vec, size_t pos) {
    if (!vec || pos >= vec->size) {
        return false;
    }
    size_t last = vec->size - 1;
    if (pos != last) {
        vecbuf_store((char*)vec->items + pos * vec->itemSize,
                     (char*)vec->items + last * vec->itemSize, vec->itemSize);
    }
    vec->size = last;
    return true;
}


/**
 * @brief Removes the last element, copying it to `out` first (`out` may be
 *        NULL). Unlike vector_pop_back, the value is safe to keep.
 *
 * @return true when an element was removed; false for a NULL or empty vector.
 */
bool vector_pop_back_value(Vector* vec, void* out) {
    if (!vec || vec->size == 0) {
        return false;
    }
    vec->size--;
    if (out) {
        memcpy(out, (char*)vec->items + vec->size * vec->itemSize, vec->itemSize);
    }
    return true;
}


/**
 * @brief Removes every element for which `pred(element, ctx)` is true,
 *        keeping the order of the rest (C++20 std::erase_if).
 *
 * @return The number of elements removed (0 for NULL arguments).
 */
size_t vector_erase_if(Vector* vec, VectorPredicateFunc pred, void* ctx) {
    if (!vec || !pred) {
        return 0;
    }
    size_t isz = vec->itemSize;
    char* base = (char*)vec->items;
    size_t w = 0;
    for (size_t r = 0; r < vec->size; ++r) {
        char* item = base + r * isz;
        if (!pred(item, ctx)) {
            if (w != r) {
                vecbuf_store(base + w * isz, item, isz);
            }
            w++;
        }
    }
    size_t removed = vec->size - w;
    vec->size = w;
    return removed;
}


/* Element equality: the vector's comparator, else the bytes. */
static bool vecbuf_equal(const Vector* vec, const void* a, const void* b) {
    return vec->compare ? vec->compare(a, b) == 0 : memcmp(a, b, vec->itemSize) == 0;
}


/* Index of the first of `n` elements of `isz` bytes at `base` whose bytes
 * equal `key` (VECTOR_NPOS if none). 1-byte elements use memchr; 2/4/8-byte
 * ones compare whole integers, unrolled by four (the same loop shape as
 * std::find), instead of a memcmp call per element (about 6x faster). */
#define VECBUF_SCAN(UT) do { \
        UT k_; memcpy(&k_, key, sizeof k_); \
        size_t i_ = 0; \
        for (; i_ + 4 <= n; i_ += 4) { \
            UT a_, b_, c_, d_; \
            memcpy(&a_, base + (i_ + 0) * sizeof(UT), sizeof(UT)); \
            memcpy(&b_, base + (i_ + 1) * sizeof(UT), sizeof(UT)); \
            memcpy(&c_, base + (i_ + 2) * sizeof(UT), sizeof(UT)); \
            memcpy(&d_, base + (i_ + 3) * sizeof(UT), sizeof(UT)); \
            if (a_ == k_) return i_; \
            if (b_ == k_) return i_ + 1; \
            if (c_ == k_) return i_ + 2; \
            if (d_ == k_) return i_ + 3; \
        } \
        for (; i_ < n; i_++) { \
            UT a_; memcpy(&a_, base + i_ * sizeof(UT), sizeof(UT)); \
            if (a_ == k_) return i_; \
        } \
        return VECTOR_NPOS; \
    } while (0)

static size_t vecbuf_find_bytes(const char* base, size_t n, size_t isz, const void* key) {
    switch (isz) {
        case 1: {
            const void* hit = n ? memchr(base, *(const unsigned char*)key, n) : NULL;
            return hit ? (size_t)((const char*)hit - base) : VECTOR_NPOS;
        }
        case 2: VECBUF_SCAN(uint16_t);
        case 4: VECBUF_SCAN(uint32_t);
        case 8: VECBUF_SCAN(uint64_t);
        default:
            for (size_t i = 0; i < n; i++) {
                if (memcmp(base + i * isz, key, isz) == 0) {
                    return i;
                }
            }
            return VECTOR_NPOS;
    }
}
#undef VECBUF_SCAN


/**
 * @brief Removes every element equal to `*value` (C++20 std::erase), using
 *        the comparator from vector_set_compare or the bytes. `value` may be
 *        one of the vector's own elements.
 *
 * @return The number of elements removed.
 */
size_t vector_remove_value(Vector* vec, const void* value) {
    if (!vec || !value) {
        return 0;
    }
    unsigned char local[64];
    void* heap;
    const void* key = vecbuf_stable_item(vec, value, local, sizeof local, &heap);
    if (!key) {
        return 0;
    }
    size_t isz = vec->itemSize;
    char* base = (char*)vec->items;
    size_t w = 0;
    if (!vec->compare) {
        /* copy the runs between matches in one memmove each */
        size_t r = 0;
        while (r < vec->size) {
            size_t hit = vecbuf_find_bytes(base + r * isz, vec->size - r, isz, key);
            size_t run = (hit == VECTOR_NPOS) ? vec->size - r : hit;
            if (run && w != r) {
                memmove(base + w * isz, base + r * isz, run * isz);
            }
            w += run;
            r += run + (hit == VECTOR_NPOS ? 0 : 1);
        }
    }
    else {
        for (size_t r = 0; r < vec->size; ++r) {
            char* item = base + r * isz;
            if (!vecbuf_equal(vec, item, key)) {
                if (w != r) {
                    vecbuf_store(base + w * isz, item, isz);
                }
                w++;
            }
        }
    }
    size_t removed = vec->size - w;
    vec->size = w;
    free(heap);
    return removed;
}


/**
 * @brief Index of the first element equal to `*value` (comparator from
 *        vector_set_compare, else the bytes), or VECTOR_NPOS.
 */
size_t vector_find(const Vector* vec, const void* value) {
    if (!vec || !value) {
        return VECTOR_NPOS;
    }
    const char* base = (const char*)vec->items;
    if (!vec->compare) {
        return vecbuf_find_bytes(base, vec->size, vec->itemSize, value);
    }
    for (size_t i = 0; i < vec->size; ++i) {
        if (vecbuf_equal(vec, base + i * vec->itemSize, value)) {
            return i;
        }
    }
    return VECTOR_NPOS;
}


/** @brief true when an element equal to `*value` is present (see vector_find). */
bool vector_contains(const Vector* vec, const void* value) {
    return vector_find(vec, value) != VECTOR_NPOS;
}


/**
 * @brief Sorts the elements in ascending order with `compare` (qsort; not
 *        stable). NULL `compare` uses the comparator from vector_set_compare.
 *
 * @return true when sorted; false for a NULL vector or when no comparator is
 *         available.
 */
bool vector_sort(Vector* vec, VectorCompareFunc compare) {
    if (!vec) {
        return false;
    }
    VectorCompareFunc cmp = compare ? compare : vec->compare;
    if (!cmp) {
        return false;
    }
    if (vec->size > 1) {
        qsort(vec->items, vec->size, vec->itemSize, cmp);
    }
    return true;
}


/**
 * @brief Binary search in a vector sorted ascending by `compare` (NULL: the
 *        comparator from vector_set_compare).
 *
 * @return The index of the FIRST element equal to `*value`, or VECTOR_NPOS
 *         (also without a comparator).
 */
size_t vector_binary_search(const Vector* vec, const void* value, VectorCompareFunc compare) {
    if (!vec || !value) {
        return VECTOR_NPOS;
    }
    VectorCompareFunc cmp = compare ? compare : vec->compare;
    if (!cmp) {
        return VECTOR_NPOS;
    }
    const char* base = (const char*)vec->items;
    size_t lo = 0, hi = vec->size;   /* lower bound in [lo, hi) */
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (cmp(base + mid * vec->itemSize, value) < 0) {
            lo = mid + 1;
        }
        else {
            hi = mid;
        }
    }
    if (lo < vec->size && cmp(base + lo * vec->itemSize, value) == 0) {
        return lo;
    }
    return VECTOR_NPOS;
}


/* Swaps two non-overlapping elements of `n` bytes in 64-byte steps. */
static void vecbuf_swap_bytes(char* a, char* b, size_t n) {
    unsigned char tmp[64];
    while (n) {
        size_t k = n < sizeof tmp ? n : sizeof tmp;
        memcpy(tmp, a, k);
        memcpy(a, b, k);
        memcpy(b, tmp, k);
        a += k;
        b += k;
        n -= k;
    }
}


/** @brief Reverses the order of the elements in place (std::reverse). */
void vector_reverse(Vector* vec) {
    if (!vec || vec->size < 2) {
        return;
    }
    size_t isz = vec->itemSize;
    char* lo = (char*)vec->items;
    char* hi = lo + (vec->size - 1) * isz;
    while (lo < hi) {
        vecbuf_swap_bytes(lo, hi, isz);
        lo += isz;
        hi -= isz;
    }
}


/**
 * @brief Removes consecutive duplicate elements, keeping the first of each
 *        run (std::unique + erase); sort first to remove all duplicates.
 *        Equality: the comparator from vector_set_compare, else the bytes.
 *
 * @return The number of elements removed.
 */
size_t vector_unique(Vector* vec) {
    if (!vec || vec->size < 2) {
        return 0;
    }
    size_t isz = vec->itemSize;
    char* base = (char*)vec->items;
    size_t w = 1;
    for (size_t r = 1; r < vec->size; ++r) {
        if (!vecbuf_equal(vec, base + (w - 1) * isz, base + r * isz)) {
            if (w != r) {
                vecbuf_store(base + w * isz, base + r * isz, isz);
            }
            w++;
        }
    }
    size_t removed = vec->size - w;
    vec->size = w;
    return removed;
}


/** @brief Calls `fn(element, ctx)` for every element, front to back. */
void vector_for_each(Vector* vec, VectorVisitFunc fn, void* ctx) {
    if (!vec || !fn) {
        return;
    }
    char* base = (char*)vec->items;
    for (size_t i = 0; i < vec->size; ++i) {
        fn(base + i * vec->itemSize, ctx);
    }
}
