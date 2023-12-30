#include "stack.h"
#include <stdlib.h>

Stack* stack_create(size_t itemSize) 
{
    Stack* stk = (Stack*)malloc(sizeof(Stack));
    if (!stk)
    {
        perror("Allocation failed in Stack");
        exit(-1);
    }
    
    stk->vec = vector_create(itemSize);

    if (!stk->vec) 
    {
        free(stk);
        exit(-1); // vector creation failed
    }
    return stk;
}

bool stack_is_equal(const Stack* stk1, const Stack* stk2) 
{
    if (!stk1 || !stk2) 
        return false;

    return vector_is_equal(stk1->vec, stk2->vec);
}

bool stack_is_less(const Stack* stk1, const Stack* stk2) 
{
    if (!stk1 || !stk2) 
        return false;

    return vector_is_less(stk1->vec, stk2->vec);
}

bool stack_is_greater(const Stack* stk1, const Stack* stk2) 
{
    if (!stk1 || !stk2) 
        return false;

    return vector_is_greater(stk1->vec, stk2->vec);
}

bool stack_is_less_or_equal(const Stack* stk1, const Stack* stk2) 
{
    return stack_is_less(stk1, stk2) || stack_is_equal(stk1, stk2);
}

bool stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2) 
{
    return stack_is_greater(stk1, stk2) || stack_is_equal(stk1, stk2);
}

bool stack_is_not_equal(const Stack* stk1, const Stack* stk2) 
{
    return !stack_is_equal(stk1, stk2);
}

void stack_push(Stack* stk, void* item) 
{
    if (stk == NULL || item == NULL)
    {
        perror("stack or item is null");
        exit(-1);
    }
        
    vector_push_back(stk->vec, item);
}

void* stack_pop(Stack* stk) 
{
    if (stk == NULL || vector_is_empty(stk->vec)) 
        return NULL; // Stack is empty or NULL
    
    return vector_pop_back(stk->vec);
}

void* stack_top(Stack* stk) 
{
    if (stk == NULL || vector_is_empty(stk->vec)) 
        return NULL; // Stack is empty or NULL
    
    return vector_back(stk->vec);
}

size_t stack_size(Stack* stk) 
{
    if (stk == NULL) 
        return 0;
    
    return vector_size(stk->vec);
}

bool stack_empty(Stack* stk) 
{
    if (stk == NULL) 
        return true; // Consider a NULL stack as empty
    
    return vector_is_empty(stk->vec);
}

void stack_emplace(Stack* stk, void* item) 
{
    if (stk == NULL || item == NULL) 
        return; // Invalid input
    
    vector_emplace_back(stk->vec, item, stk->vec->itemSize);
}

void stack_clear(Stack* stk) 
{
    if (stk == NULL) 
        return; // Nothing to clear if stack is NULL
    
    vector_clear(stk->vec);
}

void stack_swap(Stack* stk1, Stack* stk2) 
{
    if (stk1 == NULL || stk2 == NULL) 
        return; // Cannot swap NULL stacks
    
    Vector* tempVec = stk1->vec;
    stk1->vec = stk2->vec;
    stk2->vec = tempVec;
}

void stack_deallocate(Stack* stk) 
{
    if (stk == NULL) 
        return; // No need to deallocate a NULL stack
    
    if (stk->vec != NULL) 
        vector_deallocate(stk->vec); // Deallocate the underlying vector
    
    free(stk); // Free the stack itself
}
