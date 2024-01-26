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

### Function Descriptions

- `Span* span_create(void* data, size_t size)`: Creates a span pointing to the given data with the specified size. Uses the memory pool for allocation.
- `void span_destroy(Span* span)`: Destroys the span, freeing its resources.
- `void* span_begin(Span* span)`: Returns a pointer to the beginning of the span.
- `const void* span_cbegin(const Span* span)`: Returns a constant pointer to the beginning of the span.
- `void* span_end(Span* span)`: Returns a pointer to the end of the span.
- `const void* span_cend(const Span* span)`: Returns a constant pointer to the end of the span.
- `void* span_rbegin(Span* span)`: Returns a reverse iterator to the beginning of the span.
- `const void* span_crbegin(const Span* span)`: Returns a constant reverse iterator to the beginning of the span.
- `void* span_rend(Span* span)`: Returns a reverse iterator to the end of the span.
- `const void* span_crend(const Span* span)`: Returns a constant reverse iterator to the end of the span.
- `void* span_front(Span* span)`: Accesses the first element of the span.
- `void* span_back(Span* span)`: Accesses the last element of the span.
- `void* span_at(Span* span, size_t index)`: Accesses the element at the specified index with bounds checking.
- `void* span_operator_index(Span* span, size_t index)`: Accesses the element at the specified index without bounds checking.
- `void* span_data(Span* span)`: Provides direct access to the underlying data.
- `const void* span_cdata(const Span* span)`: Provides constant access to the underlying data.
- `size_t span_size(const Span* span)`: Returns the size of the span.
- `bool span_empty(const Span* span)`: Checks if the span is empty.
- `size_t span_size_bytes(const Span* span)`: Returns the size of the span in bytes.
- `Span span_first(Span* span, size_t count)`: Obtains a subspan consisting of the first `count` elements.
- `Span span_last(Span* span, size_t count)`: Obtains a subspan consisting of the last `count` elements.
- `Span span_subspan(Span* span, size_t offset, size_t count)`: Obtains a subspan starting at the specified offset.

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

## Example 3 : use `char**` in Span 

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    char *array[] = {"Vector", "String", "Map", "Csv", "Array"};
    size_t arraySize = sizeof(array) / sizeof(array[0]);

    // Create a span and copy the data from the array
    Span* mySpan = span_create(array, arraySize, sizeof(char*));
    char** spanData = (char**)mySpan->data;

    for (size_t i = 0; i < arraySize; ++i) { 
        fmt_printf("%s ", spanData[i]);
    }
    fmt_printf("\n");

    // Clean up
    span_destroy(mySpan);
    return 0;
}
```

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

### Example 5: Using `span_front` and `span_back` with an Array of Strings

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    char* strings[] = {"Hello", "World", "Example", "C Programming"};
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

## Example 13 : Using Iterations functoins like `span_begin`, `span_end` 

```c
#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    Span* span = span_create(array, 5, sizeof(int));

    for (int* ptr = span_begin(span); ptr != span_end(span); ptr = span_increment(span, ptr)) {
        if (ptr) {
            fmt_printf("%d ", *ptr);
        }
    }
    fmt_printf("\n");

    for (int* ptr = span_rbegin(span); ptr != span_rend(span); ptr = (int*)span_decrement(span, ptr)) {
        if (ptr) {
            fmt_printf("%d ", *ptr);
        }
    }
    fmt_printf("\n");

    span_destroy(span);
    return 0;
}

```