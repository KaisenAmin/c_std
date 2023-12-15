Based on the examples you've provided, I'll draft a README for the Vector library. This README will include an introduction, compilation instructions, and usage examples for different data types including integers, strings, and user-defined structures.

# Vector Library

## Introduction

The Vector library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::vector` in C++.

## Compilation

To compile the Vector library along with your main program, use the following GCC command:

```bash
gcc -std=c11 -O3 -o main ./main.c ./vector/vector.c ./string/string.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Vector library in your project, include the `vector.h` header file in your source code.

```c
#include "vector/vector.h"
```

### Example 1: Integers

```c
Vector* intVector = vector_create(sizeof(int));
int value = 10;
intVector->push_back(intVector, &value);

// Iterate over the vector
for (size_t i = 0; i < intVector->length(intVector); ++i) 
{
    int* item = (int*)intVector->at(intVector, i);
    printf("%d\n", *item);
}

// Cleanup
intVector->deallocate(intVector);

```

### Example 2: Strings

```c
Vector* stringVector = vector_create(sizeof(char*));
char *str = "Hello, World!";

stringVector->push_back(stringVector, &str);

// Iterate over the vector
for (size_t i = 0; i < stringVector->length(stringVector); ++i) 
{
    char** item = (char**)stringVector->at(stringVector, i);
    printf("%s\n", *item);
}

// Cleanup
stringVector->deallocate(stringVector);

```

### Example 3: User-Defined Structures

```c
typedef struct 
{
    int id;
    double value;

} MyStruct;

Vector* structVector = vector_create(sizeof(MyStruct));
MyStruct s = {1, 10.5};

structVector->push_back(structVector, &s);

// Iterate over the vector
for (size_t i = 0; i < structVector->length(structVector); ++i) 
{
    MyStruct* item = (MyStruct*)structVector->at(structVector, i);
    printf("ID: %d, Value: %.2f\n", item->id, item->value);
}

// Cleanup
structVector->deallocate(structVector);
```

### Example 4: Use relational operators

```c

Vector* vec1 = vector_create(sizeof(int));
Vector* vec2 = vector_create(sizeof(int));

int items1[] = {1, 2, 3};
for (int i = 0; i < 3; i++) 
    vec1->push_back(vec1, &items1[i]);

int items2[] = {1, 2, 4};
for (int i = 0; i < 3; i++) 
    vec2->push_back(vec2, &items2[i]);

if (vec1->is_equal(vec1, vec2)) 
    printf("vec1 is equal to vec2\n");
else 
    printf("vec1 is not equal to vec2\n");

if (vec1->is_less(vec1, vec2)) 
    printf("vec1 is less than vec2\n");
if (vec1->is_greater(vec2, vec1)) 
    printf("vec2 is greater than vec2\n");

// Cleanup
vec1->deallocate(vec1);
vec2->deallocate(vec2);

```

## Example 5: Use at and char*

```c

Vector *intVector = vector_create(sizeof(int));

int value1 = 10, value2 = 20, value3 = 30;
intVector->push_back(intVector, &value1);
intVector->push_back(intVector, &value2);
intVector->push_back(intVector, &value3);

for (size_t i = 0; i < (intVector->length(intVector)); ++i) 
{
    int* item = (int*) intVector->at(intVector, i);
    printf("%d\n", *item);
}

Vector *stringVector = vector_create(sizeof(char*));

char *str1 = "Hello";
char *str2 = "World";
char *str3 = "Example";

stringVector->push_back(stringVector, &str1);
stringVector->push_back(stringVector, &str2);
stringVector->push_back(stringVector, &str3);

for (size_t i = 0; i < stringVector->length(stringVector); ++i) 
{
    char **item = (char**) stringVector->at(stringVector, i);
    printf("%s\n", *item);
}

intVector->clear(intVector);
intVector->deallocate(intVector);
stringVector->clear(stringVector);
stringVector->deallocate(stringVector);


```
## Example 6: how to use 'swap'

```c

Vector *vector1 = vector_create(sizeof(int));
Vector *vector2 = vector_create(sizeof(int));

int value1 = 10, value2 = 20;
vector1->push_back(vector1, &value1);
vector1->push_back(vector1, &value2);

int value3 = 30, value4 = 40;
vector2->push_back(vector2, &value3);
vector2->push_back(vector2, &value4);

vector1->swap(vector1, vector2);

printf("Contents of vector1 after swap:\n");
for (size_t i = 0; i < vector1->length(vector1); ++i) 
{
    int* item = (int*) vector1->at(vector1, i);
    printf("%d\n", *item);
}

printf("Contents of vector2 after swap:\n");
for (size_t i = 0; i < vector2->length(vector2); ++i) 
{
    int* item = (int*) vector2->at(vector2, i);
    printf("%d\n", *item);
}

vector1->deallocate(vector1);
vector2->deallocate(vector2);

```

## Example 6: how to use 'pop_back' 

```c 
Vector *intVector = vector_create(sizeof(int));

// Adding elements to the vector
int values[] = {10, 20, 30, 40};
for (int i = 0; i < 4; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Pop the last element and access it
int *poppedItem = (int *)intVector->pop_back(intVector);
if (poppedItem) 
    printf("Popped item: %d\n", *poppedItem);
    
intVector->deallocate(intVector)

```

## Example 7: how to use 'front' 

```c 

Vector *intVector = vector_create(sizeof(int));

int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
int *frontItem = (int *)intVector->front(intVector);
if (frontItem) 
    printf("First item: %d\n", *frontItem);
    
intVector->deallocate(intVector);

```

## Example 8: how to use 'back'

```c
Vector *intVector = vector_create(sizeof(int));

int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
int *backItem = (int *)intVector->back(intVector);
if (backItem) 
    printf("Last item: %d\n", *backItem);
    
intVector->deallocate(intVector);

```

## Example 9: use 'data' "Returns pointer to the underlying array serving as element storage"
```c
Vector *intVector = vector_create(sizeof(int));

// Adding elements to the vector
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Accessing the underlying array
int *data = (int *)intVector->data(intVector);
if (data)
{
    for (size_t i = 0; i < intVector->length(intVector); ++i) 
        printf("Item %zu: %d\n", i, data[i]);
}
    
intVector->deallocate(intVector);

```

## Example 10: 'begin', 'end' "Return pointer to the first and last element"

```c
Vector *intVector = vector_create(sizeof(int));

// Adding elements
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Using begin and end to iterate over the vector
for (int *it = (int *)intVector->begin(intVector); it != (int *)intVector->end(intVector); ++it) 
    printf("%d\n", *it);
    
intVector->deallocate(intVector);

```

## Example 11: 'cbegin', 'cend' "Return const pointer to first and last element"

```c 
Vector *intVector = vector_create(sizeof(int));

// Adding elements
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Using cbegin and cend for read-only iteration over the vector
for (const int *it = (const int *)intVector->cbegin(intVector); it != (const int *)intVector->cend(intVector); ++it)
    printf("%d\n", *it);
    
intVector->deallocate(intVector);

```

## Example 12 : 'crbegin', 'crend' "Return reverse read-only iteration over the vector"
```c
Vector *intVector = vector_create(sizeof(int));

// Adding elements
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Using crbegin and crend for reverse read-only iteration over the vector
for (const int *it = (const int *)intVector->crbegin(intVector); it != (const int *)intVector->crend(intVector); --it) 
    printf("%d\n", *it);
    
intVector->deallocate(intVector);

```

## Example 13: Using 'rbegin' and 'rend' for reverse iteration over the vector

```c
Vector *intVector = vector_create(sizeof(int));

// Adding elements
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Using rbegin and rend for reverse iteration over the vector
for (int *it = (int *)intVector->rbegin(intVector); it != (int *)intVector->rend(intVector); --it) 
    printf("%d\n", *it);
    
intVector->deallocate(intVector);

```

## Example 14: 'insert' New element to vector 

```c 
Vector *intVector = vector_create(sizeof(int));

int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Insert a new element
int newElement = 25;
intVector->insert(intVector, 0, &newElement); // Inserts 25 at position 0

// Print elements after insertion
for (size_t i = 0; i < intVector->length(intVector); ++i) 
{
    int* item = (int*) intVector->at(intVector, i);
    printf("%d\n", *item);
}

intVector->deallocate(intVector);

```

## Example 15: 'erase' element from vector

```c

Vector *intVector = vector_create(sizeof(int));

// Adding elements to the vector
int values[] = {10, 20, 30, 40, 50};
for (int i = 0; i < 5; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Erase elements from position 1, removing 2 elements
intVector->erase(intVector, 1, 2); // Should remove 20 and 30

// Print elements after erasure
for (size_t i = 0; i < intVector->length(intVector); ++i) 
{
    int* item = (int*) intVector->at(intVector, i);
    printf("%d\n", *item);
}

intVector->deallocate(intVector);

```

## Example 16: 'reserve'

```c
Vector *intVector = vector_create(sizeof(int));

// Reserve capacity for at least 10 elements
intVector->reserve(intVector, 10);

// Add elements and observe no need for reallocation until the 11th element
for (int i = 0; i < 11; ++i) 
{
    int value = i * 10;
    intVector->push_back(intVector, &value);
}

printf("Vector size: %zu, Vector capacity: %zu\n", intVector->length(intVector), intVector->capacity(intVector));

intVector->deallocate(intVector);

```

## Example 17: 'resize' 

```c
Vector *intVector = vector_create(sizeof(int));

// Adding elements to the vector
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
// Resize to a larger size (5)
intVector->resize(intVector, 5);

printf("After resizing to larger size:\n");
for (size_t i = 0; i < intVector->length(intVector); ++i) 
{
    int* item = (int*) intVector->at(intVector, i);
    printf("%d\n", *item); // The last two elements will be zero-initialized
}

// Resize to a smaller size (2)
intVector->resize(intVector, 2);

printf("After resizing to smaller size:\n");
for (size_t i = 0; i < intVector->length(intVector); ++i) 
{
    int* item = (int*) intVector->at(intVector, i);
    printf("%d\n", *item); // Only the first two elements remain
}

intVector->clear(intVector);
intVector->deallocate(intVector);

```
## Contribution

Your contributions to this project are welcome! If you have suggestions or improvements, feel free to fork the repository, make your changes, and submit a pull request.

