/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Stack
*/

#include "stack.h"
#include "../fmt/fmt.h"
#include <stdlib.h>

/**
 * @brief Creates a new stack.
 *
 * This function allocates memory for a new stack and initializes its underlying vector.
 * If the allocation for the stack or the vector fails, it logs an error and exits the program.
 *
 * @param itemSize Size of each item that will be stored in the stack. This is used to initialize
 * the underlying vector's item size.
 * 
 * @return Pointer to the newly created stack.
 */
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

/**
 * @brief Checks if two stacks are equal.
 *
 * This function compares two stacks and returns true if they are considered equal based on 
 * their underlying vector comparison. If either stack is NULL, it logs an error and returns false.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 * 
 * @return true if the two stacks are equal, false otherwise.
 */
bool stack_is_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_equal.\n");
        #endif 
        return false;
    }
    return vector_is_equal(stk1->vec, stk2->vec);
}

/**
 * @brief Checks if the first stack is less than the second stack.
 *
 * This function compares two stacks and returns true if the first stack is considered less than 
 * the second stack based on their underlying vector comparison. If either stack is NULL, 
 * it logs an error and returns false.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 * 
 * @return true if the first stack is less than the second, false otherwise.
 */
bool stack_is_less(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_less.\n");
        #endif 
        return false;
    }
    return vector_is_less(stk1->vec, stk2->vec);
}

/**
 * @brief Checks if the first stack is greater than the second stack.
 *
 * This function compares two stacks and returns true if the first stack is considered greater than 
 * the second stack based on their underlying vector comparison. If either stack is NULL, 
 * it logs an error and returns false.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 * 
 * @return true if the first stack is greater than the second, false otherwise.
 */
bool stack_is_greater(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_greater.\n");
        #endif 
        return false;
    }
    return vector_is_greater(stk1->vec, stk2->vec);
}

/**
 * @brief Checks if the first stack is less than or equal to the second stack.
 *
 * This function compares two stacks and returns true if the first stack is considered less than 
 * or equal to the second stack based on their underlying vector comparison. If either stack is NULL, 
 * it logs an error and returns false.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 * 
 * @return true if the first stack is less than or equal to the second, false otherwise.
 */
bool stack_is_less_or_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_less_or_equal.\n");
        #endif 
        return false;
    }
    return stack_is_less(stk1, stk2) || stack_is_equal(stk1, stk2);
}

/**
 * @brief Checks if the first stack is greater than or equal to the second stack.
 *
 * This function compares two stacks and returns true if the first stack is considered greater than 
 * or equal to the second stack based on their underlying vector comparison. If either stack is NULL, 
 * it logs an error and returns false.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 * 
 * @return true if the first stack is greater than or equal to the second, false otherwise.
 */
bool stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_greater_or_equal.\n");
        #endif 
        return false;
    }
    return stack_is_greater(stk1, stk2) || stack_is_equal(stk1, stk2);
}

/**
 * @brief Checks if two stacks are not equal.
 *
 * This function compares two stacks and returns true if the two stacks are not equal 
 * based on their underlying vector comparison. If either stack is NULL, 
 * it logs an error and returns false.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 * 
 * @return true if the two stacks are not equal, false otherwise.
 */
bool stack_is_not_equal(const Stack* stk1, const Stack* stk2) {
    if (!stk1 || !stk2) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: One or both Stack pointers are NULL in stack_is_not_equal.\n");
        #endif 
        return false;
    }
    return !stack_is_equal(stk1, stk2);
}

/**
 * @brief Pushes an item onto the stack.
 *
 * This function pushes a new item onto the stack. If the stack or the item pointer is NULL, 
 * it logs an error and exits the program.
 *
 * @param stk Pointer to the stack where the item will be pushed.
 * @param item Pointer to the item that will be pushed onto the stack.
 */
void stack_push(const Stack* stk, const void* item) {
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

/**
 * @brief Removes the top item from the stack and returns it.
 *
 * This function pops the top item from the stack and returns a pointer to it. 
 * If the stack is NULL or empty, it logs an error or warning, respectively, and returns NULL.
 *
 * @param stk Pointer to the stack from which the item will be popped.
 * @return A pointer to the popped item, or NULL if the stack is empty or NULL.
 */
void* stack_pop(const Stack* stk) {
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

/**
 * @brief Returns the top item of the stack without removing it.
 *
 * This function returns a pointer to the top item of the stack without removing it. 
 * If the stack is NULL or empty, it logs an error or warning, respectively, and returns NULL.
 *
 * @param stk Pointer to the stack whose top item will be accessed.
 * @return A pointer to the top item of the stack, or NULL if the stack is empty or NULL.
 */
void* stack_top(const Stack* stk) {
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

/**
 * @brief Returns the number of items in the stack.
 *
 * This function returns the size of the stack, i.e., the number of items currently in the stack.
 * If the stack pointer is NULL, it logs a warning and returns 0.
 *
 * @param stk Pointer to the stack whose size is to be returned.
 * @return The number of items in the stack, or 0 if the stack is NULL.
 */
size_t stack_size(const Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Stack pointer is NULL in stack_size. Returning 0.\n");
        #endif 
        return 0;
    }
    return vector_size(stk->vec);
}

/**
 * @brief Checks if the stack is empty.
 *
 * This function checks whether the stack is empty. A NULL stack is considered empty.
 *
 * @param stk Pointer to the stack to be checked.
 * @return true if the stack is empty or NULL, false otherwise.
 */
bool stack_empty(const Stack* stk) {
    if (stk == NULL) {
        return true; // Consider a NULL stack as empty
    }
    return vector_is_empty(stk->vec);
}

/**
 * @brief Emplaces an item at the top of the stack.
 *
 * This function adds a new item to the top of the stack without copying it. It directly constructs the item
 * in place at the top of the stack. If the stack or the item pointer is NULL, an error is logged.
 *
 * @param stk Pointer to the stack where the item will be emplaced.
 * @param item Pointer to the item to be emplaced on the stack.
 */
void stack_emplace(const Stack* stk, void* item) {
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

/**
 * @brief Clears all items from the stack.
 *
 * This function removes all items from the stack, effectively resetting its size to zero. 
 * If the stack pointer is NULL, an error is logged.
 *
 * @param stk Pointer to the stack to be cleared.
 */
void stack_clear(const Stack* stk) {
    if (stk == NULL) {
        #ifdef STACK_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Stack pointer is NULL in stack_clear.\n");
        #endif 
        return;
    }
    vector_clear(stk->vec);
}

/**
 * @brief Swaps the contents of two stacks.
 *
 * This function exchanges the contents of two stacks. If either of the stack pointers is NULL, an error is logged.
 *
 * @param stk1 Pointer to the first stack.
 * @param stk2 Pointer to the second stack.
 */
void stack_swap(Stack* stk1, Stack* stk2) {
    if (stk1 == NULL || stk2 == NULL) {
        fmt_fprintf(stderr, "Error: One or both stack pointers are NULL in stack_swap.\n");
        return;
    }

    Vector* tempVec = stk1->vec;
    stk1->vec = stk2->vec;
    stk2->vec = tempVec;
}

/**
 * @brief Deallocates the stack and frees up memory.
 *
 * This function deallocates the memory used by the stack, including the underlying vector.
 * If the stack pointer is NULL, an error is logged.
 *
 * @param stk Pointer to the stack to be deallocated.
 */
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
