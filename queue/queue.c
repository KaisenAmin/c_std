#include "queue.h"
#include <stdlib.h>

static bool queue_empty_impl(const Queue* q);
static size_t queue_size_impl(const Queue* q);
static void queue_push_impl(Queue* q, void* item);
static void* queue_front_impl(const Queue* q);
static void* queue_back_impl(const Queue* q);
static void queue_pop_impl(Queue* q);
static void queue_emplace_impl(Queue* q, void* item, size_t itemSize); // Emplace function
static void queue_swap_impl(Queue* q1, Queue* q2);
static void queue_deallocate_impl(Queue* q); 
static bool queue_is_equal_impl(const Queue* q1, const Queue* q2);
static bool queue_is_less_impl(const Queue* q1, const Queue* q2);
static bool queue_is_greater_impl(const Queue* q1, const Queue* q2);
static bool queue_is_not_equal_impl(const Queue* q1, const Queue* q2);
static bool queue_is_less_or_equal_impl(const Queue* q1, const Queue* q2);
static bool queue_is_greater_or_equal_impl(const Queue* q1, const Queue* q2);

Queue* queue_create(size_t itemSize) 
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));

    if (!queue) 
        return NULL; // Handle allocation failure
    
    queue->vec = vector_create(itemSize);
    if (!queue->vec) 
    {
        free(queue);
        return NULL; 
    }

    // Assign function pointers
    queue->empty = queue_empty_impl;
    queue->size = queue_size_impl;
    queue->push = queue_push_impl;
    queue->front = queue_front_impl;
    queue->back = queue_back_impl;
    queue->pop = queue_pop_impl;
    queue->emplace = queue_emplace_impl;
    queue->swap = queue_swap_impl;
    queue->deallocate = queue_deallocate_impl;
    queue->is_equal = queue_is_equal_impl;
    queue->is_greater = queue_is_greater_impl;
    queue->is_greater_or_equal = queue_is_greater_or_equal_impl;
    queue->is_less = queue_is_less_impl;
    queue->is_less_or_equal = queue_is_less_or_equal_impl;
    queue->is_not_equal = queue_is_not_equal_impl;

    return queue;
}


static bool queue_empty_impl(const Queue* q) 
{
    if (!q || !q->vec) 
        return true; // Treat NULL queue as empty
    
    return q->vec->is_empty(q->vec);
}

static size_t queue_size_impl(const Queue* q) 
{
    if (!q || !q->vec) 
        return 0; // Treat NULL queue as having size 0
    
    return q->vec->length(q->vec);
}

static void queue_push_impl(Queue* q, void* item) 
{
    if (!q || !q->vec || !item) 
        return; // Handle null queue or item
    
    q->vec->push_back(q->vec, item);
}

static void* queue_front_impl(const Queue* q) 
{
    if (!q || !q->vec || q->vec->is_empty(q->vec)) 
        return NULL; // Handle empty or null queue
    
    return q->vec->front(q->vec);
}

static void* queue_back_impl(const Queue* q)
{
    if (!q || !q->vec || q->vec->is_empty(q->vec)) 
        return NULL; // Handle empty or null queue
    
    return q->vec->back(q->vec);
}

static void queue_pop_impl(Queue* q) 
{
    if (!q || !q->vec || q->vec->is_empty(q->vec)) 
        return; // Handle empty or null queue
    
    q->vec->erase(q->vec, 0, 1);
}

static void queue_emplace_impl(Queue* q, void* item, size_t itemSize) 
{
    if (!q || !q->vec || !item) 
        return; // Handle null queue or item
    
    q->vec->emplace_back(q->vec, item, itemSize);
}

static void queue_swap_impl(Queue* q1, Queue* q2)
{
    if (!q1 || !q2) 
        return; // Handle null queues

    Vector* tempVec = q1->vec;
    q1->vec = q2->vec;
    q2->vec = tempVec;
}

static void queue_deallocate_impl(Queue* q)
{
    if (!q) 
        return; // Nothing to deallocate

    if (q->vec) 
        q->vec->deallocate(q->vec); // Deallocate the underlying vector

    free(q); // Free the queue itself
}

static bool queue_is_equal_impl(const Queue* q1, const Queue* q2) 
{
    if (!q1 || !q2) 
        return q1 == q2;
    
    return q1->vec->is_equal(q1->vec, q2->vec);
}

static bool queue_is_less_impl(const Queue* q1, const Queue* q2) 
{
    if (!q1 || !q2) 
        return q1 != NULL && q2 == NULL; // NULL is considered less
    
    return q1->vec->is_less(q1->vec, q2->vec);
}

static bool queue_is_greater_impl(const Queue* q1, const Queue* q2) 
{
    if (!q1 || !q2) 
        return q1 == NULL && q2 != NULL; // NULL is considered less
    
    return q1->vec->is_greater(q1->vec, q2->vec);
}

static bool queue_is_not_equal_impl(const Queue* q1, const Queue* q2) 
{
    return !queue_is_equal_impl(q1, q2);
}

static bool queue_is_less_or_equal_impl(const Queue* q1, const Queue* q2) 
{
    return queue_is_less_impl(q1, q2) || queue_is_equal_impl(q1, q2);
}

static bool queue_is_greater_or_equal_impl(const Queue* q1, const Queue* q2) 
{
    return queue_is_greater_impl(q1, q2) || queue_is_equal_impl(q1, q2);
}
