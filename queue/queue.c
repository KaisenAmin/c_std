#include "queue.h"
#include <stdlib.h>

Queue* queue_create(size_t itemSize) 
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));

    if (!queue) 
    {
       perror("Can not allocate memory for queue");
       exit(-1); // Handle allocation failure
    }
        
    
    queue->vec = vector_create(itemSize);
    if (!queue->vec) 
    {
        free(queue);
        exit(-1); 
    }

    return queue;
}


bool queue_empty(const Queue* q) 
{
    if (!q || !q->vec) 
        return true; // Treat NULL queue as empty
    
    return vector_is_empty(q->vec);
}

size_t queue_size(const Queue* q) 
{
    if (!q || !q->vec) 
        return 0; // Treat NULL queue as having size 0
    
    return vector_size(q->vec);
}

void queue_push(Queue* q, void* item) 
{
    if (!q || !q->vec || !item) 
        return; // Handle null queue or item
    
    vector_push_back(q->vec, item);
}

void* queue_front(const Queue* q) 
{
    if (!q || !q->vec || vector_is_empty(q->vec)) 
        return NULL; // Handle empty or null queue
    
    return vector_front(q->vec);
}

void* queue_back(const Queue* q)
{
    if (!q || !q->vec || vector_is_empty(q->vec)) 
        return NULL; // Handle empty or null queue
    
    return vector_back(q->vec);
}

void queue_pop(Queue* q) 
{
    if (!q || !q->vec || vector_is_empty(q->vec)) 
        return; // Handle empty or null queue
    
    vector_erase(q->vec, 0, 1);
}

void queue_emplace(Queue* q, void* item, size_t itemSize) 
{
    if (!q || !q->vec || !item) 
        return; // Handle null queue or item
    
    vector_emplace_back(q->vec, item, itemSize);
}

void queue_swap(Queue* q1, Queue* q2)
{
    if (!q1 || !q2) 
        return; // Handle null queues

    Vector* tempVec = q1->vec;
    q1->vec = q2->vec;
    q2->vec = tempVec;
}

void queue_deallocate(Queue* q)
{
    if (!q) 
        return; // Nothing to deallocate

    if (q->vec) 
        vector_deallocate(q->vec); // Deallocate the underlying vector

    free(q); // Free the queue itself
}

bool queue_is_equal(const Queue* q1, const Queue* q2) 
{
    if (!q1 || !q2) 
        return q1 == q2;
    
    return vector_is_equal(q1->vec, q2->vec);
}

bool queue_is_less(const Queue* q1, const Queue* q2) 
{
    if (!q1 || !q2) 
        return q1 != NULL && q2 == NULL; // NULL is considered less
    
    return vector_is_less(q1->vec, q2->vec);
}

bool queue_is_greater(const Queue* q1, const Queue* q2) 
{
    if (!q1 || !q2) 
        return q1 == NULL && q2 != NULL; // NULL is considered less
    
    return vector_is_greater(q1->vec, q2->vec);
}

bool queue_is_not_equal(const Queue* q1, const Queue* q2) 
{
    return !queue_is_equal(q1, q2);
}

bool queue_is_less_or_equal(const Queue* q1, const Queue* q2) 
{
    return queue_is_less(q1, q2) || queue_is_equal(q1, q2);
}

bool queue_is_greater_or_equal(const Queue* q1, const Queue* q2) 
{
    return queue_is_greater(q1, q2) || queue_is_equal(q1, q2);
}
