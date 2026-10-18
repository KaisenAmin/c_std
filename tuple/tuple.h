/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Tuple
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in tuple.c (file-level convention).
 */

#ifndef TUPLE_H_
#define TUPLE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define TUPLE_LOGGING_ENABLE */

#ifdef TUPLE_LOGGING_ENABLE
    #define TUPLE_LOG(...) \
        do { fprintf(stderr, "[TUPLE LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
    #define TUPLE_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

/* Element comparator, qsort convention (negative / zero / positive). */
typedef int (*TupleCompareFunc)(const void* a, const void* b);

/* One slot in a Tuple. `data` is always heap-owned by the Tuple: for
 * tuple_tie / tuple_forward_as_tuple it is an 8-byte copy of the POINTER
 * that was passed in, so tuple_get returns a `T**` (the pointed-to objects
 * are borrowed, never copied or freed). `compare` is optional
 * (tuple_set_compare): without it the slot is compared byte by byte,
 * which is not the numeric order of int / float values. */
typedef struct {
    void*  data;
    size_t size;
    TupleCompareFunc compare;
} TupleElement;

/* Fixed-arity heterogeneous container. Use `tuple_create` for an
 * empty arity-N tuple; use `tuple_make_tuple` / `tuple_tie` /
 * `tuple_forward_as_tuple` to build one from inline values. */
typedef struct {
    TupleElement* elements;
    size_t        size;
} Tuple;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Tuple*  tuple_create                  (size_t size);
Tuple*  tuple_make_tuple              (size_t num, ...);
Tuple*  tuple_tie                     (size_t num, ...);
Tuple*  tuple_forward_as_tuple        (size_t num, ...);


/* ------------------------------------------------------------------ */
/* Destruction / mutation                                             */
/* ------------------------------------------------------------------ */

void    tuple_deallocate              (Tuple* tuple);
void    tuple_swap                    (Tuple* a, Tuple* b);
bool    tuple_set                     (Tuple* tuple, size_t index, void* data, size_t size);
bool    tuple_set_compare             (Tuple* tuple, size_t index, TupleCompareFunc compare);


/* ------------------------------------------------------------------ */
/* Accessors                                                          */
/* ------------------------------------------------------------------ */

void*   tuple_get                     (const Tuple* tuple, size_t index, size_t* outSize);
size_t  tuple_size                    (const Tuple* tuple);
bool    tuple_is_empty                (Tuple* t);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool    tuple_is_equal                (const Tuple* t1, const Tuple* t2);
bool    tuple_is_not_equal            (const Tuple* t1, const Tuple* t2);
bool    tuple_is_less                 (const Tuple* t1, const Tuple* t2);
bool    tuple_is_less_or_equal        (const Tuple* t1, const Tuple* t2);
bool    tuple_is_greater              (const Tuple* t1, const Tuple* t2);
bool    tuple_is_greater_or_equal     (const Tuple* t1, const Tuple* t2);


#ifdef __cplusplus
}
#endif

#endif 
