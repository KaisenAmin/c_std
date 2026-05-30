
# List Library

Author: amin tahmasebi
Release Date: 2023
License: ISC License


The List library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size List, offering similar functionality to `std::list` in C++.

## Compilation

To compile the List library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c list/list.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the List library in your project, include the `list.h` header file in your source code.

```c
#include "list/list.h"
```

## Functions Explanations

### `List* list_create(size_t itemSize, CompareFunction compare)`
**Purpose**: Allocates and initializes a new doubly-linked list.
**Parameters**:
- `itemSize`: The size of each element in bytes; must be greater than 0.
- `compare`: The element comparator used by `list_sort`, `list_remove`, and the relational operators; may be `NULL` when those operations are not needed.
**Return Value**: A pointer to the new list, or `NULL` if `itemSize` is 0 or allocation fails.
**Usage Case**: Call once at the start to create a typed list before pushing any elements.

---

### `void list_deallocate(List* list)`
**Purpose**: Frees all nodes and their element data, then frees the list struct itself.
**Parameters**:
- `list`: Pointer to the list to destroy; does nothing if `NULL`.
**Return Value**: None.
**Usage Case**: Call when the list is no longer needed to avoid memory leaks.

---

### `size_t list_length(const List* list)`
**Purpose**: Returns the number of elements currently in the list.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: The element count, or `0` if `list` is `NULL`.
**Usage Case**: Use to check how many items are stored before iterating or resizing.

---

### `bool list_empty(const List* list)`
**Purpose**: Reports whether the list contains no elements.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: `true` if the list contains no elements or is `NULL`, `false` otherwise.
**Usage Case**: Use as a guard before calling `list_front` or `list_pop_front` to avoid operating on an empty list.

---

### `void* list_front(const List* list)`
**Purpose**: Returns a pointer to the value of the first element.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the first element's value, or `NULL` if the list is empty or `NULL`.
**Usage Case**: Peek at the head of a queue-style list without removing the element.

---

### `void* list_back(const List* list)`
**Purpose**: Returns a pointer to the value of the last element.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the last element's value, or `NULL` if the list is empty or `NULL`.
**Usage Case**: Peek at the tail of a stack- or queue-style list without removing the element.

---

### `void list_push_front(List* list, void* value)`
**Purpose**: Copies `itemSize` bytes from `value` and prepends the new element at the front of the list.
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to the data to copy into the new front node.
**Return Value**: None.
**Usage Case**: Use when implementing a stack (LIFO) or when order requires new items to appear first.

---

### `void list_push_back(List* list, void* value)`
**Purpose**: Copies `itemSize` bytes from `value` and appends the new element at the back of the list.
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to the data to copy into the new back node.
**Return Value**: None.
**Usage Case**: Use when building a queue (FIFO) or accumulating items in insertion order.

---

### `void list_pop_front(List* list)`
**Purpose**: Removes and frees the first element.
**Parameters**:
- `list`: Pointer to the list; does nothing if the list is empty or `NULL`.
**Return Value**: None.
**Usage Case**: Dequeue the head element in a FIFO processing loop.

---

### `void list_pop_back(List* list)`
**Purpose**: Removes and frees the last element.
**Parameters**:
- `list`: Pointer to the list; does nothing if the list is empty or `NULL`.
**Return Value**: None.
**Usage Case**: Pop the top element in a LIFO stack implemented with this list.

---

### `void list_emplace_front(List* list, void* value)`
**Purpose**: Inserts an element at the front by storing `value` directly without an extra copy (the pointer itself is stored).
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to store directly in the new front node.
**Return Value**: None.
**Usage Case**: Use when the caller manages element lifetime and wants to avoid the copy overhead of `list_push_front`.

---

### `void list_emplace_back(List* list, void* value)`
**Purpose**: Inserts an element at the back by storing `value` directly without an extra copy.
**Parameters**:
- `list`: Pointer to the list.
- `value`: Pointer to store directly in the new back node.
**Return Value**: None.
**Usage Case**: Use when the caller manages element lifetime and wants to avoid the copy overhead of `list_push_back`.

---

### `void* list_insert(List* list, size_t index, void* value)`
**Purpose**: Inserts a copy of `value` at position `index` (0-based), shifting elements at `index` and beyond.
**Parameters**:
- `list`: Pointer to the list.
- `index`: Zero-based position at which to insert the new element.
- `value`: Pointer to the data to copy into the new node.
**Return Value**: A pointer to the inserted value, or `NULL` if the insertion fails.
**Usage Case**: Insert an element at an arbitrary position, such as maintaining a sorted-by-index structure.

---

### `void* list_erase(List* list, size_t index)`
**Purpose**: Removes the element at position `index`.
**Parameters**:
- `list`: Pointer to the list.
- `index`: Zero-based position of the element to remove.
**Return Value**: A pointer to the removed element's value (caller is responsible for freeing it if necessary), or `NULL` on failure.
**Usage Case**: Delete a specific element by position when the index is known.

---

### `void list_clear(List* list)`
**Purpose**: Removes and frees all elements, leaving the list empty but the list struct valid.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Reset a list for reuse without deallocating the list struct itself.

---

### `void list_assign(List* list, void* values, size_t numValues)`
**Purpose**: Replaces the entire list contents with copies of the `numValues` elements from the `values` array, clearing any previous contents first.
**Parameters**:
- `list`: Pointer to the list.
- `values`: Pointer to the source array of elements.
- `numValues`: Number of elements to copy from `values`.
**Return Value**: None.
**Usage Case**: Bulk-initialize or replace list contents from a C array in one call.

---

### `void list_resize(List* list, size_t newSize, void* defaultValue)`
**Purpose**: Grows or shrinks the list to `newSize`; if growing, appends copies of `defaultValue` for each new slot; if shrinking, removes elements from the back.
**Parameters**:
- `list`: Pointer to the list.
- `newSize`: The desired number of elements after the operation.
- `defaultValue`: Pointer to the value used to fill new slots when growing.
**Return Value**: None.
**Usage Case**: Pre-allocate or trim a list to a known size, filling gaps with a sentinel value.

---

### `void list_swap(List* list1, List* list2)`
**Purpose**: Swaps the full contents (head, tail, size, itemSize, comparators) of two lists in O(1).
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: None.
**Usage Case**: Exchange two lists cheaply without copying all elements, for example after building a temporary list.

---

### `void list_reverse(List* list)`
**Purpose**: Reverses the order of all elements in-place.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Flip iteration order after building a list by prepending, or to implement reverse-sorted output.

---

### `void list_sort(List* list)`
**Purpose**: Sorts the elements in-place using the comparator supplied at `list_create`.
**Parameters**:
- `list`: Pointer to the list; requires a non-NULL comparator set at creation time.
**Return Value**: None.
**Usage Case**: Sort an unsorted list before calling `list_unique` or `list_merge`.

---

### `void list_splice(List* dest, List* src, Node* pos)`
**Purpose**: Moves all nodes from `src` into `dest`, inserting them before `pos`; after the operation `src` is empty.
**Parameters**:
- `dest`: Pointer to the destination list.
- `src`: Pointer to the source list whose nodes are moved.
- `pos`: Node in `dest` before which the `src` nodes are inserted.
**Return Value**: None.
**Usage Case**: Concatenate or interleave two lists without copying element data.

---

### `void list_remove(List* list, void* value)`
**Purpose**: Removes all nodes whose stored value compares equal to `value` using the list's comparator.
**Parameters**:
- `list`: Pointer to the list; requires a non-NULL comparator.
- `value`: Pointer to the value to match against each element.
**Return Value**: None.
**Usage Case**: Delete every occurrence of a specific value from the list in one call.

---

### `void list_remove_if(List* list, ConditionFunction cond)`
**Purpose**: Removes every element for which `cond` returns `true`.
**Parameters**:
- `list`: Pointer to the list.
- `cond`: Predicate function called with a pointer to each element's value.
**Return Value**: None.
**Usage Case**: Filter out elements that satisfy an arbitrary condition, such as all values above a threshold.

---

### `void list_unique(List* list)`
**Purpose**: Removes consecutive duplicate elements; the list should be sorted first to eliminate all duplicates.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: None.
**Usage Case**: Deduplicate a sorted list after calling `list_sort`.

---

### `void list_merge(List* list1, List* list2)`
**Purpose**: Merges all nodes from `list2` into `list1`; both lists must already be sorted, and `list2` is empty after the call.
**Parameters**:
- `list1`: Pointer to the destination sorted list.
- `list2`: Pointer to the source sorted list.
**Return Value**: None.
**Usage Case**: Combine two pre-sorted lists into one sorted list without extra allocation.

---

### `Node* list_begin(const List* list)`
**Purpose**: Returns a pointer to the first node (head) for use in forward iteration.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the head node, or `NULL` if the list is empty or `NULL`.
**Usage Case**: Start a forward loop: `for (Node* n = list_begin(l); n != list_end(l); n = n->next)`.

---

### `Node* list_end(const List* list)`
**Purpose**: Returns `NULL`, which serves as the past-the-end sentinel for forward iteration.
**Parameters**:
- `list`: Pointer to the list (parameter accepted for symmetry; return value is always `NULL`).
**Return Value**: `NULL`.
**Usage Case**: Use as the loop termination condition in a forward iteration over the list.

---

### `Node* list_rbegin(const List* list)`
**Purpose**: Returns a pointer to the last node (tail) for reverse iteration.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: Pointer to the tail node, or `NULL` if the list is empty or `NULL`.
**Usage Case**: Start a reverse loop: `for (Node* n = list_rbegin(l); n != list_rend(l); n = n->prev)`.

---

### `Node* list_rend(const List* list)`
**Purpose**: Returns `NULL`, the past-the-beginning sentinel for reverse iteration.
**Parameters**:
- `list`: Pointer to the list (parameter accepted for symmetry; return value is always `NULL`).
**Return Value**: `NULL`.
**Usage Case**: Use as the loop termination condition in a reverse iteration over the list.

---

### `const Node* list_cbegin(const List* list)`
**Purpose**: Same as `list_begin` but returns a `const Node*`, signalling read-only intent.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: `const` pointer to the head node, or `NULL` if the list is empty or `NULL`.
**Usage Case**: Iterate forward over a list in a context where elements must not be modified.

---

### `const Node* list_cend(const List* list)`
**Purpose**: Same as `list_end` but returns a `const Node*`.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: `NULL` (as `const Node*`).
**Usage Case**: Termination sentinel for a const-correct forward iteration loop.

---

### `const Node* list_crbegin(const List* list)`
**Purpose**: Same as `list_rbegin` but returns a `const Node*`.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: `const` pointer to the tail node, or `NULL` if the list is empty or `NULL`.
**Usage Case**: Iterate in reverse over a list in a read-only context.

---

### `const Node* list_crend(const List* list)`
**Purpose**: Same as `list_rend` but returns a `const Node*`.
**Parameters**:
- `list`: Pointer to the list.
**Return Value**: `NULL` (as `const Node*`).
**Usage Case**: Termination sentinel for a const-correct reverse iteration loop.

---

### `bool list_is_equal(const List* list1, const List* list2)`
**Purpose**: Returns `true` if both lists have the same length and every corresponding element compares equal via the comparator.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if the lists are element-wise equal, `false` otherwise.
**Usage Case**: Assert that two lists hold identical contents after a copy or merge operation.

---

### `bool list_is_not_equal(const List* list1, const List* list2)`
**Purpose**: Returns `true` if the two lists differ in length or any corresponding elements differ.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if the lists are not equal, `false` otherwise.
**Usage Case**: Detect that two lists have diverged, for example after a modification to one of them.

---

### `bool list_is_less(const List* list1, const List* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically less than `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` is lexicographically less than `list2`, `false` otherwise.
**Usage Case**: Order two lists, for example when using them as keys in a sorted structure.

---

### `bool list_is_less_or_equal(const List* list1, const List* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically less than or equal to `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` is lexicographically less than or equal to `list2`, `false` otherwise.
**Usage Case**: Use in range checks or sorting predicates where equality is acceptable.

---

### `bool list_is_greater(const List* list1, const List* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically greater than `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` is lexicographically greater than `list2`, `false` otherwise.
**Usage Case**: Determine which of two lists sorts later, for example when implementing a priority scheme.

---

### `bool list_is_greater_or_equal(const List* list1, const List* list2)`
**Purpose**: Returns `true` if `list1` is lexicographically greater than or equal to `list2`.
**Parameters**:
- `list1`: Pointer to the first list.
- `list2`: Pointer to the second list.
**Return Value**: `true` if `list1` is lexicographically greater than or equal to `list2`, `false` otherwise.
**Usage Case**: Use in sorting predicates or assertions where equality with the second list is also acceptable.

---

## Examples 

## Example 1: Create List and Add Elements Using `list_push_back`

```c
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) {
        list_push_back(myList, &values[i]);
    }

    list_deallocate(myList);
    return 0;
}
```

---

## Example 2: Add Elements Using `list_push_front`

```c
#include <string.h>
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_string(const void* a, const void* b){
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;

    return strcmp(str_a, str_b);
}

int main() {
    List *myList = list_create(sizeof(char*), compare_string);
    const char* values[] = {"c++", "C", "Python", "Golang", "Rust"};

    for (int i = 0; i < 5; ++i) { 
        list_push_front(myList, &values[i]);
    }
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next){
        char* value = *(char**)node->value;
        fmt_printf("%s\n", value);
    }

    list_deallocate(myList);
    return 0;
}

```
**Result**
```
Rust
Golang
Python
C
c++
```

---

## Example 3: Pop Elements from Front `list_pop_front`

```c
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_front(myList, &value);
    list_pop_front(myList);

    list_deallocate(myList);
    return 0;
}
```

---

## Example 4: Pop Elements from Back with `list_pop_back`

```c
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_back(myList, &value);
    list_pop_back(myList);

    list_deallocate(myList);
    return 0;
}
```

---

## Example 5: Insert Element at Specific Position with `list_insert`

```c
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_insert(myList, 0, &value); // Insert at the beginning

    list_deallocate(myList);
    return 0;
}
```

---

## Example 6: Erase Element at Specific Position with `list_erase`

```c
#include "list/list.h"
#include <stdlib.h>

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_back(myList, &value);
    free(list_erase(myList, 0)); // Erase the first element and free the returned value

    list_deallocate(myList);
    return 0;
}
```

---

## Example 7: Clearing the List 

```c
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_back(myList, &value);

    list_deallocate(myList);
    return 0;
}
```

---

## Example 8: Resize the List with `list_resize`

```c
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *list1 = list_create(sizeof(int), compare_ints);

    int values[] = {50, 40, 30, 20, 10};
    for (int i = 0; i < 5; ++i) { 
        list_push_back(list1, &values[i]);
    }
    list_resize(list1, 10, 0);

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next){
        int* value = (int*)node->value;
        fmt_printf("%d\n", *value);
    }

    list_deallocate(list1);
    return 0;
}
```
**Result**
```
50
40
30
20
10
0
0
0
0
0
```

---

## Example 9: Swap Two Lists with `list_swap`

```c
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *list1 = list_create(sizeof(int), compare_ints);
    List *list2 = list_create(sizeof(int), compare_ints);
    int values[] = {50, 40, 30, 20, 10};
    int values2[] = {100, 200, 300, 400, 500};

    for (int i = 0; i < 5; ++i) { 
        list_push_back(list1, &values[i]);
    }
    for (int i = 0; i < 5; ++i) { 
        list_push_back(list2, &values2[i]);
    }
    list_swap(list1, list2); // Swap list1 and list2

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next){
        int* value = (int*)node->value;
        fmt_printf("%d\n", *value);
    }

    list_deallocate(list1);
    list_deallocate(list2);

    return 0;
}
```
**Result**
```
100
200
300
400
500
```

---

## Example 10: Reverse a List with `list_reverse`

```c
#include "fmt/fmt.h"
#include "list/list.h"


static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {50, 40, 30, 20, 10};

    for (int i = 0; i < 5; ++i) { 
        list_push_back(myList, &values[i]);
    }
    list_reverse(myList); // Reverse the list_
    
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next) {
        int* value = (int*)node->value;
        fmt_printf("%d\n", *value);
    }

    list_deallocate(myList);
    return 0;
}
```
**Result**
```
10
20
30
40
50
```

---

## Example 11: Sort a List with `list_sort`

```c
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {50, 40, 30, 20, 10};

    for (int i = 0; i < 5; ++i) { 
        list_push_back(myList, &values[i]);
    }
    list_sort(myList); // Sort the list
    
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next) {
        int* value = (int*)node->value;
        fmt_printf("%d\n", *value);
    }

    list_deallocate(myList);
    return 0;
}
```
**Result**
```
10
20
30
40
50
```

---

## Example 12: Check if List is Empty with `list_empty`

```c
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    bool isEmpty = list_empty(myList); // Check if list is empty

    fmt_printf("%s\n", isEmpty? "Yes its empty": "No its not empty");

    list_deallocate(myList);
    return 0;
}
```
**Result**
```
Yes its empty
```

---

## Example 13: Get the Length of the List with `list_length`

```c
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_back(myList, &value);
    size_t length = list_length(myList); // Get the length of the list

    fmt_printf("Size of List %zu\n", length);
    
    list_deallocate(myList);
    return 0;
}
```
**Result**
```
Size of List 1
```

---

### Example 14: Assign Values to List with `list_assign`

```c
#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {1, 2, 3, 4, 5};

    list_assign(myList, values, 5); // Assign values to list
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next){
        int* value = (int*)node->value;
        fmt_printf("%d\n", *value);
    }

    list_deallocate(myList);
    return 0;
}
```
**Result**
```
1
2
3
4
5
```

---

### Example 15: Merge Two Lists with `list_merge`

```c
#include "list/list.h"


static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *list1 = list_create(sizeof(int), compare_ints);
    List *list2 = list_create(sizeof(int), compare_ints);
    int value1 = 10, value2 = 20;

    list_push_back(list1, &value1);
    list_push_back(list2, &value2);
    list_merge(list1, list2); // Merge list2 into list1

    list_deallocate(list1);
    list_deallocate(list2);
    return 0;
}
```

---

### Example 16 : Storing Strings in a List and Sorting

```c
// Function to compare two strings in the list
#include "fmt/fmt.h"
#include "list/list.h"
#include "string/std_string.h"

static int compare_strings(const void* a, const void* b) {
    String* strA = *(String**)a;
    String* strB = *(String**)b;

    return string_is_less(strA, strB) ? -1 : string_is_greater(strA, strB);
}

int main() {
    List* stringList = list_create(sizeof(String*), compare_strings);
    String* str1 = string_create("Apple");
    String* str2 = string_create("Banana");
    String* str3 = string_create("Cherry");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);
    list_sort(stringList); // Sort the list of strings

    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) {
        String* str = *(String**)node->value;
        fmt_printf("%s\n", string_c_str(str));
    }

    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    list_deallocate(stringList);

    return 0;
}
```
**Result**
```
Apple
Banana
Cherry
```

---

### Example 17: Merging Two Lists of Strings

```c
#include "fmt/fmt.h"
#include "list/list.h"
#include "string/std_string.h"

int main() {
    List* list1 = list_create(sizeof(String*), NULL);
    List* list2 = list_create(sizeof(String*), NULL);

    String* str1 = string_create("Hello");
    String* str2 = string_create("World");
    String* str3 = string_create("Example");
    String* str4 = string_create("Text");

    list_push_back(list1, &str1);
    list_push_back(list1, &str2);
    list_push_back(list2, &str3);
    list_push_back(list2, &str4);
    list_merge(list1, list2); // Merge list2 into list1

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next) {
        String* str = *(String**)node->value;
        fmt_printf("%s\n", string_c_str(str));
    }

    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    string_deallocate(str4);
    
    list_deallocate(list1);
    list_deallocate(list2);

    return 0;
}
```
**Result**
```
Hello
World
Example
Text
```

---

### Example 18: Filtering Strings from a List

```c
#include "fmt/fmt.h"
#include "list/list.h"
#include "string/std_string.h"

// Condition function to filter out short strings
static bool filter_short_strings(void* value) {
    String* str = *(String**)value;
    return string_length(str) < 5;
}

int main() {
    List* stringList = list_create(sizeof(String*), NULL);
    String* str1 = string_create("Apple");
    String* str2 = string_create("Banana");
    String* str3 = string_create("Kiwi");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);
    list_remove_if(stringList, filter_short_strings); // Remove short strings

    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) {
        String* str = *(String**)node->value;
        fmt_printf("%s\n", string_c_str(str));
    }

    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    list_deallocate(stringList);

    return 0;
}
```
**Result**
```
Apple
Banana
```

---

### Example 19: Concatenating All Strings in a List

```c
#include "fmt/fmt.h"
#include "list/list.h"
#include "string/std_string.h"

int main(){
    List* stringList = list_create(sizeof(String*), NULL);
    String* concatenated = string_create("");
    String* str1 = string_create("Hello, ");
    String* str2 = string_create("world");
    String* str3 = string_create("!");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next){
        String* str = *(String**)node->value;
        string_append(concatenated, string_c_str(str));
    }
    fmt_printf("Concatenated String: %s\n", string_c_str(concatenated));

    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3); 
    string_deallocate(concatenated);
    list_deallocate(stringList);

    return 0;
}
```
**Result**
```
Concatenated String: Hello, world!
```

---

### Example 20: Reversing Each String in a List

```c
#include "fmt/fmt.h"
#include "list/list.h"
#include "string/std_string.h"

int main() {
    List* stringList = list_create(sizeof(String*), NULL);
    String* str1 = string_create("Hello");
    String* str2 = string_create("World");
    String* str3 = string_create("Example");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Reverse each string
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) {
        String* str = *(String**)node->value;
        String* reversed = string_create(""); 

        for (int i = string_length(str) - 1; i >= 0; --i) {
            string_push_back(reversed, string_c_str(str)[i]);
        }
        fmt_printf("Reversed String: %s\n", string_c_str(reversed));
        string_deallocate(reversed);
    }

    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    list_deallocate(stringList);

    return 0;
}

```
**Result**
```
Reversed String: olleH
Reversed String: dlroW
Reversed String: elpmaxE
```

---

## Example 21 : Relational operators in List 

```c
#include "fmt/fmt.h"
#include "list/list.h"

// Function to compare integers for the list
static int int_compare(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

// Function to add elements to a list
void add_elements_to_list(List* list, int* elements, size_t numElements) {
    for (size_t i = 0; i < numElements; ++i) {
        list_push_back(list, &elements[i]);
    }
}

int main() {
    List* list1 = list_create(sizeof(int), int_compare);
    List* list2 = list_create(sizeof(int), int_compare);
    int elements1[] = {1, 2, 3, 4, 5};
    int elements2[] = {1, 2, 3, 4, 6};

    add_elements_to_list(list1, elements1, 5);
    add_elements_to_list(list2, elements2, 5);

    // Perform relational comparisons
    fmt_printf("List 1 is less than List 2: %s\n", list_is_less(list1, list2) ? "true" : "false");
    fmt_printf("List 1 is greater than List 2: %s\n", list_is_greater(list1, list2) ? "true" : "false");
    fmt_printf("List 1 is equal to List 2: %s\n", list_is_equal(list1, list2) ? "true" : "false");
    fmt_printf("List 1 is less than or equal to List 2: %s\n", list_is_less_or_equal(list1, list2) ? "true" : "false");
    fmt_printf("List 1 is greater than or equal to List 2: %s\n", list_is_greater_or_equal(list1, list2) ? "true" :"false");
    fmt_printf("List 1 is not equal to List 2: %s\n", list_is_not_equal(list1, list2) ? "true" : "false");

    list_deallocate(list1);
    list_deallocate(list2);

    return 0;
}
```
**Result**
```
List 1 is less than List 2: true
List 1 is greater than List 2: false
List 1 is equal to List 2: false
List 1 is less than or equal to List 2: true
List 1 is greater than or equal to List 2: false
List 1 is not equal to List 2: true
```

---

## Example 22 : add and remove child 

```c
#include "list/list.h"
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>


typedef struct {
    const char* name;
    Node registry_links;
} Child;

Child* create_child(const char* name) {
    Child* new_child = (Child*)malloc(sizeof(Child));
    if (!new_child) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for new Child.\n");
        exit(-1);
    }

    new_child->name = string_strdup(name); 
    new_child->registry_links.next = NULL;
    new_child->registry_links.prev = NULL;
    new_child->registry_links.value = new_child;

    return new_child;
}

void child_unlink(List* list, Child* c) {
    list_remove(list, c);
}

static int compare_nodes(const void* a, const void* b) {
    const Child* Child_a = (const Child*)a;
    const Child* Child_b = (const Child*)b;

    return string_strcmp(Child_a->name, Child_b->name);
}

void deallocate(List* children_registry, Child** children, size_t num_children) {
    for (size_t i = 0; i < num_children; ++i) {
        free((void*)children[i]->name);
        free(children[i]);
    }

    list_deallocate(children_registry);
}

int main() {
    const char* names[] = {"Marry", "Bob", "Sally"};
    size_t num_children = sizeof(names) / sizeof(names[0]);

    List* children_registry = list_create(sizeof(Child), compare_nodes);
    Child* children[num_children];

    for (size_t i = 0; i < num_children; ++i) {
        children[i] = create_child(names[i]);
        list_push_back(children_registry, children[i]);
    }

    child_unlink(children_registry, children[1]);

    for (Node* node = list_begin(children_registry); node != list_end(children_registry); node = node->next) {
        Child* c = (Child*)node->value;
        fmt_printf("%s\n", c->name);
    }

    deallocate(children_registry, children, num_children);

    return 0;
}
```
**Result**
```
Marry
Sally
```

---

## License

This project is open-source and available under [ISC License].