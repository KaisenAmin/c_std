#ifndef QUEUE_H_
#define QUEUE_H_

#include "../vector/vector.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct Queue Queue;

struct Queue 
{
    Vector* vec;

    bool (*empty)(const Queue* queue);
    size_t (*size)(const Queue* queue);
    void (*push)(Queue* queue, void* item);
    void* (*front)(const Queue* queue);
    void* (*back)(const Queue* queue);
    void (*pop)(Queue* queue);
    void (*emplace)(Queue* queue, void* item, size_t itemSize); // Emplace function
    void (*swap)(Queue* q1, Queue* q2);
    void (*deallocate)(Queue* queue);

    bool (*is_equal)(const Queue* queue1, const Queue* queue2);
    bool (*is_less)(const Queue* queue1, const Queue* queue2);
    bool (*is_greater)(const Queue* queue1, const Queue* queue2);
    bool (*is_not_equal)(const Queue* queue1, const Queue* queue2);
    bool (*is_less_or_equal)(const Queue* queue1, const Queue* queue2);
    bool (*is_greater_or_equal)(const Queue* queue1, const Queue* queue2);
};

Queue* queue_create(size_t itemSize);


#endif 