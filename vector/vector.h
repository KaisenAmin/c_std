/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Vector
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in vector.c (file-level convention).
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>   /* memcpy, used by the inline fast paths below */

#ifdef __cplusplus
extern "C" {
#endif


/* #define VECTOR_LOGGING_ENABLE */

#ifdef VECTOR_LOGGING_ENABLE
    #define VECTOR_LOG(fmt, ...) \
        do { fprintf(stderr, "[VECTOR LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define VECTOR_LOG(...) do { } while (0)
#endif



typedef struct Vector Vector;

struct Vector {
    void*  items;
    size_t size;
    size_t capacitySize;  /* number of elements the buffer can hold            */
    size_t itemSize;      /* size in bytes of a single element                 */
};


/* ------------------------------------------------------------------ */
/* Hot-path helpers (inlined at the call site, like std::vector)      */
/* ------------------------------------------------------------------ */
/*
 * push_back and at are the two operations that dominate large workloads, so they
 * are defined `static inline` here: the common case (room available / in-bounds)
 * compiles straight into the caller with no cross-translation-unit function call
 * — the same thing that makes std::vector fast. Only the rare "needs to grow"
 * branch dips into the out-of-line vecbuf_push_back_grow() in vector.c.
 *
 * vecbuf_store() copies one element using a size-specialized branch: for the
 * common element sizes the size is a compile-time constant, so the compiler turns
 * memcpy into a single load/store move instead of an indirect call to memcpy with
 * a runtime length. The `vecbuf_` prefix keeps these distinct from the public
 * `vector_` API.
 */
/*
 * The size-specialized branches deliberately memcpy a compile-time-constant
 * number of bytes. When this is inlined into a call site whose element happens
 * to be smaller (e.g. push_back(&an_int) hitting the case 8/16 arms), GCC's
 * -Warray-bounds flags those *dead* arms as over-reads because it cannot
 * correlate `n` (== itemSize, the switch selector) with the source object's
 * size. The arm only runs when n really is 8/16, so the read is in fact bounded.
 * Suppress that single false positive locally — nothing else here is silenced.
 */
#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Warray-bounds"
#  pragma GCC diagnostic ignored "-Wstringop-overread"
#  pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
static inline void vecbuf_store(void* dst, const void* src, size_t n) {
    switch (n) {
        case 1:  memcpy(dst, src, 1);  break;
        case 2:  memcpy(dst, src, 2);  break;
        case 4:  memcpy(dst, src, 4);  break;
        case 8:  memcpy(dst, src, 8);  break;
        case 16: memcpy(dst, src, 16); break;
        default: memcpy(dst, src, n);  break;
    }
}
#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif

/* Out-of-line slow path: handles NULL, growth, and the copy. */
bool        vecbuf_push_back_grow            (Vector* vec, const void* item);

/* Append `item` to the end. Inlined fast path when the buffer has room. */
static inline bool vector_push_back(Vector* vec, const void* item) {
    if (vec && vec->size < vec->capacitySize) {
        vecbuf_store((char*)vec->items + vec->size * vec->itemSize, item, vec->itemSize);
        vec->size++;
        return true;
    }
    return vecbuf_push_back_grow(vec, item);
}

/* Borrowed pointer to the element at `pos`, or NULL if out of bounds / NULL vec. */
static inline void* vector_at(const Vector* vec, size_t pos) {
    return (vec && pos < vec->size)
        ? (void*)((char*)vec->items + pos * vec->itemSize)
        : NULL;
}


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Vector*     vector_create                (size_t itemSize);
void        vector_deallocate            (Vector* vec);


/* ------------------------------------------------------------------ */
/* Capacity                                                           */
/* ------------------------------------------------------------------ */

size_t      vector_size                  (const Vector* vec);
size_t      vector_capacity              (Vector* vec);
size_t      vector_max_size              (Vector* vec);
bool        vector_is_empty              (Vector* vec);
bool        vector_reserve               (Vector* vec, size_t size);
void        vector_resize                (Vector* vec, size_t size);
void        vector_shrink_to_fit         (Vector* vec);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

/* vector_push_back is defined inline above. */
bool        vector_emplace_back          (Vector* vec, void* item, size_t itemSize);
void        vector_insert                (Vector* vec, size_t pos, void* item);
void        vector_emplace               (Vector* vec, size_t pos, void* item, size_t itemSize);
void        vector_assign                (Vector* vec, size_t pos, void* item);
void        vector_erase                 (Vector* vec, size_t pos, size_t len);
void        vector_clear                 (Vector* vec);
void        vector_swap                  (Vector* vec1, Vector* vec2);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

/* vector_at is defined inline above. */
void*       vector_front                 (Vector* vec);
void*       vector_back                  (Vector* vec);
void*       vector_data                  (Vector* vec);
void*       vector_pop_back              (Vector* vec);


/* ------------------------------------------------------------------ */
/* Iterators — mutating                                               */
/* ------------------------------------------------------------------ */

void*       vector_begin                 (Vector* vec);
void*       vector_end                   (Vector* vec);
void*       vector_rbegin                (Vector* vec);
void*       vector_rend                  (Vector* vec);


/* ------------------------------------------------------------------ */
/* Iterators — const                                                  */
/* ------------------------------------------------------------------ */

const void* vector_cbegin                (Vector* vec);
const void* vector_cend                  (Vector* vec);
const void* vector_crbegin               (Vector* vec);
const void* vector_crend                 (Vector* vec);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool        vector_is_equal              (const Vector* vec1, const Vector* vec2);
bool        vector_is_not_equal          (const Vector* vec1, const Vector* vec2);
bool        vector_is_less               (const Vector* vec1, const Vector* vec2);
bool        vector_is_less_or_equal      (const Vector* vec1, const Vector* vec2);
bool        vector_is_greater            (const Vector* vec1, const Vector* vec2);
bool        vector_is_greater_or_equal   (const Vector* vec1, const Vector* vec2);


#ifdef __cplusplus
}
#endif

#endif 
