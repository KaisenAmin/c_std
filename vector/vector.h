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

/* Per-Vector arena. Allocated once at `vector_create` time and
 * grown lazily by `vector_reserve` / `vector_resize`. The block at
 * `pool` is owned by the Vector and freed in `vector_deallocate`. */
typedef struct MemoryPoolVector {
    void*  pool;       /* raw backing block                         */
    size_t poolSize;   /* total bytes available                     */
    size_t used;       /* bytes handed out so far                   */
} MemoryPoolVector;


struct Vector {
    void*             items;
    size_t            size;
    size_t            capacitySize;
    size_t            itemSize;
    MemoryPoolVector* pool;
};


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

bool        vector_push_back             (Vector* vec, const void* item);
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

void*       vector_at                    (const Vector* vec, size_t pos);
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
