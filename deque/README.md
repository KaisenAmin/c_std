# Deque Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

The Deque library provides a generic container that mimics the functionality of `std::deque` in C++. It offers a dynamic-size double-ended queue (deque) that allows fast insertion and removal of elements from both ends. 

## Compilation

To compile the Deque library along with your main program, use the following GCC command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./deque/deque.c 
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Deque library in your project, include the `deque.h` header file in your source code:

```c
#include "deque/deque.h"
```

## Function Descriptions

### `Deque* deque_create(size_t itemSize)`
**Purpose**: Allocates a new empty deque that holds elements of `itemSize` bytes.
**Parameters**:
- `itemSize`: Size in bytes of each element stored in the deque; must be greater than 0.
**Return Value**: Pointer to the newly created deque, or `NULL` on OOM or if `itemSize` is 0.
**Usage Case**: Call once at the start of any program that needs a double-ended queue, passing `sizeof` your element type.

---

### `void deque_clear(Deque* deque)`
**Purpose**: Removes all elements and resets the deque to its initial empty state, but keeps the deque struct itself valid for reuse.
**Parameters**:
- `deque`: Pointer to the deque to clear.
**Return Value**: None.
**Usage Case**: Use when you want to empty a deque and refill it without destroying and recreating the deque object.

---

### `void deque_deallocate(Deque* deque)`
**Purpose**: Frees all element data, all blocks, the block-pointer array, and the deque struct itself.
**Parameters**:
- `deque`: Pointer to the deque to free.
**Return Value**: None.
**Usage Case**: Call at the end of a deque’s lifetime to release all associated memory; the pointer must not be used after this call.

---

### `size_t deque_length(const Deque* deque)`
**Purpose**: Returns the number of elements currently in the deque.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: The current element count, or `0` if `deque` is NULL.
**Usage Case**: Use to query how many elements are stored before iterating or indexing.

---

### `size_t deque_max_size(const Deque* deque)`
**Purpose**: Returns the theoretical maximum number of elements the deque can hold.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: `SIZE_MAX`, or `0` if `deque` is NULL.
**Usage Case**: Use for upper-bound checks or to mirror the interface of C++ `std::deque::max_size`.

---

### `bool deque_empty(const Deque* deque)`
**Purpose**: Reports whether the deque contains no elements.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: `true` if the deque has no elements or is NULL, `false` otherwise.
**Usage Case**: Use as a guard before calling `deque_front`, `deque_back`, or `deque_pop_front`/`deque_pop_back`.

---

### `void deque_resize(Deque* deque, size_t newSize)`
**Purpose**: Grows or shrinks the deque to exactly `newSize` elements.
**Parameters**:
- `deque`: Pointer to the deque to resize.
- `newSize`: The desired number of elements after the call.
**Return Value**: None.
**Usage Case**: Use to truncate a deque or to pre-extend it with zero-initialized slots before filling them in.

---

### `void deque_shrink_to_fit(Deque* deque)`
**Purpose**: Releases any over-allocated blocks so that memory usage matches the current element count exactly.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: None.
**Usage Case**: Call after a large batch of removals when memory efficiency is important.

---

### `void* deque_front(const Deque* deque)`
**Purpose**: Returns a pointer to the first element without removing it.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: Pointer to the first element, or `NULL` if the deque is empty or NULL.
**Usage Case**: Peek at the front element before deciding whether to pop it.

---

### `void* deque_back(const Deque* deque)`
**Purpose**: Returns a pointer to the last element without removing it.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: Pointer to the last element, or `NULL` if the deque is empty or NULL.
**Usage Case**: Peek at the back element before deciding whether to pop it.

---

### `void* deque_at(const Deque* deque, size_t index)`
**Purpose**: Returns a pointer to the element at the given index.
**Parameters**:
- `deque`: Pointer to the deque.
- `index`: Zero-based position of the element to retrieve.
**Return Value**: Pointer to the element at `index`, or `NULL` if `index` is out of bounds or `deque` is NULL.
**Usage Case**: Use for random access to any element by position.

---

### `void deque_push_front(Deque* deque, void* item)`
**Purpose**: Copies `itemSize` bytes from `item` and prepends the new element at the front.
**Parameters**:
- `deque`: Pointer to the deque.
- `item`: Pointer to the data to copy into the new front element.
**Return Value**: None.
**Usage Case**: Use when implementing a queue or stack that requires efficient O(1) prepending.

---

### `void deque_push_back(Deque* deque, const void* item)`
**Purpose**: Copies `itemSize` bytes from `item` and appends the new element at the back.
**Parameters**:
- `deque`: Pointer to the deque.
- `item`: Pointer to the data to copy into the new back element.
**Return Value**: None.
**Usage Case**: The standard way to add elements to the end of the deque.

---

### `void deque_pop_front(Deque* deque)`
**Purpose**: Removes the front element, adjusting the front index and freeing an empty leading block if one becomes vacant.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: None.
**Usage Case**: Use to consume elements from the front, e.g., when processing a FIFO queue.

---

### `void deque_pop_back(Deque* deque)`
**Purpose**: Removes the back element, adjusting the back index and freeing an empty trailing block if one becomes vacant.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: None.
**Usage Case**: Use to remove the last element, e.g., when implementing a stack or undo buffer.

---

### `void deque_emplace_front(Deque* deque, void* item)`
**Purpose**: Inserts an element at the front without an extra copy by delegating to `deque_push_front`.
**Parameters**:
- `deque`: Pointer to the deque.
- `item`: Pointer to the data to place at the front.
**Return Value**: None.
**Usage Case**: Use in place of `deque_push_front` when in-place construction semantics are desired for consistency with C++ `emplace_front`.

---

### `void deque_emplace_back(Deque* deque, void* item)`
**Purpose**: Inserts an element at the back without an extra copy by delegating to `deque_push_back`.
**Parameters**:
- `deque`: Pointer to the deque.
- `item`: Pointer to the data to place at the back.
**Return Value**: None.
**Usage Case**: Use in place of `deque_push_back` when in-place construction semantics are desired for consistency with C++ `emplace_back`.

---

### `void deque_emplace(Deque* deque, size_t index, void* item)`
**Purpose**: Inserts an element at `index`, delegating to `deque_emplace_front` or `deque_emplace_back` for boundary indices and shifting elements otherwise.
**Parameters**:
- `deque`: Pointer to the deque.
- `index`: Zero-based position at which to insert the new element.
- `item`: Pointer to the data to insert.
**Return Value**: None.
**Usage Case**: Use for arbitrary-position in-place insertion when the exact index is known at call time.

---

### `void deque_insert(Deque* deque, size_t index, void* item)`
**Purpose**: Inserts a copy of `item` at position `index`, shifting elements at `index` and beyond one position toward the back.
**Parameters**:
- `deque`: Pointer to the deque.
- `index`: Zero-based position at which to insert the element.
- `item`: Pointer to the data to copy and insert.
**Return Value**: None.
**Usage Case**: Use to splice a new value into the middle of the deque at a known index.

---

### `void deque_erase(Deque* deque, size_t index)`
**Purpose**: Removes the element at `index`, shifting elements beyond `index` one position toward the front and freeing any empty blocks.
**Parameters**:
- `deque`: Pointer to the deque.
- `index`: Zero-based position of the element to remove.
**Return Value**: None.
**Usage Case**: Use to delete a specific element by position without destroying the entire deque.

---

### `void deque_assign(Deque* deque, size_t n, void* val)`
**Purpose**: Clears the deque and fills it with `n` copies of `val`.
**Parameters**:
- `deque`: Pointer to the deque.
- `n`: Number of copies to fill.
- `val`: Pointer to the value to copy into each element.
**Return Value**: None.
**Usage Case**: Use to reinitialize a deque with a uniform value, equivalent to `deque_clear` followed by `n` push_backs of `val`.

---

### `void deque_swap(Deque* deque, Deque* otherDeque)`
**Purpose**: Swaps all internal state (blocks, size, indices, itemSize) between two deques in O(1).
**Parameters**:
- `deque`: Pointer to the first deque.
- `otherDeque`: Pointer to the second deque.
**Return Value**: None.
**Usage Case**: Use to exchange the contents of two deques cheaply without copying elements.

---

### `DequeIterator deque_begin(const Deque* deque)`
**Purpose**: Returns an iterator pointing to the first element.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` positioned at the first element; signals an empty deque if the deque is empty.
**Usage Case**: Use as the starting point for a forward iteration loop.

---

### `DequeIterator deque_end(const Deque* deque)`
**Purpose**: Returns a sentinel iterator representing the position just past the last element.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` sentinel used as the loop terminator for forward iteration.
**Usage Case**: Compare against this value in a `while (!iterator_equals(&it, &end))` loop to detect the end of forward traversal.

---

### `DequeIterator deque_rbegin(const Deque* deque)`
**Purpose**: Returns a reverse iterator pointing to the last element.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` positioned at the last element for reverse traversal.
**Usage Case**: Use as the starting point for a reverse iteration loop.

---

### `DequeIterator deque_rend(const Deque* deque)`
**Purpose**: Returns a reverse sentinel iterator representing the position before the first element.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` sentinel used as the loop terminator for reverse iteration.
**Usage Case**: Compare against this value in a `while (!iterator_equals(&it, &rend))` loop to detect the end of reverse traversal.

---

### `DequeIterator deque_cbegin(const Deque* deque)`
**Purpose**: Same as `deque_begin` but signals read-only intent; returns a `DequeIterator` by value.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` positioned at the first element.
**Usage Case**: Use when you want to communicate that the iteration will not modify the deque’s elements.

---

### `DequeIterator deque_cend(const Deque* deque)`
**Purpose**: Same as `deque_end` but signals read-only intent.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` sentinel for the end of forward const-iteration.
**Usage Case**: Use as the loop terminator when iterating with `deque_cbegin`.

---

### `DequeIterator deque_crbegin(const Deque* deque)`
**Purpose**: Same as `deque_rbegin` but signals read-only intent.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` positioned at the last element for const reverse traversal.
**Usage Case**: Use as the starting point for a read-only reverse iteration loop.

---

### `DequeIterator deque_crend(const Deque* deque)`
**Purpose**: Same as `deque_rend` but signals read-only intent.
**Parameters**:
- `deque`: Pointer to the deque.
**Return Value**: A `DequeIterator` sentinel for the end of reverse const-iteration.
**Usage Case**: Use as the loop terminator when iterating with `deque_crbegin`.

---

### `void iterator_increment(DequeIterator* it)`
**Purpose**: Advances `it` one position forward (for forward iterators) or one position backward (for reverse iterators), setting `it->current` to `NULL` when the end or beginning is reached.
**Parameters**:
- `it`: Pointer to the `DequeIterator` to advance.
**Return Value**: None.
**Usage Case**: Call inside a traversal loop to step to the next element.

---

### `void iterator_decrement(DequeIterator* it)`
**Purpose**: Moves `it` one position backward (for forward iterators) or one position forward (for reverse iterators), setting `it->current` to `NULL` when the boundary is crossed.
**Parameters**:
- `it`: Pointer to the `DequeIterator` to step back.
**Return Value**: None.
**Usage Case**: Call to move an iterator backward, e.g., to walk a deque in reverse using a forward iterator.

---

### `bool iterator_equals(const DequeIterator* it1, const DequeIterator* it2)`
**Purpose**: Returns `true` if both iterators point to the same position (same block index and same index within the block), or if both are at the same sentinel.
**Parameters**:
- `it1`: Pointer to the first `DequeIterator`.
- `it2`: Pointer to the second `DequeIterator`.
**Return Value**: `true` if the iterators are at the same position, `false` otherwise.
**Usage Case**: Use as the loop condition to detect when an iterator has reached the end or rend sentinel.

---

### `void* iterator_get(const DequeIterator* it)`
**Purpose**: Returns a pointer to the element at the iterator’s current position.
**Parameters**:
- `it`: Pointer to the `DequeIterator` whose current element is requested.
**Return Value**: Pointer to the current element, or `NULL` if the iterator is out of bounds.
**Usage Case**: Dereference an iterator to read or write the element it points to.

---

### `bool deque_is_equal(const Deque* deque1, const Deque* deque2)`
**Purpose**: Returns `true` if both deques have the same length and every corresponding element is byte-equal.
**Parameters**:
- `deque1`: Pointer to the first deque.
- `deque2`: Pointer to the second deque.
**Return Value**: `true` if the deques are equal, `false` otherwise.
**Usage Case**: Use to check whether two deques hold identical sequences of elements.

---

### `bool deque_is_not_equal(const Deque* deque1, const Deque* deque2)`
**Purpose**: Returns `true` if the deques differ in length or any element.
**Parameters**:
- `deque1`: Pointer to the first deque.
- `deque2`: Pointer to the second deque.
**Return Value**: `true` if the deques are not equal, `false` otherwise.
**Usage Case**: Use as the inequality counterpart to `deque_is_equal`, equivalent to `!deque_is_equal(d1, d2)`.

---

### `bool deque_is_less(const Deque* deque1, const Deque* deque2)`
**Purpose**: Returns `true` if `deque1` is lexicographically less than `deque2`.
**Parameters**:
- `deque1`: Pointer to the first deque.
- `deque2`: Pointer to the second deque.
**Return Value**: `true` if `deque1` compares less than `deque2`, `false` otherwise.
**Usage Case**: Use for sorting or ordering comparisons between two deques.

---

### `bool deque_is_less_or_equal(const Deque* deque1, const Deque* deque2)`
**Purpose**: Returns `true` if `deque1` is lexicographically less than or equal to `deque2`.
**Parameters**:
- `deque1`: Pointer to the first deque.
- `deque2`: Pointer to the second deque.
**Return Value**: `true` if `deque1` is less than or equal to `deque2`, `false` otherwise.
**Usage Case**: Use when you need a non-strict ordering comparison between two deques.

---

### `bool deque_is_greater(const Deque* deque1, const Deque* deque2)`
**Purpose**: Returns `true` if `deque1` is lexicographically greater than `deque2`.
**Parameters**:
- `deque1`: Pointer to the first deque.
- `deque2`: Pointer to the second deque.
**Return Value**: `true` if `deque1` compares greater than `deque2`, `false` otherwise.
**Usage Case**: Use to check whether one deque is ordered after another in lexicographic order.

---

### `bool deque_is_greater_or_equal(const Deque* deque1, const Deque* deque2)`
**Purpose**: Returns `true` if `deque1` is lexicographically greater than or equal to `deque2`.
**Parameters**:
- `deque1`: Pointer to the first deque.
- `deque2`: Pointer to the second deque.
**Return Value**: `true` if `deque1` is greater than or equal to `deque2`, `false` otherwise.
**Usage Case**: Use when you need a non-strict descending ordering comparison between two deques.

---

## Examples

### Example 1: Basic Push and Pop Operations
```c
#include "deque/deque.h" // Include your deque header file
#include "fmt/fmt.h"

int main() {
    Deque* myDeque = deque_create(sizeof(int));
    int val1 = 10, val2 = 20;

    // Pushing elements
    deque_push_back(myDeque, &val1); // Add to back
    fmt_printf("Pushed %d to back\n", val1);

    deque_push_front(myDeque, &val2); // Add to front
    fmt_printf("Pushed %d to front\n", val2);

    // Accessing elements
    int* frontVal = (int*)deque_front(myDeque); // Access front element
    int* backVal = (int*)deque_back(myDeque); // Access back element

    fmt_printf("Front: %d, Back: %d\n", *frontVal, *backVal);

    // Popping elements
    deque_pop_front(myDeque); // Remove from front
    fmt_printf("Popped from front\n");

    deque_pop_back(myDeque); // Remove from back
    fmt_printf("Popped from back\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Pushed 10 to back
Pushed 20 to front
Front: 20, Back: 10
Popped from front
Popped from back
```

---

### Example 2: Using `deque_at` to Access Elements
```c
#include "deque/deque.h" 
#include "fmt/fmt.h"

int main() {
    Deque* myDeque = deque_create(sizeof(int));
    int nums[] = {1, 2, 3, 4, 5};

    // Pushing elements
    for (int i = 0; i < 5; ++i) {
        deque_push_back(myDeque, &nums[i]);
        fmt_printf("Pushed %d to back\n", nums[i]);
    }

    // Accessing elements using deque_at
    fmt_printf("Deque elements: ");
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Pushed 1 to back
Pushed 2 to back
Pushed 3 to back
Pushed 4 to back
Pushed 5 to back
Deque elements: 1 2 3 4 5
```

---

### Example 3: Resizing Deque
```c
#include "deque/deque.h" 
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));

    for (int i = 0; i < 10; ++i) {
        deque_push_back(myDeque, &i);
        fmt_printf("Pushed %d to back\n", i);
    }
    fmt_printf("Size before resize: %zu\n", deque_length(myDeque));

    // Resizing the deque
    deque_resize(myDeque, 5); // Resize to smaller size
    fmt_printf("Resized deque to 5 elements\n");
    fmt_printf("Size after resize: %zu\n", deque_length(myDeque));

    fmt_printf("Elements in resized deque: ");
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Pushed 0 to back
Pushed 1 to back
Pushed 2 to back
Pushed 3 to back
Pushed 4 to back
Pushed 5 to back
Pushed 6 to back
Pushed 7 to back
Pushed 8 to back
Pushed 9 to back
Size before resize: 10
Resized deque to 5 elements
Size after resize: 5
Elements in resized deque: 0 1 2 3 4
```

---

## Example 4: Using Deque with Structs

```c
#include "deque/deque.h"
#include "fmt/fmt.h"

typedef struct {
    int id;
    char name[50];
} Person;

int main() {
    Deque* personDeque = deque_create(sizeof(Person));
    Person p1 = {1, "Alice"}, p2 = {2, "Bob"};

    deque_push_back(personDeque, &p1);
    deque_push_back(personDeque, &p2);

    Person* frontPerson = (Person*)deque_front(personDeque);
    fmt_printf("Front person: %d, %s\n", frontPerson->id, frontPerson->name);

    deque_deallocate(personDeque);
    return 0;
}
```
**Result**
```
Front person: 1, Alice
```

---

## Example 5: Storing char Elements

```c
#include "deque/deque.h" 
#include "fmt/fmt.h"

typedef struct {
    int id;
    char name[50];
} Person;

int main(){
    Deque* charDeque = deque_create(sizeof(char));
    char a = 'A', b = 'B';

    deque_push_back(charDeque, &a);
    deque_push_back(charDeque, &b);

    char* frontChar = (char*)deque_front(charDeque);
    fmt_printf("Front char: %c\n", *frontChar);

    deque_deallocate(charDeque);
    return 0;
}
```
**Result**
```
Front char: A
```

---

## Example 6 : Storing float Elements

```c
#include "deque/deque.h" 
#include "fmt/fmt.h"

int main() {
    Deque* floatDeque = deque_create(sizeof(float));
    float num1 = 1.5f, num2 = 2.5f;

    deque_push_back(floatDeque, &num1);
    deque_push_back(floatDeque, &num2);

    float* backFloat = (float*)deque_back(floatDeque);
    fmt_printf("Back float: %f\n", *backFloat);

    deque_deallocate(floatDeque);
    return 0;
}
```
**Result**
```
Back float: 2.500000
```

---

## Example 7 : Reversing Elements in a Deque

```c

#include "deque/deque.h" 
#include "fmt/fmt.h"
#include <stdlib.h>

int main(){
    Deque* reverseDeque = deque_create(sizeof(int));

    for (int i = 0; i < 5; ++i) {
        deque_push_back(reverseDeque, &i);
    }
    fmt_printf("Reversed Deque: ");

    // Iterate using the length of the deque
    for (size_t i = 0; i < deque_length(reverseDeque); ++i){
        int* valPtr = (int*)deque_at(reverseDeque, deque_length(reverseDeque) - 1 - i);
        fmt_printf("%d ", *valPtr);
    }
    fmt_printf("\n");

    deque_deallocate(reverseDeque);
    return 0;
}
```
**Result**
```
Reversed Deque: 4 3 2 1 0
```

---

## Example 8 : Nested Deque 

```c
#include "deque/deque.h" 
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* outerDeque = deque_create(sizeof(Deque*));

    for (int i = 0; i < 3; ++i) {
        Deque* innerDeque = deque_create(sizeof(int));

        for (int j = 0; j < 3; ++j) {
            int val = i * 3 + j;
            deque_push_back(innerDeque, &val);
        }
        deque_push_back(outerDeque, &innerDeque);
    }

    // Access and print elements
    for (size_t i = 0; i < deque_length(outerDeque); ++i) {
        Deque* innerDeque = *(Deque**)deque_at(outerDeque, i);

        for (size_t j = 0; j < deque_length(innerDeque); ++j) {
            fmt_printf("%d ", *(int*)deque_at(innerDeque, j));
        }
        fmt_printf("\n");
        deque_deallocate(innerDeque);
    }

    deque_deallocate(outerDeque);
    return 0;
}
```
**Result**
```
0 1 2 
3 4 5 
6 7 8 
```

---

## Example 9 : Copying Deque 

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* original = deque_create(sizeof(int));

    for (int i = 0; i < 5; ++i) {
        int* ptr = (int*)malloc(sizeof(int));
        if (ptr == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory\n");
            exit(-1);
        }
        *ptr = i;
        deque_push_back(original, ptr);
        free(ptr);   /* deque copies the value; free the allocation */
    }

    // Copying the deque
    Deque* copy = deque_create(sizeof(int));
    for (size_t i = 0; i < deque_length(original); ++i) {
        int* val = (int*)deque_at(original, i);
        deque_push_back(copy, val);
    }

    // Print copied deque
    fmt_printf("Copied Deque: ");
    for (size_t i = 0; i < deque_length(copy); ++i) {
        fmt_printf("%d ", *(int*)deque_at(copy, i));
    }
    fmt_printf("\n");

    deque_deallocate(original);
    deque_deallocate(copy);
    return 0;
}
```
**Result**
```
Copied Deque: 0 1 2 3 4 
```

---

## Example 10 : Store Copies of Struct in the Deque

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>


typedef struct {
    char* name;
    int age;
} Person;

int main() {
    Deque* people = deque_create(sizeof(Person));
    const char* names[] = {"Alice", "Bob", "Charlie"};
    int ages[] = {25, 30, 35};
    
    for (size_t i = 0; i < 3; ++i) {
        Person* p = (Person*)malloc(sizeof(Person)); 
        p->name = string_strdup(names[i]);  
        p->age = ages[i];

        deque_push_back(people, p);  
        free(p);   /* deque copies the struct; free the holder (name still owned by the copy) */
    }

    // Access and print people
    for (size_t i = 0; i < deque_length(people); ++i) {
        Person* p = (Person*)deque_at(people, i);
        fmt_printf("Name: %s, Age: %d\n", p->name, p->age);
    }

    // Free the strdup'd names held by the copied structs
    for (size_t i = 0; i < deque_length(people); ++i) {
        free(((Person*)deque_at(people, i))->name);
    }

    deque_deallocate(people);
    return 0;
}
```
**Result**
```
Name: Alice, Age: 25
Name: Bob, Age: 30
Name: Charlie, Age: 35
```

---

### Example 11: Shrink to Fit Deque

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));

    // Adding elements
    for (int i = 0; i < 100; ++i) {
        int* ptr = (int*)malloc(sizeof(int)); 
        if (ptr == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory\n");
            break; 
        }

        *ptr = i; 
        deque_push_back(myDeque, ptr); 
        free(ptr);   /* deque copies the value; free the allocation */
    }
    
    deque_resize(myDeque, 10);
    deque_shrink_to_fit(myDeque);

    fmt_printf("Deque after shrink to fit (Size: %zu):\n", deque_length(myDeque));
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Deque after shrink to fit (Size: 10):
0 1 2 3 4 5 6 7 8 9
```

---

### Example 12: Inserting and Erasing Elements

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));
    int insertVal = 99;

    for (int i = 0; i < 5; ++i) {
        int* ptr = (int*)malloc(sizeof(int)); 
        if (ptr == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory\n");
            break; 
        }

        *ptr = i; 
        deque_push_back(myDeque, ptr); 
        free(ptr);   /* deque copies the value; free the allocation */
    }

    deque_insert(myDeque, 2, &insertVal);
    deque_erase(myDeque, 1);

    fmt_printf("Deque after insert and erase:\n");
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");


    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Deque after insert and erase:
0 99 2 3 4
```

---

### Example 13: Using Emplace Methods
This example demonstrates the use of `emplace_back`, `emplace_front`, and `emplace` methods for in-place construction.

```c
#include "deque/deque.h"
#include "fmt/fmt.h"

int main() {
    Deque* myDeque = deque_create(sizeof(int));
    int val = 10;

    // Emplacing elements
    deque_emplace_back(myDeque, &val);
    deque_emplace_front(myDeque, &val);
    deque_emplace(myDeque, 1, &val);

    fmt_printf("Size of myQueue %zu\n", deque_length(myDeque));
    fmt_printf("Deque after emplace operations:\n");

    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Size of myQueue 3
Deque after emplace operations:
10 10 10
```

---

### Example 14: Using Relational Operators
This example illustrates how to compare two deques using relational operators.

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* deque1 = deque_create(sizeof(int));
    Deque* deque2 = deque_create(sizeof(int));
    int val = 5;

    // Adding elements
    for (int i = 0; i < 3; ++i) {
        int *newInt1 = (int*)malloc(sizeof(int));
        *newInt1 = i;
        deque_push_back(deque1, newInt1);
        free(newInt1);   /* deque copies the value; free the allocation */

        int *newInt2 = (int*)malloc(sizeof(int));
        *newInt2 = val;
        deque_push_back(deque2, newInt2);
        free(newInt2);   /* deque copies the value; free the allocation */
    }

    if (deque_is_less(deque1, deque2)) {
        fmt_printf("Deque1 is less than Deque2\n");
    }

    deque_deallocate(deque1);
    deque_deallocate(deque2);
    return 0;
}
```
**Result**
```
Deque1 is less than Deque2
```

---

### Example 15: Iterating Using Deque Iterators
This example demonstrates iterating over a deque using its iterators.

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));

    for (int i = 0; i < 3; ++i) {
        int* newInt = (int*)malloc(sizeof(int));
        *newInt = i;
        deque_push_back(myDeque, newInt);
        free(newInt);   /* deque copies the value; free the allocation */
    }

    DequeIterator it = deque_begin(myDeque);
    DequeIterator end = deque_end(myDeque);

    fmt_printf("Deque elements: ");    
    while (!iterator_equals(&it, &end)) {
        fmt_printf("%d ", *(int*)iterator_get(&it));
        iterator_increment(&it);
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Deque elements: 0 1 2
```

---

## Example 16 : 'clear' and 'emplace_back'

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));

    for (int i = 0; i < 2; ++i) {
        int* newVal = (int*)malloc(sizeof(int));
        *newVal = 42;  
        deque_emplace_back(myDeque, newVal);  
        free(newVal);   /* deque copies the value; free the allocation */
    }

    fmt_printf("Deque after emplacing elements: ");
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");

    deque_clear(myDeque);
    fmt_printf("Deque after clearing: Length = %zu\n", deque_length(myDeque));

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Deque after emplacing elements: 42 42 
Deque after clearing: Length = 0
```

---

## Example 17 : Convert example in this url 'https://www.geeksforgeeks.org/deque-cpp-stl/' to Deque 

```c
#include <stdlib.h>
#include "deque/deque.h"
#include "fmt/fmt.h"

void showdq(Deque* dq) {
    for (size_t i = 0; i < deque_length(dq); ++i) {
        int* elem = (int*)deque_at(dq, i);
        fmt_printf("\t%d", *elem);
    }
    fmt_printf("\n");
}

int main() {
    Deque* gquiz = deque_create(sizeof(int));

    // Push elements
    int* val1 = (int*)malloc(sizeof(int)); *val1 = 10;
    int* val2 = (int*)malloc(sizeof(int)); *val2 = 20;
    int* val3 = (int*)malloc(sizeof(int)); *val3 = 30;
    int* val4 = (int*)malloc(sizeof(int)); *val4 = 15;

    deque_push_back(gquiz, val1);
    deque_push_front(gquiz, val2);
    deque_push_back(gquiz, val3);
    deque_push_front(gquiz, val4);

    // deque_push_* copy the item bytes into the deque, so the caller's
    // original allocations can be freed right away.
    free(val1);
    free(val2);
    free(val3);
    free(val4);

    fmt_printf("The deque gquiz is : ");
    showdq(gquiz);

    fmt_printf("\ngquiz.max_size() : %zu", deque_max_size(gquiz));
    fmt_printf("\ngquiz.at(2) : %d", *(int*)deque_at(gquiz, 2));
    fmt_printf("\ngquiz.front() : %d", *(int*)deque_front(gquiz));
    fmt_printf("\ngquiz.back() : %d", *(int*)deque_back(gquiz));

    fmt_printf("\ngquiz.pop_front() : ");
    deque_pop_front(gquiz);
    showdq(gquiz);

    fmt_printf("\ngquiz.pop_back() : ");
    deque_pop_back(gquiz);
    showdq(gquiz);

    // Free the deque (deque_clear and deque_deallocate handle all memory cleanup)
    deque_deallocate(gquiz);

    return 0;
}
```
**Result**
```
The deque gquiz is :    15      20      10      30

gquiz.max_size() : 18446744073709551615
gquiz.at(2) : 10
gquiz.front() : 15
gquiz.back() : 30
gquiz.pop_front() :     20      10      30

gquiz.pop_back() :      20      10
```

---

## Example 18 : 'crbegin', 'crend'

```c
#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));

    // Adding elements
    for (int i = 0; i < 5; ++i) {
        int* newInt = (int*)malloc(sizeof(int));
        *newInt = i;
        deque_push_back(myDeque, newInt);
        free(newInt);   /* deque copies the value; free the allocation */
    }

    // Constant reverse iteration
    DequeIterator crit = deque_crbegin(myDeque); // Start at last element
    DequeIterator crend = deque_crend(myDeque); // End at invalid position before first element

    fmt_printf("Constant reverse iteration: ");
    while (!iterator_equals(&crit, &crend)) {
        fmt_printf("%d ", *(const int*)iterator_get(&crit));
        iterator_increment(&crit); // Should move to the previous element
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Constant reverse iteration: 4 3 2 1 0
```

--- 

## Example 19 : `cbegin` and `cend`

```c
#include "deque/deque.h"
#include "fmt/fmt.h"

int main() {
    // Create a deque of integers
    Deque* myDeque = deque_create(sizeof(int));
    
    // Add some elements to the deque
    int val1 = 10, val2 = 20, val3 = 30;
    deque_push_back(myDeque, &val1);
    deque_push_back(myDeque, &val2);
    deque_push_back(myDeque, &val3);

    // Get constant begin and end iterators
    const DequeIterator it = deque_cbegin(myDeque);
    const DequeIterator end = deque_cend(myDeque);

    fmt_printf("Deque elements (constant iteration): ");
    for (DequeIterator iter = it; !iterator_equals(&iter, &end); iterator_increment(&iter)) {
        fmt_printf("%d ", *(const int*)iterator_get(&iter)); // Constant access
    }
    fmt_printf("\n");

    // Clean up
    deque_deallocate(myDeque);
    return 0;
}
```
**Result**
```
Deque elements (constant iteration): 10 20 30
```

---

## License

This project is open-source and available under [ISC License].