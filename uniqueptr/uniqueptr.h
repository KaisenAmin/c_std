/**
 * @author Amin Tahmasebi
 * @class UniquePtr
 * @date 2026
 *
 * Minimal, memory-safe, std::unique_ptr-shaped wrapper for C.
 *
 * Design rules:
 *   - One owner per pointer. When the UniquePtr is destroyed (or
 *     reassigned), the registered deleter runs exactly once.
 *   - Move-only by convention. Use uptr_move / uptr_take to transfer
 *     ownership; copying a UniquePtr struct does NOT duplicate
 *     ownership (the C compiler can't stop you, but the result would
 *     double-free).
 *   - No global state. No errno. NULL-safe everywhere.
 *   - No "unsafe" getters. There is only `uptr_get`; it returns the
 *     raw pointer for you to dereference. If you want exclusive
 *     ownership, call `uptr_release`.
 *   - Optional RAII via `UPTR_AUTO` (uses GCC/Clang cleanup attribute).
 *
 * std::unique_ptr ↔ UniquePtr cheat sheet:
 *
 *   std::unique_ptr<T> p(new T(...))   ↔  UniquePtr p = uptr_new(ptr, fn)
 *   ~unique_ptr                         ↔  uptr_destroy(&p)
 *   p.get()                             ↔  uptr_get(&p)
 *   p.release()                         ↔  uptr_release(&p)
 *   p.reset(q)                          ↔  uptr_reset(&p, q)
 *   p.swap(q)                           ↔  uptr_swap(&p, &q)
 *   std::move(p)                        ↔  uptr_move(&dst, &p)
 *   auto q = std::move(p)               ↔  UniquePtr q = uptr_take(&p)
 *   if (p)                              ↔  !uptr_is_null(&p)
 */

#ifndef UNIQUEPTR_H
#define UNIQUEPTR_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define UNIQUEPTR_LOGGING_ENABLE */

#ifdef UNIQUEPTR_LOGGING_ENABLE
    #define UPTR_LOG(fmt, ...) \
        do { fprintf(stderr, "[UNIQUEPTR] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define UPTR_LOG(...) do { } while (0)
#endif


/**
 * @brief Deleter signature. Any function with this shape is a valid
 *        deleter (think `delete` for the underlying type). The
 *        argument is the raw pointer the UniquePtr owns.
 *
 * Passing NULL as the deleter to uptr_new uses free() as a default.
 */
typedef void (*UniquePtrDeleter)(void* ptr);

/**
 * @brief A move-only owning pointer.
 *
 * Two fields. That's the whole shape. Construct with uptr_new (or
 * uptr_none), destroy with uptr_destroy (or let UPTR_AUTO do it for
 * you).
 */
typedef struct {
    void*            ptr;
    UniquePtrDeleter deleter;
} UniquePtr;



/* ------------------------------------------------------------------ */
/* Construction (incl. move-construct)                                */
/* ------------------------------------------------------------------ */

UniquePtr        uptr_new               (void* ptr, UniquePtrDeleter deleter);
UniquePtr        uptr_none              (void);
UniquePtr        uptr_take              (UniquePtr* src);


/* ------------------------------------------------------------------ */
/* Ownership lifecycle                                                */
/* ------------------------------------------------------------------ */

void             uptr_destroy           (UniquePtr* up);
void             uptr_reset             (UniquePtr* up, void* new_ptr);
void             uptr_swap              (UniquePtr* a, UniquePtr* b);
void             uptr_move              (UniquePtr* dst, UniquePtr* src);


/* ------------------------------------------------------------------ */
/* Accessors                                                          */
/* ------------------------------------------------------------------ */

void*            uptr_get               (const UniquePtr* up);
void*            uptr_release           (UniquePtr* up);
UniquePtrDeleter uptr_get_deleter       (const UniquePtr* up);
bool             uptr_is_null           (const UniquePtr* up);



/* ===== RAII via GCC/Clang cleanup attribute ======================= */
/* Putting UPTR_AUTO in front of a UniquePtr local makes the compiler
 * call uptr_destroy on it at the end of its enclosing scope.
 * Works on GCC and Clang; falls back to nothing on compilers that
 * don't support the attribute (you'll need to call uptr_destroy
 * yourself in that case). */
#if defined(__GNUC__) || defined(__clang__)
    #define UPTR_AUTO __attribute__((cleanup(uptr_destroy)))
#else
    #define UPTR_AUTO  /* no RAII; remember to uptr_destroy(&p) manually */
#endif



#ifdef __cplusplus
}
#endif

#endif 
