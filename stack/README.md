# Stack Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License

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

---

## Function Descriptions

### Stack Creation and Management
- `stack_create(size_t itemSize)`: Creates a new Stack object. It takes the size of the item type as a parameter.

### Relational Operators
- `stack_is_equal(const Stack* stk1, const Stack* stk2)`: Checks if two stacks are equal.
- `stack_is_less(const Stack* stk1, const Stack* stk2)`: Checks if the first stack is less than the second.
- `stack_is_greater(const Stack* stk1, const Stack* stk2)`: Checks if the first stack is greater than the second.
- `stack_is_less_or_equal(const Stack* stk1, const Stack* stk2)`: Checks if the first stack is less than or equal to the second.
- `stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2)`: Checks if the first stack is greater than or equal to the second.
- `stack_is_not_equal(const Stack* stk1, const Stack* stk2)`: Checks if two stacks are not equal.

### Stack Operations
- `stack_push(Stack* stk, void* item)`: Pushes an item onto the stack.
- `stack_pop(Stack* stk)`: Removes the top item from the stack and returns it.
- `stack_top(Stack* stk)`: Returns the top item of the stack without removing it.
- `stack_empty(Stack* stk)`: Checks if the stack is empty.
- `stack_size(Stack* stk)`: Returns the number of items in the stack.
- `stack_clear(Stack* stk)`: Removes all items from the stack.

### Advanced Stack Operations
- `stack_emplace(Stack* stk, void* item)`: Adds a new item to the top of the stack without copying.
- `stack_swap(Stack* stk1, Stack* stk2)`: Swaps the contents of two stacks.

### Cleanup
- `stack_deallocate(Stack* stk)`: Deallocates the stack and frees up the memory used.

---

## Example 1 : create Stack Obj and `stack_push_back` and get `stack_size`

```c
#include "stack/stack.h"
#include "fmt/fmt.h"

int main() {
    Stack* stack = stack_create(sizeof(int));
    int arr[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        stack->push(stack, &arr[i]);
    }
    fmt_printf("Size of stack is %d\n", stack->size(stack));

    stack->deallocate(stack);
    return 0;
}
```

## Example 2 : use `stack_top`, `stack_pop` and `stack_empty` methods 

 gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c .\string\string.c .\vector\vector.c .\stack\stack.c

```c

#include "stack/stack.h"
#include "fmt/fmt.h"

int main() {
    Stack* stack = stack_create(sizeof(int));
    int arr[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        stack_push(stack, &arr[i]);
    }
    fmt_printf("Size of stack is %zu\n", stack_size(stack));

    if (!stack_empty(stack)) {
        int topValue = *(int*)stack_top(stack);
        fmt_printf("Top Element is %d\n", topValue);

        int pop = *(int*)stack_pop(stack);
        fmt_printf("Pop value is %d\n", pop);
        fmt_printf("After Pop size is %zu\n", stack_size(stack));
    }

    stack_deallocate(stack);
    return 0;
}

```

## Example 3 : how to use String Object in Stack 

```c
#include "stack/stack.h"
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
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

    fmt_printf("Size of Stack is %zu\n", stack_size(stack));

    String** str1 = (String**)stack_pop(stack);
    fmt_printf("%s", string_c_str(*str1));

    string_deallocate(*str1);
    stack_deallocate(stack);
    
    return 0;
}
```

## Example 4 : all relationals operators as methods in Stack 

```c
#include "stack/stack.h"
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    Stack* stk1 = stack_create(sizeof(int));
    Stack* stk2 = stack_create(sizeof(int));

    if (stack_is_equal(stk1, stk2)) {
        fmt_printf("stk1 is equal to stk2\n");
    }
    if (stack_is_less(stk1, stk2)) { 
        fmt_printf("stk1 is less than stk2\n");
    }
    if (stack_is_greater(stk1, stk2)) {
        fmt_printf("stk1 is greater than stk2\n");
    }
    if (stack_is_less_or_equal(stk1, stk2)) {
        fmt_printf("stk1 is less than or equal to stk2\n");
    }
    if (stack_is_greater_or_equal(stk1, stk2)) { 
        fmt_printf("stk1 is greater than or equal to stk2\n");
    }
    if (stack_is_not_equal(stk1, stk2)) { 
        fmt_printf("stk1 is not equal to stk2\n");
    }
    // Clean up the stacks...
    stack_deallocate(stk1);
    stack_deallocate(stk2);

    return 0;
}
```

## Example 5 : Using Stack with String Objects for Expression Evaluation

This example demonstrates how to use the Stack and String libraries to evaluate a simple expression. It's a basic implementation and works with single-digit numbers and basic operators (+, -, *, /).
```c
#include "stack/stack.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <ctype.h>

int performOperation(int op1, int op2, char operator) {
    switch (operator) {
        case '+': 
            return op1 + op2;
        case '-': 
            return op1 - op2;
        case '*': 
            return op1 * op2;
        case '/': 
            return op1 / op2;
        default: 
            return 0;
    }
}

int evaluateExpression(String* expression) {
    Stack* values = stack_create(sizeof(int));
    Stack* operators = stack_create(sizeof(char));

    for (size_t i = 0; i < string_length(expression); i++) {
        char ch = string_at(expression, i);

        if (isdigit(ch)) {
            int val = ch - '0';
            stack_push(values, &val);
        } 
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            while (!stack_empty(operators)) {
                char topOp = *(char*)stack_top(operators);

                if (topOp == '*' || topOp == '/') {
                    stack_pop(operators);
                    int op2 = *(int*)stack_pop(values);
                    int op1 = *(int*)stack_pop(values);
                    int result = performOperation(op1, op2, topOp);
                    stack_push(values, &result);
                } 
                else {
                    break;
                }
            }
            stack_push(operators, &ch);
        }
    }

    while (!stack_empty(operators)) {
        char operator = *(char*)stack_pop(operators);
        int op2 = *(int*)stack_pop(values);
        int op1 = *(int*)stack_pop(values);
        int result = performOperation(op1, op2, operator);
        stack_push(values, &result);
    }

    int finalResult = *(int*)stack_pop(values);

    stack_deallocate(values);
    stack_deallocate(operators);

    return finalResult;
}

int main() {
    String* expr = string_create("3+2*2+1-8");
    int result = evaluateExpression(expr);

    printf("Result: %d\n", result);

    string_deallocate(expr);
    return 0;
}
```

### Example 6 :Stack of Vectors for Multi-level Undo Functionality

This example shows how a stack of vectors can be used to implement a multi-level undo functionality. Each vector represents a snapshot of a particular state, and we can push and pop these states from the stack to perform undo and redo operations.
```c
#include "stack/stack.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

typedef struct {
    int x;
    int y;
} Point;

void printVector(Vector* vec) {
    for (size_t i = 0; i < vector_size(vec); i++) {
        Point* p = (Point*)vector_at(vec, i);
        fmt_printf("(%d, %d) ", p->x, p->y);
    }
    fmt_printf("\n");
}

int main() {
    Stack* history = stack_create(sizeof(Vector*));

    // Initial state
    Vector* state1 = vector_create(sizeof(Point));
    Point p1 = {1, 2};
    
    vector_push_back(state1, &p1);
    stack_push(history, &state1);

    // Second state
    Vector* state2 = vector_create(sizeof(Point));
    Point p2 = {3, 4};

    vector_push_back(state2, &p1);
    vector_push_back(state2, &p2);
    stack_push(history, &state2);

    // Perform Undo
    Vector** currentState = (Vector**)stack_pop(history);

    fmt_printf("Current State After Undo: ");
    printVector(*currentState);

    // Cleanup
    vector_deallocate(state1);
    vector_deallocate(state2);
    stack_deallocate(history);

    return 0;
}
```

### Example 7 : Checking for Balanced Parentheses

The program will take an input string and use a stack to keep track of opening parentheses. For each closing parenthesis, it will check if there is a corresponding opening parenthesis in the stack. If the stack is empty or the parentheses are mismatched, the string is not balanced.
```c
#include "stack/stack.h"
#include "string/string.h"
#include "fmt/fmt.h"

bool isBalanced(String* input) {
    Stack* stack = stack_create(sizeof(char));
    size_t length = string_length(input);

    for (size_t i = 0; i < length; i++) {
        char currentChar = string_at(input, i);

        if (currentChar == '(' || currentChar == '[' || currentChar == '{') {
            stack_push(stack, &currentChar);
        }
        else if (currentChar == ')' || currentChar == ']' || currentChar == '}') {
            if (stack_empty(stack)) {
                stack_deallocate(stack);
                return false;
            }

            char topChar = *(char*)stack_top(stack);
            stack_pop(stack);

            if ((currentChar == ')' && topChar != '(') ||
                (currentChar == ']' && topChar != '[') ||
                (currentChar == '}' && topChar != '{')) {
                stack_deallocate(stack);
                return false;
            }
        }
    }

    bool balanced = stack_empty(stack);
    stack_deallocate(stack);

    return balanced;
}

int main() {
    String* str = string_create("{[()]}");

    if (isBalanced(str)) {
        fmt_printf("The string %s is balanced.\n", str->dataStr);
    }
    else {
        fmt_printf("The string %s is not balanced.\n", str->dataStr);
    }

    string_deallocate(str);
    return 0;
}
```