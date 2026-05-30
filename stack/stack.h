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
 * Const-correctness mirrors std::stack:
 *   - Functions that DO NOT modify the stack take `const Stack*`
 *     (size, empty, top, comparisons, capacity, item_size).
 *   - Functions that DO modify the stack take `Stack*`
 *     (push, pop*, emplace, clear, reserve, swap, assign, deallocate).
 *
 * Error model:
 *   - No global error state. No errno-style last-error. Every function
 *     reports failure through its return value (NULL / false / 0).
 *   - Opt-in diagnostic logging via STACK_LOGGING_ENABLE → STACK_LOG.
 *   - No library function ever calls exit().
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
    #define STACK_LOG(fmt, ...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef struct Stack Stack;

struct Stack {
    Vector* vec;
};


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

size_t  stack_size                    (const Stack* stk);
size_t  stack_capacity                (const Stack* stk);
size_t  stack_item_size               (const Stack* stk);
bool    stack_empty                   (const Stack* stk);
bool    stack_reserve                 (Stack* stk, size_t new_capacity);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

bool    stack_push                    (Stack* stk, const void* item);
void*   stack_emplace                 (Stack* stk, const void* item);
void*   stack_pop                     (Stack* stk);                       /* alias, see lifetime warning */
bool    stack_pop_void                (Stack* stk);                       /* drop top without reading    */
bool    stack_pop_value               (Stack* stk, void* out_buf);        /* copy then drop              */
bool    stack_assign                  (Stack* dest, const Stack* src);
void    stack_swap                    (Stack* stk1, Stack* stk2);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

void*   stack_top                     (const Stack* stk);


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
