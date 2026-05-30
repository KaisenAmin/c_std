/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Queue
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in queue.c (file-level convention).
 *
 * A C analog of C++ std::queue<T>, layered on top of the project's Vector.
 *
 * Semantics:
 *   - FIFO container: push at the back, pop from the front.
 *   - Mutating ops take Queue*; observers take const Queue*.
 *   - No library function ever calls exit().
 *
 * Error model:
 *   - No global error state. No errno-style last-error. Every function
 *     reports failure through its return value (NULL / false / 0).
 *   - Opt-in diagnostic logging via QUEUE_LOGGING_ENABLE → QUEUE_LOG.
 *
 * Performance note:
 *   The underlying storage is a contiguous Vector, so queue_pop()
 *   shifts the remaining elements and is O(n). Functionally this
 *   matches std::queue's interface; if you need O(1) pop, prefer a
 *   deque- or list-backed FIFO.
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "../vector/vector.h"

#ifdef __cplusplus
extern "C" {
#endif


/* #define QUEUE_LOGGING_ENABLE */

#ifdef QUEUE_LOGGING_ENABLE
    #define QUEUE_LOG(fmt, ...) \
        do { fprintf(stderr, "[QUEUE LOG]: " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define QUEUE_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef struct Queue Queue;
typedef int (*QueueCompareFunc)(const void*, const void*);

struct Queue {
    Vector* vec;
};


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Queue*  queue_create                  (size_t itemSize);
Queue*  queue_copy                    (const Queue* src);


/* ------------------------------------------------------------------ */
/* Destruction                                                        */
/* ------------------------------------------------------------------ */

void    queue_deallocate              (Queue* q);


/* ------------------------------------------------------------------ */
/* Capacity / size                                                    */
/* ------------------------------------------------------------------ */

size_t  queue_size                    (const Queue* q);
size_t  queue_capacity                (const Queue* q);
size_t  queue_item_size               (const Queue* q);
bool    queue_empty                   (const Queue* q);
bool    queue_reserve                 (Queue* q, size_t n);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

bool    queue_push                    (Queue* q, const void* item);
void*   queue_emplace                 (Queue* q, const void* item, size_t itemSize);
void    queue_pop                     (Queue* q);
bool    queue_assign                  (Queue* dest, const Queue* src);
void    queue_swap                    (Queue* q1, Queue* q2);
void    queue_sort                    (Queue* q, QueueCompareFunc comp);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

void*   queue_front                   (const Queue* q);
void*   queue_back                    (const Queue* q);
void*   queue_at                      (const Queue* q, size_t index);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool    queue_is_equal                (const Queue* q1, const Queue* q2);
bool    queue_is_not_equal            (const Queue* q1, const Queue* q2);
bool    queue_is_less                 (const Queue* q1, const Queue* q2);
bool    queue_is_less_or_equal        (const Queue* q1, const Queue* q2);
bool    queue_is_greater              (const Queue* q1, const Queue* q2);
bool    queue_is_greater_or_equal     (const Queue* q1, const Queue* q2);


#ifdef __cplusplus
}
#endif

#endif 
