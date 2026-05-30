/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class Variant
 */

#include "variant.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


static char* dup_cstr(const char* s) {
    if (!s) 
        return NULL;

    size_t n = strlen(s) + 1;
    char* out = (char*)malloc(n);

    if (out) 
        memcpy(out, s, n);
    return out;
}

/* Allocate and initialise the storage for a CUSTOM variant. Returns
 * NULL on bad info or OOM. */
static void* custom_alloc(const void* src, const VariantTypeInfo* info) {
    if (!info || info->size == 0) {
        return NULL;
    }

    void* mem = malloc(info->size);
    if (!mem) {
        return NULL;
    }
    if (info->copy) {
        info->copy(mem, src);
    }
    else {            
        memcpy(mem, src, info->size);
    }
    return mem;
}


static void custom_destroy(void* data, const VariantTypeInfo* info) {
    if (!data) {
        return;
    }
    if (info && info->destroy) {
        info->destroy(data);
    }
    free(data);
}


/**
 * @brief Build an empty (NONE-tagged) variant.
 *
 * A NONE variant carries no value. It's the canonical "absence of a
 * value" — equivalent to `std::variant`'s monostate. Always succeeds;
 * the returned value is safe to pass to every other variant_* function.
 *
 * @return A zero-initialised Variant whose `type == VARIANT_TYPE_NONE`.
 */
Variant variant_none(void) {
    Variant v;
    memset(&v, 0, sizeof v);
    v.type = VARIANT_TYPE_NONE;

    VARIANT_LOG("[variant_none]: created NONE variant");
    return v;
}


/**
 * @brief Build an INT-tagged variant holding @p x.
 * @param x  Value to store (by value).
 * @return Variant whose `type == VARIANT_TYPE_INT`.
 */
Variant variant_from_int(int x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_INT;
    v.as.i = x;

    VARIANT_LOG("[variant_from_int]: created INT variant with value %d", x);
    return v;
}


/**
 * @brief Build a LONG-tagged variant holding @p x.
 * @param x  Value to store (by value).
 * @return Variant whose `type == VARIANT_TYPE_LONG`.
 */
Variant variant_from_long(long x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_LONG;
    v.as.l = x;

    VARIANT_LOG("[variant_from_long]: created LONG variant with value %ld", x);
    return v;
}


/**
 * @brief Build a FLOAT-tagged variant holding @p x.
 * @param x  Value to store (by value).
 * @return Variant whose `type == VARIANT_TYPE_FLOAT`.
 */
Variant variant_from_float(float x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_FLOAT;
    v.as.f = x;

    VARIANT_LOG("[variant_from_float]: created FLOAT variant with value %g", (double)x);
    return v;
}


/**
 * @brief Build a DOUBLE-tagged variant holding @p x.
 * @param x  Value to store (by value).
 * @return Variant whose `type == VARIANT_TYPE_DOUBLE`.
 */
Variant variant_from_double(double x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_DOUBLE;
    v.as.d = x;

    VARIANT_LOG("[variant_from_double]: created DOUBLE variant with value %g", x);
    return v;
}


/**
 * @brief Build a CHAR-tagged variant holding the byte @p x.
 *
 * `char` here is the single-byte integer, not a code point — pass `'A'`,
 * not a multibyte sequence.
 *
 * @param x  Byte to store.
 * @return Variant whose `type == VARIANT_TYPE_CHAR`.
 */
Variant variant_from_char(char x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_CHAR;
    v.as.c = x;

    VARIANT_LOG("[variant_from_char]: created CHAR variant with value '%c'", x);
    return v;
}


/**
 * @brief Build a BOOL-tagged variant holding @p x.
 * @param x  Value to store.
 * @return Variant whose `type == VARIANT_TYPE_BOOL`.
 */
Variant variant_from_bool(bool x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_BOOL;
    v.as.b = x;

    VARIANT_LOG("[variant_from_bool]: created BOOL variant with value %s", x ? "true" : "false");
    return v;
}


/**
 * @brief Build a POINTER-tagged variant holding the raw address @p x.
 *
 * The variant does **NOT** take ownership of what @p x points to. The
 * caller must keep the underlying object alive for as long as the
 * variant might be dereferenced, and is responsible for its eventual
 * release. `variant_destroy()` on a POINTER variant only clears the
 * tag — it does NOT free @p x.
 *
 * Use this for "borrowed handle" semantics. For owning storage of an
 * arbitrary object, use `variant_from_custom`.
 *
 * @param x  Address to remember. May be NULL.
 * @return Variant whose `type == VARIANT_TYPE_POINTER`.
 */
Variant variant_from_pointer(void* x) {
    Variant v = variant_none();
    v.type = VARIANT_TYPE_POINTER;
    v.as.p = x;

    VARIANT_LOG("[variant_from_pointer]: created POINTER variant -> %p", x);
    return v;
}


/**
 * @brief Build a STRING-tagged variant holding a deep copy of @p x.
 *
 * The bytes are duplicated into freshly-allocated storage owned by the
 * variant. The caller may free / mutate the source buffer immediately
 * after this call returns. The owned copy is released by
 * `variant_destroy()` (or by any subsequent setter that re-tags the
 * variant).
 *
 * Passing NULL returns a NONE variant.
 *
 * @param x  NUL-terminated source string. May be NULL.
 * @return STRING variant on success; NONE on NULL input or OOM.
 */
Variant variant_from_string(const char* x) {
    if (!x) {
        VARIANT_LOG("[variant_from_string]: input NULL, returning NONE");
        return variant_none();
    }

    char* dup = dup_cstr(x);
    if (!dup) {
        VARIANT_LOG("[variant_from_string]: OOM duplicating \"%s\", returning NONE", x);
        return variant_none();
    }
    Variant v = variant_none();
    v.type = VARIANT_TYPE_STRING;
    v.as.s = dup;

    VARIANT_LOG("[variant_from_string]: created STRING variant \"%s\"", x);
    return v;
}

/**
 * @brief Build a CUSTOM-tagged variant holding a user-defined value.
 *
 * `info->size` bytes are deep-copied from @p src into freshly-allocated
 * storage owned by the variant. If `info->copy` is non-NULL it's used
 * for the duplication; otherwise the bytes are `memcpy`'d as-is (works
 * for POD types but not for types that own further allocations — for
 * those, register a proper `copy` hook).
 *
 * On `variant_destroy()` (or any setter that re-tags the variant), the
 * payload's `info->destroy` hook is invoked first (if non-NULL), then
 * the storage is `free()`d.
 *
 * @param src   Pointer to the source value. Must NOT be NULL.
 * @param info  Descriptor for the user type. Must NOT be NULL and must
 *              have a non-zero `size`.
 * @return CUSTOM variant on success; NONE on bad arguments or OOM.
 */
Variant variant_from_custom(const void* src, const VariantTypeInfo* info) {
    if (!src || !info || info->size == 0) {
        VARIANT_LOG("[variant_from_custom]: bad arg (src=%p info=%p size=%zu) -> NONE", src, (const void*)info, info ? info->size : (size_t)0);
        return variant_none();
    }

    void* mem = custom_alloc(src, info);
    if (!mem) {
        VARIANT_LOG("[variant_from_custom]: OOM allocating %zu bytes for type %s", info->size, info->type_name ? info->type_name : "custom");
        return variant_none();
    }

    Variant v = variant_none();
    v.type = VARIANT_TYPE_CUSTOM;
    v.as.custom.data = mem;
    v.as.custom.info = info;

    VARIANT_LOG("[variant_from_custom]: created CUSTOM variant of type %s (%zu bytes)", info->type_name ? info->type_name : "custom", info->size);
    return v;
}

/* --------------------------------------------------------------------
 *  Destruction
 * -------------------------------------------------------------------- */

/**
 * @brief Release all heap memory owned by @p v and reset it to NONE.
 *
 * Behavior by tag:
 *   - NONE / primitives / POINTER: no allocation to free; just clears
 *     the tag to NONE.
 *   - STRING: frees the owned buffer.
 *   - CUSTOM: calls the `info->destroy` hook (if any), then frees the
 *     storage.
 *
 * Idempotent — calling it twice is safe (the second call is a no-op
 * because the first already cleared the tag to NONE). NULL receiver
 * is a safe no-op.
 *
 * @param v  Variant to clear. May be NULL.
 */
void variant_destroy(Variant* v) {
    if (!v) {
        VARIANT_LOG("[variant_destroy]: NULL receiver, no-op");
        return;
    }
    VARIANT_LOG("[variant_destroy]: destroying variant of type %s", variant_type_name(v));
    switch (v->type) {
        case VARIANT_TYPE_STRING:
            free(v->as.s);
            v->as.s = NULL;
            break;
        case VARIANT_TYPE_CUSTOM:
            custom_destroy(v->as.custom.data, v->as.custom.info);
            v->as.custom.data = NULL;
            v->as.custom.info = NULL;
            break;
        default:
            break;
    }
    v->type = VARIANT_TYPE_NONE;
}


/**
 * @brief Deep-copy @p src into a brand-new Variant.
 *
 * For STRING and CUSTOM variants the underlying storage is duplicated
 * (using `info->copy` for custom types, or `strdup` for strings) so the
 * source and the clone are fully independent — mutating one does not
 * affect the other. For primitives the copy is just the value.
 *
 * Pointer variants are cloned by **pointer identity** (the clone holds
 * the same address — both alias the same external object).
 *
 * NULL or NONE input yields a NONE result.
 *
 * @param src  Source variant. May be NULL.
 * @return Independent copy of @p src; NONE on failure (e.g. OOM during
 *         string/custom duplication).
 */
Variant variant_clone(const Variant* src) {
    if (!src) {
        VARIANT_LOG("[variant_clone]: NULL source, returning NONE");
        return variant_none();
    }
    VARIANT_LOG("[variant_clone]: deep-copying %s variant", variant_type_name(src));
    switch (src->type) {
        case VARIANT_TYPE_NONE:    
            return variant_none();
        case VARIANT_TYPE_INT:     
            return variant_from_int    (src->as.i);
        case VARIANT_TYPE_LONG:    
            return variant_from_long   (src->as.l);
        case VARIANT_TYPE_FLOAT:   
            return variant_from_float  (src->as.f);
        case VARIANT_TYPE_DOUBLE:  
            return variant_from_double (src->as.d);
        case VARIANT_TYPE_CHAR:    
            return variant_from_char   (src->as.c);
        case VARIANT_TYPE_BOOL:    
            return variant_from_bool   (src->as.b);
        case VARIANT_TYPE_POINTER: 
            return variant_from_pointer(src->as.p);
        case VARIANT_TYPE_STRING:  
            return variant_from_string (src->as.s);
        case VARIANT_TYPE_CUSTOM:  
            return variant_from_custom (src->as.custom.data, src->as.custom.info);
    }
    VARIANT_LOG("[variant_clone]: unrecognised tag %d", (int)src->type);
    return variant_none();
}

/* Helper used by every variant_set_*. Destroys the current value
 * then takes over the new one. */
#define SET_PRIM(v, field, value, tag) \
    do { if (!(v)) { \
             VARIANT_LOG("[variant_set_*]: NULL receiver, returning false"); \
             return false; \
         } \
         variant_destroy(v); \
         (v)->type = (tag); \
         (v)->as.field = (value); \
         VARIANT_LOG("[variant_set_*]: re-tagged as %s", variant_type_name(v)); \
         return true; } while (0)


/**
 * @brief Re-tag @p v as INT and assign @p x.
 *
 * The previous content (if any) is destroyed first via
 * `variant_destroy`, so any owned string/custom payload is correctly
 * released before the new value lands. NULL receiver returns false.
 *
 * @param v  Receiver. May be NULL.
 * @param x  New int value.
 * @return true on success; false on NULL receiver.
 */
bool variant_set_int(Variant* v, int x) {
    SET_PRIM(v, i, x, VARIANT_TYPE_INT);
}

/**
 * @brief Re-tag @p v as LONG and assign @p x.
 * @copydetails variant_set_int
 */
bool variant_set_long(Variant* v, long x) {
    SET_PRIM(v, l, x, VARIANT_TYPE_LONG);
}

/**
 * @brief Re-tag @p v as FLOAT and assign @p x.
 * @copydetails variant_set_int
 */
bool variant_set_float(Variant* v, float x) {
    SET_PRIM(v, f, x, VARIANT_TYPE_FLOAT);
}

/**
 * @brief Re-tag @p v as DOUBLE and assign @p x.
 * @copydetails variant_set_int
 */
bool variant_set_double(Variant* v, double x) {
    SET_PRIM(v, d, x, VARIANT_TYPE_DOUBLE);
}

/**
 * @brief Re-tag @p v as CHAR and assign @p x.
 * @copydetails variant_set_int
 */
bool variant_set_char(Variant* v, char x) {
    SET_PRIM(v, c, x, VARIANT_TYPE_CHAR);
}

/**
 * @brief Re-tag @p v as BOOL and assign @p x.
 * @copydetails variant_set_int
 */
bool variant_set_bool(Variant* v, bool x) {
    SET_PRIM(v, b, x, VARIANT_TYPE_BOOL);
}

/**
 * @brief Re-tag @p v as a non-owning POINTER and assign @p x.
 *
 * As with `variant_from_pointer`, the variant does NOT take ownership;
 * the caller is responsible for the underlying object's lifetime.
 *
 * @param v  Receiver. May be NULL.
 * @param x  Address to remember. May be NULL.
 * @return true on success; false on NULL receiver.
 */
bool variant_set_pointer(Variant* v, void* x) {
    SET_PRIM(v, p, x, VARIANT_TYPE_POINTER);
}


/**
 * @brief Re-tag @p v as STRING with a deep-copied @p x.
 *
 * Safe even when @p x is a pointer the variant currently owns (e.g. the
 * result of `variant_get_string` on the same variant) — the new buffer
 * is allocated and copied BEFORE the old content is destroyed, so
 * there's no use-after-free in the self-set case.
 *
 * Behavior:
 *   - NULL receiver  → returns false
 *   - NULL @p x      → receiver is reset to NONE; returns true
 *   - OOM            → receiver is reset to NONE; returns false
 *
 * @param v  Receiver. May be NULL.
 * @param x  NUL-terminated string. May be NULL.
 * @return true on success; see Behavior above.
 */
bool variant_set_string(Variant* v, const char* x) {
    if (!v) {
        VARIANT_LOG("[variant_set_string]: NULL receiver");
        return false;
    }
    if (x == NULL) {
        VARIANT_LOG("[variant_set_string]: NULL string -> reset to NONE");
        variant_destroy(v);
        return true;
    }
    char* dup = dup_cstr(x);
    if (!dup) {
        VARIANT_LOG("[variant_set_string]: OOM duplicating \"%s\"", x);
        variant_destroy(v);
        return false;
    }
    variant_destroy(v);

    v->type = VARIANT_TYPE_STRING;
    v->as.s = dup;

    VARIANT_LOG("[variant_set_string]: set STRING \"%s\"", x);
    return true;
}


/**
 * @brief Re-tag @p v as CUSTOM and copy @p src into newly-owned storage.
 *
 * Functionally `variant_destroy(v); *v = variant_from_custom(src, info);`
 * but allocates BEFORE destroying so a failed allocation leaves the
 * receiver in a NONE (not corrupted) state.
 *
 * @param v     Receiver. May be NULL.
 * @param src   Source value to copy. Must NOT be NULL.
 * @param info  Type descriptor. Must NOT be NULL; `info->size` must be > 0.
 * @return true on success; false on bad argument or OOM. On OOM the
 *         receiver is reset to NONE.
 */
bool variant_set_custom(Variant* v, const void* src, const VariantTypeInfo* info) {
    if (!v || !src || !info || info->size == 0) {
        VARIANT_LOG("[variant_set_custom]: bad arg (v=%p src=%p info=%p)", (void*)v, src, (const void*)info);
        return false;
    }
    void* mem = custom_alloc(src, info);
    if (!mem) {
        VARIANT_LOG("[variant_set_custom]: OOM (%zu bytes)", info->size);
        variant_destroy(v);
        return false;
    }
    variant_destroy(v);

    v->type = VARIANT_TYPE_CUSTOM;
    v->as.custom.data = mem;
    v->as.custom.info = info;

    VARIANT_LOG("[variant_set_custom]: set CUSTOM (%s)", info->type_name ? info->type_name : "custom");
    return true;
}


/**
 * @brief Reset @p v to NONE, releasing any owned payload.
 *
 * Equivalent to `variant_destroy(v)` but returns a status instead of
 * void, so it composes cleanly with other setters that follow the same
 * `bool` convention.
 *
 * @param v  Receiver. May be NULL.
 * @return true on success; false on NULL receiver.
 */
bool variant_set_none(Variant* v) {
    if (!v) {
        VARIANT_LOG("[variant_set_none]: NULL receiver");
        return false;
    }
    VARIANT_LOG("[variant_set_none]: clearing variant");
    variant_destroy(v);

    return true;
}


/**
 * @brief Move 
 * @p src into 
 * @p dst (destroying @p dst's prior value).
 *  
 * After this returns, @p src is NONE. O(1), no allocation.
 */
void variant_move(Variant* dst, Variant* src) {
    if (!dst || !src) {
        VARIANT_LOG("[variant_move]: NULL arg (dst=%p src=%p)", (void*)dst, (void*)src);
        return;
    }
    if (dst == src) {
        VARIANT_LOG("[variant_move]: self-move, no-op");
        return;
    }
    VARIANT_LOG("[variant_move]: moving %s -> dst (was %s)", variant_type_name(src), variant_type_name(dst));
    variant_destroy(dst);

    *dst = *src;
    memset(src, 0, sizeof *src);
    src->type = VARIANT_TYPE_NONE;
}


/**
 * @brief Swap the contents of two variants. O(1), no allocation.
 *
 * After this call @p a holds what @p b held and vice versa. No
 * destructors run — ownership of any owned payload migrates with the
 * struct contents. NULL inputs and self-swap are no-ops.
 *
 * @param a  First variant. May be NULL.
 * @param b  Second variant. May be NULL.
 */
void variant_swap(Variant* a, Variant* b) {
    if (!a || !b || a == b) {
        VARIANT_LOG("[variant_swap]: no-op (a=%p b=%p)", (void*)a, (void*)b);
        return;
    }

    VARIANT_LOG("[variant_swap]: swapping %s <-> %s", variant_type_name(a), variant_type_name(b));
    Variant t = *a;
    *a = *b;
    *b = t;
}

/**
 * @brief Detach the owned payload (string buffer or custom data
 *        pointer) without freeing it. After this call @p v is NONE
 *        and the caller owns the returned pointer.
 *
 * @return The detached pointer. For non-owning variants (NONE / int /
 *         pointer / etc.) returns NULL and the variant is reset.
 */
void* variant_take(Variant* v) {
    if (!v) {
        VARIANT_LOG("[variant_take]: NULL receiver");
        return NULL;
    }
    VARIANT_LOG("[variant_take]: detaching payload from %s variant", variant_type_name(v));
    void* out = NULL;

    if (v->type == VARIANT_TYPE_STRING) {
        out = v->as.s;
        v->as.s = NULL;
    } 
    else if (v->type == VARIANT_TYPE_CUSTOM) {
        out = v->as.custom.data;
        v->as.custom.data = NULL;
        v->as.custom.info = NULL;
    }

    v->type = VARIANT_TYPE_NONE;
    memset(&v->as, 0, sizeof v->as);
    return out;
}


/**
 * @brief Return the variant's tag.
 * @param v  Variant. May be NULL.
 * @return `v->type` for a valid pointer; `VARIANT_TYPE_NONE` for NULL.
 */
VariantType variant_type(const Variant* v) {
    return v ? v->type : VARIANT_TYPE_NONE;
}


/**
 * @brief Return a human-readable name for the variant's active type.
 *
 * Always returns a non-NULL static string. For CUSTOM variants, uses
 * `info->type_name` if available (falls back to `"custom"`). NULL input
 * returns `"none"`.
 *
 * @param v  Variant. May be NULL.
 * @return Static string. Never NULL.
 */
const char* variant_type_name(const Variant* v) {
    if (!v) return "none";
    switch (v->type) {
        case VARIANT_TYPE_NONE:    
            return "none";
        case VARIANT_TYPE_INT:     
            return "int";
        case VARIANT_TYPE_LONG:    
            return "long";
        case VARIANT_TYPE_FLOAT:   
            return "float";
        case VARIANT_TYPE_DOUBLE:  
            return "double";
        case VARIANT_TYPE_CHAR:    
            return "char";
        case VARIANT_TYPE_BOOL:    
            return "bool";
        case VARIANT_TYPE_POINTER: 
            return "pointer";
        case VARIANT_TYPE_STRING:  
            return "string";
        case VARIANT_TYPE_CUSTOM:
            return (v->as.custom.info && v->as.custom.info->type_name)
                       ? v->as.custom.info->type_name : "custom";
    }
    return "unknown";
}


/**
 * @brief Storage size in bytes for the currently-held value.
 *
 * For primitives this is `sizeof(T)`. For STRING this is `strlen(s)`
 * (NOT including the NUL). For CUSTOM this is `info->size`. NONE
 * returns 0.
 *
 * @param v  Variant. May be NULL (returns 0).
 * @return Size in bytes; 0 for NONE / NULL.
 */
size_t variant_type_size(const Variant* v) {
    if (!v) return 0;
    switch (v->type) {
        case VARIANT_TYPE_NONE:    
            return 0;
        case VARIANT_TYPE_INT:     
            return sizeof(int);
        case VARIANT_TYPE_LONG:    
            return sizeof(long);
        case VARIANT_TYPE_FLOAT:   
            return sizeof(float);
        case VARIANT_TYPE_DOUBLE:  
            return sizeof(double);
        case VARIANT_TYPE_CHAR:    
            return sizeof(char);
        case VARIANT_TYPE_BOOL:    
            return sizeof(bool);
        case VARIANT_TYPE_POINTER: 
            return sizeof(void*);
        case VARIANT_TYPE_STRING:  
            return v->as.s ? strlen(v->as.s) : 0;
        case VARIANT_TYPE_CUSTOM:  
            return v->as.custom.info ? v->as.custom.info->size : 0;
    }
    return 0;
}

/**
 * @brief Tag predicate. True iff the variant currently holds type @p t.
 *
 * NULL receiver returns false (even for `VARIANT_TYPE_NONE` — use
 * `variant_is_none` for the "NULL or NONE" shorthand).
 *
 * @param v  Variant. May be NULL.
 * @param t  Tag to test against.
 */
bool variant_is(const Variant* v, VariantType t) {
    return v && v->type == t;
}


/**
 * @brief Convenience: true for NULL OR for a NONE-tagged variant.
 *
 * The asymmetry with `variant_is(v, VARIANT_TYPE_NONE)` is deliberate —
 * "is this value absent?" is the common question for option-style code,
 * and a NULL receiver is just another way of saying "absent".
 *
 * @param v  Variant. May be NULL.
 */
bool variant_is_none(const Variant* v) {
    return !v || v->type == VARIANT_TYPE_NONE;
}

/* --------------------------------------------------------------------
 *  Accessors
 * -------------------------------------------------------------------- */

#define GET_PRIM(v, t, field, out)                                                   \
    do { if (!(v) || !(out) || (v)->type != (t)) {                                   \
             VARIANT_LOG("[variant_get_*]: rejected (v=%p out=%p actual=%s)",        \
                         (const void*)(v), (const void*)(out),                       \
                         variant_type_name(v));                                      \
             return false;                                                           \
         }                                                                           \
         *(out) = (v)->as.field;                                                     \
         return true; } while (0)

/**
 * @brief Type-checked read of the INT value.
 *
 * `*out` is only written when the variant is INT-tagged AND @p out is
 * non-NULL — a type-mismatch leaves the caller's buffer untouched
 * (callers can rely on this to retain sentinel values).
 *
 * @param v    Variant. May be NULL.
 * @param out  Destination. May be NULL.
 * @return true iff `v->type == VARIANT_TYPE_INT` AND @p out is non-NULL.
 */
bool variant_get_int(const Variant* v, int* out) {
    GET_PRIM(v, VARIANT_TYPE_INT, i, out);
}

/**
 * @brief Type-checked read of the LONG value.
 * @copydetails variant_get_int
 */
bool variant_get_long(const Variant* v, long* out) {
    GET_PRIM(v, VARIANT_TYPE_LONG, l, out);
}

/**
 * @brief Type-checked read of the FLOAT value.
 * @copydetails variant_get_int
 */
bool variant_get_float(const Variant* v, float* out) {
    GET_PRIM(v, VARIANT_TYPE_FLOAT, f, out);
}

/**
 * @brief Type-checked read of the DOUBLE value.
 * @copydetails variant_get_int
 */
bool variant_get_double(const Variant* v, double* out) {
    GET_PRIM(v, VARIANT_TYPE_DOUBLE, d, out);
}

/**
 * @brief Type-checked read of the CHAR value.
 * @copydetails variant_get_int
 */
bool variant_get_char(const Variant* v, char* out) {
    GET_PRIM(v, VARIANT_TYPE_CHAR, c, out);
}

/**
 * @brief Type-checked read of the BOOL value.
 * @copydetails variant_get_int
 */
bool variant_get_bool(const Variant* v, bool* out) {
    GET_PRIM(v, VARIANT_TYPE_BOOL, b, out);
}

/**
 * @brief Type-checked read of the POINTER value.
 *
 * The returned pointer is borrowed — the caller is still responsible
 * for whatever it points to, since POINTER variants don't take
 * ownership at construction time.
 *
 * @copydetails variant_get_int
 */
bool variant_get_pointer(const Variant* v, void** out) {
    GET_PRIM(v, VARIANT_TYPE_POINTER, p, out);
}


/**
 * @brief Borrow the variant's underlying string buffer.
 *
 * On success `*out` is set to a pointer into the variant's owned
 * storage. That pointer is **valid only until the variant is destroyed
 * or reassigned** (`variant_destroy`, `variant_set_*`, `variant_move`,
 * or `variant_take`). Do not free it — the variant owns it.
 *
 * @param v    Variant. May be NULL.
 * @param out  Destination. May be NULL (function returns false).
 * @return true iff @p v is a STRING and @p out is non-NULL.
 */
bool variant_get_string(const Variant* v, const char** out) {
    if (!v || !out || v->type != VARIANT_TYPE_STRING) {
        VARIANT_LOG("[variant_get_string]: rejected (actual=%s)", variant_type_name(v));
        return false;
    }
    *out = v->as.s;
    return true;
}

/**
 * @brief Borrow the variant's CUSTOM payload and (optionally) its descriptor.
 *
 * On success `*out` points to the variant's owned storage; if
 * @p out_info is non-NULL it also receives the descriptor that was
 * used to construct the value. As with `variant_get_string`, the
 * borrowed pointer is invalidated by any subsequent operation that
 * mutates the variant.
 *
 * @param v         Variant. May be NULL.
 * @param out       Destination pointer-to-pointer. May be NULL (returns false).
 * @param out_info  Optional descriptor output. May be NULL.
 * @return true iff @p v is CUSTOM and @p out is non-NULL.
 */
bool variant_get_custom(const Variant* v, void** out, const VariantTypeInfo** out_info) {
    if (!v || !out || v->type != VARIANT_TYPE_CUSTOM) {
        VARIANT_LOG("[variant_get_custom]: rejected (actual=%s)", variant_type_name(v));
        return false;
    }
    *out = v->as.custom.data;
    if (out_info) {
        *out_info = v->as.custom.info;
    }
    return true;
}

/* --------------------------------------------------------------------
 *  Comparison / hash
 * -------------------------------------------------------------------- */

static int cmp_prim_int(int a, int b)       { return (a > b) - (a < b); }
static int cmp_prim_long(long a, long b)    { return (a > b) - (a < b); }
static int cmp_prim_float(float a, float b)
        { return a < b ? -1 : a > b ? 1 : 0; }
static int cmp_prim_double(double a, double b)
        { return a < b ? -1 : a > b ? 1 : 0; }
static int cmp_prim_char(char a, char b)    { return (a > b) - (a < b); }
static int cmp_prim_bool(bool a, bool b)    { return (int)a - (int)b; }
static int cmp_prim_ptr(const void* a, const void* b)
        { return (a > b) - (a < b); }

/**
 * @brief Strcmp-like compare. Same-tag values are compared by value;
 *        different-tag values are compared by tag (so NONE < INT <
 *        LONG < … gives a stable order).
 *
 *        NULL inputs sort before any non-NULL.
 */
int variant_compare(const Variant* a, const Variant* b) {
    if (!a && !b) {
        return 0;
    }
    if (!a) {
        return -1;
    }
    if (!b) {
        return  1;
    }

    VARIANT_LOG("[variant_compare]: comparing %s vs %s", variant_type_name(a), variant_type_name(b));
    if (a->type != b->type) {
        return (int)a->type - (int)b->type;
    }
    switch (a->type) {
        case VARIANT_TYPE_NONE:    
            return 0;
        case VARIANT_TYPE_INT:     
            return cmp_prim_int   (a->as.i, b->as.i);
        case VARIANT_TYPE_LONG:    
            return cmp_prim_long  (a->as.l, b->as.l);
        case VARIANT_TYPE_FLOAT:   
            return cmp_prim_float (a->as.f, b->as.f);
        case VARIANT_TYPE_DOUBLE:  
            return cmp_prim_double(a->as.d, b->as.d);
        case VARIANT_TYPE_CHAR:    
            return cmp_prim_char  (a->as.c, b->as.c);
        case VARIANT_TYPE_BOOL:    
            return cmp_prim_bool  (a->as.b, b->as.b);
        case VARIANT_TYPE_POINTER: 
            return cmp_prim_ptr   (a->as.p, b->as.p);
        case VARIANT_TYPE_STRING: {
            const char* sa = a->as.s, *sb = b->as.s;
            if (!sa && !sb) {
                return 0;
            }
            if (!sa) {
                return -1;
            }
            if (!sb) {
                return  1;
            }
            return strcmp(sa, sb);
        }
        case VARIANT_TYPE_CUSTOM:
            if (a->as.custom.info != b->as.custom.info) {
                return 0;
            }
            if (a->as.custom.info && a->as.custom.info->compare) {
                return a->as.custom.info->compare(a->as.custom.data, b->as.custom.data);
            }
            if (a->as.custom.info) {
                return memcmp(a->as.custom.data, b->as.custom.data, a->as.custom.info->size);
            }
            return 0;
    }
    return 0;
}

/**
 * @brief Convenience equality check.
 *
 * `true` exactly when `variant_compare(a, b) == 0`. Two NULLs are equal;
 * two NONEs are equal; two STRINGs are byte-identical iff their
 * underlying buffers compare equal under `strcmp`; two CUSTOMs are
 * equal iff they share the same `VariantTypeInfo` AND either the
 * descriptor's `compare` hook returns 0 OR `memcmp` over `info->size`
 * bytes returns 0.
 *
 * @param a, b  Variants to compare. Either may be NULL.
 */
bool variant_equal(const Variant* a, const Variant* b) {
    return variant_compare(a, b) == 0;
}

/* FNV-1a 64 reduced to size_t. */
static size_t fnv1a(const void* data, size_t n, size_t seed) {
    const unsigned char* p = (const unsigned char*)data;
    size_t hash = seed ^ (size_t)0xcbf29ce484222325ULL;
    while (n--) {
        hash ^= *p++;
        hash *= (size_t)0x100000001b3ULL;
    }
    return hash;
}

/** @brief Hash suitable for use as a hashmap key. */
size_t variant_hash(const Variant* v) {
    if (!v) {
        VARIANT_LOG("[variant_hash]: NULL receiver, returning 0");
        return 0;
    }
    VARIANT_LOG("[variant_hash]: hashing %s variant", variant_type_name(v));
    size_t seed = (size_t)v->type;
    switch (v->type) {
        case VARIANT_TYPE_NONE:    
            return seed;
        case VARIANT_TYPE_INT:     
            return fnv1a(&v->as.i, sizeof v->as.i, seed);
        case VARIANT_TYPE_LONG:    
            return fnv1a(&v->as.l, sizeof v->as.l, seed);
        case VARIANT_TYPE_FLOAT:   
            return fnv1a(&v->as.f, sizeof v->as.f, seed);
        case VARIANT_TYPE_DOUBLE:  
            return fnv1a(&v->as.d, sizeof v->as.d, seed);
        case VARIANT_TYPE_CHAR:    
            return fnv1a(&v->as.c, sizeof v->as.c, seed);
        case VARIANT_TYPE_BOOL:    
            return fnv1a(&v->as.b, sizeof v->as.b, seed);
        case VARIANT_TYPE_POINTER: 
            return fnv1a(&v->as.p, sizeof v->as.p, seed);
        case VARIANT_TYPE_STRING:  
            return v->as.s ? fnv1a(v->as.s, strlen(v->as.s), seed) : seed;
        case VARIANT_TYPE_CUSTOM:
            if (v->as.custom.info && v->as.custom.data) {
                return fnv1a(v->as.custom.data, v->as.custom.info->size, seed);
            }
            return seed;
    }
    return seed;
}

/* --------------------------------------------------------------------
 *  Visitor
 * -------------------------------------------------------------------- */

/**
 * @brief Invoke @p fn on @p v. Returns whatever @p fn returns
 *        (typically 0 to continue, non-zero to stop in
 *        variant_visit_all).
 */
int variant_visit(const Variant* v, VariantVisitFn fn, void* user_data) {
    if (!v || !fn) {
        VARIANT_LOG("[variant_visit]: NULL arg, returning 0");
        return 0;
    }
    VARIANT_LOG("[variant_visit]: visiting %s variant", variant_type_name(v));
    return fn(v, user_data);
}

/**
 * @brief Visit every entry of @p arr in order, stopping early if @p
 *        fn returns non-zero. Returns the first non-zero result or 0.
 */
int variant_visit_all(const Variant* arr, size_t count,
                      VariantVisitFn fn, void* user_data) {
    if (!arr || !fn) {
        VARIANT_LOG("[variant_visit_all]: NULL arg, returning 0");
        return 0;
    }
    VARIANT_LOG("[variant_visit_all]: visiting %zu variants", count);
    for (size_t i = 0; i < count; ++i) {
        int rc = fn(&arr[i], user_data);
        if (rc != 0) {
            VARIANT_LOG("[variant_visit_all]: short-circuiting at index %zu (rc=%d)", i, rc);
            return rc;
        }
    }
    return 0;
}


static char* alloc_sprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (n < 0) return NULL;
    char* out = (char*)malloc((size_t)n + 1);
    if (!out) return NULL;
    va_start(args, fmt);
    vsnprintf(out, (size_t)n + 1, fmt, args);
    va_end(args);
    return out;
}


/**
 * @brief Heap-allocated rendering of the variant. Caller frees().
 *        Uses quoting for string and 'x' for char so different types
 *        round-trip distinctly.
 */
char* variant_to_string(const Variant* v) {
    if (!v) {
        VARIANT_LOG("[variant_to_string]: NULL receiver, returning \"none\"");
        return dup_cstr("none");
    }
    VARIANT_LOG("[variant_to_string]: rendering %s variant", variant_type_name(v));
    switch (v->type) {
        case VARIANT_TYPE_NONE:    
            return dup_cstr("none");
        case VARIANT_TYPE_INT:     
            return alloc_sprintf("%d", v->as.i);
        case VARIANT_TYPE_LONG:    
            return alloc_sprintf("%ld", v->as.l);
        case VARIANT_TYPE_FLOAT:   
            return alloc_sprintf("%g", (double)v->as.f);
        case VARIANT_TYPE_DOUBLE:  
            return alloc_sprintf("%g", v->as.d);
        case VARIANT_TYPE_CHAR:    
            return alloc_sprintf("'%c'", v->as.c);
        case VARIANT_TYPE_BOOL:    
            return dup_cstr(v->as.b ? "true" : "false");
        case VARIANT_TYPE_POINTER: 
            return alloc_sprintf("%p", v->as.p);
        case VARIANT_TYPE_STRING:
            return v->as.s ? alloc_sprintf("\"%s\"", v->as.s) : dup_cstr("null");
        case VARIANT_TYPE_CUSTOM:
            if (v->as.custom.info && v->as.custom.info->print) {
                /* Ask the custom printer for the exact length first. */
                int n = v->as.custom.info->print(NULL, 0, v->as.custom.data);
                if (n < 0) {
                    n = 0;
                }

                char* out = (char*)malloc((size_t)n + 1);
                if (!out) { 
                    return NULL;
                }
                v->as.custom.info->print(out, (size_t)n + 1, v->as.custom.data);

                return out;
            }
            return alloc_sprintf("<%s>", v->as.custom.info && v->as.custom.info->type_name ? v->as.custom.info->type_name : "custom");
    }
    return dup_cstr("unknown");
}


/**
 * @brief Print @p v to stdout using `variant_to_string`'s formatting.
 *
 * No trailing newline. Internally allocates a heap buffer via
 * `variant_to_string`, writes it, and frees it.
 *
 * @param v  Variant. May be NULL (prints "none").
 * @return Number of characters written, or `-1` on allocation failure.
 */
int variant_print(const Variant* v) {
    char* s = variant_to_string(v);
    if (!s) {
        return -1;
    }
    int n = printf("%s", s);
    free(s);
    return n;
}


/**
 * @brief Like `variant_print` but appends a newline.
 *
 * @param v  Variant. May be NULL.
 * @return Total chars written (including the newline), or `-1` on error.
 */
int variant_println(const Variant* v) {
    int n = variant_print(v);
    if (n < 0) {
        return n;
    }

    return n + printf("\n");
}


struct VariantArray {
    Variant* data;
    size_t   size;
    size_t   cap;
};


/**
 * @brief Allocate a new dynamic array of variants. Initial capacity
 *        of 0 is fine; the array grows on first push.
 */
VariantArray* variant_array_new(size_t initial_capacity) {
    VariantArray* a = (VariantArray*)malloc(sizeof(VariantArray));
    if (!a) {
        VARIANT_LOG("[variant_array_new]: OOM on header");
        return NULL;
    }
    a->data = NULL;
    a->size = 0;
    a->cap  = 0;

    if (initial_capacity > 0) {
        a->data = (Variant*)malloc(initial_capacity * sizeof(Variant));
        if (!a->data) {
            VARIANT_LOG("[variant_array_new]: OOM on %zu-slot data", initial_capacity);
            free(a);
            return NULL;
        }
        a->cap = initial_capacity;
    }
    VARIANT_LOG("[variant_array_new]: created (initial_capacity=%zu)", initial_capacity);

    return a;
}


/**
 * @brief Free every variant in the array and the array itself.
 *        NULL-safe.
 */
void variant_array_free(VariantArray* a) {
    if (!a) {
        VARIANT_LOG("[variant_array_free]: NULL, no-op");
        return;
    }

    VARIANT_LOG("[variant_array_free]: freeing %zu elements", a->size);
    for (size_t i = 0; i < a->size; ++i) {
        variant_destroy(&a->data[i]);
    }

    free(a->data);
    free(a);
}


/**
 * @brief Make sure the array can hold at least @p min_capacity
 *        without further reallocation.
 */
bool variant_array_reserve(VariantArray* a, size_t min_capacity) {
    if (!a) {
        VARIANT_LOG("[variant_array_reserve]: NULL array");
        return false;
    }
    if (a->cap >= min_capacity) {
        return true;
    }

    size_t new_cap = a->cap ? a->cap : 4;
    while (new_cap < min_capacity) {
        new_cap *= 2;
    }

    Variant* nd = (Variant*)realloc(a->data, new_cap * sizeof(Variant));
    if (!nd) {
        VARIANT_LOG("[variant_array_reserve]: realloc to %zu failed", new_cap);
        return false;
    }

    VARIANT_LOG("[variant_array_reserve]: grew capacity %zu -> %zu", a->cap, new_cap);
    a->data = nd;
    a->cap  = new_cap;

    return true;
}


/** @brief Append a deep copy of @p v to the array. */
bool variant_array_push(VariantArray* a, const Variant* v) {
    if (!a || !v) {
        VARIANT_LOG("[variant_array_push]: NULL arg");
        return false;
    }
    if (!variant_array_reserve(a, a->size + 1)) {
        return false;
    }

    a->data[a->size] = variant_clone(v);
    a->size++;

    VARIANT_LOG("[variant_array_push]: pushed %s, size now %zu", variant_type_name(v), a->size);
    return true;
}


/** @brief Drop the last element. False on empty/NULL. */
bool variant_array_pop(VariantArray* a) {
    if (!a || a->size == 0) {
        VARIANT_LOG("[variant_array_pop]: empty/NULL");
        return false;
    }
    variant_destroy(&a->data[a->size - 1]);
    a->size--;

    VARIANT_LOG("[variant_array_pop]: popped, size now %zu", a->size);
    return true;
}


/** @brief Borrowed pointer to element at @p index. NULL if OOR. */
Variant* variant_array_at(VariantArray* a, size_t index) {
    if (!a || index >= a->size) {
        VARIANT_LOG("[variant_array_at]: out-of-range (index=%zu size=%zu)", index, a ? a->size : (size_t)0);
        return NULL;
    }
    return &a->data[index];
}

/** @brief Replace element at @p index with a deep copy of @p v. */
bool variant_array_set(VariantArray* a, size_t index, const Variant* v) {
    if (!a || !v || index >= a->size) {
        VARIANT_LOG("[variant_array_set]: bad arg (a=%p v=%p index=%zu)",
                    (void*)a, (const void*)v, index);
        return false;
    }
    Variant copy = variant_clone(v);
    variant_destroy(&a->data[index]);
    a->data[index] = copy;
    VARIANT_LOG("[variant_array_set]: replaced index %zu with %s",
                index, variant_type_name(v));
    return true;
}


/** @brief Remove element at @p index, shifting later elements left. */
bool variant_array_remove_at(VariantArray* a, size_t index) {
    if (!a || index >= a->size) {
        VARIANT_LOG("[variant_array_remove_at]: out-of-range (index=%zu size=%zu)",
                    index, a ? a->size : (size_t)0);
        return false;
    }

    variant_destroy(&a->data[index]);
    memmove(&a->data[index], &a->data[index + 1], (a->size - index - 1) * sizeof(Variant));
    a->size--;

    VARIANT_LOG("[variant_array_remove_at]: removed index %zu, size now %zu", index, a->size);
    return true;
}


/** @brief Destroy every element; the array becomes empty. */
void variant_array_clear(VariantArray* a) {
    if (!a) {
        VARIANT_LOG("[variant_array_clear]: NULL");
        return;
    }
    VARIANT_LOG("[variant_array_clear]: clearing %zu elements", a->size);
    for (size_t i = 0; i < a->size; ++i) {
        variant_destroy(&a->data[i]);
    }

    a->size = 0;
}


/**
 * @brief Current element count.
 * @param a  Array. May be NULL.
 * @return Number of elements; 0 if @p a is NULL.
 */
size_t variant_array_size(const VariantArray* a) {
    return a ? a->size : 0;
}


/**
 * @brief True iff the array is NULL or holds no elements.
 *
 * Capacity is not consulted — an array with cap=64 but size=0 is
 * empty. Use `variant_array_capacity` if you need to distinguish.
 *
 * @param a  Array. May be NULL.
 */
bool variant_array_empty(const VariantArray* a) {
    return !a || a->size == 0;
}


/**
 * @brief Current capacity (slots allocated, not in use).
 *
 * Always satisfies `capacity >= size`. Grows automatically on push;
 * `variant_array_clear` does NOT shrink capacity.
 *
 * @param a  Array. May be NULL.
 * @return Capacity in slots; 0 if @p a is NULL.
 */
size_t variant_array_capacity(const VariantArray* a) {
    return a ? a->cap : 0;
}



static void copy_mem_int(void* d, const void* s) { 
    *(int*)d    = *(const int*)s;    
}


static void copy_mem_long(void* d, const void* s) { 
    *(long*)d   = *(const long*)s;   
}


static void copy_mem_float(void* d, const void* s) { 
    *(float*)d  = *(const float*)s;  
}


static void copy_mem_double(void* d, const void* s) { 
    *(double*)d = *(const double*)s; 
}


static void copy_mem_char(void* d, const void* s) { 
    *(char*)d   = *(const char*)s;   
}


static void copy_mem_bool(void* d, const void* s) { 
    *(bool*)d   = *(const bool*)s;   
}


static void copy_mem_ptr(void* d, const void* s) { 
    *(void**)d  = *(void* const*)s;
}


static void copy_mem_string(void* d, const void* s) {
    const char* src = *(const char* const*)s;
    *(char**)d = src ? dup_cstr(src) : NULL;
}


static void destroy_mem_string(void* p) { 
    free(*(char**)p);
}


static int cmp_mem_int(const void* a, const void* b) { 
    return cmp_prim_int(*(const int*)a, *(const int*)b);    
}


static int cmp_mem_long(const void* a, const void* b) { 
    return cmp_prim_long(*(const long*)a, *(const long*)b);   
}


static int cmp_mem_float(const void* a, const void* b) { 
    return cmp_prim_float(*(const float*)a, *(const float*)b);  
}


static int cmp_mem_double(const void* a, const void* b) { 
    return cmp_prim_double(*(const double*)a, *(const double*)b); 
}


static int cmp_mem_char(const void* a, const void* b) { 
    return cmp_prim_char(*(const char*)a, *(const char*)b);   
}


static int cmp_mem_bool(const void* a, const void* b) { 
    return cmp_prim_bool(*(const bool*)a, *(const bool*)b);   
}


static int cmp_mem_ptr(const void* a, const void* b) { 
    return cmp_prim_ptr(*(void* const*)a, *(void* const*)b);
}


static int cmp_mem_string(const void* a, const void* b) {
    const char* sa = *(const char* const*)a;
    const char* sb = *(const char* const*)b;
    if (!sa && !sb) {
        return 0;
    }
    if (!sa) {
        return -1;
    }
    if (!sb) {
        return  1;
    }
    return strcmp(sa, sb);
}

static int print_mem_int(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "%d",  *(const int*)p);    
}


static int print_mem_long(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "%ld", *(const long*)p);   
}


static int print_mem_float(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "%g",  (double)*(const float*)p); 
}


static int print_mem_double(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "%g",  *(const double*)p); 
}


static int print_mem_char(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "'%c'", *(const char*)p);  
}


static int print_mem_bool(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "%s",  *(const bool*)p ? "true" : "false");
}


static int print_mem_ptr(char* buf, size_t n, const void* p) { 
    return snprintf(buf, n, "%p",  *(void* const*)p);
}


static int print_mem_string(char* buf, size_t n, const void* p) {
    const char* s = *(const char* const*)p;
    return snprintf(buf, n, "\"%s\"", s ? s : "");
}


const VariantTypeInfo variant_typeinfo_int     = { sizeof(int),    copy_mem_int,    NULL,                cmp_mem_int,    print_mem_int,    "int"     };
const VariantTypeInfo variant_typeinfo_long    = { sizeof(long),   copy_mem_long,   NULL,                cmp_mem_long,   print_mem_long,   "long"    };
const VariantTypeInfo variant_typeinfo_float   = { sizeof(float),  copy_mem_float,  NULL,                cmp_mem_float,  print_mem_float,  "float"   };
const VariantTypeInfo variant_typeinfo_double  = { sizeof(double), copy_mem_double, NULL,                cmp_mem_double, print_mem_double, "double"  };
const VariantTypeInfo variant_typeinfo_char    = { sizeof(char),   copy_mem_char,   NULL,                cmp_mem_char,   print_mem_char,   "char"    };
const VariantTypeInfo variant_typeinfo_bool    = { sizeof(bool),   copy_mem_bool,   NULL,                cmp_mem_bool,   print_mem_bool,   "bool"    };
const VariantTypeInfo variant_typeinfo_pointer = { sizeof(void*),  copy_mem_ptr,    NULL,                cmp_mem_ptr,    print_mem_ptr,    "pointer" };
const VariantTypeInfo variant_typeinfo_string  = { sizeof(char*),  copy_mem_string, destroy_mem_string,  cmp_mem_string, print_mem_string, "string"  };
