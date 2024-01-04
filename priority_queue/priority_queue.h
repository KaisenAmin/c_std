#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include "../vector/vector.h"
#include <stdbool.h>

typedef struct PriorityQueue PriorityQueue;

struct PriorityQueue{
    Vector* vec;
    int (*compare)(const void* a, const void* b); // Comparison function
};

PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*));
bool priority_queue_empty(const PriorityQueue* pq);
size_t priority_queue_size(const PriorityQueue* pq);
void priority_queue_push(PriorityQueue* pq, void* item);
void* priority_queue_top(const PriorityQueue* pq);
void priority_queue_pop(PriorityQueue* pq);
void priority_queue_deallocate(PriorityQueue* pq);
void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2);

#endif 