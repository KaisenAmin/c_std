
# ForwardList Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License


The ForwardList library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size ForwardList, offering similar functionality to `std::forward_list` in C++.

## Compilation

To compile the ForwardList library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c forward_list/forward_list.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the ForwardList library in your project, include the `forward_list.h` header file in your source code.

```c
#include "forward_list/forward_list.h"
```


Creating 20 different examples to demonstrate the usage of all methods in your `ForwardList` structure will provide a comprehensive understanding of how each function operates. Here are the examples:

### Example 1: Create a ForwardList and Push Front with `forward_list_create` and `forward_list_push_front`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) {
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");
    
    forward_list_deallocate(list);
    return 0;
}
```

### Example 2: Pop Front Element with `forward_list_pop_front`
```c
#include "forward_list/forward_list.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);
    forward_list_pop_front(list);

    forward_list_deallocate(list);
    return 0;
}
```

### Example 3: Access Front Element with `forward_list_front`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);
    int *frontValue = forward_list_front(list);

    fmt_printf("Front value: %d\n", *frontValue);

    forward_list_deallocate(list);
    return 0;
}
```

### Example 4: Clear List with `forward_list_clear`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) {
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_clear(list);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");
    
    forward_list_deallocate(list);
    return 0;
}
```

### Example 5: Check if List is Empty `forward_list_empty`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));

    fmt_printf("Is list empty? %s\n", forward_list_empty(list) ? "Yes" : "No");

    forward_list_deallocate(list);
    return 0;
}
```

### Example 6: Get List Length with `forward_list_length`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);
    fmt_printf("List length: %zu\n", forward_list_length(list));

    forward_list_deallocate(list);
    return 0;
}
```

### Example 7: Assign Values to List `forward_list_assign`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}

```

### Example 8: Get before_begin Iterator with `forward_list_before_begin`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    ForwardListNode *node = forward_list_before_begin(list);

    fmt_printf("Node: %p\n", (void*)node);

    forward_list_deallocate(list);
    return 0;
}
```

## Example 9: Get begin and end Iterator with `forward_list_begin` and `forward_list_end`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);
    for (ForwardListNode *node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d\n", *(int *)node->value);
    }
    
    forward_list_deallocate(list);
    return 0;
}
```

## Example 10: Get max_size with `forward_list_max_size`

```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));

    fmt_printf("Max size: %zu\n", forward_list_max_size(list));

    forward_list_deallocate(list);
    return 0;
}
```

## Example 11: Emplace Front with `forward_list_emplace_front`
```c
#include "fmt/fmt.h"
#include "forward_list/forward_list.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_emplace_front(list, &value);
    
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}

```

## Example 12: Emplace After with `forward_list_emplace_after`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_emplace_front(list, &value);
    forward_list_emplace_after(list, forward_list_before_begin(list), &value);

    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}
```

## Example 13: Insert After with `forward_list_insert_after`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20};

    forward_list_insert_after(list, forward_list_before_begin(list), values, 2);

    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) {
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}
```

## Example 14: Erase After with `forward_list_erase_after`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);
    forward_list_erase_after(list, forward_list_begin(list));

    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}

```

## Example 15: Swap ForwardLists with `forward_list_swap`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

void print_list(ForwardList *list) {
    ForwardListNode *current = forward_list_begin(list);
    while (current != NULL) {
        int *value = (int *)current->value;
        fmt_printf("%d ", *value);
        current = current->next;
    }
    fmt_printf("\n");
}

int main() {
    ForwardList* list1 = forward_list_create(sizeof(int));
    ForwardList* list2 = forward_list_create(sizeof(int));

    int values1[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) {
        forward_list_push_front(list1, &values1[i]);
    }
    
    int values2[] = {4, 5, 6};
    for (int i = 0; i < 3; i++) {
        forward_list_push_front(list2, &values2[i]);
    }

    forward_list_swap(list1, list2);

    fmt_printf("List1 after swap: ");
    print_list(list1);

    fmt_printf("List2 after swap: ");
    print_list(list2);

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);

    return 0;
}

```

## Example 16: Resize List with `forward_list_resize`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    forward_list_resize(list, 3);

    fmt_printf("Size of the list after resizing: %zu\n", forward_list_length(list));
    
    forward_list_deallocate(list);
    return 0;
}
```

## Example 17: Splice After with `forward_list_splice_after`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list1 = forward_list_create(sizeof(int));
    ForwardList* list2 = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list2, values, 3);
    forward_list_splice_after(list1, forward_list_before_begin(list1), list2);

    for (ForwardListNode* node = forward_list_begin(list1); node != forward_list_end(list1); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);
    return 0;
}
```

## Example 18: Remove Element from ForwardList with `forward_list_remove`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};
    forward_list_assign(list, values, 3);

    int valueToRemove = 20;
    forward_list_remove(list, &valueToRemove);

    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}
```

## Example 19: Remove Elements if Condition is Met with `forward_list_remove_if`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

bool is_even(void* value) {
    int val = *(int*)value;
    return val % 2 == 0;
}

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 15, 20, 25, 30};

    forward_list_assign(list, values, 5);
    forward_list_remove_if(list, is_even);

    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}
```

## Example 20: get unique elements from ForwardList with `forward_list_unique`

```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 10, 20, 30, 30, 30};
    
    forward_list_assign(list, values, 6);
    forward_list_unique(list);

    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) { 
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");

    forward_list_deallocate(list);
    return 0;
}
```

## Example 21 : Relational operators in ForwardList 

```c

#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList *list1 = forward_list_create(sizeof(int));
    ForwardList *list2 = forward_list_create(sizeof(int));
    
    int values1[] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < sizeof(values1) / sizeof(values1[0]); i++) {
        forward_list_push_front(list1, &values1[i]);
    }
    
    int values2[] = {1, 2, 3, 4, 5, 6}; 
    for (size_t i = 0; i < sizeof(values2) / sizeof(values2[0]); i++) {
        forward_list_push_front(list2, &values2[i]);
    }
    
    if (forward_list_is_less(list1, list2)) { 
        fmt_printf("List1 is less than List2\n");
    }
    else if (forward_list_is_greater(list1, list2)) {
        fmt_printf("List1 is greater than List2\n");
    }
    else if (forward_list_is_equal(list1, list2)) {
        fmt_printf("List1 is equal to List2\n");
    }
    if (forward_list_is_not_equal(list1, list2)) { 
        fmt_printf("List1 is not equal to List2\n");
    }

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);
    return 0;
}
```