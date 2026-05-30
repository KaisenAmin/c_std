/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Span
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in span.c (file-level convention).
 *
 * A C analog of C++ std::span<T>: a non-owning view over a contiguous
 * sequence of equally-sized elements.
 *
 * Semantics (matching std::span):
 *   - Span does NOT own its data. span_create / span_view do not copy.
 *   - Span.size is the NUMBER OF ELEMENTS (not bytes).
 *     Use span_size_bytes() for the byte count.
 *   - An empty span is legal: data may be NULL when size == 0.
 *   - Two construction styles are provided:
 *       * Span  span_view  (...)  -- value type, no heap, like std::span.
 *       * Span* span_create(...)  -- heap-allocated, for back-compat.
 *
 * Error model:
 *   - No global error state. No errno-style last-error. Functions report
 *     failure through their return value (NULL / 0 / false / empty span).
 *   - Opt-in diagnostic logging via SPAN_LOGGING_ENABLE → SPAN_LOG.
 *   - No library function ever calls exit().
 */

#ifndef SPAN_H_
#define SPAN_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define SPAN_LOGGING_ENABLE */

#ifdef SPAN_LOGGING_ENABLE
    #define SPAN_LOG(fmt, ...) \
        do { fprintf(stderr, "[SPAN LOG]: " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SPAN_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    void*  data;      /* pointer to externally-owned storage   */
    size_t size;      /* NUMBER OF ELEMENTS (not bytes!)        */
    size_t elemSize;  /* size of each element in bytes          */
} Span;


/* ------------------------------------------------------------------ */
/* Construction (value-type + heap)                                   */
/* ------------------------------------------------------------------ */

Span         span_view                 (void* data, size_t elemCount, size_t elemSize);
Span*        span_create               (void* data, size_t elemCount, size_t elemSize);
void         span_destroy              (Span* span);


/* ------------------------------------------------------------------ */
/* Slicing (sub-views)                                                */
/* ------------------------------------------------------------------ */

Span         span_first                (const Span* span, size_t count);
Span         span_last                 (const Span* span, size_t count);
Span         span_subspan              (const Span* span, size_t offset, size_t count);


/* ------------------------------------------------------------------ */
/* Capacity / size                                                    */
/* ------------------------------------------------------------------ */

size_t       span_size                 (const Span* span);
size_t       span_size_bytes           (const Span* span);
size_t       span_size_bits            (const Span* span);
size_t       span_elem_size            (const Span* span);
bool         span_empty                (const Span* span);


/* ------------------------------------------------------------------ */
/* Element access — mutating                                          */
/* ------------------------------------------------------------------ */

void*        span_at                   (const Span* span, size_t index);
void*        span_front                (const Span* span);
void*        span_back                 (const Span* span);
void*        span_data                 (const Span* span);


/* ------------------------------------------------------------------ */
/* Iterators — mutating                                               */
/* ------------------------------------------------------------------ */

void*        span_begin                (Span* span);
void*        span_end                  (Span* span);
void*        span_rbegin               (Span* span);
void*        span_rend                 (Span* span);
void*        span_increment            (const Span* span, void* ptr);
void*        span_decrement            (const Span* span, void* ptr);


/* ------------------------------------------------------------------ */
/* Iterators — const                                                  */
/* ------------------------------------------------------------------ */

const void*  span_cdata                (const Span* span);
const void*  span_cbegin               (const Span* span);
const void*  span_cend                 (const Span* span);
const void*  span_crbegin              (const Span* span);
const void*  span_crend                (const Span* span);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool         span_is_equal             (const Span* span1, const Span* span2);
bool         span_is_not_equal         (const Span* span1, const Span* span2);
bool         span_is_less              (const Span* span1, const Span* span2);
bool         span_is_less_or_equal     (const Span* span1, const Span* span2);
bool         span_is_greater           (const Span* span1, const Span* span2);
bool         span_is_greater_or_equal  (const Span* span1, const Span* span2);


#ifdef __cplusplus
}
#endif

#endif
