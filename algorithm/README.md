# Algorithm Library in C

The Algorithm library is a versatile collection of functions designed to bring the power and flexibility of C++ STL `<algorithm>` functionality to C programming. It provides a suite of generic algorithms for a variety of operations on arrays and other data structures, ranging from searching and sorting to transforming and accumulating elements.

## Compilation

To compile the Algorithm library with your main program, use a GCC command similar to the one below. Add any additional library files as needed:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./algorithm/algorithm.c 
```

Ensure the GCC compiler is installed on your system and that all source files are in the correct directory structure as indicated in the project.

## Usage

To use the Algorithm library in your project, include the `algorithm.h` header file in your source code:

```c
#include "algorithm/algorithm.h"
```

## Function Descriptions

### Sorting and Searching
- `algorithm_sort`: Sorts a range of elements.
- `algorithm_find`: Finds the first occurrence of a value in a range.
- `algorithm_binary_search`: Checks if a value exists in a sorted range.

### Copying and Accumulating
- `algorithm_copy`: Copies a range of elements to a new location.
- `algorithm_accumulate`: Accumulates values in a range, starting from an initial value.

### Modifying Sequence Operations
- `algorithm_transform`: Applies a function to a range and stores the result in another range.
- `algorithm_partition`: Partitions a range based on a predicate, without sorting.
- `algorithm_nth_element`: Rearranges elements such that the nth element is in its sorted position.
- `algorithm_unique`: Removes consecutive duplicates in a range.
- `algorithm_merge`: Merges two sorted ranges.
- `algorithm_inplace_merge`: Merges two consecutive sorted ranges in-place.

### Non-modifying Sequence Operations
- `algorithm_for_each`: Applies a function to each element in a range.
- `algorithm_max_element`: Finds the maximum element in a range.
- `algorithm_min_element`: Finds the minimum element in a range.
- `algorithm_all_of`: Checks if a predicate is true for all elements in a range.
- `algorithm_any_of`: Checks if a predicate is true for any element in a range.
- `algorithm_none_of`: Checks if a predicate is false for all elements in a range.
- `algorithm_lower_bound`: Finds the first position where a value can be inserted without violating order.
- `algorithm_upper_bound`: Finds the last position where a value can be inserted without violating order.

### Reduction and Comparison
- `algorithm_reduce`: Reduces elements in a range to a single value using a specified operation.


## Example 1 : Sorting array of integer using `algorithm_sort`

```c
#include <stdio.h>
#include "algorithm/algorithm.h"

int compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) {
         return -1;
    }  
    if (arg1 > arg2) {
        return 1;
    } 
    return 0;
}

int main() {
    int arr[] = {4, 3, 2, 10, 12, 1, 5, 6};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);

    algorithm_sort(arr, arraySize, sizeof(int), compare_ints);

    for (size_t i = 0; i < arraySize; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 2: Sorting a Vector of Integers

In this example, we'll create a Vector of integers, sort them using `algorithm_sort`, and then print the sorted integers.

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) {
         return -1;
    }  
    if (arg1 > arg2) {
        return 1;
    } 
    return 0;
}

int main() {
    Vector* intVector = vector_create(sizeof(int));
    int values[] = {4, 3, 2, 10, 12, 1, 5, 6};

    for (int i = 0; i < 8; i++) {
        vector_push_back(intVector, &values[i]);
    }

    algorithm_sort(vector_data(intVector), vector_size(intVector), sizeof(int), compare_ints);

    for (size_t i = 0; i < vector_size(intVector); i++) {
        int* item = (int*)vector_at(intVector, i);
        printf("%d ", *item);
    }
    printf("\n");

    vector_deallocate(intVector);
    return EXIT_SUCCESS;
}
```

### Example 3: Sorting a Vector of Custom Structures

In this example, we'll create a Vector of custom structures, sort them using `algorithm_sort`, and then print the sorted structures.

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    double value;
} MyStruct;

// Comparison function for MyStruct by id
int compare_structs(const void* a, const void* b) {
    MyStruct arg1 = *(const MyStruct*)a;
    MyStruct arg2 = *(const MyStruct*)b;

    if (arg1.id < arg2.id) {
        return -1;
    }
    if (arg1.id > arg2.id) {
        return 1;
    }
    return 0;
}

int main() {
    Vector* structVector = vector_create(sizeof(MyStruct));
    MyStruct items[] = {{2, 20.5}, {1, 10.5}, {3, 30.5}};
    
    for (int i = 0; i < 3; i++) {
        vector_push_back(structVector, &items[i]);
    }

    algorithm_sort(vector_data(structVector), vector_size(structVector), sizeof(MyStruct), compare_structs);

    for (size_t i = 0; i < vector_size(structVector); i++) {
        MyStruct* item = (MyStruct*)vector_at(structVector, i);
        printf("ID: %d, Value: %.2f\n", item->id, item->value);
    }

    vector_deallocate(structVector);
    return EXIT_SUCCESS;
}
```
