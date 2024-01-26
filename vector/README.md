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

## Examples

Several examples are provided to demonstrate the usage of the Vector library in various scenarios, including handling primitive data types, strings, and custom structures. These examples also show how to perform operations like iteration, sorting, and comparison on vectors.

## License

This library is distributed under the GNU General Public License v3.0, which allows for free use, modification, and distribution under the same license.


### Function Descriptions

- `vector_create(size_t itemSize)`: Initializes and returns a new vector with specified item size.
- `vector_is_equal(const Vector* vec1, const Vector* vec2)`: Checks if two vectors are equal in content.
- `vector_is_less(const Vector* vec1, const Vector* vec2)`: Checks if `vec1` is lexicographically less than `vec2`.
- `vector_is_greater(const Vector* vec1, const Vector* vec2)`: Checks if `vec1` is lexicographically greater than `vec2`.

- `vector_is_not_equal(const Vector* vec1, const Vector* vec2)`: Checks if two vectors are not equal.
- `vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2)`: Checks if `vec1` is lexicographically greater than or equal to `vec2`.

- `vector_is_less_or_equal(const Vector* vec1, const Vector* vec2)`: Checks if `vec1` is lexicographically less than or equal to `vec2`.

- `vector_is_empty(Vector* vec)`: Determines if the vector is empty.
- `vector_erase(Vector* vec, size_t pos, size_t len)`: Erases a range of elements from the vector.
- `vector_insert(Vector* vec, size_t pos, void* item)`: Inserts an item into the vector at a specified position.
- `vector_reserve(Vector* vec, size_t size)`: Reserves memory to enhance vector capacity.
- `vector_resize(Vector* vec, size_t size)`: Resizes the vector to contain a specific number of elements.
- `vector_shrink_to_fit(Vector* vec)`: Reduces the capacity of the vector to fit its size.
- `vector_clear(Vector* vec)`: Clears the contents of the vector.
- `vector_swap(Vector* vec1, Vector* vec2)`: Swaps the contents of two vectors.
- `vector_assign(Vector* vec, size_t pos, void* item)`: Assigns a new value to an element at a specified position.
- `vector_emplace(Vector* vec, size_t pos, void* item, size_t itemSize)`: Constructs an element in-place at a specified position.
- `vector_emplace_back(Vector* vec, void* item, size_t itemSize)`: Constructs an element in-place at the end of the vector.

- `vector_push_back(Vector* vec, void* item)`: Adds an element to the end of the vector.
- `vector_deallocate(Vector* vec)`: Frees the memory occupied by the vector.
- `vector_at(Vector* vec, size_t pos)`: Accesses an element at a specific position.
- `vector_rbegin(Vector* vec)`: Returns a pointer to the beginning of the reversed vector.
- `vector_rend(Vector* vec)`: Returns a pointer to the end of the reversed vector.
- `vector_cbegin(Vector* vec)`: Returns a constant pointer to the beginning of the vector.
- `vector_cend(Vector* vec)`: Returns a constant pointer to the end of the vector.
- `vector_crbegin(Vector* vec)`: Returns a constant pointer to the beginning of the reversed vector.
- `vector_crend(Vector* vec)`: Returns a constant pointer to the end of the reversed vector.
- `vector_begin(Vector* vec)`: Returns a pointer to the first element.
- `vector_end(Vector* vec)`: Returns a pointer to the element following the last element.
- `vector_pop_back(Vector* vec)`: Removes the last element and returns a pointer to it.
- `vector_front(Vector* vec)`: Accesses the first element.
- `vector_back(Vector* vec)`: Accesses the last element.
- `vector_data(Vector* vec)`: Returns a pointer to the underlying array.
- `vector_size(Vector* vec)`: Returns the number of elements.
- `vector_capacity(Vector* vec)`: Returns the capacity of the vector.
- `vector_max_size(Vector* vec)`: Returns the maximum number of elements the vector can hold.



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

    // Cleanup
    vector_deallocate(intVector);
    return 0;
}
```

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
        printf("vec1 is equal to vec2\n");
    }
    else {
        printf("vec1 is not equal to vec2\n");
    }

    if (vector_is_less(vec1, vec2)) { 
        printf("vec1 is less than vec2\n");
    }
    if (vector_is_greater(vec2, vec1)) {
        printf("vec2 is greater than vec2\n");
    }
    // Cleanup
    vector_deallocate(vec1);
    vector_deallocate(vec2);
        
    return 0;
}
```

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
    char *str1 = "Hello";
    char *str2 = "World";
    char *str3 = "Example";

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

    fmt_println("Contents of vector1 after swap:", FMT_END_ARGS);
    for (size_t i = 0; i < vector_size(vector1); ++i) {
        int* item = (int*) vector_at(vector1, i);
        fmt_printf("%d\n", *item);
    }

    fmt_println("Contents of vector2 after swap:", FMT_END_ARGS);
    for (size_t i = 0; i < vector_size(vector2); ++i) {
        int* item = (int*)vector_at(vector2, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);   
    return 0;
}

```

## Example 7: how to use `vector_pop_back`

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() 
{   
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
    printf("After resizing to larger size:\n");

    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        printf("%d\n", *item); // The last two elements will be zero-initialized
    }

    // Resize to a smaller size (2)
    vector_resize(intVector, 2);

    printf("After resizing to smaller size:\n");
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        printf("%d\n", *item); // Only the first two elements remain
    }

    vector_clear(intVector);
    vector_deallocate(intVector);

    return 0;
}
```

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

    printf("Size before shrink_to_fit: %zu, Capacity before shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    // Shrink to fit the actual number of elements
    vector_shrink_to_fit(intVector);    
    printf("Size after shrink_to_fit: %zu, Capacity after shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);
    return 0;
}
```

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
        printf("%s\n", *item);
    }

    vector_deallocate(stringVector);
    return 0;
}
```

## Example 21 : how to use `String` object in Vector

```c
#include "string/string.h"
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

### Example 23 : how to use `vector_begin` and `vector_end` with String Object 

just you should now we can do this in lots of way by String ..
also don't use printf in loop because it is not optimal.

Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 0.000010 seconds

```c
#include "string/string.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <time.h>

int main() {
    struct timespec start, end;
    double time_elapsed;

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

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t index = 0; index < vector_size(vec); ++index) {
        String **strPtr = (String**)vector_at(vec, index);
        string_push_back(*strPtr, '\n');
        string_concatenate(concat, *strPtr);
    }
    
    for (size_t index = 0; index < 5; index++) {
        string_deallocate(fruits[index]);
    }

    vector_deallocate(vec);

    clock_gettime(CLOCK_MONOTONIC, &end);
    time_elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    fmt_printf("%s", string_c_str(concat));
    fmt_printf("Time taken: %f seconds\n", time_elapsed);

    string_deallocate(concat);
    return 0;
}
```

also result in Cpp 

Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 1.1523e-05 seconds

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

### Example 25 : Vector of Structs with Dynamic String Fields

This example demonstrates how to use the Vector library to store a collection of structs, where each struct contains dynamically allocated string fields (using the String library).
```c
#include "vector/vector.h"
#include "string/string.h"
#include "fmt/fmt.h"

typedef struct {
    String* name;
    String* description;
} Item;

int main() {
    Vector* items = vector_create(sizeof(Item));
    char* names[] = {"Item1", "Item2", "Item3"};
    char* descriptions[] = {"Description1", "Description2", "Description3"};

    for (int i = 0; i < 3; i++) {
        Item item;
        item.name = string_create(names[i]);
        item.description = string_create(descriptions[i]);

        vector_push_back(items, &item);
    }

    for (size_t i = 0; i < vector_size(items); i++) {
        Item* itemPtr = (Item*)vector_at(items, i);
        printf("Name: %s, Description: %s\n", 
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

### Example 26 : Vector of Vectors (2D Vector) of Custom Structs 

```c
#include "vector/vector.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

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
