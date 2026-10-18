/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Vector
 *
 * Contiguous, growable array of fixed-size elements: the C counterpart of
 * std::vector. Out-of-line functions are documented above their definitions
 * in vector.c; the hot accessors and the typed macros are documented here.
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
    #define VECTOR_LOG(...) \
        do { fprintf(stderr, "[VECTOR LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
    #define VECTOR_LOG(...) do { } while (0)
#endif


/* "Not found" index returned by vector_find / vector_binary_search. */
#define VECTOR_NPOS ((size_t)-1)


typedef struct Vector Vector;

/* Element comparator, qsort convention (negative / zero / positive). */
typedef int (*VectorCompareFunc)(const void* a, const void* b);

/* Predicate / visitor callbacks; `ctx` is passed through unchanged. */
typedef bool (*VectorPredicateFunc)(const void* item, void* ctx);
typedef void (*VectorVisitFunc)(void* item, void* ctx);

struct Vector {
    void*  items;
    size_t size;
    size_t capacitySize;  /* number of elements the buffer can hold            */
    size_t itemSize;      /* size in bytes of a single element                 */
    /* Optional element comparator for the is_* comparisons (vector_set_compare).
     * NULL compares each element's bytes with memcmp: fine for equality of
     * plain data, but only a meaningful ORDER for byte-like elements (char,
     * unsigned char, big-endian keys) - not for int, negative numbers or
     * floating point. It is also what vector_find / vector_sort / vector_unique
     * / vector_remove_value use. */
    VectorCompareFunc compare;
};



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

/* Out-of-line slow paths: growth for one element (with the copy of `item`),
   and growth so that `extra` more elements fit. */
bool        vecbuf_push_back_grow            (Vector* vec, const void* item);
bool        vecbuf_grow_for                  (Vector* vec, size_t extra);

/* Append a copy of `item` (itemSize bytes). Inlined fast path when the buffer
   has room; `item` may point into this vector. false for NULL / OOM. */
static inline bool vector_push_back(Vector* vec, const void* item) {
    if (vec && item) {
        size_t n = vec->size;
        if (n < vec->capacitySize) {
            size_t isz = vec->itemSize;
            vec->size = n + 1;
            vecbuf_store((char*)vec->items + n * isz, item, isz);
            return true;
        }
    }
    return vecbuf_push_back_grow(vec, item);
}

/* Borrowed pointer to the element at `pos`, or NULL if out of bounds / NULL vec. */
static inline void* vector_at(const Vector* vec, size_t pos) {
    return (vec && pos < vec->size)
        ? (void*)((char*)vec->items + pos * vec->itemSize)
        : NULL;
}

/* Number of elements (0 for NULL). */
static inline size_t vector_size(const Vector* vec) {
    return vec ? vec->size : 0;
}

/* Number of elements the buffer holds before it has to grow (0 for NULL). */
static inline size_t vector_capacity(const Vector* vec) {
    return vec ? vec->capacitySize : 0;
}

/* Size in bytes of one element (0 for NULL). */
static inline size_t vector_item_size(const Vector* vec) {
    return vec ? vec->itemSize : 0;
}

/* true when there are no elements (also for NULL). */
static inline bool vector_is_empty(const Vector* vec) {
    return !vec || vec->size == 0;
}

/* The element buffer (NULL for NULL, or for an empty vector without one). */
static inline void* vector_data(const Vector* vec) {
    return vec ? vec->items : NULL;
}

/* First / last element, or NULL when empty or NULL. */
static inline void* vector_front(const Vector* vec) {
    return (vec && vec->size) ? vec->items : NULL;
}

static inline void* vector_back(const Vector* vec) {
    return (vec && vec->size) ? (void*)((char*)vec->items + (vec->size - 1) * vec->itemSize) : NULL;
}

/* Removes the last element and returns a pointer to it (still in the buffer:
   valid until the next push / insert / reserve). NULL when empty or NULL. */
static inline void* vector_pop_back(Vector* vec) {
    if (!vec || vec->size == 0) {
        return NULL;
    }
    vec->size--;
    return (char*)vec->items + vec->size * vec->itemSize;
}

/* [begin, end) iteration pointers; begin == end for an empty vector, both
   NULL for a NULL vector (or an empty one whose buffer was released). */
static inline void* vector_begin(const Vector* vec) {
    return vec ? vec->items : NULL;
}

static inline void* vector_end(const Vector* vec) {
    return (vec && vec->items) ? (void*)((char*)vec->items + vec->size * vec->itemSize) : NULL;
}


/* ------------------------------------------------------------------ */
/* Typed fast paths (macros)                                          */
/* ------------------------------------------------------------------ */
/*
 * The functions above work with `void*` and a runtime element size, which
 * costs a little in tight loops: the element copy goes through memcpy, which
 * may alias anything, so the compiler must reload the vector's fields after
 * every store. These macros take the element TYPE and use typed loads and
 * stores, so the compiler can keep everything in registers, exactly like
 * std::vector<T>. Use them where the element type is known.
 *
 * Rules: `vec` is evaluated more than once (pass a plain variable), `T` must
 * be the element type the vector was created with (sizeof(T) == itemSize),
 * and `value` / `cond` are evaluated once per call / per element.
 *
 *   VECTOR_PUSH_BACK(v, T, value)  append; true, or false when memory runs out
 *   VECTOR_AT(v, T, i)             element i as an lvalue, unchecked (operator[])
 *   VECTOR_DATA(v, T)              T* to the buffer
 *   VECTOR_FRONT(v, T) / VECTOR_BACK(v, T)   first / last element (lvalue, unchecked)
 *   VECTOR_POP_BACK(v, T)          remove and return the last element (unchecked)
 *   VECTOR_APPEND_UNINIT(v, T, n)  T* to n new uninitialised elements at the end
 *   VECTOR_FOREACH(v, T, p)        for (T* p ...) over every element
 *   VECTOR_ERASE_IF(v, T, x, cond) remove every element x for which cond is true
 *   VECTOR_DEFINE_SORT(name, T, LESS)  defines `void name(Vector*)`, a typed
 *                                  pdqsort with LESS inlined (see below)
 */
static inline bool vecbuf_room_for_one(Vector* vec) {
    return vec->size < vec->capacitySize || vecbuf_grow_for(vec, 1);
}

#define VECTOR_PUSH_BACK(vec, T, value) \
    (vecbuf_room_for_one(vec) \
        ? (((T*)(vec)->items)[(vec)->size] = (value), (vec)->size++, true) \
        : false)

#define VECTOR_AT(vec, T, i)    (((T*)(vec)->items)[(i)])
#define VECTOR_DATA(vec, T)     ((T*)(vec)->items)
#define VECTOR_FRONT(vec, T)    (((T*)(vec)->items)[0])
#define VECTOR_BACK(vec, T)     (((T*)(vec)->items)[(vec)->size - 1])
#define VECTOR_POP_BACK(vec, T) (((T*)(vec)->items)[--(vec)->size])

/* `n` new, UNINITIALISED elements at the end (vector_append_uninit), as T*;
   NULL when memory runs out. Fill them through the pointer. */
#define VECTOR_APPEND_UNINIT(vec, T, n) ((T*)vector_append_uninit((vec), (n)))

#define VECTOR_FOREACH(vec, T, p) \
    for (T* p = (T*)(vec)->items, *p##_vec_end_ = p ? p + (vec)->size : p; p < p##_vec_end_; ++p)

#define VECTOR_ERASE_IF(vec, T, x, cond) \
    do { \
        T* vecp_data_ = (T*)(vec)->items; \
        size_t vecp_n_ = (vec)->size, vecp_w_ = 0; \
        for (size_t vecp_r_ = 0; vecp_r_ < vecp_n_; ++vecp_r_) { \
            T x = vecp_data_[vecp_r_]; \
            if (!(cond)) { \
                vecp_data_[vecp_w_++] = x; \
            } \
        } \
        (vec)->size = vecp_w_; \
    } while (0)

/*
 * VECTOR_DEFINE_SORT(name, T, LESS) defines two functions
 *
 *     static inline void name(Vector* vec);            sorts the vector
 *     static inline void name_array(T* a, size_t n);   sorts a plain array
 *
 * which sort elements of type T in ascending order of LESS. LESS is the name
 * of a function-like macro (or a function) taking two T values - lvalues of
 * the elements - that is true when the first goes before the second:
 *
 *     #define INT_LESS(a, b) ((a) < (b))
 *     VECTOR_DEFINE_SORT(sort_ints, int, INT_LESS)
 *
 *     #define BY_SCORE(a, b) ((a).score < (b).score)
 *     VECTOR_DEFINE_SORT(sort_players, Player, BY_SCORE)
 *     ...
 *     sort_ints(v);
 *
 * The algorithm is pattern-defeating quicksort (pdqsort, Orson Peters; the
 * unstable sort of Rust and Go): introsort with branchless block
 * partitioning, a ninther pivot, pattern breaking, special handling of runs
 * of equal elements and early exit for already sorted input. O(n log n)
 * worst case (heapsort fallback), O(n) for sorted, reversed or all-equal
 * input, not stable, no allocation. LESS is inlined; measured against
 * libstdc++ std::sort (5M ints, g++ -O2) it is about 2x faster on random
 * input and 2-12x on sorted, reversed, few-distinct-value and sorted-run
 * inputs; an interleaved ascending/descending input is the one measured case
 * where it is slower (1.9x). vector_sort (qsort + a comparator pointer) is
 * about 4x slower than this. Put it at file scope; LESS must be a strict
 * weak order (like std::sort's comparator).
 */
#if defined(__GNUC__) || defined(__clang__)
#  define VECBUF_MAYBE_UNUSED __attribute__((unused))
#else
#  define VECBUF_MAYBE_UNUSED
#endif
#define VECTOR_DEFINE_SORT(NAME, T, LESS) \
    static inline void NAME##_vs_ins_(T* a_, size_t n_, int guarded_) {                                        \
        for (size_t i_ = 1; i_ < n_; ++i_) {                                                                   \
            if (LESS(a_[i_], a_[i_ - 1])) {                                                                    \
                T t_ = a_[i_];                                                                                 \
                size_t j_ = i_;                                                                                \
                do { a_[j_] = a_[j_ - 1]; --j_; } while ((!guarded_ || j_ > 0) && LESS(t_, (a_ + j_)[-1]));    \
                a_[j_] = t_;                                                                                   \
            }                                                                                                  \
        }                                                                                                      \
    }                                                                                                          \
    static inline int NAME##_vs_partial_ins_(T* a_, size_t n_) {                                               \
        size_t moved_ = 0;                                                                                     \
        for (size_t i_ = 1; i_ < n_; ++i_) {                                                                   \
            if (LESS(a_[i_], a_[i_ - 1])) {                                                                    \
                T t_ = a_[i_];                                                                                 \
                size_t j_ = i_;                                                                                \
                do { a_[j_] = a_[j_ - 1]; --j_; } while (j_ > 0 && LESS(t_, a_[j_ - 1]));                      \
                a_[j_] = t_;                                                                                   \
                moved_ += i_ - j_;                                                                             \
                if (moved_ > 8) { return 0; }                                                                  \
            }                                                                                                  \
        }                                                                                                      \
        return 1;                                                                                              \
    }                                                                                                          \
    static inline void NAME##_vs_sift_(T* a_, size_t i_, size_t n_) {                                          \
        T t_ = a_[i_];                                                                                         \
        for (;;) {                                                                                             \
            size_t c_ = 2 * i_ + 1;                                                                            \
            if (c_ >= n_) { break; }                                                                           \
            if (c_ + 1 < n_ && LESS(a_[c_], a_[c_ + 1])) { ++c_; }                                             \
            if (!LESS(t_, a_[c_])) { break; }                                                                  \
            a_[i_] = a_[c_];                                                                                   \
            i_ = c_;                                                                                           \
        }                                                                                                      \
        a_[i_] = t_;                                                                                           \
    }                                                                                                          \
    static inline void NAME##_vs_heap_(T* a_, size_t n_) {                                                     \
        for (size_t i_ = n_ / 2; i_ > 0; ) { --i_; NAME##_vs_sift_(a_, i_, n_); }                              \
        for (size_t e_ = n_; e_ > 1; ) {                                                                       \
            --e_;                                                                                              \
            T t_ = a_[0]; a_[0] = a_[e_]; a_[e_] = t_;                                                         \
            NAME##_vs_sift_(a_, 0, e_);                                                                        \
        }                                                                                                      \
    }                                                                                                          \
    static inline void NAME##_vs_sort2_(T* a_, size_t i_, size_t j_) {                                         \
        if (LESS(a_[j_], a_[i_])) { T t_ = a_[i_]; a_[i_] = a_[j_]; a_[j_] = t_; }                             \
    }                                                                                                          \
    static inline void NAME##_vs_sort3_(T* a_, size_t i_, size_t j_, size_t k_) {                              \
        NAME##_vs_sort2_(a_, i_, j_);                                                                          \
        NAME##_vs_sort2_(a_, j_, k_);                                                                          \
        NAME##_vs_sort2_(a_, i_, j_);                                                                          \
    }                                                                                                          \
    static inline size_t NAME##_vs_part_left_(T* a_, size_t n_) {                                              \
        T p_ = a_[0];                                                                                          \
        size_t f_ = 0, l_ = n_;                                                                                \
        while (LESS(p_, a_[--l_])) { }                                                                         \
        if (l_ + 1 == n_) { while (f_ < l_ && !LESS(p_, a_[++f_])) { } }                                       \
        else { while (!LESS(p_, a_[++f_])) { } }                                                               \
        while (f_ < l_) {                                                                                      \
            T t_ = a_[f_]; a_[f_] = a_[l_]; a_[l_] = t_;                                                       \
            while (LESS(p_, a_[--l_])) { }                                                                     \
            while (!LESS(p_, a_[++f_])) { }                                                                    \
        }                                                                                                      \
        a_[0] = a_[l_];                                                                                        \
        a_[l_] = p_;                                                                                           \
        return l_;                                                                                             \
    }                                                                                                          \
    static inline size_t NAME##_vs_part_right_(T* a_, size_t n_, int* done_) {                                 \
        T p_ = a_[0];                                                                                          \
        size_t f_ = 0, l_ = n_;                                                                                \
        while (LESS(a_[++f_], p_)) { }                                                                         \
        if (f_ == 1) { while (f_ < l_ && !LESS(a_[--l_], p_)) { } }                                            \
        else { while (!LESS(a_[--l_], p_)) { } }                                                               \
        *done_ = f_ >= l_;                                                                                     \
        if (!*done_) {                                                                                         \
            T t_ = a_[f_]; a_[f_] = a_[l_]; a_[l_] = t_;                                                       \
            ++f_;                                                                                              \
            unsigned char offl_[64], offr_[64];                                                                \
            size_t basel_ = f_, baser_ = l_;                                                                   \
            size_t numl_ = 0, numr_ = 0, startl_ = 0, startr_ = 0;                                             \
            while (f_ < l_) {                                                                                  \
                size_t unknown_ = l_ - f_;                                                                     \
                size_t lsplit_ = numl_ == 0 ? (numr_ == 0 ? unknown_ / 2 : unknown_) : 0;                      \
                size_t rsplit_ = numr_ == 0 ? (unknown_ - lsplit_) : 0;                                        \
                if (lsplit_ > 64) { lsplit_ = 64; }                                                            \
                if (rsplit_ > 64) { rsplit_ = 64; }                                                            \
                for (size_t i_ = 0; i_ < lsplit_; ++i_) {                                                      \
                    offl_[numl_] = (unsigned char)i_;                                                          \
                    numl_ += !LESS(a_[f_], p_);                                                                \
                    ++f_;                                                                                      \
                }                                                                                              \
                for (size_t i_ = 0; i_ < rsplit_; ) {                                                          \
                    offr_[numr_] = (unsigned char)++i_;                                                        \
                    numr_ += LESS(a_[--l_], p_);                                                               \
                }                                                                                              \
                size_t num_ = numl_ < numr_ ? numl_ : numr_;                                                   \
                if (numl_ == numr_) {                                                                          \
                    for (size_t i_ = 0; i_ < num_; ++i_) {                                                     \
                        size_t x_ = basel_ + offl_[startl_ + i_], y_ = baser_ - offr_[startr_ + i_];           \
                        T t2_ = a_[x_]; a_[x_] = a_[y_]; a_[y_] = t2_;                                         \
                    }                                                                                          \
                }                                                                                              \
                else if (num_ > 0) {                                                                           \
                    size_t x_ = basel_ + offl_[startl_], y_ = baser_ - offr_[startr_];                         \
                    T t2_ = a_[x_];                                                                            \
                    a_[x_] = a_[y_];                                                                           \
                    for (size_t i_ = 1; i_ < num_; ++i_) {                                                     \
                        x_ = basel_ + offl_[startl_ + i_];                                                     \
                        a_[y_] = a_[x_];                                                                       \
                        y_ = baser_ - offr_[startr_ + i_];                                                     \
                        a_[x_] = a_[y_];                                                                       \
                    }                                                                                          \
                    a_[y_] = t2_;                                                                              \
                }                                                                                              \
                numl_ -= num_; numr_ -= num_;                                                                  \
                startl_ += num_; startr_ += num_;                                                              \
                if (numl_ == 0) { startl_ = 0; basel_ = f_; }                                                  \
                if (numr_ == 0) { startr_ = 0; baser_ = l_; }                                                  \
            }                                                                                                  \
            if (numl_) {                                                                                       \
                while (numl_--) {                                                                              \
                    size_t x_ = basel_ + offl_[startl_ + numl_];                                               \
                    --l_;                                                                                      \
                    T t2_ = a_[x_]; a_[x_] = a_[l_]; a_[l_] = t2_;                                             \
                }                                                                                              \
                f_ = l_;                                                                                       \
            }                                                                                                  \
            if (numr_) {                                                                                       \
                while (numr_--) {                                                                              \
                    size_t y_ = baser_ - offr_[startr_ + numr_];                                               \
                    T t2_ = a_[y_]; a_[y_] = a_[f_]; a_[f_] = t2_;                                             \
                    ++f_;                                                                                      \
                }                                                                                              \
                l_ = f_;                                                                                       \
            }                                                                                                  \
        }                                                                                                      \
        size_t pos_ = f_ - 1;                                                                                  \
        a_[0] = a_[pos_];                                                                                      \
        a_[pos_] = p_;                                                                                         \
        return pos_;                                                                                           \
    }                                                                                                          \
    static inline void NAME##_vs_loop_(T* a_, size_t n_, int bad_, int leftmost_) {                            \
        for (;;) {                                                                                             \
            if (n_ < 24) {                                                                                     \
                NAME##_vs_ins_(a_, n_, leftmost_);                                                             \
                return;                                                                                        \
            }                                                                                                  \
            size_t h_ = n_ / 2;                                                                                \
            if (n_ > 128) {                                                                                    \
                NAME##_vs_sort3_(a_, 0, h_, n_ - 1);                                                           \
                NAME##_vs_sort3_(a_, 1, h_ - 1, n_ - 2);                                                       \
                NAME##_vs_sort3_(a_, 2, h_ + 1, n_ - 3);                                                       \
                NAME##_vs_sort3_(a_, h_ - 1, h_, h_ + 1);                                                      \
                T t_ = a_[0]; a_[0] = a_[h_]; a_[h_] = t_;                                                     \
            }                                                                                                  \
            else {                                                                                             \
                NAME##_vs_sort3_(a_, h_, 0, n_ - 1);                                                           \
            }                                                                                                  \
            if (!leftmost_ && !LESS(a_[-1], a_[0])) {                                                          \
                size_t pos_ = NAME##_vs_part_left_(a_, n_) + 1;                                                \
                a_ += pos_;                                                                                    \
                n_ -= pos_;                                                                                    \
                continue;                                                                                      \
            }                                                                                                  \
            int done_;                                                                                         \
            size_t pos_ = NAME##_vs_part_right_(a_, n_, &done_);                                               \
            size_t ln_ = pos_, rn_ = n_ - pos_ - 1;                                                            \
            if (ln_ < n_ / 8 || rn_ < n_ / 8) {                                                                \
                if (--bad_ == 0) {                                                                             \
                    NAME##_vs_heap_(a_, n_);                                                                   \
                    return;                                                                                    \
                }                                                                                              \
                if (ln_ >= 24) {                                                                               \
                    T* b_ = a_; T* e_ = a_ + pos_; size_t q_ = ln_ / 4; T t_;                                  \
                    t_ = b_[0]; b_[0] = b_[q_]; b_[q_] = t_;                                                   \
                    t_ = e_[-1]; e_[-1] = e_[-(ptrdiff_t)q_]; e_[-(ptrdiff_t)q_] = t_;                         \
                    if (ln_ > 128) {                                                                           \
                        t_ = b_[1]; b_[1] = b_[q_ + 1]; b_[q_ + 1] = t_;                                       \
                        t_ = b_[2]; b_[2] = b_[q_ + 2]; b_[q_ + 2] = t_;                                       \
                        t_ = e_[-2]; e_[-2] = e_[-(ptrdiff_t)(q_ + 1)]; e_[-(ptrdiff_t)(q_ + 1)] = t_;         \
                        t_ = e_[-3]; e_[-3] = e_[-(ptrdiff_t)(q_ + 2)]; e_[-(ptrdiff_t)(q_ + 2)] = t_;         \
                    }                                                                                          \
                }                                                                                              \
                if (rn_ >= 24) {                                                                               \
                    T* b_ = a_ + pos_ + 1; T* e_ = a_ + n_; size_t q_ = rn_ / 4; T t_;                         \
                    t_ = b_[0]; b_[0] = b_[q_]; b_[q_] = t_;                                                   \
                    t_ = e_[-1]; e_[-1] = e_[-(ptrdiff_t)q_]; e_[-(ptrdiff_t)q_] = t_;                         \
                    if (rn_ > 128) {                                                                           \
                        t_ = b_[1]; b_[1] = b_[q_ + 1]; b_[q_ + 1] = t_;                                       \
                        t_ = b_[2]; b_[2] = b_[q_ + 2]; b_[q_ + 2] = t_;                                       \
                        t_ = e_[-2]; e_[-2] = e_[-(ptrdiff_t)(q_ + 1)]; e_[-(ptrdiff_t)(q_ + 1)] = t_;         \
                        t_ = e_[-3]; e_[-3] = e_[-(ptrdiff_t)(q_ + 2)]; e_[-(ptrdiff_t)(q_ + 2)] = t_;         \
                    }                                                                                          \
                }                                                                                              \
            }                                                                                                  \
            else if (done_ && NAME##_vs_partial_ins_(a_, ln_) && NAME##_vs_partial_ins_(a_ + pos_ + 1, rn_)) { \
                return;                                                                                        \
            }                                                                                                  \
            NAME##_vs_loop_(a_, ln_, bad_, leftmost_);                                                         \
            a_ += pos_ + 1;                                                                                    \
            n_ = rn_;                                                                                          \
            leftmost_ = 0;                                                                                     \
        }                                                                                                      \
    }                                                                                                          \
    VECBUF_MAYBE_UNUSED static inline void NAME##_array(T* a_, size_t n_) {                                    \
        if (a_ && n_ > 1) {                                                                                    \
            int bad_ = 0;                                                                                      \
            for (size_t k_ = n_; k_ > 1; k_ >>= 1) { bad_++; }                                                 \
            NAME##_vs_loop_(a_, n_, bad_, 1);                                                                  \
        }                                                                                                      \
    }                                                                                                          \
    VECBUF_MAYBE_UNUSED static inline void NAME(Vector* vec_) {                                                \
        if (vec_) { NAME##_array((T*)vec_->items, vec_->size); }                                               \
    }


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Vector*     vector_create                (size_t itemSize);
Vector*     vector_create_with_capacity  (size_t itemSize, size_t capacity);
Vector*     vector_from_array            (size_t itemSize, const void* data, size_t count);
Vector*     vector_from_buffer           (void* buffer, size_t itemSize, size_t count, size_t capacity);
Vector*     vector_copy                  (const Vector* src);
Vector*     vector_slice                 (const Vector* vec, size_t pos, size_t count);
void*       vector_release               (Vector* vec, size_t* count);
void        vector_set_compare           (Vector* vec, VectorCompareFunc compare);
void        vector_deallocate            (Vector* vec);


/* ------------------------------------------------------------------ */
/* Capacity                                                           */
/* ------------------------------------------------------------------ */

/* vector_size / vector_capacity / vector_item_size / vector_is_empty are inline above. */
size_t      vector_max_size              (Vector* vec);
bool        vector_reserve               (Vector* vec, size_t size);
bool        vector_resize                (Vector* vec, size_t size);
bool        vector_resize_with           (Vector* vec, size_t size, const void* fill);
void        vector_shrink_to_fit         (Vector* vec);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

/* vector_push_back / vector_pop_back are inline above. */
bool        vector_emplace_back          (Vector* vec, void* item, size_t itemSize);
void*       vector_emplace_back_slot     (Vector* vec);
void        vector_insert                (Vector* vec, size_t pos, void* item);
bool        vector_insert_range          (Vector* vec, size_t pos, const void* data, size_t count);
bool        vector_insert_fill           (Vector* vec, size_t pos, size_t count, const void* item);
void        vector_emplace               (Vector* vec, size_t pos, void* item, size_t itemSize);
void        vector_assign                (Vector* vec, size_t pos, void* item);
bool        vector_assign_range          (Vector* vec, const void* data, size_t count);
bool        vector_assign_fill           (Vector* vec, size_t count, const void* item);
void        vector_fill                  (Vector* vec, const void* item);
bool        vector_append                (Vector* vec, const void* data, size_t count);
void*       vector_append_uninit         (Vector* vec, size_t count);
bool        vector_append_vector         (Vector* dst, const Vector* src);
void        vector_erase                 (Vector* vec, size_t pos, size_t len);
bool        vector_swap_remove           (Vector* vec, size_t pos);
bool        vector_pop_back_value        (Vector* vec, void* out);
size_t      vector_erase_if              (Vector* vec, VectorPredicateFunc pred, void* ctx);
size_t      vector_remove_value          (Vector* vec, const void* value);
void        vector_clear                 (Vector* vec);
void        vector_swap                  (Vector* vec1, Vector* vec2);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

/* vector_at / vector_front / vector_back / vector_data are inline above. */


/* ------------------------------------------------------------------ */
/* Iterators                                                          */
/* ------------------------------------------------------------------ */

/* vector_begin / vector_end are inline above. */
void*       vector_rbegin                (Vector* vec);
void*       vector_rend                  (Vector* vec);
const void* vector_cbegin                (Vector* vec);
const void* vector_cend                  (Vector* vec);
const void* vector_crbegin               (Vector* vec);
const void* vector_crend                 (Vector* vec);


/* ------------------------------------------------------------------ */
/* Searching / algorithms                                             */
/* ------------------------------------------------------------------ */

size_t      vector_find                  (const Vector* vec, const void* value);
bool        vector_contains              (const Vector* vec, const void* value);
bool        vector_sort                  (Vector* vec, VectorCompareFunc compare);
size_t      vector_binary_search         (const Vector* vec, const void* value, VectorCompareFunc compare);
void        vector_reverse               (Vector* vec);
size_t      vector_unique                (Vector* vec);
void        vector_for_each              (Vector* vec, VectorVisitFunc fn, void* ctx);


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
