
# Vector Library

The Vector library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::vector` in C++.

## Compilation

To compile the Vector library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./vector/vector.c ./string/string.c
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

## Example 9: use 'data' Returns pointer to the underlying array serving as element storage
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

## Example 10: 'begin', 'end' Return pointer to the first and last element

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

## Example 11: 'cbegin', 'cend' Return const pointer to first and last element

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

## Example 12 : 'crbegin', 'crend' Return reverse read-only iteration over the vector
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

## Example 18 : 'shrink_to_fit' fixed capacity

```c

Vector *intVector = vector_create(sizeof(int));

// Reserve larger capacity
intVector->reserve(intVector, 10);

// Adding a few elements
int values[] = {10, 20, 30};
for (int i = 0; i < 3; ++i) 
    intVector->push_back(intVector, &values[i]);
    
printf("Size before shrink_to_fit: %zu, Capacity before shrink_to_fit: %zu\n", intVector->length(intVector), intVector->capacity(intVector));

// Shrink to fit the actual number of elements
intVector->shrink_to_fit(intVector);
    
printf("Size after shrink_to_fit: %zu, Capacity after shrink_to_fit: %zu\n", intVector->length(intVector), intVector->capacity(intVector));

intVector->deallocate(intVector);

```

## Example 19 : use 'emplace' and 'emplace_back' insert new element at positin and at the end of 

```c

Vector *stringVector = vector_create(sizeof(char*));
char* values[] = {"Hello", "World", "Vector", "Example"};

for (int i = 0; i < 4; ++i) 
    stringVector->push_back(stringVector, &values[i]);
    
char* newValue = "NewString";
stringVector->assign(stringVector, 1, &newValue);

char* emplaceValue = "EmplacedString";
stringVector->emplace(stringVector, 2, &emplaceValue, sizeof(char*));

char* emplaceBackValue = "EmplacedBackString";
stringVector->emplace_back(stringVector, &emplaceBackValue, sizeof(char*));

for (size_t i = 0; i < stringVector->length(stringVector); ++i) 
{
    char** item = (char **)stringVector->at(stringVector, i);
    printf("%s\n", *item);
}

stringVector->deallocate(stringVector);

```

## Example 20 : how to use 'String' object in Vector

```c

#include "vector/vector.h"
#include "string/string.h"


Vector* vec = vector_create(sizeof(String*));

String* myString1 = string_create("Hello");
String* myString2 = string_create("World");
String* myString3 = string_create("Example");

vec->push_back(vec, &myString1);
vec->push_back(vec, &myString2);
vec->push_back(vec, &myString3);

for (size_t i = 0; i < vec->length(vec); ++i) 
{
    String** strPtr = (String**) vec->at(vec, i);
    if (strPtr && *strPtr) 
        printf("%s\n", (*strPtr)->dataStr); 
}

for (size_t i = 0; i < vec->length(vec); ++i) 
{
    String** strPtr = (String**) vec->at(vec, i);

    if (strPtr && *strPtr) 
    {
        (*strPtr)->deallocate(*strPtr);
        free(*strPtr);
    }
}

vec->deallocate(vec);

myString1->deallocate(myString1);
myString2->deallocate(myString2);
myString3->deallocate(myString3);

```

## Example 21 : how to use relationals operators in Vector 

```c

Vector *vector1 = vector_create(sizeof(int));
Vector *vector2 = vector_create(sizeof(int));

int value1 = 10, value2 = 20;
vector1->push_back(vector1, &value1);
vector1->push_back(vector1, &value2);

int value3 = 30, value4 = 40;
vector2->push_back(vector2, &value3);
vector2->push_back(vector2, &value4);
   
if (vector1->is_equal(vector1, vector2))
    printf("Vector1 is equal with Vector2\n");

if (vector1->is_less(vector1, vector2))
    printf("Vector1 is less than Vector2\n");

if (vector1->is_greater(vector1, vector2))
    printf("Vector1 is greater than Vector2\n");

vector1->deallocate(vector1);
vector2->deallocate(vector2);

```

## Example 22 : bench 

gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./vector/vector.c
Average Custom Vector Time: 0.008564 seconds

```c 

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ELEMENTS 100000000

int main() 
{
    struct timespec start, end;
    double time_sum = 0;


    Vector* vec = vector_create(sizeof(int));
    if (!vec) 
    {
        perror("Vector creation failed");
        return EXIT_FAILURE;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
        
    for (int i = 0; i < NUM_ELEMENTS; i++) 
        vec->push_back(vec, &i);
        
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sum += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    double average_time = time_sum / NUM_ITERATIONS;
    printf("Average Custom Vector Time: %f seconds\n", average_time);

    vec->deallocate(vec);

    return EXIT_SUCCESS;
}


```

and in c++ 
g++ -std=c++14 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.cpp
std::vector Time: 0.344828 seconds

```c
#include <iostream>
#include <vector>
#include <chrono>

#define NUM_ELEMENTS 100000000

int main() 
{
    std::vector<int> vec;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ELEMENTS; i++) 
        vec.push_back(i);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_spent = end - start;

    std::cout << "std::vector Time: " << time_spent.count() << " seconds\n";

    return 0;
}


```
## Contribution

Your contributions to this project are welcome! If you have suggestions or improvements, feel free to fork the repository, make your changes, and submit a pull request.

