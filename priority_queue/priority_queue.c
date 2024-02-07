/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class PriorityQueue
*/

#include "priority_queue.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>


PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*)) {
    if (itemSize == 0) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item size cannot be zero in priority_queue_create.\n");
        #endif 
        exit(-1);
    }

    if (!compare) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Comparison function cannot be NULL in priority_queue_create.\n");
        #endif 
        exit(-1);
    }

    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for PriorityQueue in priority_queue_create.\n");
        #endif 
        exit(-1); // Allocation failure
    }

    pq->vec = vector_create(itemSize);
    if (!pq->vec) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector creation failed in priority_queue_create.\n");
        #endif 
        free(pq);
        exit(-1); // Vector creation failure
    }
    pq->compare = compare;

    return pq;
}

// Helper function to swap two elements in the same vector
static void element_swap(Vector* vec, size_t index1, size_t index2) {
    if (!vec) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL Vector pointer in element_swap.\n");
        #endif 
        return;
    }

    if (index1 >= vec->size || index2 >= vec->size) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Index out of bounds in element_swap.\n");
        #endif 
        return; // Out of bounds check
    }

    void* temp = malloc(vec->itemSize);
    if (!temp) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for temporary element in element_swap.\n");
        #endif 
        return; 
    }

    // Swap elements at index1 and index2
    memcpy(temp, (char*)vec->items + index1 * vec->itemSize, vec->itemSize);
    memcpy((char*)vec->items + index1 * vec->itemSize, (char*)vec->items + index2 * vec->itemSize, vec->itemSize);
    memcpy((char*)vec->items + index2 * vec->itemSize, temp, vec->itemSize);

    free(temp);
}
// Function to maintain the heap property from bottom to top
static void heapify_up(Vector* vec, size_t index, int (*compare)(const void*, const void*)) {
    if (!vec || !compare) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL vector or comparison function in heapify_up.\n");
        #endif 
        return;
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;
        void* parent = vector_at(vec, parent_index);
        void* current = vector_at(vec, index);

        if (!parent || !current) {
            #ifdef PQUEUE_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Invalid index in vector in heapify_up.\n");
            #endif 
            break;
        }
        if (compare(current, parent) > 0) {  // Assuming a max heap
            element_swap(vec, index, parent_index);
            index = parent_index;
        } 
        else {
            break;
        }
    }
}

// Function to maintain the heap property from top to bottom
static void heapify_down(Vector* vec, size_t index, int (*compare)(const void*, const void*)) {
    if (!vec || !compare) {
        #ifdef PQUEUE_LOGGING_ENABLE
           fmt_fprintf(stderr, "Error: NULL vector or comparison function in heapify_down.\n");
        #endif 
        return;
    }

    size_t size = vector_size(vec);
    size_t left_child_index, right_child_index, largest_index;

    while (index < size) {
        largest_index = index;
        left_child_index = 2 * index + 1;
        right_child_index = 2 * index + 2;

        void* largest = vector_at(vec, largest_index);
        void* left_child = left_child_index < size ? vector_at(vec, left_child_index) : NULL;
        void* right_child = right_child_index < size ? vector_at(vec, right_child_index) : NULL;

        if (left_child && compare(left_child, largest) > 0) {
            largest_index = left_child_index;
            largest = left_child;
        }

        if (right_child && compare(right_child, largest) > 0) { 
            largest_index = right_child_index;
        }
        if (largest_index != index) {
            element_swap(vec, index, largest_index);
            index = largest_index;
        } 
        else {
            break;
        }
    }
}

bool priority_queue_empty(const PriorityQueue* pq) {
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL PriorityQueue pointer in priority_queue_empty.\n");
        #endif 
        return true; // Treat NULL PriorityQueue as empty.
    }
    return vector_is_empty(pq->vec);
}

size_t priority_queue_size(const PriorityQueue* pq) {
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL PriorityQueue pointer in priority_queue_size.\n");
        #endif 
        return 0; // Return 0 for NULL PriorityQueue.
    }
    return vector_size(pq->vec);
}

void priority_queue_push(PriorityQueue* pq, void* item) {
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL PriorityQueue pointer in priority_queue_push.\n");
        #endif 
        return;
    }

    if (!item) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: NULL item pointer in priority_queue_push.\n");
        #endif 
        return;
    }
    vector_push_back(pq->vec, item);
    // Correctly calling the size method
    size_t currentSize = vector_size(pq->vec);
    heapify_up(pq->vec, currentSize - 1, pq->compare);
}

void* priority_queue_top(const PriorityQueue* pq) {
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: priority_queue_top called with NULL PriorityQueue pointer.\n");
        #endif 
        return NULL;
    }
    if (priority_queue_empty(pq)) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Attempted to access top element of an empty PriorityQueue.\n");
        #endif 
        return NULL; // Handle empty queue
    }
    return vector_front(pq->vec);
}

void priority_queue_pop(PriorityQueue* pq) {
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: priority_queue_pop called with NULL PriorityQueue pointer.\n");
        #endif 
        return;
    }

    if (priority_queue_empty(pq)) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Attempted to pop an element from an empty PriorityQueue.\n");
        #endif 
        return; // Handle empty queue
    }

    size_t currentSize = vector_size(pq->vec);
    element_swap(pq->vec, 0, currentSize - 1);
    vector_pop_back(pq->vec);
    heapify_down(pq->vec, 0, pq->compare);
}

void priority_queue_deallocate(PriorityQueue* pq) {
    if (!pq) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Attempted to deallocate a NULL PriorityQueue pointer.\n");
        #endif 
        return;
    }
    if (pq->vec) { 
        vector_deallocate(pq->vec);
    }
    free(pq);
}

void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2) {
    if (!pq1 || !pq2) {
        #ifdef PQUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: priority_queue_swap called with NULL PriorityQueue pointer(s).\n");
        #endif 
        return;
    }

    Vector* tempVec = pq1->vec;
    pq1->vec = pq2->vec;
    pq2->vec = tempVec;

    int (*tempCompare)(const void*, const void*) = pq1->compare;
    pq1->compare = pq2->compare;
    pq2->compare = tempCompare;
}
