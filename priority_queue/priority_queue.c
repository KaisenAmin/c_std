#include "priority_queue.h"
#include <stdlib.h>
#include <string.h>

// static int priority_queue_compare_impl(const void* a, const void* b); 


PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*)) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));

    if (!pq) {
        return NULL; // Allocation failure
    }

    pq->vec = vector_create(itemSize);
    if (!pq->vec) {
        free(pq);
        return NULL; // Vector creation failure
    }
    pq->compare = compare;

    return pq;
}

// Helper function to swap two elements in the same vector
static void element_swap(Vector* vec, size_t index1, size_t index2) {
    if (index1 >= vec->size || index2 >= vec->size) {
        return; // Out of bounds check
    }

    void* temp = malloc(vec->itemSize);
    if (!temp) { 
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
    while (index > 0) {
        size_t parent_index = (index - 1) / 2;
        void* parent = vector_at(vec, parent_index);
        void* current = vector_at(vec, index);

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
    return vector_is_empty(pq->vec);
}

size_t priority_queue_size(const PriorityQueue* pq) {
    return vector_size(pq->vec);
}

void priority_queue_push(PriorityQueue* pq, void* item) {
    vector_push_back(pq->vec, item);
    // Correctly calling the size method
    size_t currentSize = vector_size(pq->vec);
    heapify_up(pq->vec, currentSize - 1, pq->compare);
}

void* priority_queue_top(const PriorityQueue* pq) {
    if (priority_queue_empty(pq)) {
        return NULL; // Handle empty queue
    }
    return vector_front(pq->vec);
}

void priority_queue_pop(PriorityQueue* pq) {
    if (priority_queue_empty(pq)) {
        return; // Handle empty queue
    }
    size_t currentSize = vector_size(pq->vec);

    element_swap(pq->vec, 0, currentSize - 1);
    vector_pop_back(pq->vec);
    heapify_down(pq->vec, 0, pq->compare);
}

void priority_queue_deallocate(PriorityQueue* pq) {
    if (pq) {
        if (pq->vec) { 
            vector_deallocate(pq->vec);
        }
        free(pq);
    }
}

void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2) {
    if (!pq1 || !pq2) {
        return;
    }
    // Swap the internal Vector pointers
    Vector* tempVec = pq1->vec;
    pq1->vec = pq2->vec;
    pq2->vec = tempVec;

    // Swap the comparison functions (if different priority queues might have different comparison logic)
    int (*tempCompare)(const void*, const void*) = pq1->compare;
    pq1->compare = pq2->compare;
    pq2->compare = tempCompare;
}