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
