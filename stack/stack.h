/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Stack
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in stack.c (file-level convention).
 *
 * A C analog of C++ std::stack<T>, layered on top of the project's Vector.
 *
 * Pointer-lifetime warning for stack_pop():
 *   The pointer returned by stack_pop() aliases storage that is still
 *   owned by the underlying vector. It is valid only until the NEXT
 *   modifying operation on the stack (push / emplace / clear / reserve /
 *   deallocate). For safer extraction, use stack_pop_value().
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "../vector/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/* #define STACK_LOGGING_ENABLE */

#ifdef STACK_LOGGING_ENABLE
    #define STACK_LOG(fmt, ...) \
        do { fprintf(stderr, "[STACK LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define STACK_LOG(...) do { } while (0)
#endif


typedef struct Stack Stack;

struct Stack {
    Vector* vec;
};


/* ------------------------------------------------------------------ */
/* Inlined hot ops                                                    */
/* ------------------------------------------------------------------ */
/*
 * A stack is LIFO at the back of the vector, so push / top / pop / empty / size
 * are tiny O(1) operations that dominate any real workload. They are defined
 * `static inline` here (the Vector struct and its inline push_back/at are
 * visible, and <string.h> comes in via vector.h) so the call folds into the
 * caller exactly like std::stack — no out-of-line wrapper around an already
 * inline vector op. Pops adjust the vector's size directly.
 */
static inline size_t stack_size(const Stack* stk) {
    return (stk && stk->vec) ? stk->vec->size : 0;
}


static inline bool stack_empty(const Stack* stk) {
    return !stk || !stk->vec || stk->vec->size == 0;
}


static inline void* stack_top(const Stack* stk) {
    if (!stk || !stk->vec || stk->vec->size == 0) {
        return NULL;
    }
    return (char*)stk->vec->items + (stk->vec->size - 1) * stk->vec->itemSize;
}


static inline bool stack_push(Stack* stk, const void* item) {
    if (!stk || !stk->vec || !item) {
        return false;
    }
    return vector_push_back(stk->vec, item);
}


static inline bool stack_pop_void(Stack* stk) {
    if (!stk || !stk->vec || stk->vec->size == 0) {
        return false;
    }
    stk->vec->size--;
    return true;
}


static inline bool stack_pop_value(Stack* stk, void* out_buf) {
    if (!stk || !stk->vec || stk->vec->size == 0) {
        return false;
    }
    stk->vec->size--;
    if (out_buf) {
        memcpy(out_buf,
               (char*)stk->vec->items + stk->vec->size * stk->vec->itemSize,
               stk->vec->itemSize);
    }
    return true;
}


static inline void* stack_pop(Stack* stk) {
    if (!stk || !stk->vec || stk->vec->size == 0) {
        return NULL;
    }
    stk->vec->size--;
    return (char*)stk->vec->items + stk->vec->size * stk->vec->itemSize;
}


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Stack*  stack_create                  (size_t itemSize);
Stack*  stack_copy                    (const Stack* src);


/* ------------------------------------------------------------------ */
/* Destruction                                                        */
/* ------------------------------------------------------------------ */

void    stack_clear                   (Stack* stk);
void    stack_deallocate              (Stack* stk);


/* ------------------------------------------------------------------ */
/* Capacity / size                                                    */
/* ------------------------------------------------------------------ */

size_t  stack_capacity                (const Stack* stk);
size_t  stack_item_size               (const Stack* stk);
bool    stack_reserve                 (Stack* stk, size_t new_capacity);
/* stack_size / stack_empty are defined static inline above. */


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

void*   stack_emplace                 (Stack* stk, const void* item);
bool    stack_assign                  (Stack* dest, const Stack* src);
void    stack_swap                    (Stack* stk1, Stack* stk2);
/* stack_push / stack_pop / stack_pop_void / stack_pop_value are defined
 * static inline above. stack_top likewise (element access). */


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool    stack_is_equal                (const Stack* stk1, const Stack* stk2);
bool    stack_is_not_equal            (const Stack* stk1, const Stack* stk2);
bool    stack_is_less                 (const Stack* stk1, const Stack* stk2);
bool    stack_is_less_or_equal        (const Stack* stk1, const Stack* stk2);
bool    stack_is_greater              (const Stack* stk1, const Stack* stk2);
bool    stack_is_greater_or_equal     (const Stack* stk1, const Stack* stk2);


#ifdef __cplusplus
}
#endif

#endif 
