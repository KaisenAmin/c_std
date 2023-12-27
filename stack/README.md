# Stack Library in C

The Stack library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::stack` in C++.

## Compilation

To compile the Stack library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./stack/stack.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Stack library in your project, include the `stack.h` header file in your source code.

```c
#include "stack/stack.h"
```


## Example 1 : create Stack Obj and 'push_back' and get 'size'

```c

Stack* stack = stack_create(sizeof(int));
int arr[] = {10, 20, 30, 40, 50};

for (int i = 0; i < 5; i++)
    stack->push(stack, &arr[i]);

printf("Size of stack is %d\n", stack->size(stack));

stack->deallocate(stack);

```


## Example 2 : use 'top', 'pop' and 'empty' methods 

 gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c .\string\string.c .\vector\vector.c .\stack\stack.c

```c

#include "stack/stack.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    Stack* stack = stack_create(sizeof(int));
    int arr[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++)
        stack_push(stack, &arr[i]);

    printf("Size of stack is %zu\n", stack_size(stack));

    if (!stack_empty(stack))
    {
        int topValue = *(int*)stack_top(stack);
        printf("Top Element is %d\n", topValue);

        int pop = *(int*)stack_pop(stack);
        printf("Pop value is %d\n", pop);
        printf("After Pop size is %zu\n", stack_size(stack));
    }

    stack_deallocate(stack);

    return EXIT_SUCCESS;
}

```

## Example 3 : how to use String Object in Stack 

```c
#include "stack/stack.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    String* myString = string_create("");
    Stack* stack = stack_create(sizeof(char*));

    char* value1 = "Amin";
    string_append(myString, value1);
    stack_push(stack, &myString);

    char* value2 = "Tahmasebi";
    string_append(myString, value2);
    stack_push(stack, &myString);

    char* value3 = "C Programming";
    string_append(myString, value3);
    stack_push(stack, &myString);

    printf("Size of Stack is %zu\n", stack_size(stack));

    String** str1 = (String**)stack_pop(stack);
    printf("%s", string_c_str(*str1));

    string_deallocate(*str1);
    stack_deallocate(stack);

    return EXIT_SUCCESS;
}

```

## Example 4 : all relationals operators as methods in Stack 

```c

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

```