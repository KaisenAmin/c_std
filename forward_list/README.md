
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

### Functions Explanations

### `ForwardList *forward_list_create(size_t itemSize)`
- **Purpose**: Creates a new singly linked list (`ForwardList`) that stores elements of a specified size.
- **Parameters**:
  - `itemSize`: Size of each element in bytes. Must be greater than 0.
- **Return**: Pointer to the newly created list, or `NULL` if memory allocation fails.

---

### `void *forward_list_front(const ForwardList *list)`
- **Purpose**: Retrieves the value of the first element in the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: Pointer to the value of the first element, or `NULL` if the list is empty or `NULL`.

---

### `void forward_list_push_front(ForwardList *list, void *value)`
- **Purpose**: Adds a new element to the front of the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
  - `value`: Pointer to the value to insert at the front of the list.
- **Return**: None.

---

### `void forward_list_pop_front(ForwardList *list)`
- **Purpose**: Removes the first element from the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: None.

---

### `void forward_list_clear(ForwardList *list)`
- **Purpose**: Removes all elements from the `ForwardList`, resetting it to an empty state.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: None.

---

### `void forward_list_deallocate(ForwardList *list)`
- **Purpose**: Frees all memory associated with the `ForwardList`, including nodes and the list itself.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: None.

---

### `void forward_list_assign(ForwardList *list, void *values, size_t numValues)`
- **Purpose**: Replaces the contents of the `ForwardList` with values from an array.
- **Parameters**:
  - `list`: Pointer to the list.
  - `values`: Pointer to the array of values to insert.
  - `numValues`: Number of values to insert.
- **Return**: None.

---

### `void forward_list_emplace_front(ForwardList *list, void *value)`
- **Purpose**: Inserts a new element at the front of the `ForwardList` without copying the value.
- **Parameters**:
  - `list`: Pointer to the list.
  - `value`: Pointer to the value to emplace.
- **Return**: None.

---

### `void forward_list_emplace_after(ForwardList *list, ForwardListNode *pos, void *value)`
- **Purpose**: Inserts a new element directly after the specified node (`pos`) in the `ForwardList` without copying the value.
- **Parameters**:
  - `list`: Pointer to the list.
  - `pos`: Pointer to the node after which the new value will be inserted.
  - `value`: Pointer to the value to emplace.
- **Return**: None.

---

### `void forward_list_insert_after(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues)`
- **Purpose**: Inserts multiple elements into the `ForwardList` after the specified node (`pos`).
- **Parameters**:
  - `list`: Pointer to the list.
  - `pos`: Pointer to the node after which the elements will be inserted.
  - `value`: Pointer to the array of values.
  - `numValues`: Number of elements to insert.
- **Return**: None.

---

### `void forward_list_erase_after(ForwardList *list, ForwardListNode *pos)`
- **Purpose**: Removes the node immediately after the specified node (`pos`) in the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
  - `pos`: Pointer to the node before the node to be erased.
- **Return**: None.

---

### `void forward_list_swap(ForwardList *list1, ForwardList *list2)`
- **Purpose**: Swaps the contents of two `ForwardLists`.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: None.

---

### `void forward_list_resize(ForwardList *list, size_t newSize)`
- **Purpose**: Resizes the `ForwardList` to contain `newSize` elements.
- **Parameters**:
  - `list`: Pointer to the list.
  - `newSize`: New desired size of the list.
- **Return**: None.

---

### `void forward_list_splice_after(ForwardList *list, ForwardListNode *pos, ForwardList *other)`
- **Purpose**: Moves all elements from the `other` list into the current `list` after the node `pos`.
- **Parameters**:
  - `list`: Pointer to the destination list.
  - `pos`: Pointer to the node in `list` after which the elements from `other` will be inserted.
  - `other`: Pointer to the source list whose elements will be moved.
- **Return**: None.

---

### `void forward_list_remove(ForwardList *list, void *value)`
- **Purpose**: Removes all elements in the `ForwardList` that match the specified `value`.
- **Parameters**:
  - `list`: Pointer to the list.
  - `value`: Pointer to the value to remove.
- **Return**: None.

---

### `void forward_list_remove_if(ForwardList *list, bool (*condition)(void*))`
- **Purpose**: Removes all elements from the `ForwardList` that satisfy the condition specified by the `condition` function.
- **Parameters**:
  - `list`: Pointer to the list.
  - `condition`: Function pointer to the condition function to test each element.
- **Return**: None.

---

### `void forward_list_unique(ForwardList *list)`
- **Purpose**: Removes consecutive duplicate elements from the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: None.

---

### `void forward_list_merge(ForwardList *list1, ForwardList *list2)`
- **Purpose**: Merges two sorted `ForwardLists` (`list1` and `list2`) into one sorted list stored in `list1`.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list, which becomes empty after the merge.
- **Return**: None.

---

### `void forward_list_sort(ForwardList *list)`
- **Purpose**: Sorts the elements of the `ForwardList` in ascending order using merge sort.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: None.

---

### `void forward_list_reverse(ForwardList *list)`
- **Purpose**: Reverses the order of the elements in the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: None.

---

### `size_t forward_list_length(const ForwardList *list)`
- **Purpose**: Returns the number of elements in the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: Number of elements in the list.

---

### `size_t forward_list_max_size(const ForwardList *list)`
- **Purpose**: Returns the theoretical maximum number of elements the `ForwardList` can hold.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: The maximum number of elements the list can hold, or `(size_t)-1` in case of error.

---

### `ForwardListNode *forward_list_before_begin(ForwardList *list)`
- **Purpose**: Returns a pointer to the node before the first node of the `ForwardList`, which is always `NULL` for singly linked lists.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: Always `NULL`.

---

### `ForwardListNode *forward_list_begin(ForwardList *list)`
- **Purpose**: Returns a pointer to the first node (head) of the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: Pointer to the first node, or `NULL` if the list is empty.

---

### `ForwardListNode *forward_list_end(ForwardList *list)`
- **Purpose**: Returns `NULL`, representing the end of the `ForwardList`.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: `NULL`.

---

### `bool forward_list_is_less(const ForwardList *list1, const ForwardList *list2)`
- **Purpose**: Compares two `ForwardLists` lexicographically to check if `list1` is less than `list2`.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: `true` if `list1` is lexicographically less than `list2`, `false` otherwise.

---

### `bool forward_list_is_greater(const ForwardList *list1, const ForwardList *list2)`
- **Purpose**: Compares two `ForwardLists` lexicographically to check if `list1` is greater than `list2`.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: `true` if `list1` is lexicographically greater than `list2`, `false` otherwise.

---

### `bool forward_list_is_equal(const ForwardList *list1, const ForwardList *list2)`
- **Purpose**: Compares two `ForwardLists` for equality.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: `true` if both lists are equal, `false` otherwise.

---

### `bool forward_list_is_less_or_equal(const ForwardList *list1, const ForwardList *list2)`
- **Purpose**: Checks if `list1` is lexicographically less than or equal to `list2`.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: `true` if `list1` is less than or equal to `list2`, `false` otherwise.

---

### `bool forward_list_is_greater_or_equal(const ForwardList *list1, const ForwardList *list2)`
- **Purpose**: Checks if `list1` is lexicographically greater than or equal to `list2`.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: `true` if `list1` is greater than or equal to `list2`, `false` otherwise.

---

### `bool forward_list_is_not_equal(const ForwardList *list1, const ForwardList *list2)`
- **Purpose**: Checks if two `ForwardLists` are not equal.
- **Parameters**:
  - `list1`: Pointer to the first list.
  - `list2`: Pointer to the second list.
- **Return**: `true` if the lists are not equal, `false` otherwise.

---

### `bool forward_list_empty(const ForwardList *list)`
- **Purpose**: Checks if the `ForwardList` is empty.
- **Parameters**:
  - `list`: Pointer to the list.
- **Return**: `true` if the list is empty, `false` otherwise.

---


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
**Result**
```
10
```

---

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

---

### Example 3: Access Front Element with `forward_list_front`
```c
#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);
    int *frontValue = (int*)forward_list_front(list);

    fmt_printf("Front value: %d\n", *frontValue);

    forward_list_deallocate(list);
    return 0;
}
```
**Result**
```
Front value: 10
```

---

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
**Result**
```
10 20 30 

```

---

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
**Result**
```
Is list empty? Yes
```

---

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
**Result**
```
List length: 3
```

---

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
**Result**
```
10 20 30
```

---

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
**Result**
```
Node: 0000000000000000
```

---

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
**Result**
```
10
20
30
```

---

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
**Result**
```
Max size: 18446744073709551615
```

---

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
**Result**
```
10
```

---

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
**Result**
```
10 10
```

---

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
**Result**
```
20
10
```

---

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
**Result**
```
10 30
```

---

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
**Result**
```
List1 after swap: 
6 5 4
List2 after swap: 
3 2 1
```

---

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
**Result**
```
Size of the list after resizing: 3
```

---

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
**Result**
```
10 20 30
```

---

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
**Result**
```
10 30
```

---

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
**Result**
```
15 25
```

---

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
**Result**
```
10 20 30
```

---

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
**Result**
```
List1 is less than List2
List1 is not equal to List2
```

---

## License

This project is open-source and available under [ISC License].