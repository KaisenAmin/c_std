
# Array Library

The Array library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::array` in C++.

## Compilation

To compile the Array library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./array/array.c 
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Array library in your project, include the `array.h` header file in your source code.

```c
#include "array/array.h"
```

## Example 1 : how to create Array object and 'fill' array with value then get them with 'at' method

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* intArray = array_create(sizeof(int), 5);

    if (intArray == NULL) 
    {
        printf("Failed to create array.\n");
        return 1;
    }

    int valueToFill = 42;
    array_fill(intArray, &valueToFill);

    for (int i = 0; i < array_size(intArray); i++)
        printf("Number is %d\n", *(int*)array_at(intArray, i));

    printf("-----------\n");

    for (size_t i = 0; i < array_size(intArray); ++i) 
    {
        int* element = (int*)array_at(intArray, i);

        if (element != NULL) 
            printf("Element %zu: %d\n", i, *element);
    }

    array_deallocate(intArray);


    return EXIT_SUCCESS;
}

```

## Example 2 : check array is 'empty' or not 

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* intArray = array_create(sizeof(int), 5);

    if (intArray == NULL) 
    {
        printf("Failed to create array.\n");
        return 1;
    }

    int valueToFill = 42;
    array_fill(intArray, &valueToFill);

    // Check if the array is empty
    if (array_empty(intArray)) 
        printf("Array is empty.\n");
    else 
        printf("Array is not empty.\n");
        
    array_deallocate(intArray);

    return EXIT_SUCCESS;
}


```

## Example 3 : 'front', 'back' return ref to first and end of the Array

```c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* intArray = array_create(sizeof(int), 5);

    if (intArray == NULL) 
    {
        printf("Failed to create array.\n");
        return 1;
    }

    int valueToFill = 42;
    array_fill(intArray, &valueToFill);

    *(int*)array_front(intArray) = 1563;
    // Access the front and back elements

    int* frontElement = (int*)array_front(intArray);
    int* backElement = (int*)array_back(intArray);

    if (frontElement != NULL && backElement != NULL) 
        printf("Front element: %d, Back element: %d\n", *frontElement, *backElement);

    array_deallocate(intArray);

    return EXIT_SUCCESS;
}


```

## Example 4 : how to set data in position with 'set' 

```c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* intArray = array_create(sizeof(int), 5);

    if (intArray == NULL) 
    {
        printf("Failed to create array.\n");
        return 1;
    }

    int valueToFill = 42;
    array_fill(intArray, &valueToFill);

    for (size_t i = 0; i < array_size(intArray); ++i) 
    {
        int value = i * 2; 
        array_set(intArray, i, &value);
        int* element = (int*)array_at(intArray, i);

        if (element != NULL) 
            printf("Element %zu: %d\n", i, *element);
            
    }
    // Deallocate the array
    array_deallocate(intArray);

    return EXIT_SUCCESS;
}



```


## Example 5: Iterating with 'begin' and end with 'double'


```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* doubleArray = array_create(sizeof(double), 5);

    if (doubleArray == NULL) 
    {
        printf("Failed to create double array.\n");
        return 1;
    }

    double values[] = {1.5, 2.5, 3.5, 4.5, 5.5};
    for (size_t i = 0; i < array_size(doubleArray); ++i) 
        array_set(doubleArray, i, &values[i]);
        
    for (double* it = (double*)array_begin(doubleArray); it != (double*)array_end(doubleArray); ++it) 
        printf("Element: %f\n", *it);

    array_deallocate(doubleArray);

    return EXIT_SUCCESS;
}


```

## Example 6 : Using 'cbegin' and 'cend' with char*

```c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* stringArray = array_create(sizeof(char*), 3);

    if (stringArray == NULL) 
    {
        printf("Failed to create string array.\n");
        return 1;
    }

    char* strings[] = {"Hello", "World", "Array"};
    for (size_t i = 0; i < array_size(stringArray); ++i) 
        array_set(stringArray, i, &strings[i]);
        
    for (const char** it = (const char**)array_cbegin(stringArray); it != (const char**)array_cend(stringArray); ++it) 
        printf("String: %s\n", *it);
        
    array_deallocate(stringArray);

    return EXIT_SUCCESS;
}


```

## Example 7 : Using Relational Operators

```c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* arr1 = array_create(sizeof(int), 3);
    Array* arr2 = array_create(sizeof(int), 3);

    int arr1Values[] = {1, 2, 3};
    int arr2Values[] = {1, 2, 3};

    for (size_t i = 0; i < 3; ++i) 
    {
        array_set(arr1, i, &arr1Values[i]);
        array_set(arr2, i, &arr2Values[i]);
    }

    if (array_is_equal(arr1, arr2))
        printf("Arrays are equal.\n");
    else
        printf("Arrays are not equal.\n");

    array_deallocate(arr1);
    array_deallocate(arr2);

    return EXIT_SUCCESS;
}


```

## Example 8 : Using 'rbegin' and 'rend' with float

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* floatArray = array_create(sizeof(float), 4);

    if (floatArray == NULL) 
    {
        printf("Failed to create float array.\n");
        return 1;
    }

    float floatValues[] = {1.1f, 2.2f, 3.3f, 4.4f};
    for (size_t i = 0; i < array_size(floatArray); ++i) 
        array_set(floatArray, i, &floatValues[i]);

    for (float* it = (float*)array_rbegin(floatArray); it != (float*)array_rend(floatArray); --it) 
        printf("Element: %f\n", *it);

    array_deallocate(floatArray);

    return EXIT_SUCCESS;
}



```

## Example 9 : How to create 2D Array 

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    const size_t rows = 3;
    const size_t cols = 4;

    // Step 1 & 2: Create an Array of Array Pointers
    Array* array2D = array_create(sizeof(Array*), rows);

    // Step 3: Initialize Each Row Array
    for (size_t i = 0; i < rows; ++i) 
    {
        Array* row = array_create(sizeof(int), cols);
        int valueToFill = i; // Example value to fill

        array_fill(row, &valueToFill);
        array_set(array2D, i, &row);
    }

    // Step 4: Use the 2D Array
    for (size_t i = 0; i < rows; ++i) 
    {
        Array* row = *(Array**)array_at(array2D, i);
        for (size_t j = 0; j < cols; ++j) 
        {
            int* element = (int*)array_at(row, j);
            printf("%d ", *element);
        }
        printf("\n");
    }

    // Step 5: Clean Up
    for (size_t i = 0; i < rows; ++i) 
    {
        Array* row = *(Array**)array_at(array2D, i);
        array_deallocate(row);
    }

    array_deallocate(array2D);

    return EXIT_SUCCESS;
}


```

## Example 10 : Use String object in Array 

```c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    #include "string/string.h"

    const size_t numStrings = 3; 
    Array* stringArray = array_create(sizeof(String*), numStrings);

    // Initialize each String in the Array
    for (size_t i = 0; i < numStrings; ++i) 
    {
        String* str = string_create("Initial Text");
        array_set(stringArray, i, &str);
    }

    // Example manipulation: Append text to each string
    for (size_t i = 0; i < numStrings; ++i) 
    {
        String** strPtr = (String**)array_at(stringArray, i);
        String* str = *strPtr;

        string_append(str, " - Appended Text");
        printf("String %zu: %s\n", i, string_c_str(str));
    }

    // Clean up: Deallocate each String and the Array
    for (size_t i = 0; i < numStrings; ++i) 
    {
        String** strPtr = (String**)array_at(stringArray, i);
        String* str = *strPtr;

        string_deallocate(str);
    }
        
    array_deallocate(stringArray);

    return EXIT_SUCCESS;
}


```