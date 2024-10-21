/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class PriorityQueue
*/

#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"


/**
 * @brief Creates a new Priority Queue object.
 * 
 * This function initializes a new PriorityQueue object with the specified item size and comparison function.
 * 
 * @param itemSize Size of each item that the priority queue will hold.
 * @param compare Pointer to a comparison function used to maintain the priority order in the queue.
 * 
 * @return Pointer to the newly created PriorityQueue object.
 */
PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*)) {
    PQUEUE_LOG("[priority_queue_create]: Entering with itemSize: %zu", itemSize);

    if (itemSize == 0) {
        PQUEUE_LOG("[priority_queue_create]: Error: Item size cannot be zero.");
        exit(-1);
    }

    if (!compare) {
        PQUEUE_LOG("[priority_queue_create]: Error: Comparison function cannot be NULL.");
        exit(-1);
    }

    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        PQUEUE_LOG("[priority_queue_create]: Error: Memory allocation failed for PriorityQueue.");
        exit(-1); 
    }

    PQUEUE_LOG("[priority_queue_create]: Successfully allocated memory for PriorityQueue.");

    pq->vec = vector_create(itemSize);
    if (!pq->vec) {
        PQUEUE_LOG("[priority_queue_create]: Error: Vector creation failed.");
        free(pq);
        exit(-1); 
    }

    PQUEUE_LOG("[priority_queue_create]: Successfully created vector with itemSize: %zu", itemSize);

    pq->compare = compare;
    PQUEUE_LOG("[priority_queue_create]: PriorityQueue created successfully with itemSize: %zu and comparison function.", itemSize);

    return pq;
}

/**
 * @brief Swaps two elements within the same vector.
 * 
 * This helper function swaps the elements at the specified indices within the provided vector.
 * 
 * @param vec Pointer to the Vector object.
 * @param index1 The index of the first element to swap.
 * @param index2 The index of the second element to swap.
 */
static void element_swap(Vector* vec, size_t index1, size_t index2) {
    PQUEUE_LOG("[element_swap]: Entering with Vector pointer: %p, index1: %zu, index2: %zu", (void*)vec, index1, index2);

    if (!vec) {
        PQUEUE_LOG("[element_swap]: Error: NULL Vector pointer.");
        return;
    }

    if (index1 >= vec->size || index2 >= vec->size) {
        PQUEUE_LOG("[element_swap]: Error: Index out of bounds (index1: %zu, index2: %zu, size: %zu).", index1, index2, vec->size);
        return; // Out of bounds check
    }

    void* temp = malloc(vec->itemSize);
    if (!temp) {
        PQUEUE_LOG("[element_swap]: Error: Memory allocation failed for temporary element.");
        return;
    }

    // Swap elements at index1 and index2
    memcpy(temp, (char*)vec->items + index1 * vec->itemSize, vec->itemSize);
    memcpy((char*)vec->items + index1 * vec->itemSize, (char*)vec->items + index2 * vec->itemSize, vec->itemSize);
    memcpy((char*)vec->items + index2 * vec->itemSize, temp, vec->itemSize);

    free(temp);
    PQUEUE_LOG("[element_swap]: Swapped elements at index1: %zu and index2: %zu.", index1, index2);
}

/**
 * @brief Maintains the heap property from bottom to top.
 * 
 * This function ensures that the heap property is maintained in the vector by comparing each element with its parent and swapping them if necessary, moving upwards.
 * 
 * @param vec Pointer to the Vector object representing the heap.
 * @param index The index of the element to heapify upwards.
 * @param compare Pointer to the comparison function used to compare the elements.
 */
static void heapify_up(Vector* vec, size_t index, int (*compare)(const void*, const void*)) {
    PQUEUE_LOG("[heapify_up]: Entering with Vector pointer: %p, index: %zu", (void*)vec, index);

    if (!vec || !compare) {
        PQUEUE_LOG("[heapify_up]: Error: NULL vector or comparison function.");
        return;
    }

    while (index > 0) {
        size_t parent_index = (index - 1) / 2;
        void* parent = vector_at(vec, parent_index);
        void* current = vector_at(vec, index);

        PQUEUE_LOG("[heapify_up]: Current index: %zu, Parent index: %zu", index, parent_index);

        if (!parent || !current) {
            PQUEUE_LOG("[heapify_up]: Error: Invalid index in vector (current: %p, parent: %p).", current, parent);
            break;
        }
        if (compare(current, parent) > 0) {  // Assuming a max heap
            element_swap(vec, index, parent_index);
            PQUEUE_LOG("[heapify_up]: Swapped index: %zu with parent index: %zu", index, parent_index);
            index = parent_index;
        } 
        else {
            break;
        }
    }

    PQUEUE_LOG("[heapify_up]: Heapify up completed.");
}

// Function to maintain the heap property from top to bottom
static void heapify_down(Vector* vec, size_t index, int (*compare)(const void*, const void*)) {
    PQUEUE_LOG("[heapify_down]: Entering with Vector pointer: %p, index: %zu", (void*)vec, index);

    if (!vec || !compare) {
        PQUEUE_LOG("[heapify_down]: Error: NULL vector or comparison function.");
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

        PQUEUE_LOG("[heapify_down]: Index: %zu, Left Child Index: %zu, Right Child Index: %zu", 
                    index, left_child_index, right_child_index);

        if (left_child && compare(left_child, largest) > 0) {
            largest_index = left_child_index;
            largest = left_child;
            PQUEUE_LOG("[heapify_down]: Left child is larger, updating largest_index to %zu", largest_index);
        }
        if (right_child && compare(right_child, largest) > 0) { 
            largest_index = right_child_index;
            PQUEUE_LOG("[heapify_down]: Right child is larger, updating largest_index to %zu", largest_index);
        }

        if (largest_index != index) {
            element_swap(vec, index, largest_index);
            PQUEUE_LOG("[heapify_down]: Swapped elements at index %zu and %zu", index, largest_index);
            index = largest_index;
        } 
        else {
            break;
        }
    }

    PQUEUE_LOG("[heapify_down]: Heapify down completed.");
}

/**
 * @brief Checks if the priority queue is empty.
 * 
 * This function checks whether the priority queue contains any elements.
 * 
 * @param pq Pointer to the PriorityQueue object.
 * @return `true` if the priority queue is empty or `pq` is NULL, otherwise `false`.
 */
bool priority_queue_empty(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_empty]: Entering with PriorityQueue pointer: %p", (void*)pq);

    if (!pq) {
        PQUEUE_LOG("[priority_queue_empty]: Error: NULL PriorityQueue pointer.");
        return true; // Treat NULL PriorityQueue as empty.
    }

    bool isEmpty = vector_is_empty(pq->vec);
    PQUEUE_LOG("[priority_queue_empty]: PriorityQueue is %sempty", isEmpty ? "" : "not ");
    
    return isEmpty;
}

/**
 * @brief Returns the number of elements in the priority queue.
 * 
 * This function returns the current size (number of elements) of the priority queue.
 * 
 * @param pq Pointer to the PriorityQueue object.
 * @return The number of elements in the priority queue, or 0 if `pq` is NULL.
 */
size_t priority_queue_size(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_size]: Entering with PriorityQueue pointer: %p", (void*)pq);

    if (!pq) {
        PQUEUE_LOG("[priority_queue_size]: Error: NULL PriorityQueue pointer.");
        return 0; 
    }

    size_t size = vector_size(pq->vec);
    PQUEUE_LOG("[priority_queue_size]: Returning size: %zu", size);

    return size;
}

/**
 * @brief Adds an item to the priority queue.
 * 
 * This function inserts a new item into the priority queue, maintaining the heap property 
 * based on the provided comparison function.
 * 
 * @param pq Pointer to the PriorityQueue object.
 * @param item Pointer to the item to be added to the priority queue.
 */
void priority_queue_push(PriorityQueue* pq, void* item) {
    PQUEUE_LOG("[priority_queue_push]: Entering with PriorityQueue pointer: %p and item: %p", (void*)pq, item);

    if (!pq) {
        PQUEUE_LOG("[priority_queue_push]: Error: NULL PriorityQueue pointer.");
        return;
    }
    if (!item) {
        PQUEUE_LOG("[priority_queue_push]: Error: NULL item pointer.");
        return;
    }

    vector_push_back(pq->vec, item);
    size_t currentSize = vector_size(pq->vec);
    PQUEUE_LOG("[priority_queue_push]: Current PriorityQueue size after push: %zu", currentSize);

    heapify_up(pq->vec, currentSize - 1, pq->compare);
    PQUEUE_LOG("[priority_queue_push]: Heapified up after inserting the new item.");
}

/**
 * @brief Returns a pointer to the top element in the priority queue.
 * 
 * This function returns a pointer to the element with the highest priority in the priority queue without removing it.
 * 
 * @param pq Pointer to the PriorityQueue object.
 * @return Pointer to the top element, or `NULL` if the priority queue is empty or `pq` is NULL.
 */
void* priority_queue_top(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_top]: Entering with PriorityQueue pointer: %p", (void*)pq);

    if (!pq) {
        PQUEUE_LOG("[priority_queue_top]: Error: NULL PriorityQueue pointer.");
        return NULL;
    }
    if (priority_queue_empty(pq)) {
        PQUEUE_LOG("[priority_queue_top]: Error: Attempted to access top element of an empty PriorityQueue.");
        return NULL; 
    }

    void* topElement = vector_front(pq->vec);
    PQUEUE_LOG("[priority_queue_top]: Returning top element pointer: %p", topElement);
    return topElement;
}

/**
 * @brief Removes the top element from the priority queue.
 * 
 * This function removes the element with the highest priority (i.e., the element at the front of the priority queue) 
 * and rebalances the heap to maintain the priority order.
 * 
 * @param pq Pointer to the PriorityQueue object.
 */
void priority_queue_pop(PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_pop]: Entering with PriorityQueue pointer: %p", (void*)pq);

    if (!pq) {
        PQUEUE_LOG("[priority_queue_pop]: Error: NULL PriorityQueue pointer.");
        return;
    }
    if (priority_queue_empty(pq)) {
        PQUEUE_LOG("[priority_queue_pop]: Warning: Attempted to pop an element from an empty PriorityQueue.");
        return; 
    }

    size_t currentSize = vector_size(pq->vec);
    PQUEUE_LOG("[priority_queue_pop]: Current PriorityQueue size: %zu", currentSize);

    element_swap(pq->vec, 0, currentSize - 1);
    PQUEUE_LOG("[priority_queue_pop]: Swapped top element with the last element.");

    vector_pop_back(pq->vec);
    PQUEUE_LOG("[priority_queue_pop]: Popped the last element.");

    heapify_down(pq->vec, 0, pq->compare);
    PQUEUE_LOG("[priority_queue_pop]: Rebalanced the heap.");
}

/**
 * @brief Deallocates the priority queue and frees all associated memory.
 * 
 * This function deallocates the PriorityQueue object, including its underlying vector, 
 * and frees any memory that was allocated for storing the elements.
 * 
 * @param pq Pointer to the PriorityQueue object to be deallocated.
 */
void priority_queue_deallocate(PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_deallocate]: Entering with PriorityQueue pointer: %p", (void*)pq);

    if (!pq) {
        PQUEUE_LOG("[priority_queue_deallocate]: Warning: Attempted to deallocate a NULL PriorityQueue pointer.");
        return;
    }
    if (pq->vec) {
        PQUEUE_LOG("[priority_queue_deallocate]: Deallocating the underlying vector.");
        vector_deallocate(pq->vec);
    }

    free(pq);
    PQUEUE_LOG("[priority_queue_deallocate]: Deallocated PriorityQueue memory.");
}

/**
 * @brief Swaps the contents of two priority queues.
 * 
 * This function swaps the internal data (elements and comparison function) of two PriorityQueue objects. 
 * 
 * @param pq1 Pointer to the first PriorityQueue object.
 * @param pq2 Pointer to the second PriorityQueue object.
 */
void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2) {
    PQUEUE_LOG("[priority_queue_swap]: Entering with PriorityQueue pointers: pq1 = %p, pq2 = %p", (void*)pq1, (void*)pq2);

    if (!pq1 || !pq2) {
        PQUEUE_LOG("[priority_queue_swap]: Error: NULL PriorityQueue pointer(s) passed.");
        return;
    }

    Vector* tempVec = pq1->vec;
    pq1->vec = pq2->vec;
    pq2->vec = tempVec;
    PQUEUE_LOG("[priority_queue_swap]: Swapped the underlying vectors.");

    int (*tempCompare)(const void*, const void*) = pq1->compare;
    pq1->compare = pq2->compare;
    pq2->compare = tempCompare;
    PQUEUE_LOG("[priority_queue_swap]: Swapped the comparison functions.");
}