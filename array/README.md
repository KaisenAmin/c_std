
# Array Library

The Array library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::array` in C++.

## Compilation

To compile the Array library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -o main ./main.c ./array/array.c 
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Array library in your project, include the `array.h` header file in your source code.

```c
#include "array/array.h"
```

## Example 1 : how to create Array object and 'fill' array with value then get them with 'at' method

```c
Array* intArray = array_create(sizeof(int), 5);

if (intArray == NULL) 
{
    printf("Failed to create array.\n");
    return 1;
}

int valueToFill = 42;
intArray->fill(intArray, &valueToFill);

for (int i = 0; i < intArray->size(intArray); i++)
    printf("Number is %d\n", *(int*)intArray->at(intArray, i));

printf("-----------\n");

for (size_t i = 0; i < intArray->size(intArray); ++i) 
{
    int* element = (int*)intArray->at(intArray, i);

    if (element != NULL) 
        printf("Element %zu: %d\n", i, *element);
}

intArray->deallocate(intArray);

```

## Example 2 : check array is 'empty' or not 

```c
Array* intArray = array_create(sizeof(int), 5);

if (intArray == NULL) 
{
    printf("Failed to create array.\n");
    return 1;
}

int valueToFill = 42;
intArray->fill(intArray, &valueToFill);

// Check if the array is empty
if (intArray->empty(intArray)) 
    printf("Array is empty.\n");
else 
    printf("Array is not empty.\n");
    
intArray->deallocate(intArray);

```

## Example 3 : 'front', 'back' return ref to first and end of the Array

```c

Array* intArray = array_create(sizeof(int), 5);

if (intArray == NULL) 
{
    printf("Failed to create array.\n");
    return 1;
}

int valueToFill = 42;
intArray->fill(intArray, &valueToFill);

*(int*)intArray->front(intArray) = 1563;
// Access the front and back elements

int* frontElement = (int*)intArray->front(intArray);
int* backElement = (int*)intArray->back(intArray);

if (frontElement != NULL && backElement != NULL) 
    printf("Front element: %d, Back element: %d\n", *frontElement, *backElement);

intArray->deallocate(intArray);

```

## Example 4 : how to set data in position with 'set' 

```c

Array* intArray = array_create(sizeof(int), 5);

if (intArray == NULL) 
{
    printf("Failed to create array.\n");
    return 1;
}

int valueToFill = 42;
intArray->fill(intArray, &valueToFill);

for (size_t i = 0; i < intArray->size(intArray); ++i) 
{
    int value = i * 2; 
    intArray->set(intArray, i, &value);
    int* element = (int*)intArray->at(intArray, i);

    if (element != NULL) 
        printf("Element %zu: %d\n", i, *element);
        
}
// Deallocate the array
intArray->deallocate(intArray);


```