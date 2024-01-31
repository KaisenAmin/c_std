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

size_t span_size(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span is NULL in span_size.\n");
        #endif 
        return 0;
    }
    return span->size;
}

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

void* span_data(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_data.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span->data;
}

const void* span_cdata(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_cdata.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span->data;
}

bool span_empty(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_empty.\n");
        #endif 
        return true; // Treat NULL span as empty
    }
    return span->size == 0;
}

size_t span_size_bits(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: span_size_bits: Span is NULL.\n");
        #endif 
        return 0;
    }
    return span->size * span->elemSize;
}

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

bool span_is_greater(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_greater.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    return span_is_less(span2, span1);
}

bool span_is_not_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_not_equal.\n");
        #endif 
        return span1 != span2; // If one is NULL and the other is not, they are not equal
    }
    return !span_is_equal(span1, span2);    
}

bool span_is_greater_or_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_greater_or_equal.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    return span_is_greater(span1, span2) || span_is_equal(span1, span2);
}

bool span_is_less_or_equal(const Span* span1, const Span* span2) {
    if (!span1 || !span2) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Span pointers are NULL in span_is_less_or_equal.\n");
        #endif 
        return false; // Handle NULL span pointers
    }
    return span_is_less(span1, span2) || span_is_equal(span1, span2);
}

void* span_begin(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_begin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span ? span->data : NULL;
}

const void* span_cbegin(const Span* span){
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_cbegin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_begin((Span*)span);
}

void* span_end(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_end.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span ? (char*)span->data + span->size: NULL;
}

const void* span_cend(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_cend.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_end((Span*)span);
}

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

const void* span_crbegin(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_crbegin.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_rbegin((Span*)span);
}

void* span_rend(Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_rend.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span? (char*)span->data - span->elemSize: NULL;
}

const void* span_crend(const Span* span) {
    if (!span) {
        #ifdef SPAN_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Span pointer is NULL in span_crend.\n");
        #endif 
        return NULL; // Handle NULL span pointer
    }
    return span_rend((Span*)span);
}

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

