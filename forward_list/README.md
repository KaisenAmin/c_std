
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

### Functions and Their Explanations

#### **List Creation and Initialization**

- **`ForwardList* forward_list_create(size_t itemSize)`**  
  Creates a new `ForwardList` with the specified item size. This function allocates memory for the list structure and initializes it with no elements.  
  - **Parameters**: `itemSize` – The size of each element in the list.
  - **Returns**: A pointer to the newly created `ForwardList` structure.

#### **Element Access**

- **`void* forward_list_front(const ForwardList* list)`**  
  Returns a pointer to the first element in the list.  
  - **Returns**: A pointer to the first element's data. If the list is empty, `NULL` is returned.

#### **List Insertion and Deletion**

- **`void forward_list_push_front(ForwardList* list, void* value)`**  
  Inserts a new element at the front of the list.  
  - **Parameters**: `list` – The list to insert into. `value` – A pointer to the value to insert.

- **`void forward_list_pop_front(ForwardList* list)`**  
  Removes the first element from the list.  
  - **Parameters**: `list` – The list to remove the element from.

- **`void forward_list_clear(ForwardList* list)`**  
  Removes all elements from the list, effectively clearing it.  
  - **Parameters**: `list` – The list to clear.

- **`void forward_list_deallocate(ForwardList* list)`**  
  Deallocates the list and all its elements, freeing memory.  
  - **Parameters**: `list` – The list to deallocate.

#### **List Properties and Size**

- **`bool forward_list_empty(const ForwardList* list)`**  
  Checks whether the list is empty.  
  - **Returns**: `true` if the list is empty, `false` otherwise.

- **`size_t forward_list_length(const ForwardList* list)`**  
  Returns the number of elements in the list.  
  - **Returns**: The number of elements in the list.

- **`size_t forward_list_max_size(const ForwardList* list)`**  
  Returns the maximum size the list can hold.  
  - **Returns**: The maximum number of elements the list can theoretically hold (often platform-dependent).

#### **List Assignment and Initialization**

- **`void forward_list_assign(ForwardList* list, void* values, size_t numValues)`**  
  Replaces the contents of the list with the elements from the provided array.  
  - **Parameters**: `list` – The list to assign values to. `values` – A pointer to the array of values. `numValues` – The number of elements to assign.

#### **List Iteration and Traversal**

- **`ForwardListNode* forward_list_before_begin(ForwardList* list)`**  
  Returns an iterator to the element before the first element.  
  - **Returns**: A pointer to the node before the first element, or `NULL` if the list is empty.

- **`ForwardListNode* forward_list_begin(ForwardList* list)`**  
  Returns an iterator to the first element in the list.  
  - **Returns**: A pointer to the first node.

- **`ForwardListNode* forward_list_end(ForwardList* list)`**  
  Returns an iterator to the end of the list (equivalent to `NULL`).  
  - **Returns**: `NULL`, representing the end of the list.

#### **Element Insertion and Erasure**

- **`void forward_list_emplace_front(ForwardList* list, void* value)`**  
  Emplaces a new element at the front of the list (uses existing memory instead of copying).  
  - **Parameters**: `list` – The list to insert into. `value` – A pointer to the value to insert.

- **`void forward_list_emplace_after(ForwardList* list, ForwardListNode* pos, void* value)`**  
  Emplaces a new element after the specified position in the list.  
  - **Parameters**: `list` – The list to insert into. `pos` – A pointer to the node after which to insert. `value` – A pointer to the value to insert.

- **`void forward_list_insert_after(ForwardList* list, ForwardListNode* pos, void* value, size_t numValues)`**  
  Inserts one or more elements after the specified position in the list.  
  - **Parameters**: `list` – The list to insert into. `pos` – A pointer to the node after which to insert. `value` – A pointer to the value to insert. `numValues` – The number of values to insert.

- **`void forward_list_erase_after(ForwardList* list, ForwardListNode* pos)`**  
  Removes the element following the specified position.  
  - **Parameters**: `list` – The list to remove from. `pos` – A pointer to the node before the one to be erased.

#### **List Manipulation**

- **`void forward_list_swap(ForwardList* list1, ForwardList* list2)`**  
  Swaps the contents of two lists.  
  - **Parameters**: `list1`, `list2` – The lists to swap.

- **`void forward_list_resize(ForwardList* list, size_t newSize)`**  
  Resizes the list to contain the specified number of elements.  
  - **Parameters**: `list` – The list to resize. `newSize` – The new size of the list.

- **`void forward_list_splice_after(ForwardList* list, ForwardListNode* pos, ForwardList* other)`**  
  Transfers elements from another list into this list after the specified position.  
  - **Parameters**: `list` – The list to splice into. `pos` – A pointer to the node after which to splice. `other` – The list to splice from.

- **`void forward_list_remove(ForwardList* list, void* value)`**  
  Removes all elements that match the specified value.  
  - **Parameters**: `list` – The list to remove from. `value` – The value to remove.

- **`void forward_list_remove_if(ForwardList* list, bool (*condition)(void*))`**  
  Removes all elements that satisfy the specified condition.  
  - **Parameters**: `list` – The list to remove from. `condition` – A pointer to the condition function.

- **`void forward_list_unique(ForwardList* list)`**  
  Removes consecutive duplicate elements from the list.  
  - **Parameters**: `list` – The list to remove duplicates from.

- **`void forward_list_merge(ForwardList* list1, ForwardList* list2)`**  
  Merges two sorted lists into one sorted list.  
  - **Parameters**: `list1`, `list2` – The lists to merge. The result is stored in `list1`.

- **`void forward_list_sort(ForwardList* list)`**  
  Sorts the elements in the list in ascending order.  
  - **Parameters**: `list` – The list to sort.

- **`void forward_list_reverse(ForwardList* list)`**  
  Reverses the order of the elements in the list.  
  - **Parameters**: `list` – The list to reverse.

#### **Relational Operators**

- **`bool forward_list_is_less(const ForwardList* list1, const ForwardList* list2)`**  
  Compares two lists to determine if the first is lexicographically less than the second.  
  - **Returns**: `true` if `list1` is less than `list2`, `false` otherwise.

- **`bool forward_list_is_greater(const ForwardList* list1, const ForwardList* list2)`**  
  Compares two lists to determine if the first is lexicographically greater than the second.  
  - **Returns**: `true` if `list1` is greater than `list2`, `false` otherwise.

- **`bool forward_list_is_equal(const ForwardList* list1, const ForwardList* list2)`**  
  Compares two lists for equality.  
  - **Returns**: `true` if `list1` is equal to `list2`, `false` otherwise.

- **`bool forward_list_is_less_or_equal(const ForwardList* list1, const ForwardList* list2)`**  
  Checks if the first list is less than or equal to the second list.  
  - **Returns**: `true` if `list1` is less than or equal to `list2`, `false` otherwise.

- **`bool forward_list_is_greater_or_equal(const ForwardList* list1, const ForwardList* list2)`**  
  Checks if the first list

 is greater than or equal to the second list.  
  - **Returns**: `true` if `list1` is greater than or equal to `list2`, `false` otherwise.

- **`bool forward_list_is_not_equal(const ForwardList* list1, const ForwardList* list2)`**  
  Compares two lists for inequality.  
  - **Returns**: `true` if `list1` is not equal to `list2`, `false` otherwise.


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