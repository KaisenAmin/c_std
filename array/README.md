
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


## Example 5: Iterating with 'begin' and end with 'double'


```c
Array* doubleArray = array_create(sizeof(double), 5);

if (doubleArray == NULL) 
{
    printf("Failed to create double array.\n");
    return 1;
}

double values[] = {1.5, 2.5, 3.5, 4.5, 5.5};
for (size_t i = 0; i < doubleArray->size(doubleArray); ++i) 
    doubleArray->set(doubleArray, i, &values[i]);
    
for (double* it = (double*)doubleArray->begin(doubleArray); it != (double*)doubleArray->end(doubleArray); ++it) 
    printf("Element: %f\n", *it);

doubleArray->deallocate(doubleArray);

```

## Example 6 : Using 'cbegin' and 'cend' with char*

```c

Array* stringArray = array_create(sizeof(char*), 3);

if (stringArray == NULL) 
{
    printf("Failed to create string array.\n");
    return 1;
}

char* strings[] = {"Hello", "World", "Array"};
for (size_t i = 0; i < stringArray->size(stringArray); ++i) 
    stringArray->set(stringArray, i, &strings[i]);
    
for (const char** it = (const char**)stringArray->cbegin(stringArray); it != (const char**)stringArray->cend(stringArray); ++it) 
    printf("String: %s\n", *it);
    
stringArray->deallocate(stringArray);

```

## Example 7 : Using Relational Operators

```c

Array* arr1 = array_create(sizeof(int), 3);
Array* arr2 = array_create(sizeof(int), 3);

int arr1Values[] = {1, 2, 3};
int arr2Values[] = {1, 2, 3};

for (size_t i = 0; i < 3; ++i) 
{
    arr1->set(arr1, i, &arr1Values[i]);
    arr2->set(arr2, i, &arr2Values[i]);
}

if (arr1->is_equal(arr1, arr2))
    printf("Arrays are equal.\n");
else
    printf("Arrays are not equal.\n");

arr1->deallocate(arr1);
arr2->deallocate(arr2);

```

## Example 8 : Using 'rbegin' and 'rend' with float

```c
Array* floatArray = array_create(sizeof(float), 4);

if (floatArray == NULL) 
{
    printf("Failed to create float array.\n");
    return 1;
}

float floatValues[] = {1.1f, 2.2f, 3.3f, 4.4f};
for (size_t i = 0; i < floatArray->size(floatArray); ++i) 
    floatArray->set(floatArray, i, &floatValues[i]);

for (float* it = (float*)floatArray->rbegin(floatArray); it != (float*)floatArray->rend(floatArray); --it) 
    printf("Element: %f\n", *it);

floatArray->deallocate(floatArray);


```

## Example 9 : How to create 2D Array 

```c
const size_t rows = 3;
const size_t cols = 4;

// Step 1 & 2: Create an Array of Array Pointers
Array* array2D = array_create(sizeof(Array*), rows);

// Step 3: Initialize Each Row Array
for (size_t i = 0; i < rows; ++i) 
{
    Array* row = array_create(sizeof(int), cols);
    int valueToFill = i; // Example value to fill

    row->fill(row, &valueToFill);
    array2D->set(array2D, i, &row);
}

// Step 4: Use the 2D Array
for (size_t i = 0; i < rows; ++i) 
{
    Array* row = *(Array**)array2D->at(array2D, i);
    for (size_t j = 0; j < cols; ++j) 
    {
        int* element = (int*)row->at(row, j);
        printf("%d ", *element);
    }
    printf("\n");
}

// Step 5: Clean Up
for (size_t i = 0; i < rows; ++i) 
{
    Array* row = *(Array**)array2D->at(array2D, i);
    row->deallocate(row);
}

array2D->deallocate(array2D);

```