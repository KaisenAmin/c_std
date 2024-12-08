# Tuple Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview
This library offers a versatile implementation of tuples, structures that can hold a fixed number of elements of potentially different types. It's designed for efficiency and ease of use in a variety of applications, from data structures to systems programming.

## Key Features
- **Versatility**: Can hold any type of data, including custom structures.
- **Dynamic Memory Management**: Allocates and deallocates memory as needed.
- **Error Handling**: Robust error reporting for better debugging and reliability.
- **Comparative Functions**: Includes relational operators for comparing tuples.
- **Flexible API**: Functions for creating, modifying, and querying tuples.
- **Cross Platform**: Work in Windows and linux.


### Tuple Structure
The `Tuple` structure contains two key elements:
- **TupleElement**: This holds individual elements of the tuple, which consist of a pointer to the data and the size of the data type.
- **Tuple**: This holds an array of `TupleElement`s, representing the tuple's elements, and the size of the tuple, i.e., how many elements it holds.

### Functions:


### `Tuple* tuple_create(size_t size)`  
- **Purpose**: Allocates memory for a tuple and its elements, initializing each element to `NULL`.  
- **Parameters**:  
  - `size`: The number of elements the tuple will contain.  
- **Returns**:  
  - A pointer to the newly created `Tuple`, or `NULL` if memory allocation fails.  
- **Use case**: Used to create a new tuple with a specified number of elements.

---

### `void tuple_deallocate(Tuple* tuple)`  
- **Purpose**: Frees all memory associated with the tuple, including the memory used by each element.  
- **Parameters**:  
  - `tuple`: A pointer to the `Tuple` to be deallocated.  
- **Returns**:  
  - Nothing (void).  
- **Use case**: Used to clean up and free memory when a tuple is no longer needed.

---

### `bool tuple_set(Tuple* tuple, size_t index, void* data, size_t size)`  
- **Purpose**: Assigns new data to the specified element of the tuple, allocating memory for the data and replacing any existing data.  
- **Parameters**:  
  - `tuple`: A pointer to the `Tuple` in which to set the data.  
  - `index`: The index of the element to be set.  
  - `data`: A pointer to the data to be set in the tuple.  
  - `size`: The size of the data to be set.  
- **Returns**:  
  - `true` if the data is successfully set, `false` otherwise.  
- **Use case**: Used to modify a tuple by inserting or replacing data at a specified position.

---

### `void* tuple_get(const Tuple* tuple, size_t index, size_t* outSize)`  
- **Purpose**: Returns a pointer to the data stored at the specified index in the tuple and provides the size of the data.  
- **Parameters**:  
  - `tuple`: A pointer to the `Tuple` from which data is to be retrieved.  
  - `index`: The index of the element to retrieve.  
  - `outSize`: A pointer to a `size_t` variable where the size of the retrieved data will be stored.  
- **Returns**:  
  - A pointer to the data at the specified index, or `NULL` if an error occurs.  
- **Use case**: Used to access data stored at a specific index in the tuple, useful for retrieving dynamically allocated content.

---

### `Tuple* tuple_make_tuple(size_t num, ...)`  
- **Purpose**: Creates a new tuple with the specified number of elements, where each element is set using the provided data and size.  
- **Parameters**:  
  - `num`: The number of elements to store in the tuple.  
  - `...`: A variable number of arguments, each consisting of a pointer to data followed by a `size_t` indicating the size of the data.  
- **Returns**:  
  - A pointer to the newly created `Tuple`, or `NULL` if creation or element setting fails.  
- **Use case**: Used to create a tuple and initialize it with multiple pieces of data in one function call.

---

### `Tuple* tuple_tie(size_t num, ...)`  
- **Purpose**: Takes a variable number of pointers and stores them in a newly created tuple. Each element in the tuple is a pointer to the original data.  
- **Parameters**:  
  - `num`: The number of pointers to be tied together into the tuple.  
  - `...`: The variable number of pointers to be tied together.  
- **Returns**:  
  - A pointer to the newly created `Tuple`, or `NULL` if the creation fails.  
- **Use case**: Useful for creating a tuple that references existing variables without copying their values.

---

### `void tuple_swap(Tuple* a, Tuple* b)`  
- **Purpose**: Exchanges the contents of two tuples. After the swap, the first tuple contains the elements of the second tuple and vice versa.  
- **Parameters**:  
  - `a`: A pointer to the first `Tuple` object.  
  - `b`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - Nothing (void).  
- **Use case**: Used when two tuples need to swap their elements and sizes.

---

### `Tuple* tuple_forward_as_tuple(size_t num, ...)`  
- **Purpose**: Takes a variable number of arguments and stores them in a newly created tuple. The pointers to the original arguments are stored directly without allocating new memory.  
- **Parameters**:  
  - `num`: The number of arguments to be stored in the tuple.  
  - `...`: A variable number of arguments to be forwarded as a tuple.  
- **Returns**:  
  - A pointer to the newly created `Tuple`, or `NULL` if creation fails.  
- **Use case**: Useful for passing a set of arguments as a tuple without copying the data.

---

### `size_t tuple_size(const Tuple* tuple)`  
- **Purpose**: Returns the number of elements stored in the tuple.  
- **Parameters**:  
  - `tuple`: A pointer to the `Tuple` object.  
- **Returns**:  
  - The number of elements in the tuple, or 0 if the tuple is `NULL`.  
- **Use case**: Used to retrieve the number of elements in a tuple.

---

### `bool tuple_is_equal(const Tuple* t1, const Tuple* t2)`  
- **Purpose**: Compares two tuples element by element. Returns `true` if the tuples have the same size and their corresponding elements are equal in both size and content.  
- **Parameters**:  
  - `t1`: A pointer to the first `Tuple` object.  
  - `t2`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - `true` if the tuples are equal, `false` otherwise.  
- **Use case**: Used to check if two tuples are identical in size and content.

---

### `bool tuple_is_less(const Tuple* t1, const Tuple* t2)`  
- **Purpose**: Compares two tuples element by element and returns `true` if the first tuple is lexicographically less than the second tuple.  
- **Parameters**:  
  - `t1`: A pointer to the first `Tuple` object.  
  - `t2`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - `true` if `t1` is less than `t2`, `false` otherwise.  
- **Use case**: Used to compare two tuples lexicographically based on their size and content.

---

### `bool tuple_is_greater(const Tuple* t1, const Tuple* t2)`  
- **Purpose**: Compares two tuples element by element and returns `true` if the first tuple is lexicographically greater than the second tuple.  
- **Parameters**:  
  - `t1`: A pointer to the first `Tuple` object.  
  - `t2`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - `true` if `t1` is greater than `t2`, `false` otherwise.  
- **Use case**: Used to compare two tuples lexicographically and determine if one is greater than the other.

---

### `bool tuple_is_not_equal(const Tuple* t1, const Tuple* t2)`  
- **Purpose**: Compares two tuples and returns `true` if the tuples are not equal, i.e., if they differ in size or content.  
- **Parameters**:  
  - `t1`: A pointer to the first `Tuple` object.  
  - `t2`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - `true` if `t1` and `t2` are not equal, `false` otherwise.  
- **Use case**: Used to check if two tuples are different in size or content.

---

### `bool tuple_is_greater_or_equal(const Tuple* t1, const Tuple* t2)`  
- **Purpose**: Compares two tuples and returns `true` if the first tuple is greater than or equal to the second tuple according to lexicographical order.  
- **Parameters**:  
  - `t1`: A pointer to the first `Tuple` object.  
  - `t2`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - `true` if `t1` is greater than or equal to `t2`, `false` otherwise.  
- **Use case**: Used to determine if the first tuple is greater than or equal to the second one.

---

### `bool tuple_is_less_or_equal(const Tuple* t1, const Tuple* t2)`  
- **Purpose**: Compares two tuples and returns `true` if the first tuple is less than or equal to the second tuple according to lexicographical order.  
- **Parameters**:  
  - `t1`: A pointer to the first `Tuple` object.  
  - `t2`: A pointer to the second `Tuple` object.  
- **Returns**:  
  - `true` if `t1` is less than or equal to `t2`, `false` otherwise.  
- **Use case**: Used to determine if the first tuple is less than or equal to the second one.

---

### `bool tuple_is_empty(Tuple* t)`  
- **Purpose**: Checks whether the tuple is empty, i.e., if it contains no elements.  
- **Parameters**:  
  - `t`: A pointer to the `Tuple` object.  
- **Returns**:  
  - `true` if the tuple is empty or if `t` is `NULL`, `false` otherwise.  
- **Use case**: Used to verify if a tuple is empty or has no elements.

---

## Compilation
To compile a program using the Tuple library, ensure that all relevant source files are included. For a program `main.c` that uses the Tuple library, compile with the following command:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./tuple/tuple.c
```

This command uses GCC with C17 standard, optimizing for speed (`-O3`) and the native architecture (`-march=native`). The `-flto` and `-funroll-loops` flags enable further optimizations, while `-Wall`, `-Wextra`, and `-pedantic` enable a wide range of warnings for better code quality. The `-s` flag strips the binary to reduce size.


### Examples 

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
**Result:**
```
Int: 42
Float: 3.140000
```

---

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
**Result:**
```
Int: 42
Float: 3.140000
Vector item: 100
```

---

## Example 3: Tuple with String, Array, and Queue

```c
#include "tuple/tuple.h"
#include "vector/vector.h"
#include "array/array.h"
#include "queue/queue.h"
#include "string/std_string.h"
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
**Result:**
```
String: Hello Tuple
Array first element: 2
Queue front element: 10
```

---

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
**Result:**
```
Int: 42
Float: 3.140000
Char: A
```

---

## Example 5 : Tuple as return type functions 

```c
#include "tuple/tuple.h"
#include "string/std_string.h"
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
**Result:**
```
Age: 25
Grade: 3.80
Name: John Doe
```

---

## Example 6 : send Tuple as parameter to function 

```c
#include <stdlib.h>
#include "tuple/tuple.h"
#include "string/std_string.h"
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include "random/random.h"


void process_person_data(Tuple* personData) {
    if (!personData) {
        fmt_printf("Invalid tuple.\n");
        return;
    }
    size_t size;
    int* age = (int*)tuple_get(personData, 0, &size);
    float* grade = (float*)tuple_get(personData, 1, &size);
    Vector** vector = (Vector**)tuple_get(personData, 2, &size);
    String** name = (String**)tuple_get(personData, 3, &size);

    if (age) {
        fmt_printf("Age: %d\n", *age);
    }
    if (grade) { 
        fmt_printf("Grade: %.2f\n", *grade);
    }
    if (vector && *vector) { 
        fmt_printf("Vector size: %zu\n", vector_size(*vector));
    }
    if (name && *name) { 
        fmt_printf("Name: %s\n", string_c_str(*name));
    }
}

void random_int_generator(void *output) {
    *(int *)output = random_randint(1, 100);
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
**Result:**
```
Age: 27
Grade: 19.32
Vector size: 10
Name: amin
```

---

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
**Result:**
```
Int: 42
Float: 3.140000
Char: A
Modified Int: 100
Modified Float: 9.990000
Modified Char: Z
```

---

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
**Result:**
```
Vector item: 42
Queue front item: Hello, World!
Simple Integer: 100
```

---

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
**Result:**
```
Before swap:
Tuple1 first element: 42
Tuple2 first element: 100
After swap:
Tuple1 first element: 100
Tuple2 first element: 42
```

---

## Example 10 : forward tuple to functions as parameter with `tuple_forwrad_as_tuple`

```c
#include "tuple/tuple.h"
#include "string/std_string.h"
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
**Result in C:**
```
Name is Hello amin and age is 27
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
**Result in C++:**
```
John Smith, 25
John Daniels, 22
```

---

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
**Result:**
```
Tuple1 is less than Tuple2: true
Tuple1 is greater than Tuple2: false
Tuple1 is equal to Tuple2: false
Tuple1 is not equal to Tuple2: true
Tuple1 is greater or equal to Tuple2: false
Tuple1 is less or equal to Tuple2: true
Tuple1 is empty: false
Empty Tuple is empty: true
```

---

## License

This project is open-source and available under [ISC License].