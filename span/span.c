#include "span.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct MemoryPoolSpan 
{
    void *pool;
    size_t poolSize;
    size_t used;

} MemoryPoolSpan;

static MemoryPoolSpan *global_span_pool = NULL;

static void initialize_span_memory_pool(size_t size) 
{
    if (global_span_pool != NULL) 
        return; // Already initialized

    global_span_pool = (MemoryPoolSpan*)malloc(sizeof(MemoryPoolSpan));
    if (!global_span_pool) 
    {
        perror("Failed to allocate memory for span pool");
        exit(-1);
    }

    global_span_pool->pool = malloc(size);
    if (!global_span_pool->pool) 
    {
        free(global_span_pool);
        perror("Failed to allocate memory for span pool data");
        exit(-1);
    }

    global_span_pool->poolSize = size;
    global_span_pool->used = 0;
}

static void destroy_span_memory_pool() 
{
    if (global_span_pool) 
    {
        free(global_span_pool->pool);
        free(global_span_pool);

        global_span_pool = NULL;
    }
}

static void* memory_pool_span_allocate(size_t size) 
{
    if (!global_span_pool || global_span_pool->used + size > global_span_pool->poolSize) 
    {
        perror("Not enough space in span pool");
        return NULL;
    }

    void *allocated = (char*)global_span_pool->pool + global_span_pool->used;
    global_span_pool->used += size;

    return allocated;
}

Span* span_create(void* data, size_t elemCount, size_t elemSize) 
{
    if (!data) 
    {
        perror("Null data provided to span_create");
        return NULL;
    }

    if (!global_span_pool) 
        initialize_span_memory_pool(10 * 10); // For example, initialize with 1MB

    Span *newSpan = (Span*)malloc(sizeof(Span));
    if (!newSpan) 
    {
        perror("Failed to allocate memory for Span structure");
        exit(-1);
    }

    newSpan->data = memory_pool_span_allocate(elemCount * elemSize);
    if (!newSpan->data) 
    {
        perror("Failed to allocate memory for Span data");
        exit(-1);
    }

    memcpy(newSpan->data, data, elemCount * elemSize);
    newSpan->size = elemCount * elemSize;
    newSpan->elemSize = elemSize;

    return newSpan;
}

void span_destroy(Span* span) 
{
    if (span) 
    {
        // Reset the Span structure
        span->data = NULL;
        span->size = 0;
        free(span);
        // Consider destroying the memory pool if no longer needed
        destroy_span_memory_pool();
    }
}

void* span_at(Span* span, size_t index) 
{
    if (span && index < span->size / span->elemSize) 
        return (char*)span->data + (index * span->elemSize);

    return NULL; // Index out of bounds or span is NULL
}

size_t span_size(const Span* span) 
{
    if (!span) 
    {
        perror("span_size: Span is NULL");
        return 0;
    }
    return span->size;
}

void* span_front(Span* span) 
{
    if (span == NULL || span->size == 0) 
        return NULL; // Span is empty or NULL
    
    return span->data; // The first element is at the start of data
}

void* span_back(Span* span) 
{
    if (span == NULL || span->size == 0) 
        return NULL; // Span is empty or NULL

    return (char*)span->data + (span->size - span->elemSize);// Calculate the address of the last element
}

void* span_data(Span* span) 
{
    if (!span) 
    {
        perror("span_data: Span is NULL");
        return NULL;
    }
    return span->data;
}

const void* span_cdata(const Span* span) 
{
    if (!span) 
    {
        perror("span_cdata: Span is NULL");
        return NULL;
    }
    return span->data;
}

bool span_empty(const Span* span) {
    return (span == NULL || span->size == 0);
}

size_t span_size_bytes(const Span* span) 
{
    if (!span) 
    {
        perror("span_size_bytes: Span is NULL");
        return 0;
    }
    return span->size * span->elemSize;
}

Span span_first(Span* span, size_t count) 
{
    Span result = {NULL, 0, 0};

    if (!span || count == 0 || span->size == 0) 
        return result;

    result.data = span->data;
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    return result;
}

Span span_last(Span* span, size_t count) 
{
    Span result = {NULL, 0, 0};

    if (!span || count == 0 || span->size == 0) 
        return result;

    result.data = (char*)span->data + (span->size - count * span->elemSize);
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    return result;
}

Span span_subspan(Span* span, size_t offset, size_t count) 
{
    Span result = {NULL, 0, 0};

    if (!span || offset * span->elemSize >= span->size || count == 0) 
        return result;

    result.data = (char*)span->data + offset * span->elemSize;
    result.size = count * span->elemSize;
    result.elemSize = span->elemSize;

    return result;
}

bool span_is_equal(const Span* span1, const Span* span2) {
    if (span1->size != span2->size) {
        return false;
    }
    else {
        return memcmp(span1->data, span2->data, span1->size) == 0;
    }
}

bool span_is_less(const Span* span1, const Span* span2) {
    size_t minSize = (span1->size < span2->size) ? span1->size : span2->size;
    int result = memcmp(span1->data, span2->data, minSize);

    return result < 0 || (result == 0 && span1->size < span2->size);
}

bool span_is_greater(const Span* span1, const Span* span2) {
    return span_is_less(span2, span1);
}

bool span_is_not_equal(const Span* span1, const Span* span2) {
    return !span_is_equal(span1, span2);    
}

bool span_is_greater_or_equal(const Span* span1, const Span* span2) {
    return span_is_greater(span1, span2) || span_is_equal(span1, span2);
}

bool span_is_less_or_equal(const Span* span1, const Span* span2) {
    return span_is_less(span1, span2) || span_is_equal(span1, span2);
}

void* span_begin(Span* span) {
    return span ? span->data : NULL;
}

const void* span_cbegin(const Span* span){
    return span_begin((Span*)span);
}

void* span_end(Span* span) {
    return span ? (char*)span->data + span->size: NULL;
}

const void* span_cend(const Span* span) {
    return span_end((Span*)span);
}

void* span_rbegin(Span* span) {
    return span ? (char*)span->data + span->size - span->elemSize: NULL;
}

const void* span_crbegin(const Span* span) {
    return span_rbegin((Span*)span);
}

void* span_rend(Span* span) {
    return span? (char*)span->data - span->elemSize: NULL;
}

const void* span_crend(const Span* span) {
    return span_rend((Span*)span);
}

void* span_increment(Span* span, void* ptr) {
    if (!span || !ptr || (char*)ptr + span->elemSize > (char*)span->data + span->size) {
        return NULL; // Out of bounds or invalid arguments
    }
    return (char*)ptr + span->elemSize;
}

void* span_decrement(Span* span, void* ptr) {
    if (!span || !ptr || (char*)ptr <= (char*)span->data - span->elemSize) {
        return NULL; // Out of bounds or invalid arguments
    }
    return (char*)ptr - span->elemSize;
}

