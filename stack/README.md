# Stack Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License

The Stack library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::stack` in C++.

## Compilation

The Stack module is implemented on top of the project's `vector` module, so `vector.c` must be compiled together with `stack.c`. A typical command line is:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s \
    -o main ./main.c ./stack/stack.c ./vector/vector.c
```

If your program also uses other modules (e.g. `string`, `fmt`), append their `.c` files (`./string/std_string.c`, `./fmt/fmt.c`, ...). Ensure GCC is installed and on your `PATH`, and that the source files keep the directory layout shown in the project.

## Usage

To use the Stack library in your project, include the `stack.h` header file in your source code.

```c
#include "stack/stack.h"
```

---

## Function Descriptions

### `Stack* stack_create(size_t itemSize)`
- **Purpose**: Creates a new Stack object and initializes it to store items of a specified size.
- **Parameters**: 
    - `itemSize`: The size of each item that will be stored in the stack.
- **Returns**: A pointer to the newly created `Stack`.
- **Use case**: This function is used to create a stack that can hold items of a specific type or structure.

---

### `bool stack_is_equal(const Stack* stk1, const Stack* stk2)`
- **Purpose**: Checks whether two stacks are equal by comparing their contents.
- **Parameters**: 
    - `stk1`: The first stack to compare.
    - `stk2`: The second stack to compare.
- **Returns**: `true` if the stacks are equal, `false` otherwise.
- **Use case**: Useful when you need to verify if two stacks contain the same elements.

---

### `bool stack_is_less(const Stack* stk1, const Stack* stk2)`
- **Purpose**: Compares two stacks to determine if the first stack is less than the second stack (based on the lexicographical order of the elements).
- **Parameters**: 
    - `stk1`: The first stack.
    - `stk2`: The second stack.
- **Returns**: `true` if `stk1` is less than `stk2`, `false` otherwise.
- **Use case**: For sorting or ranking stacks based on their contents.

---

###  `bool stack_is_greater(const Stack* stk1, const Stack* stk2)`
- **Purpose**: Compares two stacks to determine if the first stack is greater than the second stack (based on the lexicographical order of the elements).
- **Parameters**: 
    - `stk1`: The first stack.
    - `stk2`: The second stack.
- **Returns**: `true` if `stk1` is greater than `stk2`, `false` otherwise.
- **Use case**: Used in sorting or ranking stacks.

---

### `bool stack_is_less_or_equal(const Stack* stk1, const Stack* stk2)`
- **Purpose**: Determines if the first stack is less than or equal to the second stack.
- **Parameters**: 
    - `stk1`: The first stack.
    - `stk2`: The second stack.
- **Returns**: `true` if `stk1` is less than or equal to `stk2`, `false` otherwise.
- **Use case**: For relational comparisons between stacks.

---

### `bool stack_is_greater_or_equal(const Stack* stk1, const Stack* stk2)`
- **Purpose**: Determines if the first stack is greater than or equal to the second stack.
- **Parameters**: 
    - `stk1`: The first stack.
    - `stk2`: The second stack.
- **Returns**: `true` if `stk1` is greater than or equal to `stk2`, `false` otherwise.
- **Use case**: For relational comparisons between stacks.

---

### `bool stack_is_not_equal(const Stack* stk1, const Stack* stk2)`
- **Purpose**: Checks whether two stacks are not equal by comparing their contents.
- **Parameters**: 
    - `stk1`: The first stack to compare.
    - `stk2`: The second stack to compare.
- **Returns**: `true` if the stacks are not equal, `false` otherwise.
- **Use case**: For checking inequality between two stacks.

---

### `void stack_push(const Stack* stk, const void* item)`
- **Purpose**: Pushes a new item onto the top of the stack.
- **Parameters**: 
    - `stk`: The stack onto which the item will be pushed.
    - `item`: A pointer to the item to be added to the stack.
- **Returns**: `void`.
- **Use case**: Used to add a new item to the stack.

---

### `void* stack_pop(const Stack* stk)`
- **Purpose**: Removes the top item from the stack and returns it.
- **Parameters**: 
    - `stk`: The stack from which the top item will be popped.
- **Returns**: A pointer to the item that was removed from the stack.
- **Use case**: Used to remove and retrieve the last added item from the stack.

---

### `void* stack_top(const Stack* stk)`
- **Purpose**: Retrieves the top item of the stack without removing it.
- **Parameters**: 
    - `stk`: The stack from which the top item will be retrieved.
- **Returns**: A pointer to the top item in the stack.
- **Use case**: Used when you need to look at the top item without modifying the stack.

---

### `bool stack_empty(const Stack* stk)`
- **Purpose**: Checks whether the stack is empty.
- **Parameters**: 
    - `stk`: The stack to check.
- **Returns**: `true` if the stack is empty, `false` otherwise.
- **Use case**: Useful to check if there are no items in the stack before performing operations like popping.

---

### `size_t stack_size(const Stack* stk)`
- **Purpose**: Returns the number of items currently stored in the stack.
- **Parameters**: 
    - `stk`: The stack whose size will be checked.
- **Returns**: The number of items in the stack.
- **Use case**: Used when the number of items in the stack needs to be known.

---

### `void stack_clear(const Stack* stk)`
- **Purpose**: Removes all items from the stack, effectively emptying it.
- **Parameters**: 
    - `stk`: The stack to be cleared.
- **Returns**: `void`.
- **Use case**: Used when you need to reset the stack by removing all items.

---

### `void stack_emplace(const Stack* stk, void* item)`
- **Purpose**: Adds a new item to the top of the stack without copying the item.
- **Parameters**: 
    - `stk`: The stack onto which the item will be added.
    - `item`: A pointer to the item that will be added directly (without copying).
- **Returns**: `void`.
- **Use case**: Used for performance optimization, particularly when avoiding unnecessary memory copying is important.

---

### `void stack_swap(Stack* stk1, Stack* stk2)`
- **Purpose**: Swaps the contents of two stacks.
- **Parameters**: 
    - `stk1`: The first stack.
    - `stk2`: The second stack.
- **Returns**: `void`.
- **Use case**: Useful when you need to exchange the contents of two stacks without copying the individual elements.

---

### `void stack_deallocate(Stack* stk)`
- **Purpose**: Deallocates the stack, freeing up the memory used by the stack. Passing `NULL` is a safe no-op.
- **Parameters**: 
    - `stk`: The stack to deallocate.
- **Returns**: `void`.
- **Use case**: This function is essential for freeing the memory used by the stack once it is no longer needed, preventing memory leaks.

---

### `bool stack_pop_void(Stack* stk)`
- **Purpose**: Removes the top element from the stack and discards it — the exact C analog of C++ `std::stack::pop()`.
- **Parameters**:
    - `stk`: The stack to pop from.
- **Returns**: `true` on success, `false` if the stack is `NULL` or empty.
- **Use case**: Use when you want to discard the top element without reading it. Safer than `stack_pop()` because there is no dangling-pointer risk.

---

### `bool stack_pop_value(Stack* stk, void* out_buf)`
- **Purpose**: Copies the top element (`itemSize` bytes) into `out_buf`, then removes it. **Prefer this over the legacy `stack_pop()`**, whose returned pointer is only valid until the next modifying call.
- **Parameters**:
    - `stk`: The stack to pop from.
    - `out_buf`: Destination buffer for the copied value; pass `NULL` to just drop the element.
- **Returns**: `true` on success, `false` if the stack is `NULL` or empty.
- **Use case**: Preferred way to read-and-remove an element safely. The caller owns the copy and there is no lifetime concern.

---

### `Stack* stack_copy(const Stack* src)`
- **Purpose**: Creates an independent deep copy of `src`. Every element is duplicated; subsequent modifications to either stack do not affect the other.
- **Parameters**:
    - `src`: The stack to copy. May be `NULL` (returns `NULL`).
- **Returns**: A pointer to the new stack, or `NULL` on allocation failure or `NULL` input.
- **Use case**: Snapshot the current stack state for rollback, undo, or parallel processing.

---

### `bool stack_assign(Stack* dest, const Stack* src)`
- **Purpose**: Replaces `dest`'s contents with a deep copy of `src`. `dest` is cleared first, then every element from `src` is copied in.
- **Parameters**:
    - `dest`: Target stack (must already exist and have the same `itemSize` as `src`).
    - `src`: Source stack.
- **Returns**: `true` on success. `false` if either pointer is `NULL`, if the underlying vector cannot be reserved, or if the element sizes differ.
- **Use case**: Reassign an existing stack without reallocating its wrapper struct.

---

### `bool stack_reserve(Stack* stk, size_t new_capacity)`
- **Purpose**: Pre-allocates storage in the underlying vector so that at least `new_capacity` elements can be pushed without triggering a reallocation.
- **Parameters**:
    - `stk`: The stack to reserve space for.
    - `new_capacity`: Minimum number of elements to accommodate.
- **Returns**: `true` on success, `false` if `stk` is `NULL` or the underlying `vector_reserve` fails.
- **Use case**: Avoids repeated reallocations when the final size is known in advance.

---

### `size_t stack_capacity(const Stack* stk)`
- **Purpose**: Returns the total number of elements the stack can hold before the next reallocation.
- **Parameters**:
    - `stk`: The stack to query. May be `NULL` (returns `0`).
- **Returns**: Current capacity, or `0` on `NULL` input.
- **Use case**: Inspect how much headroom is available without pushing an element.

---

### `size_t stack_item_size(const Stack* stk)`
- **Purpose**: Returns the size in bytes of each element stored in the stack.
- **Parameters**:
    - `stk`: The stack to query. May be `NULL` (returns `0`).
- **Returns**: The `itemSize` passed to `stack_create`, or `0` on `NULL` input.
- **Use case**: Useful for generic code that manipulates stacks through `void*` without knowing the element type at compile time.

---

### Behavioral notes

- `stack_push` now returns `bool` (`false` on allocation failure or NULL args). Old code that ignores the return value still compiles.
- `stack_emplace` now returns a `void*` to the just-inserted top element (mirrors C++17 `std::stack::emplace`'s return). `NULL` on failure.
- `stack_swap` refuses to swap stacks with differing element sizes; it sets `STACK_ERROR_ITEM_SIZE_MISMATCH` and leaves both stacks untouched.
- `stack_is_equal(NULL, NULL)` returns `true` (consistent with `stack_is_not_equal(NULL, NULL) == false`).

---

### Examples 

## Example 1 : create Stack Obj and `stack_push_back` and get `stack_size`

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

    stack_deallocate(stack);
    return 0;
}
```
**Result:**
```
Size of stack is 5
```

---

## Example 2 : use `stack_top`, `stack_pop` and `stack_empty` methods 

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s \
    -o main ./main.c ./string/std_string.c ./vector/vector.c ./stack/stack.c
```

 also you can use cmake and ... 

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
**Result:**
```
Size of stack is 5
Top Element is 50
Pop value is 50
After Pop size is 4
```

---

## Example 3 : how to use String Object in Stack 

```c
#include "stack/stack.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* myString = string_create("");
    Stack* stack = stack_create(sizeof(char*));

    const char* value1 = "Amin";
    string_append(myString, value1);
    stack_push(stack, &myString);

    const char* value2 = "Tahmasebi";
    string_append(myString, value2);
    stack_push(stack, &myString);

    const char* value3 = "C Programming";
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
**Result:**
```
Size of Stack is 3
AminTahmasebiC Programming
```

---

## Example 4 : all relationals operators as methods in Stack 

```c
#include "stack/stack.h"
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
    
    stack_deallocate(stk1);
    stack_deallocate(stk2);

    return 0;
}
```
**Result:**
```
stk1 is equal to stk2
stk1 is less than or equal to stk2
stk1 is greater than or equal to stk2
```

---

## Example 5 : Using Stack with String Objects for Expression Evaluation

This example demonstrates how to use the Stack and String libraries to evaluate a simple expression. It's a basic implementation and works with single-digit numbers and basic operators (+, -, *, /).
```c
#include "stack/stack.h"
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <ctype.h>



int performOperation(int op1, int op2, char op) {
    switch (op) {
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
        char op = *(char*)stack_pop(operators);
        int op2 = *(int*)stack_pop(values);
        int op1 = *(int*)stack_pop(values);
        int result = performOperation(op1, op2, op);

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

    fmt_printf("Result: %d\n", result);

    string_deallocate(expr);
    return 0;
}
```
**Result:**
```
Result: 0
```

---

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
**Result:**
```
Current State After Undo: (1, 2) (3, 4) 
```

---

### Example 7 : Checking for Balanced Parentheses

The program will take an input string and use a stack to keep track of opening parentheses. For each closing parenthesis, it will check if there is a corresponding opening parenthesis in the stack. If the stack is empty or the parentheses are mismatched, the string is not balanced.
```c
#include "stack/stack.h"
#include "string/std_string.h"
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

            if ((currentChar == ')' && topChar != '(') || (currentChar == ']' && topChar != '[') || (currentChar == '}' && topChar != '{')) {
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
        fmt_printf("The string %s is balanced.\n", string_c_str(str));
    }
    else {
        fmt_printf("The string %s is not balanced.\n", string_c_str(str));
    }

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
The string {[()]} is balanced.
```

---

## Example 8: `stack_pop_void` and `stack_pop_value`

Demonstrates the two safe pop variants. `stack_pop_void` discards the top element without reading it; `stack_pop_value` copies it into a caller-owned buffer before dropping it.

```c
#include "stack/stack.h"
#include "fmt/fmt.h"

int main(void) {
    Stack *stk = stack_create(sizeof(int));
    int vals[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) stack_push(stk, &vals[i]);

    /* Discard 30 silently */
    stack_pop_void(stk);
    fmt_printf("After pop_void, size = %zu\n", stack_size(stk));  /* 2 */

    /* Copy 20 into a local variable, then drop */
    int out = 0;
    stack_pop_value(stk, &out);
    fmt_printf("pop_value gave: %d, size = %zu\n", out, stack_size(stk)); /* 20, 1 */

    /* Pass NULL to just drop without reading */
    stack_pop_value(stk, NULL);
    fmt_printf("After final drop, empty = %s\n", stack_empty(stk) ? "true" : "false");

    stack_deallocate(stk);
    return 0;
}
```
**Result:**
```
After pop_void, size = 2
pop_value gave: 20, size = 1
After final drop, empty = true
```

---

## Example 9: `stack_copy` — independent deep copy

`stack_copy` produces a fully independent clone. Pushing onto or popping from one stack does not affect the other.

```c
#include "stack/stack.h"
#include "fmt/fmt.h"

int main(void) {
    Stack *src = stack_create(sizeof(int));
    int vals[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) stack_push(src, &vals[i]);

    Stack *dst = stack_copy(src);
    fmt_printf("src size = %zu, dst size = %zu\n", stack_size(src), stack_size(dst));

    /* Push onto src — dst must not change */
    int extra = 99;
    stack_push(src, &extra);
    fmt_printf("after push on src: src=%zu  dst=%zu\n", stack_size(src), stack_size(dst));

    fmt_printf("dst top = %d\n", *(int*)stack_top(dst));   /* still 5 */

    stack_deallocate(src);
    stack_deallocate(dst);

    return 0;
}
```
**Result:**
```
src size = 5, dst size = 5
after push on src: src=6  dst=5
dst top = 5
```

---

## Example 10: `stack_assign` — replace stack contents

`stack_assign` replaces the destination stack's contents with a deep copy of the source, without reallocating the `Stack` wrapper itself.

```c
#include "stack/stack.h"
#include "fmt/fmt.h"

int main(void) {
    Stack *s1 = stack_create(sizeof(int));
    Stack *s2 = stack_create(sizeof(int));

    int v1[] = {10, 20, 30};
    int v2[] = {100, 200};
    for (int i = 0; i < 3; i++) stack_push(s1, &v1[i]);
    for (int i = 0; i < 2; i++) stack_push(s2, &v2[i]);

    fmt_printf("before assign: s2 size = %zu, top = %d\n", stack_size(s2), *(int*)stack_top(s2));

    stack_assign(s2, s1);

    fmt_printf("after assign:  s2 size = %zu, top = %d\n", stack_size(s2), *(int*)stack_top(s2));

    /* Modifying s1 must NOT affect s2 */
    int extra = 999;
    stack_push(s1, &extra);
    fmt_printf("s1 size = %zu, s2 size = %zu (independent)\n", stack_size(s1), stack_size(s2));

    stack_deallocate(s1);
    stack_deallocate(s2);

    return 0;
}
```
**Result:**
```
before assign: s2 size = 2, top = 200
after assign:  s2 size = 3, top = 30
s1 size = 4, s2 size = 3 (independent)
```

---

## Example 11: `stack_reserve`, `stack_capacity`, `stack_item_size`

Pre-allocate storage to avoid repeated reallocations when the maximum load is known.

```c
#include "stack/stack.h"
#include "fmt/fmt.h"

int main(void) {
    Stack *stk = stack_create(sizeof(double));

    fmt_printf("item size    = %zu bytes\n", stack_item_size(stk));

    /* Reserve space for 200 doubles upfront */
    stack_reserve(stk, 200);
    fmt_printf("capacity after reserve(200) = %zu\n", stack_capacity(stk));
    fmt_printf("size after reserve           = %zu\n", stack_size(stk));

    for (int i = 0; i < 10; i++) {
        double d = (double)i * 1.5;
        stack_push(stk, &d);
    }

    fmt_printf("size after 10 pushes = %zu\n", stack_size(stk));
    fmt_printf("capacity unchanged   = %zu\n", stack_capacity(stk));

    stack_deallocate(stk);
    return 0;
}
```
**Result:**
```
item size    = 8 bytes
capacity after reserve(200) = 200
size after reserve           = 0
size after 10 pushes = 10
capacity unchanged   = 200
```

---

## Example 12: `stack_swap`

Swaps the entire contents of two stacks in O(1) — only the internal vector pointers are exchanged.

```c
#include "stack/stack.h"
#include "fmt/fmt.h"

int main(void) {
    Stack *s1 = stack_create(sizeof(int));
    Stack *s2 = stack_create(sizeof(int));

    int a = 10; stack_push(s1, &a);
    int b = 20; stack_push(s2, &b);

    fmt_printf("before swap: s1 top = %d, s2 top = %d\n", *(int*)stack_top(s1), *(int*)stack_top(s2));

    stack_swap(s1, s2);

    fmt_printf("after  swap: s1 top = %d, s2 top = %d\n", *(int*)stack_top(s1), *(int*)stack_top(s2));

    /* Swap between stacks with different item sizes is a safe */
    Stack *s3 = stack_create(sizeof(double));
    double x = 3.14; 

    stack_push(s3, &x);
    stack_swap(s1, s3);   /* refused: int ≠ double */
    fmt_printf("s1 top unchanged = %d\n", *(int*)stack_top(s1));

    stack_deallocate(s1);
    stack_deallocate(s2);
    stack_deallocate(s3);

    return 0;
}
```
**Result:**
```
before swap: s1 top = 10, s2 top = 20
after  swap: s1 top = 20, s2 top = 10
s1 top unchanged = 20
```


## License

This project is open-source and available under the ISC License.
