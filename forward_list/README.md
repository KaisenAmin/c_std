
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

### `ForwardList* forward_list_create(size_t itemSize)`
**Purpose**: Allocates and initializes a new empty singly-linked list.
**Parameters**:
- `itemSize`: The size in bytes of each element (must be > 0).
**Return Value**: Pointer to the newly created list, or NULL if `itemSize` is 0 or allocation fails.
**Usage Case**: Call once at the start to create a typed list before inserting any elements.

---

### `void forward_list_deallocate(ForwardList* list)`
**Purpose**: Frees all nodes and the list struct itself.
**Parameters**:
- `list`: Pointer to the list to free.
**Return Value**: None.
**Usage Case**: Call when the list is no longer needed to avoid memory leaks; safe to call with NULL.

---

### `void forward_list_clear(ForwardList* list)`
**Purpose**: Removes and frees all nodes, leaving the list empty but the struct valid for reuse.
**Parameters**:
- `list`: Pointer to the list to clear.
**Return Value**: None.
**Usage Case**: Use to reset a list to empty without destroying the list struct itself.

---

### `size_t forward_list_length(const ForwardList* list)`
**Purpose**: Returns the number of elements in the list.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: The element count, or 0 if `list` is NULL.
**Usage Case**: Use to query how many elements are currently stored before iterating or resizing.

---

### `size_t forward_list_max_size(const ForwardList* list)`
**Purpose**: Returns the theoretical maximum number of elements the list can hold.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Typically `SIZE_MAX` on the current platform, or `(size_t)-1` if `list` is NULL.
**Usage Case**: Use for bounds checking or portability checks when the list may grow very large.

---

### `bool forward_list_empty(const ForwardList* list)`
**Purpose**: Returns `true` if the list contains no elements or is NULL.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: `true` if the list is empty or NULL, `false` otherwise.
**Usage Case**: Use as a guard before calling `forward_list_front` or `forward_list_pop_front`.

---

### `void* forward_list_front(const ForwardList* list)`
**Purpose**: Returns a pointer to the value of the first element.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the first element's value, or NULL if the list is empty or NULL.
**Usage Case**: Use to peek at or read the head element without removing it.

---

### `void forward_list_push_front(ForwardList* list, void* value)`
**Purpose**: Copies `itemSize` bytes from `value` and prepends a new node at the head of the list.
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to the value to copy into the new head node.
**Return Value**: None.
**Usage Case**: Use to efficiently insert an element at the beginning of the list in O(1).

---

### `void forward_list_pop_front(ForwardList* list)`
**Purpose**: Removes and frees the first node.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Use to discard the head element; does nothing if the list is empty or NULL.

---

### `void forward_list_emplace_front(ForwardList* list, void* value)`
**Purpose**: Inserts a new element at the front by copying `itemSize` bytes; functionally equivalent to `push_front`, provided for C++ naming parity.
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to the value to copy into the new head node.
**Return Value**: None.
**Usage Case**: Use when code style prefers the `emplace` naming convention for front insertion.

---

### `void forward_list_emplace_after(ForwardList* list, ForwardListNode* pos, void* value)`
**Purpose**: Inserts a new element immediately after `pos`.
**Parameters**:
- `list`: Pointer to the list.
- `pos`: Pointer to the node after which the new element is inserted; pass the sentinel from `forward_list_before_begin` to insert at the head.
- `value`: Pointer to the value to copy into the new node.
**Return Value**: None.
**Usage Case**: Use for precise mid-list insertion when the predecessor node is already known.

---

### `void forward_list_insert_after(ForwardList* list, ForwardListNode* pos, void* value, size_t numValues)`
**Purpose**: Inserts `numValues` elements from the array pointed to by `value` after `pos`.
**Parameters**:
- `list`: Pointer to the list.
- `pos`: Pointer to the node after which elements are inserted; pass the sentinel from `forward_list_before_begin` to insert at the head.
- `value`: Pointer to the array of values to insert.
- `numValues`: Number of elements to insert from the array.
**Return Value**: None.
**Usage Case**: Use to bulk-insert multiple elements at a specific position in a single call.

---

### `void forward_list_erase_after(ForwardList* list, ForwardListNode* pos)`
**Purpose**: Removes and frees the node immediately after `pos`.
**Parameters**:
- `list`: Pointer to the list.
- `pos`: Pointer to the node whose successor will be removed.
**Return Value**: None.
**Usage Case**: Use to delete a known node during traversal when you hold a pointer to its predecessor.

---

### `void forward_list_assign(ForwardList* list, void* values, size_t numValues)`
**Purpose**: Clears the list and fills it with `numValues` copies from the `values` array.
**Parameters**:
- `list`: Pointer to the list.
- `values`: Pointer to the source array of elements.
- `numValues`: Number of elements to copy from the array.
**Return Value**: None.
**Usage Case**: Use to fully replace the list's contents with a new set of values in one call.

---

### `void forward_list_resize(ForwardList* list, size_t newSize)`
**Purpose**: Grows or shrinks the list to `newSize`; new slots when growing are zero-initialized.
**Parameters**:
- `list`: Pointer to the list.
- `newSize`: The desired number of elements after the operation.
**Return Value**: None.
**Usage Case**: Use to truncate an oversized list or extend it with zero-value placeholders.

---

### `void forward_list_swap(ForwardList* list1, ForwardList* list2)`
**Purpose**: Swaps the head pointer, size, and itemSize of two lists in O(1).
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: None.
**Usage Case**: Use to exchange the entire contents of two lists without copying any elements.

---

### `void forward_list_reverse(ForwardList* list)`
**Purpose**: Reverses the order of all elements in-place.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Use when the list must be iterated or processed in reverse order.

---

### `void forward_list_sort(ForwardList* list)`
**Purpose**: Sorts the elements in ascending order using merge sort.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Use before calling `forward_list_unique` or `forward_list_merge`, which both require a sorted list.

---

### `void forward_list_splice_after(ForwardList* list, ForwardListNode* pos, ForwardList* other)`
**Purpose**: Moves all nodes from `other` into `list`, inserting them after `pos`; `other` is empty after the call.
**Parameters**:
- `list`: Pointer to the destination list.
- `pos`: Pointer to the node in `list` after which `other`'s nodes are inserted; pass the sentinel from `forward_list_before_begin` to prepend.
- `other`: Pointer to the source list whose nodes are transferred.
**Return Value**: None.
**Usage Case**: Use to merge two lists by node transfer (no copies) at a specific insertion point.

---

### `void forward_list_remove(ForwardList* list, void* value)`
**Purpose**: Removes all nodes whose stored value byte-compares equal to `value`.
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to the value to match against each element.
**Return Value**: None.
**Usage Case**: Use to purge all occurrences of a known value from the list in a single pass.

---

### `void forward_list_remove_if(ForwardList* list, bool (*condition)(void*))`
**Purpose**: Removes every node for which `condition(node->value)` returns `true`.
**Parameters**:
- `list`: Pointer to the list.
- `condition`: Function pointer that receives a pointer to an element's value and returns `true` to remove it.
**Return Value**: None.
**Usage Case**: Use to filter the list with arbitrary predicate logic without manual traversal.

---

### `void forward_list_unique(ForwardList* list)`
**Purpose**: Removes consecutive duplicate elements whose values are byte-equal to the immediately preceding node.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Call after `forward_list_sort` to eliminate all duplicate values from the list.

---

### `void forward_list_merge(ForwardList* list1, ForwardList* list2)`
**Purpose**: Merges `list2` into `list1`, preserving sorted order; both lists must be sorted beforehand and `list2` is empty after the call.
**Parameters**:
- `list1`: Pointer to the destination sorted list.
- `list2`: Pointer to the source sorted list whose nodes are moved into `list1`.
**Return Value**: None.
**Usage Case**: Use to combine two independently sorted lists into one sorted list without extra allocation.

---

### `ForwardListNode* forward_list_before_begin(ForwardList* list)`
**Purpose**: Returns a pointer to the per-list sentinel node whose `next` is always refreshed to the current head.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the sentinel node, or NULL if `list` is NULL.
**Usage Case**: Pass to `insert_after`, `erase_after`, or `splice_after` to operate before the first element.

---

### `ForwardListNode* forward_list_begin(ForwardList* list)`
**Purpose**: Returns a pointer to the first node (head) of the list.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the head node, or NULL if the list is empty or NULL.
**Usage Case**: Use as the starting point in a forward iteration loop: `for (ForwardListNode* n = forward_list_begin(l); n != forward_list_end(l); n = n->next)`.

---

### `ForwardListNode* forward_list_end(ForwardList* list)`
**Purpose**: Returns NULL, the past-the-end sentinel for forward iteration.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: NULL.
**Usage Case**: Use as the loop termination condition when iterating with `forward_list_begin`.

---

### `bool forward_list_is_equal(const ForwardList* list1, const ForwardList* list2)`
**Purpose**: Returns `true` if both lists have the same length and every corresponding element is byte-equal.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if the lists are equal, `false` otherwise.
**Usage Case**: Use to compare two lists for exact content equality.

---

### `bool forward_list_is_not_equal(const ForwardList* list1, const ForwardList* list2)`
**Purpose**: Returns `true` if the lists differ in length or any corresponding element differs.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if the lists are not equal, `false` otherwise.
**Usage Case**: Use as the inequality counterpart to `forward_list_is_equal`.

---

### `bool forward_list_is_less(const ForwardList* list1, const ForwardList* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically less than `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` compares less than `list2`, `false` otherwise.
**Usage Case**: Use to establish ordering between two lists for sorting or priority comparisons.

---

### `bool forward_list_is_less_or_equal(const ForwardList* list1, const ForwardList* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically less than or equal to `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` compares less than or equal to `list2`, `false` otherwise.
**Usage Case**: Use in range or boundary checks where equality must also be accepted.

---

### `bool forward_list_is_greater(const ForwardList* list1, const ForwardList* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically greater than `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` compares greater than `list2`, `false` otherwise.
**Usage Case**: Use to determine which of two lists is larger in lexicographic order.

---

### `bool forward_list_is_greater_or_equal(const ForwardList* list1, const ForwardList* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically greater than or equal to `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` compares greater than or equal to `list2`, `false` otherwise.
**Usage Case**: Use in range or boundary checks where `list1` being equal to `list2` should also pass.

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
**Result**
```
(no output — element is pushed then immediately popped)
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

    /* before_begin() returns the per-list sentinel node (not NULL).
     * Its ->next is always refreshed to point to begin(). */
    ForwardListNode *bb = forward_list_before_begin(list);

    fmt_printf("before_begin is non-NULL:       %s\n", bb != NULL ? "yes" : "no");
    fmt_printf("before_begin()->next == begin(): %s\n",
               bb->next == forward_list_begin(list) ? "yes" : "no");

    forward_list_deallocate(list);
    return 0;
}
```
**Result**
```
before_begin is non-NULL:       yes
before_begin()->next == begin(): yes
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
10 20 
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
List1 after swap: 6 5 4 
List2 after swap: 3 2 1 
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