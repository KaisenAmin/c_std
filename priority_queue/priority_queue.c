#include "priority_queue.h"
#include <stdlib.h>
#include <string.h>

// static int priority_queue_compare_impl(const void* a, const void* b); 
static bool priority_queue_empty_impl(const PriorityQueue* pq);
static size_t priority_queue_size_impl(const PriorityQueue* pq);
static void priority_queue_push_impl(PriorityQueue* pq, void* item);
static void* priority_queue_top_impl(const PriorityQueue* pq);
static void priority_queue_pop_impl(PriorityQueue* pq);
static void priority_queue_deallocate_impl(PriorityQueue* pq);


PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*)) 
{
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));

    if (!pq) 
        return NULL; // Allocation failure
    
    pq->vec = vector_create(itemSize);

    if (!pq->vec) 
    {
        free(pq);
        return NULL; // Vector creation failure
    }

    pq->compare = compare;
    pq->empty = priority_queue_empty_impl;
    pq->size = priority_queue_size_impl;
    pq->push = priority_queue_push_impl;
    pq->top = priority_queue_top_impl;
    pq->pop = priority_queue_pop_impl;
    pq->deallocate = priority_queue_deallocate_impl;

    return pq;
}

// Helper function to swap two elements in the same vector
static void element_swap(Vector* vec, size_t index1, size_t index2) 
{
    if (index1 >= vec->size || index2 >= vec->size) 
        return; // Out of bounds check
    
    void* temp = malloc(vec->itemSize);
    if (!temp) 
        return; 
    
    // Swap elements at index1 and index2
    memcpy(temp, (char*)vec->items + index1 * vec->itemSize, vec->itemSize);
    memcpy((char*)vec->items + index1 * vec->itemSize, (char*)vec->items + index2 * vec->itemSize, vec->itemSize);
    memcpy((char*)vec->items + index2 * vec->itemSize, temp, vec->itemSize);

    free(temp);
}
// Function to maintain the heap property from bottom to top
static void heapify_up(Vector* vec, size_t index, int (*compare)(const void*, const void*)) 
{
    while (index > 0) 
    {
        size_t parent_index = (index - 1) / 2;
        void* parent = vec->at(vec, parent_index);
        void* current = vec->at(vec, index);

        if (compare(current, parent) > 0) 
        {  // Assuming a max heap
            element_swap(vec, index, parent_index);
            index = parent_index;
        } 
        else 
            break;
    }
}

// Function to maintain the heap property from top to bottom
static void heapify_down(Vector* vec, size_t index, int (*compare)(const void*, const void*)) 
{
    size_t size = vec->length(vec);
    size_t left_child_index, right_child_index, largest_index;

    while (index < size) 
    {
        largest_index = index;
        left_child_index = 2 * index + 1;
        right_child_index = 2 * index + 2;

        void* largest = vec->at(vec, largest_index);
        void* left_child = left_child_index < size ? vec->at(vec, left_child_index) : NULL;
        void* right_child = right_child_index < size ? vec->at(vec, right_child_index) : NULL;

        if (left_child && compare(left_child, largest) > 0) 
        {
            largest_index = left_child_index;
            largest = left_child;
        }

        if (right_child && compare(right_child, largest) > 0) 
            largest_index = right_child_index;
        
        if (largest_index != index) 
        {
            element_swap(vec, index, largest_index);
            index = largest_index;
        } 
        else 
            break;
    }
}

static bool priority_queue_empty_impl(const PriorityQueue* pq) 
{
    return pq->vec->is_empty(pq->vec);
}

static size_t priority_queue_size_impl(const PriorityQueue* pq) 
{
    return pq->vec->length(pq->vec);
}

static void priority_queue_push_impl(PriorityQueue* pq, void* item) 
{
    pq->vec->push_back(pq->vec, item);
    // Correctly calling the size method
    size_t currentSize = pq->vec->length(pq->vec);
    heapify_up(pq->vec, currentSize - 1, pq->compare);
}

static void* priority_queue_top_impl(const PriorityQueue* pq) 
{
    if (pq->empty(pq)) 
        return NULL; // Handle empty queue
    
    return pq->vec->front(pq->vec);
}

static void priority_queue_pop_impl(PriorityQueue* pq) 
{
    if (pq->empty(pq)) 
        return; // Handle empty queue
    
    size_t currentSize = pq->vec->length(pq->vec);

    element_swap(pq->vec, 0, currentSize - 1);
    pq->vec->pop_back(pq->vec);

    heapify_down(pq->vec, 0, pq->compare);
}

static void priority_queue_deallocate_impl(PriorityQueue* pq) 
{
    if (pq) 
    {
        if (pq->vec) 
            pq->vec->deallocate(pq->vec);
    
        free(pq);
    }
}