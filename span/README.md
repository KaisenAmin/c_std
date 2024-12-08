# Span Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License

The Span library is a custom C implementation inspired by the C++ `std::span` container. It provides a view over a contiguous sequence of elements, allowing for efficient and safe access without owning the underlying data.

## Features
- **Non-owning:** Spans do not own the data they point to.
- **Memory-efficient:** Utilizes a memory pool for managing memory allocations.
- **Safe Access:** Provides bounds-checked access to elements.
- **Flexible:** Can be used with any contiguous data structure like arrays.
- **Iterator Support:** Supports iterator-like access to elements.

## Usage
To use the Span library, include the `span.h` header file in your C source file.

## Compilation
To compile the Span library along with your main program, ensure all source files are in the correct directory and use a command similar to:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main your_program .\span\span.c
```


### Structure Definition

#### **`Span`**
- **Definition**: A span is a non-owning view over a contiguous block of data. The `Span` structure represents this view.
  
  **Fields**:
  - `void* data`: A pointer to the data block.
  - `size_t size`: The number of elements in the span.
  - `size_t elemSize`: The size (in bytes) of each element in the span.

---

### Function Descriptions


### `Span* span_create(void* data, size_t elemCount, size_t elemSize)`
  - **Purpose**: Creates a new span that points to the given data with the specified number of elements (`elemCount`) and size of each element (`elemSize`).
  - **Parameters**:
    - `data`: A pointer to the data block.
    - `elemCount`: The number of elements in the span.
    - `elemSize`: The size of each element in bytes.
  - **Returns**: A pointer to the newly created `Span`.
  - **Use case**: Used when you want a non-owning view over an array or block of data without copying it.

---

### `void span_destroy(Span* span)`
  - **Purpose**: Destroys the span and cleans up its resources.
  - **Parameters**:
    - `span`: The span to be destroyed.
  - **Returns**: `void`.
  - **Use case**: Use when a span is no longer needed to release its resources, although the data itself (pointed by `data`) is not deallocated by this function.

---

### `void* span_begin(Span* span)`
  - **Purpose**: Returns a pointer to the first element in the span.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to the first element.
  - **Use case**: Used to iterate over the span starting from the first element.

---

### `const void* span_cbegin(const Span* span)`
  - **Purpose**: Returns a constant pointer to the first element in the span (read-only access).
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A constant pointer to the first element.
  - **Use case**: Used for iterating over a constant span where modification is not allowed.

---

### `void* span_end(Span* span)`
  - **Purpose**: Returns a pointer to one past the last element in the span (end iterator).
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to the end of the span.
  - **Use case**: Used to define the end of an iteration over the span.

---

### `const void* span_cend(const Span* span)`
  - **Purpose**: Returns a constant pointer to one past the last element in the span (read-only).
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A constant pointer to the end of the span.
  - **Use case**: Used when you want to iterate over the span without modifying the data.

---

### `void* span_rbegin(Span* span)`
  - **Purpose**: Returns a reverse iterator to the beginning of the span (last element in reverse iteration).
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to the last element in the span.
  - **Use case**: Used to iterate over the span in reverse order.

---

### `const void* span_crbegin(const Span* span)`
  - **Purpose**: Returns a constant reverse iterator to the beginning of the span (read-only access).
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A constant pointer to the last element.
  - **Use case**: Used for read-only reverse iteration.

---

### `void* span_rend(Span* span)`**
  - **Purpose**: Returns a reverse iterator pointing to one before the first element in the span.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to one before the first element.
  - **Use case**: Used to define the end of reverse iteration.

---

### `const void* span_crend(const Span* span)`**
  - **Purpose**: Returns a constant reverse iterator pointing to one before the first element in the span.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A constant pointer to one before the first element.
  - **Use case**: Used to define the end of reverse iteration in a read-only context.

---

### `void* span_front(Span* span)`
  - **Purpose**: Accesses the first element in the span.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to the first element.
  - **Use case**: Access the first element without iterating.

---

### `void* span_back(Span* span)`
  - **Purpose**: Accesses the last element in the span.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to the last element.
  - **Use case**: Access the last element without iterating.

---

### `void* span_at(Span* span, size_t index)`
  - **Purpose**: Accesses the element at the specified index with bounds checking.
  - **Parameters**:
    - `span`: The span to operate on.
    - `index`: The index of the element to access.
  - **Returns**: A pointer to the element at the specified index or `NULL` if the index is out of bounds.
  - **Use case**: Used to safely access an element within bounds.

---

### `void* span_data(Span* span)`
  - **Purpose**: Returns a pointer to the underlying data block.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A pointer to the data.
  - **Use case**: Provides direct access to the raw data.

---

### `const void* span_cdata(const Span* span)`
  - **Purpose**: Returns a constant pointer to the underlying data block.
  - **Parameters**:
    - `span`: The span to operate on.
  - **Returns**: A constant pointer to the data.
  - **Use case**: Provides read-only access to the raw data.

---

### `Span span_first(Span* span, size_t count)`
  - **Purpose**: Returns a new span consisting of the first `count` elements of the original span.
  - **Parameters**:
    - `span`: The span to operate on.
    - `count`: The number of elements to include in the new span.
  - **Returns**: A new `Span` with the first `count` elements.
  - **Use case**: Useful when working with a sub-range of data.

---

### `Span span_last(Span* span, size_t count)`
  - **Purpose**: Returns a new span consisting of the last `count` elements of the original span.
  - **Parameters**:
    - `span`: The span to operate on.
    - `count`: The number of elements to include in the new span.
  - **Returns**: A new `Span` with the last `count` elements.
  - **Use case**: Useful when working with the end of the data block.

### `Span span_subspan(Span* span, size_t offset, size_t count)`
  - **Purpose**: Returns a new span starting at the specified `offset` and consisting of `count` elements.
  - **Parameters**:
    - `span`: The span to operate on.
    - `offset`: The starting position of the subspan.
    - `count`: The number of elements in the subspan.
  - **Returns**: A new `Span` with the specified range.
  - **Use case**: Used when you need a subset of elements from the middle of the span.

---

### `bool span_is_equal(const Span* span1, const Span* span2)`
  - **Purpose**: Checks if two spans are equal (based on element comparison).
  - **Parameters**:
    - `span1`: The first span.
    - `span2`: The second span.
  - **Returns**: `true` if the spans are equal, `false` otherwise.
  - **Use case**: Used for equality comparison between two spans.

---

### `bool span_is_less(const Span* span1, const Span* span2)`
  - **Purpose**: Checks if the first span is less than the second span (lexicographically).
  - **Parameters**:
    - `span1`: The first span.
    - `span2`: The second span.
  - **Returns**: `true` if `span1` is less than `span2`, `false` otherwise.
  - **Use case**: Used for comparison or sorting purposes.

---

### `bool span_is_greater(const Span* span1, const Span* span2)`
    - **Purpose**: Checks if the first span is greater than the second span (lexicographically).
    - **Parameters**:
      - `span1`: The first span.
      - `span2`: The second span.
    - **Returns**: `true` if `span1` is greater than `span2`, `false` otherwise.
    - **Use case**: Used for comparison or sorting purposes.

---

### `bool span_is_not_equal(const Span* span1, const Span* span2)`
  - **Purpose**: Checks if two spans are not equal.
  - **Parameters**:
    - `span1`: The first span.
    - `span2`: The second span.
  - **Returns**: `true` if the spans are not equal, `false` otherwise.
  - **Use case**: Used for inequality comparison.

---

### `bool span_is_greater_or_equal(const Span* span1, const Span* span2)`
  - **Purpose**: Checks if the first span is greater than or equal to the second span.
  - **Parameters**:
    - `span1`: The first span.
    - `span2`: The second span.
  - **Returns**: `true` if `span1` is greater than or equal to `span2`, `false` otherwise.
  - **Use case**: Useful in ordered operations on spans.

---

### `bool span_is_less_or_equal(const Span* span1, const Span* span2)`
  - **Purpose**: Checks if the first span is less than or equal to the second span.
  - **Parameters**:
    - `span1`: The first span.
    - `span2`: The second span.
  - **Returns**: `true` if `span1` is less than or equal to `span2`, `false` otherwise.
  - **Use case**: Useful in ordered operations on spans.

---

### `bool span_empty(const Span* span)`
  - **Purpose**: Checks whether the span is empty (i.e., has no elements).
  - **Parameters**:
    - `span`: The span to check.
  - **Returns**: `true` if the span is empty, `false` otherwise.
  - **Use case**: Used to check if a span contains any data.

---

### `size_t span_size(const Span* span)`
  - **Purpose**: Returns the number of elements in the span.
  - **Parameters**:
    - `span`: The span whose size is being queried.
  - **Returns**: The number of elements in the span.
  - **Use case**: Useful for determining the length of the span.

---

### `size_t span_size_bits(const Span* span)`
    - **Purpose**: Returns the total size of the span in bits.
    - **Parameters**:
      - `span`: The span whose size is being queried.
    - **Returns**: The size of the span in bits.
    - **Use case**: Used when working with bit-level data operations.

---

### `size_t span_size_bytes(const Span* span)`
    - **Purpose**: Returns the total size of the span in bytes.
    - **Parameters**:
      - `span`: The span whose size is being queried.
    - **Returns**: The size of the span in bytes.
    - **Use case**: Useful for memory or buffer-related operations where byte size is important.

---

### Examples 

### Example 1: Using `span_create` with Data Copy

This example demonstrates how to create a span from an array of integers and access its elements.

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {1, 2, 3, 4, 5};
    size_t arraySize = sizeof(array) / sizeof(array[0]);

    // Create a span and copy the data from the array
    Span* mySpan = span_create(array, arraySize, sizeof(int));

    // Access elements in the span
    int* spanData = (int*)mySpan->data;
    for (size_t i = 0; i < arraySize; ++i) { 
        fmt_printf("%d ", spanData[i]);
    }
    fmt_printf("\n");

    // Clean up
    span_destroy(mySpan);
    return 0;
}
```
**Result:**
```
1 2 3 4 5 
```

---

### Example 2 : Vector in Span also use `span_size` and `span_at`

in this example a Vector of integers is created and populated 
```c
#include "vector/vector.h"
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    Vector* intVector = vector_create(sizeof(int));
    int array[] = {15, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        vector_push_back(intVector, &array[i]);
    }
    // Create a Span from the Vector's data
    int* data = (int*) vector_data(intVector);
    Span* span = span_create(data, vector_size(intVector), sizeof(int));

    fmt_printf("Elements in Span:\n");
    for (size_t i = 0; i < span_size(span) / sizeof(int); ++i) {
        int* value = (int*) span_at(span, i);
        fmt_printf("%d ", *value);
    }
    fmt_printf("\n");

    span_destroy(span);
    vector_deallocate(intVector);

    return 0;
}
```
**Result:**
```
Elements in Span:
15 20 30 40 50
```

---

## Example 3 : use `char**` in Span 

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    const char *array[] = {"Vector", "String", "Map", "Csv", "Array"};
    size_t arraySize = sizeof(array) / sizeof(array[0]);

    // Create a span and copy the data from the array
    Span* mySpan = span_create(array, arraySize, sizeof(char*));
    char** spanData = (char**)mySpan->data;

    for (size_t i = 0; i < arraySize; ++i) { 
        fmt_printf("%s ", spanData[i]);
    }
    fmt_printf("\n");

    span_destroy(mySpan);
    return 0;
}
```
**Result:**
```
Vector String Map Csv Array
```

---

### Example 4: Using `span_front` and `span_back` with an Array of Integers

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span* span = span_create(array, arraySize, sizeof(int));

    // Access the first and last elements
    int* firstElement = (int*)span_front(span);
    int* lastElement = (int*)span_back(span);

    if (firstElement != NULL && lastElement != NULL) {
        fmt_printf("First element: %d\n", *firstElement);
        fmt_printf("Last element: %d\n", *lastElement);
    }

    span_destroy(span);
    return 0;
}
```
**Result:**
```
First element: 10
Last element: 50
```

---

### Example 5: Using `span_front` and `span_back` with an Array of Strings

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    const char* strings[] = {"Hello", "World", "Example", "C Programming"};
    size_t arraySize = sizeof(strings) / sizeof(strings[0]);
    Span* span = span_create(strings, arraySize, sizeof(char*));

    char** firstString = (char**)span_front(span);
    char** lastString = (char**)span_back(span);

    if (firstString != NULL && lastString != NULL) {
        fmt_printf("First string: %s\n", *firstString);
        fmt_printf("Last string: %s\n", *lastString);
    }

    span_destroy(span);
    return 0;
}
```
**Result:**
```
First string: Hello
Last string: C Programming
```

---

## Example 6 : Using `span_data`` and `span_cdata`

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {1, 2, 3, 4, 5};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span* span = span_create(array, arraySize, sizeof(int));

    int* data = (int*)span_data(span);
    fmt_printf("Original data: ");
    for (size_t i = 0; i < arraySize; ++i) { 
        fmt_printf("%d ", data[i]);
    }
    fmt_printf("\n");
    data[0] = 10; // Modifying the first element

    const int* constData = (const int*)span_cdata(span);
    fmt_printf("Modified data: ");
    for (size_t i = 0; i < arraySize; ++i) { 
        fmt_printf("%d ", constData[i]);
    }
    fmt_printf("\n");

    span_destroy(span);
    return 0;
}
```
**Result:**
```
Original data: 1 2 3 4 5 
Modified data: 10 2 3 4 5
```

---

## Example 7 : Using `span_empty`

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    Span* span = span_create(NULL, 0, 0); // Creating an empty span

    if (span_empty(span)) {
        fmt_printf("Span is empty\n");
    }
    else { 
        fmt_printf("Span is not empty\n");
    }

    span_destroy(span);
    return 0;
}
```
**Result:**
```
Error: Null data provided to span_create
```

---

## Example 8 : Using `span_size_bytes`

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {1, 2, 3, 4, 5};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span* span = span_create(array, arraySize, sizeof(int));

    fmt_printf("Size of span in bytes: %zu\n", span_size_bits(span));

    span_destroy(span);
    return 0;
}
```
**Result:**
```
Size of span in bytes: 80
```

---

## Example 9 : Using `span_first`

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span *span = span_create(array, arraySize, sizeof(int));

    Span firstThree = span_first(span, 3);
    fmt_printf("First three elements: ");

    for (size_t i = 0; i < firstThree.size / sizeof(int); ++i) { 
        fmt_printf("%d ", ((int*)firstThree.data)[i]);
    }
    fmt_printf("\n");
    
    span_destroy(span);
    return 0;
}
```
**Result:**
```
First three elements: 10 20 30
```

---

## Example 10 : Using `span_last`

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    Span *span = span_create(array, 5, sizeof(int));

    Span lastTwo = span_last(span, 2);
    fmt_printf("Last two elements: ");
    for (size_t i = 0; i < lastTwo.size / sizeof(int); ++i) { 
        fmt_printf("%d ", ((int*)lastTwo.data)[i]);
    }
    fmt_printf("\n");
    
    span_destroy(span);
    return 0;
}
```
**Result:**
```
Last two elements: 40 50 
```

---

## Example 11 : Using `span_subspan`

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    Span *span = span_create(array, 5, sizeof(int));

    Span middleSpan = span_subspan(span, 1, 3);
    fmt_printf("Middle three elements: ");
    for (size_t i = 0; i < middleSpan.size / sizeof(int); ++i) { 
        fmt_printf("%d ", ((int*)middleSpan.data)[i]);
    }
    fmt_printf("\n");
    
    span_destroy(span);
    return 0;
}
```
**Result:**
```
Middle three elements: 20 30 40 
```

---

## Example 12 : Using relation operators in Span 

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array1[] = {1, 2, 3, 4, 5};
    int array2[] = {1, 2, 3, 4, 6};

    Span* span1 = span_create(array1, 5, sizeof(int));
    Span* span2 = span_create(array2, 5, sizeof(int));

    fmt_printf("Span1 is equal to Span2: %s\n", span_is_equal(span1, span2) ? "true" : "false");
    fmt_printf("Span1 is less than Span2: %s\n", span_is_less(span1, span2) ? "true" : "false");
    fmt_printf("Span1 is greater than Span2: %s\n", span_is_greater(span1, span2) ? "true" : "false");
    fmt_printf("Span1 is not equal to Span2: %s\n", span_is_not_equal(span1, span2) ? "true" : "false");
    fmt_printf("Span1 is greater or equal to Span2: %s\n", span_is_greater_or_equal(span1, span2) ? "true" : "false");
    fmt_printf("Span1 is less or equal to Span2: %s\n", span_is_less_or_equal(span1, span2) ? "true" : "false");

    span_destroy(span1);
    span_destroy(span2);

    return 0;
}
```
**Result:**
```
Span1 is equal to Span2: false
Span1 is less than Span2: true
Span1 is greater than Span2: false
Span1 is not equal to Span2: true
Span1 is greater or equal to Span2: false
Span1 is less or equal to Span2: true
Error: Span memory pool is NULL in destroy_span_memory_pool
```

---

## Example 13 : Using Iterations functoins like `span_begin`, `span_end` 

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    Span* span = span_create(array, 5, sizeof(int));

    for (int* ptr = (int*)span_begin(span); ptr != span_end(span); ptr = (int*)span_increment(span, ptr)) {
        if (ptr) {
            fmt_printf("%d ", *ptr);
        }
    }
    fmt_printf("\n");

    for (int* ptr = (int*)span_rbegin(span); ptr != span_rend(span); ptr = (int*)span_decrement(span, ptr)) {
        if (ptr) {
            fmt_printf("%d ", *ptr);
        }
    }
    fmt_printf("\n");

    span_destroy(span);
    return 0;
}
```
**Result:**
```
10 20 30 40 50 
50 40 30 20 10
```