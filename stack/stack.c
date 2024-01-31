/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Stack
*/

#include "stack.h"
#include "../fmt/fmt.h"
#include <stdlib.h>

Stack* stack_create(size_t itemSize) {
    Stack* stk = (Stack*)malloc(sizeof(Stack));
    if (!stk) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Allocation failed for Stack in stack_create.\n");
        #endif 
        exit(-1);
    }
    
    stk->vec = vector_create(itemSize);
    if (!stk->vec) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Vector creation failed in stack_create.\n");
        #endif 
        free(stk);
        exit(-1);
    }
    return stk;
}

bool stack_is_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_equal.\n");
        #endif 
        return false;
    }
    return vector_is_equal(stk1->vec, stk2->vec);
}

bool stack_is_less(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_less.\n");
        #endif 
        return false;
    }
    return vector_is_less(stk1->vec, stk2->vec);
}

bool stack_is_greater(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_greater.\n");
        #endif 
        return false;
    }
    return vector_is_greater(stk1->vec, stk2->vec);
}

bool stack_is_less_or_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_less_or_equal.\n");
        #endif 
        return false;
    }
    return stack_is_less(stk1, stk2) || stack_is_equal(stk1, stk2);
}

bool stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_greater_or_equal.\n");
        #endif 
        return false;
    }
    return stack_is_greater(stk1, stk2) || stack_is_equal(stk1, stk2);
}

bool stack_is_not_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_not_equal.\n");
        #endif 
        return false;
    }
    return !stack_is_equal(stk1, stk2);
}

void stack_push(Stack* stk, void* item) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_push.\n");
        #endif
        exit(-1);
    }
    if (item == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item pointer is NULL in stack_push.\n");
        #endif 
        exit(-1);
    }
    vector_push_back(stk->vec, item);
}

void* stack_pop(Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_pop.\n");
        #endif 
        return NULL;
    }
    if (vector_is_empty(stk->vec)) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Attempting to pop from an empty stack in stack_pop.\n");
        #endif 
        return NULL;
    }
    return vector_pop_back(stk->vec);
}

void* stack_top(Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_top.\n");
        #endif 
        return NULL;
    }
    if (vector_is_empty(stk->vec)) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Attempting to access top of an empty stack in stack_top.\n");
        #endif 
        return NULL;
    }
    return vector_back(stk->vec);
}

size_t stack_size(Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Stack pointer is NULL in stack_size. Returning 0.\n");
        #endif 
        return 0;
    }
    return vector_size(stk->vec);
}

bool stack_empty(Stack* stk) {
    if (stk == NULL) {
        return true; // Consider a NULL stack as empty
    }
    return vector_is_empty(stk->vec);
}

void stack_emplace(Stack* stk, void* item) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_emplace.\n");
        #endif 
        return;
    }
    if (item == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Item pointer is NULL in stack_emplace.\n");
        #endif 
        return;
    }
    vector_emplace_back(stk->vec, item, stk->vec->itemSize);
}

void stack_clear(Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_clear.\n");
        #endif 
        return;
    }
    vector_clear(stk->vec);
}

void stack_swap(Stack* stk1, Stack* stk2) {
    if (stk1 == NULL || stk2 == NULL) {
        fmt_fprintf(stderr, "Error: One or both stack pointers are NULL in stack_swap.\n");
        return;
    }

    Vector* tempVec = stk1->vec;
    stk1->vec = stk2->vec;
    stk2->vec = tempVec;
}

void stack_deallocate(Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_deallocate.\n");
        #endif 
        return;
    }
    if (stk->vec != NULL) { 
        vector_deallocate(stk->vec); // Deallocate the underlying vector
    }
    free(stk); // Free the stack itself
}
