# Vector Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2023
**License:** ISC License

## Overview

This Vector library, written in C, is an implementation that mimics the functionality of the C++ `std::vector`. It aims to provide dynamic array capabilities to C programs. This library is part of a project to reimplement features of the C++ standard library in C, making it useful for developers familiar with C++ but working in C environments.

The Vector library allows for creating dynamic arrays that can resize automatically when elements are added or removed. It supports various operations such as inserting, deleting, and accessing elements, as well as capacity management (reserve / shrink-to-fit) for efficient memory use.

## Key Features

- **Dynamic Resizing:** Automatically adjust size when elements are added or removed.
- **Memory Efficiency:** Elements live in a single contiguous buffer grown with `realloc` (doubling), so peak memory tracks the live element count — the same storage model as C++ `std::vector`. Scales cleanly to 10–100 million elements.
- **Generic Container:** Can store elements of any data type, including structures.
- **Various Utility Functions:** Functions for comparison, swapping, and checking the state of the vector.

## Installation and Compilation

To use this library, include `vector.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O2 -Wall -Wextra -o test.exe test.c vector/vector.c string/std_string.c -I.
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like creating a vector, adding and removing elements, as well as more complex functionalities such as capacity management and vector comparison.

### Function Descriptions:

### `Vector* vector_create(size_t itemSize)`

**Purpose**: Initializes and returns a new vector capable of storing elements of the specified size.

**Parameters**:
- `itemSize`: The size in bytes of each element that will be stored in the vector.

**Return Value**: A pointer to the newly created vector. Returns `NULL` if memory allocation fails.

**Usage Case**: Use at the start to create a typed dynamic array before pushing elements.

---

### `bool vector_is_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Compares two vectors and checks if they are equal in terms of both size and content.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if both vectors have identical elements in the same order, otherwise `false`.

**Usage Case**: Use when you need to verify that two vectors hold the same sequence of elements.

---

### `bool vector_is_less(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically less than the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is lexicographically smaller than `vec2`, otherwise `false`.

**Usage Case**: Use for sorting or ordering comparisons between two vectors.

---

### `bool vector_is_greater(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically greater than the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is lexicographically greater than `vec2`, otherwise `false`.

**Usage Case**: Use for sorting or ordering comparisons between two vectors.

---

### `bool vector_is_not_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Compares two vectors and checks if they are not equal.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if the vectors are not equal, otherwise `false`.

**Usage Case**: Use when you need to detect differences between two vectors.

---

### `bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically greater than or equal to the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is greater than or equal to `vec2`, otherwise `false`.

**Usage Case**: Use in conditional checks where equality or greater order is acceptable.

---

### `bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically less than or equal to the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is less than or equal to `vec2`, otherwise `false`.

**Usage Case**: Use in conditional checks where equality or lesser order is acceptable.

---

### `bool vector_is_empty(Vector* vec)`

**Purpose**: Checks if the vector is empty, meaning it contains no elements.

**Parameters**:
- `vec`: The vector to check.

**Return Value**: `true` if the vector is empty, otherwise `false`.

**Usage Case**: Use before accessing elements to guard against operating on an empty vector.

---

### `bool vector_reserve(Vector* vec, size_t size)`

**Purpose**: Allocates enough memory for the vector to hold at least the specified number of elements without resizing.

**Parameters**:
- `vec`: The vector to reserve space for.
- `size`: The minimum number of elements to reserve capacity for.

**Return Value**: `true` if memory allocation is successful, otherwise `false`.

**Usage Case**: Use when you know ahead of time how many elements will be added, to avoid repeated reallocations.

---

### `bool vector_push_back(Vector* vec, const void* item)`

**Purpose**: Adds an item to the end of the vector, resizing the vector if necessary.

**Parameters**:
- `vec`: The vector to which the item is added.
- `item`: A pointer to the item being added to the vector.

**Return Value**: `true` if the operation succeeds, otherwise `false` (e.g., memory allocation failure).

**Usage Case**: Use to append elements one at a time to the end of the vector.

---

### `bool vector_emplace_back(Vector* vec, void* item, size_t itemSize)`

**Purpose**: Constructs an item in place at the end of the vector, potentially avoiding copying and allowing more efficient insertion.

**Parameters**:
- `vec`: The vector to add the item to.
- `item`: A pointer to the item to be added.
- `itemSize`: The size of the item being added.

**Return Value**: `true` if successful, otherwise `false`.

**Usage Case**: Use when inserting items at the end with explicit size control for efficiency.

---

### `void vector_erase(Vector* vec, size_t pos, size_t len)`

**Purpose**: Erases a range of elements from the vector, starting at the specified position and removing the specified number of elements.

**Parameters**:
- `vec`: The vector from which elements will be removed.
- `pos`: The starting position of the elements to be removed.
- `len`: The number of elements to erase.

**Return Value**: None.

**Usage Case**: Use to remove a contiguous block of elements from any position in the vector.

---

### `void vector_insert(Vector* vec, size_t pos, void* item)`

**Purpose**: Inserts an item into the vector at the specified position, shifting subsequent elements to the right.

**Parameters**:
- `vec`: The vector where the item will be inserted.
- `pos`: The index at which the item will be inserted.
- `item`: A pointer to the item to insert.

**Return Value**: None.

**Usage Case**: Use to add an element at a specific index rather than at the end.

---

### `void vector_resize(Vector* vec, size_t size)`

**Purpose**: Resizes the vector to contain the specified number of elements. If the new size is larger, additional elements will be zero-initialized.

**Parameters**:
- `vec`: The vector to resize.
- `size`: The new size of the vector.

**Return Value**: None.

**Usage Case**: Use to explicitly set the number of elements, growing or shrinking the vector.

---

### `void vector_shrink_to_fit(Vector* vec)`

**Purpose**: Shrinks the capacity of the vector to match its current size, freeing any excess allocated memory.

**Parameters**:
- `vec`: The vector to shrink.

**Return Value**: None.

**Usage Case**: Use after many removals to reclaim unused memory.

---

### `void vector_clear(Vector* vec)`

**Purpose**: Clears all elements from the vector, setting its size to 0 but maintaining its current capacity.

**Parameters**:
- `vec`: The vector to clear.

**Return Value**: None.

**Usage Case**: Use to empty the vector for reuse without releasing its allocated memory.

---

### `void vector_swap(Vector* vec1, Vector* vec2)`

**Purpose**: Swaps the contents of two vectors, exchanging their sizes, capacities, and elements.

**Parameters**:
- `vec1`: The first vector.
- `vec2`: The second vector.

**Return Value**: None.

**Usage Case**: Use to exchange the contents of two vectors efficiently without copying elements.

---

### `void vector_assign(Vector* vec, size_t pos, void* item)`

**Purpose**: Assigns a new value to the element at the specified position.

**Parameters**:
- `vec`: The vector to modify.
- `pos`: The position of the element to assign a new value to.
- `item`: A pointer to the new item.

**Return Value**: None.

**Usage Case**: Use to update an existing element at a known index.

---

### `void vector_emplace(Vector* vec, size_t pos, void* item, size_t itemSize)`

**Purpose**: Constructs an item in place at the specified position within the vector.

**Parameters**:
- `vec`: The vector where the item will be constructed.
- `pos`: The index at which to construct the item.
- `item`: A pointer to the item to be placed.
- `itemSize`: The size of the item.

**Return Value**: None.

**Usage Case**: Use to insert an element at an arbitrary position with explicit size control.

---

### `void vector_deallocate(Vector* vec)`

**Purpose**: Frees all memory associated with the vector, including its elements and the vector structure itself.

**Parameters**:
- `vec`: The vector to deallocate.

**Return Value**: None.

**Usage Case**: Call when done using a vector to release all its memory.

---

### `const void* vector_cbegin(Vector* vec)`

**Purpose**: Returns a constant pointer to the first element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to the first element of the vector.

**Usage Case**: Use for read-only forward iteration from the beginning.

---

### `const void* vector_cend(Vector* vec)`

**Purpose**: Returns a constant pointer to the element following the last element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to one past the last element (end sentinel).

**Usage Case**: Use as the end bound for read-only forward iteration.

---

### `const void* vector_crbegin(Vector* vec)`

**Purpose**: Returns a constant pointer to the first element of the reversed vector (i.e., the last element).

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to the last element of the vector.

**Usage Case**: Use for read-only reverse iteration starting from the last element.

---

### `const void* vector_crend(Vector* vec)`

**Purpose**: Returns a constant pointer to the element preceding the first element in the reversed vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to one before the first element (reverse-end sentinel).

**Usage Case**: Use as the end bound for read-only reverse iteration.

---

### `void* vector_begin(Vector* vec)`

**Purpose**: Returns a pointer to the first element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the first element.

**Usage Case**: Use to start forward iteration over the vector's elements.

---

### `void* vector_end(Vector* vec)`

**Purpose**: Returns a pointer to the element following the last element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to one past the last element (end sentinel).

**Usage Case**: Use as the end bound for forward iteration.

---

### `void* vector_pop_back(Vector* vec)`

**Purpose**: Removes the last element from the vector and returns a pointer to it.

**Parameters**:
- `vec`: The vector to modify.

**Return Value**: A pointer to the removed last element.

**Usage Case**: Use to retrieve and remove the last element from the vector.

---

### `void* vector_front(Vector* vec)`

**Purpose**: Returns a pointer to the first element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the first element.

**Usage Case**: Use when you need to read or modify the first element without removing it.

---

### `void* vector_back(Vector* vec)`

**Purpose**: Returns a pointer to the last element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the last element.

**Usage Case**: Use when you need to read or modify the last element without removing it.

---

### `void* vector_data(Vector* vec)`

**Purpose**: Returns a pointer to the underlying array used by the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the underlying array of elements.

**Usage Case**: Use when you need direct access to the raw array, such as for passing to C APIs.

---

### `void* vector_at(const Vector* vec, size_t pos)`

**Purpose**: Returns a pointer to the element at the specified position.

**Parameters**:
- `vec`: The vector to access.
- `pos`: The position of the element to access.

**Return Value**: A pointer to the element at the specified position.

**Usage Case**: Use for random access to any element by index.

---

### `void* vector_rbegin(Vector* vec)`

**Purpose**: Returns a pointer to the last element in the vector (start of reverse iteration).

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the last element of the vector.

**Usage Case**: Use to start reverse iteration over the vector's elements.

---

### `void* vector_rend(Vector* vec)`

**Purpose**: Returns a pointer to the element preceding the first element of the vector (end of reverse iteration).

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to one before the first element (reverse-end sentinel).

**Usage Case**: Use as the end bound for reverse iteration.

---

### `size_t vector_size(const Vector* vec)`

**Purpose**: Returns the number of elements currently stored in the vector.

**Parameters**:
- `vec`: The vector whose size is to be returned.

**Return Value**: The number of elements in the vector.

**Usage Case**: Use to check how many elements are currently stored before iterating or accessing.

---

### `size_t vector_capacity(Vector* vec)`

**Purpose**: Returns the total capacity of the vector (the number of elements it can store without resizing).

**Parameters**:
- `vec`: The vector whose capacity is to be returned.

**Return Value**: The capacity of the vector.

**Usage Case**: Use to understand how much space is pre-allocated before a reallocation occurs.

---

### `size_t vector_max_size(Vector* vec)`

**Purpose**: Returns the maximum number of elements that the vector can hold, based on the system's limitations.

**Parameters**:
- `vec`: The vector whose maximum size is to be returned.

**Return Value**: The maximum number of elements the vector can hold.

**Usage Case**: Use to guard against requesting sizes larger than what the system can support.

---

## Examples

Several examples are provided to demonstrate the usage of the Vector library in various scenarios, including handling primitive data types, strings, and custom structures. These examples also show how to perform operations like iteration, sorting, and comparison on vectors.


```c
#include "vector/vector.h"
```

## Example 1: Integers

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector* intVector = vector_create(sizeof(int));
    int value = 10;
    vector_push_back(intVector, &value);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*)vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
10
```

---

## Example 2: Strings

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector* stringVector = vector_create(sizeof(char*));
    char *str = "Hello, World!";

    vector_push_back(stringVector, &str);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(stringVector); ++i) {
        char** item = (char**)vector_at(stringVector, i);
        fmt_printf("%s\n", *item);
    }

    // Cleanup
    vector_deallocate(stringVector);
    return 0;
}
```
**Result**
```
Hello, World!
```

---

## Example 3: User-Defined Structures

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

typedef struct {
    int id;
    double value;

} MyStruct;

int main() {   
    Vector* structVector = vector_create(sizeof(MyStruct));
    MyStruct s = {1, 10.5};

    vector_push_back(structVector, &s);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(structVector); ++i) {
        MyStruct* item = (MyStruct*)vector_at(structVector, i);
        fmt_printf("ID: %d, Value: %.2f\n", item->id, item->value);
    }

    vector_deallocate(structVector);   
    return 0;
}
```
**Result**
```
ID: 1, Value: 10.50
```

---

## Example 4: Use relational operators

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector* vec1 = vector_create(sizeof(int));
    Vector* vec2 = vector_create(sizeof(int));
    int items1[] = {1, 2, 3};
    int items2[] = {1, 2, 4};

    for (int i = 0; i < 3; i++) {
        vector_push_back(vec1, &items1[i]);
    }
    for (int i = 0; i < 3; i++) {
        vector_push_back(vec2, &items2[i]);
    }

    if (vector_is_equal(vec1, vec2)) { 
        fmt_printf("vec1 is equal to vec2\n");
    }
    else {
        fmt_printf("vec1 is not equal to vec2\n");
    }

    if (vector_is_less(vec1, vec2)) { 
        fmt_printf("vec1 is less than vec2\n");
    }
    if (vector_is_greater(vec2, vec1)) {
        fmt_printf("vec2 is greater than vec1\n");
    }
    // Cleanup
    vector_deallocate(vec1);
    vector_deallocate(vec2);
        
    return 0;
}
```
**Result**
```
vec1 is not equal to vec2
vec1 is less than vec2
vec2 is greater than vec1
```

---

## Example 5: Use at and char*

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int value1 = 10, value2 = 20, value3 = 30;

    vector_push_back(intVector, &value1);
    vector_push_back(intVector, &value2);
    vector_push_back(intVector, &value3);

    for (size_t i = 0; i < (vector_size(intVector)); ++i) {
        int* item = (int*)vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    Vector *stringVector = vector_create(sizeof(char*));
    const char *str1 = "Hello";
    const char *str2 = "World";
    const char *str3 = "Example";

    vector_push_back(stringVector, &str1);
    vector_push_back(stringVector, &str2);
    vector_push_back(stringVector, &str3);

    for (size_t i = 0; i < vector_size(stringVector); ++i) {
        char **item = (char**) vector_at(stringVector, i);
        fmt_printf("%s\n", *item);
    }

    vector_clear(intVector);
    vector_deallocate(intVector);
    vector_clear(stringVector);
    vector_deallocate(stringVector);
    
    return 0;
}
```
**Result**
```
10
20
30
Hello
World
Example
```

---

## Example 6: how to use 'swap'

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *vector1 = vector_create(sizeof(int));
    Vector *vector2 = vector_create(sizeof(int));
    int value1 = 10, value2 = 20;

    vector_push_back(vector1, &value1);
    vector_push_back(vector1, &value2);

    int value3 = 30, value4 = 40;
    vector_push_back(vector2, &value3);
    vector_push_back(vector2, &value4);

    vector_swap(vector1, vector2);

    fmt_printf("Contents of vector1 after swap:\n");
    for (size_t i = 0; i < vector_size(vector1); ++i) {
        int* item = (int*) vector_at(vector1, i);
        fmt_printf("%d\n", *item);
    }

    fmt_printf("Contents of vector2 after swap:\n");
    for (size_t i = 0; i < vector_size(vector2); ++i) {
        int* item = (int*)vector_at(vector2, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);   

    return 0;
}
```
**Result**
```
Contents of vector1 after swap:
30
40
Contents of vector2 after swap:
10
20
```

---

## Example 7: how to use `vector_pop_back`

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40};

    for (int i = 0; i < 4; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Pop the last element and access it
    int *poppedItem = (int *)vector_pop_back(intVector);
    if (poppedItem) {
        fmt_printf("Popped item: %d\n", *poppedItem);
    }

    vector_deallocate(intVector);       
    return 0;
}
```
**Result**
```
Popped item: 40
```

---

## Example 8: how to use `vector_front`

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    int *frontItem = (int *)vector_front(intVector);
    if (frontItem) {
        fmt_printf("First item: %d\n", *frontItem);
    }

    vector_deallocate(intVector);        
    return 0;
}
```
**Result**
```
First item: 10
```

---

## Example 9: how to use `vector_back`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) {
        vector_push_back(intVector, &values[i]);
    }

    int *backItem = (int *)vector_back(intVector);
    if (backItem) { 
        fmt_printf("Last item: %d\n", *backItem);
    }

    vector_deallocate(intVector);        
    return 0;
}
```
**Result**
```
Last item: 30
```

---

## Example 10: use `vector_data` Returns pointer to the underlying array serving as element storage
```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Accessing the underlying array
    int *data = (int *)vector_data(intVector);
    if (data){
        for (size_t i = 0; i < vector_size(intVector); ++i) { 
            fmt_printf("Item %zu: %d\n", i, data[i]);
        }
    }
        
    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
Item 0: 10
Item 1: 20
Item 2: 30
```

---

## Example 11: `vector_begin`, `vector_end` Return pointer to the first and last element

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using begin and end to iterate over the vector
    for (int *it = (int *)vector_begin(intVector); it != (int *)vector_end(intVector); ++it) { 
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
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

## Example 12: `vector_cbegin`, `vector_cend` Return const pointer to first and last element

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using cbegin and cend for read-only iteration over the vector
    for (const int *it = (const int *)vector_cbegin(intVector); it != (const int *)vector_cend(intVector); ++it) {
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
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

## Example 13 : `vector_crbegin`, `vector_crend` Return reverse read-only iteration over the vector
```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using crbegin and crend for reverse read-only iteration over the vector
    for (const int *it = (const int *)vector_crbegin(intVector); it != (const int *)vector_crend(intVector); --it) {
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
30
20
10
```

---

## Example 14: Using `vector_rbegin` and `vector_rend` for reverse iteration over the vector

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using rbegin and rend for reverse iteration over the vector
    for (int *it = (int *)vector_rbegin(intVector); it != (int *)vector_rend(intVector); --it) { 
        fmt_printf("%d\n", *it);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
30
20
10
```

---

## Example 15: `vector_insert` New element to vector 

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Insert a new element
    int newElement = 25;
    vector_insert(intVector, 0, &newElement); // Inserts 25 at position 0
    
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
25
10
20
30
```

---

## Example 16: `vector_erase` element from vector

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Erase elements from position 1, removing 2 elements
    vector_erase(intVector, 1, 2); // Should remove 20 and 30

    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
10
40
50
```

---

## Example 17: `vector_reserve`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *intVector = vector_create(sizeof(int));

    // Reserve capacity for at least 10 elements
    vector_reserve(intVector, 10);
    for (int i = 0; i < 11; ++i) {
        int value = i * 10;
        vector_push_back(intVector, &value);
    }
    fmt_printf("Vector size: %zu, Vector capacity: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
Vector size: 11, Vector capacity: 32
```

---

## Example 18: resize size of the vector with `vector_resize` 

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Resize to a larger size (5)
    vector_resize(intVector, 5);
    fmt_printf("After resizing to larger size:\n");

    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item); 
    }

    // Resize to a smaller size (2)
    vector_resize(intVector, 2);

    fmt_printf("After resizing to smaller size:\n");
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item); // Only the first two elements remain
    }

    vector_clear(intVector);
    vector_deallocate(intVector);

    return 0;
}
```
**Result**
```
After resizing to larger size:
10
20
30
0
0
After resizing to smaller size:
10
20
```

---

## Example 19 : `vector_shrink_to_fit` fixed capacity

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *intVector = vector_create(sizeof(int));

    // Reserve larger capacity
    vector_reserve(intVector, 10);

    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) {
        vector_push_back(intVector, &values[i]);
    }

    fmt_printf("Size before shrink_to_fit: %zu, Capacity before shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    // Shrink to fit the actual number of elements
    vector_shrink_to_fit(intVector);    
    fmt_printf("Size after shrink_to_fit: %zu, Capacity after shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
Size before shrink_to_fit: 3, Capacity before shrink_to_fit: 32
Size after shrink_to_fit: 3, Capacity after shrink_to_fit: 3
```

---

## Example 20 : use `vector_emplace` and `vector_emplace_back` insert new element at positin and at the end of 

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *stringVector = vector_create(sizeof(char*));
    char* values[] = {"Hello", "World", "Vector", "Example"};

    for (int i = 0; i < 4; ++i) { 
        vector_push_back(stringVector, &values[i]);
    }

    char* newValue = "NewString";
    vector_assign(stringVector, 1, &newValue);

    char* emplaceValue = "EmplacedString";
    vector_emplace(stringVector, 2, &emplaceValue, sizeof(char*));

    char* emplaceBackValue = "EmplacedBackString";
    vector_emplace_back(stringVector, &emplaceBackValue, sizeof(char*));

    for (size_t i = 0; i < vector_size(stringVector); ++i) {
        char** item = (char **)vector_at(stringVector, i);
        fmt_printf("%s\n", *item);
    }

    vector_deallocate(stringVector);
    return 0;
}
```
**Result**
```
Hello
NewString
EmplacedString
Vector
Example
EmplacedBackString
```

---

## Example 21 : how to use `String` object in Vector

```c
#include "string/std_string.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector* vec = vector_create(sizeof(String*));
    String* myString1 = string_create("Hello");
    String* myString2 = string_create("World");
    String* myString3 = string_create("Example");
    String** strPtr = NULL;

    vector_push_back(vec, &myString1);
    vector_push_back(vec, &myString2);
    vector_push_back(vec, &myString3);

    for (size_t i = 0; i < vector_size(vec); ++i) {
        strPtr = (String**) vector_at(vec, i);
        if (strPtr && *strPtr) {
            fmt_printf("%s\n", (*strPtr)->dataStr); 
        }
    }

    string_deallocate(myString1);
    string_deallocate(myString2);
    string_deallocate(myString3);

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Hello
World
Example
```

---

## Example 22 : how to use relationals operators in `Vector` 

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *vector1 = vector_create(sizeof(int));
    Vector *vector2 = vector_create(sizeof(int));

    int value1 = 10, value2 = 20;
    vector_push_back(vector1, &value1);
    vector_push_back(vector1, &value2);

    int value3 = 30, value4 = 40;
    vector_push_back(vector2, &value3);
    vector_push_back(vector2, &value4);
    
    if (vector_is_equal(vector1, vector2)) {
        fmt_printf("Vector1 is equal with Vector2\n");
    }
    if (vector_is_less(vector1, vector2)) {
        fmt_printf("Vector1 is less than Vector2\n");
    }
    if (vector_is_greater(vector1, vector2)) {
        fmt_printf("Vector1 is greater than Vector2\n");
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);

    return 0;
}
```
**Result**
```
Vector1 is less than Vector2
```

---

## Example 23 : how to use `vector_begin` and `vector_end` with String Object

```c
#include "string/std_string.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {

    Vector *vec = vector_create(sizeof(String*));
    String *fruits[5] = {
        string_create("Apple"),
        string_create("Banana"),
        string_create("Cherry"),
        string_create("Lemon"),
        string_create("Watermelon")
    };
    String *concat = string_create("");

    for (size_t index = 0; index < 5; index++) {
        vector_push_back(vec, &fruits[index]);
    }

    Time* start_time = time_current_time();

    for (size_t index = 0; index < vector_size(vec); ++index) {
        String **strPtr = (String**)vector_at(vec, index);
        string_push_back(*strPtr, '\n');
        string_concatenate(concat, *strPtr);
    }
    
    for (size_t index = 0; index < 5; index++) {
        string_deallocate(fruits[index]);
    }

    vector_deallocate(vec);

    Time* end_time = time_current_time();
    double time_elapsed = time_diff_in_seconds(start_time, end_time);
    
    fmt_printf("%s", string_c_str(concat));
    fmt_printf("Time taken: %lf seconds\n", time_elapsed);

    time_deallocate(start_time);
    time_deallocate(end_time);
    string_deallocate(concat);
    return 0;
}
```
**Result in C:**
```
Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 0.000009 seconds
```

`also result in Cpp` 

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>

int main() {
    std::vector<std::string> fruits = {"Apple", "Banana", "Cherry", "Lemon", "Watermelon"};
    std::stringstream buffer;

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& fruit : fruits) { 
        buffer << fruit << "\n";
    }
    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << buffer.str(); // Single print call
    std::cout << "Time taken: " << elapsed.count() << " seconds\n";

    return 0;
}
```
**Result in C++:**
```
Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 1.2074e-05 seconds
```

---

## Example 24: Vector of Vectors (2D Vector)

This example creates a two-dimensional vector, where each element of the main vector is another vector. This can be useful for matrix-like structures or grid representations.
```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    // Create a vector of vectors (2D vector)
    Vector* vec2D = vector_create(sizeof(Vector*));

    for (int i = 0; i < 3; i++) {
        Vector* subVec = vector_create(sizeof(int));
        for (int j = 0; j < 3; j++) {
            int value = i * 3 + j;
            vector_push_back(subVec, &value);
        }
        vector_push_back(vec2D, &subVec);
    }

    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** subVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* subVec = *subVecPtr;

        for (size_t j = 0; j < vector_size(subVec); j++) {
            int* valPtr = (int*)vector_at(subVec, j);
            fmt_printf("%d ", *valPtr);
        }
        fmt_printf("\n");
    }

    // Cleanup
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** subVecPtr = (Vector**)vector_at(vec2D, i);
        vector_deallocate(*subVecPtr);
    }
    
    vector_deallocate(vec2D);
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

## Example 25: Vector of Structs with Dynamic String Fields

This example demonstrates how to use the Vector library to store a collection of structs, where each struct contains dynamically allocated string fields (using the String library).
```c
#include "vector/vector.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

typedef struct {
    String* name;
    String* description;
} Item;

int main() {
    Vector* items = vector_create(sizeof(Item));
    const char* names[] = {"Item1", "Item2", "Item3"};
    const char* descriptions[] = {"Description1", "Description2", "Description3"};

    for (int i = 0; i < 3; i++) {
        Item item;
        item.name = string_create(names[i]);
        item.description = string_create(descriptions[i]);

        vector_push_back(items, &item);
    }

    for (size_t i = 0; i < vector_size(items); i++) {
        Item* itemPtr = (Item*)vector_at(items, i);
        fmt_printf("Name: %s, Description: %s\n", 
               string_c_str(itemPtr->name), 
               string_c_str(itemPtr->description));
    }

    for (size_t i = 0; i < vector_size(items); i++) {
        Item* itemPtr = (Item*)vector_at(items, i);
        string_deallocate(itemPtr->name);
        string_deallocate(itemPtr->description);
    }

    vector_deallocate(items);
    return 0;
}
```
**Result**
```
Name: Item1, Description: Description1
Name: Item2, Description: Description2
Name: Item3, Description: Description3
```

---

## Example 26: Vector of Vectors (2D Vector) of Custom Structs

```c
#include <stdlib.h>
#include "vector/vector.h"
#include "string/std_string.h"
#include "fmt/fmt.h"


typedef struct {
    String* name;
    int age;
} Person;

Person* create_person(int group, int index) {
    Person* p = (Person*)malloc(sizeof(Person));
    p->name = string_create("");
    string_format(p->name, "Person_%d_%d", group, index);
    p->age = group * 10 + index;

    return p;
}

void deallocate_person(Person* p) {
    if (p) {
        string_deallocate(p->name);
        free(p);
    }
}

int main() {
    Vector* vec2D = vector_create(sizeof(Vector*));

    // Populate the 2D vector with vectors of Persons
    for (int i = 0; i < 2; i++) {
        Vector* peopleVec = vector_create(sizeof(Person*));

        for (int j = 0; j < 3; j++) {
            Person* person = create_person(i, j);
            vector_push_back(peopleVec, &person);
        }
        vector_push_back(vec2D, &peopleVec);
    }

    // Iterate and print each person's details
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** peopleVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* peopleVec = *peopleVecPtr;

        for (size_t j = 0; j < vector_size(peopleVec); j++) {
            Person** personPtr = (Person**)vector_at(peopleVec, j);
            Person* person = *personPtr;

            fmt_printf("Name: %s, Age: %d\n", string_c_str(person->name), person->age);
        }
    }

    // Cleanup
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** peopleVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* peopleVec = *peopleVecPtr;

        for (size_t j = 0; j < vector_size(peopleVec); j++) {
            Person** personPtr = (Person**)vector_at(peopleVec, j);
            deallocate_person(*personPtr);
        }
        vector_deallocate(peopleVec);
    }

    vector_deallocate(vec2D);
    return 0;
}
```
**Result**
```
Name: Person_0_0, Age: 0
Name: Person_0_1, Age: 1
Name: Person_0_2, Age: 2
Name: Person_1_0, Age: 10
Name: Person_1_1, Age: 11
Name: Person_1_2, Age: 12
```

---

## Example 27 : sort and search a Vector with the `Algorithm` library

This example stores integers in a vector, then sorts the vector's underlying storage in place with `algorithm_sort` and locates a value with `algorithm_find`. `vector_data` exposes the contiguous buffer the algorithm functions operate on.

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Vector* vec = vector_create(sizeof(int));
    int values[] = {42, 7, 19, 3, 25, 11};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Sort the vector's underlying storage in place with algorithm_sort
    algorithm_sort(vector_data(vec), vector_size(vec), sizeof(int), compare_ints);

    fmt_printf("Sorted: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%d ", *(int*)vector_at(vec, i));
    }
    fmt_printf("\n");

    // Search the sorted data with algorithm_find
    int key = 19;
    int* found = (int*)algorithm_find(vector_data(vec), vector_size(vec), sizeof(int), &key, compare_ints);
    if (found) {
        fmt_printf("Found %d at index %ld\n", *found, (long)(found - (int*)vector_data(vec)));
    }

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Sorted: 3 7 11 19 25 42 
Found 19 at index 3
```

---

## Example 28 : build a delimited `String` from a Vector of integers

This example converts each integer in a vector to a `String` with `string_from_int` and appends it to a running `String` to produce a comma-separated list. Each temporary piece is deallocated as soon as it is appended.

```c
#include "vector/vector.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main(void) {
    Vector* vec = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Build a single comma-separated String from the vector's integers
    String* csv = string_create("");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int* item = (int*)vector_at(vec, i);
        String* piece = string_from_int(*item);

        if (i > 0) {
            string_append(csv, ", ");
        }
        string_append(csv, string_c_str(piece));
        string_deallocate(piece);
    }

    fmt_printf("Joined: %s\n", string_c_str(csv));
    fmt_printf("Length: %zu\n", string_length(csv));

    string_deallocate(csv);
    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Joined: 10, 20, 30, 40
Length: 14
```

---

## Example 29 : copy a fixed `Array` into a Vector and reduce it

This example uses the compile-time fixed-size `Array` library as a data source: every element is copied into a dynamic `Vector`, and `array_reduce` collapses the fixed array down to a single sum.

```c
#include "vector/vector.h"
#include "array/array.h"
#include "fmt/fmt.h"

array_create(int, 5, ArrayInt5);

static void sum_reduce(const void* acc, const void* elem, void* out) {
    *(int*)out = *(const int*)acc + *(const int*)elem;
}

int main(void) {
    // A compile-time fixed array as the source of data
    ArrayInt5 fixed = { .data = {5, 10, 15, 20, 25} };

    // Copy every element of the fixed array into a dynamic vector
    Vector* vec = vector_create(sizeof(int));
    for (size_t i = 0; i < array_size(fixed); ++i) {
        int v = array_at(fixed, i);
        vector_push_back(vec, &v);
    }

    fmt_printf("Vector contents: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%d ", *(int*)vector_at(vec, i));
    }
    fmt_printf("\n");

    // Reduce the fixed array to a single sum using the array API
    int total = 0;
    array_reduce(fixed, &total, sum_reduce);

    fmt_printf("Sum from array_reduce: %d\n", total);
    fmt_printf("Vector size: %zu\n", vector_size(vec));

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Vector contents: 5 10 15 20 25 
Sum from array_reduce: 75
Vector size: 5
```

---

## Example 30 : serialize a Vector of numbers as a `Json` array

This example walks a vector of `double` values, wraps each in a `JSON_NUMBER` element, appends them to a `JSON_ARRAY`, and serializes the result. The serialized buffer returned by `json_serialize` is `free`d and the whole element tree is released with `json_deallocate`.

```c
#include "vector/vector.h"
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    Vector* vec = vector_create(sizeof(double));
    double values[] = {1.5, 2.0, 3.25, 4.0};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Build a JSON array element from the vector's numbers
    JsonElement* arr = json_create(JSON_ARRAY);
    for (size_t i = 0; i < vector_size(vec); ++i) {
        double* item = (double*)vector_at(vec, i);
        JsonElement* num = json_create(JSON_NUMBER);
        num->value.number_val = *item;
        json_add_to_array(arr, num);
    }

    fmt_printf("Array size: %zu\n", json_array_size(arr));

    char* serialized = json_serialize(arr);
    fmt_printf("JSON: %s\n", serialized);
    free(serialized);

    json_deallocate(arr);
    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Array size: 4
JSON: [1.5, 2, 3.25, 4]
```

---

## Example 31 : compute descriptive statistics over a Vector

Because `vector_data` returns a contiguous block, a vector of `double` can be handed straight to the `Statistics` library. This example computes the mean, median, and population standard deviation of the stored samples.

```c
#include "vector/vector.h"
#include "statistics/statistics.h"
#include "fmt/fmt.h"

int main(void) {
    Vector* vec = vector_create(sizeof(double));
    double samples[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};

    for (size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        vector_push_back(vec, &samples[i]);
    }

    // vector_data gives a contiguous double[] the statistics API can read directly
    const double* data = (const double*)vector_data(vec);
    size_t n = vector_size(vec);

    fmt_printf("Count:  %zu\n", n);
    fmt_printf("Mean:   %.4f\n", statistics_mean(data, n));
    fmt_printf("Median: %.4f\n", statistics_median(data, n));
    fmt_printf("Pstdev: %.4f\n", statistics_pstdev(data, n, false, 0.0));

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Count:  8
Mean:   5.0000
Median: 4.5000
Pstdev: 2.0000
```

---

## License

This project is open-source and available under [ISC License].