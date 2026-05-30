/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class PriorityQueue
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in priority_queue.c (file-level convention).
 *
 * A C analog of C++ std::priority_queue<T, Vector, Compare>, layered
 * on top of the project's Vector.  Maintains a binary heap with
 * O(log n) push/pop and O(1) top.
 *
 * Semantics:
 *   - Mutating ops take PriorityQueue*; observers take const PriorityQueue*.
 *   - The comparator follows the qsort convention: compare(a, b) > 0 means
 *     `a` has higher priority than `b` (i.e. `a` should bubble toward the
 *     top).  Use the negated comparator to get a min-heap.
 *   - No library function ever calls exit().
 *
 * Error model:
 *   - No global error state. No errno-style last-error. Every function
 *     reports failure through its return value (NULL / false / 0).
 *   - Opt-in diagnostic logging via PQUEUE_LOGGING_ENABLE → PQUEUE_LOG.
 */

#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "../vector/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #define PQUEUE_LOGGING_ENABLE */

#ifdef PQUEUE_LOGGING_ENABLE
    #define PQUEUE_LOG(fmt, ...) \
        do { fprintf(stderr, "[PQUEUE LOG]: " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define PQUEUE_LOG(...) do { } while (0)
#endif



typedef struct PriorityQueue PriorityQueue;
typedef int (*PQCompareFunc)(const void* a, const void* b);

struct PriorityQueue {
    Vector*       vec;
    PQCompareFunc compare;
};


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

PriorityQueue*  priority_queue_create               (size_t itemSize, PQCompareFunc compare);
PriorityQueue*  priority_queue_copy                 (const PriorityQueue* src);


/* ------------------------------------------------------------------ */
/* Destruction                                                        */
/* ------------------------------------------------------------------ */

void            priority_queue_clear                (PriorityQueue* pq);
void            priority_queue_deallocate           (PriorityQueue* pq);


/* ------------------------------------------------------------------ */
/* Capacity / size                                                    */
/* ------------------------------------------------------------------ */

size_t          priority_queue_size                 (const PriorityQueue* pq);
size_t          priority_queue_capacity             (const PriorityQueue* pq);
size_t          priority_queue_item_size            (const PriorityQueue* pq);
bool            priority_queue_empty                (const PriorityQueue* pq);
bool            priority_queue_reserve              (PriorityQueue* pq, size_t n);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

bool            priority_queue_push                 (PriorityQueue* pq, const void* item);
void*           priority_queue_emplace              (PriorityQueue* pq, const void* item, size_t itemSize);
void            priority_queue_pop                  (PriorityQueue* pq);
bool            priority_queue_assign               (PriorityQueue* dest, const PriorityQueue* src);
void            priority_queue_swap                 (PriorityQueue* pq1, PriorityQueue* pq2);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

void*           priority_queue_top                  (const PriorityQueue* pq);
void*           priority_queue_front                (const PriorityQueue* pq);
void*           priority_queue_back                 (const PriorityQueue* pq);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool            priority_queue_is_equal             (const PriorityQueue* a, const PriorityQueue* b);
bool            priority_queue_is_not_equal         (const PriorityQueue* a, const PriorityQueue* b);
bool            priority_queue_is_less              (const PriorityQueue* a, const PriorityQueue* b);
bool            priority_queue_is_less_or_equal     (const PriorityQueue* a, const PriorityQueue* b);
bool            priority_queue_is_greater           (const PriorityQueue* a, const PriorityQueue* b);
bool            priority_queue_is_greater_or_equal  (const PriorityQueue* a, const PriorityQueue* b);


#ifdef __cplusplus
}
#endif

#endif 
