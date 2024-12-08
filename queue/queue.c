/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Queue
*/


#include <stdlib.h>
#include "queue.h"
#include "../algorithm/algorithm.h"


/**
 * @brief Creates a new Queue object with the specified item size.
 * 
 * This function allocates memory for a new Queue object and initializes it with
 * a vector capable of holding items of the specified size. If the item size is 0 
 * or if memory allocation fails, the function will log an error (if logging is enabled)
 * and terminate the program.
 * 
 * @param itemSize The size of each item in the queue. Must be greater than zero.
 * @return Pointer to the newly created Queue object. The function will terminate 
 *         the program if allocation fails.
 */
Queue* queue_create(size_t itemSize) {
    QUEUE_LOG("[queue_create]: Entering with itemSize: %zu", itemSize);

    if (itemSize == 0) {
        QUEUE_LOG("[queue_create]: Error: Item size must be greater than zero");
        exit(-1); 
    }

    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        QUEUE_LOG("[queue_create]: Error: Cannot allocate memory for queue");
        exit(-1); 
    }

    queue->vec = vector_create(itemSize);
    if (!queue->vec) {
        QUEUE_LOG("[queue_create]: Error: Cannot allocate memory for queue->vec");
        free(queue);
        exit(-1); 
    }

    QUEUE_LOG("[queue_create]: Queue created successfully at %p", (void*)queue);
    return queue;
}

/**
 * @brief Checks if the queue is empty.
 * 
 * This function checks whether the queue is empty. If the queue or its underlying
 * vector is NULL, it returns true.
 * 
 * @param q Pointer to the Queue object.
 * @return true if the queue is empty or NULL, false otherwise.
 */
bool queue_empty(const Queue* q) {
    QUEUE_LOG("[queue_empty]: Entering with queue: %p", (void*)q);

    if (!q) {
        QUEUE_LOG("[queue_empty]: Error: Queue pointer is NULL");
        return true; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_empty]: Error: Queue's vector is NULL");
        return true; 
    }

    bool result = vector_is_empty(q->vec);
    QUEUE_LOG("[queue_empty]: Result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief Returns the number of elements in the queue.
 * 
 * This function returns the size of the queue, which is the number of elements it contains.
 * If the queue or its underlying vector is NULL, it returns 0.
 * 
 * @param q Pointer to the Queue object.
 * @return The number of elements in the queue, or 0 if the queue is NULL.
 */
size_t queue_size(const Queue* q) {
    QUEUE_LOG("[queue_size]: Entering with queue: %p", (void*)q);

    if (!q) {
        QUEUE_LOG("[queue_size]: Error: Queue pointer is NULL");
        return 0; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_size]: Error: Queue's vector is NULL");
        return 0; 
    }

    size_t size = vector_size(q->vec);
    QUEUE_LOG("[queue_size]: Queue size is: %zu", size);

    return size;
}

/**
 * @brief Adds an item to the back of the queue.
 * 
 * This function pushes an item onto the back of the queue. If the queue, its vector, or the item
 * is NULL, the function does nothing.
 * 
 * @param q Pointer to the Queue object.
 * @param item Pointer to the item to be added to the queue.
 */
void queue_push(Queue* q, void* item) {
    QUEUE_LOG("[queue_push]: Entering with queue: %p, item: %p", (void*)q, (void*)item);

    if (!q) {
        QUEUE_LOG("[queue_push]: Error: Queue is NULL");
        return; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_push]: Error: Queue's vector is NULL");
        return; 
    }
    if (!item) {
        QUEUE_LOG("[queue_push]: Error: Item pointer is NULL");
        return; 
    }

    vector_push_back(q->vec, item);
    QUEUE_LOG("[queue_push]: Item pushed to queue");
}

/**
 * @brief Sorts the elements of the queue in place using the specified comparison function.
 *
 * This function sorts the elements of a `Queue` object by directly operating
 * on the underlying vector data using a non-stable quicksort algorithm. The
 * sorting order is determined by the user-provided comparison function.
 *
 * @param q Pointer to the `Queue` object to be sorted.
 * @param comp Comparison function used to order the elements. It should return:
 *             - A negative value if the first element is less than the second.
 *             - Zero if the two elements are equal.
 *             - A positive value if the first element is greater than the second.
 *
 * @note The function does nothing if the `Queue` or its underlying vector is NULL.
 */
void queue_sort(Queue* q, QueueCompareFunc comp) {
    if (!q || !q->vec) {
        QUEUE_LOG("[queue_sort]: Error: Queue or vector is NULL.");
        return;
    }

    algorithm_sort(vector_data(q->vec), vector_size(q->vec), q->vec->itemSize, comp);
    QUEUE_LOG("[queue_sort]: Queue sorted successfully.");
}

/**
 * @brief Returns a pointer to the front element in the queue.
 * 
 * This function returns a pointer to the front element in the queue. If the queue
 * is empty or if the queue or its vector is NULL, it returns NULL.
 * 
 * @param q Pointer to the Queue object.
 * @return Pointer to the front element, or NULL if the queue is empty or invalid.
 */
void* queue_front(const Queue* q) {
    QUEUE_LOG("[queue_front]: Entering with queue: %p", (void*)q);

    if (!q) {
        QUEUE_LOG("[queue_front]: Error: Queue pointer is NULL");
        return NULL; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_front]: Error: Queue's vector is NULL");
        return NULL; 
    }
    if (vector_is_empty(q->vec)) {
        QUEUE_LOG("[queue_front]: Error: Queue is empty");
        return NULL; 
    }

    void* front = vector_front(q->vec);
    QUEUE_LOG("[queue_front]: Returning front element: %p", (void*)front);

    return front;
}

/**
 * @brief Returns a pointer to the last element in the queue.
 * 
 * This function returns a pointer to the last element in the queue. If the queue 
 * is empty or if the queue pointer is NULL, it returns NULL.
 * 
 * @param q Pointer to the Queue object.
 * @return Pointer to the last element, or NULL if the queue is empty or invalid.
 */
void* queue_back(const Queue* q) {
    QUEUE_LOG("[queue_back]: Entering with queue: %p", (void*)q);

    if (!q) {
        QUEUE_LOG("[queue_back]: Error: Queue pointer is NULL");
        return NULL; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_back]: Error: Queue's vector is NULL");
        return NULL; 
    }
    if (vector_is_empty(q->vec)) {
        QUEUE_LOG("[queue_back]: Error: Queue is empty");
        return NULL; 
    }

    void* back = vector_back(q->vec);
    QUEUE_LOG("[queue_back]: Returning back element: %p", (void*)back);
    return back;
}

/**
 * @brief Removes the first element from the queue.
 * 
 * This function removes the first element from the queue. If the queue is empty or 
 * if the queue pointer is NULL, the function does nothing.
 * 
 * @param q Pointer to the Queue object.
 */
void queue_pop(Queue* q) {
    QUEUE_LOG("[queue_pop]: Entering with queue: %p", (void*)q);

    if (!q) {
        QUEUE_LOG("[queue_pop]: Error: Queue pointer is NULL");
        return; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_pop]: Error: Queue's vector is NULL");
        return; 
    }
    if (vector_is_empty(q->vec)) {
        QUEUE_LOG("[queue_pop]: Error: Queue is empty");
        return; 
    }

    vector_erase(q->vec, 0, 1);
    QUEUE_LOG("[queue_pop]: First element removed from queue");
}

/**
 * @brief Emplaces an item at the back of the queue.
 * 
 * This function emplaces a new element at the back of the queue. The item is 
 * constructed directly in the memory of the queue using the provided size.
 * 
 * @param q Pointer to the Queue object.
 * @param item Pointer to the item to be added.
 * @param itemSize Size of the item to be added.
 */
void queue_emplace(Queue* q, void* item, size_t itemSize) {
    QUEUE_LOG("[queue_emplace]: Entering with queue: %p, item: %p, itemSize: %zu", (void*)q, (void*)item, itemSize);

    if (!q) {
        QUEUE_LOG("[queue_emplace]: Error: Queue pointer is NULL");
        return; 
    }
    if (!q->vec) {
        QUEUE_LOG("[queue_emplace]: Error: Queue's vector is NULL");
        return; 
    }
    if (!item) {
        QUEUE_LOG("[queue_emplace]: Error: Item pointer is NULL");
        return; 
    }

    vector_emplace_back(q->vec, item, itemSize);
    QUEUE_LOG("[queue_emplace]: Item emplaced in queue at back");
}

/**
 * @brief Swaps the contents of two queues.
 * 
 * This function swaps the internal data (vectors) of two Queue objects,
 * effectively exchanging their contents.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 */
void queue_swap(Queue* q1, Queue* q2) {
    QUEUE_LOG("[queue_swap]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_swap]: Error: One or both Queue pointers are NULL");
        return; // Handle NULL queue pointers
    }

    Vector* tempVec = q1->vec;
    q1->vec = q2->vec;
    q2->vec = tempVec;

    QUEUE_LOG("[queue_swap]: Successfully swapped q1 and q2");
}

/**
 * @brief Deallocates the queue and frees its resources.
 * 
 * This function deallocates the memory used by the Queue object and
 * its internal vector. After calling this function, the Queue pointer
 * should not be used unless it is reinitialized.
 * 
 * @param q Pointer to the Queue object to be deallocated.
 */
void queue_deallocate(Queue* q) {
    QUEUE_LOG("[queue_deallocate]: Entering with q: %p", (void*)q);

    if (!q) {
        QUEUE_LOG("[queue_deallocate]: Error: Queue pointer is NULL");
        return;
    }

    if (q->vec) {
        vector_deallocate(q->vec); 
        QUEUE_LOG("[queue_deallocate]: Vector deallocated");
    }

    free(q); 
    QUEUE_LOG("[queue_deallocate]: Queue deallocated successfully");
}

/**
 * @brief Checks if two queues are equal.
 * 
 * This function compares two Queue objects and returns true if they are
 * equal in size and content.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 * 
 * @return true if the queues are equal, false otherwise.
 */
bool queue_is_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_equal]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_is_equal]: Error: One or both Queue pointers are NULL");
        return q1 == q2; 
    }

    bool result = vector_is_equal(q1->vec, q2->vec);
    QUEUE_LOG("[queue_is_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}
/**
 * @brief Checks if the first queue is less than the second queue.
 * 
 * This function compares two Queue objects and returns true if the first
 * queue is considered less than the second queue based on their elements.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 * 
 * @return true if the first queue is less, false otherwise.
 */
bool queue_is_less(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_less]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_is_less]: Error: One or both Queue pointers are NULL");
        return q1 != NULL && q2 == NULL; 
    }

    bool result = vector_is_less(q1->vec, q2->vec);
    QUEUE_LOG("[queue_is_less]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief Checks if the first queue is greater than the second queue.
 * 
 * This function compares two queues and returns true if the first queue
 * is considered greater than the second queue based on their elements.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 * 
 * @return true if the first queue is greater, false otherwise.
 */
bool queue_is_greater(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_greater]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_is_greater]: Error: One or both Queue pointers are NULL");
        return q1 == NULL && q2 != NULL;
    }

    bool result = vector_is_greater(q1->vec, q2->vec);
    QUEUE_LOG("[queue_is_greater]: Comparison result: %s", result ? "true" : "false");

    return result;
}


/**
 * @brief Checks if two queues are not equal.
 * 
 * This function compares two queues and returns true if they are not equal,
 * either in size or in content.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 * 
 * @return true if the queues are not equal, false otherwise.
 */
bool queue_is_not_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_not_equal]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_is_not_equal]: Error: One or both Queue pointers are NULL");
        return q1 != q2; 
    }

    bool result = !queue_is_equal(q1, q2);
    QUEUE_LOG("[queue_is_not_equal]: Comparison result: %s", result ? "true" : "false");
    return result;
}

/**
 * @brief Checks if the first queue is less than or equal to the second queue.
 * 
 * This function compares two queues and returns true if the first queue
 * is less than or equal to the second queue based on their elements.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 * 
 * @return true if the first queue is less than or equal, false otherwise.
 */
bool queue_is_less_or_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_less_or_equal]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_is_less_or_equal]: Error: One or both Queue pointers are NULL");
        return q1 != NULL || q2 == NULL; 
    }

    bool result = queue_is_less(q1, q2) || queue_is_equal(q1, q2);
    QUEUE_LOG("[queue_is_less_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}

/**
 * @brief Checks if the first queue is greater than or equal to the second queue.
 * 
 * This function compares two queues and returns true if the first queue
 * is greater than or equal to the second queue based on their elements.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 * 
 * @return true if the first queue is greater than or equal, false otherwise.
 */
bool queue_is_greater_or_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_greater_or_equal]: Entering with q1: %p, q2: %p", (void*)q1, (void*)q2);

    if (!q1 || !q2) {
        QUEUE_LOG("[queue_is_greater_or_equal]: Error: One or both Queue pointers are NULL");
        return q1 == NULL || q2 != NULL; 
    }

    bool result = queue_is_greater(q1, q2) || queue_is_equal(q1, q2);
    QUEUE_LOG("[queue_is_greater_or_equal]: Comparison result: %s", result ? "true" : "false");

    return result;
}