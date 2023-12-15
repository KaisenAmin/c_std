#include "stack.h"

#include <stdlib.h>

static bool stack_is_equal_impl(const Stack* stk1, const Stack* stk2);
static bool stack_is_less_impl(const Stack* stk1, const Stack* stk2);
static bool stack_is_greater_impl(const Stack* stk1, const Stack* stk2);
static bool stack_is_less_or_equal_impl(const Stack* stk1, const Stack* stk2);
static bool stack_is_greater_or_equal_impl(const Stack* stk1, const Stack* stk2);
static bool stack_is_not_equal_impl(const Stack* stk1, const Stack* stk2);
static bool stack_empty_impl(Stack* stk);
static size_t stack_size_impl(Stack* stk);
static void* stack_top_impl(Stack* stk);
static void* stack_pop_impl(Stack* stk);
static void stack_push_impl(Stack* stk, void* item);
static void stack_emplace_impl(Stack* stk, void* item);
static void stack_clear_impl(Stack* stk);
static void stack_swap_impl(Stack* stk1, Stack* stk2);
static void stack_deallocate_impl(Stack* stk);


Stack* stack_create(size_t itemSize) 
{
    Stack* stk = (Stack*)malloc(sizeof(Stack));
    if (!stk) 
        return NULL; // Allocation failed
    
    stk->vec = vector_create(itemSize);

    if (!stk->vec) 
    {
        free(stk);
        return NULL; // Vector creation failed
    }

    // Assign function pointers
    stk->is_equal = stack_is_equal_impl;
    stk->is_less = stack_is_less_impl;
    stk->is_greater = stack_is_greater_impl;
    stk->is_less_or_equal = stack_is_less_or_equal_impl;
    stk->is_greater_or_equal = stack_is_greater_or_equal_impl;
    stk->is_not_equal = stack_is_not_equal_impl;
    stk->empty = stack_empty_impl;
    stk->size = stack_size_impl;
    stk->top = stack_top_impl;
    stk->pop = stack_pop_impl;
    stk->push = stack_push_impl;
    stk->emplace = stack_emplace_impl;
    stk->clear = stack_clear_impl;
    stk->swap = stack_swap_impl;
    stk->deallocate = stack_deallocate_impl;

    return stk;
}

static bool stack_is_equal_impl(const Stack* stk1, const Stack* stk2) 
{
    if (!stk1 || !stk2) 
        return false;

    return stk1->vec->is_equal(stk1->vec, stk2->vec);
}

static bool stack_is_less_impl(const Stack* stk1, const Stack* stk2) 
{
    if (!stk1 || !stk2) 
        return false;

    return stk1->vec->is_less(stk1->vec, stk2->vec);
}

static bool stack_is_greater_impl(const Stack* stk1, const Stack* stk2) 
{
    if (!stk1 || !stk2) 
        return false;

    return stk1->vec->is_greater(stk1->vec, stk2->vec);
}

static bool stack_is_less_or_equal_impl(const Stack* stk1, const Stack* stk2) 
{
    return stk1->is_less(stk1, stk2) || stk1->is_equal(stk1, stk2);
}

static bool stack_is_greater_or_equal_impl(const Stack* stk1, const Stack* stk2) 
{
    return stk1->is_greater(stk1, stk2) || stk1->is_equal(stk1, stk2);
}

static bool stack_is_not_equal_impl(const Stack* stk1, const Stack* stk2) 
{
    return !stk1->is_equal(stk1, stk2);
}

static void stack_push_impl(Stack* stk, void* item) 
{
    if (stk == NULL || item == NULL) 
        return; // Invalid input
    
    stk->vec->push_back(stk->vec, item);
}

static void* stack_pop_impl(Stack* stk) 
{
    if (stk == NULL || stk->vec->is_empty(stk->vec)) 
        return NULL; // Stack is empty or NULL
    
    return stk->vec->pop_back(stk->vec);
}

static void* stack_top_impl(Stack* stk) 
{
    if (stk == NULL || stk->vec->is_empty(stk->vec)) 
        return NULL; // Stack is empty or NULL
    
    return stk->vec->back(stk->vec);
}

static size_t stack_size_impl(Stack* stk) 
{
    if (stk == NULL) 
        return 0;
    
    return stk->vec->length(stk->vec);
}

static bool stack_empty_impl(Stack* stk) 
{
    if (stk == NULL) 
        return true; // Consider a NULL stack as empty
    
    return stk->vec->is_empty(stk->vec);
}

static void stack_emplace_impl(Stack* stk, void* item) 
{
    if (stk == NULL || item == NULL) 
        return; // Invalid input
    
    stk->vec->emplace_back(stk->vec, item, stk->vec->itemSize);
}

static void stack_clear_impl(Stack* stk) 
{
    if (stk == NULL) 
        return; // Nothing to clear if stack is NULL
    
    stk->vec->clear(stk->vec);
}

static void stack_swap_impl(Stack* stk1, Stack* stk2) 
{
    if (stk1 == NULL || stk2 == NULL) 
        return; // Cannot swap NULL stacks
    
    Vector* tempVec = stk1->vec;
    stk1->vec = stk2->vec;
    stk2->vec = tempVec;
}

static void stack_deallocate_impl(Stack* stk) 
{
    if (stk == NULL) 
        return; // No need to deallocate a NULL stack
    
    if (stk->vec != NULL) 
        stk->vec->deallocate(stk->vec); // Deallocate the underlying vector
    
    free(stk); // Free the stack itself
}
