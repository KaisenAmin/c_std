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

- **Purpose**: Creates a new deque with a specified item size.
- **Parameters**:
  - `itemSize`: The size of each item in the deque. Must be greater than 0.
- **Return**: Pointer to the newly created deque or `NULL` if memory allocation fails.
- **Details**: 
  - Initializes the deque with a middle index for efficient push operations at both ends. Allocates memory for the deque structure and the first block of memory for storing elements.

---

### `bool deque_empty(const Deque* deque)`

- **Purpose**: Checks if the deque is empty.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: `true` if the deque is empty or `NULL`, `false` otherwise.
- **Details**: 
  - Logs an error if the deque is `NULL` and returns `true`.

---

### `size_t deque_length(const Deque* deque)`

- **Purpose**: Returns the number of elements in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: The number of elements in the deque or `0` if the deque is `NULL`.
- **Details**: 
  - Logs the size of the deque and returns `0` if the deque pointer is `NULL`.

---

### `void deque_push_front(Deque* deque, void* item)`

- **Purpose**: Inserts an item at the front of the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `item`: Pointer to the item to be inserted.
- **Return**: No return value.
- **Details**: 
  - Inserts a new item at the front of the deque. If necessary, it allocates a new block at the front to accommodate the new item. Logs errors if the deque or item is `NULL`.

---

### `void deque_push_back(Deque* deque, const void* item)`

- **Purpose**: Inserts an item at the back of the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `item`: Pointer to the item to be inserted.
- **Return**: No return value.
- **Details**: 
  - If the deque reaches the back of the allocated block, a new block is added. Logs errors if memory allocation fails or the input is `NULL`.

---

### `void* deque_front(const Deque* deque)`

- **Purpose**: Retrieves the front element of the deque without removing it.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: Pointer to the front element or `NULL` if the deque is empty or `NULL`.
- **Details**: 
  - Logs errors if the deque is empty or `NULL` and returns the front element if available.

---

### `void* deque_back(const Deque* deque)`

- **Purpose**: Retrieves the back element of the deque without removing it.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: Pointer to the back element or `NULL` if the deque is empty or `NULL`.
- **Details**: 
  - Logs errors if the deque is empty or `NULL` and returns the back element if available.

---

### `void deque_pop_front(Deque* deque)`

- **Purpose**: Removes the front element from the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: No return value.
- **Details**: 
  - Logs errors if the deque is `NULL` or empty. It adjusts the front index, removes memory blocks if necessary, and updates the deque's size.

---

### `void deque_pop_back(Deque* deque)`

- **Purpose**: Removes the back element from the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: No return value.
- **Details**: Frees the back element, adjusts the back index, and manages memory blocks when necessary. Logs errors if the deque is empty or `NULL`.

---

### `void* deque_at(const Deque* deque, size_t index)`

- **Purpose**: Retrieves the element at the specified index in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `index`: The index of the element to retrieve.
- **Return**: Pointer to the element at the specified index or `NULL` if the index is out of bounds or the deque is `NULL`.
- **Details**: Logs errors for out-of-bounds access or invalid input.

---

### `void deque_clear(Deque* deque)`

- **Purpose**: Clears all elements from the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: No return value.
- **Details**: Frees all elements and blocks, then resets the deque to its initial state. Logs errors for `NULL` input.

---

### `void deque_deallocate(Deque* deque)`

- **Purpose**: Deallocates all memory associated with the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: No return value.
- **Details**: Frees all elements and memory blocks, and then deallocates the deque structure itself. Logs the deallocation process.

---

### `void deque_shrink_to_fit(Deque* deque)`

- **Purpose**: Shrinks the deque's memory usage to fit its current size.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: No return value.
- **Details**: Reduces the memory usage by deallocating excess blocks and reallocating memory to fit the current size of the deque. Logs any errors during the shrinking process.

---

### `void deque_insert(Deque* deque, size_t index, void* item)`

- **Purpose**: Inserts an element at a specified index in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `index`: The position where the element will be inserted.
  - `item`: Pointer to the item to insert.
- **Details**: If the index is out of bounds or the deque needs to be expanded, memory is reallocated, and elements are shifted to the right to accommodate the new element. Logs errors if the input is invalid.

---

### `void deque_erase(Deque* deque, size_t index)`

- **Purpose**: Removes an element at a specified index in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `index`: The position of the element to remove.
- **Details**: Shifts elements to the left to fill the gap left by the removed element. If blocks become empty, they are deallocated. Logs errors if the index is out of bounds or the deque is `NULL`.

---

### `void deque_resize(Deque* deque, size_t newSize)`

- **Purpose**: Resizes the deque to the specified size.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `newSize`: The new size of the deque.
- **Details**: Expands or shrinks the deque to the new size. If the size is reduced, memory is optimized by deallocating unnecessary blocks. Logs actions and handles memory reallocation.

---

### `void deque_swap(Deque* deque, Deque* otherDeque)`

- **Purpose**: Swaps the contents of two deques.
- **Parameters**:
  - `deque`: Pointer to the first deque.
  - `otherDeque`: Pointer to the second deque.
- **Details**: The entire internal structure, size, and blocks of the two deques are swapped.

---

### `void deque_assign(Deque* deque, size_t n, void* val)`

- **Purpose**: Assigns a specified value to a range of elements in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `n`: The number of elements to assign.
  - `val`: Pointer to the value to assign to each element.
- **Details**: Clears the deque and resizes it to hold `n` elements, each initialized to `val`.

---

### `void deque_emplace_back(Deque* deque, void* item)`

- **Purpose**: Inserts an element at the end of the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `item`: Pointer to the item to insert.
- **Details**: If the deque’s current block at the back is full, allocates a new block and places the item.

---

### `void deque_emplace_front(Deque* deque, void* item)`

- **Purpose**: Inserts an element at the front of the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `item`: Pointer to the item to insert.
- **Details**: If the front block is full, allocates a new block and places the item at the front.

---

### `void deque_emplace(Deque* deque, size_t index, void* item)`

- **Purpose**: Inserts an element at the specified position in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
  - `index`: The position at which to insert the element.
  - `item`: Pointer to the item to be inserted.
- **Details**: Inserts an element at the given index. If the index is at the front or back, it delegates to `deque_emplace_front` or `deque_emplace_back`. Otherwise, it shifts elements to make space.

---

### `size_t deque_max_size(const Deque* deque)`

- **Purpose**: Returns the maximum number of elements the deque can hold.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: The maximum size of the deque, which is typically `SIZE_MAX`.
- **Details**: Returns the system-defined maximum size the deque can hold.

---

### `bool deque_is_equal(const Deque* deque1, const Deque* deque2)`

- **Purpose**: Compares two deques for equality.
- **Parameters**:
  - `deque1`: Pointer to the first deque.
  - `deque2`: Pointer to the second deque.
- **Return**: `true` if the deques are equal (same size and elements), `false` otherwise.
- **Details**: Compares the size and contents of two deques to check for equality.

---

### `bool deque_is_less(const Deque* deque1, const Deque* deque2)`

- **Purpose**: Compares two deques lexicographically to determine if the first is less than the second.
- **Parameters**:
  - `deque1`: Pointer to the first deque.
  - `deque2`: Pointer to the second deque.
- **Return**: `true` if `deque1` is lexicographically less than `deque2`, otherwise `false`.
- **Details**: Compares elements in sequence, stopping when a difference is found or one deque runs out of elements.

---

### `bool deque_is_greater(const Deque* deque1, const Deque* deque2)`

- **Purpose**: Determines if the first deque is greater than the second by reversing the comparison using `deque_is_less`.
- **Parameters**:
  - `deque1`: Pointer to the first deque.
  - `deque2`: Pointer to the second deque.
- **Return**: `true` if `deque1` is greater than `deque2`, otherwise `false`.
- **Details**: Uses `deque_is_less` in reverse to determine the result.

---

### `bool deque_is_not_equal(const Deque* deque1, const Deque* deque2)`

- **Purpose**: Compares two deques for inequality.
- **Parameters**:
  - `deque1`: Pointer to the first deque.
  - `deque2`: Pointer to the second deque.
- **Return**: `true` if the deques are not equal, otherwise `false`.
- **Details**: Returns the negation of `deque_is_equal`.

---

### `bool deque_is_less_or_equal(const Deque* deque1, const Deque* deque2)`

- **Purpose**: Checks if the first deque is less than or equal to the second.
- **Parameters**:
  - `deque1`: Pointer to the first deque.
  - `deque2`: Pointer to the second deque.
- **Return**: `true` if `deque1` is less than or equal to `deque2`, otherwise `false`.
- **Details**: Uses `deque_is_less` and `deque_is_equal` to determine the result.

---

### `bool deque_is_greater_or_equal(const Deque* deque1, const Deque* deque2) `

- **Purpose**: Checks if the first deque is greater than or equal to the second.
- **Parameters**:
  - `deque1`: Pointer to the first deque.
  - `deque2`: Pointer to the second deque.
- **Return**: `true` if `deque1` is greater than or equal to `deque2`, otherwise `false`.
- **Details**: Uses `deque_is_greater` and `deque_is_equal`.

---

### `DequeIterator deque_begin(const Deque* deque)`

- **Purpose**: Returns an iterator to the first element in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: An iterator pointing to the first element.
- **Details**: Initializes an iterator to point to the first element of the deque, or signals an empty deque if necessary.

---

### `DequeIterator deque_end(const Deque* deque)`

- **Purpose**: Returns an iterator to the position just past the last element in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: An iterator pointing just past the last element.
- **Details**: Commonly used as a sentinel in iteration loops.

---

### `DequeIterator deque_rbegin(const Deque* deque)`

- **Purpose**: Returns a reverse iterator pointing to the last element in the deque.
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: A `DequeIterator` pointing to the last element.
- **Details**: Allows backward traversal of the deque by starting from the last element.

---

### `DequeIterator deque_rend(const Deque* deque)`

- **Purpose**: Returns a reverse iterator pointing to the position before the first element (i.e., the "rend" position).
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: A `DequeIterator` representing the "rend" position (before the first element).
- **Details**: Typically used to mark the end of reverse iteration.

---

### `const DequeIterator* deque_cbegin(const Deque* deque)`

- **Purpose**: Returns a constant iterator pointing to the first element in the deque (read-only).
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: A pointer to a constant `DequeIterator` pointing to the first element.
- **Details**: Ensures that the iterator is read-only.

---

### `const DequeIterator* deque_cend(const Deque* deque)`

- **Purpose**: Returns a constant iterator pointing just past the last element (read-only).
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: A pointer to a constant `DequeIterator` representing the "end" position.
- **Details**: Used as the sentinel for forward traversal.

---

### `const DequeIterator* deque_crbegin(const Deque* deque)`

- **Purpose**: Returns a constant reverse iterator pointing to the last element (read-only).
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: A pointer to a constant `DequeIterator` pointing to the last element.
- **Details**: Used for reverse traversal, ensuring the elements are read-only.

---

### `const DequeIterator* deque_crend(const Deque* deque)`

- **Purpose**: Returns a constant reverse iterator pointing to the position before the first element (read-only).
- **Parameters**:
  - `deque`: Pointer to the deque.
- **Return**: A pointer to a constant `DequeIterator` representing the reverse "end" position.
- **Details**: Used as the sentinel for reverse iteration, ensuring the iterator is read-only.

---

### `void iterator_increment(DequeIterator* it)`

- **Purpose**: Increments the position of the iterator.
- **Parameters**:
  - `it`: Pointer to the `DequeIterator` that will be incremented.
- **Return**: No return value.
- **Details**: 
  - For **forward iteration**, this function moves the iterator one element forward. If the end of a block is reached, it transitions to the next block.
  - For **reverse iteration**, it moves the iterator backward. If the beginning of a block is reached, it transitions to the previous block.
  - If the iterator reaches the end or beginning of the deque, the `current` element is set to `NULL`.

---

### `void iterator_decrement(DequeIterator* it)`

- **Purpose**: Decrements the position of the iterator.
- **Parameters**:
  - `it`: Pointer to the `DequeIterator` that will be decremented.
- **Return**: No return value.
- **Details**: 
  - For **forward iteration**, this function moves the iterator one element backward. If the start of a block is reached, it transitions to the previous block.
  - For **reverse iteration**, it moves the iterator forward. If the end of a block is reached, it transitions to the next block.
  - If the iterator reaches the beginning or end of the deque, the `current` element is set to `NULL`.

---

### `bool iterator_equals(const DequeIterator* it1, const DequeIterator* it2)`

- **Purpose**: Compares two iterators for equality.
- **Parameters**:
  - `it1`: Pointer to the first `DequeIterator`.
  - `it2`: Pointer to the second `DequeIterator`.
- **Return**: `true` if the iterators are equal, `false` otherwise.
- **Details**:
  - This function checks if both iterators point to the same position in the deque by comparing their `blockIndex` and `indexInBlock`.
  - If either iterator is at the end (or reverse end) position, the comparison is based on whether both iterators are at the end/rend.
  - Logs whether the iterators are equal or not, and handles cases where one or both iterators are `NULL`.

---

### `void* iterator_get(const DequeIterator* it)`

- **Purpose**: Retrieves the value pointed to by the iterator.
- **Parameters**:
  - `it`: Pointer to the `DequeIterator` from which to retrieve the value.
- **Return**: A pointer to the value at the current position of the iterator, or `NULL` if the iterator is out of bounds.
- **Details**:
  - The function returns the value located at the current position of the iterator, handling both forward and reverse iteration.
  - In **reverse iteration**, it returns the element at the current block and index, or `NULL` if the iterator is out of bounds.
  - In **forward iteration**, it retrieves the element from the block and index, or returns `NULL` if the iterator is out of bounds.
  - Logs the position of the iterator and errors if the iterator or deque is `NULL` or out of bounds.

---

## Examples

### Example 1: Basic Push and Pop Operations
```c
#include "deque/deque.h" // Include your bitset header file
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
        int* ptr = (int*)malloc(sizeof(int));

        if (ptr == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory\n");
            break; 
        }
        *ptr = i; 
        deque_push_back(myDeque, ptr); 

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

    // Allocate and push back elements
    for (int i = 0; i < 5; ++i) {
        int* ptr = (int*)malloc(sizeof(int));
        if (ptr == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory\n");
            exit(-1);
        }

        *ptr = i;
        deque_push_back(reverseDeque, ptr);
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
        Deque* innerDeque = deque_create(sizeof(int*));

        for (int j = 0; j < 3; ++j) {
            int* valPtr = (int*)malloc(sizeof(int));
            if (valPtr == NULL) {
                fmt_fprintf(stderr, "Failed to allocate memory\n");
                exit(-1);
            }
            *valPtr = i * 3 + j;
            deque_push_back(innerDeque, valPtr);
        }
        deque_push_back(outerDeque, &innerDeque);
    }

    // Access and print elements
    for (size_t i = 0; i < deque_length(outerDeque); ++i) {
        Deque* innerDeque = *(Deque**)deque_at(outerDeque, i);

        for (size_t j = 0; j < deque_length(innerDeque); ++j) {
            int* valPtr = (int*)deque_at(innerDeque, j);
            fmt_printf("%d ", *valPtr);
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
            fprintf(stderr, "Failed to allocate memory\n");
            exit(-1);
        }
        *ptr = i;
        deque_push_back(original, ptr);
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
    }

    // Access and print people
    for (size_t i = 0; i < deque_length(people); ++i) {
        Person* p = (Person*)deque_at(people, i);
        fmt_printf("Name: %s, Age: %d\n", p->name, p->age);
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

        int *newInt2 = (int*)malloc(sizeof(int));
        *newInt2 = val;
        deque_push_back(deque2, newInt2);
    }

    if (deque_is_less(deque1, deque2)) {
        fmt_printf("Deque1 is less than Deque2\n");
    }

    for (size_t i = 0; i < 3; ++i) {
        free(deque_at(deque1, i));
        free(deque_at(deque2, i));
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