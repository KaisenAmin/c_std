#ifndef STACK_H_
#define STACK_H_

#include "../vector/vector.h"

typedef struct Stack Stack;

struct Stack 
{   
    Vector* vec;

    bool (*is_equal)(const Stack* stk1, const Stack* stk2);
    bool (*is_less)(const Stack* stk1, const Stack* stk2);
    bool (*is_greater)(const Stack* stk1, const Stack* stk2);
    bool (*is_less_or_equal)(const Stack* stk1, const Stack* stk2);
    bool (*is_greater_or_equal)(const Stack* stk1, const Stack* stk2);
    bool (*is_not_equal)(const Stack* stk1, const Stack* stk2);
    bool (*empty)(Stack* stk);
    size_t (*size)(Stack* stk);
    void* (*top)(Stack* stk);
    void* (*pop)(Stack* stk);
    void (*push)(Stack* stk, void* item);
    void (*emplace)(Stack* stk, void* item);
    void (*clear)(Stack* stk);
    void (*swap)(Stack* stk1, Stack* stk2);
    void (*deallocate)(Stack* stk);
};

Stack* stack_create(size_t itemSize);

#endif 