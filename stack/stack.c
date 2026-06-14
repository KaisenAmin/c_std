/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Stack
 *
 * std::stack-style adapter over Vector.
 *
 * Error model:
 *   - No global error state. Failures are reported through the
 *     return value (NULL / false / 0).
 *   - Diagnostics are emitted through STACK_LOG (opt-in via
 *     STACK_LOGGING_ENABLE), exactly like the sibling modules.
 *   - No library function ever calls exit().
 */

#include <stdlib.h>
#include <string.h>
#include "stack.h"


/**
 * @brief Create a new stack whose elements are `itemSize` bytes each.
 * @return Pointer to the new stack, or NULL on bad input / OOM.
 */
Stack* stack_create(size_t itemSize) {
    STACK_LOG("[stack_create]: itemSize=%zu", itemSize);

    if (itemSize == 0) {
        STACK_LOG("[stack_create]: itemSize is zero");
        return NULL;
    }

    Stack* stk = (Stack*)malloc(sizeof(Stack));
    if (!stk) {
        STACK_LOG("[stack_create]: malloc(Stack) failed");
        return NULL;
    }

    stk->vec = vector_create(itemSize);
    if (!stk->vec) {
        STACK_LOG("[stack_create]: vector_create failed");
        free(stk);
        return NULL;
    }

    return stk;
}


/**
 * @brief Deep-copy an existing stack.
 * @return New stack with the same contents, or NULL on failure.
 */
Stack* stack_copy(const Stack* src) {
    STACK_LOG("[stack_copy]: src=%p", (const void*)src);

    if (!src || !src->vec) {
        STACK_LOG("[stack_copy]: NULL source");
        return NULL;
    }

    Stack* dst = stack_create(src->vec->itemSize);
    if (!dst) {
        return NULL;
    }

    size_t n = vector_size(src->vec);
    if (n > 0 && !vector_reserve(dst->vec, n)) {
        STACK_LOG("[stack_copy]: reserve(%zu) failed", n);
        stack_deallocate(dst);

        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        void* elem = vector_at(src->vec, i);
        if (!elem || !vector_push_back(dst->vec, elem)) {
            STACK_LOG("[stack_copy]: push_back failed at index %zu", i);
            stack_deallocate(dst);
            return NULL;
        }
    }

    return dst;
}


/**
 * @brief Replace `dest`'s contents with a deep copy of `src`.
 * @return true on success.
 */
bool stack_assign(Stack* dest, const Stack* src) {
    STACK_LOG("[stack_assign]: dest=%p, src=%p", (void*)dest, (const void*)src);

    if (!dest || !dest->vec || !src || !src->vec) {
        STACK_LOG("[stack_assign]: NULL receiver/source");
        return false;
    }
    if (dest == src)  {
        return true;
    }
    if (dest->vec->itemSize != src->vec->itemSize) {
        STACK_LOG("[stack_assign]: item-size mismatch (%zu vs %zu)",
                  dest->vec->itemSize, src->vec->itemSize);
        return false;
    }

    vector_clear(dest->vec);

    size_t n = vector_size(src->vec);
    if (n > 0 && !vector_reserve(dest->vec, n)) {
        STACK_LOG("[stack_assign]: reserve(%zu) failed", n);
        return false;
    }
    for (size_t i = 0; i < n; ++i) {
        void* elem = vector_at(src->vec, i);
        if (!elem || !vector_push_back(dest->vec, elem)) {
            STACK_LOG("[stack_assign]: push_back failed at index %zu", i);
            return false;
        }
    }

    return true;
}


/**
 * @brief Destroy a stack and release all underlying memory.
 *        Safe to call with NULL (no-op).
 */
void stack_deallocate(Stack* stk) {
    STACK_LOG("[stack_deallocate]: stk=%p", (void*)stk);

    if (!stk) return;
    if (stk->vec) {
        vector_deallocate(stk->vec);
        stk->vec = NULL;
    }
    free(stk);
}


/** @brief Storage capacity of the underlying vector. */
size_t stack_capacity(const Stack* stk) {
    if (!stk || !stk->vec) {
        return 0;
    }
    return vector_capacity((Vector*)stk->vec);
}


/** @brief Size in bytes of each element. */
size_t stack_item_size(const Stack* stk) {
    if (!stk || !stk->vec) {
        return 0;
    }
    return stk->vec->itemSize;
}


/**
 * @brief Like push, but returns a pointer to the newly-installed top
 *        element. NULL on failure.
 */
void* stack_emplace(Stack* stk, const void* item) {
    STACK_LOG("[stack_emplace]: stk=%p item=%p", (void*)stk, item);

    if (!stk || !stk->vec || !item) { 
        return NULL;
    }
    if (!vector_emplace_back(stk->vec, (void*)item, stk->vec->itemSize)) {
        STACK_LOG("[stack_emplace]: vector_emplace_back failed");
        return NULL;
    }

    return vector_back(stk->vec);
}


/** @brief Drop every element; the stack becomes empty. */
void stack_clear(Stack* stk) {
    STACK_LOG("[stack_clear]: stk=%p", (void*)stk);

    if (!stk || !stk->vec) {
        return;
    }
    vector_clear(stk->vec);
}


/** @brief Pre-allocate storage for at least @p new_capacity elements. */
bool stack_reserve(Stack* stk, size_t new_capacity) {
    STACK_LOG("[stack_reserve]: stk=%p cap=%zu", (void*)stk, new_capacity);

    if (!stk || !stk->vec) {
        return false;
    }
    if (!vector_reserve(stk->vec, new_capacity)) {
        return false;
    }

    return true;
}


/**
 * @brief Constant-time swap of two stacks' internals.
 *
 * Rejects (no-op) when item sizes differ, since C++ std::stack::swap
 * is only well-defined between same-type stacks.
 */
void stack_swap(Stack* stk1, Stack* stk2) {
    STACK_LOG("[stack_swap]: stk1=%p stk2=%p", (void*)stk1, (void*)stk2);

    if (!stk1 || !stk2 || !stk1->vec || !stk2->vec) { 
        return;
    }
    if (stk1->vec->itemSize != stk2->vec->itemSize) {
        STACK_LOG("[stack_swap]: refused: item sizes differ (%zu vs %zu)", stk1->vec->itemSize, stk2->vec->itemSize);
        return;
    }

    Vector* tmp = stk1->vec;
    stk1->vec = stk2->vec;
    stk2->vec = tmp;
}


/** @brief true if both stacks hold the same bytes (NULL == NULL). */
bool stack_is_equal(const Stack* stk1, const Stack* stk2) {
    if (stk1 == stk2) {
        return true;
    }
    if (!stk1 || !stk2 || !stk1->vec || !stk2->vec) {
        return false;
    }

    return vector_is_equal(stk1->vec, stk2->vec);
}


/** @brief Inverse of stack_is_equal. */
bool stack_is_not_equal(const Stack* stk1, const Stack* stk2) {
    return !stack_is_equal(stk1, stk2);
}


/** @brief Lexicographic less-than over the underlying vector bytes. */
bool stack_is_less(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2 || !stk1->vec || !stk2->vec) {
        return false;
    }

    return vector_is_less(stk1->vec, stk2->vec);
}


/** @brief Lexicographic greater-than. */
bool stack_is_greater(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2 || !stk1->vec || !stk2->vec) {
        return false;
    }
    return vector_is_greater(stk1->vec, stk2->vec);
}


/** @brief Less-than or equal. */
bool stack_is_less_or_equal(const Stack* stk1, const Stack* stk2) {
    return stack_is_less(stk1, stk2) || stack_is_equal(stk1, stk2);
}


/** @brief Greater-than or equal. */
bool stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2) {
    return stack_is_greater(stk1, stk2) || stack_is_equal(stk1, stk2);
}
