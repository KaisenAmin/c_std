# Algorithm Library in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License


The Algorithm library is a versatile collection of functions designed to bring the power and flexibility of C++ STL `<algorithm>` functionality to C programming. It provides a suite of generic algorithms for a variety of operations on arrays and other data structures, ranging from searching and sorting to transforming and accumulating elements.

## Compilation

To compile the Algorithm library with your main program, use a GCC command similar to the one below. Add any additional library files as needed:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./algorithm/algorithm.c 
```

Ensure the GCC compiler is installed on your system and that all source files are in the correct directory structure as indicated in the project.

## Usage

To use the Algorithm library in your project, include the `algorithm.h` header file in your source code:

```c
#include "algorithm/algorithm.h"
```

## Function Descriptions

### `void algorithm_stable_sort(void *base, size_t num, size_t size, CompareFunc comp)`
- **Description**: Performs a stable sort using the merge sort algorithm, which preserves the relative order of equal elements.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
  - `comp`: Comparison function to define the order of elements.
- **Return Value**:  
  No return value. The array is sorted in place.

---

### `void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp)`
- **Description**: Uses quicksort to sort an array. This is a non-stable sort, which means the relative order of equal elements may not be preserved.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
  - `comp`: Comparison function to define the order of elements.
- **Return Value**:  
  No return value. The array is sorted in place.

---

### `void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp)`
- **Description**: Finds the first element in the array that matches the provided value.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
  - `val`: Pointer to the value to find.
  - `comp`: Comparison function used to check for equality between elements.
- **Return Value**:  
  Returns a pointer to the first matching element if found; otherwise, returns `NULL`.

---

### `void *algorithm_find_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred)`
- **Description**: Finds the first element in the array that satisfies the given predicate.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
  - `pred`: Predicate function that returns true if the element matches the condition.
- **Return Value**:  
  Returns a pointer to the first matching element if found; otherwise, returns `NULL`.

---

### `void *algorithm_find_if_not(const void *base, size_t num, size_t size, BoolPredicateFunc pred)`
- **Description**: Finds the first element in the array that does not satisfy the given predicate.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
  - `pred`: Predicate function that returns true if the element should be skipped.
- **Return Value**:  
  Returns a pointer to the first element that does not satisfy the predicate; otherwise, returns `NULL`.

---

### `void *algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp)`
- **Description**: Finds the last occurrence of a subsequence within a larger array.
- **Parameters**:
  - `base1`: Pointer to the main array.
  - `num1`: Number of elements in the main array.
  - `size1`: Size of each element in the main array.
  - `base2`: Pointer to the subsequence array.
  - `num2`: Number of elements in the subsequence array.
  - `size2`: Size of each element in the subsequence array.
  - `comp`: Comparison function to compare elements.
- **Return Value**:  
  Returns a pointer to the last occurrence of the subsequence in the main array if found; otherwise, returns `NULL`.

---

### `void *algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp)`
- **Description**: Finds the first occurrence of any element from one array in another array.
- **Parameters**:
  - `base1`: Pointer to the main array.
  - `num1`: Number of elements in the main array.
  - `size1`: Size of each element in the main array.
  - `base2`: Pointer to the array of elements to search for.
  - `num2`: Number of elements in the search array.
  - `size2`: Size of each element in the search array.
  - `comp`: Comparison function to compare elements.
- **Return Value**:  
  Returns a pointer to the first matching element in the main array if found; otherwise, returns `NULL`.

---

### `size_t algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp)`

- **Purpose**: Performs a binary search on a sorted array to find the index of a specific value.
- **Parameters**:
  - `base`: Pointer to the sorted array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in the array.
  - `val`: Pointer to the value to find.
  - `comp`: Comparison function used to compare elements.
- **Return**: Index of the found element, or `(size_t)-1` if not found.
- **Use Case**: Efficiently finding an element in a sorted array.

---

### `void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Finds the maximum element in an array based on a comparison function.
- **Parameters**:
  - `base`: Pointer to the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `comp`: Comparison function that returns a negative value if the first element is less than the second, zero if they are equal, and a positive value if the first element is greater.
- **Return**: Pointer to the maximum element, or `NULL` if the array is empty.

---

### `void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Finds the minimum element in an array based on a comparison function.
- **Parameters**:
  - Same as `algorithm_max_element`.
- **Return**: Pointer to the minimum element, or `NULL` if the array is empty.

---

### `void algorithm_for_each(void *base, size_t num, size_t size, ForEachOpFunc op)`

- **Purpose**: Applies a function to each element in an array.
- **Parameters**:
  - `base`: Pointer to the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `op`: A function to apply to each element.
- **Return**: None. The function performs an operation on each element.

---

### `void algorithm_copy(const void *source, size_t num, size_t size, void *dest)`

- **Purpose**: Copies elements from a source array to a destination array.
- **Parameters**:
  - `source`: Pointer to the source array.
  - `num`: Number of elements to copy.
  - `size`: Size of each element.
  - `dest`: Pointer to the destination array.
- **Return**: None. The elements are copied from the source to the destination.

---

### `void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, AccumulateOpFunc op)`

- **Purpose**: Accumulates a result by applying an operation to each element and an initial value.
- **Parameters**:
  - `base`: Pointer to the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `init`: Pointer to the initial value and where the result will be stored.
  - `op`: A function that defines how to accumulate the values.
- **Return**: Pointer to the accumulated result, which is the same as `init`.

---

### `bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred)`

- **Purpose**: Checks if all elements in a range satisfy a predicate.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `pred`: Predicate function that returns `true` if the element satisfies the condition.
- **Return**: `true` if all elements satisfy the predicate, otherwise `false`.

---

### `bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred)`

- **Purpose**: Checks if any element in a range satisfies a predicate.
- **Parameters**:
  - Same as `algorithm_all_of`.
- **Return**: `true` if any element satisfies the predicate, otherwise `false`.

---

### `bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred)`

- **Purpose**: Checks if none of the elements in a range satisfy a predicate.
- **Parameters**:
  - Same as `algorithm_all_of`.
- **Return**: `true` if none of the elements satisfy the predicate, otherwise `false`.

---

### `void algorithm_fill(void *first, void *last, size_t size, const void *val)`

- **Purpose**: Fills a range of elements with a specified value.
- **Parameters**:
  - `first`: Pointer to the start of the range.
  - `last`: Pointer to the end of the range.
  - `size`: Size of each element.
  - `val`: Value to fill the range with.
- **Return**: None. The range is modified in place.

---

### `void algorithm_fill_n(void *first, size_t n, size_t size, const void *val)`

- **Purpose**: Fills the first `n` elements of a range with a specified value.
- **Parameters**:
  - `first`: Pointer to the start of the range.
  - `n`: Number of elements to fill.
  - Same as `algorithm_fill`.
- **Return**: None.

---

### `size_t algorithm_count(const void *base, size_t num, size_t size, const void *val, CompareFunc comp)`

- **Purpose**: Counts the number of occurrences of a value in a range.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements.
  - `size`: Size of each element.
  - `val`: Value to count.
  - `comp`: Comparison function that returns `0` if elements are equal.
- **Return**: The number of occurrences of `val` in the array.

---

### `size_t algorithm_count_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred)`

- **Purpose**: Counts the number of elements in a range that satisfy a predicate.
- **Parameters**:
  - Same as `algorithm_count`, except instead of a value to compare, a predicate function `pred` is provided.
- **Return**: The number of elements that satisfy the predicate.

---

### `void algorithm_shuffle(void *base, size_t num, size_t size, UniformRandomNumberGenerator rng)`

- **Purpose**: Randomly shuffles the elements of an array.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `rng`: Random number generator function.
- **Return**: None. The array is shuffled in place.

---

### `void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp)`

- **Purpose**: Finds the first position in a sorted range where a value can be inserted without violating the order.
- **Parameters**:
  - `base`: Pointer to the sorted array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `val`: Value to insert.
  - `comp`: Comparison function used to compare elements.
- **Return**: Pointer to the first position where `val` can be inserted.

---

### `void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp)`

- **Purpose**: Finds the first position in a sorted range where the elements are greater than a specified value.
- **Parameters**: Same as `algorithm_lower_bound`.
- **Return**: Pointer to the first position where elements are greater than `val`.

---

### `void algorithm_transform(const void *base, size_t num, size_t size, void *result, TransformFunc op)`

- **Purpose**: Transforms each element in an array and stores the result in another array.
- **Parameters**:
  - `base`: Pointer to the input array.
  - `num`: Number of elements.
  - `size`: Size of each element.
  - `result`: Pointer to the output array.
  - `op`: Function that defines the transformation.
- **Return**: None. The transformed elements are stored in the result array.

---

### `void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, ReduceFunc op)`

- **Purpose**: Reduces a range of elements to a single value using a specified binary operation.
- **Parameters**:
  - `base`: Pointer to the input array.
  - `num`: Number of elements.
  - `size`: Size of each element.
  - `init`: Initial value and result storage.
  - `op`: Binary operation function.
- **Return**: Pointer to the accumulated result (`init`).

---

### `size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Removes consecutive duplicate elements from a sorted array.
- **Parameters**:
  - `base`: Pointer to the sorted array.
  - `num`: Number of elements.
  - `size`: Size of each element.
  - `comp`: Comparison function used to compare elements.
- **Return**: The number of unique elements remaining in the array.

---

### `bool algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp)`

- **Purpose**: Compares two ranges to determine if they contain the same elements in the same order.
- **Parameters**:
  - `base1`: Pointer to the first array.
  - `num1`: Number of elements in the first array.
  - `size1`: Size of each element in the first array.
  - `base2`: Pointer to the second array.
  - `num2`: Number of elements in the second array.
  - `size2`: Size of each element in the second array.
  - `comp`: Comparison function to compare elements.
- **Return**: `true` if the two ranges are equal, `false` otherwise.

---

### `bool algorithm_next_permutation(void *first, void *last, size_t size, CompareFuncBool comp) `

- **Purpose**: Rearranges elements into the next lexicographical permutation.
- **Parameters**:
  - `first`: Pointer to the start of the range.
  - `last`: Pointer to one past the end of the range.
  - `size`: Size of each element.
  - `comp`: Comparison function to compare elements.
- **Return**: `true` if the next permutation is generated, `false` if the range is reset to the smallest permutation.

---

### `bool algorithm_prev_permutation(void *first, void *last, size_t size, CompareFuncBool comp)`

- **Purpose**: Rearranges elements into the previous lexicographical permutation.
- **Parameters**: Same as `algorithm_next_permutation`.
- **Return**: `true` if the previous permutation is generated, `false` if the range is reset to the largest permutation.

---

### `void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred)`

- **Purpose**: Partitions a range of elements so that all elements for which the predicate returns `true` appear before those for which it returns `false`.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements.
  - `size`: Size of each element.
  - `pred`: Predicate function that returns `true` for elements to appear before others.
- **Return**: Pointer to the first element in the second group (where `pred` returns `false`).

---

### `void algorithm_generate(void *first, void *last, size_t size, GeneratorFunc gen)`

- **Purpose**: Fills a range with values generated by a generator function.
- **Parameters**:
  - `first`: Pointer to the start of the range.
  - `last`: Pointer to the end of the range.
  - `size`: Size of each element.
  - `gen`: Generator function that produces values.
- **Return**: None.

---

### `void algorithm_generate_n(void *first, size_t n, size_t size, GeneratorFunc gen)`

- **Purpose**: This function fills the first `n` elements of a range with values produced by a generator function. It applies the generator function `gen` to generate values and stores them sequentially in the memory region starting from `first`.

- **Parameters**:
  - `first`: Pointer to the beginning of the memory region where the values will be stored.
  - `n`: The number of elements to generate.
  - `size`: The size of each element in bytes.
  - `gen`: A generator function that returns the value to be stored in each element.
- **Return**: No return value.
- **Use Case**: Use this function to populate an array or data structure with values generated on the fly, such as initializing a set of random numbers or filling a range with a specific pattern.

---

### `void algorithm_copy_backward(const void *first, const void *last, size_t size, void *result)`

- **Purpose**: Copies elements from one range to another in reverse order.
- **Parameters**:
  - `first`: Pointer to the start of the range.
  - `last`: Pointer to the end of the range.
  - `size`: Size of each element.
  - `result`: Pointer to the destination array.
- **Return**: None.

---

### `void algorithm_copy_if(const void *first, const void *last, size_t size, void *result, UnaryPredicateFunc pred)`

- **Purpose**: Copies elements from one array to another if they satisfy a predicate function.
- **Parameters**:
  - `first`, `last`: Source range.
  - `size`: Size of each element.
  - `result`: Destination array.
  - `pred`: Predicate function that determines which elements to copy.
- **Return**: None.

---

### `void algorithm_copy_n(const void *first, size_t n, size_t size, void *result)`

- **Purpose**: Copies the first `n` elements from one array to another.
- **Parameters**:
  - `first`: Pointer to the start of the source array.
  - `n`: Number of elements to copy.
  - `size`: Size of each element.
  - `result`: Destination array.
- **Return**: None.

---

### `Pair algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, CompareFunc comp)`

- **Purpose**: Finds the range of elements equal to a specified value in a sorted array.
- **Parameters**:
  - `base`: Pointer to the sorted array.
  - `num`: Number of elements.
  - `size`: Size of each element.
  - `val`: Pointer to the value to search for.
  - `comp`: Comparison function.
- **Return**: A `Pair` structure with `first` pointing to the first element equal to `val` and `second` pointing to one past the last.

---

### `bool algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, CompareFunc comp)`

- **Purpose**: Determines if all elements of a sorted range are included in another sorted range.
- **Parameters**:
  - `first1`, `num1`, `size1`: The main array.
  - `first2`, `num2`, `size2`: The array whose elements are being checked for inclusion.
  - `comp`: Comparison function.
- **Return**: `true` if all elements of `first2` are included in `first1`, `false` otherwise.

---

### `size_t algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, CompareFunc comp)`

- **Purpose**: Copies unique elements from a sorted array to a new array.
- **Parameters**:
  - `first`, `num`, `size`: Source array.
  - `result`: Destination array.
  - `comp`: Comparison function to determine uniqueness.
- **Return**: Number of unique elements copied.

---

### `void algorithm_swap(void *a, void *b, size_t size)`

- **Purpose**: Swaps the values of two elements.
- **Parameters**:
  - `a`, `b`: Pointers to the elements to swap.
  - `size`: Size of each element.
- **Return**: None.

---

### `void algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size)`

- **Purpose**: Swaps elements between two ranges.
- **Parameters**:
  - `first1`, `first2`: Pointers to the start of the ranges.
  - `num`: Number of elements to swap.
  - `size`: Size of each element.
- **Return**: None.

---

### `bool algorithm_is_sorted(const void *base, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Checks if the elements in a range are sorted in non-decreasing order.
- **Parameters**:
  - `base`, `num`, `size`: The array to check.
  - `comp`: Comparison function to check the order.
- **Return**: `true` if the range is sorted, `false` otherwise.

---

### `void *algorithm_is_sorted_until(const void *base, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Finds the first position in a range where the elements are no longer sorted in non-decreasing order.
- **Parameters**:
  - `base`, `num`, `size`: The array to check.
  - `comp`: Comparison function to check the order.
- **Return**: Pointer to the first element where the sequence is no longer sorted, or the end of the array if it's fully sorted.

---

### `void algorithm_rotate(void *first, void *middle, void *last, size_t size)`

- **Purpose**: Rotates elements in a range, shifting elements between `first`, `middle`, and `last`.
- **Parameters**:
  - `first`, `middle`, `last`: Pointers to the start, middle, and end of the range to rotate.
  - `size`: Size of each element.
- **Return**: None.

---

### `void algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result)`

- **Purpose**: Rotates elements in a range and copies the result to another destination array.
- **Parameters**:
  - `first`, `middle`, `last`: Pointers to the start, middle, and end of the range to rotate.
  - `size`: Size of each element.
  - `result`: Pointer to the destination array where the rotated elements are copied.
- **Return**: None.

---

### `void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp)`

- **Purpose**: Merges two sorted arrays into a single sorted array.
- **Parameters**:
  - `base1`, `num1`: The first sorted array.
  - `base2`, `num2`: The second sorted array.
  - `size`: Size of each element.
  - `result`: Pointer to the array where the merged result will be stored.
  - `comp`: Comparison function to determine the order.
- **Return**: None.

---

### `void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Merges two sorted halves of an array in place.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `middle`: Index of the middle element.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
  - `comp`: Comparison function to order elements.
- **Description**: This function rearranges elements between two sorted parts of an array into one merged sorted array.

---

### `void *algorithm_adjacent_find(const void *base, size_t num, size_t size, CompareFunc comp)`

- **Purpose**: Finds the first occurrence of two consecutive elements that are equal.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element.
  - `comp`: Comparison function to check equality.
- **Return**: Pointer to the first element of the first pair of consecutive equal elements, or `NULL` if no match is found.

---

### `Pair algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFuncBool comp)`

- **Purpose**: Finds the first position where two ranges differ.
- **Parameters**:
  - `base1`, `num1`, `size1`: First array details.
  - `base2`, `num2`, `size2`: Second array details.
  - `comp`: Comparison function to compare elements.
- **Return**: A `Pair` where `first` and `second` point to the first differing elements in the two arrays. If the ranges are equal, both pointers are `NULL`.

---

### `bool algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp)`

- **Purpose**: Checks if two arrays are permutations of each other.
- **Parameters**:
  - `base1`, `num1`, `size1`: First array details.
  - `base2`, `num2`, `size2`: Second array details.
  - `comp`: Comparison function to compare elements.
- **Return**: `true` if the arrays are permutations of each other, `false` otherwise.

---

### `const void* algorithm_search(const void* first1, const void* last1, size_t size1, const void* first2, const void* last2, size_t size2, CompareFuncBool comp)`

- **Purpose**: Searches for the first occurrence of a sequence within another sequence.
- **Parameters**:
  - `first1`, `last1`, `size1`: Details of the first sequence (array).
  - `first2`, `last2`, `size2`: Details of the second sequence to search for.
  - `comp`: Comparison function to compare elements between sequences.
- **Return**: Pointer to the first element of the first occurrence of `first2` within `first1`. Returns `last1` if not found.

---

### `const void *algorithm_search_n(const void *first, const void* last, size_t size, size_t count, const void *val, CompareFuncBool comp)`

- **Purpose**: Searches for a sequence of `n` identical elements within a range.
- **Parameters**:
  - `first`, `last`, `size`: Details of the range (array).
  - `count`: Number of consecutive elements to search for.
  - `val`: Pointer to the value that the elements should match.
  - `comp`: Comparison function to compare elements with `val`.
- **Return**: Pointer to the first element of the found sequence, or `last` if not found.

---

### `void *algorithm_remove(void *base, size_t num, size_t size, const void *val, CompareFunc comp)`

- **Purpose**: Removes elements that match a specified value from an array.
- **Parameters**:
  - `base`, `num`, `size`: Details of the array.
  - `val`: Pointer to the value to remove from the array.
  - `comp`: Comparison function to compare elements with `val`.
- **Return**: Pointer to the new end of the array after the matching elements are removed.

---

### `void algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, CompareFunc comp)`

- **Purpose**: Copies elements from a source array to a destination array, excluding elements that match a specified value.
- **Parameters**:
  - `source`, `num`, `size`: Details of the source array.
  - `result`: Pointer to the destination array.
  - `val`: Pointer to the value to exclude.
  - `comp`: Comparison function to compare elements with `val`.
- **Return**: No return value, but copies elements excluding those that match `val`.

---

### `size_t algorithm_remove_copy_if(const void *source, size_t num, size_t size, void *result, BoolPredicateFunc pred)`

- **Purpose**: Removes elements from the source array based on a predicate function and copies the remaining elements to the result array.
- **Parameters**:
  - `source`, `num`, `size`: Details of the source array.
  - `result`: Pointer to the destination array.
  - `pred`: Predicate function to determine if an element should be removed.
- **Return**: The number of elements copied to the result array.

---

### `void algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, CompareFunc comp)`

- **Purpose**: Replaces occurrences of a value in an array.
- **Parameters**:
  - `base`, `num`, `size`: Details of the array.
  - `old_val`: Pointer to the value to replace.
  - `new_val`: Pointer to the new value.
  - `comp`: Comparison function to compare elements with `old_val`.
- **Return**: No return value, but replaces elements matching `old_val` with `new_val`.

---

### `void algorithm_replace_if(void *base, size_t num, size_t size, const void *new_val, BoolPredicateFunc pred)`

- **Purpose**: Replaces elements in an array based on a predicate function.
- **Parameters**:
  - `base`, `num`, `size`: Details of the array.
  - `new_val`: Pointer to the value to replace the elements with.
  - `pred`: Predicate function that determines if an element should be replaced.
- **Return**: No return value, but replaces elements that satisfy the predicate with `new_val`.

---

### `void* algorithm_begin(void* base)`
- **Description**: Returns a pointer to the first element of the array. This is equivalent to the C++ `std::begin` function and is useful for iterating over the array or other collections.
- **Parameters**:
  - `base`: Pointer to the start of the array.
- **Return Value**:  
  A pointer to the first element of the array.

---

### `void* algorithm_end(void* base, size_t num, size_t size)`
- **Description**: Returns a pointer to one past the last element of the array. This is equivalent to the C++ `std::end` function and is useful for defining the range [begin, end) in algorithms that iterate over collections.
- **Parameters**:
  - `base`: Pointer to the start of the array.
  - `num`: Number of elements in the array.
  - `size`: Size of each element in bytes.
- **Return Value**:  
  A pointer to one past the last element of the array. This is used as a boundary for iteration.

---

### `void algorithm_iota(void* first, void* last, void* val, size_t size, DataType type)`
- **Description**: Fills a range `[first, last)` with successive values, starting from `val` and incrementing it for each element. It is a generic implementation that works for various data types such as `int`, `char`, `float`, `double`, `short`, `long`, `long long`, and `unsigned long`. The behavior is similar to C++'s `std::iota`.
- **Parameters**:
  - `first`: Pointer to the start of the range.
  - `last`: Pointer to one past the end of the range.
  - `val`: Pointer to the initial value that is assigned to the first element. The value is incremented after each assignment.
  - `size`: Size of each element in the array in bytes.
  - `type`: The enumeration type for showing exactly data type.
- **Return Value**:  
  No return value. The range is filled in place with incremented values.

---

## Example 1 : Sorting array of integer using `algorithm_sort`

```c
#include "fmt/fmt.h"
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
        fmt_printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
1 2 3 4 5 6 10 12
```

---

## Example 2: Sorting a Vector of Integers

In this example, we'll create a Vector of integers, sort them using `algorithm_sort`, and then print the sorted integers.

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
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
        fmt_printf("%d ", *item);
    }
    fmt_printf("\n");

    vector_deallocate(intVector);
    return EXIT_SUCCESS;
}
```
***Result:**
```
1 2 3 4 5 6 10 12
```

---

## Example 3: Sorting a Vector of Custom Structures

In this example, we'll create a Vector of custom structures, sort them using `algorithm_sort`, and then print the sorted structures.

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
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
        fmt_printf("ID: %d, Value: %.2f\n", item->id, item->value);
    }

    vector_deallocate(structVector);
    return EXIT_SUCCESS;
}
```
**Result:**
```
ID: 1, Value: 10.50
ID: 2, Value: 20.50
ID: 3, Value: 30.50
```

---

## Example 4 : Using `algorithm_find` 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 3, 4, 8, 15, 23};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);
    int key = 8;
    int *found = (int *)algorithm_find(arr, arraySize, sizeof(int), &key, compare_ints);

    if (found != NULL) {
        fmt_printf("Found %d\n", *found);
    } 
    else {
        fmt_printf("Not found\n");
    }

    return 0;
}
```
**Result:**
```
Found 8
```

---

## Example 5 : Using `algorith_find_if`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("Found %d\n", *found);
    } 
    else {
        fmt_printf("Not found\n");
    }

    int *found_even = (int *)algorithm_find_if(arr, arraySize, sizeof(int), is_even);
    if (found_even != NULL) {
        fmt_printf("First even number in Array: %d\n", *found_even);
    } 
    else {
        fmt_printf("No even arraySizeber found\n");
    }

    return 0;
}
```
**Result:**
```
Found 8
First even number in Array: 4
```

---

## Example 6: `algorithm_find` with Strings

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("Found %s\n", *found);
    } 
    else {
        fmt_printf("Not found\n");
    }

    return 0;
}
```
**Result:**
```
Found two
```

---

## Example 7: `algorithm_find_if` with Custom Struct

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("Found struct with ID: %d and Value: %.2f\n", found->id, found->value);
    } 
    else {
        fmt_printf("No matching struct found\n");
    }
    
    return 0;
}
```
**Result:**
```
Found struct with ID: 2 and Value: 60.50
```

---

## Example 8: `algorithm_find_if_not` with Integers

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

bool is_positive(const void *a) {
    return *(const int*)a > 0;
}

int main() {
    int arr[] = {-1, 3, 4, -8, 0, 23};
    size_t arraySize = sizeof(arr) / sizeof(arr[0]);

    int *found = (int *)algorithm_find_if_not(arr, arraySize, sizeof(int), is_positive);
    if (found != NULL) {
        fmt_printf("First non-positive number: %d\n", *found);
    } 
    else {
        fmt_printf("All numbers are positive\n");
    }

    return 0;
}
```
**Result:**
```
First non-positive number: -1
```

---

## Example 9: `algorithm_find` with Doubles

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("Found %f\n", *found);
    } 
    else {
        fmt_printf("Not found\n");
    }

    return 0;
}
```
**Result:**
```
Found 5.500000
```

---

## Example 10: `algorithm_find_if_not` with Characters

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("First consonant: %c\n", *found);
    } 
    else {
        fmt_printf("All are vowels\n");
    }

    return 0;
}
```
**Result:**
```
First consonant: b
```

---

## Example 11: Using `algorithm_find_end` with Integers

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};
    int sub[] = {2, 3};

    int *found = (int *)algorithm_find_end(arr, 10, sizeof(int), sub, 2, sizeof(int), compare_ints);
    if (found != NULL) {
        fmt_printf("Last occurrence of subsequence found at index: %ld\n", found - arr);
    } 
    else {
        fmt_printf("Subsequence not found\n");
    }
    return 0;
}
```
**Result:**
```
Last occurrence of subsequence found at index: 8
```

---

## Example 12: Using `algorithm_find_first_of` with Characters

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char str[] = "hello world";
    char chars[] = {'a', 'e', 'i', 'o', 'u'};
    
    char *found = (char *)algorithm_find_first_of(str, strlen(str), sizeof(char), chars, 5, sizeof(char), compare_chars);
    if (found != NULL) {
        fmt_printf("First vowel found: %c\n", *found);
    } 
    else {
        fmt_printf("No vowels found\n");
    }

    return 0;
}
```
**Result:**
```
First vowel found: e
```

---

## Example 13: `algorithm_find_end` with Double Arrays

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("Last occurrence of subsequence found at index: %ld\n", found - arr);
    } 
    else {
        fmt_printf("Subsequence not found\n");
    }

    return 0;
}
```
**Result:**
```
Last occurrence of subsequence found at index: 5
```

---

## Example 14: `algorithm_find_first_of` with Integer Arrays

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int elements[] = {3, 6, 9};
    int *found = (int *)algorithm_find_first_of(arr, 10, sizeof(int), elements, 3, sizeof(int), compare_ints);

    if (found != NULL) {
        fmt_printf("First matching element: %d\n", *found);
    }
    else {
        fmt_printf("No matching elements found\n");
    }

    return 0;
}
```
**Result:**
```
First matching element: 3
```

---

## Example 15: Binary Search on Integer Array `algorithm_binary_search`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int value = 7;

    size_t found = algorithm_binary_search(arr, 10, sizeof(int), &value, compare_ints);
    if (found) {
        fmt_printf("Value %d found in the array at index %zu.\n", value, found);
    } 
    else {
        fmt_printf("Value %d not found in the array.\n", value);
    }

    return 0;
}
```
**Result:**
```
Value 7 found in the array at index 6.
```

---

## Example 16: Binary Search on Double Array `algorithm_binary_search`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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

    size_t found = algorithm_binary_search(arr, 9, sizeof(double), &value, compare_doubles);
    if (found) {
        fmt_printf("Value %.1f found in the array at index %zu.\n", value, found);
    } 
    else {
        fmt_printf("Value %.1f not found in the array.\n", value);
    }

    return 0;
}
```
**Result:**
```
Value 4.4 found in the array at index 3.
```

---

## Example 17: Max Element in Integer Array

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 5, 3, 7, 9, 2};
    int *max = (int *)algorithm_max_element(arr, 6, sizeof(int), compare_ints);

    if (max) {
        fmt_printf("Max element is: %d\n", *max);
    }
    
    return 0;
}
```
**Result:**
```
Max element is: 9
```

---

## Example 18: Min Element in Double Array

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("Min element is: %.1f\n", *min);
    }

    return 0;
}
```
**Result:**
```
Min element is: 1.1
```

---

## Example 19: Max Element in Char Array

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char arr[] = {'a', 'd', 'c', 'b'};
    char *max = (char *)algorithm_max_element(arr, 4, sizeof(char), compare_chars);

    if (max) {
        fmt_printf("Max character is: %c\n", *max);
    }

    return 0;
}
```
**Result:**
```
Max character is: d
```

---

## Example 20: Min Element in Custom Struct Array

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("Min struct value is: %.1f\n", min->value);
    }

    return 0;
}
```
**Result:**
```
Min struct value is: 2.1
```

---

## Example 21 : `algorithm_for_each` using char Array .. to_uppercase 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
    fmt_printf("%s\n", str);

    return 0;
}
```
**Result:**
```
HELLO WORLD
```

---

## Example 22 : Using `algorith_for_each` with Integer Array

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void print_int(void *element) {
    fmt_printf("%d ", *(int *)element);
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    algorithm_for_each(arr, 5, sizeof(int), print_int);
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
1 2 3 4 5
```

---

## Example 23 : `algorithm_copy` char* Array 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

int main() {
    const char *source = "Hello, World!";
    char dest[50];

    algorithm_copy(source, strlen(source) + 1, sizeof(char), dest);
    fmt_printf("Copied string: %s\n", dest);

    return 0;
}
```
**Result:**
```
Copied string: Hello, World!
```

---

## Example 24 : copy Integer Array `algorithm_copy`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    int source[] = {1, 2, 3, 4, 5};
    int dest[5];

    algorithm_copy(source, 5, sizeof(int), dest);
    
    for (int i = 0; i < 5; ++i) {
        fmt_printf("%d ", dest[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
1 2 3 4 5 
```

---

## Example 25 : Copy Vector Using `algorithm_copy`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *sourceVec = vector_create(sizeof(int));
    Vector *destVec = vector_create(sizeof(int));

    int values[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        vector_push_back(sourceVec, &values[i]);
    }

    algorithm_copy(sourceVec->items, vector_size(sourceVec), sizeof(int), destVec->items);
    destVec->size = vector_size(sourceVec);

    for (size_t i = 0; i < vector_size(destVec); ++i) {
        int *item = (int *)vector_at(destVec, i);
        fmt_printf("%d ", *item);
    }
    fmt_printf("\n");

    vector_deallocate(sourceVec);
    vector_deallocate(destVec);

    return 0;
}
```
**Result:**
```
1 2 3 4 5 
```

---

## Example 26 : copy String object by `algorithm_copy`

```c
#include "algorithm/algorithm.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *source = string_create("Hello C Programmers");
    String *dest = string_create("");

    fmt_printf("Source is -> %s\n", string_c_str(source));
    
    string_reserve(dest, string_length(source));
    algorithm_copy(string_c_str(source), string_length(source) + 1, sizeof(char), dest->dataStr);

    const char* value = string_c_str(dest);
    fmt_printf("Destination is -> %s\n", value);

    string_deallocate(source);
    string_deallocate(dest);

    return 0;
}
```
**Result:**
```
Source is  Hello C Programmers
Destination is Hello C Programmers
```

---

## Example 27 : sum of Integers Using `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void sum_ints(void *result, const void *element) {
    *(int *)result += *(const int *)element;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int sum = 0;

    algorithm_accumulate(arr, 5, sizeof(int), &sum, sum_ints);
    fmt_printf("Sum of array: %d\n", sum);

    return 0;
}
```
**Result:**
```
Sum of array: 15
```

## Example 28 : Concatenate char* using `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
    fmt_printf("Concatenated string: %s\n", result);

    return 0;
}
```
**Result:**
```
Concatenated string: Hello, World!
```

---

## Example 29 : Multiply Elements of Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void multiply_ints(void *result, const void *element) {
    *(int *)result *= *(const int *)element;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int product = 1;

    algorithm_accumulate(arr, 5, sizeof(int), &product, multiply_ints);
    fmt_printf("Product of array elements: %d\n", product);

    return 0;
}
```
**Result:**
```
Product of array elements: 120
```

---

## Example 30 : Find Maximum element in double Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void max_double(void *result, const void *element) {
    if (*(double *)result < *(const double *)element) {
        *(double *)result = *(const double *)element;
    }
}

int main() {
    double arr[] = {1.5, 2.7, 3.3, 4.4, 5.1};
    double max = 0.0;

    algorithm_accumulate(arr, 5, sizeof(double), &max, max_double);
    fmt_printf("Maximum element: %f\n", max);
    
    return 0;
}
```
**Result:**
```
Maximum element: 5.100000
```

---

## Example 31 : compute average of float Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void accumulate_floats(void *result, const void *element) {
    *(float *)result += *(const float *)element;
}

int main() {
    float arr[] = {1.2f, 2.3f, 3.4f, 4.5f, 5.6f};
    float sum = 0.0f;

    algorithm_accumulate(arr, 5, sizeof(float), &sum, accumulate_floats);
    float average = sum / 5;

    fmt_printf("Average of array elements: %f\n", average);
    return 0;
}
```
**Result:**
```
Average of array elements: 3.400000
```

---

## Example 32 : Counting Occurence of a Specific element in Array 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void count_occurrences(void *result, const void *element) {
    if (*(const int *)element == 3) {
        (*(int *)result)++;
    }
}

int main() {
    int arr[] = {1, 3, 3, 4, 3, 5, 3};
    int count = 0;

    algorithm_accumulate(arr, 7, sizeof(int), &count, count_occurrences);
    fmt_printf("Number of occurrences of 3: %d\n", count);

    return 0;
}
```
**Result:**
```
Number of occurrences of 3: 4
```

---

## Example 33 : Building String from char Array `algorithm_accumulate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
    fmt_printf("Constructed string: %s\n", str);

    return 0;
}
```
**Result:**
```
Constructed string: Hello
```

---

## Example 34: Check if All Integers are Positive `algorithm_all_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_positive(const void *a) {
    return *(const int *)a > 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    if (algorithm_all_of(arr, 5, sizeof(int), is_positive)) {
        fmt_printf("All elements are positive.\n");
    } 
    else {
        fmt_printf("Not all elements are positive.\n");
    }

    return 0;
}
```
**Result:**
```
All elements are positive.
```

---

## Example 35: Check if All Characters are Uppercase `algorithm_all_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <ctype.h>

bool is_uppercase(const void *ch) {
    return isupper(*(const char *)ch);
}

int main() {
    char str[] = "HELLO WORLD";

    if (algorithm_all_of(str, sizeof(str) - 1, sizeof(char), is_uppercase)) {
        fmt_printf("All characters are uppercase.\n");
    } 
    else {
        fmt_printf("Not all characters are uppercase.\n");
    }

    return 0;
}
```
**Result:**
```
Not all characters are uppercase.
```

---

## Example 36: Check if All Values in a Custom Struct Array are Above a Threshold `algorithm_all_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("All struct elements have values above 10.0.\n");
    } 
    else {
        fmt_printf("Not all struct elements have values above 10.0.\n");
    }

    return 0;
}
```
**Result:**
```
All struct elements have values above 10.0.
```

---

## Example 37: Check if Any Integer is Negative `algorithm_any_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_negative(const void *a) {
    return *(const int *)a < 0;
}

int main() {
    int arr[] = {1, -2, 3, 4, 5};

    if (algorithm_any_of(arr, 5, sizeof(int), is_negative)) {
        fmt_printf("At least one element is negative.\n");
    } 
    else {
        fmt_printf("No negative elements found.\n");
    }

    return 0;
}
```
**Result:**
```
At least one element is negative.
```

---

## Example 38: Check if Any Character is a Digit `algorithm_any_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <ctype.h>

bool is_digit(const void *ch) {
    return isdigit(*(const char *)ch);
}

int main() {
    char str[] = "Hello World 2024";

    if (algorithm_any_of(str, sizeof(str) - 1, sizeof(char), is_digit)) {
        fmt_printf("At least one character is a digit.\n");
    } 
    else {
        fmt_printf("No digits found.\n");
    }

    return 0;
}
```
**Result:**
```
At least one character is a digit.
```

---

## Example 39: Check if Any Element in a Custom Struct Array Has a Specific ID `algorithm_any_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("At least one struct element has ID 3.\n");
    } 
    else {
        fmt_printf("No struct element with ID 3 found.\n");
    }

    return 0;
}
```
**Result:**
```
At least one struct element has ID 3.
```

---

## Example 40: Check if No Integer is Negative `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_negative(const void *a) {
    return *(const int *)a < 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    if (algorithm_none_of(arr, 5, sizeof(int), is_negative)) {
        fmt_printf("No elements are negative.\n");
    } 
    else {
        fmt_printf("There are negative elements.\n");
    }

    return 0;
}
```
**Result:**
```
No elements are negative.
```

---

## Example 41: Check if No Character is a Vowel `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_vowel(const void *ch) {
    char c = *(const char *)ch;
    return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

int main() {
    char str[] = "bcdfg";

    if (algorithm_none_of(str, sizeof(str) - 1, sizeof(char), is_vowel)) {
        fmt_printf("No vowels in the string.\n");
    } 
    else {
        fmt_printf("There are vowels in the string.\n");
    }

    return 0;
}
```
**Result:**
```
No vowels in the string.
```

---

## Example 42: Check if No Element in an Array is Zero `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_zero(const void *a) {
    return *(const int *)a == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};

    if (algorithm_none_of(arr, 5, sizeof(int), is_zero)) {
        fmt_printf("No elements are zero.\n");
    } 
    else {
        fmt_printf("There are zero elements.\n");
    }

    return 0;
}
```
**Result:**
```
No elements are zero.
```

---

## Example 43: Check if No Struct Has a Specific Value `algorithm_none_of`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("No struct elements have the value 10.0.\n");
    } 
    else {
        fmt_printf("Some struct elements have the value 10.0.\n");
    }

    return 0;
}
```
**Result:**
```
No struct elements have the value 10.0.
```

---

## Example 44 : Using `algorithm_sort` for benchmarking with C++
`gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c .\algorithm\algorithm.c`
C Algorithm sort time: 0.004748 seconds

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
    fmt_printf("%d\n", *(const int*)number);
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
    fmt_printf("C Algorithm sort time: %f seconds\n", timeTaken);

    // algorithm_for_each(data, 100000, sizeof(int), printInteger);

    free(data);
    return 0;
}
```
**Result in C:**
```
C Algorithm sort time: 0.016241 seconds
```

`C++`

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
**Result in C++:**
```
C++ STL sort time: 0.0192416 seconds
```

---

## Example 45: Filling an Array of Integers `algorithm_fill`

```c
#include "algorithm.h"
#include "fmt/fmt.h"

int main() {
    int array[10];
    int value = 7;

    algorithm_fill(array, array + 10, sizeof(array[0]), &value);

    for (int i = 0; i < 10; ++i) {
        fmt_printf("%d ", array[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
7 7 7 7 7 7 7 7 7 7
```

---

## Example 46: Partially Filling an Array of Doubles `algorithm_fill_n`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    double array[10] = {0};
    double value = 3.14;

    algorithm_fill_n(array, 5, sizeof(array[0]), &value);

    for (int i = 0; i < 10; ++i) {
        fmt_printf("%.2f ", array[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
3.14 3.14 3.14 3.14 3.14 0.00 0.00 0.00 0.00 0.00
```

---

## Example 47: Filling a String with a Character `algorithm_fill`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    char str[11];
    char ch = 'A';

    algorithm_fill(str, str + 10, sizeof(char), &ch);
    str[10] = '\0'; // Null-terminate the string

    fmt_printf("%s\n", str);

    return 0;
}
```
**Result:**
```
AAAAAAAAAA
```

---

## Example 48: Filling a Struct Array `algorithm_fill`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

typedef struct {
    int id;
    double value;
} Item;

int main() {
    Item items[5];
    Item fillItem = {1, 9.99};

    algorithm_fill(items, items + 5, sizeof(Item), &fillItem);

    for (int i = 0; i < 5; ++i) {
        fmt_printf("Item %d: id = %d, value = %.2f\n", i, items[i].id, items[i].value);
    }

    return 0;
}
```
**Result:**
```
Item 0: id = 1, value = 9.99
Item 1: id = 1, value = 9.99
Item 2: id = 1, value = 9.99
Item 3: id = 1, value = 9.99
Item 4: id = 1, value = 9.99
```

---

## Example 49: Partially Filling an Array of Chars with Different Characters `algorithm_fill_n`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    char letters[26];
    char firstHalf = 'A';
    char secondHalf = 'N';

    algorithm_fill_n(letters, 13, sizeof(char), &firstHalf);
    algorithm_fill_n(letters + 13, 13, sizeof(char), &secondHalf);

    for (int i = 0; i < 26; ++i) {
        fmt_printf("%c ", letters[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
A A A A A A A A A A A A A N N N N N N N N N N N N N 
```

---

## Example 50: Counting Integers Equal to a Given Value `algorithm_count`

```c
#include "fmt/fmt.h"
#include "algorithm/algorithm.h"

int compare_ints(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

int main() {
    int arr[] = {1, 2, 3, 4, 3, 2, 1};
    int value = 3;

    size_t count = algorithm_count(arr, 7, sizeof(int), &value, compare_ints);
    fmt_printf("Count of 3: %zu\n", count);

    return 0;
}
```
**Result:**
```
Count of 3: 2
```

---

## Example 51: Counting Characters in a String `algorithm_count`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_chars(const void *a, const void *b) {
    return *(const char *)a - *(const char *)b;
}

int main() {
    char str[] = "hello world";
    char ch = 'l';

    size_t count = algorithm_count(str, sizeof(str) - 1, sizeof(char), &ch, compare_chars); 
    fmt_printf("Count of 'l': %zu\n", count);

    return 0;
}
```
**Result:**
```
Count of 'l': 3
```

---

## Example 52: Counting Even Numbers in an Array `algorithm_count_if`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdbool.h>

bool is_even(const void *a) {
    return *(const int *)a % 2 == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    size_t count = algorithm_count_if(arr, 10, sizeof(int), is_even);
    fmt_printf("Count of even numbers: %zu\n", count);

    return 0;
}
```
**Result:**
```
Count of even numbers: 5
```

---

## Example 53: Counting Elements Greater Than a Certain Value `algorithm_count_if`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdbool.h>

bool is_greater_than_five(const void *a) {
    return *(const int *)a > 5;
}

int main() {
    int arr[] = {3, 7, 2, 9, 5, 6};

    size_t count = algorithm_count_if(arr, 6, sizeof(int), is_greater_than_five);
    fmt_printf("Count of numbers greater than 5: %zu\n", count);

    return 0;
}
```
**Result:**
```
Count of numbers greater than 5: 3
```

---

## Example 54: Shuffling an Array of Characters `algorithm_shuffle`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>

uint32_t simple_rng() {
    return rand(); 
}

int main() {
    srand((unsigned int)time(NULL));  
    char str[] = "HelloWorld";
    size_t size = sizeof(str) - 1;

    algorithm_shuffle(str, size, sizeof(char), simple_rng);

    fmt_printf("Shuffled String: %s\n", str);

    return 0;
}
```
**Result:**
```
Shuffled String: dWHollelro
```

---

## Example 55: Shuffling an Array of Structs `algorithm_shuffle`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>

typedef struct {
    int id;
    char name[10];
} Person;

uint32_t simple_rng() {
    return rand();  
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

    fmt_printf("Shuffled People:\n");
    for (size_t i = 0; i < size; ++i) {
        fmt_printf("ID: %d, Name: %s\n", people[i].id, people[i].name);
    }

    return 0;
}
```
**Result:**
```
Shuffled People:
ID: 5, Name: Eve
ID: 1, Name: Alice
ID: 3, Name: Charlie
ID: 4, Name: Dave
ID: 2, Name: Bob
```

---

## Example 56 : Usinf Shuffle in Array of integer `algorithm_shuffle`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("%d ", data[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
10 9 7 4 8 1 6 3 5 2
```

---

## Example 57: Find Lower Bound in an Array of Integers `algorithm_lower_bound`
```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 3, 5, 7, 9};
    int val = 4;

    int *lb = (int *)algorithm_lower_bound(arr, 5, sizeof(int), &val, compare_ints);
    fmt_printf("Lower bound of 4 is at index: %lld\n", lb - arr);

    return 0;
}
```
**Result:**
```
Lower bound of 4 is at index: 2
```

---

## Example 58: Find Upper Bound in an Array of Integers `algorithm_upper_bound`
```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 3, 5, 7, 9};
    int val = 6;

    int *ub = (int *)algorithm_upper_bound(arr, 5, sizeof(int), &val, compare_ints);
    fmt_printf("Upper bound of 6 is at index: %lld\n", ub - arr);

    return 0;
}
```
**Result:**
```
Upper bound of 6 is at index: 3
```

---

## Example 59: Lower Bound in an Array of Doubles `algorithm_lower_bound`
```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_doubles(const void *a, const void *b) {
    const double diff = *(const double*)a - *(const double*)b;
    return (diff < 0) ? -1 : (diff > 0);
}

int main() {
    double arr[] = {1.1, 3.3, 5.5, 7.7, 9.9};
    double val = 4.4;

    double *lb = (double *)algorithm_lower_bound(arr, 5, sizeof(double), &val, compare_doubles);
    fmt_printf("Lower bound of 4.4 is at index: %lld\n", lb - arr);

    return 0;
}
```
**Result:**
```
Lower bound of 4.4 is at index: 2
```

---

## Example 60: Upper Bound in an Array of Characters `algorithm_upper_bound`
```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char arr[] = {'a', 'c', 'e', 'g', 'i'};
    char val = 'f';

    char *ub = (char *)algorithm_upper_bound(arr, 5, sizeof(char), &val, compare_chars);
    fmt_printf("Upper bound of 'f' is at index: %lld\n", ub - arr);

    return 0;
}
```
**Result:**
```
Upper bound of 'f' is at index: 3
```

---

## Example 61 : `algorithm_transform`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("%d ", outputArray[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
1 4 9 16 25 
```

---

## Example 62: Negating Integers `algorithm_transform`

This example negates each integer in an array.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("%d ", outputArray[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
-1 2 -3 4 -5 
```

---

### Example 63: Converting Floats to Integers `algorithm_transform`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
        fmt_printf("%d ", outputArray[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
1 -2 3 -4 5
```

---

### Example 64: Computing Lengths of Strings `algorithm_transform`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
        fmt_printf("%zu ", outputArray[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
5 5 7 1 11 
```

---

## Example 65 : sum numbers with `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void sum(void *result, const void *element) {
    *(int *)result += *(const int *)element;
}

int main() {
    int array[] = {1, 2, 3, 4, 5};
    int sum_result = 0;
    size_t numElements = sizeof(array) / sizeof(array[0]);

    algorithm_reduce(array, numElements, sizeof(int), &sum_result, sum);
    fmt_printf("Sum: %d\n", sum_result);
    
    return 0;
}
```
**Result:**
```
Sum: 15
```

---

## Example 66: Finding the Maximum Value in an Array of Integers `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
    fmt_printf("Maximum Value: %d\n", maxResult);

    return 0;
}
```
**Result:**
```
Maximum Value: 9
```

---

### Example 67: Concatenating Strings `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

void concatenate_strings(void *result, const void *element) {
    strcat((char *)result, *(const char **)element);
}

int main() {
    const char *strings[] = {"Hello", ", ", "world", "!"};
    size_t numElements = sizeof(strings) / sizeof(strings[0]);
    char concatenated[50] = "";

    algorithm_reduce(strings, numElements, sizeof(char *), concatenated, concatenate_strings);

    fmt_printf("Concatenated String: %s\n", concatenated);

    return 0;
}
```
**Result:**
```
Concatenated String: Hello, world!
```

---

### Example 68: Computing the Logical AND of Boolean Values `algorithm_reduce`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
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
    fmt_printf("Logical AND of flags: %s\n", andResult ? "true" : "false");

    return 0;
}
```
**Result:**
```
Logical AND of flags: false
```

---

## Example 69: Unique Integers in an Array `algorithm_unique`

This example demonstrates removing consecutive duplicates in an array of integers.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

int main() {
    int arr[] = {1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 4};
    size_t arrSize = sizeof(arr) / sizeof(arr[0]);

    size_t newSize = algorithm_unique(arr, arrSize, sizeof(int), compare_ints);

    fmt_printf("Unique elements: ");
    for (size_t i = 0; i < newSize; i++) {
        fmt_printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Unique elements: 1 2 3 4
```

---

## Example 70: Unique Characters in a String `algorithm_unique`

This example focuses on removing consecutive duplicate characters in a string.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

int main() {
    char str[] = "aabbbcdddeeeffg";
    size_t strSize = sizeof(str) - 1; // Exclude null terminator

    size_t newSize = algorithm_unique(str, strSize, sizeof(char), compare_chars);

    fmt_printf("Unique characters: ");
    for (size_t i = 0; i < newSize; i++) {
        fmt_printf("%c", str[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Unique characters: abcdefg
```

---

## Example 71: Unique Floats in an Array `algorithm_unique`

This example demonstrates the removal of consecutive duplicate floating-point numbers in an array.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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

    fmt_printf("Unique floats: ");
    for (size_t i = 0; i < newSize; i++) {
        fmt_printf("%.1f ", arr[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Unique floats: 1.1 2.2 3.3 4.4 5.5 
```

---

## Example 72: Comparing Integer Arrays `algorithm_equal`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
    fmt_printf("Arr1 is equal to Arr2: %s\n", isEqual ? "true" : "false");

    isEqual = algorithm_equal(arr1, 5, sizeof(int), arr3, 5, sizeof(int), compare_ints);
    fmt_printf("Arr1 is equal to Arr3: %s\n", isEqual ? "true" : "false");

    return 0;
}
```
**Result:**
```
Arr1 is equal to Arr2: true
Arr1 is equal to Arr3: false
```

---

### Example 73: Comparing Character Arrays `algorithm_equal`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
    fmt_printf("Str1 is equal to Str2: %s\n", isEqual ? "true" : "false");

    isEqual = algorithm_equal(str1, 5, sizeof(char), str3, 5, sizeof(char), compare_chars);
    fmt_printf("Str1 is equal to Str3: %s\n", isEqual ? "true" : "false");

    return 0;
}
```
**Result:**
```
Str1 is equal to Str2: true
Str1 is equal to Str3: false
```

---

### Example 74: Comparing Double Arrays `algorithm_equal`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
    fmt_printf("Arr1 is equal to Arr2: %s\n", isEqual ? "true" : "false");

    isEqual = algorithm_equal(arr1, 3, sizeof(double), arr3, 3, sizeof(double), compare_doubles);
    fmt_printf("Arr1 is equal to Arr3: %s\n", isEqual ? "true" : "false");

    return 0;
}
```
**Result:**
```
Arr1 is equal to Arr2: true
Arr1 is equal to Arr3: false
```

---

## Example 75 : Next Permutation of Integer Array `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    int arr[] = {1, 2, 3};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    do {
        for (size_t i = 0; i < size; ++i) {
            fmt_printf("%d ", arr[i]);
        }
        fmt_printf("\n");
    } while (algorithm_next_permutation(arr, arr + size, sizeof(arr[0]), less_int));

    return 0;
}
```
**Result:**
```
1 2 3 
1 3 2
2 1 3
2 3 1
3 1 2
3 2 1
```

---

## Example 76 : Using with Character `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

bool less_char(const void *a, const void *b) {
    return *(char *)a < *(char *)b;
}

int main() {
    char str[] = "abc";
    size_t size = strlen(str);

    do {
        fmt_printf("%s\n", str);
    } while (algorithm_next_permutation(str, str + size, sizeof(char), less_char));

    return 0;
}
```
**Result:**
```
abc
acb
bac
bca
cab
cba
```

---

## Example 77 : With Custom Structures `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
            fmt_printf("{%d, %.1f} ", arr[i].id, arr[i].value);
        }
        fmt_printf("\n");
    } while (algorithm_next_permutation(arr, arr + size, sizeof(MyStruct), less_mystruct));

    return 0;
}
```
**Result:**
```
{1, 10.5} {2, 20.5} {3, 30.5} 
{1, 10.5} {3, 30.5} {2, 20.5}
{2, 20.5} {1, 10.5} {3, 30.5}
{2, 20.5} {3, 30.5} {1, 10.5}
{3, 30.5} {1, 10.5} {2, 20.5}
{3, 30.5} {2, 20.5} {1, 10.5}
```

---

## Example 78 : Using with Vector Lib `algorithm_next_permutation`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

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
            fmt_printf("%d ", *item);
        }
        fmt_printf("\n");
        
    } while (algorithm_next_permutation(vector_begin(vec), vector_end(vec), sizeof(int), less_int));

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
1 2 3 
1 3 2
2 1 3
2 3 1
3 1 2
3 2 1
```

---

### Example 79: Prev Permutation of Integer Array `algorithm_prev_permutation`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    int arr[] = {3, 2, 1};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    do {
        for (size_t i = 0; i < size; ++i) {
            fmt_printf("%d ", arr[i]);
        }
        fmt_printf("\n");
    } while (algorithm_prev_permutation(arr, arr + size, sizeof(int), less_int));

    return 0;
}
```
**Result:**
```
3 2 1 
3 1 2
2 3 1
2 1 3
1 3 2
1 2 3
```

---

### Example 80: Prev Permutation with Characters `algorithm_prev_permutation`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

bool less_char(const void *a, const void *b) {
    return *(char *)a < *(char *)b;
}

int main() {
    char str[] = "cba";
    size_t size = strlen(str);

    do {
        fmt_printf("%s\n", str);
    } while (algorithm_prev_permutation(str, str + size, sizeof(char), less_char));

    return 0;
}
```
**Result:**
```
cba
cab
bca
bac
acb
abc
```

---

### Example 81: Using with Vector Library `algorithm_prev_permutation`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

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
            fmt_printf("%d ", *item);
        }
        fmt_printf("\n");
    } while (algorithm_prev_permutation(vector_begin(vec), vector_end(vec), sizeof(int), less_int));

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
3 2 1 
3 1 2
2 3 1
2 1 3
1 3 2
1 2 3
```

---

## Example 82 : Partitioning an Array of Integers `algorithm_partition`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_odd(const void *a) {
    return (*(int *)a) % 2 != 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    char *partition_point = (char *)algorithm_partition(arr, size, sizeof(arr[0]), is_odd);

    fmt_printf("Odd elements: ");
    for (char *ptr = (char *)arr; ptr != partition_point; ptr += sizeof(arr[0])) {
        fmt_printf("%d ", *(int *)ptr);
    }
    fmt_printf("\n");

    fmt_printf("Even elements: ");
    for (char *ptr = partition_point; ptr != (char *)arr + size * sizeof(arr[0]); ptr += sizeof(arr[0])) {
        fmt_printf("%d ", *(int *)ptr);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result in C:**
```
Odd elements: 1 9 3 7 5 
Even elements: 6 4 8 2
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
**Result in C++:**
```
odd elements: 1 9 3 7 5 
even elements: 6 4 8 2
```

---

## Example 83 : Partitioning a String Based on Uppercase Characters `algorithm_partition`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <ctype.h>

bool is_uppercase(const void *a) {
    return isupper(*(char *)a);
}

int main() {
    char str[] = "HelloWorld";
    size_t size = sizeof(str) - 1;

    char *partition_point = (char *)algorithm_partition(str, size, sizeof(char), is_uppercase);

    fmt_printf("Uppercase characters: ");
    for (char *ptr = str; ptr != partition_point; ptr += sizeof(char)) {
        fmt_printf("%c", *ptr);
    }
    fmt_printf("\n");

    fmt_printf("Other characters: ");
    for (char *ptr = partition_point; ptr != str + size * sizeof(char); ptr += sizeof(char)) {
        fmt_printf("%c", *ptr);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Uppercase characters: HW
Other characters: lloeorld
```

---

## Example 85 : Partitioning an Array of Structures Based on a Positive Value `algorithm_partition`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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

    fmt_printf("Positive values: ");
    for (char *ptr = (char *)arr; ptr != partition_point; ptr += sizeof(MyStruct)) {
        MyStruct *ms = (MyStruct *)ptr;
        fmt_printf("{%d, %.1f} ", ms->id, ms->value);
    }
    fmt_printf("\n");

    fmt_printf("Non-positive values: ");
    for (char *ptr = partition_point; ptr != (char *)arr + size * sizeof(MyStruct); ptr += sizeof(MyStruct)) {
        MyStruct *ms = (MyStruct *)ptr;
        fmt_printf("{%d, %.1f} ", ms->id, ms->value);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Positive values: {4, 40.5} {2, 20.5} 
Non-positive values: {3, -30.5} {1, -10.5}
```

---

## Example 86: Generating Random Numbers `algorithm_generate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>

// Generator function to fill with random numbers
void random_number_generator(void *output) {
    *(int *)output = rand() % 100; // Generate a random number between 0 and 99
}

int main() {
    int array[10];
    algorithm_generate(array, array + 10, sizeof(int), random_number_generator);

    fmt_printf("Random numbers: ");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%d ", array[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Random numbers: 41 67 34 0 69 24 78 58 62 64 
```

---

## Example 87: Generating Sequential Numbers `algorithm_generate`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

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

    fmt_printf("Sequential numbers: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%d ", *(int *)vector_at(vec, i));
    }
    fmt_printf("\n");

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Sequential numbers: 0 1 2 3 4 5 6 7 8 9
```

---

### Example 88: Filling a Vector with Constant Value `algorithm_generate`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

// Generator function to fill with a constant value
void constant_generator(void *output) {
    static const int constant_value = 5;
    *(int *)output = constant_value;
}

int main() {
    Vector *vec = vector_create(sizeof(int));
    vector_resize(vec, 10); // Resize vector to hold 10 elements

    algorithm_generate(vector_begin(vec), vector_end(vec), sizeof(int), constant_generator);

    fmt_printf("Constant values: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%d ", *(int *)vector_at(vec, i));
    }
    fmt_printf("\n");

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Constant values: 5 5 5 5 5 5 5 5 5 5 
```

---

### Example 89: Generating a Fixed Number of Random Integers `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>

void random_int_generator(void *output) {
    *(int *)output = rand() % 100;
}

int main() {
    int array[5];
    algorithm_generate_n(array, 5, sizeof(int), random_int_generator);

    fmt_printf("Random integers: ");
    for (int i = 0; i < 5; ++i) {
        fmt_printf("%d ", array[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Random integers: 41 67 34 0 69 
```

---

### Example 90: Generating a Sequence of Characters `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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

    fmt_printf("Character sequence: ");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%c ", sequence[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Character sequence: A B C D E F G H I J 
```

---

### Example 91: Generating a String Array `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include "string/std_string.h"
#include "fmt/fmt.h"
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

    fmt_printf("Generated strings: \n");
    for (int i = 0; i < 3; ++i) {
        fmt_printf("%s\n", stringArray[i]->dataStr);
        string_deallocate(stringArray[i]);
    }

    return 0;
}
```
**Result:**
```
Generated strings: 
String 1
String 2
String 3
```

---

### Example 92: Generating Floats in a Vector `algorithm_generate_n`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

void float_generator(void *output) {
    static float value = 0.5;
    *(float *)output = value;
    value += 0.5;
}

int main() {
    Vector *vec = vector_create(sizeof(float));
    vector_resize(vec, 5);

    algorithm_generate_n(vector_begin(vec), 5, sizeof(float), float_generator);

    fmt_printf("Generated floats: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%.2f ", *(float *)vector_at(vec, i));
    }
    fmt_printf("\n");

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Generated floats: 0.50 1.00 1.50 2.00 2.50 
```

---

## Example 93 : `algorithm_copy_backward` how to use it 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t array_size = sizeof(array) / sizeof(array[0]);

    // Copy elements in the middle of the array to the end
    // The result pointer should point just past the last element of the array
    algorithm_copy_backward(array + 3, array + 7, sizeof(int), array + array_size);

    // Print the array
    fmt_printf("Array after copy_backward: ");
    for (size_t i = 0; i < array_size; ++i) {
        fmt_printf("%d ", array[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Array after copy_backward: 1 2 3 4 5 6 4 5 6 7 
```

---

## Example 94 : use Vector with `algorithm_copy_backward`

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void print_vector(Vector *vec) {
    fmt_printf("Destination vector after copy_backward: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int *value = (int *)vector_at(vec, i);
        fmt_printf("%d ", *value);
    }
    fmt_printf("\n");
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
**Result:**
```
Destination vector after copy_backward: 1 2 3 4 5 
```

---

## Example 95 : Using `algorithm_copy_backward` with a Struct 
We'll define a simple struct, say Person, with a couple of fields, and then use `algorithm_copy_backward` to copy elements of an array of Person structs.

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    int age;
} Person;

void print_people_array(Person *people, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        fmt_printf("Name: %s, Age: %d\n", people[i].name, people[i].age);
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

    fmt_printf("Destination array after copy_backward:\n");
    print_people_array(destination, people_size);

    return 0;
}
```
**Result:**
```
Destination array after copy_backward:
Name: Alice, Age: 30
Name: Bob, Age: 25
Name: Carol, Age: 27
Name: Dave, Age: 32
Name: Eve, Age: 29
```

---

## Example 96 : Using `algorithm_copy_if` with Integer Array 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_positive(const void *element) {
    const int *value = (const int *)element;
    return *value > 0;
}

int main() {
    int source[] = {1, -2, 3, -4, 5};
    int dest[5] = {0}; // should be initialize 
    size_t num_elements = sizeof(source) / sizeof(source[0]);

    algorithm_copy_if(source, source + num_elements, sizeof(int), dest, is_positive);

    for (size_t i = 0; i < num_elements; ++i) {
        fmt_printf("%d ", dest[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
1 3 5 0 0 
```

---

## Example 97 : Check number is_even `algorithm_copy_if`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_even(const void *element) {
    const int *value = (const int *)element;
    return (*value % 2) == 0;
}

int main() {
    int source[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int dest[10] = {0}; 
    size_t num_elements = sizeof(source) / sizeof(source[0]);

    algorithm_copy_if(source, source + num_elements, sizeof(int), dest, is_even);

    fmt_printf("Even numbers: ");
    for (size_t i = 0; i < num_elements; ++i) {
        if (dest[i] != 0) {
            fmt_printf("%d ", dest[i]);
        }
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Even numbers: 2 4 6 8 10
```

---

## Example 98 : Using `algorithm_generate` and `algorithm_copy_if` with Vector

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>


void int_generator(void *output) {
    static int value = 0;
   *(int *)output = value++;
}

bool greater_than_five(const void *element) {
    const int *value = (const int *)element;
    return *value > 5;
}

void print_vector(Vector *vec) {
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int *value = (int *)vector_at(vec, i);
        if (*value != 0) {
            fmt_printf("%d ", *value);
        }
    }
    fmt_printf("\n");
}

int main() {
    Vector *source = vector_create(sizeof(int));
    Vector *destination = vector_create(sizeof(int));

    vector_resize(source, 10);
    vector_resize(destination, 10);

    algorithm_generate(vector_begin(source), vector_end(source), sizeof(int), int_generator);
    algorithm_generate(vector_begin(destination), vector_end(destination), sizeof(int), int_generator);
    algorithm_copy_if(vector_begin(source), vector_end(source), sizeof(int), vector_begin(destination), greater_than_five);

    fmt_printf("Elements greater than 5: ");
    print_vector(destination);

    vector_deallocate(source);
    vector_deallocate(destination);

    return 0;
}
```
**Result:**
```
Elements greater than 5: 6 7 8 9 14 15 16 17 18 19 
```

---

## Example 99 : Using `algorithm_copy_n` with Integer Array

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>


int main() {
    int source[] = {10, 20, 30, 40, 50, 60, 70};
    int dest[7] = {0};

    algorithm_copy_n(source, 7, sizeof(int), dest);

    fmt_printf("dest array contains:");
    for (size_t i = 0; i < 7; ++i) {
        fmt_printf(" %d", dest[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
dest array contains: 10 20 30 40 50 60 70
```

---

### Example 100: Using `algorithm_copy_n` with an Integer Array
This example copies a specified number of elements from one integer array to another using `algorithm_copy_n`.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    int source[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int dest[5]; // Destination array for the first 5 elements

    algorithm_copy_n(source, 5, sizeof(int), dest);

    fmt_printf("First 5 elements of source: ");
    for (size_t i = 0; i < 5; ++i) {
        fmt_printf("%d ", dest[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
First 5 elements of source: 10 20 30 40 50
```

---

### Example 101: Using `algorithm_copy_n` with `Vector`
In this example, we will use `algorithm_copy_n` to copy a specified number of elements from one `Vector` to another.

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void print_vector(Vector *vec) {
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int *value = (int *)vector_at(vec, i);
        fmt_printf("%d ", *value);
    }
    fmt_printf("\n");
}

int main() {
    Vector *source = vector_create(sizeof(int));
    Vector *destination = vector_create(sizeof(int));

    for (int i = 1; i <= 10; ++i) {
        vector_push_back(source, &i);
    }

    vector_resize(destination, 5); // Allocate space for 5 elements
    algorithm_copy_n(vector_begin(source), 5, sizeof(int), vector_begin(destination));

    fmt_printf("First 5 elements from source: ");
    print_vector(destination);

    vector_deallocate(source);
    vector_deallocate(destination);

    return 0;
}
```
**Result:**
```
First 5 elements from source: 1 2 3 4 5 
```

---

## Example 102 : Using `algorithm_equal_range` with Integer Array 

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    const int arg1 = *(const int *)a;
    const int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int greater_int(const void* a, const void* b) {
    const int arg1 = *(const int*)a;
    const int arg2 = *(const int*)b;

    return (arg1 < arg2) - (arg1 > arg2);
}

int main() {
    int array[] = {10, 20, 30, 30, 20, 10, 10, 20};
    size_t num_elements = sizeof(array) / sizeof(array[0]);
    int val = 20;

    algorithm_sort(array, num_elements, sizeof(int), compare_ints);
    Pair bounds = algorithm_equal_range(array, num_elements, sizeof(int), &val, compare_ints);

    algorithm_sort(array, num_elements, sizeof(int), greater_int);
    bounds = algorithm_equal_range(array, num_elements, sizeof(int), &val, greater_int);
    
    size_t lower_bound_index = ((int *)bounds.first - array);
    size_t upper_bound_index = ((int *)bounds.second - array);

    fmt_printf("Bounds at positions %zu and %zu\n", lower_bound_index, upper_bound_index);

    return 0;
}
```
**Result in C:**
```
Bounds at positions 2 and 5
```

`Cpp code`

```cpp 
#include <iostream>     
#include <algorithm>    
#include <vector>       

bool mygreater (int i,int j) { 
    return (i>j); 
}

int main () {
  int myints[] = {10,20,30,30,20,10,10,20};
  std::vector<int> v(myints,myints+8);                         // 10 20 30 30 20 10 10 20
  std::pair<std::vector<int>::iterator,std::vector<int>::iterator> bounds;

  // using default comparison:
  std::sort (v.begin(), v.end());                              // 10 10 10 20 20 20 30 30
  bounds=std::equal_range (v.begin(), v.end(), 20);            //          ^        ^

  // using "mygreater" as comp:
  std::sort (v.begin(), v.end(), mygreater);                   // 30 30 20 20 20 10 10 10
  bounds=std::equal_range (v.begin(), v.end(), 20, mygreater); //       ^        ^

  std::cout << "bounds at positions " << (bounds.first - v.begin());
  std::cout << " and " << (bounds.second - v.begin()) << '\n';

  return 0;
}
```
**Result:**
```
bounds at positions 2 and 5
```

---

## Example 103 : `algorithm_equal_range` with Array of Doubles 

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_doubles(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    double array[] = {1.5, 2.5, 3.5, 3.5, 2.5, 1.5, 1.5, 2.5};
    size_t num_elements = sizeof(array) / sizeof(array[0]);
    double val = 2.5;

    algorithm_sort(array, num_elements, sizeof(double), compare_doubles);
    Pair bounds = algorithm_equal_range(array, num_elements, sizeof(double), &val, compare_doubles);

    size_t lower_bound_index = ((double *)bounds.first - array);
    size_t upper_bound_index = ((double *)bounds.second - array);

    fmt_printf("Bounds for 2.5 in double array: %zu and %zu\n", lower_bound_index, upper_bound_index);

    return 0;
}
```
**Result:**
```
Bounds for 2.5 in double array: 3 and 6
```

---

## Example 104 : Using Array and Vector lib in `algorithm_equal_range`

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_chars(const void *a, const void *b) {
    char arg1 = *(const char *)a;
    char arg2 = *(const char *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    char myChars[] =  {'a', 'b', 'c', 'c', 'b', 'a', 'a', 'b'};
    size_t numElements = sizeof(myChars) / sizeof(char);
    Vector *vec = vector_create(sizeof(char));
    char val = 'b';
    
    vector_resize(vec, numElements);
    algorithm_copy(myChars, numElements, sizeof(char), vector_begin(vec));
    algorithm_sort(vector_begin(vec), vector_size(vec), sizeof(char), compare_chars);

    Pair bounds = algorithm_equal_range(vector_begin(vec), vector_size(vec), sizeof(char), &val, compare_chars);
    size_t lower_bound_index = ((char *)bounds.first - (char *)vector_begin(vec));
    size_t upper_bound_index = ((char *)bounds.second - (char *)vector_begin(vec));

    fmt_printf("Bounds for 'b' in char array: %zu and %zu\n", lower_bound_index, upper_bound_index);

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Bounds for 'b' in char array: 3 and 6
```

---

## Example 105 : Using `algorithm_includes` with Integer Array 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    int container[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    int continent[] = {40, 30, 20, 10};
    size_t container_size = sizeof(container) / sizeof(container[0]);
    size_t continent_size = sizeof(continent) / sizeof(continent[0]);

    algorithm_sort(container, container_size, sizeof(int), compare_ints);
    algorithm_sort(continent, continent_size, sizeof(int), compare_ints);

    // Using algorithm_includes
    if (algorithm_includes(container, container_size, sizeof(int), continent, continent_size, sizeof(int), compare_ints)) {
        fmt_printf("container includes continent!\n");
    } 
    else {
        fmt_printf("container does not include continent.\n");
    }

    return 0;
}
```
**Result:**
```
container includes continent!
```

---

### Example 106: Checking Inclusion of a Subset of Characters

This example checks if a subset of characters is included in a larger set of characters.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_chars(const void *a, const void *b) {
    char arg1 = *(const char *)a;
    char arg2 = *(const char *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    char letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
    char subset[] = {'b', 'f', 'i'};
    size_t letters_size = sizeof(letters) / sizeof(letters[0]);
    size_t subset_size = sizeof(subset) / sizeof(subset[0]);

    algorithm_sort(letters, letters_size, sizeof(char), compare_chars);
    algorithm_sort(subset, subset_size, sizeof(char), compare_chars);

    if (algorithm_includes(letters, letters_size, sizeof(char), subset, subset_size, sizeof(char), compare_chars)) {
        fmt_printf("letters array includes the subset!\n");
    } 
    else {
        fmt_printf("letters array does not include the subset.\n");
    }

    return 0;
}
```
**Result:**
```
letters array includes the subset!
```

---

### Example 107: Inclusion of Subset of Custom Structs

This example checks if a set of custom structs contains a subset based on a specific field. Assume we have a struct `Person` with `age` and `name`.

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    int age;
    char name[50];
} Person;

int compare_person_age(const void *a, const void *b) {
    Person arg1 = *(const Person *)a;
    Person arg2 = *(const Person *)b;
    return (arg1.age > arg2.age) - (arg1.age < arg2.age);
}

int main() {
    Person people[] = {{30, "Alice"}, {40, "Bob"}, {20, "Charlie"}, {50, "Diana"}};
    Person subset[] = {{20, "Charlie"}, {50, "Diana"}};
    size_t people_size = sizeof(people) / sizeof(people[0]);
    size_t subset_size = sizeof(subset) / sizeof(subset[0]);

    algorithm_sort(people, people_size, sizeof(Person), compare_person_age);
    algorithm_sort(subset, subset_size, sizeof(Person), compare_person_age);

    if (algorithm_includes(people, people_size, sizeof(Person), subset, subset_size, sizeof(Person), compare_person_age)) {
        fmt_printf("people array includes the subset!\n");
    } 
    else {
        fmt_printf("people array does not include the subset.\n");
    }

    return 0;
}
```
**Result:**
```
people array includes the subset!
```

---

## Example 108 : `algorithm_unique_copy` with and Integers Array

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

void print_int(void *element) {
    fmt_printf("%d ", *(int *)element);
}

int main() {
    int arr[] = {1, 1, 2, 2, 2, 3, 3, 4, 4, 4, 4};
    int result[11];
    size_t arrSize = sizeof(arr) / sizeof(arr[0]);

    size_t newSize = algorithm_unique_copy(arr, arrSize, sizeof(int), result, compare_ints);

    fmt_printf("Unique elements: \n");
    algorithm_for_each(result, newSize, sizeof(int), print_int);

    return 0;
}
```
**Result:**
```
Unique elements: 1 2 3 4
```

---

## Example 109 : `algorithm_unique_copy` with char* 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_chars(const void *a, const void *b) {
    return *(const char*)a - *(const char*)b;
}

void print_char(void* element) {
    fmt_printf("%c ", *(char*)element);
}

int main() {
    char str[] = "aabbbcdddeeeffg";
    char result[sizeof(str)];
    size_t strSize = sizeof(str) - 1; 

    size_t newSize = algorithm_unique_copy(str, strSize, sizeof(char), result, compare_chars);

    fmt_printf("Unique characters: ");
    algorithm_for_each(result, newSize, sizeof(char), print_char);

    return 0;
}
```
**Result:**
```
Unique characters: a b c d e f g
```

---

#### Example 110: Swapping Integers with `algoritm_swap`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    int x = 10, y = 20;
    fmt_printf("Before swap: x = %d, y = %d\n", x, y);

    algorithm_swap(&x, &y, sizeof(int));
    fmt_printf("After swap: x = %d, y = %d\n", x, y);

    return 0;
}
```
**Result:**
```
Before swap: x = 10, y = 20
After swap: x = 20, y = 10
```

---

#### Example 111: Swapping Arrays with `algorithm_swap`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void print_int(void *element) {
    fmt_printf("%d ", *(int *)element);
}

int main() {
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6};
    size_t arrSize = sizeof(arr1) / sizeof(int);

    fmt_printf("Before swap:\narr1: ");
    algorithm_for_each(arr1, arrSize, sizeof(int), print_int);

    fmt_printf("\narr2: ");
    algorithm_for_each(arr2, arrSize, sizeof(int), print_int);

    algorithm_swap(arr1, arr2, arrSize); // swap two array

    fmt_printf("\nAfter swap:\narr1: ");
    algorithm_for_each(arr1, arrSize, sizeof(int), print_int);

    fmt_printf("\narr2: ");
    algorithm_for_each(arr2, arrSize, sizeof(int), print_int);

    return 0;
}
```
**Result:**
```
Before swap:
arr1: 1 2 3 
arr2: 4 5 6
After swap:
arr1: 4 2 3
arr2: 1 5 6
```

---

#### Example 112: Swapping Ranges of Integers with `algorithm_swap_ranges`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void print_int(void *element) {
    fmt_printf("%d ", *(int *)element);
}

int main() {
    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {10, 20, 30, 40, 50};
    size_t num = 3;

    fmt_printf("Before swap:\narr1: ");
    algorithm_for_each(arr1, num, sizeof(int), print_int);

    fmt_printf("\narr2: ");
    algorithm_for_each(arr2, num, sizeof(int), print_int);

    algorithm_swap_ranges(arr1, arr2, num, sizeof(int)); //swap reanges

    fmt_printf("\nAfter swap:\narr1: ");
    algorithm_for_each(arr1, num, sizeof(int), print_int);

    fmt_printf("\narr2: ");
    algorithm_for_each(arr2, num, sizeof(int), print_int);

    return 0;
}
```
**Result:**
```
Before swap:
arr1: 1 2 3 
arr2: 10 20 30
After swap:
arr1: 10 20 30
arr2: 1 2 3
```

---

#### Example 113: Swapping Partial Ranges of Characters `algorithm_swap_ranges`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    char str1[] = "Hello";
    char str2[] = "World";
    size_t num = 3; // Swap first three characters

    fmt_printf("Before swap:\nstr1: %s\nstr2: %s\n", str1, str2);
    algorithm_swap_ranges(str1, str2, num, sizeof(char));

    fmt_printf("After swap:\nstr1: %s\nstr2: %s\n", str1, str2);

    return 0;
}
```
**Result:**
```
Before swap:
str1: Hello
str2: World
After swap:
str1: Worlo
str2: Helld
```

---

## Examples 114 : `algorithm_stable_sort` same as C++ 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <math.h>  // For fabs

int compare_doubles(const void *a, const void *b) {
    double arg1 = *(const double *)a;
    double arg2 = *(const double *)b;

    if (fabs(arg1 - arg2) < 1e-6) { // Consider floating point precision
        return 0;
    }

    return (arg1 > arg2) - (arg1 < arg2);
}

int compare_as_ints(const void *a, const void *b) {
    int arg1 = (int)*(const double *)a;
    int arg2 = (int)*(const double *)b;
    return arg1 - arg2;
}

void print_doubles(void* element) {
    fmt_printf("%.2f ", *(double*)element);
}

int main() {
    double original[] = {3.14, 1.41, 2.72, 4.67, 1.73, 1.32, 1.62, 2.58};
    double mydoubles[] = {3.14, 1.41, 2.72, 4.67, 1.73, 1.32, 1.62, 2.58};
    size_t mydoubles_size = sizeof(mydoubles) / sizeof(mydoubles[0]);

    algorithm_stable_sort(mydoubles, mydoubles_size, sizeof(double), compare_doubles);
    
    fmt_printf("using default comparison:");
    algorithm_for_each(mydoubles, mydoubles_size, sizeof(double), print_doubles);
    fmt_printf("\n");

    algorithm_copy(original, mydoubles_size, sizeof(double), mydoubles);    
    algorithm_stable_sort(mydoubles, mydoubles_size, sizeof(double), compare_as_ints);
    
    fmt_printf("using 'compare_as_ints':");
    algorithm_for_each(mydoubles, mydoubles_size, sizeof(double), print_doubles);
    fmt_printf("\n");

    return 0;
}
```
**Result in C:**
```
using default comparison:1.32 1.41 1.62 1.73 2.58 2.72 3.14 4.67 
using 'compare_as_ints':1.41 1.73 1.32 1.62 2.72 2.58 3.14 4.67 
```

`C++ Code`

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::stable_sort
#include <vector>       // std::vector

bool compare_as_ints (double i,double j) {
  return (int(i)<int(j));
}

int main () {
  double mydoubles[] = {3.14, 1.41, 2.72, 4.67, 1.73, 1.32, 1.62, 2.58};

  std::vector<double> myvector;

  myvector.assign(mydoubles,mydoubles+8);

  std::cout << "using default comparison:";
  std::stable_sort (myvector.begin(), myvector.end());
  for (std::vector<double>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  myvector.assign(mydoubles,mydoubles+8);

  std::cout << "using 'compare_as_ints' :";
  std::stable_sort (myvector.begin(), myvector.end(), compare_as_ints);
  for (std::vector<double>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
```
**Result in C++:**
```
using default comparison: 1.32 1.41 1.62 1.73 2.58 2.72 3.14 4.67
using 'compare_as_ints' : 1.41 1.73 1.32 1.62 2.72 2.58 3.14 4.67
```

---

## Example 115 : Using `algorithm_stable_sort` with Struct

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

typedef struct {
    char name[50];
    int age;
} Person;

int compare_persons_by_age(const void *a, const void *b) {
    Person *personA = (Person *)a;
    Person *personB = (Person *)b;
    
    return personA->age - personB->age;
}

void print_person(void* element) {
    Person *person = (Person*)element;
    fmt_printf("Name: %s, Age: %d\n", person->name, person->age);
}

int main() {
    Vector* people = vector_create(sizeof(Person));

    Person john = {"John", 30};
    Person alice = {"Alice", 25};
    Person bob = {"Bob", 30};
    Person carol = {"Carol", 22};

    vector_push_back(people, &john);
    vector_push_back(people, &alice);
    vector_push_back(people, &bob);
    vector_push_back(people, &carol);

    algorithm_stable_sort(vector_data(people), vector_size(people), sizeof(Person), compare_persons_by_age);

    fmt_printf("People sorted by age:\n");
    algorithm_for_each(vector_data(people), vector_size(people), sizeof(Person), print_person);

    vector_deallocate(people);

    return 0;
}
```
**Result:**
```
People sorted by age:
Name: Carol, Age: 22
Name: Alice, Age: 25
Name: John, Age: 30
Name: Bob, Age: 30
```

---

## Example 116: Check if an Array of Integers is Sorted `algorithm_is_sorted`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    size_t size = sizeof(arr) / sizeof(arr[0]);

    if (algorithm_is_sorted(arr, size, sizeof(int), compare_ints)) {
        fmt_printf("Array is sorted.\n");
    } 
    else {
        fmt_printf("Array is not sorted.\n");
    }

    return 0;
}
```
**Result:**
```
Array is sorted.
```

---

## Example 117: Check if a Vector of Doubles is Sorted `algorithm_is_sorted`

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

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
    Vector *vec = vector_create(sizeof(double));
    double values[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    size_t size = sizeof(values) / sizeof(values[0]);
    
    for (size_t i = 0; i < size; ++i) {
        vector_push_back(vec, &values[i]);
    }

    if (algorithm_is_sorted(vector_begin(vec), vector_size(vec), sizeof(double), compare_doubles)) {
        fmt_printf("Vector is sorted.\n");
    } 
    else {
        fmt_printf("Vector is not sorted.\n");
    }

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Vector is sorted.
```

---

## Example 118 : same as C++ `algorithm_is_sorted` I've used (ptrdiff_t)(size * sizeof(int)) to cast the result of the size calculation to ptrdiff_t`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;

    return (arg1 > arg2) - (arg1 < arg2);
}

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

void print_array(void* element) {
    static int counter = 0;
    
    if (counter % 4 == 0) {
        fmt_printf("\n");
    }

    counter++;
    fmt_printf(" %d", *(int*)element);
}

int main() {
    int foo[] = {2, 4, 1, 3};
    size_t size = sizeof(foo) / sizeof(foo[0]);

    fmt_printf("foo:");
    do {
        algorithm_prev_permutation(foo, foo + size, sizeof(int), less_int);
        algorithm_for_each(foo, size, sizeof(int), print_array);
        
    } while (!algorithm_is_sorted(foo, size, sizeof(int), compare_ints));

    fmt_printf("\nThe range is sorted!\n");

    return 0;
}
```
**Result in C:**
```
foo:
 2 3 4 1
 2 3 1 4
 2 1 4 3
 2 1 3 4
 1 4 3 2
 1 4 2 3
 1 3 4 2
 1 3 2 4
 1 2 4 3
 1 2 3 4
The range is sorted!
```

```C++ same Code```

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::is_sorted, std::prev_permutation
#include <array>        // std::array

int main () {
  std::array<int,4> foo {2,4,1,3};

  do {
    // try a new permutation:
    std::prev_permutation(foo.begin(),foo.end());

    std::cout << "foo:";
    for (int& x:foo) {
        std::cout << ' ' << x;
    }
    std::cout << '\n';

  } while (!std::is_sorted(foo.begin(),foo.end()));

  std::cout << "the range is sorted!\n";

  return 0;
}
```
**Result in C++:**
```
foo: 2 3 4 1
foo: 2 3 1 4
foo: 2 1 4 3
foo: 2 1 3 4
foo: 1 4 3 2
foo: 1 4 2 3
foo: 1 3 4 2
foo: 1 3 2 4
foo: 1 2 4 3
foo: 1 2 3 4
the range is sorted!
```

---

## Example 119 : `algorithm_is_sorted_until` 

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;

    return (arg1 > arg2) - (arg1 < arg2);
}

bool less_int(const void *a, const void *b) {
    return *(int *)a < *(int *)b;
}

int main() {
    int foo[] = {2, 4, 1, 3};
    size_t size = sizeof(foo) / sizeof(foo[0]);

    void *sorted_until;

    fmt_printf("foo:");
    do {
        algorithm_prev_permutation(foo, foo + size, sizeof(int), less_int);

        for (size_t i = 0; i < size; ++i) {
            fmt_printf(" %d", foo[i]);
        }

        sorted_until = algorithm_is_sorted_until(foo, size, sizeof(int), compare_ints);
        ptrdiff_t sorted_elements = ((char *)sorted_until - (char *)foo) / sizeof(int);

        fmt_printf(" (%td elements sorted)\n", sorted_elements);
        
    } while (((char *)sorted_until - (char *)foo) != (ptrdiff_t)(size * sizeof(int)));

    fmt_printf("The range is sorted!\n");

    return 0;
}
```
**Result in C:**
```
foo: 2 3 4 1 (3 elements sorted)
 2 3 1 4 (2 elements sorted)
 2 1 4 3 (1 elements sorted)
 2 1 3 4 (1 elements sorted)
 1 4 3 2 (2 elements sorted)
 1 4 2 3 (2 elements sorted)
 1 3 4 2 (3 elements sorted)
 1 3 2 4 (2 elements sorted)
 1 2 4 3 (3 elements sorted)
 1 2 3 4 (4 elements sorted)
The range is sorted!
```

`C++ same Code`

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::is_sorted_until, std::prev_permutation
#include <array>        // std::array

int main () {
  std::array<int,4> foo {2,4,1,3};
  std::array<int,4>::iterator it;

  do {
    // try a new permutation:
    std::prev_permutation(foo.begin(),foo.end());

    // print range:
    std::cout << "foo:";
    for (int& x:foo) {
        std::cout << ' ' << x;
    }
    it=std::is_sorted_until(foo.begin(),foo.end());
    std::cout << " (" << (it-foo.begin()) << " elements sorted)\n";

  } while (it!=foo.end());

  std::cout << "the range is sorted!\n";

  return 0;
}
```
**Result in C++:**
```
foo: 2 3 4 1 (3 elements sorted)
foo: 2 3 1 4 (2 elements sorted)
foo: 2 1 4 3 (1 elements sorted)
foo: 2 1 3 4 (1 elements sorted)
foo: 1 4 3 2 (2 elements sorted)
foo: 1 4 2 3 (2 elements sorted)
foo: 1 3 4 2 (3 elements sorted)
foo: 1 3 2 4 (2 elements sorted)
foo: 1 2 4 3 (3 elements sorted)
foo: 1 2 3 4 (4 elements sorted)
the range is sorted!
```

---

## Example 120 : rotate array of integer with `algorithm_rotate`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"


int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = sizeof(arr) / sizeof(arr[0]);
    size_t middle_index = 3; // Rotate around the 4th element

    algorithm_rotate(arr, arr + middle_index, arr + size, sizeof(arr[0]));

    fmt_printf("Rotated array:");
    for (size_t i = 0; i < size; ++i) {
        fmt_printf(" %d", arr[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Rotated array: 4 5 6 7 8 9 1 2 3
```

---

## Example 121 : Using Vector with `algorithm_rotate`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *vec = vector_create(sizeof(int));
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t size = sizeof(values) / sizeof(values[0]);
    
    for (size_t i = 0; i < size; ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Rotate around the 5th element
    algorithm_rotate(vector_begin(vec), vector_at(vec, 3), vector_end(vec), sizeof(int)); // or (int*)vector_begin(vec) + 3

    fmt_printf("Rotated vector:");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int *item = vector_at(vec, i);
        fmt_printf(" %d", *item);
    }
    fmt_printf("\n");

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Rotated vector: 4 5 6 7 8 9 1 2 3
```

`C++ Same Code`

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::rotate
#include <vector>       // std::vector

int main () {
  std::vector<int> myvector;

  // set some values:
  for (int i=1; i<10; ++i) 
    myvector.push_back(i); // 1 2 3 4 5 6 7 8 9

  std::rotate(myvector.begin(),myvector.begin()+3,myvector.end());
                                                  // 4 5 6 7 8 9 1 2 3
  // print out content:
  std::cout << "myvector contains:";
  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
```
**Result in C++:**
```
myvector contains: 4 5 6 7 8 9 1 2 3
```

---

### Example 122: Rotate Copy an Array of Integers

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    int result[7];
    size_t size = sizeof(arr) / sizeof(arr[0]);

    algorithm_rotate_copy(arr, arr + 3, arr + size, sizeof(arr[0]), result);

    fmt_printf("Rotated copied array:");
    for (size_t i = 0; i < size; ++i) {
        fmt_printf(" %d", result[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Rotated copied array: 4 5 6 7 1 2 3
```

---

### Example 123: Using Vector with `algorithm_rotate_copy`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *vec = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40, 50, 60, 70};
    size_t size = sizeof(values) / sizeof(values[0]);
    int result[7];
    
    for (size_t i = 0; i < size; ++i) {
        vector_push_back(vec, &values[i]);
    }

    algorithm_rotate_copy(vector_begin(vec), vector_at(vec, 3), vector_end(vec), sizeof(int), result);

    fmt_printf("Rotated copied vector:");
    for (size_t i = 0; i < size; ++i) {
        fmt_printf(" %d", result[i]);
    }
    fmt_printf("\n");

    vector_deallocate(vec);
    return 0;
}
```
**Result:**
```
Rotated copied vector: 40 50 60 70 10 20 30
```

`C++ Same Code`

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::rotate_copy
#include <vector>       // std::vector

int main () {
  int myints[] = {10,20,30,40,50,60,70};

  std::vector<int> myvector (7);

  std::rotate_copy(myints, myints + 3, myints + 7, myvector.begin());

  // print out content:
  std::cout << "myvector contains:";
  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
```
**Result in C++:**
```
myvector contains: 40 50 60 70 10 20 30
```

---

## Example 124 : How to use `algorithm_merge`
C Algorithm sort time: 0.000002 seconds

```c
#include "algorithm/algorithm.h"
#include "array/array.h"
#include "fmt/fmt.h"
#include <time.h>

int compare_ints(const void* a, const void* b) {
	int one = *(const int*)a;
	int two = *(const int*)b;

	return (one > two) - (one < two);
}

void print_int(void* number) {
	fmt_printf("%d ", *(int*)number);
}

int main() {
	struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

	int first[] = {5,10,15,20,25};
  	int second[] = {50,40,30,20,10};
	size_t size_first = sizeof(first) / sizeof(first[0]);
	size_t size_second = sizeof(second) / sizeof(first[0]);
	Array* arr = array_create(sizeof(int), 10);

	algorithm_sort(first, size_first, sizeof(int), compare_ints);
	algorithm_sort(second, size_second, sizeof(int), compare_ints);
	algorithm_merge(first, size_first, second, size_second, sizeof(int), array_begin(arr), compare_ints);
	
	clock_gettime(CLOCK_MONOTONIC, &end);

    double timeTaken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fmt_printf("C Algorithm sort time: %f seconds\n", timeTaken);

	array_deallocate(arr);
    return 0;
}
```
**Result in C:**
```
C Algorithm sort time: 0.000019 seconds
```

`C++ Time Take : 4.375e-06 seconds`

```cpp
#include <iostream>     // For std::cout
#include <algorithm>    // For std::merge, std::sort
#include <vector>       // For std::vector
#include <chrono>       // For std::chrono

int main () {
    auto start = std::chrono::high_resolution_clock::now();
    int first[] = {5,10,15,20,25};
    int second[] = {50,40,30,20,10};
    std::vector<int> v(10);

    std::sort (first, first + 5);
    std::sort (second, second + 5);
    std::merge (first, first + 5, second, second + 5, v.begin());

    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = stop - start;
    std::cout << "Time taken: " << duration.count() << " seconds\n";

    return 0;
}
```
**Result in C++:**
```
Time taken: 2e-05 seconds
```

---

## Example 125 : Point Struct with Using `algorithm_merge`

```c
#include "algorithm/algorithm.h"
#include "array/array.h"
#include "fmt/fmt.h"
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

int compare_points(const void* a, const void* b) {
    Point point1 = *(const Point*)a;
    Point point2 = *(const Point*)b;

    return (point1.x > point2.x) - (point1.x < point2.x);
}

void print_point(void* p) {
    Point* point = (Point*)p;
    fmt_printf("(%d, %d) ", point->x, point->y);
}

int main() {
    Point first[] = {{1, 2}, {3, 4}, {5, 6}};
    Point second[] = {{7, 8}, {9, 10}, {11, 12}};
    size_t size_first = sizeof(first) / sizeof(first[0]);
    size_t size_second = sizeof(second) / sizeof(second[0]);
    Array* arr = array_create(sizeof(Point), size_first + size_second);

    algorithm_sort(first, size_first, sizeof(Point), compare_points);
    algorithm_sort(second, size_second, sizeof(Point), compare_points);
    algorithm_merge(first, size_first, second, size_second, sizeof(Point), array_begin(arr), compare_points);

    for (size_t i = 0; i < array_size(arr); i++) {
        print_point(array_at(arr, i));
    }

    array_deallocate(arr);
    return 0;
}
```
**Result:**
```
(1, 2) (3, 4) (5, 6) (7, 8) (9, 10) (11, 12) 
```

---

## Example 126 : How to use `algorithm_inplace_merge`

```c
#include "algorithm/algorithm.h"
#include "array/array.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int compare_ints(const void* a, const void* b) {
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    int first[] = {5, 10, 15, 20, 25};
    int second[] = {50, 40, 30, 20, 10};
    size_t size_first = sizeof(first) / sizeof(first[0]);
    size_t size_second = sizeof(second) / sizeof(second[0]);
    size_t total_size = size_first + size_second;
    Array* arr = array_create(sizeof(int), total_size);

    // Sort both arrays in ascending order
    algorithm_sort(first, size_first, sizeof(int), compare_ints);
    algorithm_sort(second, size_second, sizeof(int), compare_ints);

    // Copy sorted arrays into `arr`
    algorithm_copy(first, size_first, sizeof(int), array_begin(arr));
    algorithm_copy(second, size_second, sizeof(int), array_at(arr, size_first));

    // Inplace merge within `arr`
    algorithm_inplace_merge(array_begin(arr), size_first, total_size, sizeof(int), compare_ints);

    fmt_printf("The resulting array contains:");
    for (size_t i = 0; i < total_size; i++) {
        int* p = (int*)array_at(arr, i);
        fmt_printf(" %d", *p);
    }
    fmt_printf("\n");

    array_deallocate(arr);
    return 0;
}
```
**Result in C:**
```
The resulting array contains: 5 10 10 15 20 20 25 30 40 50
```

`C++ Version`

```c
#include <iostream>     // std::cout
#include <algorithm>    // std::inplace_merge, std::sort, std::copy
#include <vector>       // std::vector

int main () {
  int first[] = {5,10,15,20,25};
  int second[] = {50,40,30,20,10};
  std::vector<int> v(10);
  std::vector<int>::iterator it;

  std::sort (first,first+5);
  std::sort (second,second+5);

  it=std::copy (first, first+5, v.begin());
     std::copy (second,second+5,it);

  std::inplace_merge (v.begin(),v.begin()+5,v.end());

  std::cout << "The resulting vector contains:";
  for (it=v.begin(); it!=v.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
```
**Result in C++:**
```
The resulting vector contains: 5 10 10 15 20 20 25 30 40 50
```

---

## Example 127 : inplace_merge with struct `algorithm_inplace_merge`

```c
#include "algorithm/algorithm.h"
#include "array/array.h"
#include "fmt/fmt.h"
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

int compare_points(const void* a, const void* b) {
    Point* point1 = (Point*)a;
    Point* point2 = (Point*)b;

    return (point1->x > point2->x) - (point1->x < point2->x);
}

void print_point(void* p) {
    Point* point = (Point*)p;
    fmt_printf("(%d, %d) ", point->x, point->y);
}

int main() {
    Point first[] = {{3, 1}, {1, 2}, {4, 3}};
    Point second[] = {{6, 4}, {5, 5}, {7, 6}};
    size_t size_first = sizeof(first) / sizeof(first[0]);
    size_t size_second = sizeof(second) / sizeof(second[0]);
    size_t total_size = size_first + size_second;

    Array* arr = array_create(sizeof(Point), total_size);

    algorithm_sort(first, size_first, sizeof(Point), compare_points);
    algorithm_sort(second, size_second, sizeof(Point), compare_points);

    algorithm_copy(first, size_first, sizeof(Point), array_begin(arr));
    algorithm_copy(second, size_second, sizeof(Point), array_at(arr, size_first));

    algorithm_inplace_merge(array_begin(arr), size_first, total_size, sizeof(Point), compare_points);
	algorithm_for_each(array_begin(arr), total_size, sizeof(Point), print_point);

    array_deallocate(arr);
    return 0;
}
```
**Result:**
```
(1, 2) (3, 1) (4, 3) (5, 5) (6, 4) (7, 6)
```

---

## Example 128 : Using String object with `algorithm_adjacent_find`

```c
#include "string/std_string.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <string.h>

int compare_strings(const void *a, const void *b) {
    const String *str1 = *(const String **)a;
    const String *str2 = *(const String **)b;

    return string_compare(str1, str2);
}

int main() {
    String** mystrings = string_create_from_initializer(9, "apple", "banana", "banana", "cherry", 
                                                        "cherry", "date", "elderberry", "fig", "fig");
    size_t num_elements = 9;

    const String** it = (const String**)algorithm_adjacent_find((const void**)mystrings, num_elements, sizeof(String*),compare_strings);
    if (it != NULL) {
        fmt_printf("The first pair of repeated elements are: %s\n", string_c_str(*it));
        it++;
    } 
	else {
        fmt_printf("No first pair of repeated elements found.\n");
        return 0;
    }

    size_t remaining_elements = num_elements - ((String**)it - mystrings);
    const String** it2 = (const String**)algorithm_adjacent_find((const void**)it, remaining_elements, sizeof(String*), compare_strings);
    if (it2 != NULL) {
        fmt_printf("The second pair of repeated elements are: %s\n", string_c_str(*it2));
    } 
	else {
        fmt_printf("No second pair of repeated elements found.\n");
    }
    
    for (size_t i = 0; i < num_elements; i++) {
        string_deallocate(mystrings[i]);
    }
    return 0;
}
```
**Result:**
```
The first pair of repeated elements are: banana
The second pair of repeated elements are: cherry
```

---

## Example 129 : Using Intger Array with `algorithm_adjacent_find`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdbool.h>


int compare_ints(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;

    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
    int myints[] = {5, 20, 5, 30, 30, 20, 10, 10, 20};
    size_t num_elements = sizeof(myints) / sizeof(myints[0]);

    int* it = algorithm_adjacent_find(myints, num_elements, sizeof(int), compare_ints);
    if (it != NULL) {
        fmt_printf("The first pair of repeated elements are: %d\n", *it);
		it++;
    }

    int* it2 = algorithm_adjacent_find(it, num_elements - (it - myints), sizeof(int), compare_ints);
    if (it2 != NULL) {
        fmt_printf("The second pair of repeated elements are: %d\n", *it2);
    }

    return 0;
}
```
**Result:**
```
The first pair of repeated elements are: 30
The second pair of repeated elements are: 10
```

---

## Example 130 : implement `algorithm_mismatch`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <stdbool.h>

bool mypredicate(const void *i, const void *j) {
    return (*(const int *)i != *(const int *)j);
}

int main() {
	Vector* vec = vector_create(sizeof(int));
	int myints[] = {10, 20, 80, 320, 1024};
	size_t myints_size = sizeof(myints) / sizeof(myints[0]);
	Pair mypair;

	for (size_t index = 1; index < 6; index++) {
		vector_push_back(vec, &index);
	}

    mypair = algorithm_mismatch(vector_begin(vec), vector_size(vec), sizeof(int), myints, myints_size, sizeof(int), mypredicate);
    if (mypair.first != NULL && mypair.second != NULL) {
        fmt_printf("First mismatching elements: %d and %d\n", *(int *)mypair.first, *(int *)mypair.second);
    }

    // Move iterators to the next elements
    if (mypair.first != NULL) {
        mypair.first = (char *)mypair.first + sizeof(int);
    }
    if (mypair.second != NULL) {
        mypair.second = (char *)mypair.second + sizeof(int);
    }

    mypair = algorithm_mismatch(mypair.first, vector_size(vec), sizeof(int), mypair.second, myints_size, sizeof(int), mypredicate);
    if (mypair.first != NULL && mypair.second != NULL) {
        fmt_printf("Second mismatching elements: %d and %d\n", *(int *)mypair.first, *(int *)mypair.second);
    }

    return 0;
}
```
**Result:**
```
First mismatching elements: 1 and 10
Second mismatching elements: 2 and 20
```

---

## Example 131 : `algorithm_is_sequence`

```c
#include "array/array.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>

void random_number_generator(void *output) {
    *(int *)output = (rand() % 5) + 1; 
}

int compare_ints(const void *a, const void *b) {
    const int arg1 = *(const int *)a;
    const int arg2 = *(const int *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

int main() {
	srand(time(NULL));
    Array* foo = array_create(sizeof(int), 5);
	Array* bar = array_create(sizeof(int), 5);

	algorithm_generate(array_begin(foo), array_end(foo), sizeof(int), random_number_generator);
	algorithm_generate(array_begin(bar), array_end(bar), sizeof(int), random_number_generator);

	if (algorithm_is_permutation(array_begin(foo), array_size(foo), sizeof(int), 
									array_begin(bar), array_size(bar), sizeof(int), compare_ints)) {
			fmt_printf("foo and bar contain the same elements");
	}

	array_deallocate(foo);
	array_deallocate(bar);

    return EXIT_SUCCESS;
}
```
**Result:**
```
foo and bar contain the same elements
```

---

## Example 132 : Using Integers Array with `algorithm_search`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool int_equal(const void* a, const void* b) {
    return *(const int*)a == *(const int*)b;
}

int main() {
    int haystack[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    int needle[] = {40, 50, 60, 70};
    const int* result = (const int* )algorithm_search(haystack, haystack + 9, sizeof(int), needle, needle + 4, sizeof(int), int_equal);

    if (result != haystack + 9) {
        fmt_printf("Subsequence found at position %lld\n", result - haystack);
    } 
    else {
        fmt_printf("Subsequence not found\n");
    }

    return 0;
}
```
**Result:**
```
Subsequence found at position 3
```

---

## Example 133 : Using struct with `algorithm_search`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char name[50];
    int age;
}Person;

bool is_equal(const void* a, const void* b) {
    const Person* arg1 = (const Person*)a;
    const Person* arg2 = (const Person*)b;
    return (strcmp(arg1->name, arg2->name) == 0) && (arg1->age == arg2->age);
}

int main() {
    Person information[] = {{"amin", 27}, {"omid", 28}, {"ali", 25}, {"milad", 27}};
    Person person[] = {{"omid", 28}, {"ali", 25}};

    if (algorithm_search(information, information + 4, sizeof(person), 
                        person, person + 2, sizeof(person), is_equal)) {
        fmt_printf("its equal");
    }
    else {
        fmt_printf("its not equal");
    }
    
    return 0;
}
```
**Result:**
```
its equal
```

---

## Example 134 : Use Integer Array with `algorithm_search_n`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include <stdbool.h>

bool int_equal(const void* a, const void* b) {
    return *(const int*)a == *(const int*)b;
}

int main() {
    int myints[] = {10, 20, 30, 30, 20, 10, 10, 20};
    size_t n = sizeof(myints) / sizeof(myints[0]);
    int val = 30;

    const int* result = (const int*)algorithm_search_n(myints, myints + n, sizeof(int), 2, &val, int_equal);

    if (result != myints + n) {
        fmt_printf("Two 30s found at position %lld\n", result - myints);
    } 
    else {
        fmt_printf("Match not found\n");
    }
    
    val = 10;
    const int* new_result = (const int*)algorithm_search_n(myints, myints + n, sizeof(int), 2, &val, int_equal);

    if (new_result != myints + n) {
        fmt_printf("Two 10 found at %lld", new_result - myints);
    }
    else {
        fmt_printf("Match not found");
    }

    return 0;
}
```
**Result in C:**
```
Two 30s found at position 2
Two 10 found at 5
```

`C++ Same Code`

```cpp
#include <iostream>     // std::cout
#include <algorithm>    // std::search_n
#include <vector>       // std::vector

bool mypredicate (int i, int j) {
  return (i==j);
}

int main () {
  int myints[]={10,20,30,30,20,10,10,20};
  std::vector<int> myvector (myints,myints+8);

  std::vector<int>::iterator it;

  // using default comparison:
  it = std::search_n (myvector.begin(), myvector.end(), 2, 30);

  if (it!=myvector.end())
    std::cout << "two 30s found at position " << (it-myvector.begin()) << '\n';
  else
    std::cout << "match not found\n";

  // using predicate comparison:
  it = std::search_n (myvector.begin(), myvector.end(), 2, 10, mypredicate);

  if (it!=myvector.end())
    std::cout << "two 10s found at position " << int(it-myvector.begin()) << '\n';
  else
    std::cout << "match not found\n";

  return 0;
}
```
**Result in C++:**
```
two 30s found at position 2
two 10s found at position 5
```

---

## Example 135 : Use `algorithm_remove`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <stdbool.h>

int int_compare(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia > *ib) - (*ia < *ib);
}


int main() {
    int myints[] = {10,20,30,30,20,10,10,20}; 
    Vector* vec = vector_create(sizeof(int));
    int value = 20;

    vector_resize(vec, 8);
    algorithm_copy(myints, 8, sizeof(int), vec->items);

    for (int *begin = (int*)vector_begin(vec); begin != (int*)vector_end(vec); begin++) {
        fmt_printf("%d ", *begin);
    }
    fmt_printf("\n");
    
    int *ptr = (int*)algorithm_remove((int*)vector_begin(vec), vector_size(vec), sizeof(int), &value, int_compare);

    for (int *begin = (int*)vector_begin(vec); begin != ptr; begin++) {
        fmt_printf("%d ", *begin);
    }

    return 0;
}
```
**Result in C:**
```
10 20 30 30 20 10 10 20 
10 30 30 10 10
```

`c++ same code`

```c
#include <iostream>     // std::cout
#include <algorithm>    // std::remove

int main () {
  int myints[] = {10,20,30,30,20,10,10,20};      // 10 20 30 30 20 10 10 20

  // bounds of range:
  int* pbegin = myints;                          // ^
  int* pend = myints+sizeof(myints)/sizeof(int); // ^                       ^

  pend = std::remove (pbegin, pend, 20);         // 10 30 30 10 10 ?  ?  ?
                                                 // ^              ^
  std::cout << "range contains:";
  for (int* p=pbegin; p!=pend; ++p)
    std::cout << ' ' << *p;
  std::cout << '\n';

  return 0;
}
```
**Result in C++:**
```
range contains: 10 30 30 10 10
```

---

## Example 136 : using `algorithm_remove_copy`

```c
#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <stdbool.h>

int int_compare(const void *a, const void *b) {
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia > *ib) - (*ia < *ib);
}

int main() {
    int myints[] = {10, 20, 30, 30, 20, 10, 10, 20};
    Vector *vec = vector_create(sizeof(int));
    Vector *resultVec = vector_create(sizeof(int));
    int value = 20;

    vector_resize(vec, 8);
    algorithm_copy(myints, 8, sizeof(int), vec->items);
    vector_resize(resultVec, 8); 
    algorithm_remove_copy(vector_begin(vec), vector_size(vec), sizeof(int), vector_begin(resultVec), &value, int_compare);
    
    for (int *begin = (int*)vector_begin(resultVec); begin != (int*)vector_end(resultVec); ++begin) {
        fmt_printf("%d ", *begin);
    }

    vector_deallocate(vec);
    vector_deallocate(resultVec);

    return 0;
}

```
**Result:**
```
10 30 30 10 10 0 0 0
```

---

## Example 137: Using `algorithm_replace`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"


int compare_ints(const void *a, const void *b) {
    int num1 = *(int *)a;
    int num2 = *(int *)b;
    return num1 - num2;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 5, 5, 8, 9, 10};
    int old_val = 5;
    int new_val = -1;

    fmt_printf("Before replacement:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    algorithm_replace(arr, 10, sizeof(int), &old_val, &new_val, compare_ints);

    fmt_printf("After replacement:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Before replacement:
1 2 3 4 5 5 5 8 9 10 
After replacement:
1 2 3 4 -1 -1 -1 8 9 10
```

---

## Example 138: Using `algorithm_replace_if`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"


bool is_even(const void *n) {
    int num = *(int *)n;
    return num % 2 == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int new_val = -1;

    fmt_printf("Before replacement:\n");
    for (int i = 0; i < 10; ++i) {
        printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    algorithm_replace_if(arr, 10, sizeof(int), &new_val, is_even);

    fmt_printf("After replacement:\n");
    for (int i = 0; i < 10; ++i) {
        printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Before replacement:
1 2 3 4 5 6 7 8 9 10 
After replacement:
1 -1 3 -1 5 -1 7 -1 9 -1
```

---

## Example 139: Using `algorithm_remove_copy_if`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

bool is_even(const void *n) {
    int num = *(int *)n;
    return num % 2 == 0;
}

int main() {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int result[10] = {0};

    fmt_printf("Source array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%d ", arr[i]);
    }
    fmt_printf("\n");

    size_t count = algorithm_remove_copy_if(arr, 10, sizeof(int), result, is_even);

    fmt_printf("Result array:\n");
    for (size_t i = 0; i < count; ++i) {
        fmt_printf("%d ", result[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result:**
```
Source array:
1 2 3 4 5 6 7 8 9 10 
Result array:
1 3 5 7 9
```

---

## Example 140 : How `begin` and `end` Work

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

int main() {
    int arr[] = {10, 20, 30, 40, 50};
    size_t num = sizeof(arr) / sizeof(arr[0]);
    size_t size = sizeof(arr[0]);  

    int* begin = algorithm_begin(arr);
    int* end = algorithm_end(arr, num, size);

    fmt_printf("Array elements: ");
    for (int* it = begin; it != end; ++it) {
        fmt_printf("%d ", *it);
    }
    fmt_printf("\n");

    return 0;
}
```

---

## Example 141 : how to put list of data in array with `algorithm_iota`

```c
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"


int main() {
    unsigned long arr_ulong[10];
    unsigned long start_ulong = 1000;
    
    algorithm_iota(algorithm_begin(arr_ulong), algorithm_end(arr_ulong, 10, sizeof(unsigned long)), &start_ulong, sizeof(unsigned long), TYPE_UNSIGNED_LONG);

    fmt_printf("Unsigned long array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%lu\n", arr_ulong[i]);
    }
    fmt_printf("\n");

    unsigned char arr_char[10];
    unsigned char start_char = 'A'; 
    
    algorithm_iota(arr_char, arr_char + 10, &start_char, sizeof(unsigned char), TYPE_UNSIGNED_CHAR);

    fmt_printf("Unsigned Char array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%c\n", arr_char[i]);
    }
    fmt_printf("\n");

    float arr_float[10];
    float start_float = 1.5f; 

    algorithm_iota(arr_float, arr_float + 10, &start_float, sizeof(float), TYPE_FLOAT);

    fmt_printf("Float array:\n");
    for (int i = 0; i < 10; ++i) {
        fmt_printf("%f\n", arr_float[i]);
    }

    return 0;
}
```
**Result**
```
Unsigned long array:
1000
1001
1002
1003
1004
1005
1006
1007
1008
1009

Unsigned Char array:
A
B
C
D
E
F
G
H
I
J

Float array:
1.500000
2.500000
3.500000
4.500000
5.500000
6.500000
7.500000
8.500000
9.500000
10.500000
```

## License

This project is open-source and available under [ISC License].