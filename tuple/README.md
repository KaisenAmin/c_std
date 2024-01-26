# Tuple Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview
This C library offers a versatile implementation of tuples, structures that can hold a fixed number of elements of potentially different types. It's designed for efficiency and ease of use in a variety of applications, from data structures to systems programming.

## Key Features
- **Versatility**: Can hold any type of data, including custom structures.
- **Dynamic Memory Management**: Allocates and deallocates memory as needed.
- **Error Handling**: Robust error reporting for better debugging and reliability.
- **Comparative Functions**: Includes relational operators for comparing tuples.
- **Flexible API**: Functions for creating, modifying, and querying tuples.
- **Cross Platform**: Work in Windows and linux.

## Function Explanations

- `tuple_create`: Initializes a new tuple with the specified size.
- `tuple_deallocate`: Freely releases all memory associated with the tuple.
- `tuple_set`: Sets a value at a specific index in the tuple.
- `tuple_get`: Retrieves a value from a specific index in the tuple.
- `tuple_make_tuple`: Creates a tuple from a list of values.
- `tuple_tie`: Creates a tuple that ties variables together.
- `tuple_swap`: Swaps the contents of two tuples.
- `tuple_forward_as_tuple`: Forwards arguments as a tuple, typically used in functions.
- `tuple_size`: Returns the number of elements in the tuple.
- `tuple_is_equal`, `tuple_is_less`, etc.: Relational operators for tuple comparison.
- `tuple_is_empty`: Checks if the tuple is empty.

## Compilation
To compile a program using the Tuple library, ensure that all relevant source files are included. For a program `main.c` that uses the Tuple library, compile with the following command:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./tuple/tuple.c
```

This command uses GCC with C17 standard, optimizing for speed (`-O3`) and the native architecture (`-march=native`). The `-flto` and `-funroll-loops` flags enable further optimizations, while `-Wall`, `-Wextra`, and `-pedantic` enable a wide range of warnings for better code quality. The `-s` flag strips the binary to reduce size.


## Example 1 : create `tuple` with type int and float 

```c
#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    Tuple* myTuple = tuple_create(2);
    int intValue = 42;
    float floatValue = 3.14f;

    tuple_set(myTuple, 0, &intValue, sizeof(int));
    tuple_set(myTuple, 1, &floatValue, sizeof(float));

    size_t size;

    int* retrievedInt = (int*)tuple_get(myTuple, 0, &size);
    if (retrievedInt) {
        fmt_printf("Int: %d\n", *retrievedInt);
    }

    float* retrievedFloat = (float*)tuple_get(myTuple, 1, &size);
    if (retrievedFloat) {
        fmt_printf("Float: %f\n", *retrievedFloat);
    }

    tuple_deallocate(myTuple);
    return 0;
}
```

## Example 2: `Tuple` with Int, Float, and Vector

```c
#include "tuple/tuple.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Tuple* myTuple = tuple_create(3);
    int intValue = 42;
    float floatValue = 3.14f;
    Vector* vectorValue = vector_create(sizeof(int));
    int vectorData = 100;
    vector_push_back(vectorValue, &vectorData);

    tuple_set(myTuple, 0, &intValue, sizeof(int));
    tuple_set(myTuple, 1, &floatValue, sizeof(float));
    tuple_set(myTuple, 2, &vectorValue, sizeof(Vector*));

    size_t size;

    int* retrievedInt = (int*)tuple_get(myTuple, 0, &size);
    float* retrievedFloat = (float*)tuple_get(myTuple, 1, &size);
    Vector** retrievedVector = (Vector**)tuple_get(myTuple, 2, &size);

    if (retrievedInt) {
        fmt_printf("Int: %d\n", *retrievedInt);
    }
    if (retrievedFloat) {
        fmt_printf("Float: %f\n", *retrievedFloat);
    }
    if (retrievedVector) {
        int* item = (int*)vector_at(*retrievedVector, 0);
        fmt_printf("Vector item: %d\n", *item);
    }

    tuple_deallocate(myTuple);
    vector_deallocate(vectorValue);
    return 0;
}
```

## Example 3: Tuple with String, Array, and Queue

```c
#include "tuple/tuple.h"
#include "vector/vector.h"
#include "array/array.h"
#include "queue/queue.h"
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    Tuple *myTuple = tuple_create(3);
    String *stringValue = string_create("Hello Tuple");
    Array *arrayValue = array_create(sizeof(int), 3);
    Queue *queueValue = queue_create(sizeof(int));

    int arrayData[] = {1, 2, 3};
    for (int i = 0; i < 3; i++){
        array_set(arrayValue, i, &arrayData[i]);
    }

    int queueData = 10;
    queue_push(queueValue, &queueData);

    tuple_set(myTuple, 0, &stringValue, sizeof(String *));
    tuple_set(myTuple, 1, &arrayValue, sizeof(Array *));
    tuple_set(myTuple, 2, &queueValue, sizeof(Queue *));

    size_t size;

    String **retrievedString = (String **)tuple_get(myTuple, 0, &size);
    Array **retrievedArray = (Array **)tuple_get(myTuple, 1, &size);
    Queue **retrievedQueue = (Queue **)tuple_get(myTuple, 2, &size);

    if (retrievedString){
        fmt_printf("String: %s\n", string_c_str(*retrievedString));
    }
    if (retrievedArray){
        int *element = (int *)array_at(*retrievedArray, 1);
        fmt_printf("Array first element: %d\n", *element);
    }
    if (retrievedQueue){
        int *queueFront = (int *)queue_front(*retrievedQueue);
        fmt_printf("Queue front element: %d\n", *queueFront);
    }

    tuple_deallocate(myTuple);
    string_deallocate(stringValue);
    array_deallocate(arrayValue);
    queue_deallocate(queueValue);

    return 0;
}
```

## Example 4 : make_tuple with `tuple_make_tuple`

```c
#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    int intValue = 42;
    float floatValue = 3.14f;
    char charValue = 'A';

    Tuple* myTuple = tuple_make_tuple(3, &intValue, sizeof(int), &floatValue, sizeof(float), &charValue, sizeof(char));

    if (myTuple) {
        size_t size;
        int* retrievedInt = (int*)tuple_get(myTuple, 0, &size);
        float* retrievedFloat = (float*)tuple_get(myTuple, 1, &size);
        char* retrievedChar = (char*)tuple_get(myTuple, 2, &size);

        if (retrievedInt) {
            fmt_printf("Int: %d\n", *retrievedInt);
        }
        if (retrievedFloat) { 
            fmt_printf("Float: %f\n", *retrievedFloat);
        }
        if (retrievedChar) { 
            fmt_printf("Char: %c\n", *retrievedChar);
        }

        tuple_deallocate(myTuple);
    }
    return 0;
}
```

## Example 5 : Tuple as return type functions 

```c
#include "tuple/tuple.h"
#include "string/string.h"
#include "fmt/fmt.h"

Tuple* create_person_tuple(int age, float grade, const char* name) {
    Tuple* personTuple = tuple_create(3); // Create a tuple with 3 elements
    String* nameStr = string_create(name);

    tuple_set(personTuple, 0, &age, sizeof(int));
    tuple_set(personTuple, 1, &grade, sizeof(float));
    tuple_set(personTuple, 2, &nameStr, sizeof(String*));

    return personTuple;
}

int main() {
    Tuple* person = create_person_tuple(25, 3.8f, "John Doe");

    if (person) {
        size_t size;

        int* age = (int*)tuple_get(person, 0, &size);
        if (age) {
            fmt_printf("Age: %d\n", *age);
        }

        float* grade = (float*)tuple_get(person, 1, &size);
        if (grade) {
            fmt_printf("Grade: %.2f\n", *grade);
        }

        String** name = (String**)tuple_get(person, 2, &size);
        if (name) {
            fmt_printf("Name: %s\n", string_c_str(*name));
        }
        tuple_deallocate(person);
    }
    return 0;
}
```

## Example 6 : send Tuple as parameter to function 

```c
#include "tuple/tuple.h"
#include "string/string.h"
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>

void process_person_data(Tuple* personData) {
    if (!personData) {
        printf("Invalid tuple.\n");
        return;
    }
    size_t size;
    int* age = (int*)tuple_get(personData, 0, &size);
    float* grade = (float*)tuple_get(personData, 1, &size);
    Vector** vector = (Vector**)tuple_get(personData, 2, &size);
    String** name = (String**)tuple_get(personData, 3, &size);

    if (age) {
        printf("Age: %d\n", *age);
    }
    if (grade) { 
        printf("Grade: %.2f\n", *grade);
    }
    if (vector && *vector) { 
        printf("Vector size: %zu\n", vector_size(*vector));
    }
    if (name && *name) { 
        printf("Name: %s\n", string_c_str(*name));
    }
}

void random_int_generator(void *output) {
    *(int *)output = rand() % 100;
}

int main() {
    int age = 27;
    float grade = 19.32f;
    Vector* vec = vector_create(sizeof(int));
    String* name = string_create("amin");
    Tuple* information = NULL;

    vector_resize(vec, 10);
    algorithm_generate(vector_begin(vec), vector_end(vec), sizeof(int), random_int_generator);
    
    information = tuple_make_tuple(4, &age, sizeof(int), &grade, sizeof(float), &vec, sizeof(Vector*), &name, sizeof(String*));
    process_person_data(information);

    vector_deallocate(vec);
    string_deallocate(name);
    tuple_deallocate(information);

    return 0;
}
```

## Example 7 : how to use tuple_tie `tuple_tie`

```c
#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    int intValue = 42;
    float floatValue = 3.14f;
    char charValue = 'A';

    Tuple* myTuple = tuple_tie(3, &intValue, &floatValue, &charValue);

    if (myTuple) {
        size_t size;
        int** retrievedInt = (int**)tuple_get(myTuple, 0, &size);
        float** retrievedFloat = (float**)tuple_get(myTuple, 1, &size);
        char** retrievedChar = (char**)tuple_get(myTuple, 2, &size);

        if (retrievedInt) {
            fmt_printf("Int: %d\n", **retrievedInt);
        }
        if (retrievedFloat) { 
            fmt_printf("Float: %f\n", **retrievedFloat);
        }
        if (retrievedChar) { 
            fmt_printf("Char: %c\n", **retrievedChar);
        }

        // Modifying the original variables through the tuple
        **retrievedInt = 100;
        **retrievedFloat = 9.99f;
        **retrievedChar = 'Z';

        fmt_printf("Modified Int: %d\n", intValue);
        fmt_printf("Modified Float: %f\n", floatValue);
        fmt_printf("Modified Char: %c\n", charValue);

        tuple_deallocate(myTuple);
    }
    return 0;
}
```

## Example 8 : how to create make_tuple Using Queue and Vector 

```c
#include "tuple/tuple.h"
#include "vector/vector.h"
#include "queue/queue.h"
#include "fmt/fmt.h"

int main() {
    Vector* intVector = vector_create(sizeof(int));
    int intValue = 42;
    vector_push_back(intVector, &intValue);

    Queue* stringQueue = queue_create(sizeof(char*));
    char* str = "Hello, World!";
    queue_emplace(stringQueue, &str, sizeof(char*));

    int simpleInt = 100;
    Tuple* myTuple = tuple_make_tuple(3, &intVector, sizeof(Vector*), &stringQueue, sizeof(Queue*), &simpleInt, sizeof(int));

    if (myTuple) {
        size_t size;
        Vector** retrievedVector = (Vector**)tuple_get(myTuple, 0, &size);
        if (retrievedVector && *retrievedVector) {
            int* item = (int*)vector_at(*retrievedVector, 0);
            fmt_printf("Vector item: %d\n", *item);
        }

        Queue** retrievedQueue = (Queue**)tuple_get(myTuple, 1, &size);
        if (retrievedQueue && *retrievedQueue) {
            char** frontItem = (char**)queue_front(*retrievedQueue);
            if (frontItem) {
                fmt_printf("Queue front item: %s\n", *frontItem);
            }
        }

        int* retrievedInt = (int*)tuple_get(myTuple, 2, &size);
        if (retrievedInt) {
            fmt_printf("Simple Integer: %d\n", *retrievedInt);
        }

        tuple_deallocate(myTuple);
        vector_deallocate(intVector);
        queue_deallocate(stringQueue);
    }
    return 0;
}
```

## Example 9 : swap tuple with `tuple_swap`

```c
#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    size_t size1, size2;

    Tuple* tuple1 = tuple_create(1);
    int intValue1 = 42;
    tuple_set(tuple1, 0, &intValue1, sizeof(int));

    Tuple* tuple2 = tuple_create(1);
    int intValue2 = 100;
    tuple_set(tuple2, 0, &intValue2, sizeof(int));

    fmt_printf("Before swap:\n");
    fmt_printf("Tuple1 first element: %d\n", *(int*)tuple_get(tuple1, 0, &size1));
    fmt_printf("Tuple2 first element: %d\n", *(int*)tuple_get(tuple2, 0, &size2));

    tuple_swap(tuple1, tuple2);

    fmt_printf("After swap:\n");
    fmt_printf("Tuple1 first element: %d\n", *(int*)tuple_get(tuple1, 0, &size1));
    fmt_printf("Tuple2 first element: %d\n", *(int*)tuple_get(tuple2, 0, &size2));

    tuple_deallocate(tuple1);
    tuple_deallocate(tuple2);
    return 0;
}
```

## Example 10 : forward tuple to functions as parameter with `tuple_forwrad_as_tuple`

```c
#include "tuple/tuple.h"
#include "string/string.h"
#include "fmt/fmt.h"

void print_back(Tuple* tuple) {
    size_t size;
    String** retrievedString = (String**)tuple_get(tuple, 0, &size);
    int* retrievedAge = (int*)tuple_get(tuple, 1, &size);
    
    fmt_printf("Name is %s and age is %d\n", string_c_str(*retrievedString), *retrievedAge);

    tuple_deallocate(tuple);
}

int main() {
    String* name = string_create("Hello amin");    
    int age = 27;

    print_back(tuple_forward_as_tuple(2, &name, &age));

    string_deallocate(name);
    return 0;
}
```

`C++ Same Code`

```c
#include <iostream>     // std::cout
#include <tuple>        // std::tuple, std::get, std::forward_as_tuple
#include <string>       // std::string

void print_pack (std::tuple<std::string&&,int&&> pack) {
  std::cout << std::get<0>(pack) << ", " << std::get<1>(pack) << '\n';
}

int main() {
  std::string str ("John");
  print_pack (std::forward_as_tuple(str+" Smith",25));
  print_pack (std::forward_as_tuple(str+" Daniels",22));
  return 0;
}
```

## Example 11 : relationals opeartors and `tuple_empty`

```c
#include "tuple/tuple.h"
#include "fmt/fmt.h"

int main() {
    Tuple* tuple1 = tuple_create(2);
    Tuple* tuple2 = tuple_create(2);
    Tuple* emptyTuple = tuple_create(0);

    int value1 = 100;
    int value2 = 200;
    tuple_set(tuple1, 0, &value1, sizeof(int));
    tuple_set(tuple1, 1, &value2, sizeof(int));
    tuple_set(tuple2, 0, &value2, sizeof(int));
    tuple_set(tuple2, 1, &value1, sizeof(int));

    fmt_printf("Tuple1 is less than Tuple2: %s\n", tuple_is_less(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is greater than Tuple2: %s\n", tuple_is_greater(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is equal to Tuple2: %s\n", tuple_is_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is not equal to Tuple2: %s\n", tuple_is_not_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is greater or equal to Tuple2: %s\n", tuple_is_greater_or_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is less or equal to Tuple2: %s\n", tuple_is_less_or_equal(tuple1, tuple2) ? "true" : "false");
    fmt_printf("Tuple1 is empty: %s\n", tuple_is_empty(tuple1) ? "true" : "false");
    fmt_printf("Empty Tuple is empty: %s\n", tuple_is_empty(emptyTuple) ? "true" : "false");

    tuple_deallocate(tuple1);
    tuple_deallocate(tuple2);

    return 0;
}
```