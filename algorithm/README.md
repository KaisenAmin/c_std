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
- `algorithm_find_if`: is a function that searches a range for the first element that satisfies a specified predicate.
- `algorithm_find_if_not`: Searches a range for the first element that does not satisfy a specified predicate.
- `algorithm_find_end`: Finds the last occurrence of a subsequence within a range.
- `algorithm_find_first_of`: Searches for the first element in a range that matches any element from another range.
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
- `algorithm_fill` : This function fills a range of elements with a specified value. It takes a pointer to the beginning of the range, a pointer to the end of the range (exclusive), the size of each element, and the pointer to the value to fill.
- `algorithm_fill_n`: This function fills n elements starting from a given pointer with a specified value. It takes a pointer to the beginning of the range, the number of elements to fill, the size of each element, and the pointer to the value to fill.

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
- `algorithm_adjacent_find`: Finds the first pair of adjacent elements in a range that satisfy a comparison function.
- `algorithm_count`: Counts the number of elements in a range that are equal to a specified value, as determined by a comparison function.
- `algorithm_count_if`: Counts the elements in a range that satisfy a specified boolean predicate function.
- `algorithm_mismatch`: Finds the first position where two ranges differ, using a comparison function.
- `algorithm_equal`: Determines if two ranges are equal, based on a comparison function.
- `algorithm_is_permutation`: Checks if one range is a permutation of another, using a comparison function.
- `algorithm_search`: Searches for a subrange within a range, using a comparison function.
- `algorithm_search_n`: Looks for a sequence of repeated values within a range, based on a specified count and a comparison function.

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

## Example 2: Sorting a Vector of Integers

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

## Example 3: Sorting a Vector of Custom Structures

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

## Example 4 : Using `algorithm_find` 

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 3, 4, 8, 15, 23};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);
    int key = 8;
    int *found = (int *)algorithm_find(arr, arraySize, sizeof(int), &key, compare_ints);

    if (found != NULL) {
        printf("Found %d\n", *found);
    } else {
        printf("Not found\n");
    }

    return 0;
}

```

## Example 5 : Using `algorith_find_if`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

bool is_even(const void *a) {
    return (*(const int*)a % 2) == 0;
}

int main() {
    int arr[] = {1, 3, 4, 8, 15, 23};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);
    int key = 8;
    int *found = (int *)algorithm_find(arr, arraySize, sizeof(int), &key, compare_ints);

    if (found != NULL) {
        printf("Found %d\n", *found);
    } else {
        printf("Not found\n");
    }

    int *found_even = (int *)algorithm_find_if(arr, arraySize, sizeof(int), is_even);
    if (found_even != NULL) {
        printf("First even number in Array: %d\n", *found_even);
    } else {
        printf("No even arraySizeber found\n");
    }

    return 0;
}
```

## Example 6: `algorithm_find` with Strings

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int compare_strings(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

bool is_string_length_three(const void *str) {
    return strlen((const char*)str) == 3;
}

int main() {
    const char *arr[] = {"one", "two", "three", "four", "five"};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);

    const char *key = "two";
    const char **found = (const char **)algorithm_find(arr, arraySize, sizeof(char*), &key, compare_strings);
    if (found != NULL) {
        printf("Found %s\n", *found);
    } else {
        printf("Not found\n");
    }

    return 0;
}
```

## Example 7: `algorithm_find_if` with Custom Struct

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    int id;
    double value;
} MyStruct;

bool is_high_value(const void *element) {
    const MyStruct *structElem = (const MyStruct *)element;
    return structElem->value > 50.0;
}

int main() {
    MyStruct arr[] = {{1, 45.5}, {2, 60.5}, {3, 30.2}};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);

    MyStruct *found = (MyStruct *)algorithm_find_if(arr, arraySize, sizeof(MyStruct), is_high_value);
    if (found != NULL) {
        printf("Found struct with ID: %d and Value: %.2f\n", found->id, found->value);
    } else {
        printf("No matching struct found\n");
    }
    
    return 0;
}
```

## Example 8: `algorithm_find_if_not` with Integers

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

bool is_positive(const void *a) {
    return *(const int*)a > 0;
}

int main() {
    int arr[] = {-1, 3, 4, -8, 0, 23};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);

    int *found = (int *)algorithm_find_if_not(arr, arraySize, sizeof(int), is_positive);
    if (found != NULL) {
        printf("First non-positive number: %d\n", *found);
    } else {
        printf("All numbers are positive\n");
    }

    return 0;
}
```

## Example 9: `algorithm_find` with Doubles

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int compare_doubles(const void *a, const void *b) {
    const double diff = *(const double*)a - *(const double*)b;

    if (diff < 0) { 
        return -1;
    }
    else if (diff > 0) {
        return 1;
    }
    return 0;
}

int main() {
    double arr[] = {1.1, 3.3, 5.5, 7.7, 9.9};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);
    double key = 5.5;

    double *found = (double *)algorithm_find(arr, arraySize, sizeof(double), &key, compare_doubles);
    if (found != NULL) {
        printf("Found %f\n", *found);
    } else {
        printf("Not found\n");
    }

    return 0;
}
```

## Example 10: `algorithm_find_if_not` with Characters

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

bool is_vowel(const void *ch) {
    char c = *(const char *)ch;
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

int main() {
    char arr[] = {'a', 'b', 'c', 'd', 'e', 'f'};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);

    char *found = (char *)algorithm_find_if_not(arr, arraySize, sizeof(char), is_vowel);
    if (found != NULL) {
        printf("First consonant: %c\n", *found);
    } else {
        printf("All are vowels\n");
    }

    return 0;
}
```

## Example 11: Using `algorithm_find_end` with Integers

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
    int sub[] = {2, 3};

    int *found = (int *)algorithm_find_end(arr, 10, sizeof(int), sub, 2, sizeof(int), compare_ints);
    if (found != NULL) {
        printf("Last occurrence of subsequence found at index: %ld\n", found - arr);
    } else {
        printf("Subsequence not found\n");
    }
    return 0;
}
```

## Example 12: Using `algorithm_find_first_of` with Characters

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char str[] = "hello world";
    char chars[] = {'a', 'e', 'i', 'o', 'u'};
    
    char *found = (char *)algorithm_find_first_of(str, strlen(str), sizeof(char), chars, 5, sizeof(char), compare_chars);
    if (found != NULL) {
        printf("First vowel found: %c\n", *found);
    } else {
        printf("No vowels found\n");
    }

    return 0;
}
```

## Example 13: `algorithm_find_end` with Double Arrays

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int compare_doubles(const void *a, const void *b) {
    const double diff = *(const double*)a - *(const double*)b;

    if (diff < 0) {
        return -1;
    }
    else if (diff > 0) {
        return 1;
    }
    return 0;
}

int main() {
    double arr[] = {1.1, 2.2, 3.3, 4.4, 5.5, 1.1, 2.2, 3.3};
    double sub[] = {2.2, 3.3};
    double *found = (double *)algorithm_find_end(arr, 8, sizeof(double), sub, 2, sizeof(double), compare_doubles);
    if (found != NULL) {
        printf("Last occurrence of subsequence found at index: %ld\n", found - arr);
    } else {
        printf("Subsequence not found\n");
    }
    return 0;
}
```

## Example 14: `algorithm_find_first_of` with Integer Arrays

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int elements[] = {3, 6, 9};
    int *found = (int *)algorithm_find_first_of(arr, 10, sizeof(int), elements, 3, sizeof(int), compare_ints);
    if (found != NULL) {
        printf("First matching element: %d\n", *found);
    } else {
        printf("No matching elements found\n");
    }
    return 0;
}
```

## Example 15: Binary Search on Integer Array `algorithm_binary_search`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int value = 7;

    bool found = algorithm_binary_search(arr, 10, sizeof(int), &value, compare_ints);
    if (found) {
        printf("Value %d found in the array.\n", value);
    } else {
        printf("Value %d not found in the array.\n", value);
    }

    return 0;
}
```

## Example 16: Binary Search on Double Array `algorithm_binary_search`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_doubles(const void *a, const void *b) {
    const double diff = *(const double*)a - *(const double*)b;
    if (diff < 0) {
        return -1; 
    }
    else if (diff > 0) {
        return 1;
    }
    return 0;
}

int main() {
    double arr[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
    double value = 4.4;

    bool found = algorithm_binary_search(arr, 9, sizeof(double), &value, compare_doubles);
    if (found) {
        printf("Value %.1f found in the array.\n", value);
    } else {
        printf("Value %.1f not found in the array.\n", value);
    }

    return 0;
}
```
## Example 17: Max Element in Integer Array

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 5, 3, 7, 9, 2};
    int *max = (int *)algorithm_max_element(arr, 6, sizeof(int), compare_ints);
    if (max) {
        printf("Max element is: %d\n", *max);
    }
    return 0;
}
```

## Example 18: Min Element in Double Array

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_doubles(const void *a, const void *b) {
    const double diff = *(const double*)a - *(const double*)b;
    if (diff < 0) {
        return -1; 
    }
    else if (diff > 0) {
        return 1;
    }
    return 0;
}

int main() {
    double arr[] = {2.2, 5.5, 1.1, 4.4, 3.3};
    double *min = (double *)algorithm_min_element(arr, 5, sizeof(double), compare_doubles);

    if (min) {
        printf("Min element is: %.1f\n", *min);
    }

    return 0;
}
```

## Example 19: Max Element in Char Array

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char arr[] = {'a', 'd', 'c', 'b'};
    char *max = (char *)algorithm_max_element(arr, 4, sizeof(char), compare_chars);

    if (max) {
        printf("Max character is: %c\n", *max);
    }

    return 0;
}
```

## Example 20: Min Element in Custom Struct Array

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;
} MyStruct;

int compare_structs_by_value(const void *a, const void *b) {
    double diff = ((const MyStruct *)a)->value - ((const MyStruct *)b)->value;
    return (diff < 0) ? -1 : (diff > 0);
}

int main() {
    MyStruct arr[] = {{1, 4.5}, {2, 2.1}, {3, 3.6}};
    MyStruct *min = (MyStruct *)algorithm_min_element(arr, 3, sizeof(MyStruct), compare_structs_by_value);

    if (min) {
        printf("Min struct value is: %.1f\n", min->value);
    }

    return 0;
}
```

## Example 21 : `algorithm_for_each` using char Array .. to_uppercase 

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

void to_uppercase(void *element) {
    char *c = (char *)element;

    if (*c >= 'a' && *c <= 'z') {
        *c = *c - 'a' + 'A';
    }
}

int main() {
    char str[] = "hello world";

    algorithm_for_each(str, strlen(str), sizeof(char), to_uppercase);
    printf("%s\n", str);

    return 0;
}

```

## Example 22 : Using `algorith_for_each` with Integer Array

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void print_int(void *element) {
    printf("%d ", *(int *)element);
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    algorithm_for_each(arr, 5, sizeof(int), print_int);
    printf("\n");

    return 0;
}

```

## Example 23 : `algorithm_copy` char* Array 

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

int main() {
    const char *source = "Hello, World!";
    char dest[50];

    algorithm_copy(source, strlen(source) + 1, sizeof(char), dest);
    printf("Copied string: %s\n", dest);

    return 0;
}
```

## Example 24 : copy Integer Array `algorithm_copy`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int main() {
    int source[] = {1, 2, 3, 4, 5};
    int dest[5];

    algorithm_copy(source, 5, sizeof(int), dest);
    
    for (int i = 0; i < 5; ++i) {
        printf("%d ", dest[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 25 : Copy Vector Using `algorithm_copy`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>

int main() {
    Vector *sourceVec = vector_create(sizeof(int));
    Vector *destVec = vector_create(sizeof(int));

    int values[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        vector_push_back(sourceVec, &values[i]);
    }

    algorithm_copy(sourceVec, vector_size(sourceVec), sizeof(Vector), destVec);
    
    for (size_t i = 0; i < vector_size(destVec); ++i) {
        int *item = (int *)vector_at(destVec, i);
        printf("%d ", *item);
    }
    printf("\n");

    vector_deallocate(sourceVec);
    vector_deallocate(destVec);

    return 0;
}

```

## Example 26 : copy String object by `algorithm_copy`

```c
#include "algorithm/algorithm.h"
#include "string/string.h"
#include <stdio.h>

int main() {
    String *source = string_create("Hello C Programmers");
    String *dest = string_create("");

    algorithm_copy(source, string_length(source), sizeof(String), dest);
    printf("Value is %s\n", string_c_str(dest));

    string_deallocate(source);
    string_deallocate(dest);

    return 0;
}
```

## Example 27 : sum of Integers Using `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void sum_ints(void *result, const void *element) {
    *(int *)result += *(const int *)element;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int sum = 0;

    algorithm_accumulate(arr, 5, sizeof(int), &sum, sum_ints);
    printf("Sum of array: %d\n", sum);

    return 0;
}

```

## Example 28 : Concatenate char* using `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

void concatenate_strings(void *result, const void *element) {
    char *result_str = (char *)result;
    const char *element_str = *(const char **)element; // Correctly access the string pointer
    size_t result_len = strlen(result_str);
    size_t element_len = strlen(element_str);

    memcpy(result_str + result_len, element_str, element_len + 1); // +1 to include null terminator
}

int main() {
    const char *strings[] = {"Hello", ", ", "World", "!"};
    char result[50] = "";

    algorithm_accumulate(strings, 4, sizeof(char *), result, concatenate_strings);
    printf("Concatenated string: %s\n", result);

    return 0;
}

```

## Example 29 : Multiply Elements of Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void multiply_ints(void *result, const void *element) {
    *(int *)result *= *(const int *)element;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int product = 1;

    algorithm_accumulate(arr, 5, sizeof(int), &product, multiply_ints);
    printf("Product of array elements: %d\n", product);

    return 0;
}

```

## Example 30 : Find Maximum element in double Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void max_double(void *result, const void *element) {
    if (*(double *)result < *(const double *)element) {
        *(double *)result = *(const double *)element;
    }
}

int main() {
    double arr[] = {1.5, 2.7, 3.3, 4.4, 5.1};
    double max = 0.0;

    algorithm_accumulate(arr, 5, sizeof(double), &max, max_double);
    printf("Maximum element: %f\n", max);
    
    return 0;
}
```

## Example 31 : compute average of float Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void accumulate_floats(void *result, const void *element) {
    *(float *)result += *(const float *)element;
}

int main() {
    float arr[] = {1.2f, 2.3f, 3.4f, 4.5f, 5.6f};
    float sum = 0.0f;

    algorithm_accumulate(arr, 5, sizeof(float), &sum, accumulate_floats);
    float average = sum / 5;

    printf("Average of array elements: %f\n", average);
    return 0;
}
```

## Example 32 : Counting Occurence of a Specific element in Array 

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void count_occurrences(void *result, const void *element) {
    if (*(const int *)element == 3) {
        (*(int *)result)++;
    }
}

int main() {
    int arr[] = {1, 3, 3, 4, 3, 5, 3};
    int count = 0;

    algorithm_accumulate(arr, 7, sizeof(int), &count, count_occurrences);
    printf("Number of occurrences of 3: %d\n", count);

    return 0;
}

```

## Example 33 : Building String from char Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

void build_string(void *result, const void *element) {
    size_t len = strlen((char *)result);

    ((char *)result)[len] = *(const char *)element;
    ((char *)result)[len + 1] = '\0';
}

int main() {
    char arr[] = {'H', 'e', 'l', 'l', 'o'};
    char str[6] = "";

    algorithm_accumulate(arr, 5, sizeof(char), str, build_string);
    printf("Constructed string: %s\n", str);

    return 0;
}

```

## Example 34: Check if All Integers are Positive `algorithm_all_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool is_positive(const void *a) {
    return *(const int *)a > 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    if (algorithm_all_of(arr, 5, sizeof(int), is_positive)) {
        printf("All elements are positive.\n");
    } else {
        printf("Not all elements are positive.\n");
    }

    return 0;
}
```

## Example 35: Check if All Characters are Uppercase `algorithm_all_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <ctype.h>

bool is_uppercase(const void *ch) {
    return isupper(*(const char *)ch);
}

int main() {
    char str[] = "HELLO WORLD";

    if (algorithm_all_of(str, sizeof(str) - 1, sizeof(char), is_uppercase)) {
        printf("All characters are uppercase.\n");
    } else {
        printf("Not all characters are uppercase.\n");
    }

    return 0;
}
```

## Example 36: Check if All Values in a Custom Struct Array are Above a Threshold `algorithm_all_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;
} MyStruct;

bool is_above_threshold(const void *element) {
    const MyStruct *structElem = (const MyStruct *)element;
    return structElem->value > 10.0;
}

int main() {
    MyStruct arr[] = {{1, 12.5}, {2, 15.5}, {3, 11.2}};

    if (algorithm_all_of(arr, 3, sizeof(MyStruct), is_above_threshold)) {
        printf("All struct elements have values above 10.0.\n");
    } else {
        printf("Not all struct elements have values above 10.0.\n");
    }

    return 0;
}
```

## Example 37: Check if Any Integer is Negative `algorithm_any_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool is_negative(const void *a) {
    return *(const int *)a < 0;
}

int main() {
    int arr[] = {1, -2, 3, 4, 5};

    if (algorithm_any_of(arr, 5, sizeof(int), is_negative)) {
        printf("At least one element is negative.\n");
    } else {
        printf("No negative elements found.\n");
    }

    return 0;
}
```

## Example 38: Check if Any Character is a Digit `algorithm_any_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <ctype.h>

bool is_digit(const void *ch) {
    return isdigit(*(const char *)ch);
}

int main() {
    char str[] = "Hello World 2024";

    if (algorithm_any_of(str, sizeof(str) - 1, sizeof(char), is_digit)) {
        printf("At least one character is a digit.\n");
    } else {
        printf("No digits found.\n");
    }

    return 0;
}
```

## Example 39: Check if Any Element in a Custom Struct Array Has a Specific ID `algorithm_any_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;
} MyStruct;

bool has_id_3(const void *element) {
    const MyStruct *structElem = (const MyStruct *)element;
    return structElem->id == 3;
}

int main() {
    MyStruct arr[] = {{1, 12.5}, {2, 15.5}, {3, 11.2}};

    if (algorithm_any_of(arr, 3, sizeof(MyStruct), has_id_3)) {
        printf("At least one struct element has ID 3.\n");
    } else {
        printf("No struct element with ID 3 found.\n");
    }

    return 0;
}
```

## Example 40: Check if No Integer is Negative `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool is_negative(const void *a) {
    return *(const int *)a < 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    if (algorithm_none_of(arr, 5, sizeof(int), is_negative)) {
        printf("No elements are negative.\n");
    } else {
        printf("There are negative elements.\n");
    }

    return 0;
}
```

## Example 41: Check if No Character is a Vowel `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool is_vowel(const void *ch) {
    char c = *(const char *)ch;
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

int main() {
    char str[] = "bcdfg";

    if (algorithm_none_of(str, sizeof(str) - 1, sizeof(char), is_vowel)) {
        printf("No vowels in the string.\n");
    } else {
        printf("There are vowels in the string.\n");
    }

    return 0;
}
```

## Example 42: Check if No Element in an Array is Zero `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool is_zero(const void *a) {
    return *(const int *)a == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    if (algorithm_none_of(arr, 5, sizeof(int), is_zero)) {
        printf("No elements are zero.\n");
    } else {
        printf("There are zero elements.\n");
    }

    return 0;
}
```

## Example 43: Check if No Struct Has a Specific Value `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;

} MyStruct;

bool has_value_10(const void *element) {
    const MyStruct *structElem = (const MyStruct *)element;
    return structElem->value == 10.0;
}

int main() {
    MyStruct arr[] = {{1, 11.5}, {2, 15.5}, {3, 11.2}};

    if (algorithm_none_of(arr, 3, sizeof(MyStruct), has_value_10)) {
        printf("No struct elements have the value 10.0.\n");
    } else {
        printf("Some struct elements have the value 10.0.\n");
    }

    return 0;
}
```

## Example 44 : Using `algorithm_sort` for benchmarking with C++
`gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c .\algorithm\algorithm.c`
C Algorithm sort time: 0.004748 seconds

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate random data
void generateRandomData(int *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        data[i] = rand() % 1000; // Random numbers between 0 and 999
    }
}

int compareInts(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void printInteger(void* number) {
    printf("%d\n", *(const int*)number);
}

int main() {
    const size_t dataSize = 100000; 
    int *data = malloc(dataSize * sizeof(int));

    srand((unsigned)time(NULL));
    generateRandomData(data, dataSize);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    algorithm_sort(data, dataSize, sizeof(int), compareInts); // sort
    clock_gettime(CLOCK_MONOTONIC, &end);

    double timeTaken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("C Algorithm sort time: %f seconds\n", timeTaken);

    // algorithm_for_each(data, 100000, sizeof(int), printInteger);

    free(data);
    return 0;
}

```

C++ STL sort time: 0.0192416 seconds

```cpp 
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <chrono>

int main() {
    const size_t dataSize = 100000; // Same size as used in the C example
    std::vector<int> data(dataSize);

    // Generate random data
    std::generate(data.begin(), data.end(), []() { return rand() % 1000; });

    auto start = std::chrono::high_resolution_clock::now();
    std::sort(data.begin(), data.end());
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "C++ STL sort time: " << elapsed.count() << " seconds\n";

    return 0;
}
```
 
## Example 45: Filling an Array of Integers `algorithm_fill`

```c
#include "algorithm.h"
#include <stdio.h>

int main() {
    int array[10];
    int value = 7;

    algorithm_fill(array, array + 10, sizeof(array[0]), &value);

    for (int i = 0; i < 10; ++i) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 46: Partially Filling an Array of Doubles `algorithm_fill_n`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int main() {
    double array[10] = {0};
    double value = 3.14;

    algorithm_fill_n(array, 5, sizeof(array[0]), &value);

    for (int i = 0; i < 10; ++i) {
        printf("%.2f ", array[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 47: Filling a String with a Character `algorithm_fill`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int main() {
    char str[11];
    char ch = 'A';

    algorithm_fill(str, str + 10, sizeof(char), &ch);
    str[10] = '\0'; // Null-terminate the string

    printf("%s\n", str);

    return 0;
}
```

## Example 48: Filling a Struct Array `algorithm_fill`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;
} Item;

int main() {
    Item items[5];
    Item fillItem = {1, 9.99};

    algorithm_fill(items, items + 5, sizeof(Item), &fillItem);

    for (int i = 0; i < 5; ++i) {
        printf("Item %d: id = %d, value = %.2f\n", i, items[i].id, items[i].value);
    }

    return 0;
}
```

## Example 49: Partially Filling an Array of Chars with Different Characters `algorithm_fill_n`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int main() {
    char letters[26];
    char firstHalf = 'A';
    char secondHalf = 'N';

    algorithm_fill_n(letters, 13, sizeof(char), &firstHalf);
    algorithm_fill_n(letters + 13, 13, sizeof(char), &secondHalf);

    for (int i = 0; i < 26; ++i) {
        printf("%c ", letters[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 50: Counting Integers Equal to a Given Value `algorithm_count`

```c
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

int main() {
    int arr[] = {1, 2, 3, 4, 3, 2, 1};
    int value = 3;

    size_t count = algorithm_count(arr, 7, sizeof(int), &value, compare_ints);
    printf("Count of 3: %zu\n", count);

    return 0;
}
```

## Example 51: Counting Characters in a String `algorithm_count`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_chars(const void *a, const void *b) {
    return *(const char *)a - *(const char *)b;
}

int main() {
    char str[] = "hello world";
    char ch = 'l';

    size_t count = algorithm_count(str, sizeof(str) - 1, sizeof(char), &ch, compare_chars); 
    printf("Count of 'l': %zu\n", count);

    return 0;
}
```

## Example 52: Counting Even Numbers in an Array `algorithm_count_if`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdbool.h>

bool is_even(const void *a) {
    return *(const int *)a % 2 == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    size_t count = algorithm_count_if(arr, 10, sizeof(int), is_even);
    printf("Count of even numbers: %zu\n", count);

    return 0;
}
```

## Example 53: Counting Elements Greater Than a Certain Value `algorithm_count_if`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdbool.h>

bool is_greater_than_five(const void *a) {
    return *(const int *)a > 5;
}

int main() {
    int arr[] = {3, 7, 2, 9, 5, 6};

    size_t count = algorithm_count_if(arr, 6, sizeof(int), is_greater_than_five);
    printf("Count of numbers greater than 5: %zu\n", count);

    return 0;
}
```

## Example 54: Shuffling an Array of Characters `algorithm_shuffle`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint32_t simple_rng() {
    return rand();  // Simple RNG, replace with a better one if needed
}

int main() {
    srand((unsigned int)time(NULL));  
    char str[] = "HelloWorld";
    size_t size = sizeof(str) - 1;

    algorithm_shuffle(str, size, sizeof(char), simple_rng);

    printf("Shuffled String: %s\n", str);

    return 0;
}
```

## Example 55: Shuffling an Array of Structs `algorithm_shuffle`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int id;
    char name[10];
} Person;

uint32_t simple_rng() {
    return rand();  // Simple RNG, replace with a better one if needed
}

int main() {
    srand((unsigned int)time(NULL)); 
    Person people[] = {
        {1, "Alice"},
        {2, "Bob"},
        {3, "Charlie"},
        {4, "Dave"},
        {5, "Eve"}
    };
    size_t size = sizeof(people) / sizeof(people[0]);

    algorithm_shuffle(people, size, sizeof(Person), simple_rng);

    printf("Shuffled People:\n");
    for (size_t i = 0; i < size; ++i) {
        printf("ID: %d, Name: %s\n", people[i].id, people[i].name);
    }

    return 0;
}
```

## Example 56 : Usinf Shuffle in Array of integer `algorithm_shuffle`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint32_t simple_rng() {
    return rand();  // Simple RNG, replace with a better one if needed
}

int main() {
    srand((unsigned int)time(NULL));  
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t size = sizeof(data) / sizeof(data[0]);

    algorithm_shuffle(data, size, sizeof(data[0]), simple_rng);

    for (size_t i = 0; i < size; ++i) {
        printf("%d ", data[i]);
    }
    printf("\n");

    return 0;
}

```

## Example 57: Find Lower Bound in an Array of Integers `algorithm_lower_bound`
```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 3, 5, 7, 9};
    int val = 4;

    int *lb = (int *)algorithm_lower_bound(arr, 5, sizeof(int), &val, compare_ints);
    printf("Lower bound of 4 is at index: %lld\n", lb - arr);

    return 0;
}
```

## Example 58: Find Upper Bound in an Array of Integers `algorithm_upper_bound`
```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 3, 5, 7, 9};
    int val = 6;

    int *ub = (int *)algorithm_upper_bound(arr, 5, sizeof(int), &val, compare_ints);
    printf("Upper bound of 6 is at index: %lld\n", ub - arr);

    return 0;
}
```

## Example 59: Lower Bound in an Array of Doubles `algorithm_lower_bound`
```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_doubles(const void *a, const void *b) {
    const double diff = *(const double*)a - *(const double*)b;
    return (diff < 0) ? -1 : (diff > 0);
}

int main() {
    double arr[] = {1.1, 3.3, 5.5, 7.7, 9.9};
    double val = 4.4;

    double *lb = (double *)algorithm_lower_bound(arr, 5, sizeof(double), &val, compare_doubles);
    printf("Lower bound of 4.4 is at index: %lld\n", lb - arr);

    return 0;
}
```

## Example 60: Upper Bound in an Array of Characters `algorithm_upper_bound`
```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char arr[] = {'a', 'c', 'e', 'g', 'i'};
    char val = 'f';

    char *ub = (char *)algorithm_upper_bound(arr, 5, sizeof(char), &val, compare_chars);
    printf("Upper bound of 'f' is at index: %lld\n", ub - arr);

    return 0;
}
```

## Example 61 : `algorithm_transform`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void square(void *output, const void *input) {
    int inputValue = *(const int *)input;
    *(int *)output = inputValue * inputValue;
}

int main() {
    int inputArray[] = {1, 2, 3, 4, 5};
    int outputArray[5];
    size_t numElements = sizeof(inputArray) / sizeof(inputArray[0]);

    algorithm_transform(inputArray, numElements, sizeof(int), outputArray, square);

    for (size_t i = 0; i < numElements; ++i) {
        printf("%d ", outputArray[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 62: Negating Integers `algorithm_transform`

This example negates each integer in an array.

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void negate(void *output, const void *input) {
    int inputValue = *(const int *)input;
    *(int *)output = -inputValue;
}

int main() {
    int inputArray[] = {1, -2, 3, -4, 5};
    int outputArray[5];
    size_t numElements = sizeof(inputArray) / sizeof(inputArray[0]);

    algorithm_transform(inputArray, numElements, sizeof(int), outputArray, negate);

    for (size_t i = 0; i < numElements; ++i) {
        printf("%d ", outputArray[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 63: Converting Floats to Integers `algorithm_transform`

This example converts floating-point numbers to integers by truncating the decimal part.

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void float_to_int(void *output, const void *input) {
    float inputValue = *(const float *)input;
    *(int *)output = (int)inputValue;
}

int main() {
    float inputArray[] = {1.2f, -2.8f, 3.5f, -4.9f, 5.0f};
    int outputArray[5];
    size_t numElements = sizeof(inputArray) / sizeof(inputArray[0]);

    algorithm_transform(inputArray, numElements, sizeof(float), outputArray, float_to_int);

    for (size_t i = 0; i < numElements; ++i) {
        printf("%d ", outputArray[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 64: Computing Lengths of Strings `algorithm_transform`

This example computes the length of each string in an array of strings.

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

void string_length(void *output, const void *input) {
    const char *inputStr = *(const char **)input;
    *(size_t *)output = strlen(inputStr);
}

int main() {
    const char *inputArray[] = {"hello", "world", "example", "C", "programming"};
    size_t outputArray[5];
    size_t numElements = sizeof(inputArray) / sizeof(inputArray[0]);

    algorithm_transform(inputArray, numElements, sizeof(const char *), outputArray, string_length);

    for (size_t i = 0; i < numElements; ++i) {
        printf("%zu ", outputArray[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 65 : sum numbers with `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void sum(void *result, const void *element) {
    *(int *)result += *(const int *)element;
}

int main() {
    int array[] = {1, 2, 3, 4, 5};
    int sum_result = 0;
    size_t numElements = sizeof(array) / sizeof(array[0]);

    algorithm_reduce(array, numElements, sizeof(int), &sum_result, sum);
    printf("Sum: %d\n", sum_result);
    
    return 0;
}
```

## Example 66: Finding the Maximum Value in an Array of Integers `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <limits.h>

void max_int(void *result, const void *element) {
    int *currentMax = (int *)result;
    int value = *(const int *)element;
    if (value > *currentMax) {
        *currentMax = value;
    }
}

int main() {
    int array[] = {4, 1, 7, 3, 9, 5};
    size_t numElements = sizeof(array) / sizeof(array[0]);
    int maxResult = INT_MIN;

    algorithm_reduce(array, numElements, sizeof(int), &maxResult, max_int);

    printf("Maximum Value: %d\n", maxResult);

    return 0;
}
```

### Example 67: Concatenating Strings `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

void concatenate_strings(void *result, const void *element) {
    strcat((char *)result, *(const char **)element);
}

int main() {
    const char *strings[] = {"Hello", ", ", "world", "!"};
    size_t numElements = sizeof(strings) / sizeof(strings[0]);
    char concatenated[50] = "";

    algorithm_reduce(strings, numElements, sizeof(char *), concatenated, concatenate_strings);

    printf("Concatenated String: %s\n", concatenated);

    return 0;
}
```

### Example 68: Computing the Logical AND of Boolean Values `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdbool.h>

void logical_and(void *result, const void *element) {
    bool *res = (bool *)result;
    bool val = *(const bool *)element;
    *res = *res && val;
}

int main() {
    bool flags[] = {true, true, false, true};
    size_t numElements = sizeof(flags) / sizeof(flags[0]);
    bool andResult = true;

    algorithm_reduce(flags, numElements, sizeof(bool), &andResult, logical_and);

    printf("Logical AND of flags: %s\n", andResult ? "true" : "false");

    return 0;
}
```

### Example 69: Unique Integers in an Array `algorithm_unique`

This example demonstrates removing consecutive duplicates in an array of integers.

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 4};
    size_t arrSize = sizeof(arr) / sizeof(arr[0]);

    size_t newSize = algorithm_unique(arr, arrSize, sizeof(int), compare_ints);

    printf("Unique elements: ");
    for (size_t i = 0; i < newSize; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 70: Unique Characters in a String `algorithm_unique`

This example focuses on removing consecutive duplicate characters in a string.

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char str[] = "aabbbcdddeeeffg";
    size_t strSize = sizeof(str) - 1; // Exclude null terminator

    size_t newSize = algorithm_unique(str, strSize, sizeof(char), compare_chars);

    printf("Unique characters: ");
    for (size_t i = 0; i < newSize; i++) {
        printf("%c", str[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 71: Unique Floats in an Array `algorithm_unique`

This example demonstrates the removal of consecutive duplicate floating-point numbers in an array.

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_floats(const void *a, const void *b) {
    const float diff = *(const float*)a - *(const float*)b;
    if (diff == 0.0f) {
        return 0;
    }
    return diff < 0 ? -1 : 1;
}

int main() {
    float arr[] = {1.1f, 1.1f, 2.2f, 3.3f, 3.3f, 3.3f, 4.4f, 5.5f};
    size_t arrSize = sizeof(arr) / sizeof(arr[0]);

    size_t newSize = algorithm_unique(arr, arrSize, sizeof(float), compare_floats);

    printf("Unique floats: ");
    for (size_t i = 0; i < newSize; i++) {
        printf("%.1f ", arr[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 72: Comparing Integer Arrays `algorithm_equal`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {1, 2, 3, 4, 5};
    int arr3[] = {1, 2, 3, 4, 6};

    bool isEqual = algorithm_equal(arr1, 5, sizeof(int), arr2, 5, sizeof(int), compare_ints);
    printf("Arr1 is equal to Arr2: %s\n", isEqual ? "true" : "false");

    isEqual = algorithm_equal(arr1, 5, sizeof(int), arr3, 5, sizeof(int), compare_ints);
    printf("Arr1 is equal to Arr3: %s\n", isEqual ? "true" : "false");

    return 0;
}
```

### Example 73: Comparing Character Arrays `algorithm_equal`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_chars(const void *a, const void *b) {
    char arg1 = *(const char *)a;
    char arg2 = *(const char *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    char str1[] = "hello";
    char str2[] = "hello";
    char str3[] = "world";

    bool isEqual = algorithm_equal(str1, 5, sizeof(char), str2, 5, sizeof(char), compare_chars);
    printf("Str1 is equal to Str2: %s\n", isEqual ? "true" : "false");

    isEqual = algorithm_equal(str1, 5, sizeof(char), str3, 5, sizeof(char), compare_chars);
    printf("Str1 is equal to Str3: %s\n", isEqual ? "true" : "false");

    return 0;
}
```

### Example 74: Comparing Double Arrays `algorithm_equal`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

int compare_doubles(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;
    if (arg1 < arg2) {
        return -1;
    }
    if (arg1 > arg2) {
        return 1;
    }
    return 0;
}

int main() {
    double arr1[] = {1.1, 2.2, 3.3};
    double arr2[] = {1.1, 2.2, 3.3};
    double arr3[] = {1.1, 2.2, 4.4};

    bool isEqual = algorithm_equal(arr1, 3, sizeof(double), arr2, 3, sizeof(double), compare_doubles);
    printf("Arr1 is equal to Arr2: %s\n", isEqual ? "true" : "false");

    isEqual = algorithm_equal(arr1, 3, sizeof(double), arr3, 3, sizeof(double), compare_doubles);
    printf("Arr1 is equal to Arr3: %s\n", isEqual ? "true" : "false");

    return 0;
}
```

## Example 75 : Next Permutation of Integer Array `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    int arr[] = {1, 2, 3};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    do {
        for (size_t i = 0; i < size; ++i) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    } while (algorithm_next_permutation(arr, arr + size, sizeof(arr[0]), less_int));

    return 0;
}
```

## Example 76 : Using with Character `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

bool less_char(const void *a, const void *b) {
    return *(char *)a < *(char *)b;
}

int main() {
    char str[] = "abc";
    size_t size = strlen(str);

    do {
        printf("%s\n", str);
    } while (algorithm_next_permutation(str, str + size, sizeof(char), less_char));

    return 0;
}

```

## Example 77 : With Custom Structures `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;
} MyStruct;

bool less_mystruct(const void *a, const void *b) {
    return ((MyStruct *)a)->id < ((MyStruct *)b)->id;
}

int main() {
    MyStruct arr[] = {{1, 10.5}, {2, 20.5}, {3, 30.5}};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    do {
        for (size_t i = 0; i < size; ++i) {
            printf("{%d, %.1f} ", arr[i].id, arr[i].value);
        }
        printf("\n");
    } while (algorithm_next_permutation(arr, arr + size, sizeof(MyStruct), less_mystruct));

    return 0;
}

```

## Example 78 : Using with Vector Lib `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    Vector *vec = vector_create(sizeof(int));
    int values[] = {1, 2, 3};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    do {
        for (size_t i = 0; i < vector_size(vec); ++i) {
            int *item = vector_at(vec, i);
            printf("%d ", *item);
        }
        printf("\n");
    } while (algorithm_next_permutation(vector_begin(vec), vector_end(vec), sizeof(int), less_int));

    vector_deallocate(vec);
    return 0;
}

```

### Example 79: Prev Permutation of Integer Array `algorithm_prev_permutation`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    int arr[] = {3, 2, 1};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    do {
        for (size_t i = 0; i < size; ++i) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    } while (algorithm_prev_permutation(arr, arr + size, sizeof(int), less_int));

    return 0;
}
```

### Example 80: Prev Permutation with Characters `algorithm_prev_permutation`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <string.h>

bool less_char(const void *a, const void *b) {
    return *(char *)a < *(char *)b;
}

int main() {
    char str[] = "cba";
    size_t size = strlen(str);

    do {
        printf("%s\n", str);
    } while (algorithm_prev_permutation(str, str + size, sizeof(char), less_char));

    return 0;
}
```

### Example 81: Using with Vector Library `algorithm_prev_permutation`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    Vector *vec = vector_create(sizeof(int));
    int values[] = {3, 2, 1};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    do {
        for (size_t i = 0; i < vector_size(vec); ++i) {
            int *item = vector_at(vec, i);
            printf("%d ", *item);
        }
        printf("\n");
    } while (algorithm_prev_permutation(vector_begin(vec), vector_end(vec), sizeof(int), less_int));

    vector_deallocate(vec);
    return 0;
}
```

## Example 82 : Partitioning an Array of Integers `algorithm_partition`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

bool is_odd(const void *a) {
    return (*(int *)a) % 2 != 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    char *partition_point = (char *)algorithm_partition(arr, size, sizeof(arr[0]), is_odd);

    printf("Odd elements: ");
    for (char *ptr = (char *)arr; ptr != partition_point; ptr += sizeof(arr[0])) {
        printf("%d ", *(int *)ptr);
    }
    printf("\n");

    printf("Even elements: ");
    for (char *ptr = partition_point; ptr != (char *)arr + size * sizeof(arr[0]); ptr += sizeof(arr[0])) {
        printf("%d ", *(int *)ptr);
    }
    printf("\n");

    return 0;
}
```

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::partition
#include <vector>       // std::vector

bool IsOdd (int i) {
    return (i%2)==1; 
}

int main () {
  std::vector<int> myvector;

  // set some values:
  for (int i=1; i<10; ++i) {
    myvector.push_back(i); // 1 2 3 4 5 6 7 8 9
  }
  std::vector<int>::iterator bound;
  bound = std::partition (myvector.begin(), myvector.end(), IsOdd);

  // print out content:
  std::cout << "odd elements:";
  for (std::vector<int>::iterator it=myvector.begin(); it!=bound; ++it) {
    std::cout << ' ' << *it;
  }
  std::cout << '\n';

  std::cout << "even elements:";
  for (std::vector<int>::iterator it=bound; it!=myvector.end(); ++it) {
    std::cout << ' ' << *it;
  }
  std::cout << '\n';

  return 0;
}
```

## Example 83 : Partitioning a String Based on Uppercase Characters `algorithm_partition`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <ctype.h>

bool is_uppercase(const void *a) {
    return isupper(*(char *)a);
}

int main() {
    char str[] = "HelloWorld";
    size_t size = sizeof(str) - 1;

    char *partition_point = (char *)algorithm_partition(str, size, sizeof(char), is_uppercase);

    printf("Uppercase characters: ");
    for (char *ptr = str; ptr != partition_point; ptr += sizeof(char)) {
        printf("%c", *ptr);
    }
    printf("\n");

    printf("Other characters: ");
    for (char *ptr = partition_point; ptr != str + size * sizeof(char); ptr += sizeof(char)) {
        printf("%c", *ptr);
    }
    printf("\n");

    return 0;
}

```

## Example 85 : Partitioning an Array of Structures Based on a Positive Value `algorithm_partition`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

typedef struct {
    int id;
    double value;
} MyStruct;

bool is_positive_value(const void *a) {
    return ((MyStruct *)a)->value > 0;
}

int main() {
    MyStruct arr[] = {{1, -10.5}, {2, 20.5}, {3, -30.5}, {4, 40.5}};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    char *partition_point = (char *)algorithm_partition(arr, size, sizeof(MyStruct), is_positive_value);

    printf("Positive values: ");
    for (char *ptr = (char *)arr; ptr != partition_point; ptr += sizeof(MyStruct)) {
        MyStruct *ms = (MyStruct *)ptr;
        printf("{%d, %.1f} ", ms->id, ms->value);
    }
    printf("\n");

    printf("Non-positive values: ");
    for (char *ptr = partition_point; ptr != (char *)arr + size * sizeof(MyStruct); ptr += sizeof(MyStruct)) {
        MyStruct *ms = (MyStruct *)ptr;
        printf("{%d, %.1f} ", ms->id, ms->value);
    }
    printf("\n");

    return 0;
}

```

## Example 86: Generating Random Numbers `algorithm_generate`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>

// Generator function to fill with random numbers
void random_number_generator(void *output) {
    *(int *)output = rand() % 100; // Generate a random number between 0 and 99
}

int main() {
    int array[10];
    algorithm_generate(array, array + 10, sizeof(int), random_number_generator);

    printf("Random numbers: ");
    for (int i = 0; i < 10; ++i) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
```

## Example 87: Generating Sequential Numbers `algorithm_generate`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>

// Generator function for sequential numbers
struct sequential_number {
    int current;
};

void sequential_number_generator(void *output) {
    static struct sequential_number seq = {0};
    *(int *)output = seq.current++;
}

int main() {
    Vector *vec = vector_create(sizeof(int));

    vector_resize(vec, 10);
    // Fill vector with sequential numbers
    algorithm_generate(vector_begin(vec), vector_end(vec), sizeof(int), sequential_number_generator);

    printf("Sequential numbers: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        printf("%d ", *(int *)vector_at(vec, i));
    }
    printf("\n");

    vector_deallocate(vec);
    return 0;
}
```

### Example 88: Filling a Vector with Constant Value `algorithm_generate`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>

// Generator function to fill with a constant value
void constant_generator(void *output) {
    static const int constant_value = 5;
    *(int *)output = constant_value;
}

int main() {
    Vector *vec = vector_create(sizeof(int));
    vector_resize(vec, 10); // Resize vector to hold 10 elements

    algorithm_generate(vector_begin(vec), vector_end(vec), sizeof(int), constant_generator);

    printf("Constant values: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        printf("%d ", *(int *)vector_at(vec, i));
    }
    printf("\n");

    vector_deallocate(vec);
    return 0;
}
```

### Example 89: Generating a Fixed Number of Random Integers `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>

void random_int_generator(void *output) {
    *(int *)output = rand() % 100;
}

int main() {
    int array[5];
    algorithm_generate_n(array, 5, sizeof(int), random_int_generator);

    printf("Random integers: ");
    for (int i = 0; i < 5; ++i) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 90: Generating a Sequence of Characters `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include <stdio.h>

void char_sequence_generator(void *output) {
    static char current = 'A';
    *(char *)output = current++;

    if (current > 'Z') {
        current = 'A';
    }
}

int main() {
    char sequence[10];
    algorithm_generate_n(sequence, 10, sizeof(char), char_sequence_generator);

    printf("Character sequence: ");
    for (int i = 0; i < 10; ++i) {
        printf("%c ", sequence[i]);
    }
    printf("\n");

    return 0;
}
```

### Example 91: Generating a String Array `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>

void string_generator(void *output) {
    static int counter = 1;
    char buffer[30];

    sprintf(buffer, "String %d", counter++);
    *(String **)output = string_create(buffer);
}

int main() {
    String *stringArray[3];
    algorithm_generate_n(stringArray, 3, sizeof(String *), string_generator);

    printf("Generated strings: \n");
    for (int i = 0; i < 3; ++i) {
        printf("%s\n", stringArray[i]->dataStr);
        string_deallocate(stringArray[i]);
    }

    return 0;
}
```

### Example 92: Generating Floats in a Vector `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>

void float_generator(void *output) {
    static float value = 0.5;
    *(float *)output = value;
    value += 0.5;
}

int main() {
    Vector *vec = vector_create(sizeof(float));
    vector_resize(vec, 5);

    algorithm_generate_n(vector_begin(vec), 5, sizeof(float), float_generator);

    printf("Generated floats: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        printf("%.2f ", *(float *)vector_at(vec, i));
    }
    printf("\n");

    vector_deallocate(vec);
    return 0;
}
```

## Example 93 : `algorithm_copy_backward` how to use it 

```c
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t array_size = sizeof(array) / sizeof(array[0]);

    // Copy elements in the middle of the array to the end
    // The result pointer should point just past the last element of the array
    algorithm_copy_backward(array + 3, array + 7, sizeof(int), array + array_size);

    // Print the array
    printf("Array after copy_backward: ");
    for (size_t i = 0; i < array_size; ++i) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}

```

## Example 94 : use Vector with `algorithm_copy_backward`

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include <stdio.h>

void print_vector(Vector *vec) {
    printf("Destination vector after copy_backward: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int *value = (int *)vector_at(vec, i);
        printf("%d ", *value);
    }
    printf("\n");
}

int main() {
    Vector *source = vector_create(sizeof(int));
    Vector *destination = vector_create(sizeof(int));
    int zero = 0;

    for (int i = 1; i <= 5; i++) {
        vector_push_back(source, &i);
    }
    for (int i = 0; i < 5; i++) {
        vector_push_back(destination, &zero);
    }

    algorithm_copy_backward(vector_begin(source), vector_end(source), sizeof(int), vector_end(destination));
    print_vector(destination);
    
    vector_deallocate(source);
    vector_deallocate(destination);
    return 0;
}

```

## Example 95 : Using `algorithm_copy_backward` with a Struct 
We'll define a simple struct, say Person, with a couple of fields, and then use `algorithm_copy_backward` to copy elements of an array of Person structs.

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    int age;
} Person;

void print_people_array(Person *people, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("Name: %s, Age: %d\n", people[i].name, people[i].age);
    }
}

int main() {
    Person people[] = {
        {"Alice", 30},
        {"Bob", 25},
        {"Carol", 27},
        {"Dave", 32},
        {"Eve", 29}
    };
    size_t people_size = sizeof(people) / sizeof(people[0]);
    Person destination[5];

    memset(destination, 0, sizeof(destination));  // Initialize to zero
    // Copy people to destination in reverse order
    algorithm_copy_backward(people, people + people_size, sizeof(Person), destination + people_size);

    // Print the destination array
    printf("Destination array after copy_backward:\n");
    print_people_array(destination, people_size);

    return 0;
}

```