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

    newSpan->data = memory_pool_span_allocate(elemCount);
    if (!newSpan->data) 
    {
        perror("Failed to allocate memory for Span data");
        exit(-1);
    }

    memcpy(newSpan->data, data, elemCount);
    newSpan->size = elemCount;
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