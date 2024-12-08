/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Span
*/

#include <stdlib.h>
#include <string.h>
#include "span.h"


typedef struct MemoryPoolSpan {
    void *pool;
    size_t poolSize;
    size_t used;
} MemoryPoolSpan;

static MemoryPoolSpan *global_span_pool = NULL;

/**
 * @brief Initializes the span memory pool with the given size.
 */
static void initialize_span_memory_pool(size_t size) {
    SPAN_LOG("[initialize_span_memory_pool]: Entering with size: %zu", size);

    if (global_span_pool != NULL) {
        SPAN_LOG("[initialize_span_memory_pool]: Memory pool already initialized");
        return; // Already initialized
    }

    global_span_pool = (MemoryPoolSpan*)malloc(sizeof(MemoryPoolSpan));
    if (!global_span_pool) {
        SPAN_LOG("[initialize_span_memory_pool]: Error: Failed to allocate memory for span pool structure");
        exit(-1); // Handle memory allocation failure for pool structure
    }

    global_span_pool->pool = malloc(size);
    if (!global_span_pool->pool) {
        free(global_span_pool);
        SPAN_LOG("[initialize_span_memory_pool]: Error: Failed to allocate memory for span pool data");
        exit(-1); // Handle memory allocation failure for pool data
    }

    global_span_pool->poolSize = size;
    global_span_pool->used = 0;

    SPAN_LOG("[initialize_span_memory_pool]: Memory pool initialized successfully with size: %zu", size);
}

/**
 * @brief Destroys the span memory pool.
 */
static void destroy_span_memory_pool() {
    SPAN_LOG("[destroy_span_memory_pool]: Entering");

    if (!global_span_pool) {
        SPAN_LOG("[destroy_span_memory_pool]: Error: Memory pool is NULL");
        return; // Handle uninitialized memory pool
    }

    free(global_span_pool->pool);
    free(global_span_pool);
    global_span_pool = NULL;

    SPAN_LOG("[destroy_span_memory_pool]: Memory pool destroyed successfully");
}

/**
 * @brief Allocates memory from the span memory pool.
 */
static void* memory_pool_span_allocate(size_t size) {
    SPAN_LOG("[memory_pool_span_allocate]: Entering with size: %zu", size);

    if (!global_span_pool) {
        SPAN_LOG("[memory_pool_span_allocate]: Error: Memory pool is not initialized");
        return NULL; 
    }
    if (global_span_pool->used + size > global_span_pool->poolSize) {
        SPAN_LOG("[memory_pool_span_allocate]: Error: Not enough space in span pool");
        return NULL; 
    }

    void* allocated = (char*)global_span_pool->pool + global_span_pool->used;
    global_span_pool->used += size;

    SPAN_LOG("[memory_pool_span_allocate]: Memory allocated at address: %p, new used size: %zu", (void*)allocated, global_span_pool->used);
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
    SPAN_LOG("[span_create]: Entering with data: %p, elemCount: %zu, elemSize: %zu", (void*)data, elemCount, elemSize);

    if (!data) {
        SPAN_LOG("[span_create]: Error: Null data provided");
        exit(-1); 
    }
    if (elemCount == 0 || elemSize == 0) {
        SPAN_LOG("[span_create]: Error: Element count or size is zero");
        exit(-1); 
    }

    if (!global_span_pool) {
        SPAN_LOG("[span_create]: Initializing memory pool");
        initialize_span_memory_pool(10 * 10); // Initialize memory pool
    }

    Span* newSpan = (Span*)malloc(sizeof(Span));
    if (!newSpan) {
        SPAN_LOG("[span_create]: Error: Failed to allocate memory for Span structure");
        exit(-1); 
    }

    newSpan->data = memory_pool_span_allocate(elemCount * elemSize);
    if (!newSpan->data) {
        SPAN_LOG("[span_create]: Error: Failed to allocate memory for Span data");
        free(newSpan);
        exit(-1); 
    }

    memcpy(newSpan->data, data, elemCount * elemSize);
    newSpan->size = elemCount * elemSize;
    newSpan->elemSize = elemSize;

    SPAN_LOG("[span_create]: Span created successfully with size: %zu, elemSize: %zu", newSpan->size, newSpan->elemSize);
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
    SPAN_LOG("[span_destroy]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_destroy]: Error: Span pointer is NULL");
        return; 
    }

    span->data = NULL;
    span->size = 0;
    free(span);

    SPAN_LOG("[span_destroy]: Span destroyed and memory pool cleanup initiated");
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
    SPAN_LOG("[span_at]: Entering with span: %p, index: %zu", (void*)span, index);

    if (!span) {
        SPAN_LOG("[span_at]: Error: Span pointer is NULL");
        return NULL; 
    }
    if (index < span->size / span->elemSize) {
        void* result = (char*)span->data + (index * span->elemSize);
        SPAN_LOG("[span_at]: Returning pointer to element at index %zu: %p", index, (void*)result);
        return result;
    }

    SPAN_LOG("[span_at]: Error: Index out of bounds");
    return NULL; 
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
    SPAN_LOG("[span_size]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_size]: Error: Span is NULL");
        return 0; 
    }

    SPAN_LOG("[span_size]: Returning span size: %zu", span->size);
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
    SPAN_LOG("[span_front]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_front]: Error: Span pointer is NULL");
        return NULL; 
    }
    if (span->size == 0) {
        SPAN_LOG("[span_front]: Error: Span is empty");
        return NULL; 
    }

    SPAN_LOG("[span_front]: Returning pointer to first element: %p", (void*)(span->data));
    return span->data; 
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
    SPAN_LOG("[span_back]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_back]: Error: Span pointer is NULL");
        return NULL; 
    }
    if (span->size == 0) {
        SPAN_LOG("[span_back]: Error: Span is empty");
        return NULL; 
    }

    void* lastElement = (char*)span->data + (span->size - span->elemSize);
    SPAN_LOG("[span_back]: Returning pointer to last element: %p", (void*)lastElement);

    return lastElement; 
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
    SPAN_LOG("[span_data]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_data]: Error: Span pointer is NULL");
        return NULL; 
    }

    SPAN_LOG("[span_data]: Returning span data: %p", (void*)(span->data));
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
    SPAN_LOG("[span_cdata]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_cdata]: Error: Span pointer is NULL");
        return NULL;
    }

    SPAN_LOG("[span_cdata]: Returning constant pointer to span data: %p", (void*)(span->data));
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
    SPAN_LOG("[span_empty]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_empty]: Error: Span pointer is NULL, treating as empty");
        return true;
    }

    bool result = span->size == 0;
    SPAN_LOG("[span_empty]: Span is %s", result ? "empty" : "not empty");

    return result;
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
    SPAN_LOG("[span_size_bits]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_size_bits]: Error: Span is NULL");
        return 0; 
    }

    size_t sizeInBits = span->size * span->elemSize;
    SPAN_LOG("[span_size_bits]: Span size in bits: %zu", sizeInBits);

    return sizeInBits;
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
    SPAN_LOG("[span_first]: Entering with span: %p, count: %zu", (void*)span, count);

    Span result = {NULL, 0, 0};

    if (!span) {
        SPAN_LOG("[span_first]: Error: Span pointer is NULL");
        return result; 
    }
    if (count == 0 || span->size == 0) {
        SPAN_LOG("[span_first]: Error: Count is zero or Span is empty");
        return result; 
    }
    if (count * span->elemSize > span->size) {
        SPAN_LOG("[span_first]: Error: Count exceeds Span size");
        return result; 
    }

    result.data = span->data;
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    SPAN_LOG("[span_first]: Subspan created with size: %zu, elemSize: %zu", result.size, result.elemSize);
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
    SPAN_LOG("[span_last]: Entering with span: %p, count: %zu", (void*)span, count);

    Span result = {NULL, 0, 0};

    if (!span) {
        SPAN_LOG("[span_last]: Error: Span pointer is NULL");
        return result; 
    }
    if (count == 0 || span->size == 0) {
        SPAN_LOG("[span_last]: Error: Count is zero or Span is empty");
        return result;
    }
    if (count * span->elemSize > span->size) {
        SPAN_LOG("[span_last]: Error: Count exceeds Span size");
        return result; 
    }

    result.data = (char*)span->data + (span->size - count * span->elemSize);
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    SPAN_LOG("[span_last]: Subspan created with size: %zu, elemSize: %zu", result.size, result.elemSize);
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
    SPAN_LOG("[span_subspan]: Entering with span: %p, offset: %zu, count: %zu", (void*)span, offset, count);

    Span result = {NULL, 0, 0};

    if (!span) {
        SPAN_LOG("[span_subspan]: Error: Span pointer is NULL");
        return result; 
    }
    if (offset * span->elemSize >= span->size || count == 0) {
        SPAN_LOG("[span_subspan]: Error: Offset out of bounds or count is zero");
        return result; 
    }
    if (offset * span->elemSize + count * span->elemSize > span->size) {
        SPAN_LOG("[span_subspan]: Error: Subspan exceeds Span bounds");
        return result; 
    }

    result.data = (char*)span->data + offset * span->elemSize;
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    SPAN_LOG("[span_subspan]: Subspan created with data: %p, size: %zu, elemSize: %zu", (void*)(result.data), result.size, result.elemSize);
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
    SPAN_LOG("[span_is_equal]: Entering with span1: %p, span2: %p", (void*)span1, (void*)span2);

    if (!span1 || !span2) {
        SPAN_LOG("[span_is_equal]: Error: One or both Span pointers are NULL");
        return false; 
    }
    if (span1->size != span2->size) {
        SPAN_LOG("[span_is_equal]: Spans are not equal (different sizes)");
        return false; 
    }

    bool result = memcmp(span1->data, span2->data, span1->size) == 0;
    SPAN_LOG("[span_is_equal]: Spans are %s", result ? "equal" : "not equal");

    return result;
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
    SPAN_LOG("[span_is_less]: Entering with span1: %p, span2: %p", (void*)span1, (void*)span2);

    if (!span1 || !span2) {
        SPAN_LOG("[span_is_less]: Error: One or both Span pointers are NULL");
        return false; 
    }

    size_t minSize = (span1->size < span2->size) ? span1->size : span2->size;
    int result = memcmp(span1->data, span2->data, minSize);

    bool comparisonResult = result < 0 || (result == 0 && span1->size < span2->size);
    SPAN_LOG("[span_is_less]: Comparison result: %s", comparisonResult ? "true" : "false");

    return comparisonResult;
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
    SPAN_LOG("[span_is_greater]: Entering with span1: %p, span2: %p", (void*)span1, (void*)span2);

    if (!span1 || !span2) {
        SPAN_LOG("[span_is_greater]: Error: One or both Span pointers are NULL");
        return false;
    }

    bool result = span_is_less(span2, span1);
    SPAN_LOG("[span_is_greater]: Comparison result: %s", result ? "true" : "false");

    return result;
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
    SPAN_LOG("[span_is_not_equal]: Entering with span1: %p, span2: %p", (void*)span1, (void*)span2);

    if (!span1 || !span2) {
        SPAN_LOG("[span_is_not_equal]: Error: One or both Span pointers are NULL");
        return span1 != span2; 
    }

    bool result = !span_is_equal(span1, span2);
    SPAN_LOG("[span_is_not_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
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
    SPAN_LOG("[span_is_greater_or_equal]: Entering with span1: %p, span2: %p", (void*)span1, (void*)span2);

    if (!span1 || !span2) {
        SPAN_LOG("[span_is_greater_or_equal]: Error: One or both Span pointers are NULL");
        return false; 
    }

    bool result = span_is_greater(span1, span2) || span_is_equal(span1, span2);
    SPAN_LOG("[span_is_greater_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
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
    SPAN_LOG("[span_is_less_or_equal]: Entering with span1: %p, span2: %p", (void*)span1, (void*)span2);

    if (!span1 || !span2) {
        SPAN_LOG("[span_is_less_or_equal]: Error: One or both Span pointers are NULL");
        return false; 
    }

    bool result = span_is_less(span1, span2) || span_is_equal(span1, span2);
    SPAN_LOG("[span_is_less_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
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
    SPAN_LOG("[span_begin]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_begin]: Error: Span pointer is NULL");
        return NULL; 
    }

    void* result = span->data;
    SPAN_LOG("[span_begin]: Returning iterator to beginning: %p", (void*)result);

    return result;
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
const void* span_cbegin(const Span* span) {
    SPAN_LOG("[span_cbegin]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_cbegin]: Error: Span pointer is NULL");
        return NULL; 
    }

    const void* result = span_begin((Span*)span);
    SPAN_LOG("[span_cbegin]: Returning constant iterator to beginning: %p", (void*)result);

    return result;
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
    SPAN_LOG("[span_end]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_end]: Error: Span pointer is NULL");
        return NULL; 
    }

    void* result = (char*)span->data + span->size;
    SPAN_LOG("[span_end]: Returning iterator to end: %p", (void*)result);

    return result;
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
    SPAN_LOG("[span_cend]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_cend]: Error: Span pointer is NULL");
        return NULL; 
    }

    const void* result = span_end((Span*)span);
    SPAN_LOG("[span_cend]: Returning constant iterator to end: %p", (void*)result);

    return result;
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
    SPAN_LOG("[span_rbegin]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_rbegin]: Error: Span pointer is NULL");
        return NULL; // Handle NULL span pointer
    }
    if (span->size == 0) {
        SPAN_LOG("[span_rbegin]: Error: Span is empty");
        return NULL; // Handle empty span
    }

    void* result = (char*)span->data + span->size - span->elemSize;
    SPAN_LOG("[span_rbegin]: Returning reverse iterator: %p", (void*)result);

    return result;
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
    SPAN_LOG("[span_crbegin]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_crbegin]: Error: Span pointer is NULL");
        return NULL; // Handle NULL span pointer
    }

    const void* result = span_rbegin((Span*)span);
    SPAN_LOG("[span_crbegin]: Returning constant reverse iterator: %p", (void*)result);

    return result;
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
    SPAN_LOG("[span_rend]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_rend]: Error: Span pointer is NULL");
        return NULL; // Handle NULL span pointer
    }

    void* result = (char*)span->data - span->elemSize;
    SPAN_LOG("[span_rend]: Returning reverse end iterator: %p", (void*)result);
    return result;
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
    SPAN_LOG("[span_crend]: Entering with span: %p", (void*)span);

    if (!span) {
        SPAN_LOG("[span_crend]: Error: Span pointer is NULL");
        return NULL; // Handle NULL span pointer
    }

    const void* result = span_rend((Span*)span);
    SPAN_LOG("[span_crend]: Returning constant reverse end iterator: %p", (void*)result);

    return result;
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
    SPAN_LOG("[span_increment]: Entering with span: %p, ptr: %p", (void*)span, (void*)ptr);

    if (!span) {
        SPAN_LOG("[span_increment]: Error: Span pointer is NULL");
        return NULL; // Handle NULL span pointer
    }
    if (!ptr) {
        SPAN_LOG("[span_increment]: Error: Pointer is NULL");
        return NULL; // Handle NULL pointer
    }
    if ((char*)ptr + span->elemSize > (char*)span->data + span->size) {
        SPAN_LOG("[span_increment]: Error: Incrementing would move pointer out of Span bounds");
        return NULL; // Prevent going out of bounds
    }

    void* new_ptr = (char*)ptr + span->elemSize;
    SPAN_LOG("[span_increment]: Incremented pointer to: %p", (void*)new_ptr);
    return new_ptr;
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
    SPAN_LOG("[span_decrement]: Entering with span: %p, ptr: %p", (void*)span, (void*)ptr);

    if (!span) {
        SPAN_LOG("[span_decrement]: Error: Span pointer is NULL");
        return NULL; // Handle NULL span pointer
    }
    if (!ptr) {
        SPAN_LOG("[span_decrement]: Error: Pointer is NULL");
        return NULL; // Handle NULL pointer
    }
    if ((char*)ptr <= (char*)span->data - span->elemSize) {
        SPAN_LOG("[span_decrement]: Error: Decrementing would move pointer before Span");
        return NULL; // Out of bounds or invalid arguments
    }

    void* new_ptr = (char*)ptr - span->elemSize;
    SPAN_LOG("[span_decrement]: Decremented pointer to: %p", (void*)new_ptr);
    return new_ptr;
}

