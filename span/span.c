/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Span
*/

#include "span.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

typedef struct MemoryPoolSpan {
    void *pool;
    size_t poolSize;
    size_t used;
} MemoryPoolSpan;

static MemoryPoolSpan *global_span_pool = NULL;

static void initialize_span_memory_pool(size_t size) {
    if (global_span_pool != NULL) {
        return; // Already initialized
    }

    global_span_pool = (MemoryPoolSpan*)malloc(sizeof(MemoryPoolSpan));
    if (!global_span_pool) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to allocate memory for span pool\n");
        #endif 
        exit(-1); // Consider a more graceful exit strategy
    }

    global_span_pool->pool = malloc(size);
    if (!global_span_pool->pool) {
        free(global_span_pool);
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to allocate memory for span pool data\n");
        #endif 
        exit(-1); // Consider a more graceful exit strategy
    }

    global_span_pool->poolSize = size;
    global_span_pool->used = 0;
}

static void destroy_span_memory_pool() {
    if (!global_span_pool) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span memory pool is NULL in destroy_span_memory_pool\n");
        #endif 
        return;
    }
    free(global_span_pool->pool);
    free(global_span_pool);
    global_span_pool = NULL;
}

static void* memory_pool_span_allocate(size_t size) {
    if (!global_span_pool) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span memory pool is not initialized in memory_pool_span_allocate\n");
        #endif 
        return NULL;
    }
    if (global_span_pool->used + size > global_span_pool->poolSize) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Not enough space in span pool\n");
        #endif 
        return NULL;
    }
    void *allocated = (char*)global_span_pool->pool + global_span_pool->used;
    global_span_pool->used += size;

    return allocated;
}

/**
 * @brief Creates a new Span object.
 * 
 * This function initializes a new Span structure that points to a contiguous block of memory. 
 * It allocates memory for the span using a memory pool. If the data is NULL or either the 
 * element count or size is zero, the function will log an error (if logging is enabled) and terminate the program.
 * 
 * @param data Pointer to the data to be managed by the span.
 * @param elemCount Number of elements in the data.
 * @param elemSize Size of each element in bytes.
 * 
 * @return Span* Pointer to the newly created Span structure.
 */
Span* span_create(void* data, size_t elemCount, size_t elemSize) {
    if (!data) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null data provided to span_create\n");
        #endif 
        exit(-1);
    }
    if (elemCount == 0 || elemSize == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Element count or size is zero in span_create\n");
        #endif 
        exit(-1);
    }

    if (!global_span_pool) {
        initialize_span_memory_pool(10 * 10); // For example, initialize with 1MB
    }

    Span *newSpan = (Span*)malloc(sizeof(Span));
    if (!newSpan) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to allocate memory for Span structure\n");
        #endif 
        exit(-1); // Consider a more graceful exit strategy
    }

    newSpan->data = memory_pool_span_allocate(elemCount * elemSize);
    if (!newSpan->data) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Failed to allocate memory for Span data\n");
        #endif 
        free(newSpan); // Free allocated Span structure
        exit(-1); // Consider a more graceful exit strategy
    }

    memcpy(newSpan->data, data, elemCount * elemSize);
    newSpan->size = elemCount * elemSize;
    newSpan->elemSize = elemSize;

    return newSpan;
}

/**
 * @brief Destroys the Span object.
 * 
 * This function deallocates the memory used by the Span structure. It also resets the 
 * Span's data pointer and size to zero. If the span pointer is NULL, it logs an error 
 * (if logging is enabled) and returns without performing any action.
 * 
 * @param span Pointer to the Span structure to be destroyed.
 */
void span_destroy(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_destroy\n");
        #endif 
        return;
    }
    // Reset the Span structure
    span->data = NULL;
    span->size = 0;
    free(span);
    // Consider destroying the memory pool if no longer needed
    destroy_span_memory_pool();
}

/**
 * @brief Retrieves a pointer to an element at the specified index within the span.
 * 
 * This function returns a pointer to the element located at the given index within the span. 
 * If the span is NULL or the index is out of bounds, the function logs an error (if logging 
 * is enabled) and returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @param index Index of the element to access.
 * 
 * @return void* Pointer to the element at the specified index, or NULL if out of bounds.
 */
void* span_at(Span* span, size_t index) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_at\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    if (span && index < span->size / span->elemSize) {
        return (char*)span->data + (index * span->elemSize);
    }
    return NULL; // Index out of bounds or span is NULL
}

/**
 * @brief Returns the size of the span.
 * 
 * This function returns the total size of the span in bytes. If the span is NULL, it logs 
 * an error (if logging is enabled) and returns 0.
 * 
 * @param span Pointer to the Span structure.
 * @return size_t Size of the span in bytes, or 0 if the span is NULL.
 */
size_t span_size(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span is NULL in span_size.\n");
        #endif 
        return 0;
    }
    return span->size;
}

/**
 * @brief Returns a pointer to the first element in the span.
 * 
 * This function returns a pointer to the first element in the span. If the span is NULL or empty, 
 * it returns NULL and logs an error if logging is enabled.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the first element, or NULL if the span is NULL or empty.
 */
void* span_front(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_front.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    if (span->size == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span is empty in span_front.\n");
        #endif 
        return NULL; // Handle empty span
    }
    return span->data; // The first element is at the start of data
}

/**
 * @brief Returns a pointer to the last element in the span.
 * 
 * This function returns a pointer to the last element in the span. If the span is NULL or empty, 
 * it returns NULL and logs an error if logging is enabled.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the last element, or NULL if the span is NULL or empty.
 */
void* span_back(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_back.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    if (span->size == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span is empty in span_back.\n");
        #endif 
        return NULL; // Handle empty span
    }
    return (char*)span->data + (span->size - span->elemSize);// Calculate the address of the last element
}

/**
 * @brief Returns a pointer to the data managed by the span.
 * 
 * This function returns a pointer to the data managed by the span. If the span is NULL, 
 * it returns NULL and logs an error if logging is enabled.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the span's data, or NULL if the span is NULL.
 */
void* span_data(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_data.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span->data;
}

/**
 * @brief Returns a constant pointer to the data managed by the span.
 * 
 * This function returns a constant pointer to the data managed by the span. If the span is NULL, 
 * it returns NULL and logs an error if logging is enabled.
 * 
 * @param span Pointer to the Span structure.
 * @return Constant pointer to the span's data, or NULL if the span is NULL.
 */
const void* span_cdata(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_cdata.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span->data;
}

/**
 * @brief Checks if the span is empty.
 * 
 * This function checks if the span is empty (i.e., size is zero). If the span is NULL, 
 * it logs an error if logging is enabled and returns true.
 * 
 * @param span Pointer to the Span structure.
 * @return true if the span is empty or NULL, false otherwise.
 */
bool span_empty(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_empty.\n");
        #endif 
        return true; // Treat NULL span as empty
    }
    return span->size == 0;
}

/**
 * @brief Returns the size of the span in bits.
 * 
 * This function returns the size of the span in bits. If the span is NULL, it logs an error 
 * if logging is enabled and returns 0.
 * 
 * @param span Pointer to the Span structure.
 * @return Size of the span in bits, or 0 if the span is NULL.
 */
size_t span_size_bits(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: span_size_bits: Span is NULL.\n");
        #endif 
        return 0;
    }
    return span->size * span->elemSize;
}

/**
 * @brief Returns a subspan containing the first `count` elements of the span.
 * 
 * This function creates a subspan that includes the first `count` elements of the 
 * original span. If the count exceeds the span size or the span is NULL, it returns 
 * an empty span.
 * 
 * @param span Pointer to the Span structure.
 * @param count Number of elements to include in the subspan.
 * 
 * @return A subspan containing the first `count` elements, or an empty span if invalid.
 */
Span span_first(Span* span, size_t count) {
    Span result = {NULL, 0, 0};

    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_first.\n");
        #endif 
        return result; // Handle NULL span pointer
    }
    if (count == 0 || span->size == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Count is zero or Span is empty in span_first.\n");
        #endif 
        return result; // Handle zero count or empty span
    }
    if (count * span->elemSize > span->size) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Count exceeds Span size in span_first.\n");
        #endif 
        return result; // Handle out-of-bounds count
    }

    result.data = span->data;
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    return result;
}

/**
 * @brief Returns a subspan containing the last `count` elements of the span.
 * 
 * This function creates a subspan that includes the last `count` elements of the 
 * original span. If the count exceeds the span size or the span is NULL, it returns 
 * an empty span.
 * 
 * @param span Pointer to the Span structure.
 * @param count Number of elements to include in the subspan.
 * 
 * @return A subspan containing the last `count` elements, or an empty span if invalid.
 */
Span span_last(Span* span, size_t count) {
    Span result = {NULL, 0, 0};

    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_last.\n");
        #endif 
        return result; // Handle NULL span pointer
    }
    if (count == 0 || span->size == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Count is zero or Span is empty in span_last.\n");
        #endif 
        return result; // Handle zero count or empty span
    }
    if (count * span->elemSize > span->size) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Count exceeds Span size in span_last.\n");
        #endif 
        return result; // Handle out-of-bounds count
    }

    result.data = (char*)span->data + (span->size - count * span->elemSize);
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    return result;
}

/**
 * @brief Returns a subspan starting from the specified offset and containing `count` elements.
 * 
 * This function creates a subspan starting at the given `offset` and includes `count` elements. 
 * If the offset or count exceeds the span bounds, or the span is NULL, it returns an empty span.
 * 
 * @param span Pointer to the Span structure.
 * @param offset Starting position within the span.
 * @param count Number of elements to include in the subspan.
 * 
 * @return A subspan starting at `offset` with `count` elements, or an empty span if invalid.
 */
Span span_subspan(Span* span, size_t offset, size_t count) {
    Span result = {NULL, 0, 0};

    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_subspan.\n");
        #endif 
        return result; // Handle NULL span pointer
    }
    if (offset * span->elemSize >= span->size || count == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Offset out of bounds or count is zero in span_subspan.\n");
        #endif 
        return result; // Handle out-of-bounds offset or zero count
    }
    if (offset * span->elemSize + count * span->elemSize > span->size) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Subspan exceeds Span bounds in span_subspan.\n");
        #endif 
        return result; // Handle subspan exceeding span bounds
    }

    result.data = (char*)span->data + offset * span->elemSize;
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    return result;
}

/**
 * @brief Compares two spans for equality.
 * 
 * This function checks if two spans are equal by comparing their sizes and data.
 * If either span is NULL, it returns false.
 * 
 * @param span1 Pointer to the first Span structure.
 * @param span2 Pointer to the second Span structure.
 * 
 * @return true if the spans are equal, false otherwise.
 */
bool span_is_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_equal.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    if (span1->size != span2->size) {
        return false; // Different sizes imply inequality
    }
    return memcmp(span1->data, span2->data, span1->size) == 0;
}

/**
 * @brief Compares if one span is less than another span.
 * 
 * This function compares the data of two spans up to the size of the smaller span.
 * If the data is identical up to that point, the function compares their sizes.
 * 
 * @param span1 Pointer to the first Span structure.
 * @param span2 Pointer to the second Span structure.
 * 
 * @return true if span1 is less than span2, false otherwise.
 */
bool span_is_less(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_less.\n");
        #endif 
        return false; // Handle NULL span pointers
    }

    size_t minSize = (span1->size < span2->size) ? span1->size : span2->size;
    int result = memcmp(span1->data, span2->data, minSize);

    return result < 0 || (result == 0 && span1->size < span2->size);
}

/**
 * @brief Compares if one span is greater than another span.
 * 
 * This function determines if span1 is greater than span2 by checking the inverse 
 * of span_is_less.
 * 
 * @param span1 Pointer to the first Span structure.
 * @param span2 Pointer to the second Span structure.
 * 
 * @return true if span1 is greater than span2, false otherwise.
 */
bool span_is_greater(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_greater.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    return span_is_less(span2, span1);
}

/**
 * @brief Compares two spans for inequality.
 * 
 * This function checks if two spans are not equal by negating the result of span_is_equal.
 * If either span is NULL, it returns true if they are different, false otherwise.
 * 
 * @param span1 Pointer to the first Span structure.
 * @param span2 Pointer to the second Span structure.
 * 
 * @return true if the spans are not equal, false otherwise.
 */
bool span_is_not_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_not_equal.\n");
        #endif 
        return span1 != span2; // If one is NULL and the other is not, they are not equal
    }
    return !span_is_equal(span1, span2);    
}

/**
 * @brief Compares if one span is greater than or equal to another span.
 * 
 * This function checks if span1 is greater than or equal to span2 by using the 
 * results of span_is_greater and span_is_equal.
 * 
 * @param span1 Pointer to the first Span structure.
 * @param span2 Pointer to the second Span structure.
 * 
 * @return true if span1 is greater than or equal to span2, false otherwise.
 */
bool span_is_greater_or_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_greater_or_equal.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    return span_is_greater(span1, span2) || span_is_equal(span1, span2);
}

/**
 * @brief Compares if one span is less than or equal to another span.
 * 
 * This function checks if span1 is less than or equal to span2 by using the 
 * results of span_is_less and span_is_equal.
 * 
 * @param span1 Pointer to the first Span structure.
 * @param span2 Pointer to the second Span structure.
 * 
 * @return true if span1 is less than or equal to span2, false otherwise.
 */
bool span_is_less_or_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_less_or_equal.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    return span_is_less(span1, span2) || span_is_equal(span1, span2);
}

/**
 * @brief Returns an iterator to the beginning of the span.
 * 
 * This function provides a pointer to the first element in the span. 
 * If the span is NULL, it returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the first element in the span, or NULL if the span is NULL.
 */
void* span_begin(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_begin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span ? span->data : NULL;
}

/**
 * @brief Returns a constant iterator to the beginning of the span.
 * 
 * Provides a constant pointer to the first element in the span.
 * If the span is NULL, it returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Constant pointer to the first element in the span, or NULL if the span is NULL.
 */
const void* span_cbegin(const Span* span){
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_cbegin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_begin((Span*)span);
}

/**
 * @brief Returns an iterator to the end of the span.
 * 
 * Provides a pointer to the position just after the last element in the span.
 * If the span is NULL, it returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the position after the last element in the span, or NULL if the span is NULL.
 */
void* span_end(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_end.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span ? (char*)span->data + span->size: NULL;
}

/**
 * @brief Returns a constant iterator to the end of the span.
 * 
 * Provides a constant pointer to the position just after the last element in the span.
 * If the span is NULL, it returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Constant pointer to the position after the last element in the span, or NULL if the span is NULL.
 */
const void* span_cend(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_cend.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_end((Span*)span);
}

/**
 * @brief Returns a reverse iterator to the last element in the span.
 * 
 * Provides a pointer to the last element in the span for reverse iteration.
 * If the span is empty or NULL, it returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the last element in the span, or NULL if the span is empty or NULL.
 */
void* span_rbegin(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_rbegin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    if (span->size == 0) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span is empty in span_rbegin.\n");
        #endif 
        return NULL; // Handle empty span
    }
    return span ? (char*)span->data + span->size - span->elemSize: NULL;
}

/**
 * @brief Returns a reverse constant iterator to the last element in the span.
 * 
 * Provides a constant pointer to the last element in the span for reverse iteration.
 * If the span is NULL, returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to the last element in the span, or NULL if the span is NULL.
 */
const void* span_crbegin(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_crbegin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_rbegin((Span*)span);
}

/**
 * @brief Returns a reverse iterator to one element before the first element in the span.
 * 
 * Provides a pointer that points to one element before the first element in the span.
 * This can be used to mark the end of a reverse iteration. If the span is NULL, returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to one element before the first element in the span, or NULL if the span is NULL.
 */
void* span_rend(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_rend.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span? (char*)span->data - span->elemSize: NULL;
}

/**
 * @brief Returns a reverse constant iterator to one element before the first element in the span.
 * 
 * Provides a constant pointer that points to one element before the first element in the span.
 * This can be used to mark the end of a reverse iteration. If the span is NULL, returns NULL.
 * 
 * @param span Pointer to the Span structure.
 * @return Pointer to one element before the first element in the span, or NULL if the span is NULL.
 */
const void* span_crend(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_crend.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_rend((Span*)span);
}

/**
 * @brief Increments a pointer within the span.
 * 
 * Advances the pointer to the next element within the span, taking into account
 * the element size. Ensures that the resulting pointer does not exceed the span's bounds.
 * 
 * @param span Pointer to the Span structure.
 * @param ptr Current pointer within the span.
 * 
 * @return Pointer to the next element, or NULL if out of bounds.
 */
void* span_increment(Span* span, void* ptr) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_increment.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    if (!ptr) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Pointer is NULL in span_increment.\n");
        #endif 
        return NULL; // Handle NULL pointer
    }
    if ((char*)ptr + span->elemSize > (char*)span->data + span->size) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Incrementing would move pointer out of Span bounds in span_increment.\n");
        #endif 
        return NULL; // Prevent going out of bounds
    }
    return (char*)ptr + span->elemSize;
}

/**
 * @brief Decrements a pointer within the span.
 * 
 * Moves the pointer to the previous element within the span, based on the element size.
 * Ensures that the resulting pointer does not fall below the span's starting address.
 * 
 * @param span Pointer to the Span structure.
 * @param ptr Current pointer within the span.
 * @return Pointer to the previous element, or NULL if out of bounds.
 */
void* span_decrement(Span* span, void* ptr) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_decrement.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    if (!ptr) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Pointer is NULL in span_decrement.\n");
        #endif 
        return NULL; // Handle NULL pointer
    }
    if ((char*)ptr <= (char*)span->data - span->elemSize) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Decrementing would move pointer before Span in span_decrement.\n");
        #endif 
        return NULL; // Out of bounds or invalid arguments
    }
    return (char*)ptr - span->elemSize;
}

