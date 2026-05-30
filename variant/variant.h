/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class Variant
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in variant.c (file-level convention).
 *
 * A small, memory-safe, C++-flavoured `std::variant` for C. A
 * `Variant` is a tagged union that can hold any of the built-in
 * primitive types (int, long, float, double, char, bool, string,
 * pointer) or a fully user-defined type registered via
 * `VariantTypeInfo`.
 *
 * Design rules:
 *   - Every public function is NULL-safe and reports failure via the
 *     return value (no global errno-style state).
 *   - Memory is always owned by the Variant. `variant_destroy()`
 *     releases everything; double-destroy is safe.
 *   - There is no "unsafe" getter — every accessor checks the type
 *     and returns false on mismatch.
 */

#ifndef VARIANT_H
#define VARIANT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define VARIANT_LOGGING_ENABLE */

#ifdef VARIANT_LOGGING_ENABLE
    #define VARIANT_LOG(fmt, ...) \
        do { fprintf(stderr, "[VARIANT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define VARIANT_LOG(...) do { } while (0)
#endif


typedef enum {
    VARIANT_TYPE_NONE = 0,
    VARIANT_TYPE_INT,
    VARIANT_TYPE_LONG,
    VARIANT_TYPE_FLOAT,
    VARIANT_TYPE_DOUBLE,
    VARIANT_TYPE_CHAR,
    VARIANT_TYPE_BOOL,
    VARIANT_TYPE_POINTER,   /* non-owning                                 */
    VARIANT_TYPE_STRING,    /* owning, NUL-terminated                     */
    VARIANT_TYPE_CUSTOM     /* owning, see VariantTypeInfo                */
} VariantType;


/* User-supplied callbacks describing a CUSTOM type. */
typedef void (*VariantCopyFn)   (void* dest, const void* src);
typedef void (*VariantDestroyFn)(void* p);
typedef int  (*VariantCompareFn)(const void* a, const void* b);
typedef int  (*VariantPrintFn)  (char* buf, size_t buflen, const void* p);


typedef struct {
    size_t            size;        /* sizeof(T)                            */
    VariantCopyFn     copy;        /* memcpy fallback if NULL              */
    VariantDestroyFn  destroy;     /* no-op fallback if NULL               */
    VariantCompareFn  compare;     /* memcmp fallback if NULL              */
    VariantPrintFn    print;       /* "<TYPE>" fallback if NULL            */
    const char*       type_name;   /* "custom" fallback if NULL            */
} VariantTypeInfo;


typedef struct Variant {
    VariantType type;
    union {
        int     i;
        long    l;
        float   f;
        double  d;
        char    c;
        bool    b;
        void*   p;
        char*   s;       /* owning */
        struct {
            void*                  data;        /* owning                 */
            const VariantTypeInfo* info;
        } custom;
    } as;
} Variant;


/* User visitor for variant_visit / variant_visit_all. */
typedef int (*VariantVisitFn)(const Variant* v, void* user_data);

/* Opaque dynamic-array-of-Variant. */
typedef struct VariantArray VariantArray;


/* ------------------------------------------------------------------ */
/* Constructors                                                       */
/* ------------------------------------------------------------------ */

Variant     variant_none                 (void);
Variant     variant_from_int             (int v);
Variant     variant_from_long            (long v);
Variant     variant_from_float           (float v);
Variant     variant_from_double          (double v);
Variant     variant_from_char            (char v);
Variant     variant_from_bool            (bool v);
Variant     variant_from_pointer         (void* v);                        /* non-owning */
Variant     variant_from_string          (const char* v);                  /* deep copy  */
Variant     variant_from_custom          (const void* src, const VariantTypeInfo* info);
Variant     variant_clone                (const Variant* src);


/* ------------------------------------------------------------------ */
/* Destructors / ownership transfer                                   */
/* ------------------------------------------------------------------ */

void        variant_destroy              (Variant* v);
void        variant_swap                 (Variant* a, Variant* b);
void        variant_move                 (Variant* dst, Variant* src);
void*       variant_take                 (Variant* v);


/* ------------------------------------------------------------------ */
/* Setters (in-place type change)                                     */
/* ------------------------------------------------------------------ */

bool        variant_set_none             (Variant* v);
bool        variant_set_int              (Variant* v, int    x);
bool        variant_set_long             (Variant* v, long   x);
bool        variant_set_float            (Variant* v, float  x);
bool        variant_set_double           (Variant* v, double x);
bool        variant_set_char             (Variant* v, char   x);
bool        variant_set_bool             (Variant* v, bool   x);
bool        variant_set_pointer          (Variant* v, void*  x);
bool        variant_set_string           (Variant* v, const char* x);
bool        variant_set_custom           (Variant* v, const void* src, const VariantTypeInfo* info);


/* ------------------------------------------------------------------ */
/* Getters (type-checked; return false on mismatch)                   */
/* ------------------------------------------------------------------ */

bool        variant_get_int              (const Variant* v, int*    out);
bool        variant_get_long             (const Variant* v, long*   out);
bool        variant_get_float            (const Variant* v, float*  out);
bool        variant_get_double           (const Variant* v, double* out);
bool        variant_get_char             (const Variant* v, char*   out);
bool        variant_get_bool             (const Variant* v, bool*   out);
bool        variant_get_pointer          (const Variant* v, void**  out);
bool        variant_get_string           (const Variant* v, const char** out);
bool        variant_get_custom           (const Variant* v, void** out, const VariantTypeInfo** out_info);


/* ------------------------------------------------------------------ */
/* Introspection                                                      */
/* ------------------------------------------------------------------ */

VariantType variant_type                 (const Variant* v);
const char* variant_type_name            (const Variant* v);
size_t      variant_type_size            (const Variant* v);
size_t      variant_hash                 (const Variant* v);
bool        variant_is                   (const Variant* v, VariantType t);
bool        variant_is_none              (const Variant* v);


/* ------------------------------------------------------------------ */
/* Comparison / equality                                              */
/* ------------------------------------------------------------------ */

bool        variant_equal                (const Variant* a, const Variant* b);
int         variant_compare              (const Variant* a, const Variant* b);


/* ------------------------------------------------------------------ */
/* Printing / formatting                                              */
/* ------------------------------------------------------------------ */

char*       variant_to_string            (const Variant* v);
int         variant_print                (const Variant* v);                /* stdout, no newline */
int         variant_println              (const Variant* v);                /* stdout + newline   */


/* ------------------------------------------------------------------ */
/* Visitors                                                           */
/* ------------------------------------------------------------------ */

int         variant_visit                (const Variant* v, VariantVisitFn fn, void* user_data);
int         variant_visit_all            (const Variant* arr, size_t count, VariantVisitFn fn, void* user_data);


/* ------------------------------------------------------------------ */
/* VariantArray — dynamic array of Variants                           */
/* ------------------------------------------------------------------ */

VariantArray* variant_array_new          (size_t initial_capacity);
void          variant_array_free         (VariantArray* a);
void          variant_array_clear        (VariantArray* a);

bool          variant_array_push         (VariantArray* a, const Variant* v);
bool          variant_array_pop          (VariantArray* a);                 /* drops last */
bool          variant_array_set          (VariantArray* a, size_t index, const Variant* v);
bool          variant_array_remove_at    (VariantArray* a, size_t index);
bool          variant_array_reserve      (VariantArray* a, size_t min_capacity);

Variant*      variant_array_at           (VariantArray* a, size_t index);
size_t        variant_array_size         (const VariantArray* a);
size_t        variant_array_capacity     (const VariantArray* a);
bool          variant_array_empty        (const VariantArray* a);


/* ------------------------------------------------------------------ */
/* Built-in VariantTypeInfo descriptors                               */
/* ------------------------------------------------------------------ */

extern const VariantTypeInfo variant_typeinfo_int;
extern const VariantTypeInfo variant_typeinfo_long;
extern const VariantTypeInfo variant_typeinfo_float;
extern const VariantTypeInfo variant_typeinfo_double;
extern const VariantTypeInfo variant_typeinfo_char;
extern const VariantTypeInfo variant_typeinfo_bool;
extern const VariantTypeInfo variant_typeinfo_pointer;
extern const VariantTypeInfo variant_typeinfo_string;


#ifdef __cplusplus
}
#endif

#endif 
