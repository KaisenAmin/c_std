# Span Library in C

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
#include <stdio.h>

int main() 
{
    int array[] = {1, 2, 3, 4, 5};
    size_t arraySize = sizeof(array) / sizeof(array[0]);

    // Create a span and copy the data from the array
    Span* mySpan = span_create(array, arraySize, sizeof(int));

    // Access elements in the span
    int* spanData = (int*)mySpan->data;
    for (size_t i = 0; i < arraySize; ++i) 
        printf("%d ", spanData[i]);
    printf("\n");

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
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Vector* intVector = vector_create(sizeof(int));
    int array[] = {15, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) 
        vector_push_back(intVector, &array[i]);

    // Create a Span from the Vector's data
    int* data = (int*) vector_data(intVector);
    Span* span = span_create(data, vector_size(intVector), sizeof(int));

    printf("Elements in Span:\n");
    for (size_t i = 0; i < span_size(span) / sizeof(int); ++i) 
    {
        int* value = (int*) span_at(span, i);
        printf("%d ", *value);
    }
    printf("\n");

    span_destroy(span);
    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}
```

## Example 3 : use `char**` in Span 

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    char *array[] = {"Vector", "String", "Map", "Csv", "Array"};
    size_t arraySize = sizeof(array) / sizeof(array[0]);

    // Create a span and copy the data from the array
    Span* mySpan = span_create(array, arraySize, sizeof(char*));

    // Access elements in the span
    char** spanData = (char**)mySpan->data;
    for (size_t i = 0; i < arraySize; ++i) 
        printf("%s ", spanData[i]);
    printf("\n");

    // Clean up
    span_destroy(mySpan);

    return 0;
}
```

### Example 4: Using `span_front` and `span_back` with an Array of Integers

```c
#include "span.h"
#include <stdio.h>

int main() 
{
    int array[] = {10, 20, 30, 40, 50};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span* span = span_create(array, arraySize, sizeof(int));

    // Access the first and last elements
    int* firstElement = (int*)span_front(span);
    int* lastElement = (int*)span_back(span);

    if (firstElement != NULL && lastElement != NULL) 
    {
        printf("First element: %d\n", *firstElement);
        printf("Last element: %d\n", *lastElement);
    }

    span_destroy(span);
    return 0;
}
```

### Example 5: Using `span_front` and `span_back` with an Array of Strings

```c
#include "span.h"
#include <stdio.h>

int main() {
    char* strings[] = {"Hello", "World", "Example", "C Programming"};
    size_t arraySize = sizeof(strings) / sizeof(strings[0]);
    Span* span = span_create(strings, arraySize, sizeof(char*));

    char** firstString = (char**)span_front(span);
    char** lastString = (char**)span_back(span);

    if (firstString != NULL && lastString != NULL) 
    {
        printf("First string: %s\n", *firstString);
        printf("Last string: %s\n", *lastString);
    }

    span_destroy(span);
    return 0;
}
```

## Example 6 : Using `span_data`` and `span_cdata`

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    int array[] = {1, 2, 3, 4, 5};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span* span = span_create(array, arraySize, sizeof(int));

    int* data = (int*)span_data(span);
    printf("Original data: ");
    for (size_t i = 0; i < arraySize; ++i) 
        printf("%d ", data[i]);
    
    printf("\n");
    data[0] = 10; // Modifying the first element

    const int* constData = (const int*)span_cdata(span);
    printf("Modified data: ");
    for (size_t i = 0; i < arraySize; ++i) 
        printf("%d ", constData[i]);

    printf("\n");

    span_destroy(span);
    return 0;
}

```

## Example 7 : Using `span_empty`

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    Span* span = span_create(NULL, 0, 0); // Creating an empty span

    if (span_empty(span)) 
        printf("Span is empty\n");
    else 
        printf("Span is not empty\n");
    
    span_destroy(span);
    return 0;
}

```

## Example 8 : Using `span_size_bytes`

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    int array[] = {1, 2, 3, 4, 5};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span* span = span_create(array, arraySize, sizeof(int));

    printf("Size of span in bytes: %zu\n", span_size_bytes(span));

    span_destroy(span);
    return 0;
}

```

## Example 9 : Using `span_first`

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    int array[] = {10, 20, 30, 40, 50};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span *span = span_create(array, arraySize, sizeof(int));

    Span firstThree = span_first(span, 3);
    printf("First three elements: ");
    for (size_t i = 0; i < firstThree.size / sizeof(int); ++i) 
        printf("%d ", ((int*)firstThree.data)[i]);

    printf("\n");
    span_destroy(span);
    
    return 0;
}

```

## Example 10 : Using `span_last`

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    int array[] = {10, 20, 30, 40, 50};
    Span *span = span_create(array, 5, sizeof(int));

    Span lastTwo = span_last(span, 2);
    printf("Last two elements: ");
    for (size_t i = 0; i < lastTwo.size / sizeof(int); ++i) 
        printf("%d ", ((int*)lastTwo.data)[i]);
    
    printf("\n");
    span_destroy(span);

    return 0;
}

```

## Example 11 : Using `span_subspan`

```c
#include "span/span.h"
#include <stdio.h>

int main() 
{
    int array[] = {10, 20, 30, 40, 50};
    Span *span = span_create(array, 5, sizeof(int));

    Span middleSpan = span_subspan(span, 1, 3);
    printf("Middle three elements: ");
    for (size_t i = 0; i < middleSpan.size / sizeof(int); ++i) 
        printf("%d ", ((int*)middleSpan.data)[i]);
    
    printf("\n");
    span_destroy(span);
    
    return 0;
}
```