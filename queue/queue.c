/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Queue
 *
 * std::queue-style FIFO adapter over Vector.
 *
 * Error model
 * -----------
 *   - No global error state. Failures are reported through the return
 *     value: NULL for pointer-returning functions, false for bool-
 *     returning ones, 0 for size_t observers, "no-op" for void mutators.
 *   - Diagnostics are emitted through QUEUE_LOG (opt-in via the
 *     QUEUE_LOGGING_ENABLE macro).
 *   - No library function ever calls exit(), abort(), or assert().
 *   - Every public function is NULL-safe in the documented sense (see
 *     each function's contract).
 *
 * Container model
 * ---------------
 *   - Storage is a contiguous Vector owned by the Queue. Push goes to
 *     the back; pop drops the front (and shifts the remainder one slot).
 *   - All element pointers returned by `queue_front`, `queue_back`,
 *     `queue_at`, and `queue_emplace` are BORROWED — they live as long
 *     as the underlying vector buffer isn't reallocated. Any push /
 *     emplace / reserve / sort / pop can invalidate them.
 *
 * Performance note
 * ----------------
 *   `queue_pop` is O(n) because the underlying vector is contiguous.
 *   If you need O(1) pop semantics, prefer a deque- or linked-list-
 *   backed FIFO.
 */

#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "../algorithm/algorithm.h"



/**
 * @brief Create an empty queue whose elements are @p itemSize bytes each.
 *
 * Allocates a Queue header and an empty backing Vector. No elements
 * are allocated until you push.
 *
 * @param itemSize Size in bytes of one element (must be > 0).
 *
 * @return Newly-allocated Queue the caller must free with
 *         @ref queue_deallocate, or NULL on:
 *           - `itemSize == 0`
 *           - Queue-header malloc failure
 *           - underlying vector_create failure
 *
 * @code
 * Queue* q = queue_create(sizeof(int));
 * if (!q) { return; }   // OOM or bad itemSize
 * int x = 42;
 * queue_push(q, &x);
 * queue_deallocate(q);
 * @endcode
 */
Queue* queue_create(size_t itemSize) {
    QUEUE_LOG("[queue_create]: enter itemSize=%zu", itemSize);

    if (itemSize == 0) {
        QUEUE_LOG("[queue_create]: itemSize is zero -> NULL");
        return NULL;
    }

    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        QUEUE_LOG("[queue_create]: malloc(sizeof(Queue)) failed -> NULL");
        return NULL;
    }

    q->vec = vector_create(itemSize);
    if (!q->vec) {
        QUEUE_LOG("[queue_create]: vector_create(%zu) failed -> NULL", itemSize);
        free(q);
        return NULL;
    }
    QUEUE_LOG("[queue_create]: exit ok q=%p vec=%p", (void*)q, (void*)q->vec);
    return q;
}


/**
 * @brief Deep-copy an existing queue.
 *
 * The returned queue owns its own backing vector and an independent
 * copy of every element. Element ordering is preserved.
 *
 * @param src Source queue to clone (must be non-NULL with a valid vector).
 *
 * @return Newly-allocated Queue with the same item size and contents,
 *         or NULL on any failure (NULL src / OOM / vector op failure).
 */
Queue* queue_copy(const Queue* src) {
    QUEUE_LOG("[queue_copy]: enter src=%p", (const void*)src);

    if (!src || !src->vec) {
        QUEUE_LOG("[queue_copy]: NULL src or src->vec -> NULL");
        return NULL;
    }

    Queue* dst = queue_create(src->vec->itemSize);
    if (!dst) {
        QUEUE_LOG("[queue_copy]: queue_create(itemSize=%zu) failed -> NULL", src->vec->itemSize);
        return NULL;
    }

    size_t n = vector_size(src->vec);
    if (n > 0 && !vector_reserve(dst->vec, n)) {
        QUEUE_LOG("[queue_copy]: vector_reserve(%zu) failed -> NULL", n);
        queue_deallocate(dst);
        return NULL;
    }
    for (size_t i = 0; i < n; ++i) {
        void* elem = vector_at(src->vec, i);
        if (!elem || !vector_push_back(dst->vec, elem)) {
            QUEUE_LOG("[queue_copy]: copy failed at index %zu -> NULL", i);
            queue_deallocate(dst);
            return NULL;
        }
    }
    QUEUE_LOG("[queue_copy]: exit ok dst=%p (%zu elements copied)", (void*)dst, n);
    return dst;
}


/**
 * @brief Replace @p dest's contents with a deep copy of @p src.
 *
 * Behavior:
 *   - `dest == src` is a no-op success.
 *   - `dest->itemSize != src->itemSize` is rejected (false; dest is
 *     left unmodified).
 *   - Otherwise dest is cleared, capacity is reserved for src's size,
 *     and every element is copied in order.
 *
 * @param dest Destination queue (must be non-NULL).
 * @param src  Source queue (must be non-NULL).
 *
 * @return true on success, false on NULL input / item-size mismatch /
 *         underlying vector op failure.
 */
bool queue_assign(Queue* dest, const Queue* src) {
    QUEUE_LOG("[queue_assign]: enter dest=%p src=%p", (void*)dest, (const void*)src);

    if (!dest || !dest->vec || !src || !src->vec) {
        QUEUE_LOG("[queue_assign]: NULL receiver / src -> false");
        return false;
    }
    if (dest == src) {
        QUEUE_LOG("[queue_assign]: dest == src, no-op success");
        return true;
    }
    if (dest->vec->itemSize != src->vec->itemSize) {
        QUEUE_LOG("[queue_assign]: item-size mismatch (%zu vs %zu) -> false", dest->vec->itemSize, src->vec->itemSize);
        return false;
    }

    vector_clear(dest->vec);

    size_t n = vector_size(src->vec);
    if (n > 0 && !vector_reserve(dest->vec, n)) {
        QUEUE_LOG("[queue_assign]: reserve(%zu) failed -> false", n);
        return false;
    }
    for (size_t i = 0; i < n; ++i) {
        void* elem = vector_at(src->vec, i);
        if (!elem || !vector_push_back(dest->vec, elem)) {
            QUEUE_LOG("[queue_assign]: push_back failed at index %zu -> false", i);
            return false;
        }
    }
    QUEUE_LOG("[queue_assign]: exit ok (%zu elements assigned)", n);
    return true;
}


/**
 * @brief Destroy the queue and release all memory it owns.
 *
 * Frees both the underlying vector AND the Queue header itself. NULL-safe.
 *
 * @param q Queue to destroy. Pass NULL for a no-op.
 */
void queue_deallocate(Queue* q) {
    QUEUE_LOG("[queue_deallocate]: enter q=%p", (void*)q);
    if (!q) {
        QUEUE_LOG("[queue_deallocate]: NULL receiver, no-op");
        return;
    }
    if (q->vec) {
        QUEUE_LOG("[queue_deallocate]: freeing vec=%p", (void*)q->vec);
        vector_deallocate(q->vec);
        q->vec = NULL;
    }

    free(q);
    QUEUE_LOG("[queue_deallocate]: exit (queue header freed)");
}


/**
 * @brief Number of elements currently in the queue.
 *
 * @param q Queue pointer. NULL returns 0.
 * @return Element count, mirroring `std::queue::size`.
 */
size_t queue_size(const Queue* q) {
    QUEUE_LOG("[queue_size]: enter q=%p", (const void*)q);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_size]: NULL receiver -> 0");
        return 0;
    }
    size_t out = vector_size(q->vec);
    QUEUE_LOG("[queue_size]: exit -> %zu", out);

    return out;
}


/**
 * @brief Allocated capacity of the underlying vector.
 *
 * Capacity ≥ size; the queue can absorb (capacity - size) more
 * pushes without re-allocating.
 *
 * @param q Queue pointer. NULL returns 0.
 * @return Capacity in elements.
 */
size_t queue_capacity(const Queue* q) {
    QUEUE_LOG("[queue_capacity]: enter q=%p", (const void*)q);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_capacity]: NULL receiver -> 0");
        return 0;
    }
    size_t out = vector_capacity((Vector*)q->vec);
    QUEUE_LOG("[queue_capacity]: exit -> %zu", out);

    return out;
}


/**
 * @brief Bytes per element (as supplied to @ref queue_create).
 *
 * @param q Queue pointer. NULL returns 0.
 * @return Per-element size in bytes.
 */
size_t queue_item_size(const Queue* q) {
    QUEUE_LOG("[queue_item_size]: enter q=%p", (const void*)q);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_item_size]: NULL receiver -> 0");
        return 0;
    }
    size_t out = q->vec->itemSize;
    QUEUE_LOG("[queue_item_size]: exit -> %zu", out);

    return out;
}


/**
 * @brief Returns true if the queue has no elements (or is NULL).
 *
 * @param q Queue pointer. NULL is treated as empty (returns true).
 * @return true if `q == NULL` or `queue_size(q) == 0`.
 */
bool queue_empty(const Queue* q) {
    QUEUE_LOG("[queue_empty]: enter q=%p", (const void*)q);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_empty]: NULL receiver -> true");
        return true;
    }
    bool out = vector_is_empty(q->vec);
    QUEUE_LOG("[queue_empty]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Borrowed pointer to the FRONT (oldest, FIFO peek) element.
 *
 * @param q Queue pointer.
 * @return Pointer to the next element @ref queue_pop will remove,
 *         or NULL if @p q is NULL or empty.
 */
void* queue_front(const Queue* q) {
    QUEUE_LOG("[queue_front]: enter q=%p", (const void*)q);
    if (!q || !q->vec) {
        QUEUE_LOG("[queue_front]: NULL receiver -> NULL");
        return NULL;
    }
    if (vector_is_empty(q->vec)) {
        QUEUE_LOG("[queue_front]: empty queue -> NULL");
        return NULL;
    }
    void* out = vector_front(q->vec);
    QUEUE_LOG("[queue_front]: exit -> %p", out);

    return out;
}


/**
 * @brief Borrowed pointer to the BACK (newest) element.
 *
 * @param q Queue pointer.
 * @return Pointer to the most recently pushed element, or NULL if
 *         @p q is NULL or empty.
 */
void* queue_back(const Queue* q) {
    QUEUE_LOG("[queue_back]: enter q=%p", (const void*)q);
    if (!q || !q->vec) {
        QUEUE_LOG("[queue_back]: NULL receiver -> NULL");
        return NULL;
    }
    if (vector_is_empty(q->vec)) {
        QUEUE_LOG("[queue_back]: empty queue -> NULL");
        return NULL;
    }
    void* out = vector_back(q->vec);
    QUEUE_LOG("[queue_back]: exit -> %p", out);

    return out;
}


/**
 * @brief Bounds-checked random-access borrowed pointer.
 *
 * Index 0 is the front (next to pop), index `size - 1` is the back.
 *
 * @param q     Queue pointer.
 * @param index Zero-based element index.
 *
 * @return Element pointer, or NULL when @p q is NULL or
 *         `index >= queue_size(q)`.
 */
void* queue_at(const Queue* q, size_t index) {
    QUEUE_LOG("[queue_at]: enter q=%p index=%zu", (const void*)q, index);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_at]: NULL receiver -> NULL");
        return NULL;
    }
    if (index >= vector_size(q->vec)) {
        QUEUE_LOG("[queue_at]: out of bounds (size=%zu, index=%zu) -> NULL", vector_size(q->vec), index);
        return NULL;
    }

    void* out = vector_at(q->vec, index);
    QUEUE_LOG("[queue_at]: exit -> %p", out);

    return out;
}


/**
 * @brief Push a copy of @p item onto the back of the queue.
 *
 * Copies exactly `queue_item_size(q)` bytes from @p item into the
 * queue. The caller's buffer can be reused or freed afterwards — the
 * queue owns the copy.
 *
 * @param q    Destination queue.
 * @param item Pointer to the value to copy. Must NOT be NULL.
 *
 * @return true on success, false on NULL inputs or underlying vector
 *         push failure (e.g. OOM).
 *
 * @code
 * Queue* q = queue_create(sizeof(int));
 * for (int i = 1; i <= 5; ++i) queue_push(q, &i);
 * // queue_front -> 1, queue_back -> 5
 * @endcode
 */
bool queue_push(Queue* q, const void* item) {
    QUEUE_LOG("[queue_push]: enter q=%p item=%p", (void*)q, item);

    if (!q || !q->vec || !item) {
        QUEUE_LOG("[queue_push]: NULL receiver / item -> false");
        return false;
    }
    if (!vector_push_back(q->vec, item)) {
        QUEUE_LOG("[queue_push]: vector_push_back failed -> false");
        return false;
    }
    QUEUE_LOG("[queue_push]: exit ok (new size = %zu)", vector_size(q->vec));
    return true;
}


/**
 * @brief Construct-in-place at the back and return a borrowed pointer
 *        to the new element.
 *
 * Equivalent in observable behaviour to `queue_push` except that the
 * `itemSize` argument is validated against the queue's per-element
 * size, AND the return value is a pointer to the newly-inserted slot
 * (the borrowed pointer is valid until the next reallocation; see the
 * Element access section).
 *
 * @param q        Destination queue.
 * @param item     Pointer to the source bytes (must be non-NULL).
 * @param itemSize Must equal `queue_item_size(q)`; otherwise rejected.
 *
 * @return Borrowed pointer to the new back element on success, or
 *         NULL on NULL input / size mismatch / OOM.
 */
void* queue_emplace(Queue* q, const void* item, size_t itemSize) {
    QUEUE_LOG("[queue_emplace]: enter q=%p item=%p itemSize=%zu", (void*)q, item, itemSize);

    if (!q || !q->vec || !item) {
        QUEUE_LOG("[queue_emplace]: NULL receiver / item -> NULL");
        return NULL;
    }
    if (itemSize != q->vec->itemSize) {
        QUEUE_LOG("[queue_emplace]: item-size mismatch (%zu vs %zu) -> NULL", itemSize, q->vec->itemSize);
        return NULL;
    }
    if (!vector_emplace_back(q->vec, (void*)item, itemSize)) {
        QUEUE_LOG("[queue_emplace]: vector_emplace_back failed -> NULL");
        return NULL;
    }
    void* out = vector_back(q->vec);
    QUEUE_LOG("[queue_emplace]: exit -> %p (new size = %zu)", out, vector_size(q->vec));

    return out;
}


/**
 * @brief Drop the FRONT element (the next one a peek would return).
 *
 * O(n) — shifts the remaining `size - 1` elements one slot left in the
 * underlying vector. No-op on NULL or empty queue.
 *
 * Iterator / pointer invalidation: any borrowed pointer obtained from
 * `queue_front` / `queue_back` / `queue_at` / `queue_emplace` BEFORE
 * this call should be considered stale afterwards.
 *
 * @param q Queue to mutate.
 */
void queue_pop(Queue* q) {
    QUEUE_LOG("[queue_pop]: enter q=%p", (void*)q);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_pop]: NULL receiver, no-op");
        return;
    }
    if (vector_is_empty(q->vec)) {
        QUEUE_LOG("[queue_pop]: empty queue, no-op");
        return;
    }

    QUEUE_LOG("[queue_pop]: erasing front (size before = %zu)", vector_size(q->vec));
    vector_erase(q->vec, 0, 1);
    QUEUE_LOG("[queue_pop]: exit (size after = %zu)", vector_size(q->vec));
}


/**
 * @brief Pre-allocate underlying storage for at least @p n elements.
 *
 * Useful before a tight bulk-push loop to avoid mid-loop reallocations.
 *
 * @param q Queue to grow.
 * @param n Target minimum capacity.
 *
 * @return true on success, false on NULL queue or underlying
 *         vector_reserve failure (e.g. OOM).
 */
bool queue_reserve(Queue* q, size_t n) {
    QUEUE_LOG("[queue_reserve]: enter q=%p n=%zu", (void*)q, n);

    if (!q || !q->vec) {
        QUEUE_LOG("[queue_reserve]: NULL receiver -> false");
        return false;
    }
    if (!vector_reserve(q->vec, n)) {
        QUEUE_LOG("[queue_reserve]: vector_reserve(%zu) failed -> false", n);
        return false;
    }
    QUEUE_LOG("[queue_reserve]: exit ok (capacity = %zu)", vector_capacity(q->vec));

    return true;
}


/**
 * @brief Constant-time swap of two queues' internals.
 *
 * Only the underlying vector pointers are swapped — no element-by-
 * element copy. Rejects (no-op) when item sizes differ, since
 * `std::queue::swap` is only well-defined between same-type queues.
 *
 * @param q1 First queue.
 * @param q2 Second queue.
 */
void queue_swap(Queue* q1, Queue* q2) {
    QUEUE_LOG("[queue_swap]: enter q1=%p q2=%p", (void*)q1, (void*)q2);

    if (!q1 || !q2 || !q1->vec || !q2->vec) {
        QUEUE_LOG("[queue_swap]: NULL receiver, no-op");
        return;
    }
    if (q1->vec->itemSize != q2->vec->itemSize) {
        QUEUE_LOG("[queue_swap]: refused: item sizes differ (%zu vs %zu)", q1->vec->itemSize, q2->vec->itemSize);
        return;
    }

    Vector* tmp = q1->vec;
    q1->vec = q2->vec;
    q2->vec = tmp;

    QUEUE_LOG("[queue_swap]: exit (q1.size=%zu, q2.size=%zu)", vector_size(q1->vec), vector_size(q2->vec));
}


/**
 * @brief Sort the queue in-place using @p comp (qsort-style 3-way
 *        comparator).
 *
 * After sorting, @ref queue_front returns the smallest element under
 * the given ordering — i.e. the queue becomes a min-priority FIFO.
 * No-op on NULL queue or NULL comparator.
 *
 * Iterator / pointer invalidation: any borrowed element pointer
 * obtained before this call should be considered stale afterwards.
 *
 * @param q    Queue to sort.
 * @param comp Comparator returning negative / 0 / positive in the
 *             same convention as `qsort`.
 */
void queue_sort(Queue* q, QueueCompareFunc comp) {
    QUEUE_LOG("[queue_sort]: enter q=%p comp=%p", (void*)q, (void*)comp);

    if (!q || !q->vec || !comp) {
        QUEUE_LOG("[queue_sort]: NULL receiver or comp, no-op");
        return;
    }
    size_t n = vector_size(q->vec);
    algorithm_sort(vector_data(q->vec), n, q->vec->itemSize, comp);

    QUEUE_LOG("[queue_sort]: exit (sorted %zu elements)", n);
}


/**
 * @brief Byte-exact equality between two queues (NULL == NULL).
 *
 * Two queues are equal iff they point to the same Queue OR they have
 * matching size, item size, and every element compares equal byte-
 * for-byte.
 *
 * @param q1 Left queue.
 * @param q2 Right queue.
 * @return true if the queues are byte-exact equal.
 */
bool queue_is_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_equal]: enter q1=%p q2=%p", (const void*)q1, (const void*)q2);

    if (q1 == q2) {
        QUEUE_LOG("[queue_is_equal]: identity (q1 == q2) -> true");
        return true;
    }
    if (!q1 || !q2 || !q1->vec || !q2->vec) {
        QUEUE_LOG("[queue_is_equal]: one receiver NULL -> false");
        return false;
    }
    bool out = vector_is_equal(q1->vec, q2->vec);
    QUEUE_LOG("[queue_is_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Inverse of @ref queue_is_equal.
 *
 * @param q1 Left queue.
 * @param q2 Right queue.
 * @return `!queue_is_equal(q1, q2)`.
 */
bool queue_is_not_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_not_equal]: enter q1=%p q2=%p", (const void*)q1, (const void*)q2);
    bool out = !queue_is_equal(q1, q2);
    QUEUE_LOG("[queue_is_not_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Lexicographic less-than over the underlying vector bytes.
 *
 * Returns false if either operand is NULL.
 *
 * @param q1 Left queue.
 * @param q2 Right queue.
 * @return true if @p q1 is lexicographically less than @p q2.
 */
bool queue_is_less(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_less]: enter q1=%p q2=%p", (const void*)q1, (const void*)q2);

    if (!q1 || !q2 || !q1->vec || !q2->vec) {
        QUEUE_LOG("[queue_is_less]: one receiver NULL -> false");
        return false;
    }
    bool out = vector_is_less(q1->vec, q2->vec);
    QUEUE_LOG("[queue_is_less]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Lexicographic greater-than over the underlying vector bytes.
 *
 * Returns false if either operand is NULL.
 *
 * @param q1 Left queue.
 * @param q2 Right queue.
 * @return true if @p q1 is lexicographically greater than @p q2.
 */
bool queue_is_greater(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_greater]: enter q1=%p q2=%p", (const void*)q1, (const void*)q2);
    if (!q1 || !q2 || !q1->vec || !q2->vec) {
        QUEUE_LOG("[queue_is_greater]: one receiver NULL -> false");
        return false;
    }

    bool out = vector_is_greater(q1->vec, q2->vec);
    QUEUE_LOG("[queue_is_greater]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Less-than-or-equal.
 *
 * @param q1 Left queue.
 * @param q2 Right queue.
 * @return `queue_is_less(q1, q2) || queue_is_equal(q1, q2)`.
 */
bool queue_is_less_or_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_less_or_equal]: enter q1=%p q2=%p", (const void*)q1, (const void*)q2);
    bool out = queue_is_less(q1, q2) || queue_is_equal(q1, q2);
    QUEUE_LOG("[queue_is_less_or_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Greater-than-or-equal.
 *
 * @param q1 Left queue.
 * @param q2 Right queue.
 * @return `queue_is_greater(q1, q2) || queue_is_equal(q1, q2)`.
 */
bool queue_is_greater_or_equal(const Queue* q1, const Queue* q2) {
    QUEUE_LOG("[queue_is_greater_or_equal]: enter q1=%p q2=%p", (const void*)q1, (const void*)q2);
    bool out = queue_is_greater(q1, q2) || queue_is_equal(q1, q2);
    QUEUE_LOG("[queue_is_greater_or_equal]: exit -> %s", out ? "true" : "false");

    return out;
}
