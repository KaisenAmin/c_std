/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Deque
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in deque.c (file-level convention).
 *
 * Block-based double-ended queue, modeled on C++ std::deque<T>.
 * Provides O(1) amortized push/pop at both ends and O(1) random
 * access via `deque_at`.
 */

#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* #define DEQUE_LOGGING_ENABLE */

#ifdef DEQUE_LOGGING_ENABLE
    #define DEQUE_LOG(fmt, ...) \
        do { fprintf(stderr, "[DEQUE LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define DEQUE_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

#define DEFAULT_BLOCK_SIZE 64


typedef struct Deque         Deque;
typedef struct DequeIterator DequeIterator;

struct DequeIterator {
    const Deque*  deque;
    bool          isReverse;
    void*         current;        /* pointer to the current element     */
    size_t        blockIndex;     /* current block index                */
    size_t        indexInBlock;   /* index within the current block     */
};


struct Deque {
    size_t  itemSize;
    size_t  blockSize;            /* size of each block                 */
    size_t  size;                 /* total number of elements           */
    size_t  blockCount;           /* number of blocks                   */
    size_t  frontIndex;           /* index of the front element         */
    size_t  backIndex;            /* index of the back element          */
    void*** blocks;               /* pointer to blocks of elements      */
};


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Deque*         deque_create                   (size_t itemSize);
void           deque_clear                    (Deque* deque);
void           deque_deallocate               (Deque* deque);


/* ------------------------------------------------------------------ */
/* Capacity                                                           */
/* ------------------------------------------------------------------ */

size_t         deque_length                   (const Deque* deque);
size_t         deque_max_size                 (const Deque* deque);
bool           deque_empty                    (const Deque* deque);
void           deque_resize                   (Deque* deque, size_t newSize);
void           deque_shrink_to_fit            (Deque* deque);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

void*          deque_front                    (const Deque* deque);
void*          deque_back                     (const Deque* deque);
void*          deque_at                       (const Deque* deque, size_t index);


/* ------------------------------------------------------------------ */
/* Modifiers — push / pop / insert / erase                            */
/* ------------------------------------------------------------------ */

void           deque_push_front               (Deque* deque, void* item);
void           deque_push_back                (Deque* deque, const void* item);
void           deque_pop_front                (Deque* deque);
void           deque_pop_back                 (Deque* deque);
void           deque_emplace_front            (Deque* deque, void* item);
void           deque_emplace_back             (Deque* deque, void* item);
void           deque_emplace                  (Deque* deque, size_t index, void* item);
void           deque_insert                   (Deque* deque, size_t index, void* item);
void           deque_erase                    (Deque* deque, size_t index);
void           deque_assign                   (Deque* deque, size_t n, void* val);
void           deque_swap                     (Deque* deque, Deque* otherDeque);


/* ------------------------------------------------------------------ */
/* Iterators — mutating                                               */
/* ------------------------------------------------------------------ */

DequeIterator  deque_begin                    (const Deque* deque);
DequeIterator  deque_end                      (const Deque* deque);
DequeIterator  deque_rbegin                   (const Deque* deque);
DequeIterator  deque_rend                     (const Deque* deque);


/* ------------------------------------------------------------------ */
/* Iterators — const                                                  */
/* ------------------------------------------------------------------ */

DequeIterator  deque_cbegin                   (const Deque* deque);
DequeIterator  deque_cend                     (const Deque* deque);
DequeIterator  deque_crbegin                  (const Deque* deque);
DequeIterator  deque_crend                    (const Deque* deque);


/* ------------------------------------------------------------------ */
/* Iterator operations                                                */
/* ------------------------------------------------------------------ */

void           iterator_increment             (DequeIterator* it);
void           iterator_decrement             (DequeIterator* it);
bool           iterator_equals                (const DequeIterator* it1, const DequeIterator* it2);
void*          iterator_get                   (const DequeIterator* it);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool           deque_is_equal                 (const Deque* deque1, const Deque* deque2);
bool           deque_is_not_equal             (const Deque* deque1, const Deque* deque2);
bool           deque_is_less                  (const Deque* deque1, const Deque* deque2);
bool           deque_is_less_or_equal         (const Deque* deque1, const Deque* deque2);
bool           deque_is_greater               (const Deque* deque1, const Deque* deque2);
bool           deque_is_greater_or_equal      (const Deque* deque1, const Deque* deque2);


#ifdef __cplusplus
}
#endif

#endif 
