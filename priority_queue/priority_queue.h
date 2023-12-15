#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include "../vector/vector.h"
#include <stdbool.h>
#include <stdbool.h>

typedef struct PriorityQueue PriorityQueue;

struct PriorityQueue
{
    Vector* vec;
    int (*compare)(const void* a, const void* b); // Comparison function

    bool (*empty)(const PriorityQueue* pq);
    size_t (*size)(const PriorityQueue* pq);
    void (*push)(PriorityQueue* pq, void* item);
    void* (*top)(const PriorityQueue* pq);
    void (*pop)(PriorityQueue* pq);
    void (*deallocate)(PriorityQueue* pq);
};

PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*));


#endif 