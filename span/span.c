/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Span
 *
 * std::span-style non-owning view in pure C.
 *
 * Error model
 * -----------
 *   - No global error state. Failures return NULL / 0 / false / empty
 *     span. Diagnostics are emitted via SPAN_LOG (opt-in via the
 *     SPAN_LOGGING_ENABLE macro).
 *   - The library never calls exit(), abort(), or assert(). Every public
 *     function is NULL-safe in the documented sense (see each function).
 *   - The library does not allocate, copy, or free user data. Span is
 *     strictly a view.
 *
 * Iterator model
 * --------------
 *   - Forward iteration uses raw `void*` pointers:
 *
 *         for (void* p = span_begin(&s); p != span_end(&s);
 *              p = span_increment(&s, p)) { ... }
 *
 *   - Reverse iteration uses `span_rbegin` (a pointer to the LAST
 *     element) and `span_decrement` (which returns NULL when stepping
 *     below begin). `span_rend` exists purely as a NULL sentinel for
 *     parity with `span_end`; see span_rend()'s comment for why it has
 *     to be NULL rather than `data - elemSize`.
 */

#include <limits.h> /* CHAR_BIT */
#include <stdlib.h>
#include <string.h>
#include "span.h"


/**
 * @brief Build a non-owning Span value (std::span style — no heap, no copy).
 *
 * Semantics:
 *   - `(data != NULL, elemCount > 0, elemSize > 0)` → valid non-empty span.
 *   - `(elemCount == 0, any data, any elemSize)`   → valid empty span
 *     whose `elemSize` field is preserved so callers can ask
 *     `span_elem_size()` on the result.
 *   - `(data == NULL, elemCount > 0, ...)`         → empty span (the
 *     library refuses to record a non-empty span over a NULL buffer).
 *   - `(elemSize == 0, ...)`                        → empty span.
 *
 * The returned span never owns the pointee — the caller's buffer must
 * outlive every use of the span.
 *
 * @param data       Pointer to the start of the externally-owned storage.
 * @param elemCount  Number of elements visible through the span.
 * @param elemSize   Size in bytes of one element.
 *
 * @return A Span by value. On any invalid input the returned span has
 *         `data == NULL` and `size == 0`; `elemSize` is preserved.
 *
 * @code
 * int data[5] = {1, 2, 3, 4, 5};
 * Span s = span_view(data, 5, sizeof(int));
 * @endcode
 */
Span span_view(void* data, size_t elemCount, size_t elemSize) {
    SPAN_LOG("[span_view]: enter data=%p elemCount=%zu elemSize=%zu",
             data, elemCount, elemSize);

    Span out = { NULL, 0, elemSize };

    if (elemCount == 0) {
        SPAN_LOG("[span_view]: empty span (elemCount=0), elemSize preserved");
        return out;
    }
    if (!data || elemSize == 0) {
        SPAN_LOG("[span_view]: invalid args (data=%p elemSize=%zu), returning empty span",
                 data, elemSize);
        return out;
    }
    out.data = data;
    out.size = elemCount;
    SPAN_LOG("[span_view]: exit ok data=%p size=%zu elemSize=%zu",
             out.data, out.size, out.elemSize);
    return out;
}

/**
 * @brief Allocate a Span on the heap that points at user-owned `data`.
 *
 * The Span struct itself is heap-allocated (so it can be returned by
 * pointer, stored in C-only contexts, etc.). The pointee is NOT copied
 * and is NOT freed by `span_destroy` — the caller owns the buffer.
 *
 * Validation:
 *   - `(elemCount > 0, !data || elemSize == 0)` → returns NULL.
 *   - `(elemCount == 0, ...)` → valid empty Span* with `data == NULL`.
 *
 * @param data       External buffer to view (may be NULL when elemCount == 0).
 * @param elemCount  Number of elements visible through the span.
 * @param elemSize   Size in bytes of one element.
 *
 * @return Newly-allocated Span* the caller must free with
 *         @ref span_destroy, or NULL on bad input / OOM.
 *
 * @code
 * int* heap = malloc(10 * sizeof(int));
 * Span* s = span_create(heap, 10, sizeof(int));
 * // ... use s ...
 * span_destroy(s);   // does NOT free heap
 * free(heap);
 * @endcode
 */
Span* span_create(void* data, size_t elemCount, size_t elemSize) {
    SPAN_LOG("[span_create]: enter data=%p elemCount=%zu elemSize=%zu",
             data, elemCount, elemSize);

    if (elemCount > 0 && (!data || elemSize == 0)) {
        SPAN_LOG("[span_create]: invalid args (data=%p elemSize=%zu, elemCount=%zu)",
                 data, elemSize, elemCount);
        return NULL;
    }

    Span* s = (Span*)malloc(sizeof(Span));
    if (!s) {
        SPAN_LOG("[span_create]: malloc(sizeof(Span)) failed");
        return NULL;
    }
    s->data     = (elemCount == 0) ? NULL : data;
    s->size     = elemCount;
    s->elemSize = elemSize;
    SPAN_LOG("[span_create]: exit ok span=%p data=%p size=%zu elemSize=%zu",
             (void*)s, s->data, s->size, s->elemSize);
    return s;
}

/**
 * @brief Frees the heap allocation made by @ref span_create.
 *
 * Does NOT free the pointee — Span is non-owning. NULL-safe.
 *
 * @param span Span returned by @ref span_create. Pass NULL for a no-op.
 */
void span_destroy(Span* span) {
    SPAN_LOG("[span_destroy]: enter span=%p", (void*)span);
    if (!span) {
        SPAN_LOG("[span_destroy]: NULL receiver, no-op");
        return;
    }
    free(span);
    SPAN_LOG("[span_destroy]: exit (header freed; pointee untouched)");
}


/**
 * @brief Number of elements visible through @p s (NOT bytes).
 *
 * @param s Span pointer. NULL returns 0.
 * @return Element count, mirroring `std::span::size`.
 */
size_t span_size(const Span* s) {
    SPAN_LOG("[span_size]: enter span=%p", (const void*)s);
    size_t out = s ? s->size : 0;
    SPAN_LOG("[span_size]: exit -> %zu", out);
    return out;
}


/**
 * @brief Total size in bytes (`size * elemSize`).
 *
 * @param s Span pointer. NULL returns 0.
 * @return Byte count of all elements combined.
 */
size_t span_size_bytes(const Span* s) {
    SPAN_LOG("[span_size_bytes]: enter span=%p", (const void*)s);
    size_t out = s ? s->size * s->elemSize : 0;
    SPAN_LOG("[span_size_bytes]: exit -> %zu", out);
    return out;
}


/**
 * @brief Total size in bits (`size_bytes * CHAR_BIT`).
 *
 * @param s Span pointer. NULL returns 0.
 * @return Bit count of all elements combined.
 */
size_t span_size_bits(const Span* s) {
    SPAN_LOG("[span_size_bits]: enter span=%p", (const void*)s);
    size_t out = s ? s->size * s->elemSize * (size_t)CHAR_BIT : 0;
    SPAN_LOG("[span_size_bits]: exit -> %zu", out);
    return out;
}


/**
 * @brief Bytes per element.
 *
 * @param s Span pointer. NULL returns 0.
 * @return Size of one element in bytes, as supplied to the constructor.
 */
size_t span_elem_size(const Span* s) {
    SPAN_LOG("[span_elem_size]: enter span=%p", (const void*)s);
    size_t out = s ? s->elemSize : 0;
    SPAN_LOG("[span_elem_size]: exit -> %zu", out);
    return out;
}


/**
 * @brief Returns true if the span has no elements (or is NULL).
 *
 * @param s Span pointer. NULL is treated as empty (returns true).
 * @return true if `s == NULL` or `s->size == 0`, false otherwise.
 */
bool span_empty(const Span* s) {
    SPAN_LOG("[span_empty]: enter span=%p", (const void*)s);
    if (!s) {
        SPAN_LOG("[span_empty]: NULL span, returning true");
        return true;
    }
    bool out = s->size == 0;
    SPAN_LOG("[span_empty]: exit -> %s", out ? "true" : "false");
    return out;
}


/**
 * @brief Bounds-checked element pointer.
 *
 * @param s     Span pointer.
 * @param index Zero-based element index.
 *
 * @return Pointer to element @p index, or NULL if @p s is NULL or
 *         @p index >= `s->size`. Equivalent to
 *         `(char*)s->data + index * s->elemSize` on success.
 */
void* span_at(const Span* s, size_t index) {
    SPAN_LOG("[span_at]: enter span=%p index=%zu", (const void*)s, index);
    if (!s || index >= s->size) {
        SPAN_LOG("[span_at]: out of bounds (s=%p, size=%zu, index=%zu) -> NULL",
                 (const void*)s, s ? s->size : 0, index);
        return NULL;
    }
    void* out = (char*)s->data + index * s->elemSize;
    SPAN_LOG("[span_at]: exit -> %p", out);
    return out;
}


/**
 * @brief Pointer to the first element of the span.
 *
 * @param s Span pointer.
 * @return Same as `s->data` for a non-empty span, NULL otherwise.
 */
void* span_front(const Span* s) {
    SPAN_LOG("[span_front]: enter span=%p", (const void*)s);
    if (!s || s->size == 0) {
        SPAN_LOG("[span_front]: NULL or empty span -> NULL");
        return NULL;
    }
    SPAN_LOG("[span_front]: exit -> %p", s->data);
    return s->data;
}


/**
 * @brief Pointer to the last element of the span.
 *
 * @param s Span pointer.
 * @return `data + (size - 1) * elemSize` for a non-empty span, NULL
 *         otherwise.
 */
void* span_back(const Span* s) {
    SPAN_LOG("[span_back]: enter span=%p", (const void*)s);
    if (!s || s->size == 0) {
        SPAN_LOG("[span_back]: NULL or empty span -> NULL");
        return NULL;
    }
    void* out = (char*)s->data + (s->size - 1) * s->elemSize;
    SPAN_LOG("[span_back]: exit -> %p", out);
    return out;
}


/**
 * @brief Raw mutable data pointer (may be NULL for an empty span).
 *
 * @param s Span pointer.
 * @return `s->data` or NULL.
 */
void* span_data(const Span* s) {
    SPAN_LOG("[span_data]: enter span=%p", (const void*)s);
    void* out = s ? s->data : NULL;
    SPAN_LOG("[span_data]: exit -> %p", out);
    return out;
}


/**
 * @brief Read-only raw data pointer.
 *
 * @param s Span pointer.
 * @return `s->data` (as `const void*`) or NULL.
 */
const void* span_cdata(const Span* s) {
    SPAN_LOG("[span_cdata]: enter span=%p", (const void*)s);
    const void* out = s ? s->data : NULL;
    SPAN_LOG("[span_cdata]: exit -> %p", out);
    return out;
}

/**
 * @brief Returns a span over the first @p count elements.
 *
 * Behavior:
 *   - `count <= s->size`  → span of @p count elements starting at
 *                           `s->data`. For `count == 0` the returned
 *                           span has `data == NULL` but `elemSize` is
 *                           preserved.
 *   - `count >  s->size`  → empty span (with `elemSize` preserved).
 *   - `s == NULL`         → zeroed empty span.
 *
 * @param s     Source span.
 * @param count Number of elements to retain from the front.
 * @return New Span value.
 */
Span span_first(const Span* s, size_t count) {
    SPAN_LOG("[span_first]: enter span=%p count=%zu", (const void*)s, count);

    Span out = { NULL, 0, 0 };
    if (!s) {
        SPAN_LOG("[span_first]: NULL source -> zeroed empty span");
        return out;
    }
    out.elemSize = s->elemSize;

    if (count > s->size) {
        SPAN_LOG("[span_first]: count=%zu > size=%zu, empty span (elemSize=%zu)",
                 count, s->size, s->elemSize);
        return out;
    }
    out.data = (count == 0) ? NULL : s->data;
    out.size = count;
    SPAN_LOG("[span_first]: exit -> data=%p size=%zu elemSize=%zu",
             out.data, out.size, out.elemSize);
    return out;
}


/**
 * @brief Returns a span over the last @p count elements.
 *
 * Behavior:
 *   - `count <= s->size`  → span of @p count elements ending at the
 *                           same position as @p s. For `count == 0`
 *                           the returned span has `data == NULL`.
 *   - `count >  s->size`  → empty span (with `elemSize` preserved).
 *   - `s == NULL`         → zeroed empty span.
 *
 * @param s     Source span.
 * @param count Number of elements to retain from the back.
 * @return New Span value.
 */
Span span_last(const Span* s, size_t count) {
    SPAN_LOG("[span_last]: enter span=%p count=%zu", (const void*)s, count);

    Span out = { NULL, 0, 0 };
    if (!s) {
        SPAN_LOG("[span_last]: NULL source -> zeroed empty span");
        return out;
    }
    out.elemSize = s->elemSize;

    if (count > s->size) {
        SPAN_LOG("[span_last]: count=%zu > size=%zu, empty span (elemSize=%zu)",
                 count, s->size, s->elemSize);
        return out;
    }
    out.size = count;
    out.data = (count == 0) ? NULL : (char*)s->data + (s->size - count) * s->elemSize;
    SPAN_LOG("[span_last]: exit -> data=%p size=%zu elemSize=%zu",
             out.data, out.size, out.elemSize);
    return out;
}


/**
 * @brief Returns a sub-span starting at @p offset for @p count elements.
 *
 * Behavior:
 *   - `offset > s->size` OR `count > s->size - offset`
 *                         → empty span (`elemSize` preserved).
 *   - `count == 0`        → empty span; `data == NULL`.
 *   - `s == NULL`         → zeroed empty span.
 *
 * @param s      Source span.
 * @param offset Starting index (zero-based).
 * @param count  Number of elements in the sub-span.
 * @return New Span value.
 */
Span span_subspan(const Span* s, size_t offset, size_t count) {
    SPAN_LOG("[span_subspan]: enter span=%p offset=%zu count=%zu",
             (const void*)s, offset, count);

    Span out = { NULL, 0, 0 };
    if (!s) {
        SPAN_LOG("[span_subspan]: NULL source -> zeroed empty span");
        return out;
    }
    out.elemSize = s->elemSize;

    if (offset > s->size || count > s->size - offset) {
        SPAN_LOG("[span_subspan]: out of range (size=%zu, offset=%zu, count=%zu) -> empty",
                 s->size, offset, count);
        return out;
    }
    out.size = count;
    out.data = (count == 0) ? NULL : (char*)s->data + offset * s->elemSize;
    SPAN_LOG("[span_subspan]: exit -> data=%p size=%zu elemSize=%zu",
             out.data, out.size, out.elemSize);
    return out;
}


/**
 * @brief Byte-exact equality between two spans (NULL == NULL).
 *
 * Two spans are equal iff:
 *   - they point at the same Span (reflexive), OR
 *   - both are NULL (per docstring), OR
 *   - they share the same `size` AND `elemSize`, AND either `size == 0`
 *     or `memcmp(data, other->data, size*elemSize) == 0`.
 *
 * **Empty-span quirk:** two empty spans are equal only if their
 * `elemSize` matches. This is the strict-memcmp interpretation and
 * matches `std::span` in modern C++.
 *
 * @param a Left span.
 * @param b Right span.
 * @return true if the spans are byte-exact equal.
 */
bool span_is_equal(const Span* a, const Span* b) {
    SPAN_LOG("[span_is_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);

    if (a == b) {
        SPAN_LOG("[span_is_equal]: identity (a == b) -> true");
        return true;
    }
    if (!a || !b) {
        SPAN_LOG("[span_is_equal]: one operand is NULL -> false");
        return false;
    }
    if (a->size != b->size || a->elemSize != b->elemSize) {
        SPAN_LOG("[span_is_equal]: size/elemSize mismatch "
                 "(a.size=%zu a.elem=%zu vs b.size=%zu b.elem=%zu) -> false",
                 a->size, a->elemSize, b->size, b->elemSize);
        return false;
    }
    if (a->size == 0) {
        SPAN_LOG("[span_is_equal]: both empty with equal elemSize -> true");
        return true;
    }
    bool out = memcmp(a->data, b->data, a->size * a->elemSize) == 0;
    SPAN_LOG("[span_is_equal]: memcmp result -> %s", out ? "true" : "false");
    return out;
}


/**
 * @brief Inverse of @ref span_is_equal.
 *
 * @param a Left span.
 * @param b Right span.
 * @return `!span_is_equal(a, b)`.
 */
bool span_is_not_equal(const Span* a, const Span* b) {
    SPAN_LOG("[span_is_not_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    bool out = !span_is_equal(a, b);
    SPAN_LOG("[span_is_not_equal]: exit -> %s", out ? "true" : "false");
    return out;
}


/**
 * @brief Lexicographic less-than over the raw bytes.
 *
 * Behavior matches `std::lexicographical_compare` over the byte
 * representations:
 *   - Compare common prefix bytes via `memcmp`.
 *   - On equal prefix, the shorter span is less.
 *   - Either operand NULL → false.
 *
 * @param a Left span.
 * @param b Right span.
 * @return true if @p a is lexicographically less than @p b.
 */
bool span_is_less(const Span* a, const Span* b) {
    SPAN_LOG("[span_is_less]: enter a=%p b=%p", (const void*)a, (const void*)b);
    if (!a || !b) {
        SPAN_LOG("[span_is_less]: one operand is NULL -> false");
        return false;
    }

    size_t aBytes = a->size * a->elemSize;
    size_t bBytes = b->size * b->elemSize;
    size_t minBytes = aBytes < bBytes ? aBytes : bBytes;
    int c = (minBytes == 0) ? 0 : memcmp(a->data, b->data, minBytes);
    bool out = c < 0 || (c == 0 && aBytes < bBytes);

    SPAN_LOG("[span_is_less]: aBytes=%zu bBytes=%zu memcmp=%d -> %s", aBytes, bBytes, c, out ? "true" : "false");
    return out;
}


/**
 * @brief Lexicographic greater-than. Equivalent to
 *        `span_is_less(b, a)`.
 *
 * @param a Left span.
 * @param b Right span.
 * @return true if @p a > @p b.
 */
bool span_is_greater(const Span* a, const Span* b) {
    SPAN_LOG("[span_is_greater]: enter a=%p b=%p", (const void*)a, (const void*)b);
    if (!a || !b) {
        SPAN_LOG("[span_is_greater]: one operand is NULL -> false");
        return false;
    }
    bool out = span_is_less(b, a);
    SPAN_LOG("[span_is_greater]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Less-than-or-equal.
 *
 * @param a Left span.
 * @param b Right span.
 * @return `span_is_less(a, b) || span_is_equal(a, b)`.
 */
bool span_is_less_or_equal(const Span* a, const Span* b) {
    SPAN_LOG("[span_is_less_or_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    bool out = span_is_less(a, b) || span_is_equal(a, b);
    SPAN_LOG("[span_is_less_or_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Greater-than-or-equal.
 *
 * @param a Left span.
 * @param b Right span.
 * @return `span_is_greater(a, b) || span_is_equal(a, b)`.
 */
bool span_is_greater_or_equal(const Span* a, const Span* b) {
    SPAN_LOG("[span_is_greater_or_equal]: enter a=%p b=%p", (const void*)a, (const void*)b);
    bool out = span_is_greater(a, b) || span_is_equal(a, b);
    SPAN_LOG("[span_is_greater_or_equal]: exit -> %s", out ? "true" : "false");

    return out;
}


/**
 * @brief Pointer to the first element of the span. NULL-safe.
 *
 * Equivalent to `s->data` (or NULL when `s == NULL`).
 *
 * @param s Span pointer.
 * @return Begin pointer.
 */
void* span_begin(Span* s) {
    SPAN_LOG("[span_begin]: enter span=%p", (void*)s);
    void* out = s ? s->data : NULL;
    SPAN_LOG("[span_begin]: exit -> %p", out);

    return out;
}


/**
 * @brief Read-only @ref span_begin.
 *
 * @param s Span pointer.
 * @return Begin pointer as `const void*`.
 */
const void* span_cbegin(const Span* s) {
    SPAN_LOG("[span_cbegin]: enter span=%p", (const void*)s);
    const void* out = s ? s->data : NULL;
    SPAN_LOG("[span_cbegin]: exit -> %p", out);

    return out;
}


/**
 * @brief One-past-the-end pointer.
 *
 * For an empty span returns `s->data` (which may be NULL); this is
 * legal per the C standard for `T*` arithmetic — both `begin` and `end`
 * coincide.
 *
 * The returned value is a valid sentinel for comparison but must NOT
 * be dereferenced.
 *
 * @param s Span pointer.
 * @return End pointer (one past the last element), or NULL if @p s is NULL.
 */
void* span_end(Span* s) {
    SPAN_LOG("[span_end]: enter span=%p", (void*)s);
    if (!s) {
        SPAN_LOG("[span_end]: NULL span -> NULL");
        return NULL;
    }
    if (s->size == 0) {
        SPAN_LOG("[span_end]: empty span -> data=%p", s->data);
        return s->data;
    }
    void* out = (char*)s->data + s->size * s->elemSize;
    SPAN_LOG("[span_end]: exit -> %p", out);

    return out;
}


/**
 * @brief Read-only @ref span_end.
 *
 * @param s Span pointer.
 * @return End pointer as `const void*`.
 */
const void* span_cend(const Span* s) {
    SPAN_LOG("[span_cend]: enter span=%p", (const void*)s);
    if (!s) {
        SPAN_LOG("[span_cend]: NULL span -> NULL");
        return NULL;
    }
    if (s->size == 0) {
        SPAN_LOG("[span_cend]: empty span -> data=%p", s->data);
        return s->data;
    }
    const void* out = (const char*)s->data + s->size * s->elemSize;
    SPAN_LOG("[span_cend]: exit -> %p", out);

    return out;
}


/**
 * @brief Pointer to the LAST element (the "first" of the reverse walk).
 *
 * The natural reverse-iteration loop:
 *
 * @code
 * for (void* p = span_rbegin(&s); p != NULL; p = span_decrement(&s, p)) {
 *     // ... use *p ...
 * }
 * @endcode
 *
 * @param s Span pointer.
 * @return Pointer to `data + (size - 1) * elemSize`, or NULL if @p s
 *         is NULL or empty.
 */
void* span_rbegin(Span* s) {
    SPAN_LOG("[span_rbegin]: enter span=%p", (void*)s);
    if (!s || s->size == 0) {
        SPAN_LOG("[span_rbegin]: NULL or empty span -> NULL");
        return NULL;
    }
    void* out = (char*)s->data + (s->size - 1) * s->elemSize;
    SPAN_LOG("[span_rbegin]: exit -> %p", out);

    return out;
}


/**
 * @brief Read-only @ref span_rbegin.
 *
 * @param s Span pointer.
 * @return Pointer to the last element as `const void*`, or NULL.
 */
const void* span_crbegin(const Span* s) {
    SPAN_LOG("[span_crbegin]: enter span=%p", (const void*)s);
    if (!s || s->size == 0) {
        SPAN_LOG("[span_crbegin]: NULL or empty span -> NULL");
        return NULL;
    }
    const void* out = (const char*)s->data + (s->size - 1) * s->elemSize;
    SPAN_LOG("[span_crbegin]: exit -> %p", out);
    return out;
}


/**
 * @brief Reverse-end sentinel — always NULL.
 *
 * **Why always NULL?** The symmetric "one-before-the-first" pointer
 * (`data - elemSize`) is **undefined behaviour** in C even if it is
 * never dereferenced — the language only permits forming pointers from
 * the start of an object up to one-past-the-end. NULL is the only
 * portable sentinel that:
 *   - can be computed without UB,
 *   - can never collide with a real element pointer for a non-empty
 *     span (because `s->data` of a non-empty span is itself never NULL),
 *   - is the same value `span_decrement` returns when it steps below
 *     `begin`, so the natural loop `p != span_rend(&s)` and the
 *     pragmatic loop `p != NULL` agree.
 *
 * The function takes a `Span*` purely for API symmetry with
 * @ref span_end; the value is independent of the span.
 *
 * @param s Ignored (kept for symmetry with @ref span_end).
 * @return NULL.
 */
void* span_rend(Span* s) {
    SPAN_LOG("[span_rend]: enter span=%p (ignored; always returns NULL)", (void*)s);
    (void)s;
    return NULL;
}

/**
 * @brief Read-only reverse-end sentinel — always NULL.
 *
 * See @ref span_rend for the rationale.
 *
 * @param s Ignored.
 * @return NULL.
 */
const void* span_crend(const Span* s) {
    SPAN_LOG("[span_crend]: enter span=%p (ignored; always returns NULL)", (const void*)s);
    (void)s;
    return NULL;
}


/**
 * @brief Step forward one element from @p ptr.
 *
 * @p ptr must point at an element currently inside the span (i.e.
 * in `[span_begin(s), span_end(s))`). Crucially, a pointer equal to
 * `span_end(s)` is NOT valid input and yields NULL — the function is
 * not a "next-or-end" helper, it's strictly an increment of an iterator
 * positioned ON an element.
 *
 * The returned pointer may equal `span_end(s)`, which is a valid
 * one-past-the-end sentinel callers can compare against.
 *
 * @param s   Span over which iteration is happening.
 * @param ptr Current iterator pointer.
 * @return Next-element pointer, or NULL on invalid input.
 */
void* span_increment(const Span* s, void* ptr) {
    SPAN_LOG("[span_increment]: enter span=%p ptr=%p", (const void*)s, ptr);
    if (!s || !ptr) {
        SPAN_LOG("[span_increment]: NULL receiver / ptr -> NULL");
        return NULL;
    }

    char* p   = (char*)ptr;
    char* beg = (char*)s->data;
    char* end = beg + s->size * s->elemSize;

    if (p < beg || p >= end) {
        SPAN_LOG("[span_increment]: ptr=%p out of span [%p, %p) -> NULL", (void*)p, (void*)beg, (void*)end);
        return NULL;
    }
    void* out = p + s->elemSize;
    SPAN_LOG("[span_increment]: exit -> %p (may equal end %p)", out, (void*)end);
    
    return out;
}


/**
 * @brief Step backward one element from @p ptr.
 *
 * Valid inputs are pointers in `(span_begin(s), span_end(s)]` — i.e.
 * any element except the first, plus the one-past-the-end pointer.
 * A pointer equal to `span_begin(s)` returns NULL (you cannot step
 * below begin).
 *
 * This is what makes the reverse-walk loop terminate cleanly:
 *
 * @code
 * for (void* p = span_rbegin(&s); p != NULL; p = span_decrement(&s, p))
 *     // ...
 * @endcode
 *
 * @param s   Span over which iteration is happening.
 * @param ptr Current iterator pointer.
 * @return Previous-element pointer, or NULL if @p ptr is at or below
 *         `span_begin(s)` or @p ptr is outside the span.
 */
void* span_decrement(const Span* s, void* ptr) {
    SPAN_LOG("[span_decrement]: enter span=%p ptr=%p", (const void*)s, ptr);
    if (!s || !ptr) {
        SPAN_LOG("[span_decrement]: NULL receiver / ptr -> NULL");
        return NULL;
    }

    char* p   = (char*)ptr;
    char* beg = (char*)s->data;
    char* end = beg + s->size * s->elemSize;

    if (p <= beg || p > end) {
        SPAN_LOG("[span_decrement]: ptr=%p out of (begin=%p, end=%p] -> NULL", (void*)p, (void*)beg, (void*)end);
        return NULL;
    }
    void* out = p - s->elemSize;
    SPAN_LOG("[span_decrement]: exit -> %p", out);

    return out;
}
