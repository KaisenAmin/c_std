/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class PriorityQueue
 *
 * std::priority_queue-style binary heap layered over Vector.
 *
 * Performance
 * -----------
 *   - push: O(log n) amortised.
 *   - pop:  O(log n).
 *   - top / front / back / observers: O(1).
 *   - copy / assign: O(n); assign rebuilds the heap under dest's
 *     comparator via Floyd's bottom-up heapify (O(n)).
 *   - sift-up / sift-down are hole-based (each level is a single element
 *     move, not a 3-copy swap) and the moves go through pq_move, a
 *     size-specialized copy that becomes one load/store for the common
 *     element sizes instead of a memcpy call with a runtime length.
 *   - pop overwrites the root with the last element (one move) rather than
 *     swapping, since the old root is discarded immediately.
 *   - The OOM fallback path (huge element + malloc failure) swaps in place
 *     via pq_swap, a 256-byte stack buffer with chunking for the remainder.
 */

#include <stdlib.h>
#include <string.h>
#include "priority_queue.h"



static inline void pq_move(void* dst, const void* src, size_t n) {
    switch (n) {
        case 1:  memcpy(dst, src, 1);  break;
        case 2:  memcpy(dst, src, 2);  break;
        case 4:  memcpy(dst, src, 4);  break;
        case 8:  memcpy(dst, src, 8);  break;
        case 16: memcpy(dst, src, 16); break;
        default: memcpy(dst, src, n);  break;
    }
}


static void pq_swap(char* a, char* b, size_t n) {
    if (a == b || n == 0) {
        return;
    }
    char tmp[256];
    while (n > 0) {
        size_t chunk = (n > sizeof tmp) ? sizeof tmp : n;

        memcpy(tmp, a, chunk);
        memcpy(a,   b, chunk);
        memcpy(b,   tmp, chunk);
        a += chunk;
        b += chunk;
        n -= chunk;
    }
}


static size_t pq_sift_up(Vector* vec, size_t index, PQCompareFunc compare) {
    if (!vec || !compare || index == 0) {
        return index;
    }
    char*  base = (char*)vec->items;
    size_t isz  = vec->itemSize;
    if (!base) {
        return index;
    }

    size_t parent = (index - 1) / 2;
    char*  cur = base + index * isz;
    if (compare(cur, base + parent * isz) <= 0) {
        return index;                       /* no bubble: zero data movement */
    }

    char  stackbuf[256];
    char* tmp = (isz <= sizeof stackbuf) ? stackbuf : (char*)malloc(isz);
    if (tmp) {
        pq_move(tmp, cur, isz);
        size_t hole = index;
        while (hole > 0) {
            size_t p = (hole - 1) / 2;
            if (compare(tmp, base + p * isz) > 0) {
                pq_move(base + hole * isz, base + p * isz, isz);
                hole = p;
            }
            else {
                break;
            }
        }
        pq_move(base + hole * isz, tmp, isz);
        if (tmp != stackbuf) {
            free(tmp);
        }
        return hole;
    }

    /* OOM fallback (huge element + allocation failure): swap-based bubble. */
    while (index > 0) {
        size_t p = (index - 1) / 2;
        if (compare(base + index * isz, base + p * isz) > 0) {
            pq_swap(base + index * isz, base + p * isz, isz);
            index = p;
        }
        else {
            break;
        }
    }
    return index;
}


static void pq_sift_down(Vector* vec, size_t top, PQCompareFunc compare) {
    if (!vec || !compare) {
        return;
    }
    char*  base = (char*)vec->items;
    size_t isz  = vec->itemSize;
    size_t n    = vec->size;
    if (!base || n == 0 || top >= n) {
        return;
    }

    char  stackbuf[256];
    char* tmp = (isz <= sizeof stackbuf) ? stackbuf : (char*)malloc(isz);
    if (tmp) {
        pq_move(tmp, base + top * isz, isz);
        size_t hole = top;

        for (;;) {
            size_t child = 2 * hole + 1;
            if (child >= n) {
                break;
            }
            if (child + 1 < n && compare(base + (child + 1) * isz, base + child * isz) > 0) {
                child++;
            }
            pq_move(base + hole * isz, base + child * isz, isz);
            hole = child;
        }

        while (hole > top) {
            size_t parent = (hole - 1) / 2;
            if (compare(tmp, base + parent * isz) > 0) {
                pq_move(base + hole * isz, base + parent * isz, isz);
                hole = parent;
            }
            else {
                break;
            }
        }
        pq_move(base + hole * isz, tmp, isz);
        if (tmp != stackbuf) {
            free(tmp);
        }
        return;
    }

    size_t index = top;
    for (;;) {
        size_t left  = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t best  = index;

        if (left  < n && compare(base + left  * isz, base + best * isz) > 0) {
            best = left;
        }
        if (right < n && compare(base + right * isz, base + best * isz) > 0) {
            best = right;
        }
        if (best == index) {
            break;
        }

        pq_swap(base + index * isz, base + best * isz, isz);
        index = best;
    }
}


/**
 * @brief Create an empty priority queue with the given element size
 *        and comparator.
 *
 * Allocates a PriorityQueue header and an empty backing Vector. No
 * elements are allocated until you push.
 *
 * @param itemSize Size in bytes of one element (must be > 0).
 * @param compare  Three-way comparator that returns `> 0` when `a` has
 *                 higher priority than `b`, `< 0` for lower, `0` for
 *                 equal. Pass a negated ascending comparator to obtain
 *                 a min-heap. Must be non-NULL.
 *
 * @return Newly-allocated PriorityQueue the caller must free with
 *         @ref priority_queue_deallocate, or NULL on:
 *           - `itemSize == 0`
 *           - `compare == NULL`
 *           - PriorityQueue-header malloc failure
 *           - underlying vector_create failure
 */
PriorityQueue* priority_queue_create(size_t itemSize, PQCompareFunc compare) {
    PQUEUE_LOG("[priority_queue_create]: enter itemSize=%zu", itemSize);

    if (itemSize == 0) {
        PQUEUE_LOG("[priority_queue_create]: itemSize is zero -> NULL");
        return NULL;
    }
    if (!compare) {
        PQUEUE_LOG("[priority_queue_create]: compare is NULL -> NULL");
        return NULL;
    }

    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        PQUEUE_LOG("[priority_queue_create]: malloc(PriorityQueue) failed -> NULL");
        return NULL;
    }

    pq->vec = vector_create(itemSize);
    if (!pq->vec) {
        PQUEUE_LOG("[priority_queue_create]: vector_create(%zu) failed -> NULL", itemSize);
        free(pq);

        return NULL;
    }
    pq->compare = compare;

    PQUEUE_LOG("[priority_queue_create]: exit ok pq=%p vec=%p", (void*)pq, (void*)pq->vec);
    return pq;
}


/**
 * @brief Deep-copy an existing priority queue.
 *
 * The new queue gets the same item size, comparator, and elements.
 * Because @p src is already a valid heap under its comparator,
 * preserving the underlying storage layout preserves the heap
 * property — no re-heapify is needed.
 *
 * @param src Source queue to clone (must be non-NULL with a valid
 *            vector and comparator).
 *
 * @return Newly-allocated PriorityQueue with the same contents, or
 *         NULL on any failure (NULL src / OOM / underlying vector op
 *         failure).
 */
PriorityQueue* priority_queue_copy(const PriorityQueue* src) {
    PQUEUE_LOG("[priority_queue_copy]: enter src=%p", (const void*)src);

    if (!src || !src->vec || !src->compare) {
        PQUEUE_LOG("[priority_queue_copy]: NULL src / vec / compare -> NULL");
        return NULL;
    }

    PriorityQueue* dst = priority_queue_create(src->vec->itemSize, src->compare);
    if (!dst) {
        PQUEUE_LOG("[priority_queue_copy]: priority_queue_create(itemSize=%zu) failed -> NULL", src->vec->itemSize);
        return NULL;
    }

    size_t n = vector_size(src->vec);
    if (n > 0 && !vector_reserve(dst->vec, n)) {
        PQUEUE_LOG("[priority_queue_copy]: vector_reserve(%zu) failed -> NULL", n);
        priority_queue_deallocate(dst);

        return NULL;
    }
    for (size_t i = 0; i < n; ++i) {
        void* elem = vector_at(src->vec, i);
        if (!elem || !vector_push_back(dst->vec, elem)) {
            PQUEUE_LOG("[priority_queue_copy]: push_back failed at index %zu -> NULL", i);
            priority_queue_deallocate(dst);

            return NULL;
        }
    }

    PQUEUE_LOG("[priority_queue_copy]: exit ok dst=%p (%zu elements copied)", (void*)dst, n);
    return dst;
}


/**
 * @brief Replace @p dest's contents with the elements of @p src.
 *
 * Behavior:
 *   - `dest == src` is a no-op success.
 *   - `dest->itemSize != src->itemSize` is rejected (false; dest is
 *     left unmodified).
 *   - Otherwise dest is cleared, capacity is reserved for src's size,
 *     every element is copied, then the heap is rebuilt under DEST's
 *     comparator via Floyd's O(n) bottom-up heapify (so a max-heap
 *     can be assigned from a min-heap and end up correctly ordered).
 *
 * @param dest Destination queue (must be non-NULL with a valid
 *             comparator).
 * @param src  Source queue (must be non-NULL).
 *
 * @return true on success, false on NULL input / item-size mismatch /
 *         underlying vector op failure.
 */
bool priority_queue_assign(PriorityQueue* dest, const PriorityQueue* src) {
    PQUEUE_LOG("[priority_queue_assign]: enter dest=%p src=%p", (void*)dest, (const void*)src);

    if (!dest || !dest->vec || !dest->compare || !src || !src->vec) {
        PQUEUE_LOG("[priority_queue_assign]: NULL receiver / src -> false");
        return false;
    }
    if (dest == src) {
        PQUEUE_LOG("[priority_queue_assign]: dest == src, no-op success");
        return true;
    }
    if (dest->vec->itemSize != src->vec->itemSize) {
        PQUEUE_LOG("[priority_queue_assign]: item-size mismatch (%zu vs %zu) -> false", dest->vec->itemSize, src->vec->itemSize);
        return false;
    }

    vector_clear(dest->vec);

    size_t n = vector_size(src->vec);
    if (n > 0 && !vector_reserve(dest->vec, n)) {
        PQUEUE_LOG("[priority_queue_assign]: reserve(%zu) failed -> false", n);
        return false;
    }
    for (size_t i = 0; i < n; ++i) {
        void* elem = vector_at(src->vec, i);
        if (!elem || !vector_push_back(dest->vec, elem)) {
            PQUEUE_LOG("[priority_queue_assign]: push_back failed at index %zu -> false", i);
            return false;
        }
    }
    if (n > 1) {
        for (size_t i = (n / 2); i-- > 0; ) {
            pq_sift_down(dest->vec, i, dest->compare);
        }
    }

    PQUEUE_LOG("[priority_queue_assign]: exit ok (%zu elements assigned)", n);
    return true;
}


/**
 * @brief Destroy the priority queue and release all memory it owns.
 *
 * Frees both the underlying vector AND the PriorityQueue header.
 * NULL-safe.
 *
 * @param pq Queue to destroy. Pass NULL for a no-op.
 */
void priority_queue_deallocate(PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_deallocate]: enter pq=%p", (void*)pq);
    if (!pq) {
        PQUEUE_LOG("[priority_queue_deallocate]: NULL receiver, no-op");
        return;
    }
    if (pq->vec) {
        PQUEUE_LOG("[priority_queue_deallocate]: freeing vec=%p", (void*)pq->vec);
        vector_deallocate(pq->vec);
        pq->vec = NULL;
    }
    free(pq);
    PQUEUE_LOG("[priority_queue_deallocate]: exit (queue header freed)");
}


/**
 * @brief Returns true if the priority queue has no elements (or is NULL).
 *
 * @param pq Queue pointer. NULL is treated as empty (returns true).
 * @return true if `pq == NULL` or `priority_queue_size(pq) == 0`.
 */
bool priority_queue_empty(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_empty]: enter pq=%p", (const void*)pq);
    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_empty]: NULL receiver -> true");
        return true;
    }
    bool out = vector_is_empty(pq->vec);
    PQUEUE_LOG("[priority_queue_empty]: exit -> %s", out ? "true" : "false");
    return out;
}


/**
 * @brief Number of elements currently in the heap.
 *
 * @param pq Queue pointer. NULL returns 0.
 * @return Element count.
 */
size_t priority_queue_size(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_size]: enter pq=%p", (const void*)pq);

    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_size]: NULL receiver -> 0");
        return 0;
    }
    size_t out = vector_size(pq->vec);
    PQUEUE_LOG("[priority_queue_size]: exit -> %zu", out);

    return out;
}


/**
 * @brief Allocated capacity of the underlying vector.
 *
 * Capacity ≥ size; the queue can absorb (capacity − size) more pushes
 * without re-allocating.
 *
 * @param pq Queue pointer. NULL returns 0.
 * @return Capacity in elements.
 */
size_t priority_queue_capacity(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_capacity]: enter pq=%p", (const void*)pq);

    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_capacity]: NULL receiver -> 0");
        return 0;
    }
    size_t out = vector_capacity((Vector*)pq->vec);
    PQUEUE_LOG("[priority_queue_capacity]: exit -> %zu", out);

    return out;
}


/**
 * @brief Bytes per element (as supplied to @ref priority_queue_create).
 *
 * @param pq Queue pointer. NULL returns 0.
 * @return Per-element size in bytes.
 */
size_t priority_queue_item_size(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_item_size]: enter pq=%p", (const void*)pq);

    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_item_size]: NULL receiver -> 0");
        return 0;
    }
    size_t out = pq->vec->itemSize;
    PQUEUE_LOG("[priority_queue_item_size]: exit -> %zu", out);

    return out;
}


/**
 * @brief Borrowed pointer to the highest-priority element (heap root).
 *
 * This is the element that @ref priority_queue_pop will remove next.
 * The returned pointer becomes stale the moment the underlying buffer
 * is reallocated (any push / emplace / pop / reserve / clear / swap /
 * assign can invalidate it).
 *
 * @param pq Queue pointer.
 * @return Pointer to the root element, or NULL if @p pq is NULL or empty.
 */
void* priority_queue_top(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_top]: enter pq=%p", (const void*)pq);
    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_top]: NULL receiver -> NULL");
        return NULL;
    }
    if (vector_is_empty(pq->vec)) {
        PQUEUE_LOG("[priority_queue_top]: empty queue -> NULL");
        return NULL;
    }

    void* out = vector_front(pq->vec);
    PQUEUE_LOG("[priority_queue_top]: exit -> %p", out);

    return out;
}


/**
 * @brief Alias for @ref priority_queue_top.
 *
 * Provided for API symmetry with `queue_front` callers.
 *
 * @param pq Queue pointer.
 * @return Same as @ref priority_queue_top.
 */
void* priority_queue_front(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_front]: enter pq=%p (delegating to top)", (const void*)pq);
    return priority_queue_top(pq);
}


/**
 * @brief Borrowed pointer to the LAST slot of the underlying heap
 *        storage.
 *
 * Diagnostic / iteration helper only — this is NOT in general the
 * lowest-priority element. The heap property only constrains
 * parent/child relationships; the leaves do not form a total order.
 * Use sorted-iteration via repeated pop if you need ordering.
 *
 * @param pq Queue pointer.
 * @return Pointer to the last storage slot, or NULL if @p pq is NULL
 *         or empty.
 */
void* priority_queue_back(const PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_back]: enter pq=%p", (const void*)pq);
    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_back]: NULL receiver -> NULL");
        return NULL;
    }
    if (vector_is_empty(pq->vec)) {
        PQUEUE_LOG("[priority_queue_back]: empty queue -> NULL");
        return NULL;
    }

    void* out = vector_back(pq->vec);
    PQUEUE_LOG("[priority_queue_back]: exit -> %p", out);

    return out;
}


/**
 * @brief Insert a copy of @p item at the appropriate heap position.
 *
 * Copies exactly `priority_queue_item_size(pq)` bytes from @p item
 * into the heap then sifts it up until the heap property is restored.
 * The caller's buffer can be reused or freed afterwards — the heap
 * owns the copy.
 *
 * @param pq   Destination queue.
 * @param item Pointer to the value to copy (must be non-NULL).
 *
 * @return true on success, false on NULL inputs or underlying vector
 *         push failure (e.g. OOM).
 *
 * @code
 * int x = 42;
 * priority_queue_push(pq, &x);
 * @endcode
 */
bool priority_queue_push(PriorityQueue* pq, const void* item) {
    PQUEUE_LOG("[priority_queue_push]: enter pq=%p item=%p", (void*)pq, item);

    if (!pq || !pq->vec || !pq->compare || !item) {
        PQUEUE_LOG("[priority_queue_push]: NULL receiver / item -> false");
        return false;
    }
    if (!vector_push_back(pq->vec, item)) {
        PQUEUE_LOG("[priority_queue_push]: vector_push_back failed -> false");
        return false;
    }
    size_t final_idx = pq_sift_up(pq->vec, vector_size(pq->vec) - 1, pq->compare);
    (void)final_idx;
    PQUEUE_LOG("[priority_queue_push]: exit ok (new size = %zu, final idx = %zu)", vector_size(pq->vec), final_idx);

    return true;
}


/**
 * @brief Construct-in-place and return a borrowed pointer to the
 *        element at its FINAL heap position (after sift-up).
 *
 * Equivalent in observable behaviour to @ref priority_queue_push,
 * except that @p itemSize is validated against the queue's per-element
 * size AND the return value is a pointer to the slot the element ended
 * up in (after the sift-up the index is not necessarily the back).
 *
 * @param pq       Destination queue.
 * @param item     Pointer to the source bytes (must be non-NULL).
 * @param itemSize Must equal `priority_queue_item_size(pq)`.
 *
 * @return Borrowed pointer to the final position of the new element,
 *         or NULL on NULL input / size mismatch / OOM. The pointer is
 *         valid until the next reallocation; see the Container model
 *         note in the file header.
 */
void* priority_queue_emplace(PriorityQueue* pq, const void* item, size_t itemSize) {
    PQUEUE_LOG("[priority_queue_emplace]: enter pq=%p item=%p itemSize=%zu", (void*)pq, item, itemSize);

    if (!pq || !pq->vec || !pq->compare || !item) {
        PQUEUE_LOG("[priority_queue_emplace]: NULL receiver / item -> NULL");
        return NULL;
    }
    if (itemSize != pq->vec->itemSize) {
        PQUEUE_LOG("[priority_queue_emplace]: item-size mismatch (%zu vs %zu) -> NULL", itemSize, pq->vec->itemSize);
        return NULL;
    }
    if (!vector_emplace_back(pq->vec, (void*)item, itemSize)) {
        PQUEUE_LOG("[priority_queue_emplace]: vector_emplace_back failed -> NULL");
        return NULL;
    }
    size_t final_idx = pq_sift_up(pq->vec, vector_size(pq->vec) - 1, pq->compare);
    void* out = (char*)pq->vec->items + final_idx * pq->vec->itemSize;
    PQUEUE_LOG("[priority_queue_emplace]: exit -> %p (final idx = %zu, new size = %zu)", out, final_idx, vector_size(pq->vec));

    return out;
}


/**
 * @brief Remove the highest-priority element (the heap root).
 *
 * Swaps the root with the last element, drops the last slot, then
 * sifts the new root down. O(log n). No-op on NULL or empty queue.
 *
 * Iterator / pointer invalidation: any borrowed pointer obtained from
 * @ref priority_queue_top / `_front` / `_back` / `_emplace` BEFORE
 * this call should be considered stale afterwards.
 *
 * @param pq Queue to mutate.
 */
void priority_queue_pop(PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_pop]: enter pq=%p", (void*)pq);

    if (!pq || !pq->vec || !pq->compare) {
        PQUEUE_LOG("[priority_queue_pop]: NULL receiver, no-op");
        return;
    }
    size_t n = vector_size(pq->vec);
    if (n == 0) {
        PQUEUE_LOG("[priority_queue_pop]: empty queue, no-op");
        return;
    }
    if (n > 1) {
        char*  base = (char*)pq->vec->items;
        size_t isz  = pq->vec->itemSize;

        pq_move(base, base + (n - 1) * isz, isz);
    }
    pq->vec->size--;   /* inlined vector_pop_back: size > 0 already checked above */
    pq_sift_down(pq->vec, 0, pq->compare);

    PQUEUE_LOG("[priority_queue_pop]: exit (size %zu -> %zu)", n, vector_size(pq->vec));
}


/**
 * @brief Pre-allocate underlying storage for at least @p n elements.
 *
 * Useful before a tight bulk-push loop to avoid mid-loop reallocations
 * (each of which would invalidate any outstanding borrowed pointers).
 *
 * @param pq Queue to grow.
 * @param n  Target minimum capacity.
 *
 * @return true on success, false on NULL queue or underlying
 *         vector_reserve failure (e.g. OOM).
 */
bool priority_queue_reserve(PriorityQueue* pq, size_t n) {
    PQUEUE_LOG("[priority_queue_reserve]: enter pq=%p n=%zu", (void*)pq, n);

    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_reserve]: NULL receiver -> false");
        return false;
    }
    if (!vector_reserve(pq->vec, n)) {
        PQUEUE_LOG("[priority_queue_reserve]: vector_reserve(%zu) failed -> false", n);
        return false;
    }

    PQUEUE_LOG("[priority_queue_reserve]: exit ok (capacity = %zu)", vector_capacity(pq->vec));
    return true;
}


/**
 * @brief Remove every element. Underlying capacity is retained.
 *
 * After this call `priority_queue_empty(pq) == true` and
 * `priority_queue_size(pq) == 0`, but subsequent pushes can reuse the
 * existing buffer without re-allocating up to its current capacity.
 *
 * @param pq Queue to clear. NULL is a no-op.
 */
void priority_queue_clear(PriorityQueue* pq) {
    PQUEUE_LOG("[priority_queue_clear]: enter pq=%p", (void*)pq);
    if (!pq || !pq->vec) {
        PQUEUE_LOG("[priority_queue_clear]: NULL receiver, no-op");
        return;
    }

    size_t before = vector_size(pq->vec);
    vector_clear(pq->vec);

    PQUEUE_LOG("[priority_queue_clear]: exit (cleared %zu elements; capacity = %zu)", before, vector_capacity(pq->vec));
    (void)before;
}


/**
 * @brief Constant-time swap of two priority queues' internals.
 *
 * Only the underlying vector pointer AND the comparator are swapped
 * — no element-by-element copy. Rejects (no-op) when item sizes
 * differ, since `std::priority_queue::swap` is only well-defined
 * between same-type queues.
 *
 * @param pq1 First queue.
 * @param pq2 Second queue.
 */
void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2) {
    PQUEUE_LOG("[priority_queue_swap]: enter pq1=%p pq2=%p", (void*)pq1, (void*)pq2);

    if (!pq1 || !pq2 || !pq1->vec || !pq2->vec) {
        PQUEUE_LOG("[priority_queue_swap]: NULL receiver, no-op");
        return;
    }
    if (pq1->vec->itemSize != pq2->vec->itemSize) {
        PQUEUE_LOG("[priority_queue_swap]: refused: item sizes differ (%zu vs %zu)", pq1->vec->itemSize, pq2->vec->itemSize);
        return;
    }

    Vector*       tv = pq1->vec;
    pq1->vec = pq2->vec;
    pq2->vec = tv;

    PQCompareFunc tc = pq1->compare;
    pq1->compare = pq2->compare;
    pq2->compare = tc;

    PQUEUE_LOG("[priority_queue_swap]: exit (pq1.size=%zu, pq2.size=%zu)", vector_size(pq1->vec), vector_size(pq2->vec));
}


/**
 * @brief Byte-exact equality of the underlying heap storage.
 *
 * Two queues are equal iff they point to the same PriorityQueue OR
 * they have matching size, item size, and every byte of storage
 * matches.
 *
 * NOTE: this compares STORAGE layout. Two heaps that contain the same
 * multiset of elements but were built in a different push order can
 * end up with different layouts and therefore compare unequal.
 *
 * @param a Left queue.
 * @param b Right queue.
 * @return true if the queues are byte-exact equal.
 */
bool priority_queue_is_equal(const PriorityQueue* a, const PriorityQueue* b) {
    PQUEUE_LOG("[priority_queue_is_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    if (a == b) {
        PQUEUE_LOG("[priority_queue_is_equal]: identity (a == b) -> true");
        return true;
    }
    if (!a || !b || !a->vec || !b->vec) {
        PQUEUE_LOG("[priority_queue_is_equal]: one receiver NULL -> false");
        return false;
    }

    bool out = vector_is_equal(a->vec, b->vec);
    PQUEUE_LOG("[priority_queue_is_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Inverse of @ref priority_queue_is_equal.
 *
 * @param a Left queue.
 * @param b Right queue.
 * @return `!priority_queue_is_equal(a, b)`.
 */
bool priority_queue_is_not_equal(const PriorityQueue* a, const PriorityQueue* b) {
    PQUEUE_LOG("[priority_queue_is_not_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    bool out = !priority_queue_is_equal(a, b);
    PQUEUE_LOG("[priority_queue_is_not_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Lexicographic less-than over the underlying storage bytes.
 *
 * Returns false if either operand is NULL.
 *
 * @param a Left queue.
 * @param b Right queue.
 * @return true if @p a is lexicographically less than @p b.
 */
bool priority_queue_is_less(const PriorityQueue* a, const PriorityQueue* b) {
    PQUEUE_LOG("[priority_queue_is_less]: enter a=%p b=%p", (const void*)a, (const void*)b);
    if (!a || !b || !a->vec || !b->vec) {
        PQUEUE_LOG("[priority_queue_is_less]: one receiver NULL -> false");
        return false;
    }

    bool out = vector_is_less(a->vec, b->vec);
    PQUEUE_LOG("[priority_queue_is_less]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Lexicographic greater-than over the underlying storage bytes.
 *
 * Returns false if either operand is NULL.
 *
 * @param a Left queue.
 * @param b Right queue.
 * @return true if @p a is lexicographically greater than @p b.
 */
bool priority_queue_is_greater(const PriorityQueue* a, const PriorityQueue* b) {
    PQUEUE_LOG("[priority_queue_is_greater]: enter a=%p b=%p", (const void*)a, (const void*)b);
    if (!a || !b || !a->vec || !b->vec) {
        PQUEUE_LOG("[priority_queue_is_greater]: one receiver NULL -> false");
        return false;
    }

    bool out = vector_is_greater(a->vec, b->vec);
    PQUEUE_LOG("[priority_queue_is_greater]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Less-than-or-equal.
 *
 * @param a Left queue.
 * @param b Right queue.
 * @return `priority_queue_is_less(a, b) || priority_queue_is_equal(a, b)`.
 */
bool priority_queue_is_less_or_equal(const PriorityQueue* a, const PriorityQueue* b) {
    PQUEUE_LOG("[priority_queue_is_less_or_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    bool out = priority_queue_is_less(a, b) || priority_queue_is_equal(a, b);
    PQUEUE_LOG("[priority_queue_is_less_or_equal]: exit -> %s", out ? "true" : "false");
    
    return out;
}


/**
 * @brief Greater-than-or-equal.
 *
 * @param a Left queue.
 * @param b Right queue.
 * @return `priority_queue_is_greater(a, b) || priority_queue_is_equal(a, b)`.
 */
bool priority_queue_is_greater_or_equal(const PriorityQueue* a, const PriorityQueue* b) {
    PQUEUE_LOG("[priority_queue_is_greater_or_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    bool out = priority_queue_is_greater(a, b) || priority_queue_is_equal(a, b);
    PQUEUE_LOG("[priority_queue_is_greater_or_equal]: exit -> %s", out ? "true" : "false");

    return out;
}
