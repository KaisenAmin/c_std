/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Algorithm
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#if defined(_MSC_VER)
#include <intrin.h>   /* _BitScanForward, _mm_prefetch */
#endif
#include "algorithm.h"





/* ================================================================== */
/* Internal helpers                                                   */
/* ================================================================== */

/* Force-inlined helpers: they are written once with a `size_t sz` element
 * size and instantiated with a constant 4 / 8 / 16 (see the *_4 / *_8 /
 * *_16 / *_any wrappers below), so for the common element sizes every
 * memcpy(.., sz) becomes a single register move. */
#if defined(_MSC_VER)
#  define ALG_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define ALG_INLINE static inline __attribute__((always_inline))
#else
#  define ALG_INLINE static inline
#endif

/* Prefetch hint (no-op where unavailable). */
#if defined(__GNUC__) || defined(__clang__)
#  define ALG_PREFETCH(p) __builtin_prefetch(p)
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#  define ALG_PREFETCH(p) _mm_prefetch((const char *)(p), _MM_HINT_T0)
#else
#  define ALG_PREFETCH(p) ((void)0)
#endif

/* Arrays larger than this are binary-searched branchlessly with prefetching
 * (memory-bound); smaller ones with the branchy loop, which lets the CPU
 * start the next comparison speculatively and wins while data is cached. */
#define ALG_BSEARCH_PREFETCH_BYTES (512u * 1024u)

/* Scratch space for one element that may be passed to a comparator: aligned
 * for any scalar type (the comparator reads it as the caller's type). */
typedef union {
    long double   ld;
    long long     ll;
    double        d;
    void*         p;
    unsigned char bytes[256];
} AlgScratch;


/* Copies one element; constant sizes compile to plain moves. */
ALG_INLINE void alg_copy_elem(void *dst, const void *src, size_t sz) {
    switch (sz) {
        case 1:  memcpy(dst, src, 1);  break;
        case 2:  memcpy(dst, src, 2);  break;
        case 4:  memcpy(dst, src, 4);  break;
        case 8:  memcpy(dst, src, 8);  break;
        case 16: memcpy(dst, src, 16); break;
        default: memcpy(dst, src, sz); break;
    }
}


/* Swaps `n` bytes between two non-overlapping regions (distinct pointers),
 * in 64-byte register-sized steps; no allocation. */
ALG_INLINE void alg_swap_bytes(void *a, void *b, size_t n) {
    unsigned char *pa = (unsigned char *)a;
    unsigned char *pb = (unsigned char *)b;
    unsigned char t[64];
    while (n > sizeof t) {
        memcpy(t, pa, sizeof t);
        memcpy(pa, pb, sizeof t);
        memcpy(pb, t, sizeof t);
        pa += sizeof t;
        pb += sizeof t;
        n  -= sizeof t;
    }
    memcpy(t, pa, n);
    memcpy(pa, pb, n);
    memcpy(pb, t, n);
}


/* Swaps two elements (a == b allowed); the usual sizes become plain moves. */
ALG_INLINE void swap(void *a, void *b, size_t size) {
    if (a == b) {
        return;
    }
    switch (size) {
        case 1:  alg_swap_bytes(a, b, 1);    break;
        case 2:  alg_swap_bytes(a, b, 2);    break;
        case 4:  alg_swap_bytes(a, b, 4);    break;
        case 8:  alg_swap_bytes(a, b, 8);    break;
        case 16: alg_swap_bytes(a, b, 16);   break;
        default: alg_swap_bytes(a, b, size); break;
    }
}


/* Reverses the elements of [first, last). */
static void reverse(void *first, void *last, size_t size) {
    ALGORITHM_LOG("[reverse] Reversing elements between memory locations %p and %p, each of size %zu bytes.", first, last, size);

    char *a = (char *)first;
    char *b = (char *)last;
    if (a == b) {
        return;
    }
    b -= size;
    switch (size) {
        case 4:  while (a < b) { alg_swap_bytes(a, b, 4);  a += 4;  b -= 4;  } break;
        case 8:  while (a < b) { alg_swap_bytes(a, b, 8);  a += 8;  b -= 8;  } break;
        default: while (a < b) { alg_swap_bytes(a, b, size); a += size; b -= size; } break;
    }

    ALGORITHM_LOG("[reverse] Completed reversing elements.");
}


/* Writes `count` copies of the `sz`-byte `val` at `dst` (val outside the
 * destination). Bytes all equal -> memset; otherwise the filled prefix is
 * doubled with memcpy, in chunks of at most 64 KiB so it stays in cache. */
static void alg_fill_bytes(char *dst, const void *val, size_t sz, size_t count) {
    if (count == 0 || sz == 0) {
        return;
    }
    const unsigned char *b = (const unsigned char *)val;
    size_t k = 1;
    while (k < sz && b[k] == b[0]) {
        k++;
    }
    if (k == sz) {
        memset(dst, b[0], count * sz);
        return;
    }
    memcpy(dst, val, sz);
    size_t done = 1;
    size_t max_chunk = (65536 / sz) ? (65536 / sz) : 1;
    while (done < count) {
        size_t chunk = done < max_chunk ? done : max_chunk;
        if (chunk > count - done) {
            chunk = count - done;
        }
        memcpy(dst + done * sz, dst, chunk * sz);
        done += chunk;
    }
}


/* Rotates the bytes of [p, p + left + right) so the `right` bytes that start
 * at p + left come first. The smaller side goes through a stack buffer (or a
 * heap one) and the larger is moved with one memmove; without memory it falls
 * back to in-place block swaps (Gries-Mills), O(n) either way. */
static void alg_rotate_bytes(char *p, size_t left, size_t right) {
    if (left == 0 || right == 0) {
        return;
    }
    unsigned char stackbuf[2048];
    size_t small = left < right ? left : right;
    unsigned char *tmp = (small <= sizeof stackbuf) ? stackbuf : (unsigned char *)malloc(small);
    if (tmp) {
        if (left <= right) {
            memcpy(tmp, p, left);
            memmove(p, p + left, right);
            memcpy(p + right, tmp, left);
        }
        else {
            memcpy(tmp, p + left, right);
            memmove(p + right, p, left);
            memcpy(p, tmp, right);
        }
        if (tmp != stackbuf) {
            free(tmp);
        }
        return;
    }
    while (left != 0 && right != 0) {
        if (left <= right) {           /* A B1 B2 -> B1 A B2 (B1 is final) */
            alg_swap_bytes(p, p + left, left);
            p += left;
            right -= left;
        }
        else {                         /* A1 A2 B -> A1 B A2 (A2 is final) */
            alg_swap_bytes(p + left - right, p + left, right);
            left -= right;
        }
    }
}


/* Uniform random index in [0, bound) from a generator of uniformly random
 * 32-bit values: rng() % bound, as algorithm_shuffle has always computed it,
 * except that a draw from the incomplete top range (r >= the largest
 * multiple of bound that fits in 2^32) is rejected and drawn again - that
 * tail is what made rng() % bound favour small indices. Accepted draws give
 * exactly the old index, so the same generator still gives the same
 * permutation unless a rejection happens (probability (2^32 mod bound) / 2^32
 * per step). Bounds above 2^32 take two calls per draw. */
static size_t alg_random_index(UniformRandomNumberGenerator rng, size_t bound) {
    if ((uint64_t)bound <= UINT64_C(0xFFFFFFFF)) {
        uint32_t b = (uint32_t)bound;
        uint32_t r = rng();
        /* The rejected tail is the top (2^32 mod b) < b values. A draw below
           2^32 - b can never be in it, so the exact limit (a second
           division) is only computed for the rare draw in the top b values. */
        if (r > UINT32_C(0xFFFFFFFF) - b) {
            uint32_t tail = (uint32_t)((UINT64_C(1) << 32) % b);     /* 2^32 mod b */
            while (tail != 0 && r > UINT32_C(0xFFFFFFFF) - tail) {
                r = rng();
            }
        }
        return (size_t)(r % b);                                     /* 32-bit division */
    }
    uint64_t b = (uint64_t)bound;
    uint64_t rem = (UINT64_MAX % b + 1) % b;             /* 2^64 mod b */
    uint64_t r;
    do {
        r = ((uint64_t)rng() << 32) | (uint64_t)rng();
    } while (rem != 0 && r > UINT64_MAX - rem);
    return (size_t)(r % b);
}


/* ================================================================== */
/* Filtering core (filter, copy_if, remove_copy_if)                  */
/* ================================================================== */

/* Index of the lowest set bit of a non-zero 64-bit mask. */
ALG_INLINE unsigned alg_ctz64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return (unsigned)__builtin_ctzll(x);
#elif defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
    unsigned long i;
    _BitScanForward64(&i, x);
    return (unsigned)i;
#elif defined(_MSC_VER)
    unsigned long i;
    if ((uint32_t)x) {
        _BitScanForward(&i, (unsigned long)(uint32_t)x);
        return (unsigned)i;
    }
    _BitScanForward(&i, (unsigned long)(uint32_t)(x >> 32));
    return (unsigned)i + 32;
#else
    unsigned n = 0;
    while (!(x & 1u)) {
        x >>= 1;
        n++;
    }
    return n;
#endif
}

/* Python truthiness for raw elements: true unless every byte is zero
 * (0, 0.0, NULL, '\0', an all-zero struct). */
ALG_INLINE int alg_is_truthy(const char *p, size_t sz) {
    switch (sz) {
        case 1: return p[0] != 0;
        case 2: { uint16_t v; memcpy(&v, p, 2); return v != 0; }
        case 4: { uint32_t v; memcpy(&v, p, 4); return v != 0; }
        case 8: { uint64_t v; memcpy(&v, p, 8); return v != 0; }
        default:
            for (size_t i = 0; i < sz; i++) {
                if (p[i]) {
                    return 1;
                }
            }
            return 0;
    }
}

/* The filter loop. 64 elements at a time: first the keep / drop decision of
 * each one goes into a bit mask (no branch on the predicate's result), then
 * only the kept elements are copied, walking the set bits. So unpredictable
 * predicates cost no mispredictions, nothing is written for dropped
 * elements (the result only needs room for the kept ones), and in-place
 * filtering moves nothing until the first dropped element. mode: 0 = pred,
 * 1 = pred with context, 2 = truthiness, 3 = NOT pred (remove_copy_if). */
ALG_INLINE size_t alg_filter_loop(const char *src, size_t num, size_t sz, char *dst, int mode,
                                  BoolPredicateFunc pred, ContextPredicateFunc cpred, void *ctx) {
    size_t kept = 0;
    for (size_t i = 0; i < num; i += 64) {
        size_t m = (num - i < 64) ? num - i : 64;
        const char *blk = src + i * sz;
        uint64_t mask = 0;
        for (size_t j = 0; j < m; ++j) {
            const char *e = blk + j * sz;
            int keep = (mode == 0) ? (pred(e) != 0)
                     : (mode == 1) ? (cpred(e, ctx) != 0)
                     : (mode == 3) ? (pred(e) == 0)
                     : alg_is_truthy(e, sz);
            mask |= (uint64_t)keep << j;
        }
        uint64_t all = (m == 64) ? ~UINT64_C(0) : ((UINT64_C(1) << m) - 1);
        if (mask == all && dst + kept * sz == blk) {
            kept += m;                  /* in place and nothing dropped yet: nothing to move */
            continue;
        }
        while (mask) {
            unsigned j = alg_ctz64(mask);
            mask &= mask - 1;
            char *d = dst + kept * sz;
            const char *e = blk + (size_t)j * sz;
            if (d != e) {
                alg_copy_elem(d, e, sz);
            }
            kept++;
        }
    }
    return kept;
}

static size_t alg_filter_dispatch(const char *src, size_t num, size_t sz, char *dst,
                                  BoolPredicateFunc pred, ContextPredicateFunc cpred, void *ctx, int negate) {
    if (pred && negate) {
        switch (sz) {
            case 4:  return alg_filter_loop(src, num, 4, dst, 3, pred, NULL, NULL);
            case 8:  return alg_filter_loop(src, num, 8, dst, 3, pred, NULL, NULL);
            default: return alg_filter_loop(src, num, sz, dst, 3, pred, NULL, NULL);
        }
    }
    if (pred) {
        switch (sz) {
            case 4:  return alg_filter_loop(src, num, 4, dst, 0, pred, NULL, NULL);
            case 8:  return alg_filter_loop(src, num, 8, dst, 0, pred, NULL, NULL);
            default: return alg_filter_loop(src, num, sz, dst, 0, pred, NULL, NULL);
        }
    }
    if (cpred) {
        switch (sz) {
            case 4:  return alg_filter_loop(src, num, 4, dst, 1, NULL, cpred, ctx);
            case 8:  return alg_filter_loop(src, num, 8, dst, 1, NULL, cpred, ctx);
            default: return alg_filter_loop(src, num, sz, dst, 1, NULL, cpred, ctx);
        }
    }
    switch (sz) {
        case 1:  return alg_filter_loop(src, num, 1, dst, 2, NULL, NULL, NULL);
        case 2:  return alg_filter_loop(src, num, 2, dst, 2, NULL, NULL, NULL);
        case 4:  return alg_filter_loop(src, num, 4, dst, 2, NULL, NULL, NULL);
        case 8:  return alg_filter_loop(src, num, 8, dst, 2, NULL, NULL, NULL);
        default: return alg_filter_loop(src, num, sz, dst, 2, NULL, NULL, NULL);
    }
}


/* ================================================================== */
/* Unstable sort: pattern-defeating quicksort (pdqsort)               */
/* ================================================================== */
/*
 * Orson Peters' pdqsort (the unstable sort of Rust and Go), adapted to opaque
 * elements and a comparator called through a pointer: median-of-3 / ninther
 * pivot, branchless block partitioning (the comparison results are counted,
 * never branched on, so random data costs no mispredictions), pattern
 * breaking on unbalanced partitions, a separate pass for runs of elements
 * equal to the pivot, an insertion-sort exit for already partitioned input,
 * and heapsort after log2(n) bad partitions: O(n log n) worst case, O(n) for
 * sorted, reversed and all-equal input. The pivot stays at a[0] during a
 * partition, so it is compared in place (no copy); `tmp` (one element of
 * scratch) is used by insertion sort, heapsort and the cyclic block swaps.
 */
#define PDQ_INSERTION_MAX 24
#define PDQ_NINTHER_MIN   128
#define PDQ_PARTIAL_LIMIT 8
#define PDQ_BLOCK         64

typedef void (*PdqLoopFunc)(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int bad, int leftmost);

ALG_INLINE void pdq_insertion(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int guarded) {
    for (size_t i = 1; i < n; ++i) {
        char *cur = a + i * sz;
        if (comp(cur, cur - sz) < 0) {
            alg_copy_elem(tmp, cur, sz);
            char *hole = cur;
            do {
                alg_copy_elem(hole, hole - sz, sz);
                hole -= sz;
            } while ((!guarded || hole != a) && comp(tmp, hole - sz) < 0);
            alg_copy_elem(hole, tmp, sz);
        }
    }
}

/* Insertion sort that gives up after PDQ_PARTIAL_LIMIT element moves; true
 * when it sorted the range. */
ALG_INLINE int pdq_partial_insertion(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp) {
    size_t moved = 0;
    for (size_t i = 1; i < n; ++i) {
        char *cur = a + i * sz;
        if (comp(cur, cur - sz) < 0) {
            alg_copy_elem(tmp, cur, sz);
            char *hole = cur;
            do {
                alg_copy_elem(hole, hole - sz, sz);
                hole -= sz;
            } while (hole != a && comp(tmp, hole - sz) < 0);
            alg_copy_elem(hole, tmp, sz);
            moved += (size_t)(cur - hole) / sz;
            if (moved > PDQ_PARTIAL_LIMIT) {
                return 0;
            }
        }
    }
    return 1;
}

/* Sift a[i] down the max-heap a[0, end). With `tmp` the element travels
 * through it; without (no memory for a large element) it is swapped down. */
static void pdq_sift_down(char *a, size_t i, size_t end, size_t sz, AlgorithmCompareFunc comp, char *tmp) {
    if (tmp) {
        alg_copy_elem(tmp, a + i * sz, sz);
    }
    for (;;) {
        size_t c = 2 * i + 1;
        if (c >= end) {
            break;
        }
        if (c + 1 < end && comp(a + c * sz, a + (c + 1) * sz) < 0) {
            ++c;
        }
        if (!(comp(tmp ? tmp : a + i * sz, a + c * sz) < 0)) {
            break;
        }
        if (tmp) {
            alg_copy_elem(a + i * sz, a + c * sz, sz);
        }
        else {
            alg_swap_bytes(a + i * sz, a + c * sz, sz);
        }
        i = c;
    }
    if (tmp) {
        alg_copy_elem(a + i * sz, tmp, sz);
    }
}

/* Heapsort: introsort's O(n log n) fallback (and the whole sort when there
 * is no memory for a scratch element). */
static void pdq_heapsort(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp) {
    for (size_t i = n / 2; i > 0; ) {
        --i;
        pdq_sift_down(a, i, n, sz, comp, tmp);
    }
    for (size_t end = n; end > 1; ) {
        --end;
        alg_swap_bytes(a, a + end * sz, sz);
        pdq_sift_down(a, 0, end, sz, comp, tmp);
    }
}

ALG_INLINE void pdq_sort2(char *x, char *y, size_t sz, AlgorithmCompareFunc comp) {
    if (comp(y, x) < 0) {
        alg_swap_bytes(x, y, sz);
    }
}

ALG_INLINE void pdq_sort3(char *x, char *y, char *z, size_t sz, AlgorithmCompareFunc comp) {
    pdq_sort2(x, y, sz, comp);
    pdq_sort2(y, z, sz, comp);
    pdq_sort2(x, y, sz, comp);
}

/* Partition around the pivot a[0]: elements equal to it go LEFT. Used when
 * the pivot equals the element just before the range (a run of equal
 * elements), which then needs no further sorting. Returns the pivot's index. */
ALG_INLINE size_t pdq_partition_left(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp) {
    const char *p = a;
    size_t f = 0, l = n;
    while (comp(p, a + (--l) * sz) < 0) { }
    if (l + 1 == n) {
        while (f < l && !(comp(p, a + (++f) * sz) < 0)) { }
    }
    else {
        while (!(comp(p, a + (++f) * sz) < 0)) { }
    }
    while (f < l) {
        alg_swap_bytes(a + f * sz, a + l * sz, sz);
        while (comp(p, a + (--l) * sz) < 0) { }
        while (!(comp(p, a + (++f) * sz) < 0)) { }
    }
    if (l) {
        alg_swap_bytes(a, a + l * sz, sz);
    }
    return l;
}

/* Partition around the pivot a[0] (elements equal to it go right) with
 * branchless block partitioning (Edelkamp & Weiss, BlockQuicksort, as tuned
 * in pdqsort). Returns the pivot's final index; *done is set when the range
 * was already partitioned. */
ALG_INLINE size_t pdq_partition_right(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int *done) {
    const char *p = a;
    size_t f = 0, l = n;
    while (comp(a + (++f) * sz, p) < 0) { }
    if (f == 1) {
        while (f < l && !(comp(a + (--l) * sz, p) < 0)) { }
    }
    else {
        while (!(comp(a + (--l) * sz, p) < 0)) { }
    }
    *done = f >= l;
    if (!*done) {
        alg_swap_bytes(a + f * sz, a + l * sz, sz);
        ++f;
        unsigned char offl[PDQ_BLOCK], offr[PDQ_BLOCK];
        size_t basel = f, baser = l;
        size_t numl = 0, numr = 0, startl = 0, startr = 0;
        while (f < l) {
            size_t unknown = l - f;
            size_t lsplit = (numl == 0) ? ((numr == 0) ? unknown / 2 : unknown) : 0;
            size_t rsplit = (numr == 0) ? (unknown - lsplit) : 0;
            if (lsplit > PDQ_BLOCK) {
                lsplit = PDQ_BLOCK;
            }
            if (rsplit > PDQ_BLOCK) {
                rsplit = PDQ_BLOCK;
            }
            for (size_t i = 0; i < lsplit; ++i) {
                offl[numl] = (unsigned char)i;
                numl += !(comp(a + f * sz, p) < 0);
                ++f;
            }
            for (size_t i = 0; i < rsplit; ) {
                offr[numr] = (unsigned char)++i;
                numr += comp(a + (--l) * sz, p) < 0;
            }
            size_t num = numl < numr ? numl : numr;
            if (num) {
                if (numl == numr) {
                    for (size_t i = 0; i < num; ++i) {
                        alg_swap_bytes(a + (basel + offl[startl + i]) * sz, a + (baser - offr[startr + i]) * sz, sz);
                    }
                }
                else {
                    size_t x = basel + offl[startl], y = baser - offr[startr];
                    alg_copy_elem(tmp, a + x * sz, sz);
                    alg_copy_elem(a + x * sz, a + y * sz, sz);
                    for (size_t i = 1; i < num; ++i) {
                        x = basel + offl[startl + i];
                        alg_copy_elem(a + y * sz, a + x * sz, sz);
                        y = baser - offr[startr + i];
                        alg_copy_elem(a + x * sz, a + y * sz, sz);
                    }
                    alg_copy_elem(a + y * sz, tmp, sz);
                }
            }
            numl -= num;
            numr -= num;
            startl += num;
            startr += num;
            if (numl == 0) {
                startl = 0;
                basel = f;
            }
            if (numr == 0) {
                startr = 0;
                baser = l;
            }
        }
        if (numl) {
            while (numl--) {
                --l;
                char *x = a + (basel + offl[startl + numl]) * sz;
                if (x != a + l * sz) {
                    alg_swap_bytes(x, a + l * sz, sz);
                }
            }
            f = l;
        }
        if (numr) {
            while (numr--) {
                char *y = a + (baser - offr[startr + numr]) * sz;
                if (y != a + f * sz) {
                    alg_swap_bytes(y, a + f * sz, sz);
                }
                ++f;
            }
            l = f;
        }
    }
    size_t pos = f - 1;
    if (pos) {
        alg_swap_bytes(a, a + pos * sz, sz);
    }
    return pos;
}

/* Swaps a[i] and a[j] when they differ (pattern breaking). */
ALG_INLINE void pdq_swap_idx(char *a, size_t i, size_t j, size_t sz) {
    if (i != j) {
        alg_swap_bytes(a + i * sz, a + j * sz, sz);
    }
}

ALG_INLINE void pdq_loop_impl(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp,
                              int bad, int leftmost, PdqLoopFunc self) {
    for (;;) {
        if (n < PDQ_INSERTION_MAX) {
            pdq_insertion(a, n, sz, comp, tmp, leftmost);
            return;
        }
        size_t h = n / 2;
        if (n > PDQ_NINTHER_MIN) {
            pdq_sort3(a, a + h * sz, a + (n - 1) * sz, sz, comp);
            pdq_sort3(a + sz, a + (h - 1) * sz, a + (n - 2) * sz, sz, comp);
            pdq_sort3(a + 2 * sz, a + (h + 1) * sz, a + (n - 3) * sz, sz, comp);
            pdq_sort3(a + (h - 1) * sz, a + h * sz, a + (h + 1) * sz, sz, comp);
            alg_swap_bytes(a, a + h * sz, sz);
        }
        else {
            pdq_sort3(a + h * sz, a, a + (n - 1) * sz, sz, comp);
        }

        /* the pivot equals the element before this range: put the run of
           equal elements on the left and skip it */
        if (!leftmost && !(comp(a - sz, a) < 0)) {
            size_t pos = pdq_partition_left(a, n, sz, comp) + 1;
            a += pos * sz;
            n -= pos;
            continue;
        }

        int done;
        size_t pos = pdq_partition_right(a, n, sz, comp, tmp, &done);
        size_t ln = pos, rn = n - pos - 1;

        if (ln < n / 8 || rn < n / 8) {
            if (--bad == 0) {
                pdq_heapsort(a, n, sz, comp, tmp);
                return;
            }
            if (ln >= PDQ_INSERTION_MAX) {
                size_t q = ln / 4;
                pdq_swap_idx(a, 0, q, sz);
                pdq_swap_idx(a, pos - 1, pos - q, sz);
                if (ln > PDQ_NINTHER_MIN) {
                    pdq_swap_idx(a, 1, q + 1, sz);
                    pdq_swap_idx(a, 2, q + 2, sz);
                    pdq_swap_idx(a, pos - 2, pos - (q + 1), sz);
                    pdq_swap_idx(a, pos - 3, pos - (q + 2), sz);
                }
            }
            if (rn >= PDQ_INSERTION_MAX) {
                size_t q = rn / 4;
                pdq_swap_idx(a, pos + 1, pos + 1 + q, sz);
                pdq_swap_idx(a, n - 1, n - q, sz);
                if (rn > PDQ_NINTHER_MIN) {
                    pdq_swap_idx(a, pos + 2, pos + 2 + q, sz);
                    pdq_swap_idx(a, pos + 3, pos + 3 + q, sz);
                    pdq_swap_idx(a, n - 2, n - (1 + q), sz);
                    pdq_swap_idx(a, n - 3, n - (2 + q), sz);
                }
            }
        }
        else if (done && pdq_partial_insertion(a, ln, sz, comp, tmp)
                      && pdq_partial_insertion(a + (pos + 1) * sz, rn, sz, comp, tmp)) {
            return;
        }

        self(a, ln, sz, comp, tmp, bad, leftmost);
        a += (pos + 1) * sz;
        n = rn;
        leftmost = 0;
    }
}

static void pdq_loop_4(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int bad, int leftmost) {
    (void)sz;
    pdq_loop_impl(a, n, 4, comp, tmp, bad, leftmost, pdq_loop_4);
}

static void pdq_loop_8(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int bad, int leftmost) {
    (void)sz;
    pdq_loop_impl(a, n, 8, comp, tmp, bad, leftmost, pdq_loop_8);
}

static void pdq_loop_16(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int bad, int leftmost) {
    (void)sz;
    pdq_loop_impl(a, n, 16, comp, tmp, bad, leftmost, pdq_loop_16);
}

static void pdq_loop_any(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, int bad, int leftmost) {
    pdq_loop_impl(a, n, sz, comp, tmp, bad, leftmost, pdq_loop_any);
}


/* ================================================================== */
/* Stable sort: bottom-up merge sort                                  */
/* ================================================================== */
/*
 * Runs of STB_RUN elements are insertion-sorted in place, then merged pairwise
 * level by level, alternating between the array and a buffer of the same
 * size (no copy-back per merge). Two runs that are already in order (last of
 * the left <= first of the right) are copied instead of merged, so sorted
 * input costs O(n). The merge picks the next element without a branch on the
 * comparison. Without memory for the buffer, the runs are merged in place
 * with rotations instead (O(n log^2 n), still stable).
 */
#define STB_RUN 16
#ifndef STB_BLOCK_BYTES
#define STB_BLOCK_BYTES (128u * 1024u)    /* sorted in cache before the global merges */
#endif

/* Stable merge of [l, le) and [r, re) into out (not overlapping either). */
ALG_INLINE void stb_merge(const char *l, const char *le, const char *r, const char *re, char *out,
                          size_t sz, AlgorithmCompareFunc comp) {
    if (l != le && r != re && comp(r, le - sz) < 0) {
        while (l != le && r != re) {
            int take_r = comp(r, l) < 0;
            const char *s = take_r ? r : l;
            alg_copy_elem(out, s, sz);
            out += sz;
            r += take_r ? sz : 0;
            l += take_r ? 0 : sz;
        }
    }
    if (l != le) {
        memcpy(out, l, (size_t)(le - l));
        out += le - l;
    }
    if (r != re) {
        memcpy(out, r, (size_t)(re - r));
    }
}

/* Stable merge of two sorted runs of the SAME length n, a and b (= a + n),
 * into out: the "parity merge" (Scandum's quadsort). The front picks the
 * smallest remaining element and the back the largest, n steps each; ties
 * go to `a` at the front and to `b` at the back, which keeps the merge
 * stable and makes front and back take disjoint elements, so no bounds
 * checks are needed. The two ends are independent chains of comparisons,
 * which the CPU overlaps: about twice the throughput of a one-ended merge. */
ALG_INLINE void stb_parity_merge(const char *a, size_t n, char *out, size_t sz, AlgorithmCompareFunc comp) {
    const char *b  = a + n * sz;
    const char *ae = b - sz;
    const char *be = b + (n - 1) * sz;
    char *oe = out + (2 * n - 1) * sz;
    for (size_t k = 0; k < n; ++k) {
        int f = comp(b, a) < 0;          /* front: b strictly smaller -> b */
        int g = comp(be, ae) < 0;        /* back: be strictly smaller -> ae is the largest */
        alg_copy_elem(out, f ? b : a, sz);
        alg_copy_elem(oe, g ? ae : be, sz);
        out += sz;
        oe  -= sz;
        b  += f ? sz : 0;
        a  += f ? 0 : sz;
        ae -= g ? sz : 0;
        be -= g ? 0 : sz;
    }
}

/* Two parity merges of equal-length run pairs (a1 | a1 + n) -> o1 and
 * (a2 | a2 + n) -> o2 in lockstep: four independent comparison chains. */
ALG_INLINE void stb_parity_merge2(const char *a1, const char *a2, size_t n, char *o1, char *o2,
                                  size_t sz, AlgorithmCompareFunc comp) {
    const char *b1 = a1 + n * sz, *ae1 = b1 - sz, *be1 = b1 + (n - 1) * sz;
    const char *b2 = a2 + n * sz, *ae2 = b2 - sz, *be2 = b2 + (n - 1) * sz;
    char *oe1 = o1 + (2 * n - 1) * sz, *oe2 = o2 + (2 * n - 1) * sz;
    for (size_t k = 0; k < n; ++k) {
        int f1 = comp(b1, a1) < 0;
        int f2 = comp(b2, a2) < 0;
        int g1 = comp(be1, ae1) < 0;
        int g2 = comp(be2, ae2) < 0;
        alg_copy_elem(o1, f1 ? b1 : a1, sz);
        alg_copy_elem(o2, f2 ? b2 : a2, sz);
        alg_copy_elem(oe1, g1 ? ae1 : be1, sz);
        alg_copy_elem(oe2, g2 ? ae2 : be2, sz);
        o1 += sz; o2 += sz; oe1 -= sz; oe2 -= sz;
        b1 += f1 ? sz : 0;  a1 += f1 ? 0 : sz;
        b2 += f2 ? sz : 0;  a2 += f2 ? 0 : sz;
        ae1 -= g1 ? sz : 0; be1 -= g1 ? 0 : sz;
        ae2 -= g2 ? sz : 0; be2 -= g2 ? 0 : sz;
    }
}

/* One merge pass: the pairs of `width`-element runs of src go to dst.
 * Ordered pairs are copied, reversed ones swapped as blocks, and pairs of
 * equal-length runs are parity-merged two at a time (four independent
 * comparison chains). */
ALG_INLINE void stb_merge_pass(const char *src, char *dst, size_t n, size_t width, size_t sz, AlgorithmCompareFunc comp) {
    const char *pend = NULL;           /* a run pair waiting for a partner */
    char *pend_out = NULL;
    size_t pend_n = 0;
    for (size_t i = 0; i < n; ) {
        size_t mid = i + ((width < n - i) ? width : n - i);
        size_t end = mid + ((width < n - mid) ? width : n - mid);
        const char *l = src + i * sz, *r = src + mid * sz, *re = src + end * sz;
        char *out = dst + i * sz;
        if (r == re || !(comp(r, r - sz) < 0)) {
            memcpy(out, l, (size_t)(re - l));              /* already in order */
        }
        else if (comp(re - sz, l) < 0) {                   /* right run entirely smaller */
            memcpy(out, r, (size_t)(re - r));
            memcpy(out + (re - r), l, (size_t)(r - l));
        }
        else if (end - mid == mid - i) {
            if (pend && pend_n == mid - i) {
                stb_parity_merge2(pend, l, pend_n, pend_out, out, sz, comp);
                pend = NULL;
            }
            else {
                if (pend) {
                    stb_parity_merge(pend, pend_n, pend_out, sz, comp);
                }
                pend = l;
                pend_out = out;
                pend_n = mid - i;
            }
        }
        else {
            stb_merge(l, r, r, re, out, sz, comp);
        }
        i = end;
    }
    if (pend) {
        stb_parity_merge(pend, pend_n, pend_out, sz, comp);
    }
}

/* Sorts a[0, n) stably: insertion-sorted runs, then merge passes between
 * `a` and `buf` (n elements). Already sorted input and strictly descending
 * input (reversed; reversing it is stable) are recognised in one pass. */
ALG_INLINE void stb_sort_impl(char *a, size_t n, size_t sz, AlgorithmCompareFunc comp, char *tmp, char *buf) {
    size_t asc = 1;
    while (asc < n && !(comp(a + asc * sz, a + (asc - 1) * sz) < 0)) {
        ++asc;
    }
    if (asc == n) {
        return;                                    /* already sorted */
    }
    if (asc == 1) {
        size_t desc = 1;
        while (desc < n && comp(a + desc * sz, a + (desc - 1) * sz) < 0) {
            ++desc;
        }
        if (desc == n) {                           /* strictly descending */
            reverse(a, a + n * sz, sz);
            return;
        }
    }

    /* Cache blocking: each block of about STB_BLOCK_BYTES is sorted completely
       while it sits in the cache; only the remaining merge levels stream
       through the whole array. */
    size_t block = STB_BLOCK_BYTES / sz;
    size_t run_levels = 0;
    while (((size_t)STB_RUN << run_levels) < block) {
        ++run_levels;
    }
    block = (size_t)STB_RUN << run_levels;             /* a power-of-two multiple of STB_RUN */
    for (size_t b0 = 0; b0 < n; b0 += block) {
        size_t bn = (n - b0 < block) ? n - b0 : block;
        char *ba = a + b0 * sz;
        for (size_t i = 0; i < bn; i += STB_RUN) {
            size_t m = (bn - i < STB_RUN) ? bn - i : STB_RUN;
            pdq_insertion(ba + i * sz, m, sz, comp, tmp, 1);
        }
        const char *bsrc = ba;
        char *bdst = buf + b0 * sz;
        for (size_t width = STB_RUN; width < bn; width *= 2) {
            stb_merge_pass(bsrc, bdst, bn, width, sz, comp);
            const char *t = bsrc;
            bsrc = bdst;
            bdst = (char *)t;
        }
        if (bsrc != ba) {
            memcpy(ba, bsrc, bn * sz);                 /* still in cache */
        }
    }
    if (n <= block) {
        return;
    }
    const char *src = a;
    char *dst = buf;
    for (size_t width = block; width < n; width *= 2) {
        stb_merge_pass(src, dst, n, width, sz, comp);
        const char *t = src;
        src = dst;
        dst = (char *)t;
    }
    if (src != a) {
        memcpy(a, src, n * sz);
    }
}

static void stb_sort_4(char *a, size_t n, size_t sz, AlgorithmCompareFunc c, char *t, char *b)   { (void)sz; stb_sort_impl(a, n, 4, c, t, b); }
static void stb_sort_8(char *a, size_t n, size_t sz, AlgorithmCompareFunc c, char *t, char *b)   { (void)sz; stb_sort_impl(a, n, 8, c, t, b); }
static void stb_sort_16(char *a, size_t n, size_t sz, AlgorithmCompareFunc c, char *t, char *b)  { (void)sz; stb_sort_impl(a, n, 16, c, t, b); }
static void stb_sort_any(char *a, size_t n, size_t sz, AlgorithmCompareFunc c, char *t, char *b) { stb_sort_impl(a, n, sz, c, t, b); }

/* Stable merge of a[0, n1) and b[0, n2) into out (no overlap). */
ALG_INLINE void alg_merge_impl(const char *a, size_t n1, const char *b, size_t n2, size_t sz, char *out, AlgorithmCompareFunc comp) {
    stb_merge(a, a + n1 * sz, b, b + n2 * sz, out, sz, comp);
}

static void alg_merge_4(const char *a, size_t n1, const char *b, size_t n2, size_t sz, char *o, AlgorithmCompareFunc c)   { (void)sz; alg_merge_impl(a, n1, b, n2, 4, o, c); }
static void alg_merge_8(const char *a, size_t n1, const char *b, size_t n2, size_t sz, char *o, AlgorithmCompareFunc c)   { (void)sz; alg_merge_impl(a, n1, b, n2, 8, o, c); }
static void alg_merge_any(const char *a, size_t n1, const char *b, size_t n2, size_t sz, char *o, AlgorithmCompareFunc c) { alg_merge_impl(a, n1, b, n2, sz, o, c); }

/* First element of [first, first + n) not less than *val / greater than *val. */
static size_t alg_lower_index(const char *first, size_t n, size_t sz, const void *val, AlgorithmCompareFunc comp) {
    size_t lo = 0;
    while (n > 0) {
        size_t half = n / 2;
        if (comp(first + (lo + half) * sz, val) < 0) {
            lo += half + 1;
            n -= half + 1;
        }
        else {
            n = half;
        }
    }
    return lo;
}

static size_t alg_upper_index(const char *first, size_t n, size_t sz, const void *val, AlgorithmCompareFunc comp) {
    size_t lo = 0;
    while (n > 0) {
        size_t half = n / 2;
        if (!(comp(val, first + (lo + half) * sz) < 0)) {
            lo += half + 1;
            n -= half + 1;
        }
        else {
            n = half;
        }
    }
    return lo;
}

/* Stable in-place merge of the sorted runs [a, a + n1) and [a + n1, a + n1 + n2)
 * without a buffer (recursive rotations, as libstdc++'s
 * __merge_without_buffer): O((n1 + n2) log(n1 + n2)). */
static void alg_merge_no_buffer(char *a, size_t n1, size_t n2, size_t sz, AlgorithmCompareFunc comp) {
    while (n1 != 0 && n2 != 0) {
        if (n1 + n2 == 2) {
            if (comp(a + sz, a) < 0) {
                alg_swap_bytes(a, a + sz, sz);
            }
            return;
        }
        size_t cut1, cut2;
        if (n1 > n2) {
            cut1 = n1 / 2;
            cut2 = alg_lower_index(a + n1 * sz, n2, sz, a + cut1 * sz, comp);
        }
        else {
            cut2 = n2 / 2;
            cut1 = alg_upper_index(a, n1, sz, a + (n1 + cut2) * sz, comp);
        }
        /* [cut1, n1) and [n1, n1 + cut2) swap places */
        alg_rotate_bytes(a + cut1 * sz, (n1 - cut1) * sz, cut2 * sz);
        size_t mid = cut1 + cut2;
        /* the smaller half recursively, the larger one in this loop */
        if (cut1 + cut2 < (n1 - cut1) + (n2 - cut2)) {
            alg_merge_no_buffer(a, cut1, cut2, sz, comp);
            a += mid * sz;
            n1 -= cut1;
            n2 -= cut2;
        }
        else {
            alg_merge_no_buffer(a + mid * sz, n1 - cut1, n2 - cut2, sz, comp);
            n1 = cut1;
            n2 = cut2;
        }
    }
}

/* Stable merge of [a, a + n1) and [a + n1, a + n) using `buf` (room for
 * min(n1, n2) elements). */
static void alg_merge_with_buffer(char *a, size_t n1, size_t n, size_t sz, AlgorithmCompareFunc comp, char *buf) {
    size_t n2 = n - n1;
    if (n1 <= n2) {
        /* left run aside, merge forward into a */
        memcpy(buf, a, n1 * sz);
        const char *l = buf, *le = buf + n1 * sz;
        const char *r = a + n1 * sz, *re = a + n * sz;
        char *out = a;
        while (l != le && r != re) {
            int take_r = comp(r, l) < 0;
            const char *s = take_r ? r : l;
            alg_copy_elem(out, s, sz);
            out += sz;
            r += take_r ? sz : 0;
            l += take_r ? 0 : sz;
        }
        if (l != le) {
            memcpy(out, l, (size_t)(le - l));
        }
    }
    else {
        /* right run aside, merge backward from the end */
        memcpy(buf, a + n1 * sz, n2 * sz);
        const char *l = a + n1 * sz, *lb = a;          /* left: [lb, l) */
        const char *r = buf + n2 * sz, *rb = buf;      /* right: [rb, r) */
        char *out = a + n * sz;
        while (l != lb && r != rb) {
            int take_l = comp(r - sz, l - sz) < 0;     /* right < left: left goes last */
            const char *s = take_l ? l - sz : r - sz;
            out -= sz;
            alg_copy_elem(out, s, sz);
            l -= take_l ? sz : 0;
            r -= take_l ? 0 : sz;
        }
        if (r != rb) {
            memcpy(a, rb, (size_t)(r - rb));
        }
    }
}


/**
 * @brief Sorts an array with a stable merge sort: equal elements keep their
 *        relative order (std::stable_sort).
 *
 * Bottom-up merge sort over insertion-sorted runs, merging between the array
 * and a buffer of the same size: cache-sized blocks first, equal-length runs
 * with parity merges (both ends at once, two run pairs in lockstep). Runs
 * already in order are concatenated, reversed ones swapped as blocks, and
 * sorted or strictly descending input is recognised in one pass (O(n)).
 * Element sizes 4, 8 and 16 use specialised copies. If the buffer cannot be
 * allocated the runs are merged in place with rotations (O(n log^2 n)) - the
 * array is always sorted.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function (negative / zero / positive, like qsort).
 */
void algorithm_stable_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num < 2 || base == NULL || size == 0 || comp == NULL) {
        ALGORITHM_LOG("[algorithm_stable_sort] No sorting needed / invalid arguments.");
        return;
    }
    ALGORITHM_LOG("[algorithm_stable_sort] Starting stable sort for %zu elements.", num);

    char *a = (char *)base;
    AlgScratch stk;
    char *tmp = (size <= sizeof stk.bytes) ? (char *)stk.bytes : (char *)malloc(size);
    char *buf = (tmp && num <= ((size_t)-1) / size) ? (char *)malloc(num * size) : NULL;

    if (buf) {
        switch (size) {
            case 4:  stb_sort_4(a, num, size, comp, tmp, buf);   break;
            case 8:  stb_sort_8(a, num, size, comp, tmp, buf);   break;
            case 16: stb_sort_16(a, num, size, comp, tmp, buf);  break;
            default: stb_sort_any(a, num, size, comp, tmp, buf); break;
        }
        free(buf);
    }
    else {
        ALGORITHM_LOG("[algorithm_stable_sort] No memory for the merge buffer; merging in place.");
        size_t run = 1;
        if (tmp) {
            run = STB_RUN;
            for (size_t i = 0; i < num; i += run) {
                pdq_insertion(a + i * size, (num - i < run) ? num - i : run, size, comp, tmp, 1);
            }
        }
        for (size_t width = run; width < num; width *= 2) {
            for (size_t i = 0; i < num; ) {
                size_t mid = i + ((width < num - i) ? width : num - i);
                size_t end = mid + ((width < num - mid) ? width : num - mid);
                alg_merge_no_buffer(a + i * size, mid - i, end - mid, size, comp);
                i = end;
            }
        }
    }
    if (tmp && tmp != (char *)stk.bytes) {
        free(tmp);
    }
    ALGORITHM_LOG("[algorithm_stable_sort] Stable sort completed.");
}


/**
 * @brief Sorts an array; the sort is STABLE: elements that compare equal keep
 *        their input order, so the result is the same on every compiler and
 *        system (like Rust's sort, Python's sorted, or std::stable_sort).
 *
 * Same algorithm as algorithm_stable_sort (bottom-up merge sort with parity
 * merges, see stb_*). It needs a buffer of num * size bytes; without memory
 * it sorts in place (slower, still stable). For the fastest sort when the
 * order of equal elements does not matter, use algorithm_sort_unstable.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function (negative / zero / positive, like qsort).
 */
void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_sort] Stable sort of %zu elements.", num);
    algorithm_stable_sort(base, num, size, comp);
}


/**
 * @brief Sorts an array without keeping the order of equal elements
 *        (std::sort / Rust's sort_unstable) - the fastest sort here.
 *
 * Pattern-defeating quicksort (see the pdq_* helpers): O(n log n) worst
 * case, O(n) for sorted, reversed and all-equal input, faster than std::sort
 * on random data although the comparator is called through a pointer, and
 * far faster on inputs with few distinct values. Element sizes 4, 8 and 16
 * use specialised moves. Allocates nothing for elements up to 256 bytes.
 * The result is deterministic (the same input always gives the same output).
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function (negative / zero / positive, like qsort).
 */
void algorithm_sort_unstable(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num <= 1 || size == 0 || base == NULL || comp == NULL) {
        ALGORITHM_LOG("[algorithm_sort_unstable] No sorting needed / invalid arguments.");
        return;
    }
    ALGORITHM_LOG("[algorithm_sort_unstable] Starting pdqsort for %zu elements.", num);

    AlgScratch stk;
    char *tmp = (size <= sizeof stk.bytes) ? (char *)stk.bytes : (char *)malloc(size);
    if (!tmp) {
        ALGORITHM_LOG("[algorithm_sort_unstable] No memory for a scratch element; using heapsort.");
        pdq_heapsort((char *)base, num, size, comp, NULL);
        return;
    }
    int bad = 0;                       /* unbalanced partitions allowed: log2(num) */
    for (size_t t = num; t > 1; t >>= 1) {
        ++bad;
    }
    switch (size) {
        case 4:  pdq_loop_4((char *)base, num, size, comp, tmp, bad, 1);   break;
        case 8:  pdq_loop_8((char *)base, num, size, comp, tmp, bad, 1);   break;
        case 16: pdq_loop_16((char *)base, num, size, comp, tmp, bad, 1);  break;
        default: pdq_loop_any((char *)base, num, size, comp, tmp, bad, 1); break;
    }
    if (tmp != (char *)stk.bytes) {
        free(tmp);
    }
    ALGORITHM_LOG("[algorithm_sort_unstable] pdqsort completed.");
}


/**
 * @brief Finds the first element in a range that matches a specified value.
 *
 * This function searches for the first element in the range `[base, base + num * size)` that is equal to `val`
 * based on the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to find.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return Pointer to the first matching element, or `NULL` if no match is found.
 */
void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    const char *ptr = (const char*)base;
    ALGORITHM_LOG("[algorithm_find] Searching for the element in an array of %zu elements.", num);

    for (size_t i = 0; i < num; i++) {
        if (comp(ptr + i * size, val) == 0) {
            ALGORITHM_LOG("[algorithm_find] Element found at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_find] Element not found in the array.");
    return NULL;
}


/**
 * @brief Finds the first element in a range that satisfies a predicate.
 *
 * This function searches for the first element in the range `[base, base + num * size)` that satisfies
 * the predicate function `pred`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for the element to be found.
 * 
 * @return Pointer to the first element that satisfies the predicate, or `NULL` if no such element is found.
 */
void *algorithm_find_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char*)base;
    ALGORITHM_LOG("[algorithm_find_if] Starting search in array of %zu elements.", num);

    for (size_t i = 0; i < num; i++) {
        if (pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_find_if] Element matching predicate found at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_find_if] No element matching the predicate found.");
    return NULL;
}


/**
 * @brief Finds the first element in a range that does not satisfy a predicate.
 *
 * This function searches for the first element in the range `[base, base + num * size)` that does not satisfy
 * the predicate function `pred`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for elements to be skipped.
 * 
 * @return Pointer to the first element that does not satisfy the predicate, or `NULL` if all elements satisfy it.
 */
void *algorithm_find_if_not(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_find_if_not] Info: Searching for the first element that does not satisfy the predicate in %zu elements.", num);

    const char *ptr = (const char*)base;
    for (size_t i = 0; i < num; i++) {
        if (!pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_find_if_not] Success: Element found at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_find_if_not] Info: No element found that does not satisfy the predicate.");
    return NULL;
}


/**
 * @brief Finds the last occurrence of a subsequence in a range.
 *
 * This function searches for the last occurrence of the sequence `[base2, base2 + num2 * size2)` in the range
 * `[base1, base1 + num1 * size1)`.
 *
 * @param base1 Pointer to the start of the main array.
 * @param num1 Number of elements in the main array.
 * @param size1 Size of each element in the main array in bytes.
 * @param base2 Pointer to the start of the subsequence array.
 * @param num2 Number of elements in the subsequence array.
 * @param size2 Size of each element in the subsequence array in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return Pointer to the start of the last occurrence of the subsequence, or `NULL` if not found.
 */
void *algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_find_end] Info: Searching for the last occurrence of the subsequence in the main array.");

    if (num2 == 0) {
        ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence is empty, returning pointer to the end of the main array.");
        return (void*)((char*)base1 + num1 * size1);
    }
    if (num2 > num1 || !base1 || !base2 || !comp) {
        ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence longer than haystack.");
        return NULL;
    }

    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    /* from the last possible start backwards: the first hit is the answer */
    for (size_t i = num1 - num2 + 1; i-- > 0; ) {
        size_t j = 0;
        while (j < num2 && comp(ptr1 + (i + j) * size1, ptr2 + j * size2) == 0) {
            ++j;
        }
        if (j == num2) {
            ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence found starting at index %zu.", i);
            return (void *)(ptr1 + i * size1);
        }
    }

    ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence not found.");
    return NULL;
}


/**
 * @brief Finds the first element in a range that matches any element in another range.
 *
 * This function searches for the first element in the range `[base1, base1 + num1 * size1)` that matches
 * any element in the range `[base2, base2 + num2 * size2)`.
 *
 * @param base1 Pointer to the start of the main array.
 * @param num1 Number of elements in the main array.
 * @param size1 Size of each element in the main array in bytes.
 * @param base2 Pointer to the start of the array to search for matches.
 * @param num2 Number of elements in the array to search for matches.
 * @param size2 Size of each element in the array to search for matches in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return Pointer to the first matching element in `base1`, or `NULL` if no match is found.
 */
void *algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_find_first_of] Info: Searching for the first occurrence of any element from the second array in the main array.");

    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    for (size_t i = 0; i < num1; ++i) {
        for (size_t j = 0; j < num2; ++j) {
            if (comp(ptr1 + i * size1, ptr2 + j * size2) == 0) {
                ALGORITHM_LOG("[algorithm_find_first_of] Success: Match found at index %zu in the main array.", i);
                return (void *)(ptr1 + i * size1);
            }
        }
    }

    ALGORITHM_LOG("[algorithm_find_first_of] Info: No match found.");
    return NULL;
}


/**
 * @brief Performs a binary search to find the index of a value in a sorted range.
 *
 * This function performs a binary search on a sorted range `[base, base + num * size)` to find the index
 * of the value `val`. The range must be sorted according to the same comparison function `comp`.
 *
 * @param base Pointer to the start of the sorted array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to find.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return The index of the found element, or `(size_t)-1` if the value is not found.
 */
size_t algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_binary_search] Info: Performing binary search on a sorted array of %zu elements.", num);

    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        int result = comp(mid_elem, val);
        if (result < 0) {
            low = mid + 1;
        } 
        else if (result > 0) {
            high = mid;
        } 
        else {
            ALGORITHM_LOG("[algorithm_binary_search] Success: Element found at index %zu.", mid);
            return (size_t)mid;
        }
    }

    ALGORITHM_LOG("[algorithm_binary_search] Info: Element not found.");
    return (size_t)-1;
}


/**
 * @brief Finds the maximum element in a range.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` to find the maximum
 * element, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the maximum element, or `NULL` if the array is empty.
 */
void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        ALGORITHM_LOG("[algorithm_max_element] Error: Array is empty, returning NULL.");
        return NULL;
    }
    ALGORITHM_LOG("[algorithm_max_element] Info: Searching for the maximum element in an array of %zu elements.", num);

    const char *max_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(max_element, element) < 0) {
            max_element = element;
            ALGORITHM_LOG("[algorithm_max_element] Info: New maximum element found at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_max_element] Success: Maximum element found.");
    return (void *)max_element;
}


/**
 * @brief Finds the minimum element in a range.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` to find the minimum
 * element, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the minimum element, or `NULL` if the array is empty.
 */
void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        ALGORITHM_LOG("[algorithm_min_element] Error: Array is empty, returning NULL.");
        return NULL;
    }
    ALGORITHM_LOG("[algorithm_min_element] Info: Searching for the minimum element in an array of %zu elements.", num);

    const char *min_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(min_element, element) > 0) {
            min_element = element;
            ALGORITHM_LOG("[algorithm_min_element] Info: New minimum element found at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_min_element] Success: Minimum element found.");
    return (void *)min_element;
}


/**
 * @brief Applies a function to each element in an array.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param op Function to apply to each element.
 */
void algorithm_for_each(void *base, size_t num, size_t size, ForEachOpFunc op) {
    ALGORITHM_LOG("[algorithm_for_each] Info: Applying operation to each of %zu elements.", num);

    char *ptr = (char *)base;
    for (size_t i = 0; i < num; ++i) {
        op(ptr + i * size);
    }

    ALGORITHM_LOG("[algorithm_for_each] Success: Operation applied to all elements.");
}


/**
 * @brief Copies elements from a source array to a destination array.
 *
 * @param source Pointer to the source array.
 * @param num Number of elements to copy.
 * @param size Size of each element in bytes.
 * @param dest Pointer to the destination array.
 */
void algorithm_copy(const void *source, size_t num, size_t size, void *dest) {
    ALGORITHM_LOG("[algorithm_copy] Info: Copying %zu elements.", num);

    if (!source || !dest || num == 0 || size == 0) {
        return;
    }
    /* one block copy; memmove keeps overlapping ranges correct */
    memmove(dest, source, num * size);

    ALGORITHM_LOG("[algorithm_copy] Success: Copy completed.");
}


/**
 * @brief Accumulates a result by applying an operation sequentially to the elements in a range.
 *
 * This function applies the operation `op` to the initial value `init` and each element in the range
 * `[base, base + num * size)` to accumulate a result. The result is stored in `init`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param init Pointer to the initial value and where the result will be stored.
 * @param op Function that defines how to accumulate the values.
 * 
 * @return Pointer to the result, which is the same as the `init` parameter.
 */
void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, AccumulateOpFunc op) {
    ALGORITHM_LOG("[algorithm_accumulate] Info: Accumulating %zu elements.", num);

    char *result = (char *)init;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        op(result, ptr + i * size);
        ALGORITHM_LOG("[algorithm_accumulate] Info: Accumulated element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_accumulate] Success: Accumulation completed.");
    return result;
}


/**
 * @brief Checks if all elements in a range satisfy a predicate.
 *
 * This function returns `true` if the predicate `pred` returns `true` for all elements in the range
 * `[base, base + num * size)`. Otherwise, it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function to check each element.
 * 
 * @return `true` if all elements satisfy the predicate, `false` otherwise.
 */
bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_all_of] Info: Checking if all of %zu elements satisfy the predicate.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (!pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_all_of] Info: Element at index %zu does not satisfy the predicate.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_all_of] Success: All elements satisfy the predicate.");
    return true;
}


/**
 * @brief Checks if any element in a range satisfies a predicate.
 *
 * This function returns `true` if the predicate `pred` returns `true` for any element in the range
 * `[base, base + num * size)`. Otherwise, it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function to check each element.
 * 
 * @return `true` if any element satisfies the predicate, `false` otherwise.
 */
bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_any_of] Info: Checking if any element of %zu satisfies the predicate.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_any_of] Success: Element at index %zu satisfies the predicate.", i);
            return true;
        }
    }

    ALGORITHM_LOG("[algorithm_any_of] Info: No elements satisfy the predicate.");
    return false;
}


/**
 * @brief Checks if none of the elements in a range satisfy a predicate.
 *
 * This function returns `true` if the predicate `pred` returns `false` for all elements in the range
 * `[base, base + num * size)`. Otherwise, it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function to check each element.
 * 
 * @return `true` if none of the elements satisfy the predicate, `false` otherwise.
 */
bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_none_of] Info: Checking if none of the elements of %zu satisfy the predicate.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_none_of] Info: Element at index %zu satisfies the predicate, returning false.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_none_of] Success: None of the elements satisfy the predicate.");
    return true;
}


/**
 * @brief Fills a range with a specified value.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to fill the range with.
 */
/* count copies of *val at dst; val may point into the destination range */
static void alg_fill_range(char *dst, size_t count, size_t size, const void *val) {
    if (count == 0) {
        return;
    }
    const char *v = (const char *)val;
    if (v + size > dst && v < dst + count * size) {      /* val inside the range: copy it aside */
        AlgScratch stk;
        char *copy = (size <= sizeof stk.bytes) ? (char *)stk.bytes : (char *)malloc(size);
        if (!copy) {
            for (size_t i = 0; i < count; ++i) {         /* rare: large element, no memory */
                if (dst + i * size != v) {
                    memmove(dst + i * size, v, size);
                }
            }
            return;
        }
        memcpy(copy, v, size);
        alg_fill_bytes(dst, copy, size, count);
        if (copy != (char *)stk.bytes) {
            free(copy);
        }
        return;
    }
    alg_fill_bytes(dst, val, size, count);
}


void algorithm_fill(void *first, void *last, size_t size, const void *val) {
    ALGORITHM_LOG("[algorithm_fill] Info: Filling range with value.");

    if (!first || !last || !val || size == 0 || (char *)last <= (char *)first) {
        return;
    }
    /* whole elements only: a range that is not a multiple of size used to
       run past `last` */
    alg_fill_range((char *)first, (size_t)((char *)last - (char *)first) / size, size, val);

    ALGORITHM_LOG("[algorithm_fill] Success: Range filled with value.");
}

/**
 * @brief Fills the first n elements of a range with a specified value.
 *
 * @param first Pointer to the start of the range.
 * @param n Number of elements to fill.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to fill the range with.
 */
void algorithm_fill_n(void *first, size_t n, size_t size, const void *val) {
    ALGORITHM_LOG("[algorithm_fill_n] Info: Filling first %zu elements with value.", n);

    if (!first || !val || size == 0) {
        return;
    }
    alg_fill_range((char *)first, n, size, val);

    ALGORITHM_LOG("[algorithm_fill_n] Success: %zu elements filled with value.", n);
}


/**
 * @brief Counts the number of occurrences of a value in a range.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` and counts
 * how many times the value `val` appears, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to be counted.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return The number of times `val` appears in the range.
 */
size_t algorithm_count(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_count] Info: Counting occurrences of value in array of %zu elements.", num);

    size_t count = 0;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) == 0) {
            ++count;
            ALGORITHM_LOG("[algorithm_count] Info: Found match at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_count] Success: Total occurrences found: %zu.", count);
    return count;
}


/**
 * @brief Counts the number of elements in a range that satisfy a predicate function.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` and counts
 * how many of them satisfy the predicate function `pred`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for elements that should be counted.
 * 
 * @return The number of elements that satisfy the predicate function `pred`.
 */
size_t algorithm_count_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_count_if] Info: Counting elements that satisfy the predicate in array of %zu elements.", num);

    size_t count = 0;
    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ++count;
            ALGORITHM_LOG("[algorithm_count_if] Info: Predicate satisfied at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_count_if] Success: Total elements that satisfy the predicate: %zu.", count);
    return count;
}


/**
 * @brief Randomly shuffles elements in an array (Fisher-Yates).
 *
 * Every permutation is equally likely, and the same generator gives the same
 * permutation as earlier versions of this function (see alg_random_index).
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param rng Generator of uniformly random 32-bit values (the full range
 *            0 .. 0xFFFFFFFF). rand() delivers only 15 random bits on some
 *            systems: combine calls, e.g. (rand() << 30) ^ (rand() << 15) ^ rand().
 */
void algorithm_shuffle(void *base, size_t num, size_t size, UniformRandomNumberGenerator rng) {
    ALGORITHM_LOG("[algorithm_shuffle] Info: Shuffling array of %zu elements.", num);

    if (num < 2 || !base || size == 0 || !rng) {
        return;
    }
    char *arr = (char *)base;
    /* Fisher-Yates, index rng() % (i + 1) as before, minus its modulo bias */
    for (size_t i = num - 1; i > 0; i--) {
        size_t j = alg_random_index(rng, i + 1);
        if (j != i) {
            alg_swap_bytes(arr + i * size, arr + j * size, size);
        }
    }

    ALGORITHM_LOG("[algorithm_shuffle] Success: Shuffling completed.");
}


/**
 * @brief Finds the first position in a sorted range where a value can be inserted without violating the order.
 *
 * This function performs a binary search on a sorted range `[base, base + num * size)` to find the first
 * position where `val` can be inserted while maintaining the sorted order. The range must be sorted
 * according to the same comparison function `comp`.
 *
 * @param base Pointer to the start of the sorted array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to be inserted.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the first position where `val` can be inserted without violating the order.
 */
void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_lower_bound] Info: Performing lower bound search in array of %zu elements.", num);

    const char *b = (const char *)base;
    if (num == 0 || !base || !comp || size == 0) {
        return (void *)b;
    }
    if (num > ALG_BSEARCH_PREFETCH_BYTES / size) {
        /* large array: branchless steps, both possible next probes prefetched */
        while (num > 1) {
            size_t half = num / 2;
            ALG_PREFETCH(b + (half / 2) * size);
            ALG_PREFETCH(b + (half + half / 2) * size);
            b = (comp(b + half * size, val) < 0) ? b + half * size : b;
            num -= half;
        }
        b += (comp(b, val) < 0) ? size : 0;
        return (void *)b;
    }
    size_t low = 0;
    size_t high = num;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        if (comp(b + mid * size, val) < 0) {
            low = mid + 1;
        }
        else {
            high = mid;
        }
    }

    ALGORITHM_LOG("[algorithm_lower_bound] Success: Found lower bound at index %zu.", low);
    return (void *)(b + low * size);
}


/**
 * @brief Finds the first position in a sorted range where a value is greater than a specified value.
 *
 * This function performs a binary search on a sorted range `[base, base + num * size)` to find the first
 * position where the elements are greater than `val`. The range must be sorted according to the same comparison function `comp`.
 *
 * @param base Pointer to the start of the sorted array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to compare against.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the first position where elements are greater than `val`.
 */
void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_upper_bound] Info: Performing upper bound search in array of %zu elements.", num);

    const char *b = (const char *)base;
    if (num == 0 || !base || !comp || size == 0) {
        return (void *)b;
    }
    if (num > ALG_BSEARCH_PREFETCH_BYTES / size) {
        while (num > 1) {
            size_t half = num / 2;
            ALG_PREFETCH(b + (half / 2) * size);
            ALG_PREFETCH(b + (half + half / 2) * size);
            b = (comp(b + half * size, val) <= 0) ? b + half * size : b;
            num -= half;
        }
        b += (comp(b, val) <= 0) ? size : 0;
        return (void *)b;
    }
    size_t low = 0;
    size_t high = num;
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        if (comp(b + mid * size, val) <= 0) {
            low = mid + 1;
        }
        else {
            high = mid;
        }
    }

    ALGORITHM_LOG("[algorithm_upper_bound] Success: Found upper bound at index %zu.", low);
    return (void *)(b + low * size);
}


/**
 * @brief Transforms each element in an array and stores the result in another array.
 *
 * @param base Pointer to the start of the input array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the output array where the results will be stored.
 * @param op Transformation function to apply to each element.
 */
void algorithm_transform(const void *base, size_t num, size_t size, void *result, TransformFunc op) {
    ALGORITHM_LOG("[algorithm_transform] Info: Applying transformation to %zu elements.", num);

    const char *input_ptr = (const char *)base;
    char *output_ptr = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        ALGORITHM_LOG("[algorithm_transform] Info: Transforming element at index %zu.", i);
        op(output_ptr + i * size, input_ptr + i * size);
    }

    ALGORITHM_LOG("[algorithm_transform] Success: Transformation completed.");
}


/**
 * @brief Reduces a range of elements to a single value using a specified binary operation.
 *
 * This function applies the binary operation `op` to each element in the range `[base, base + num * size)`
 * and accumulates the result, starting with the initial value `init`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param init Pointer to the initial value, which is also used to store the accumulated result.
 * @param op Binary operation function that takes two arguments: the accumulated result and the current element.
 * 
 * @return Pointer to the accumulated result (same as `init`).
 */
void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, ReduceFunc op) {
    ALGORITHM_LOG("[algorithm_reduce] Info: Reducing array of %zu elements.", num);

    const char *ptr = (const char *)base;
    char *result = (char *)init;

    for (size_t i = 0; i < num; ++i) {
        ALGORITHM_LOG("[algorithm_reduce] Info: Reducing element at index %zu.", i);
        op(result, ptr + i * size);
    }

    ALGORITHM_LOG("[algorithm_reduce] Success: Reduction completed.");
    return result;
}


/**
 * @brief Removes consecutive duplicate elements from a sorted array.
 *
 * This function removes consecutive duplicate elements from a sorted array `base` as determined
 * by the comparison function `comp`. The array must be sorted before calling this function.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return The number of unique elements remaining in the array.
 */
size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_unique] Info: Removing duplicates from array of %zu elements.", num);

    if (num == 0) {
        ALGORITHM_LOG("[algorithm_unique] Warning: Empty array, returning 0.");
        return 0;
    }

    size_t uniqueCount = 1; 
    char *array = (char *)base;

    for (size_t i = 1; i < num; ++i) {
        if (comp(array + (uniqueCount - 1) * size, array + i * size) != 0) {
            if (uniqueCount != i) {
                alg_copy_elem(array + uniqueCount * size, array + i * size, size);
                ALGORITHM_LOG("[algorithm_unique] Info: Moving element at index %zu to position %zu.", i, uniqueCount);
            }
            uniqueCount++;
        }
    }

    ALGORITHM_LOG("[algorithm_unique] Success: Number of unique elements is %zu.", uniqueCount);
    return uniqueCount;
}


/**
 * @brief Checks if two ranges are equal.
 *
 * This function compares two ranges `[base1, base1 + num1 * size1)` and `[base2, base2 + num2 * size2)`
 * to determine if they contain the same elements in the same order, as determined by the comparison function `comp`.
 *
 * @param base1 Pointer to the start of the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array in bytes.
 * @param base2 Pointer to the start of the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array in bytes.
 * @param comp Comparison function used to compare elements.It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return `true` if the two ranges are equal, `false` otherwise.
 */
bool algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2,  CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_equal] Info: Comparing two arrays of %zu and %zu elements.", num1, num2);
    if (num1 != num2 || size1 != size2) {
        ALGORITHM_LOG("[algorithm_equal] Warning: Arrays have different sizes, returning false.");
        return false;
    }

    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;

    for (size_t i = 0; i < num1; ++i) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            ALGORITHM_LOG("[algorithm_equal] Info: Mismatch found at index %zu.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_equal] Success: Arrays are equal.");
    return true;
}


/**
 * @brief Generates the next lexicographical permutation of a range.
 *
 * This function rearranges the elements in the range `[first, last)` into the next lexicographical
 * permutation, as determined by the comparison function `comp`. If such a permutation is not possible,
 * the range is rearranged into the smallest lexicographical order (sorted in ascending order).
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to one past the end of the range.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return `true` if the first element is less than the second, `false` otherwise.
 * 
 * @return `true` if the next permutation was generated, `false` if the range was reset to the smallest permutation.
 */
bool algorithm_next_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_next_permutation] Info: Generating next permutation.");
    if (first == last) {
        ALGORITHM_LOG("[algorithm_next_permutation] Warning: Empty range, returning false.");
        return false;
    }

    char *i = (char *)last - size;
    while (i != (char *)first) {
        char *j = i;
        i -= size;

        if (comp(i, j)) {
            char *k = (char *)last - size;
            while (!comp(i, k)) {
                k -= size;
            }

            ALGORITHM_LOG("[algorithm_next_permutation] Info: Swapping elements.");
            swap(i, k, size);
            reverse(j, last, size);

            ALGORITHM_LOG("[algorithm_next_permutation] Success: Next permutation generated.");
            return true;
        }

        if (i == (char *)first) {
            ALGORITHM_LOG("[algorithm_next_permutation] Info: Resetting to smallest permutation.");
            reverse(first, last, size);
            return false;
        }
    }

    return false; 
}


/**
 * @brief Generates the previous lexicographical permutation of a range.
 *
 * This function rearranges the elements in the range `[first, last)` into the previous lexicographical
 * permutation, as determined by the comparison function `comp`. If such a permutation is not possible,
 * the range is rearranged into the largest lexicographical order (sorted in descending order).
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to one past the end of the range.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return `true` if the first element is less than the second, `false` otherwise.
 * 
 * @return `true` if the previous permutation was generated, `false` if the range was reset to the largest permutation.
 */
bool algorithm_prev_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_prev_permutation] Info: Generating previous permutation.");
    if (first == last) {
        ALGORITHM_LOG("[algorithm_prev_permutation] Warning: Empty range, returning false.");
        return false;
    }

    char *i = (char *)last - size;
    while (i != (char *)first) {
        char *j = i;
        i -= size;

        if (comp(j, i)) {
            char *k = (char *)last - size;
            
            while (!comp(k, i)) {
                k -= size;
            }

            ALGORITHM_LOG("[algorithm_prev_permutation] Info: Swapping elements.");
            swap(i, k, size);
            reverse(j, last, size);

            ALGORITHM_LOG("[algorithm_prev_permutation] Success: Previous permutation generated.");
            return true;
        }
        if (i == (char *)first) {
            ALGORITHM_LOG("[algorithm_prev_permutation] Info: Resetting to largest permutation.");
            
            reverse(first, last, size);
            return false;
        }
    }

    return false; 
}


/**
 * @brief Partitions a range of elements based on a predicate function.
 *
 * This function rearranges the elements in the range `[base, base + num * size)` so that all elements
 * for which the predicate function `pred` returns `true` appear before those for which it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for elements that should appear before others.
 * 
 * @return Pointer to the first element in the second group (where `pred` returns `false`).
 */
void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_partition] Info: Partitioning array of %zu elements.", num);

    if (!base || !pred || num == 0 || size == 0) {
        return base;
    }

    char *first = (char *)base;
    char *last  = first + num * size;

    while (1) {
        /* Advance `first` over pred-true elements. */
        while (first != last && pred(first)) {
            first += size;
        }
        if (first == last) {
            break;
        }

        do {
            last -= size;
            if (first == last) {
                ALGORITHM_LOG("[algorithm_partition] Success: Met in the middle.");
                return first;
            }
        } while (!pred(last));

        ALGORITHM_LOG("[algorithm_partition] Info: Swapping elements.");
        swap(first, last, size);
        first += size;
    }

    ALGORITHM_LOG("[algorithm_partition] Success: Partitioning completed.");
    return first;
}


/**
 * @brief Generates values for a range using a generator function.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param gen Generator function that produces values.
 */
void algorithm_generate(void *first, void *last, size_t size, GeneratorFunc gen) {
    ALGORITHM_LOG("[algorithm_generate] Info: Generating values for range.");

    char *current = (char *)first;
    while (current != last) {
        gen(current);
        current += size;
        ALGORITHM_LOG("[algorithm_generate] Info: Value generated at position.");
    }

    ALGORITHM_LOG("[algorithm_generate] Success: Generation completed.");
}


/**
 * @brief Generates values for the first n elements of a range using a generator function.
 *
 * @param first Pointer to the start of the range.
 * @param n Number of elements to generate.
 * @param size Size of each element in bytes.
 * @param gen Generator function that produces values.
 */
void algorithm_generate_n(void *first, size_t n, size_t size, GeneratorFunc gen) {
    ALGORITHM_LOG("[algorithm_generate_n] Info: Generating values for %zu elements.", n);

    char *current = (char *)first;
    for (size_t i = 0; i < n; ++i) {
        gen(current);
        current += size;
        ALGORITHM_LOG("[algorithm_generate_n] Info: Value generated for element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_generate_n] Success: Generation completed.");
}


/**
 * @brief Copies a range of elements in reverse order.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array where elements will be copied.
 */
void algorithm_copy_backward(const void *first, const void *last, size_t size, void *result) {
    ALGORITHM_LOG("[algorithm_copy_backward] Info: Copying elements in reverse order.");

    if (!first || !last || !result || size == 0 || (const char *)last <= (const char *)first) {
        return;
    }
    size_t bytes = (size_t)((const char *)last - (const char *)first) / size * size;
    /* the destination ENDS at result; memmove handles the overlap that
       copy_backward exists for (shifting a range to the right) */
    memmove((char *)result - bytes, (const char *)last - bytes, bytes);

    ALGORITHM_LOG("[algorithm_copy_backward] Success: Copying completed.");
}


/**
 * @brief Copies elements from a source array to a destination array if they satisfy a predicate function.
 *
 * @param first Pointer to the start of the source array.
 * @param last Pointer to the end of the source array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 * @param pred Predicate function that determines if an element should be copied.
 */
void algorithm_copy_if(const void *first, const void *last, size_t size, void *result, UnaryPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_copy_if] Info: Copying elements based on predicate.");

    if (!first || !last || !result || !pred || size == 0) {
        return;
    }
    if ((const char *)last <= (const char *)first) {
        return;
    }
    /* the filter core: predicate answers collected 64 at a time, then only
       the kept elements copied - no branch mispredictions on random data */
    size_t n = (size_t)((const char *)last - (const char *)first) / size;
    alg_filter_dispatch((const char *)first, n, size, (char *)result, pred, NULL, NULL, 0);

    ALGORITHM_LOG("[algorithm_copy_if] Success: Copying completed.");
}


/**
 * @brief Copies the first n elements from a source array to a destination array.
 *
 * @param first Pointer to the start of the source array.
 * @param n Number of elements to copy.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 */
void algorithm_copy_n(const void *first, size_t n, size_t size, void *result) {
    ALGORITHM_LOG("[algorithm_copy_n] Info: Copying %zu elements of size %zu.", n, size);

    if (!first || !result || n == 0 || size == 0) {
        return;
    }
    memmove(result, first, n * size);

    ALGORITHM_LOG("[algorithm_copy_n] Success: Copy completed.");
}


/**
 * @brief Finds the range of elements that are equal to a specified value in a sorted range.
 *
 * This function returns a `Pair` representing the range of elements in the sorted array `base`
 * that are equal to the value `val`, as determined by the comparison function `comp`. The
 * `first` element in the `Pair` is the first element in the range, and the `second` element
 * is one past the last element in the range.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to search for.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return A `Pair` representing the range of elements equal to `val`.
 *         `first` points to the first element in the range, and `second` points to one past the last element.
 */
Pair algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_equal_range] Info: Finding equal range for value in array with %zu elements.", num);

    Pair range;
    range.first = algorithm_lower_bound(base, num, size, val, comp);
    /* the upper bound lies at or after the lower one */
    size_t skipped = (base && size) ? (size_t)((char *)range.first - (const char *)base) / size : 0;
    range.second = algorithm_upper_bound(range.first, num - skipped, size, val, comp);

    ALGORITHM_LOG("[algorithm_equal_range] Success: Equal range found.");
    return range;
}


/**
 * @brief Determines if a sorted range includes all the elements of another sorted range.
 *
 * This function checks if all elements of the sorted range `[first2, first2 + num2)` are contained
 * within the sorted range `[first1, first1 + num1)` as determined by the comparison function `comp`.
 *
 * @param first1 Pointer to the start of the first sorted array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array.
 * @param first2 Pointer to the start of the second sorted array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return `true` if all elements of the second range are contained within the first range, `false` otherwise.
 */
bool algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, 
                        CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_includes] Info: Checking if array1 includes array2 with %zu and %zu elements respectively.", num1, num2);

    const char *ptr1 = (const char *)first1;
    const char *ptr2 = (const char *)first2;
    const char *last1 = ptr1 + num1 * size1;
    const char *last2 = ptr2 + num2 * size2;

    while (ptr2 != last2) {
        if ((ptr1 == last1) || comp(ptr2, ptr1) < 0) {
            ALGORITHM_LOG("[algorithm_includes] Info: Array1 does not include Array2.");
            return false;
        }
        if (!comp(ptr1, ptr2)) {
            ptr2 += size2;
        }
        ptr1 += size1;
    }

    ALGORITHM_LOG("[algorithm_includes] Success: Array1 includes Array2.");
    return true;
}


/**
 * @brief Copies unique elements from a sorted range to a new range.
 *
 * This function copies unique elements from the sorted array `first` to the array `result`,
 * as determined by the comparison function `comp`. Elements that are equal (according to `comp`)
 * are only copied once.
 *
 * @param first Pointer to the start of the sorted array.
 * @param num Number of elements in the source array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the start of the destination array.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return The number of unique elements copied to the destination array.
 */
size_t algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_unique_copy] Info: Copying unique elements from an array with %zu elements.", num);

    if (num == 0) {
        ALGORITHM_LOG("[algorithm_unique_copy] Info: No elements to copy.");
        return 0;
    }

    const char *src = (const char *)first;
    char *dst = (char *)result;
    memcpy(dst, src, size); 
    size_t count = 1;

    for (size_t i = 1; i < num; ++i) {
        if (comp(dst + (count - 1) * size, src + i * size) != 0) {
            memcpy(dst + count * size, src + i * size, size);
            ++count;
            ALGORITHM_LOG("[algorithm_unique_copy] Info: Unique element %zu copied.", count);
        }
    }

    ALGORITHM_LOG("[algorithm_unique_copy] Success: Copied %zu unique elements.", count);
    return count;
}


/**
 * @brief Swaps the values of two elements.
 *
 * @param a Pointer to the first element.
 * @param b Pointer to the second element.
 * @param size Size of each element in bytes.
 */
void algorithm_swap(void *a, void *b, size_t size) {
    ALGORITHM_LOG("[algorithm_swap] Info: Swapping two elements of size %zu.", size);

    if (!a || !b || a == b || size == 0) {
        return;
    }
    /* any size, no allocation: 64-byte register steps */
    alg_swap_bytes(a, b, size);
}


/**
 * @brief Swaps ranges of elements between two arrays.
 *
 * @param first1 Pointer to the start of the first array.
 * @param first2 Pointer to the start of the second array.
 * @param num Number of elements to swap.
 * @param size Size of each element in bytes.
 */
void algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size) {
    ALGORITHM_LOG("[algorithm_swap_ranges] Info: Swapping %zu elements between two arrays of element size %zu.", num, size);

    if (!first1 || !first2 || first1 == first2 || num == 0 || size == 0) {
        return;
    }
    /* the two ranges are contiguous: swap them as one block of bytes */
    alg_swap_bytes(first1, first2, num * size);

    ALGORITHM_LOG("[algorithm_swap_ranges] Success: Swap ranges completed.");
}


/**
 * @brief Checks if the elements in a range are sorted in non-decreasing order.
 *
 * This function determines whether the elements in the array pointed to by `base` are sorted
 * in non-decreasing order, as defined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements. It should return a negative value
 *             if the first element is less than the second, zero if they are equal, and a positive
 *             value if the first element is greater than the second.
 * 
 * @return `true` if the elements are sorted in non-decreasing order, `false` otherwise.
 */
bool algorithm_is_sorted(const void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_is_sorted] Info: Checking if array of size %zu with element size %zu is sorted.", num, size);

    if (num < 2) {
        ALGORITHM_LOG("[algorithm_is_sorted] Success: Array with less than two elements is always sorted.");
        return true; 
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            ALGORITHM_LOG("[algorithm_is_sorted] Info: Array is not sorted at element %zu.", i);
            return false; 
        }
    }

    ALGORITHM_LOG("[algorithm_is_sorted] Success: Array is sorted.");
    return true; 
}


/**
 * @brief Finds the first position in a range where the elements are no longer sorted.
 *
 * This function searches through the array pointed to by `base` and finds the first element
 * where the sequence is no longer sorted in non-decreasing order, as defined by the comparison
 * function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements. It should return a negative value
 *             if the first element is less than the second, zero if they are equal, and a positive
 *             value if the first element is greater than the second.
 * 
 * @return Pointer to the first element where the sequence is no longer sorted.
 *         If the entire range is sorted, returns a pointer to the end of the array (i.e., `base + num * size`).
 */
void *algorithm_is_sorted_until(const void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_is_sorted_until] Info: Checking if the array is sorted until a specific point in an array of size %zu.", num);
    if (num < 2) {
        ALGORITHM_LOG("[algorithm_is_sorted_until] Success: Entire array is sorted (size < 2).");
        return (void *)((char *)base + num * size);
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            ALGORITHM_LOG("[algorithm_is_sorted_until] Info: Unsorted element found at index %zu.", i + 1);
            return (void *)(ptr + (i + 1) * size);
        }
    }

    ALGORITHM_LOG("[algorithm_is_sorted_until] Success: Entire array is sorted.");
    return (void *)((char *)base + num * size);
}


/**
 * @brief Rotates elements in a range.
 *
 * @param first Pointer to the start of the range.
 * @param middle Pointer to the middle of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 */
void algorithm_rotate(void *first, void *middle, void *last, size_t size) {
    if (!first || !middle || !last || size == 0) {
        ALGORITHM_LOG("[algorithm_rotate] Error: Invalid arguments.");
        return;
    }
    if (first == middle || middle == last) {
        return;
    }
    ALGORITHM_LOG("[algorithm_rotate] Info: Rotating array elements in range [first, last) with middle at position %zu.", ((char *)middle - (char *)first) / size);

    /* rotating the elements is rotating their bytes: smaller side through a
       buffer + one memmove, or block swaps without memory */
    alg_rotate_bytes((char *)first, (size_t)((char *)middle - (char *)first),
                     (size_t)((char *)last - (char *)middle));

    ALGORITHM_LOG("[algorithm_rotate] Success: Array rotation completed.");
}


/**
 * @brief Rotates elements in a range and copies them to a destination array.
 *
 * @param first Pointer to the start of the range.
 * @param middle Pointer to the middle of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 */
void algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result) {
    ALGORITHM_LOG("[algorithm_rotate_copy] Info: Copying rotated range [first, last).");

    if (!first || !middle || !last || !result || size == 0) {
        return;
    }
    size_t left  = (size_t)((const char *)middle - (const char *)first);
    size_t right = (size_t)((const char *)last - (const char *)middle);
    memcpy(result, middle, right);
    memcpy((char *)result + right, first, left);

    ALGORITHM_LOG("[algorithm_rotate_copy] Success: Rotated copy completed.");
}


/**
 * @brief Merges two sorted arrays into a single sorted array.
 *
 * @param base1 Pointer to the first sorted array.
 * @param num1 Number of elements in the first array.
 * @param base2 Pointer to the second sorted array.
 * @param num2 Number of elements in the second array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the array where the merged result will be stored.
 * @param comp Comparison function used to order the elements.
 */
void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_merge] Info: Merging two sorted arrays of sizes %zu and %zu.", num1, num2);

    if (!result || size == 0 || !comp || (num1 && !base1) || (num2 && !base2)) {
        return;
    }
    /* stable (ties take the first range), branchless element choice, the
       rest of whichever range is left in one copy */
    switch (size) {
        case 4:  alg_merge_4((const char *)base1, num1, (const char *)base2, num2, size, (char *)result, comp);   break;
        case 8:  alg_merge_8((const char *)base1, num1, (const char *)base2, num2, size, (char *)result, comp);   break;
        default: alg_merge_any((const char *)base1, num1, (const char *)base2, num2, size, (char *)result, comp); break;
    }

    ALGORITHM_LOG("[algorithm_merge] Success: Merging completed.");
}


/**
 * @brief Merges two sorted halves of an array in place.
 *
 * @param base Pointer to the start of the array.
 * @param middle Index of the middle element.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to order the elements.
 */
void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_inplace_merge] Info: Merging array segments with middle at %zu and total number of elements %zu.", middle, num);

    if (!base || !comp || size == 0 || middle == 0 || middle >= num) {
        return;
    }
    char *arr = (char *)base;
    if (!(comp(arr + middle * size, arr + (middle - 1) * size) < 0)) {
        return;                                /* already in order */
    }
    /* O(n) with a buffer for the smaller run (std::inplace_merge); without
       memory O(n log n) by rotations. Stable either way. (The old version
       shifted the tail for every element: O(n^2).) */
    size_t small = (middle < num - middle) ? middle : num - middle;
    char *buf = (small <= ((size_t)-1) / size) ? (char *)malloc(small * size) : NULL;
    if (buf) {
        alg_merge_with_buffer(arr, middle, num, size, comp, buf);
        free(buf);
    }
    else {
        alg_merge_no_buffer(arr, middle, num - middle, size, comp);
    }
    ALGORITHM_LOG("[algorithm_inplace_merge] Success: In-place merge completed.");
}


/**
 * @brief Finds the first occurrence of two consecutive elements that are equal.
 *
 * This function searches through the array pointed to by `base` and finds the first pair of consecutive 
 * elements that are equal, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * 
 * @return Pointer to the first element of the first pair of consecutive equal elements found.
 * If no such pair is found, returns `NULL`.
 */
void *algorithm_adjacent_find(const void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_adjacent_find] Info: Searching for adjacent equal elements in an array of size %zu.", num);

    //`num - 1` underflows to SIZE_MAX when num is 0, walking off the array.
    if (num < 2) {
        ALGORITHM_LOG("[algorithm_adjacent_find] Info: Fewer than 2 elements; nothing to compare.");
        return NULL;
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) == 0) {
            ALGORITHM_LOG("[algorithm_adjacent_find] Success: Found adjacent equal elements at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_adjacent_find] Info: No adjacent equal elements found.");
    return NULL;
}


/**
 * @brief Finds the first position where two ranges differ.
 *
 * This function compares two arrays element by element and returns the first position where the elements differ.
 * The comparison is done using the comparison function `comp`.
 *
 * @param base1 Pointer to the start of the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array in bytes.
 * @param base2 Pointer to the start of the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array in bytes.
 * @param comp Equality predicate — returns `true` when the two elements are
 *             equal, `false` when they differ. (Same convention as
 *             algorithm_search and algorithm_search_n.)
 *
 * @return A Pair structure:
 *      - `first`: Pointer to the first differing element in the first array.
 *      - `second`: Pointer to the first differing element in the second array.
 *      When one range is a prefix of the other, the shorter side is `NULL` (it
 *      has no element there) and the longer side points at its first extra
 *      element. Both are `NULL` only when the ranges are equal: same length
 *      and every pair matches. (Ranges of different lengths used to be
 *      reported as equal.)
 */
Pair algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_mismatch] Info: Searching for mismatch between two arrays of size %zu and %zu.", num1, num2);

    if (!base1 || !base2 || !comp) {
        Pair empty = {NULL, NULL};
        return empty;
    }

    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;
    size_t min_num = num1 < num2 ? num1 : num2;

    for (size_t i = 0; i < min_num; i++) {
        if (!comp(ptr1 + i * size1, ptr2 + i * size2)) {
            ALGORITHM_LOG("[algorithm_mismatch] Success: Mismatch found at index %zu.", i);
            Pair mismatch = {(void *)(ptr1 + i * size1), (void *)(ptr2 + i * size2)};

            return mismatch;
        }
    }

    ALGORITHM_LOG("[algorithm_mismatch] Info: No mismatch found within the compared range.");
    /* the longer range differs at index min_num, where the shorter one ended */
    Pair mismatch = { (min_num < num1) ? (void *)(ptr1 + min_num * size1) : NULL,
                      (min_num < num2) ? (void *)(ptr2 + min_num * size2) : NULL };

    return mismatch;
}


/**
 * @brief Checks if two arrays are permutations of each other.
 *
 * This function checks whether the elements of the two arrays `base1` and `base2` are permutations of each other. 
 * It does so by ensuring that each element in `base1` has a corresponding element in `base2` and vice versa, 
 * with all elements being matched exactly once.
 *
 * @param base1 Pointer to the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array in bytes.
 * @param base2 Pointer to the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array in bytes.
 * @param comp Comparison function used to compare elements between the arrays.
 * 
 * @return true if the arrays are permutations of each other; false otherwise.
 */
bool algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_is_permutation] Info: Checking if two arrays of size %zu and %zu are permutations of each other.", num1, num2);
    if (num1 != num2 || size1 != size2) {
        ALGORITHM_LOG("[algorithm_is_permutation] Info: Arrays have different sizes. They cannot be permutations.");
        return false;
    }

    /* the common prefix needs no matching (std::is_permutation does the same) */
    while (num1 > 0 && comp(base1, base2) == 0) {
        base1 = (const char *)base1 + size1;
        base2 = (const char *)base2 + size2;
        --num1;
        --num2;
    }
    if (num1 == 0) {
        ALGORITHM_LOG("[algorithm_is_permutation] Info: Both arrays empty -> trivially permutations.");
        return true;
    }

    /* Heap-allocated match flags. Portable (no VLA, so it builds on MSVC)*/
    bool *found1 = (bool *)calloc(num1, sizeof(bool));
    bool *found2 = (bool *)calloc(num2, sizeof(bool));

    if (!found1 || !found2) {
        free(found1);
        free(found2);
        ALGORITHM_LOG("[algorithm_is_permutation] Error: match-flag allocation failed.");
        return false;
    }

    for (size_t i = 0; i < num1; ++i) {
        bool matched = false;
        for (size_t j = 0; j < num2; ++j) {
            if (!found1[i] && !found2[j] && comp((char *)base1 + i * size1, (char *)base2 + j * size2) == 0) {
                found1[i] = true;
                found2[j] = true;
                matched = true;
                break;
            }
        }
        if (!matched) {
            ALGORITHM_LOG("[algorithm_is_permutation] Info: Element at index %zu in the first array does not have a match in the second array.", i);
            free(found1);
            free(found2);
            return false;
        }
    }

    free(found1);
    free(found2);
    ALGORITHM_LOG("[algorithm_is_permutation] Success: The arrays are permutations of each other.");
    return true;
}


/**
 * @brief Searches for the first occurrence of a sequence within another sequence.
 *
 * This function searches for the first occurrence of the sequence starting at `first2` and ending at `last2` 
 * within the sequence starting at `first1` and ending at `last1`.
 *
 * @param first1 Pointer to the start of the first sequence.
 * @param last1 Pointer to the end of the first sequence.
 * @param size1 Size of each element in the first sequence in bytes.
 * @param first2 Pointer to the start of the second sequence.
 * @param last2 Pointer to the end of the second sequence.
 * @param size2 Size of each element in the second sequence in bytes.
 * @param comp Comparison function used to compare elements between the sequences.
 * 
 * @return Pointer to the first element of the first occurrence of `first2` within `first1`. If not found, returns `last1`.
 */
const void* algorithm_search(const void* first1, const void* last1, size_t size1, const void* first2, const void* last2, size_t size2, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_search] Info: Starting search between two ranges.");
    
    const char* ptr1 = (const char*)first1;
    const char* ptr2 = (const char*)first2;
    const char* end1 = (const char*)last1;

    if (ptr2 == last2) {
        ALGORITHM_LOG("[algorithm_search] Info: Second range is empty, returning first.");
        return first1; 
    }

    while (ptr1 != end1) {
        const char* it1 = ptr1;
        const char* it2 = ptr2;

        while (comp(it1, it2)) { 
            it1 += size1;
            it2 += size2;

            if (it2 == last2) {
                ALGORITHM_LOG("[algorithm_search] Success: Found a match starting at element %zu.", (ptr1 - (const char*)first1) / size1);
                return ptr1; 
            }
            if (it1 == end1) {
                ALGORITHM_LOG("[algorithm_search] Info: Reached the end of the first sequence without a full match.");
                return last1; 
            }
        }
        ptr1 += size1;
    }

    ALGORITHM_LOG("[algorithm_search] Info: No match found, returning last.");
    return last1;
}


/**
 * @brief Searches for a sequence of identical elements within a range.
 *
 * This function searches for the first occurrence of a sequence of `count` identical elements 
 * (each element matching `val`) within the range `[first, last)`.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param count Number of consecutive elements to search for.
 * @param val Pointer to the value that elements should match.
 * @param comp Comparison function used to compare elements with the value.
 * 
 * @return Pointer to the first element of the found sequence. If not found, returns `last`.
 */
const void *algorithm_search_n(const void *first, const void* last, size_t size, size_t count, const void *val, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_search_n] Info: Searching for %zu consecutive elements in a sequence.", count);

    if (count == 0) {
        return first;
    }
    if (!first || !last || !val || !comp || size == 0 || (const char *)last <= (const char *)first) {
        return last;
    }
    const char *base = (const char *)first;
    size_t n = (size_t)((const char *)last - base) / size;

    /* libstdc++'s random-access search_n: test each candidate window from its
       END backwards; a mismatch at k rules out every window that contains k,
       and the matches already seen after k are reused - O(n) comparisons at
       most, usually far fewer (the old loop restarted at every position:
       O(n * count)). */
    size_t end = 0;                 /* one past the end of the next window */
    size_t remainder = count;       /* elements of that window still unchecked */
    while (remainder <= n - end) {
        end += remainder;
        size_t back = end;
        for (;;) {
            --back;
            if (!comp(base + back * size, val)) {
                break;
            }
            if (--remainder == 0) {
                ALGORITHM_LOG("[algorithm_search_n] Success: Found %zu consecutive elements.", count);
                return base + (end - count) * size;
            }
        }
        remainder = count + 1 - (end - back);
    }

    ALGORITHM_LOG("[algorithm_search_n] Info: Did not find %zu consecutive elements.", count);
    return last;
}


/**
 * @brief Removes elements that match a specified value from an array.
 *
 * This function removes all elements from the array `base` that match the value pointed to by `val`, 
 * as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to remove from the array.
 * @param comp Comparison function used to compare elements with the value.
 * @return Pointer to the new end of the array, after removing the specified elements.
 */
void *algorithm_remove(void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_remove] Info: Removing elements matching a value from %zu elements.", num);

    char *ptr = (char *)base;
    if (!base || !val || !comp || size == 0) {
        return base;
    }
    const char *key = (const char *)val;
    size_t new_num = 0;
    int inside = key >= ptr && key < ptr + num * size;

    if (inside && (size_t)(key - ptr) % size == 0) {
        /* val is one of the elements, which the compaction would overwrite:
           move with swaps instead of copies, so its bytes stay among the
           removed elements, and follow where they go (no allocation) */
        size_t key_at = (size_t)(key - ptr) / size;
        for (size_t i = 0; i < num; ++i) {
            if (comp(ptr + i * size, ptr + key_at * size) != 0) {
                if (i != new_num) {
                    swap(ptr + new_num * size, ptr + i * size, size);
                    if (key_at == new_num) {
                        key_at = i;
                    }
                }
                ++new_num;
            }
        }
    }
    else {
        AlgScratch stk;
        if (inside && size <= sizeof stk.bytes) {     /* points into an element: compare with a copy */
            memcpy(stk.bytes, key, size);
            key = (const char *)stk.bytes;
        }
        for (size_t i = 0; i < num; ++i) {
            if (comp(ptr + i * size, key) != 0) {
                if (i != new_num) {
                    alg_copy_elem(ptr + new_num * size, ptr + i * size, size);
                }
                ++new_num;
            }
        }
    }

    ALGORITHM_LOG("[algorithm_remove] Success: Removed elements, new size is %zu.", new_num);
    return ptr + new_num * size;
}


/**
 * @brief Copies elements from a source array to a destination array, excluding elements that match a given value.
 *
 * @param source Pointer to the source array.
 * @param num Number of elements in the source array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 * @param val Pointer to the value to exclude.
 * @param comp Comparison function used to compare elements with the value.
 */
void algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_remove_copy] Info: Starting with %zu elements.", num);

    if (!source || !result || !val || !comp || size == 0) {
        return;
    }
    const char *src = (const char *)source;
    char *dst = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        if (comp(src + i * size, val) != 0) {
            alg_copy_elem(dst, src + i * size, size);
            dst += size;
        }
    }

    ALGORITHM_LOG("[algorithm_remove_copy] Success: Copy completed.");
}


/**
 * Removes elements from the source array based on a given predicate function and copies the remaining elements to the result array.
 *
 * @param source The source array.
 * @param num The number of elements in the source array.
 * @param size The size of each element in bytes.
 * @param result The result array where the remaining elements will be copied.
 * @param pred The predicate function that determines whether an element should be removed or not.
 * @return The number of elements copied to the result array.
 */
size_t algorithm_remove_copy_if(const void *source, size_t num, size_t size, void *result, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_remove_copy_if] Info: Starting with %zu elements.", num);

    if (!source || !result || !pred || size == 0 || num == 0) {
        ALGORITHM_LOG("[algorithm_remove_copy_if] Error: Invalid input parameters.");
        return 0;
    }

    size_t count = alg_filter_dispatch((const char *)source, num, size, (char *)result, pred, NULL, NULL, 1);

    ALGORITHM_LOG("[algorithm_remove_copy_if] Success: Copied %zu elements.", count);
    return count;
}


/**
 * Replaces occurrences of a value in an array.
 *
 * This function replaces occurrences of the value `old_val` with the value `new_val` in the array `base`.
 * The number of elements in the array is specified by `num`, and the size of each element is specified by `size`.
 * The comparison function `comp` is used to compare elements in the array.
 *
 * @param base     Pointer to the start of the array.
 * @param num      Number of elements in the array.
 * @param size     Size of each element in bytes.
 * @param old_val  Pointer to the value to be replaced.
 * @param new_val  Pointer to the new value.
 * @param comp     Comparison function used to compare elements.
 */
void algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_replace] Info: Starting with %zu elements.", num);

    if (!base || !old_val || !new_val || !comp || size == 0 || num == 0) {
        ALGORITHM_LOG("[algorithm_replace] Error: Invalid input parameters.");
        return;
    }

    char *ptr = (char *)base;
    const char *ov = (const char *)old_val;
    const char *nv = (const char *)new_val;
    /* old_val / new_val may be elements of the array. An element equal to
       old_val is replaced by new_val; the element old_val points at is
       replaced LAST (so every comparison still sees the old value), and an
       element is never copied onto itself - no allocation needed. */
    size_t old_at = (size_t)-1;
    AlgScratch stk;
    if (ov >= ptr && ov < ptr + num * size) {
        if ((size_t)(ov - ptr) % size == 0) {
            old_at = (size_t)(ov - ptr) / size;
        }
        else if (size <= sizeof stk.bytes) {
            memcpy(stk.bytes, ov, size);
            ov = (const char *)stk.bytes;
        }
    }
    for (size_t i = 0; i < num; ++i) {
        char *e = ptr + i * size;
        if (i != old_at && e != nv && comp(e, ov) == 0) {
            alg_copy_elem(e, nv, size);
        }
    }
    if (old_at != (size_t)-1 && ptr + old_at * size != nv && comp(ptr + old_at * size, ov) == 0) {
        alg_copy_elem(ptr + old_at * size, nv, size);
    }

    ALGORITHM_LOG("[algorithm_replace] Success: Replacement completed.");
}


/**
 * Replaces elements in an array based on a given predicate function.
 *
 * This function iterates over the elements in the array pointed to by `base` and replaces
 * elements that satisfy the predicate function `pred` with the value pointed to by `new_val`.
 *
 * @param base      Pointer to the start of the array.
 * @param num       Number of elements in the array.
 * @param size      Size of each element in bytes.
 * @param new_val   Pointer to the value to replace the elements with.
 * @param pred      Predicate function that determines if an element should be replaced.
 *
 * @return None.
 */
void algorithm_replace_if(void *base, size_t num, size_t size, const void *new_val, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_replace_if] Info: Starting replace_if with %zu elements.", num);

    if (!base || !new_val || !pred || size == 0 || num == 0) {
        ALGORITHM_LOG("[algorithm_replace_if] Error: Invalid input parameters.");
        return;
    }

    char *ptr = (char *)base;
    for (size_t i = 0; i < num; ++i) {
        char *e = ptr + i * size;
        /* new_val may be an element of the array: never copy it onto itself */
        if (e != (const char *)new_val && pred(e)) {
            alg_copy_elem(e, new_val, size);
        }
    }

    ALGORITHM_LOG("[algorithm_replace_if] Success: Replacement completed.");
}


/**
 * @brief Returns a pointer to the first element of an array.
 *
 * @param base Pointer to the start of the array.
 * @return Pointer to the first element of the array.
 */
void* algorithm_begin(void* base) {
    ALGORITHM_LOG("[algorithm_begin] Info: Returning pointer to the first element.");
    return base;
}


/**
 * @brief Returns a pointer to one past the last element of an array.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @return Pointer to one past the last element of the array.
 */
void* algorithm_end(void* base, size_t num, size_t size) {
    ALGORITHM_LOG("[algorithm_end] Info: Returning pointer to one past the last element.");
    return (char*)base + num * size;
}


/**
 * @brief Fill a range with `start, ++start, ++start, ...` for any type.
 *
 * Genuinely generic: it doesn't care what `T` is. The caller supplies the
 * element size and an `increment` function that does the type-specific
 * `++` in place on a working value. For built-in numeric types this
 * library ships ready-made incrementers (`algorithm_inc_int`,
 * `algorithm_inc_double`, …) so the common case stays a one-liner; for a
 * user struct, write a three-line incrementer.
 *
 * Semantics (matches C++'s `std::iota`):
 *   - The FIRST element gets `*start`.
 *   - For each subsequent slot the working value is bumped by `increment`
 *     and copied in. `start` itself is never mutated — it's read once.
 *
 * Behavior on edge cases:
 *   - Any of `first`, `last`, `start`, `increment` NULL  →  no-op.
 *   - `size == 0`                                        →  no-op.
 *   - `first == last`                                    →  no-op.
 *   - `first > last`                                     →  no-op (the
 *     range is empty by C convention; we don't crash).
 *   - If `(last - first)` isn't a whole multiple of `size`, only the
 *     leading complete slots are filled — we never write a partial element.
 *
 * @param first     Pointer to the first element of the range.
 * @param last      Pointer one past the last element (half-open `[first, last)`).
 * @param start     Pointer to the initial value (read-only, not modified).
 * @param size      Bytes per element.
 * @param increment Function that does `++` on a value of `size` bytes.
 *
 * @code
 *   // Built-in: fill arr with 0, 1, 2, ..., 9
 *   int arr[10], start = 0;
 *   algorithm_iota(arr, arr + 10, &start, sizeof(int), algorithm_inc_int);
 *
 *   // Custom struct: walk a diagonal
 *   typedef struct { int x, y; } Point;
 *   void inc_diag(void* p) { ((Point*)p)->x++; ((Point*)p)->y++; }
 *   Point pts[5], origin = {0, 100};
 *   algorithm_iota(pts, pts + 5, &origin, sizeof(Point), inc_diag);
 *   // → (0,100), (1,101), (2,102), (3,103), (4,104)
 * @endcode
 */
void algorithm_iota(void* first, void* last, const void* start, size_t size, IncrementFunc increment) {
    if (!first || !last || !start || !increment || size == 0) {
        ALGORITHM_LOG("[algorithm_iota] NULL input or zero size; no-op");
        return;
    }

    char* p   = (char*)first;
    char* end = (char*)last;
    if (p >= end) {
        ALGORITHM_LOG("[algorithm_iota] empty or inverted range; no-op");
        return;
    }

    /* Truncate to whole elements — never write a partial slot. */
    size_t bytes = (size_t)(end - p);
    if (bytes % size != 0) {
        ALGORITHM_LOG("[algorithm_iota] range %zu B not a multiple of %zu B; truncating", bytes, size);
        end = p + (bytes / size) * size;
        if (p == end) {
            return;
        }
    }

    unsigned char stackbuf[128];
    void* work = (size <= sizeof(stackbuf)) ? (void*)stackbuf : malloc(size);
    if (!work) {
        ALGORITHM_LOG("[algorithm_iota] malloc(%zu) failed", size);
        return;
    }
    memcpy(work, start, size);

    while (p < end) {
        memcpy(p, work, size);
        p += size;
        if (p < end) {
            increment(work);
        }
    }

    if (work != (void*)stackbuf) {
        free(work);
    }
}


void algorithm_inc_int(void* c) { 
    (*(int*)c)++;                
}


void algorithm_inc_char(void* c) { 
    (*(char*)c)++;              
}


void algorithm_inc_short(void* c) { 
    (*(short*)c)++;              
}


void algorithm_inc_long(void* c) { 
    (*(long*)c)++;               
}


void algorithm_inc_long_long(void* c) { 
    (*(long long*)c)++;          
}


void algorithm_inc_uchar(void* c) { 
    (*(unsigned char*)c)++;      
}


void algorithm_inc_ushort(void* c) { 
    (*(unsigned short*)c)++;     
}


void algorithm_inc_uint(void* c) { 
    (*(unsigned int*)c)++;       
}


void algorithm_inc_ulong(void* c) { 
    (*(unsigned long*)c)++;      
}


void algorithm_inc_ulong_long(void* c) { 
    (*(unsigned long long*)c)++; 
}


void algorithm_inc_float(void* c) { 
    (*(float*)c)       += 1.0f;  
}


void algorithm_inc_double(void* c) { 
    (*(double*)c)      += 1.0;   
}


void algorithm_inc_long_double(void* c) { 
    (*(long double*)c) += 1.0L;  
}


void algorithm_inc_int8(void* c) { 
    (*(int8_t*)c)++;             
}


void algorithm_inc_int16(void* c) { 
    (*(int16_t*)c)++;            
}


void algorithm_inc_int32(void* c) { 
    (*(int32_t*)c)++;            
}


void algorithm_inc_int64(void* c) { 
    (*(int64_t*)c)++;            
}


void algorithm_inc_uint8(void* c) { 
    (*(uint8_t*)c)++;            
}


void algorithm_inc_uint16(void* c) { 
    (*(uint16_t*)c)++;           
}


void algorithm_inc_uint32(void* c) { 
    (*(uint32_t*)c)++;           
}


void algorithm_inc_uint64(void* c) { 
    (*(uint64_t*)c)++;           
}


void algorithm_inc_size_t(void* c) { 
    (*(size_t*)c)++;             
}


/**
 * @brief Keeps the elements for which `pred` returns true, in order - the
 *        counterpart of Python's `filter(function, iterable)`.
 *
 * Python's `filter` builds a new sequence; here the kept elements are copied
 * to `result`, which may be a separate buffer (the input is left untouched)
 * or `base` itself (filtering in place, like `list[:] = filter(...)`).
 *
 * With `pred == NULL` it behaves like `filter(None, iterable)`: an element is
 * kept when it is "truthy", i.e. when not all of its bytes are zero - so 0,
 * NULL pointers, '\0' and all-zero structs are dropped. (For floating point
 * that drops 0.0 but keeps -0.0, whose sign bit is set.)
 *
 * Fast: the predicate's answers are collected 64 at a time in a bit mask
 * and only the kept elements are then copied, so a random predicate causes
 * no branch mispredictions, and only the kept elements are written.
 *
 * @param base   Pointer to the input elements.
 * @param num    Number of input elements.
 * @param size   Size of each element in bytes.
 * @param result Where the kept elements go: `base` (in place), or a buffer
 *               with room for as many elements as are kept (at most `num`)
 *               that does not overlap the input.
 * @param pred   `bool pred(const void* item)`; NULL keeps truthy elements.
 *
 * @return The number of elements written to `result` (0 for invalid arguments).
 *
 * @code
 *   static bool is_even(const void* p) { return *(const int*)p % 2 == 0; }
 *   int nums[] = {1, 2, 3, 4, 5, 6};
 *   int evens[6];
 *   size_t n = algorithm_filter(nums, 6, sizeof(int), evens, is_even);   // 2 4 6, n = 3
 * @endcode
 */
size_t algorithm_filter(const void *base, size_t num, size_t size, void *result, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_filter] Info: Filtering %zu elements.", num);

    if (!base || !result || size == 0 || num == 0) {
        return 0;
    }
    return alg_filter_dispatch((const char *)base, num, size, (char *)result, pred, NULL, NULL, 0);
}


/**
 * @brief algorithm_filter with a context pointer passed to the predicate -
 *        what a Python lambda gets from the variables it captures, e.g.
 *        `filter(lambda x: x > limit, data)`.
 *
 * @param base   Pointer to the input elements.
 * @param num    Number of input elements.
 * @param size   Size of each element in bytes.
 * @param result `base` (in place) or a non-overlapping buffer for the kept elements.
 * @param pred   `bool pred(const void* item, void* ctx)`; NULL keeps truthy
 *               elements (see algorithm_filter).
 * @param ctx    Passed to `pred` unchanged.
 *
 * @return The number of elements written to `result`.
 *
 * @code
 *   static bool above(const void* p, void* ctx) { return *(const int*)p > *(const int*)ctx; }
 *   int limit = 3;
 *   size_t n = algorithm_filter_ctx(nums, 6, sizeof(int), nums, above, &limit);   // in place: 4 5 6
 * @endcode
 */
size_t algorithm_filter_ctx(const void *base, size_t num, size_t size, void *result, ContextPredicateFunc pred, void *ctx) {
    ALGORITHM_LOG("[algorithm_filter_ctx] Info: Filtering %zu elements.", num);

    if (!base || !result || size == 0 || num == 0) {
        return 0;
    }
    return alg_filter_dispatch((const char *)base, num, size, (char *)result, NULL, pred, ctx, 0);
}


/* ================================================================== */
/* Combinations (Python's itertools.combinations)                     */
/* ================================================================== */

static size_t alg_gcd(size_t a, size_t b) {
    while (b) {
        size_t t = a % b;
        a = b;
        b = t;
    }
    return a;
}


/**
 * @brief Number of r-element combinations of n elements, C(n, r) - what
 *        Python's `math.comb(n, r)` returns and how many tuples
 *        `itertools.combinations` yields.
 *
 * Computed exactly without intermediate overflow; results that do not fit
 * in a size_t are reported as SIZE_MAX.
 *
 * @return C(n, r); 0 when r > n; 1 when r == 0 or r == n.
 */
size_t algorithm_combination_count(size_t n, size_t r) {
    if (r > n) {
        return 0;
    }
    if (r > n - r) {
        r = n - r;
    }
    size_t result = 1;
    for (size_t i = 1; i <= r; ++i) {
        /* result * (n - r + i) / i is exact; divide first to stay small */
        size_t num = n - r + i;
        size_t den = i;
        size_t g = alg_gcd(result, den);
        result /= g;
        den /= g;
        num /= den;                    /* den divides num now */
        if (result > ((size_t)-1) / num) {
            return (size_t)-1;
        }
        result *= num;
    }
    return result;
}


/**
 * @brief Advances `indices` to the next r-combination of {0, ..., n-1} in
 *        lexicographic order - the index-level step behind
 *        algorithm_combination, for writing your own loop without callbacks
 *        (the combination counterpart of algorithm_next_permutation).
 *
 * Start with indices = {0, 1, ..., r-1}. Each call moves to the next
 * combination, in the same order as Python's itertools.combinations.
 *
 * @param indices r strictly increasing indices in [0, n).
 * @param n       Number of elements to choose from.
 * @param r       Number of chosen elements.
 *
 * @return true when the next combination was produced; false when `indices`
 *         was the last one (it is then reset to {0, ..., r-1}, like
 *         next_permutation's wrap-around), or for r == 0, r > n or NULL.
 *
 * @code
 *   size_t idx[2] = {0, 1};
 *   do {
 *       printf("(%c %c) ", "ABCD"[idx[0]], "ABCD"[idx[1]]);
 *   } while (algorithm_next_combination(idx, 4, 2));
 *   // (A B) (A C) (A D) (B C) (B D) (C D)
 * @endcode
 */
bool algorithm_next_combination(size_t *indices, size_t n, size_t r) {
    if (!indices || r == 0 || r > n) {
        return false;
    }
    size_t i = r;
    while (i > 0 && indices[i - 1] == n - r + (i - 1)) {
        --i;
    }
    if (i == 0) {
        for (size_t k = 0; k < r; ++k) {
            indices[k] = k;
        }
        return false;
    }
    --i;
    indices[i]++;
    for (size_t k = i + 1; k < r; ++k) {
        indices[k] = indices[k - 1] + 1;
    }
    return true;
}


/**
 * @brief Visits every r-element combination of the elements, like Python's
 *        `itertools.combinations(iterable, r)`.
 *
 * The combinations come in the same order as in Python - lexicographic by
 * position, so for sorted input the tuples come out sorted - and elements
 * are told apart by position, not value (duplicates produce repeated
 * tuples). Each combination is handed to `callback` as r consecutive
 * elements (a C array of the element type), valid during the call:
 *
 *     for c in itertools.combinations(data, r):  ->  callback(c, r, ctx)
 *
 * Returning false from the callback stops the iteration (Python's `break`).
 * Python yields one empty tuple for r == 0 and nothing for r > len(data);
 * here the callback is called once (with r = 0) and not at all, respectively.
 *
 * Fast: no allocation of all C(n, r) tuples; between consecutive
 * combinations only the changed tail is updated, and that tail is a
 * contiguous run of the input, copied with one memcpy. Indices and the
 * combination buffer are on the stack for small r.
 *
 * @param base     Pointer to the input elements.
 * @param num      Number of input elements (Python's len(iterable)).
 * @param size     Size of each element in bytes.
 * @param r        Length of each combination.
 * @param callback `bool callback(const void* combination, size_t r, void* ctx)`.
 * @param ctx      Passed to `callback` unchanged.
 *
 * @return The number of combinations passed to `callback` (C(num, r) unless it
 *         stopped early); 0 for invalid arguments or out of memory.
 *
 * @code
 *   static bool print_pair(const void* c, size_t r, void* ctx) {
 *       const char* s = c; (void)r; (void)ctx;
 *       printf("%c%c ", s[0], s[1]);
 *       return true;
 *   }
 *   algorithm_combination("ABCD", 4, 1, 2, print_pair, NULL);   // AB AC AD BC BD CD
 * @endcode
 */
size_t algorithm_combination(const void *base, size_t num, size_t size, size_t r, CombinationFunc callback, void *ctx) {
    ALGORITHM_LOG("[algorithm_combination] Info: combinations of %zu out of %zu elements.", r, num);

    if (!callback || size == 0 || r > num || (num && !base)) {
        return 0;
    }
    if (r == 0) {                       /* Python: one empty tuple */
        callback(base, 0, ctx);
        return 1;
    }
    if (r == num) {                     /* the only combination is the input itself */
        callback(base, r, ctx);
        return 1;
    }
    if (r > ((size_t)-1) / size || r > ((size_t)-1) / sizeof(size_t)) {
        return 0;
    }

    union { long double ld; long long ll; void *p; unsigned char bytes[512]; } combo_stack;
    size_t idx_stack[32];
    char *combo = (r * size <= sizeof combo_stack.bytes) ? (char *)combo_stack.bytes : (char *)malloc(r * size);
    size_t *idx = (r <= sizeof idx_stack / sizeof idx_stack[0]) ? idx_stack : (size_t *)malloc(r * sizeof(size_t));
    if (!combo || !idx) {
        if (combo && combo != (char *)combo_stack.bytes) {
            free(combo);
        }
        if (idx && idx != idx_stack) {
            free(idx);
        }
        ALGORITHM_LOG("[algorithm_combination] Error: allocation failed.");
        return 0;
    }

    const char *src = (const char *)base;
    for (size_t k = 0; k < r; ++k) {
        idx[k] = k;
    }
    memcpy(combo, src, r * size);

    size_t visited = 0;
    for (;;) {
        visited++;
        if (!callback(combo, r, ctx)) {
            break;
        }
        /* rightmost index that can still move right */
        size_t i = r;
        while (i > 0 && idx[i - 1] == num - r + (i - 1)) {
            --i;
        }
        if (i == 0) {
            break;
        }
        --i;
        idx[i]++;
        for (size_t k = i + 1; k < r; ++k) {
            idx[k] = idx[k - 1] + 1;
        }
        /* positions i .. r-1 now hold consecutive input elements */
        memcpy(combo + i * size, src + idx[i] * size, (r - i) * size);
    }

    if (combo != (char *)combo_stack.bytes) {
        free(combo);
    }
    if (idx != idx_stack) {
        free(idx);
    }
    ALGORITHM_LOG("[algorithm_combination] Success: %zu combinations visited.", visited);
    return visited;
}
