# Vector Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2023
**License:** ISC License

## Overview

This Vector library, written in C, is an implementation that mimics the functionality of the C++ `std::vector`. It aims to provide dynamic array capabilities to C programs. This library is part of a project to reimplement features of the C++ standard library in C, making it useful for developers familiar with C++ but working in C environments.

The Vector library allows for creating dynamic arrays that can resize automatically when elements are added or removed. It supports various operations such as inserting, deleting, and accessing elements, as well as more advanced functionalities like memory pooling for efficient allocation and deallocation of elements.

## Key Features

- **Dynamic Resizing:** Automatically adjust size when elements are added or removed.
- **Memory Efficiency:** Incorporates a memory pool mechanism to manage memory allocation efficiently.
- **Generic Container:** Can store elements of any data type, including structures.
- **Various Utility Functions:** Functions for comparison, swapping, and checking the state of the vector.

## Installation and Compilation

To use this library, include `vector.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./vector/vector.c ./string/string.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like creating a vector, adding and removing elements, as well as more complex functionalities such as memory pooling and vector comparison.

### Function Descriptions:

### `Vector* vector_create(size_t itemSize)`   
  - **Purpose**: Initializes and returns a new vector capable of storing elements of the specified size.
  - **Parameters:**  
    - `itemSize`: The size in bytes of each element that will be stored in the vector.
  - **Returns:**  
    - A pointer to the newly created vector. Returns `NULL` if memory allocation fails.

---

### `bool vector_is_equal(const Vector* vec1, const Vector* vec2)`  
  - **Purpose**: Compares two vectors and checks if they are equal in terms of both size and content.
  - **Parameters:**  
    - `vec1`: The first vector to compare.  
    - `vec2`: The second vector to compare.
  - **Returns:**  
    - `true` if both vectors have identical elements in the same order, otherwise `false`.

---

### `bool vector_is_less(const Vector* vec1, const Vector* vec2)`  
  - **Purpose**: Checks if the first vector is lexicographically less than the second vector.
  - **Parameters:**  
    - `vec1`: The first vector to compare.  
    - `vec2`: The second vector to compare.
  - **Returns:**  
    - `true` if `vec1` is lexicographically smaller than `vec2`, otherwise `false`.

---

### `bool vector_is_greater(const Vector* vec1, const Vector* vec2)` 
  - **Purpose**: Checks if the first vector is lexicographically greater than the second vector.
  - **Parameters:**  
    - `vec1`: The first vector to compare.  
    - `vec2`: The second vector to compare.
  - **Returns:**  
    - `true` if `vec1` is lexicographically greater than `vec2`, otherwise `false`.

---

### `bool vector_is_not_equal(const Vector* vec1, const Vector* vec2)`   
  - **Purpose**: Compares two vectors and checks if they are not equal.
  - **Parameters:**  
    - `vec1`: The first vector to compare.  
    - `vec2`: The second vector to compare.
  - **Returns:**  
    - `true` if the vectors are not equal, otherwise `false`.

---

### `bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2)`   
  - **Purpose**: Checks if the first vector is lexicographically greater than or equal to the second vector.
  - **Parameters:**  
    - `vec1`: The first vector to compare.  
    - `vec2`: The second vector to compare.
  - **Returns:**  
    - `true` if `vec1` is greater than or equal to `vec2`, otherwise `false`.

---

### `bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2)`   
  - **Purpose**: Checks if the first vector is lexicographically less than or equal to the second vector.
  - **Parameters:**  
    - `vec1`: The first vector to compare.  
    - `vec2`: The second vector to compare.
  - **Returns:**  
    - `true` if `vec1` is less than or equal to `vec2`, otherwise `false`.

---

### `bool vector_is_empty(Vector* vec)`  
  - **Purpose**: Checks if the vector is empty, meaning it contains no elements.
  - **Parameters:**  
    - `vec`: The vector to check.
  - **Returns:**  
    - `true` if the vector is empty, otherwise `false`.

---

### `bool vector_reserve(Vector* vec, size_t size)`  
  - **Purpose**: Allocates enough memory for the vector to hold at least the specified number of elements without resizing. This is useful for preallocating memory to avoid frequent reallocations.
  - **Parameters:**  
    - `vec`: The vector to reserve space for.  
    - `size`: The minimum number of elements to reserve capacity for.
  - **Returns:**  
    - `true` if memory allocation is successful, otherwise `false`.

---

### `bool vector_push_back(Vector* vec, const void* item)`   
  - **Purpose**: Adds an item to the end of the vector, resizing the vector if necessary.
  - **Parameters:**  
    - `vec`: The vector to which the item is added.  
    - `item`: A pointer to the item being added to the vector.
  - **Returns:**  
    - `true` if the operation succeeds, otherwise `false` (e.g., memory allocation failure).

---

### `bool vector_emplace_back(Vector *vec, void *item, size_t itemSize)`   
  - **Purpose**: Constructs an item in place at the end of the vector, potentially avoiding copying and allowing more efficient insertion.
  - **Parameters:**  
    - `vec`: The vector to add the item to.  
    - `item`: A pointer to the item to be added.  
    - `itemSize`: The size of the item being added.
  - **Returns:**  
    - `true` if successful, otherwise `false`.

---

### `void vector_erase(Vector* vec, size_t pos, size_t len)`   
  - **Purpose**: Erases a range of elements from the vector, starting at the specified position and removing the specified number of elements.
  - **Parameters:**  
    - `vec`: The vector from which elements will be removed.  
    - `pos`: The starting position of the elements to be removed.  
    - `len`: The number of elements to erase.

---

### `void vector_insert(Vector* vec, size_t pos, void* item)`   
  - **Purpose**: Inserts an item into the vector at the specified position, shifting subsequent elements to the right.
  - **Parameters:**  
    - `vec`: The vector where the item will be inserted.  
    - `pos`: The index at which the item will be inserted.  
    - `item`: A pointer to the item to insert.

---

### `void vector_resize(Vector* vec, size_t size)`   
  - **Purpose**: Resizes the vector to contain the specified number of elements. If the new size is larger, additional elements will be uninitialized.
  - **Parameters:**  
    - `vec`: The vector to resize.  
    - `size`: The new size of the vector.

---

### `void vector_shrink_to_fit(Vector* vec)`  
  - **Purpose**: Shrinks the capacity of the vector to match its current size, freeing any excess allocated memory.
  - **Parameters:**  
    - `vec`: The vector to shrink.

---

### `void vector_clear(Vector* vec)`   
  - **Purpose**: Clears all elements from the vector, setting its size to 0 but maintaining its current capacity.
  - **Parameters:**  
    - `vec`: The vector to clear.

---

### `void vector_swap(Vector* vec1, Vector* vec2)`  
  - **Purpose**: Swaps the contents of two vectors, exchanging their sizes, capacities, and elements.
  - **Parameters:**  
    - `vec1`: The first vector.  
    - `vec2`: The second vector.

---

### `void vector_assign(Vector* vec, size_t pos, void* item)`   
  - **Purpose**: Assigns a new value to the element at the specified position.
  - **Parameters:**  
    - `vec`: The vector to modify.  
    - `pos`: The position of the element to assign a new value to.  
    - `item`: A pointer to the new item.

---

### `void vector_emplace(Vector* vec, size_t pos, void* item, size_t itemSize)`   
  - **Purpose**: Constructs an item in place at the specified position within the vector.
  - **Parameters:**  
    - `vec`: The vector where the item will be constructed.  
    - `pos`: The index at which to construct the item.  
    - `item`: A pointer to the item to be placed.  
    - `itemSize`: The size of the item.

---

### `void vector_deallocate(Vector *vec)`   
  - **Purpose**: Frees all memory associated with the vector, including its elements and the vector structure itself.
  - **Parameters:**  
    - `vec`: The vector to deallocate.

---

### `const void* vector_cbegin(Vector* vec)` 
  - **Purpose**: Returns a constant pointer to the first element in the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A constant pointer to the first element of the vector.

---

### `const void* vector_cend(Vector* vec)`   
  - **Purpose**: Returns a constant pointer to the element following the last element in the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A constant pointer to the element following the last element.

---

### `const void* vector_crbegin(Vector* vec)`   
  - **Purpose**: Returns a constant pointer to the first element of the reversed vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A constant pointer to the first element in the reversed vector.

---

### `const void* vector_crend(Vector* vec)`   
  - **Purpose**: Returns a constant pointer to the element following the last element of the reversed vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A constant pointer to the element after the last in the reversed vector.

---

### `void* vector_begin(Vector* vec)`  
  - **Purpose**: Returns a pointer to the first element in the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the first element.

---

### `void* vector_end(Vector* vec)`  
  - **Purpose**: Returns a pointer to the element following the last element in the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the element following the last.

---

### `void* vector_pop_back(Vector* vec)`   
  - **Purpose**: Removes the last element from the vector and returns a pointer to it.
  - **Parameters:**  
    - `vec`: The vector to modify.
  - **Returns:**  
    - A pointer to the last element removed.

---

- **`void* vector_front(Vector* vec)`**  
  - **Purpose**: Returns a pointer to the first element in the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the first element.

---

### `void* vector_back(Vector* vec)`   
  - **Purpose**: Returns a pointer to the last element in the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the last element.

---

### `void* vector_data(Vector* vec)`   
  - **Purpose**: Returns a pointer to the underlying array used by the vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the underlying array of elements.

---

### `void* vector_at(const Vector* vec, size_t pos)`   
  - **Purpose**: Returns a pointer to the element at the specified position.
  - **Parameters:**  
    - `vec`: The vector to access.  
    - `pos`: The position of the element to access.
  - **Returns:**  
    - A pointer to the element at the specified position.

---

### `void* vector_rbegin(Vector* vec)` 
  - **Purpose**: Returns a pointer to the first element of the reversed vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the first element of the reversed vector.

---

### `void* vector_rend(Vector* vec)`   
  - **Purpose**: Returns a pointer to the element following the last element of the reversed vector.
  - **Parameters:**  
    - `vec`: The vector to access.
  - **Returns:**  
    - A pointer to the element following the last in the reversed vector.

---

### `size_t vector_size(const Vector* vec)`   
  - **Purpose**: Returns the number of elements currently stored in the vector.
  - **Parameters:**  
    - `vec`: The vector whose size is to be returned.
  - **Returns:**  
    - The number of elements in the vector.

---

### `size_t vector_capacity(Vector* vec)` 
  - **Purpose**: Returns the total capacity of the vector (the number of elements it can store without resizing).
  - **Parameters:**  
    - `vec`: The vector whose capacity is to be returned.
  - **Returns:**  
    - The capacity of the vector.

---

### `size_t vector_max_size(Vector* vec)`   
  - **Purpose**: Returns the maximum number of elements that the vector can hold, based on the system's limitations.
  - **Parameters:**  
    - `vec`: The vector whose maximum size is to be returned.
  - **Returns:**  
    - The maximum number of elements the vector can hold.

---

## Examples

Several examples are provided to demonstrate the usage of the Vector library in various scenarios, including handling primitive data types, strings, and custom structures. These examples also show how to perform operations like iteration, sorting, and comparison on vectors.


```c
#include "vector/vector.h"
```

### Example 1: Integers

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
**Result:**
```
10
```

---

### Example 2: Strings

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
**Result:**
```
Hello, World!
```

---

### Example 3: User-Defined Structures

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
**Result:**
```
ID: 1, Value: 10.50
```

---

### Example 4: Use relational operators

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
        fmt_printf("vec2 is greater than vec2\n");
    }
    // Cleanup
    vector_deallocate(vec1);
    vector_deallocate(vec2);
        
    return 0;
}
```
**Result:**
```
vec1 is not equal to vec2
vec1 is less than vec2
vec2 is greater than vec2
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
**Result:**
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

    fmt_println("Contents of vector1 after swap:");
    for (size_t i = 0; i < vector_size(vector1); ++i) {
        int* item = (int*) vector_at(vector1, i);
        fmt_printf("%d\n", *item);
    }

    fmt_println("Contents of vector2 after swap:");
    for (size_t i = 0; i < vector_size(vector2); ++i) {
        int* item = (int*)vector_at(vector2, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);   

    return 0;
}
```
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
        fmt_printf("%d\n", *item); // The last two elements will be zero-initialized
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
```
Vector1 is less than Vector2
```

---

### Example 23 : how to use `vector_begin` and `vector_end` with String Object 

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

### Example 24 :Vector of Vectors (2D Vector)

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
**Result:**
```
0 1 2 
3 4 5
6 7 8
```

---

### Example 25 : Vector of Structs with Dynamic String Fields

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
**Result:**
```
Name: Item1, Description: Description1
Name: Item2, Description: Description2
Name: Item3, Description: Description3
```

---

### Example 26 : Vector of Vectors (2D Vector) of Custom Structs 

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
**Result:**
```
Name: Person_0_0, Age: 0
Name: Person_0_1, Age: 1
Name: Person_0_2, Age: 2
Name: Person_1_0, Age: 10
Name: Person_1_1, Age: 11
Name: Person_1_2, Age: 12
```

---

## License

This project is open-source and available under [ISC License].