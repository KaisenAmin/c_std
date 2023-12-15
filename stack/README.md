# Stack Library

The Stack library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::stack` in C++.

## Compilation

To compile the Stack library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -o main ./main.c ./stack/stack.c
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

```c

Stack* stack = stack_create(sizeof(int));
int arr[] = {10, 20, 30, 40, 50};

for (int i = 0; i < 5; i++)
    stack->push(stack, &arr[i]);

printf("Size of stack is %d\n", stack->size(stack));

if (!stack->empty(stack))
{
    int topValue = *(int*)stack->top(stack);
    printf("Top Element is %d\n", topValue);

    int pop = *(int*)stack->pop(stack);
    printf("Pop value is %d\n", pop);
    printf("After Pop size is %d\n", stack->size(stack));
}

stack->deallocate(stack);

```

## Example 3 : how to use String Object in Stack 

```c
#include "string/string.h"

String* myString = string_create("");
Stack* stack = stack_create(sizeof(char*));

char* value1 = "Amin";
myString->append(myString, value1);
stack->push(stack, &myString);

char* value2 = "Tahmasebi";
myString->append(myString, value2);
stack->push(stack, &myString);

char* value3 = "C Programming";
myString->append(myString, value3);
stack->push(stack, &myString);

printf("Size of Stack is %d\n", stack->size(stack));

String** str1 = (String**)stack->pop(stack);
printf("%s", (*str1)->c_str(*str1));

(*str1)->deallocate(*str1);
stack->deallocate(stack);

```