/**
 * @author Amin Tahmasebi
 * @class UniquePtr
 *
 * Implementation of the UniquePtr module. Doxygen blocks live above
 * each public function.
 */
#include "uniqueptr.h"
#include <stdlib.h>



/**
 * @brief Build a UniquePtr that owns @p ptr and will release it via
 *        @p deleter.
 *
 * If @p deleter is NULL, `free` is used as the default — appropriate
 * for anything allocated with `malloc`/`calloc`/`realloc`. Passing
 * @p ptr == NULL produces an empty UniquePtr (equivalent to
 * `uptr_none()`).
 *
 * @return The new UniquePtr by value. The caller becomes its sole
 *         owner.
 */
UniquePtr uptr_new(void* ptr, UniquePtrDeleter deleter) {
    UPTR_LOG("[uptr_new]: enter ptr=%p deleter=%p (NULL -> free)", ptr, (void*)deleter);
    
    UniquePtr up;
    up.ptr     = ptr;
    up.deleter = deleter ? deleter : free;

    UPTR_LOG("[uptr_new]: exit -> {ptr=%p, deleter=%p}", up.ptr, (void*)up.deleter);
    return up;
}


/**
 * @brief Build an empty UniquePtr. Equivalent to a default-constructed
 *        `std::unique_ptr`.
 */
UniquePtr uptr_none(void) {
    UPTR_LOG("[uptr_none]: enter");

    UniquePtr up;
    up.ptr     = NULL;
    up.deleter = NULL;

    UPTR_LOG("[uptr_none]: exit -> {ptr=NULL, deleter=NULL}");
    return up;
}


/**
 * @brief Release the owned pointer (if any) by running the deleter,
 *        then reset the UniquePtr to empty.
 *
 * Idempotent and NULL-safe — calling it twice or on NULL is a no-op.
 * This is what `UPTR_AUTO` invokes at scope exit.
 */
void uptr_destroy(UniquePtr* up) {
    UPTR_LOG("[uptr_destroy]: enter up=%p", (void*)up);

    if (!up) {
        UPTR_LOG("[uptr_destroy]: NULL receiver, no-op");
        return;
    }
    if (up->ptr && up->deleter) {
        UPTR_LOG("[uptr_destroy]: releasing %p via %p",
                 up->ptr, (void*)up->deleter);
        up->deleter(up->ptr);
    } 
    else {
        UPTR_LOG("[uptr_destroy]: nothing to release (ptr=%p, deleter=%p)", up ? up->ptr : NULL, up ? (void*)up->deleter : NULL);
    }

    up->ptr     = NULL;
    up->deleter = NULL;

    UPTR_LOG("[uptr_destroy]: exit (receiver now empty)");
}


/**
 * @brief Return the raw pointer without giving up ownership.
 *
 * Use this when you want to dereference / pass the pointee but
 * intend to keep the UniquePtr in charge of lifetime.
 *
 * @return The raw pointer, or NULL if empty / NULL receiver.
 */
void* uptr_get(const UniquePtr* up) {
    UPTR_LOG("[uptr_get]: enter up=%p", (const void*)up);
    void* out = up ? up->ptr : NULL;

    UPTR_LOG("[uptr_get]: exit -> %p", out);
    return out;
}


/**
 * @brief Return the registered deleter.
 *
 * @return The deleter, or NULL on NULL/empty receiver.
 */
UniquePtrDeleter uptr_get_deleter(const UniquePtr* up) {
    UPTR_LOG("[uptr_get_deleter]: enter up=%p", (const void*)up);
    UniquePtrDeleter out = up ? up->deleter : NULL;

    UPTR_LOG("[uptr_get_deleter]: exit -> %p", (void*)out);
    return out;
}


/**
 * @brief true if @p up is empty (no owned pointer).
 */
bool uptr_is_null(const UniquePtr* up) {
    UPTR_LOG("[uptr_is_null]: enter up=%p", (const void*)up);
    bool out = (!up || up->ptr == NULL);

    UPTR_LOG("[uptr_is_null]: exit -> %s", out ? "true" : "false");
    return out;
}


/**
 * @brief Relinquish ownership without running the deleter.
 *
 * After this call the UniquePtr is empty and the returned pointer is
 * the caller's responsibility to free.
 *
 * @return The previously-owned pointer (may be NULL).
 */
void* uptr_release(UniquePtr* up) {
    UPTR_LOG("[uptr_release]: enter up=%p", (void*)up);
    if (!up) {
        UPTR_LOG("[uptr_release]: NULL receiver -> NULL");
        return NULL;
    }

    void* p     = up->ptr;
    up->ptr     = NULL;
    up->deleter = NULL;

    UPTR_LOG("[uptr_release]: exit -> handed %p to caller (receiver now empty)", p);
    return p;
}


/**
 * @brief Run the current deleter (if any), then take ownership of
 *        @p new_ptr using the SAME deleter.
 *
 * If you need to change deleters at the same time, do
 * `uptr_destroy(&p); p = uptr_new(q, new_deleter);`. If the receiver
 * had no deleter set yet, `free` is installed so the new pointer is
 * still cleaned up.
 *
 * @param up      Receiver. NULL is a no-op.
 * @param new_ptr Replacement pointer (may be NULL).
 */
void uptr_reset(UniquePtr* up, void* new_ptr) {
    UPTR_LOG("[uptr_reset]: enter up=%p new_ptr=%p", (void*)up, new_ptr);
    if (!up) {
        UPTR_LOG("[uptr_reset]: NULL receiver, no-op");
        return;
    }
    if (up->ptr && up->deleter && up->ptr != new_ptr) {
        UPTR_LOG("[uptr_reset]: releasing old %p before adopting %p",
                 up->ptr, new_ptr);
        up->deleter(up->ptr);
    } 
    else if (up->ptr == new_ptr) {
        UPTR_LOG("[uptr_reset]: new_ptr == old ptr (%p), keeping ownership intact",
                 new_ptr);
    } 
    else {
        UPTR_LOG("[uptr_reset]: receiver was empty; just adopting %p", new_ptr);
    }

    up->ptr = new_ptr;
    if (!up->deleter) {
        UPTR_LOG("[uptr_reset]: no deleter installed; defaulting to free()");
        up->deleter = free;
    }
    UPTR_LOG("[uptr_reset]: exit -> {ptr=%p, deleter=%p}", up->ptr, (void*)up->deleter);
}


/**
 * @brief Swap two UniquePtrs in O(1) with no allocations.
 *
 * NULL-safe; self-swap is a no-op.
 */
void uptr_swap(UniquePtr* a, UniquePtr* b) {
    UPTR_LOG("[uptr_swap]: enter a=%p b=%p", (void*)a, (void*)b);

    if (!a || !b) {
        UPTR_LOG("[uptr_swap]: NULL operand, no-op");
        return;
    }
    if (a == b) {
        UPTR_LOG("[uptr_swap]: self-swap, no-op");
        return;
    }

    UniquePtr t = *a;
    *a = *b;
    *b = t;
    UPTR_LOG("[uptr_swap]: exit (a now owns %p, b now owns %p)", a->ptr, b->ptr);
}


/**
 * @brief Move-assign: destroy @p dst's current payload, then transfer
 *        @p src's payload into @p dst. After the call, @p src is
 *        empty.
 */
void uptr_move(UniquePtr* dst, UniquePtr* src) {
    UPTR_LOG("[uptr_move]: enter dst=%p src=%p", (void*)dst, (void*)src);
    if (!dst || !src) {
        UPTR_LOG("[uptr_move]: NULL operand, no-op");
        return;
    }
    if (dst == src) {
        UPTR_LOG("[uptr_move]: self-move, no-op");
        return;
    }
    uptr_destroy(dst);

    dst->ptr     = src->ptr;
    dst->deleter = src->deleter;
    src->ptr     = NULL;
    src->deleter = NULL;

    UPTR_LOG("[uptr_move]: exit -> dst now owns %p (src is empty)", dst->ptr);
}


/**
 * @brief Move-construct: return a UniquePtr holding @p src's payload;
 *        @p src becomes empty.
 *
 * Mirrors `auto q = std::move(p);` exactly.
 */
UniquePtr uptr_take(UniquePtr* src) {
    UPTR_LOG("[uptr_take]: enter src=%p", (void*)src);
    UniquePtr out = uptr_none();
    if (!src) {
        UPTR_LOG("[uptr_take]: NULL src, returning empty UniquePtr");
        return out;
    }

    out.ptr     = src->ptr;
    out.deleter = src->deleter;
    src->ptr     = NULL;
    src->deleter = NULL;

    UPTR_LOG("[uptr_take]: exit -> new owner has %p (src is empty)", out.ptr);
    return out;
}
