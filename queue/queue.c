/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Queue
*/


#include "queue.h"
#include "../fmt/fmt.h"
#include <stdlib.h>

Queue* queue_create(size_t itemSize) {
    if (itemSize == 0) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item size must be greater than zero in queue_create.\n");
        #endif 
        exit(-1); // Handle invalid item size
    }

    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        #ifdef QUEUE_LOGGING_ENABLE
            mt_fprintf(stderr, "Error: Cannot allocate memory for queue in queue_create.");
        #endif 
        exit(-1); // Handle allocation failure
    }
        
    queue->vec = vector_create(itemSize);
    if (!queue->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Cannot allocate memory for queue->vec in queue_create.");
        #endif 
        free(queue);
        exit(-1); // Handle allocation failure in vector creation
    }
    return queue;
}

bool queue_empty(const Queue* q) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_empty.\n");
        #endif 
        return true; // Treat NULL queue as empty
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue vector is NULL in queue_empty.\n");
        #endif 
        return true; // Treat queue with NULL vector as empty
    }
    return vector_is_empty(q->vec);
}

size_t queue_size(const Queue* q) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_size.\n");
        #endif 
        return 0; // Treat NULL queue as having size 0
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue vector is NULL in queue_size.\n");
        #endif 
        return 0; // Treat queue with NULL vector as having size 0
    }
    return vector_size(q->vec);
}

void queue_push(Queue* q, void* item) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue is NULL in queue_push.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector is NULL in queue_push.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    if (!item) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item to push is NULL in queue_push.\n");
        #endif 
        return; // Handle the error as per your application's needs
    }
    vector_push_back(q->vec, item);
}

void* queue_front(const Queue* q) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_front.\n");
        #endif 
        return NULL; // Handle NULL queue pointer
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue's vector is NULL in queue_front.\n");
        #endif 
        return NULL; // Handle NULL vector in queue
    }
    if (vector_is_empty(q->vec)) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue is empty in queue_front.\n");
        #endif 
        return NULL; // Handle empty queue
    }
    return vector_front(q->vec);
}

void* queue_back(const Queue* q){
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_back.\n");
        #endif 
        return NULL; // Handle NULL queue pointer
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue's vector is NULL in queue_back.\n");
        #endif 
        return NULL; // Handle NULL vector in queue
    }
    if (vector_is_empty(q->vec)) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue is empty in queue_back.\n");
        #endif 
        return NULL; // Handle empty queue
    }
    return vector_back(q->vec);
}

void queue_pop(Queue* q) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_pop.\n");
        #endif 
        return; // Handle NULL queue pointer
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue's vector is NULL in queue_pop.\n");
        #endif 
        return; // Handle NULL vector in queue
    }
    if (vector_is_empty(q->vec)) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue is empty in queue_pop.\n");
        #endif 
        return; // Handle empty queue
    }
    vector_erase(q->vec, 0, 1);
}

void queue_emplace(Queue* q, void* item, size_t itemSize) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_emplace.\n");
        #endif 
        return; // Handle NULL queue pointer
    }
    if (!q->vec) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue's vector is NULL in queue_emplace.\n");
        #endif 
        return; // Handle NULL vector in queue
    }
    if (!item) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item pointer is NULL in queue_emplace.\n");
        #endif 
        return; // Handle NULL item pointer
    }
    vector_emplace_back(q->vec, item, itemSize);
}

void queue_swap(Queue* q1, Queue* q2){
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_swap.\n");
        #endif 
        return; // Handle NULL queue pointers
    }

    Vector* tempVec = q1->vec;
    q1->vec = q2->vec;
    q2->vec = tempVec;
}

void queue_deallocate(Queue* q) {
    if (!q) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Queue pointer is NULL in queue_deallocate.\n");
        #endif 
        return; // Handle NULL queue pointer
    }
    if (q->vec) { 
        vector_deallocate(q->vec); // Deallocate the underlying vector
    }
    free(q); // Free the queue itself
}

bool queue_is_equal(const Queue* q1, const Queue* q2) {
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_equal.\n");
        #endif 
        return q1 == q2; // Return true if both are NULL, false otherwise
    }
    return vector_is_equal(q1->vec, q2->vec);
}

bool queue_is_less(const Queue* q1, const Queue* q2) {
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_less.\n");
        #endif 
        return q1 != NULL && q2 == NULL; // NULL is considered less
    }
    return vector_is_less(q1->vec, q2->vec);
}

bool queue_is_greater(const Queue* q1, const Queue* q2) {
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_greater.\n");
        #endif 
        return q1 == NULL && q2 != NULL; // NULL is considered less
    }
    return vector_is_greater(q1->vec, q2->vec);
}

bool queue_is_not_equal(const Queue* q1, const Queue* q2) {
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_not_equal.\n");
        #endif 
        return q1 != q2; // Treat as not equal if one or both are NULL
    }
    return !queue_is_equal(q1, q2);
}

bool queue_is_less_or_equal(const Queue* q1, const Queue* q2) {
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_less_or_equal.\n");
        #endif 
        return q1 != NULL || q2 == NULL; // Handle NULL pointers
    }
    return queue_is_less(q1, q2) || queue_is_equal(q1, q2);
}

bool queue_is_greater_or_equal(const Queue* q1, const Queue* q2) {
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_greater_or_equal.\n");
        #endif 
        return q1 == NULL || q2 != NULL; // Handle NULL pointers
    }
    return queue_is_greater(q1, q2) || queue_is_equal(q1, q2);
}
