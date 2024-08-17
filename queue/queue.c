/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Queue
*/


#include "queue.h"
#include "../fmt/fmt.h"
#include <stdlib.h>

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
    if (itemSize == 0) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item size must be greater than zero in queue_create.\n");
        #endif 
        exit(-1); // Handle invalid item size
    }

    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Cannot allocate memory for queue in queue_create.");
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

/**
 * @brief Returns a pointer to the last element in the queue.
 * 
 * This function returns a pointer to the last element in the queue. If the queue 
 * is empty or if the queue pointer is NULL, it returns NULL.
 * 
 * @param q Pointer to the Queue object.
 * @return Pointer to the last element, or NULL if the queue is empty or invalid.
 */
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

/**
 * @brief Removes the first element from the queue.
 * 
 * This function removes the first element from the queue. If the queue is empty or 
 * if the queue pointer is NULL, the function does nothing.
 * 
 * @param q Pointer to the Queue object.
 */
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

/**
 * @brief Swaps the contents of two queues.
 * 
 * This function swaps the internal data (vectors) of two Queue objects,
 * effectively exchanging their contents.
 * 
 * @param q1 Pointer to the first Queue object.
 * @param q2 Pointer to the second Queue object.
 */
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
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_equal.\n");
        #endif 
        return q1 == q2; // Return true if both are NULL, false otherwise
    }
    return vector_is_equal(q1->vec, q2->vec);
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
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_less.\n");
        #endif 
        return q1 != NULL && q2 == NULL; // NULL is considered less
    }
    return vector_is_less(q1->vec, q2->vec);
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
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_greater.\n");
        #endif 
        return q1 == NULL && q2 != NULL; // NULL is considered less
    }
    return vector_is_greater(q1->vec, q2->vec);
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
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_not_equal.\n");
        #endif 
        return q1 != q2; // Treat as not equal if one or both are NULL
    }
    return !queue_is_equal(q1, q2);
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
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_less_or_equal.\n");
        #endif 
        return q1 != NULL || q2 == NULL; // Handle NULL pointers
    }
    return queue_is_less(q1, q2) || queue_is_equal(q1, q2);
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
    if (!q1 || !q2) {
        #ifdef QUEUE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Queue pointers are NULL in queue_is_greater_or_equal.\n");
        #endif 
        return q1 == NULL || q2 != NULL; // Handle NULL pointers
    }
    return queue_is_greater(q1, q2) || queue_is_equal(q1, q2);
}
