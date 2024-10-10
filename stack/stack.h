/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Stack
*/


#ifndef STACK_H_
#define STACK_H_

#define STACK_LOGGING_ENABLE 

#include "../vector/vector.h"

typedef struct Stack Stack;

struct Stack {   
    Vector* vec;
};

Stack* stack_create(size_t itemSize);
size_t stack_size(const Stack* stk);

bool stack_is_equal(const Stack* stk1, const Stack* stk2);
bool stack_is_less(const Stack* stk1, const Stack* stk2);
bool stack_is_greater(const Stack* stk1, const Stack* stk2);
bool stack_is_less_or_equal(const Stack* stk1, const Stack* stk2);
bool stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2);
bool stack_is_not_equal(const Stack* stk1, const Stack* stk2);
bool stack_empty(const Stack* stk);

void* stack_top(const Stack* stk);
void* stack_pop(const Stack* stk);

void stack_push(const Stack* stk, const void* item);
void stack_emplace(const Stack* stk, void* item);
void stack_clear(const Stack* stk);
void stack_swap(Stack* stk1, Stack* stk2);
void stack_deallocate(Stack* stk);

#endif 