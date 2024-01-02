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

