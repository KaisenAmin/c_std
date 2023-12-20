#include "stack/stack.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    Stack* stk1 = stack_create(sizeof(int));
    Stack* stk2 = stack_create(sizeof(int));

    if (stack_is_equal(stk1, stk2))
        printf("stk1 is equal to stk2\n");
        
    if (stack_is_less(stk1, stk2)) 
        printf("stk1 is less than stk2\n");

    if (stack_is_greater(stk1, stk2)) 
        printf("stk1 is greater than stk2\n");

    if (stack_is_less_or_equal(stk1, stk2)) 
        printf("stk1 is less than or equal to stk2\n");
        
    if (stack_is_greater_or_equal(stk1, stk2)) 
        printf("stk1 is greater than or equal to stk2\n");

    if (stack_is_not_equal(stk1, stk2)) 
        printf("stk1 is not equal to stk2\n");

    // Clean up the stacks...
    stack_deallocate(stk1);
    stack_deallocate(stk2);

    return EXIT_SUCCESS;
}