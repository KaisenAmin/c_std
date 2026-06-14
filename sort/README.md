# Sort Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2025  
**License:** ISC License

The Sort library is a comprehensive, generic sorting toolkit for C. It provides over 15 sorting algorithms, search primitives, heap and partition helpers, and performance-statistics tracking — all built around a pointer-based `ElementType` model that works with any data type.

## Compilation

```bash
gcc -std=c17 -O2 -Wall -Wextra -pedantic -o main main.c sort/sort.c
```

If your program also uses `fmt` or `string`, add their `.c` files:

```bash
gcc -std=c17 -O2 -Wall -Wextra -o main \
    main.c sort/sort.c fmt/fmt.c string/std_string.c encoding/encoding.c -lm
```

## Usage

```c
#include "sort/sort.h"
```

## Key Concepts

### Element model

Every sort routine operates on an `ElementType[]` array, where each slot holds one **user pointer** (e.g. `int*`, `char*`, `double*`). The library never copies or owns the pointed-to data.

```c
int values[] = {5, 3, 8, 1};
ElementType arr[4];
for (int i = 0; i < 4; i++) arr[i] = &values[i]; // store pointers
sort_quicksort(arr, 4, sort_compare_int, sort_swap_generic);
// values[] is NOT modified; the arr[] slots are rearranged
```

### Built-in callbacks

| Comparator | Swap | Print |
|---|---|---|
| `sort_compare_int` | `sort_swap_generic` | `sort_print_int` |
| `sort_compare_string` | | `sort_print_string` |
| `sort_compare_float` | | `sort_print_float` |
| `sort_compare_double` | | `sort_print_double` |
| `sort_compare_pointer` | | `sort_print_pointer` |

---

## Function Descriptions

### `void sort_init_stats(SortStats* stats)`

**Purpose**:  
Zero-initialises a `SortStats` block — sets all counters and flags to zero / `false`.

**Parameters**:  
- `stats`: Pointer to the block to clear. May be `NULL` (safe no-op).

**Return Value**:  
None.

**Usage Case**:  
Call before passing a `SortStats*` to `sort_generic` to ensure no stale data from a previous run leaks into the new measurement.

---

### `void sort_reset_stats(SortStats* stats)`

**Purpose**:  
Alias for `sort_init_stats`. Clears all fields in the `SortStats` block.

**Parameters**:  
- `stats`: Pointer to the block to reset.

**Return Value**:  
None.

**Usage Case**:  
Use between successive calls to `sort_generic` when you want to measure each algorithm run independently.

---

### `void sort_print_stats(const SortStats* stats)`

**Purpose**:  
Prints a human-readable summary of a `SortStats` block to `stdout`.

**Parameters**:  
- `stats`: Pointer to the statistics block. May be `NULL` (safe no-op).

**Return Value**:  
None.

**Usage Case**:  
Call after `sort_generic` to display comparisons, swaps, recursive calls, execution time, and the stable/in-place flags.

---

### `void sort_insertion(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Sorts `array` using the insertion sort algorithm. Stable, in-place, O(n²) worst case. Excellent for nearly-sorted data and small arrays (used as the small-array fallback in the optimised and introspective variants).

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: User comparator (`<0`, `0`, `>0`).
- `swap`: User swap (pass `sort_swap_generic` unless you need custom logic).

**Return Value**:  
None.

**Usage Case**:  
Ideal when the array is already nearly sorted or contains fewer than ~16 elements.

---

### `void sort_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Classic Lomuto-partition quicksort. Average O(n log n), worst case O(n²). In-place but not stable.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
General-purpose sorting when stability is not required and the dataset is random.

---

### `void sort_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Top-down merge sort. Stable, O(n log n) guaranteed. Uses O(n) auxiliary storage.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Sorting strings, structs, or anything where equal elements must keep their original relative order.

---

### `void sort_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Heap sort. O(n log n) guaranteed, in-place, not stable.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Guaranteed worst-case performance without extra memory. Preferred when stack depth is limited.

---

### `void sort_bubblesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Classic bubble sort. O(n²) worst case. Stable and in-place. Detects an already-sorted array in O(n).

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Educational purposes or tiny arrays where simplicity outweighs performance.

---

### `void sort_selection(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Selection sort. O(n²), in-place, not stable. Makes exactly n−1 swaps.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
When the cost of swapping is very high relative to comparing, and write-count minimisation matters.

---

### `void sort_shell(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Shell sort with a Ciura gap sequence. Better than O(n²) in practice. In-place.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
A practical choice between insertion sort and quicksort for medium-sized datasets.

---

### `void sort_counting(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Counting sort adapted for pointer-based element arrays. O(n + k) where k is the value range. Requires integer-valued elements.

**Parameters**:  
- `array`: Array of integer-pointer elements.
- `size`: Number of elements.
- `compare`: Comparator (used to determine ordering).
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Sorting small, bounded integers (e.g. grades 1–5, counts) where k is much smaller than n².

---

### `void sort_radix(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Radix sort (LSD, base-10) for non-negative integer elements. O(d × n) where d is the digit count.

**Parameters**:  
- `array`: Array of `int*` elements.
- `size`: Number of elements.
- `compare`: Comparator (used for fallback).
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Sorting large arrays of non-negative integers faster than comparison-based algorithms when the digit count is small.

---

### `void sort_bucket(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Bucket sort: distributes elements into buckets by value range, sorts each bucket, then concatenates. O(n + k) average.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Sorting uniformly distributed numeric data quickly.

---

### `void sort_introspective(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Introsort: starts as quicksort and switches to heapsort when recursion depth exceeds 2·log₂(n), avoiding the O(n²) worst case.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
The standard library-grade algorithm: O(n log n) worst-case, O(n log n) average, in-place.

---

### `void sort_quicksort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Quicksort enhanced with median-of-three pivot selection and an insertion-sort fallback for sub-arrays ≤ 10 elements.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Drop-in replacement for `sort_quicksort` with significantly better performance on sorted or nearly-sorted data.

---

### `void sort_mergesort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Merge sort with an insertion-sort cutoff for small sub-arrays, reducing the overhead of recursive calls.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Stable, guaranteed O(n log n) sorting with better real-world constants than the basic merge sort.

---

### `void sort_introspective_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Optimised introsort: combines median-of-three quicksort, insertion-sort for small arrays, and heapsort as a depth-limit fallback.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Best general-purpose algorithm in the library; closest to C++ `std::sort`.

---

### `void sort_parallel_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Parallel merge sort using POSIX threads. Splits the array into halves sorted concurrently, then merges.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Throughput-maximising sort on multi-core machines for large datasets.

---

### `void sort_parallel_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Parallel quicksort that spawns threads to sort sub-partitions concurrently.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Concurrent in-place sorting for large arrays on systems with multiple CPU cores.

---

### `void sort_stable_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
A stable variant of quicksort that preserves the relative order of equal elements.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
When you need the speed of quicksort combined with the stability guarantee of merge sort.

---

### `void sort_stable_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
A stable variant of heap sort that preserves the relative order of equal elements.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Guaranteed O(n log n) in-place sort with stability — a rarer combination.

---

### `void sort_adaptive(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Automatically selects the most appropriate algorithm based on array size: insertion sort for ≤ 16 elements, merge sort for ≤ 64, quicksort optimised otherwise.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Convenience wrapper when you don't want to decide which algorithm to use manually.

---

### `void sort_timsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
TimSort: finds natural runs in the data, extends short runs using insertion sort, and merges them in an efficient order. Stable, O(n log n) worst case, O(n) on already-sorted input.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Best choice for real-world data that is partially sorted or has repeated patterns. The algorithm used by Python's built-in `sort`.

---

### `void sort_generic(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap, SortAlgorithm algorithm, SortStats* stats)`

**Purpose**:  
Unified dispatcher: runs any algorithm selected via the `SortAlgorithm` enum, measures execution time, comparisons, and swaps, and stores the results in the caller-supplied `SortStats` block.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.
- `algorithm`: One of `SORT_INSERTION`, `SORT_QUICKSORT`, `SORT_MERGESORT`, `SORT_HEAPSORT`, `SORT_BUBBLESORT`, `SORT_SELECTION`, `SORT_SHELL`, `SORT_COUNTING`, `SORT_RADIX`, `SORT_BUCKET`, `SORT_INTROSPECTIVE`.
- `stats`: Output statistics block (may be `NULL` if statistics are not needed).

**Return Value**:  
None.

**Usage Case**:  
Comparing algorithms under identical conditions, or selecting an algorithm at runtime via configuration.

---

### `bool sort_is_sorted(ElementType* array, size_t size, CompareFunc compare)`

**Purpose**:  
Returns `true` if `array` is in non-decreasing order under `compare`.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.

**Return Value**:  
`true` if sorted, `false` otherwise. An array of size 0 or 1 is always sorted.

**Usage Case**:  
Pre-sort validation, post-sort assertion, or guard before calling `sort_binary_search`.

---

### `size_t sort_is_sorted_until(ElementType* array, size_t size, CompareFunc compare)`

**Purpose**:  
Returns the index of the first element that is out of order — the end of the longest sorted prefix (analog of `std::is_sorted_until`). It is the diagnostic companion to `sort_is_sorted`: it tells you not just *whether* the data is sorted, but exactly *where* the order breaks. The relationship `sort_is_sorted(a, n, cmp) == (sort_is_sorted_until(a, n, cmp) == n)` always holds.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `compare`: Comparator.

**Return Value**:  
The index in `[0, size]` of the first element strictly smaller than its predecessor, or `size` if the array is fully sorted (or has fewer than two elements, or is `NULL`).

**Usage Case**:  
Validating partially-sorted input, or locating the boundary of a sorted run before merging.

---

### `size_t sort_unique(ElementType* array, size_t size, CompareFunc compare)`

**Purpose**:  
Removes consecutive duplicate elements in-place, keeping the first of each run (analog of `std::unique`). This is the second half of the ubiquitous "**sort, then unique**" idiom: sort first, then call this to collapse each group of equal elements to one. Only the element *pointers* are moved — the function never frees or copies the pointed-to data — so it is safe regardless of who owns the elements.

**Parameters**:  
- `array`: Array of element pointers (typically already sorted).
- `size`: Number of elements.
- `compare`: Comparator.

**Return Value**:  
The number of distinct elements now packed at the front of `array` (the new logical length); `0` for a `NULL` array / `NULL` comparator / empty input. Slots beyond the returned count keep their previous (now-duplicated) pointer values — like `std::unique`, they are left unspecified-but-valid, not cleared.

**Usage Case**:  
De-duplicating a dataset after sorting — building a set, removing repeated log lines, collapsing identical records.

---

### `void sort_reverse(ElementType* array, size_t size, SwapFunc swap)`

**Purpose**:  
Reverses the order of elements in `array` in place.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Convert an ascending-sorted array to descending order in O(n) without re-sorting.

---

### `void sort_shuffle(ElementType* array, size_t size, SwapFunc swap)`

**Purpose**:  
Fisher-Yates shuffle: randomly permutes `array` in place using `rand()`.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Generating random permutations for testing, card game simulations, or random sampling.

---

### `ElementType* sort_partial_sort(ElementType* array, size_t size, size_t k, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Rearranges `array` so that positions `[0, k)` contain the `k` smallest elements in ascending sorted order. The rest of the array is in an unspecified order.

**Parameters**:  
- `array`: Array of element pointers (modified in place).
- `size`: Total number of elements.
- `k`: Number of smallest elements to expose at the front.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
Pointer to `array` (the same pointer passed in).

**Usage Case**:  
Retrieving a Top-K leaderboard, finding the k cheapest products, or pagination of sorted results without sorting the entire dataset.

---

### `ElementType sort_median(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Finds and returns the median element (position `size/2` in sorted order) using a quickselect-style algorithm. The array may be partially rearranged.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements (must be ≥ 1).
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
The median `ElementType` value, or `NULL` on bad arguments.

**Usage Case**:  
Statistical computation, pivot selection for divide-and-conquer algorithms, or finding the midpoint of a dataset.

---

### `ElementType sort_nth_element(ElementType* array, size_t size, size_t n, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Quickselect: rearranges `array` so that `array[n]` contains the element that would occupy index `n` in the sorted array. Elements before `n` are ≤ `array[n]`; elements after are ≥.

**Parameters**:  
- `array`: Array of element pointers (partially rearranged).
- `size`: Number of elements.
- `n`: Target index (0-based; must be < `size`).
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
The element at position `n`, or `NULL` on bad arguments.

**Usage Case**:  
Finding the kth smallest element in O(n) average time without fully sorting the array.

---

### `size_t sort_binary_search(ElementType* array, size_t size, ElementType target, CompareFunc compare)`

**Purpose**:  
Standard binary search on a sorted array. Returns the index of any matching element.

**Parameters**:  
- `array`: Sorted array of element pointers.
- `size`: Number of elements.
- `target`: Element to find (passed by value as `ElementType`).
- `compare`: Comparator.

**Return Value**:  
Index of a matching element, or `SIZE_MAX` if not found.

**Usage Case**:  
Fast O(log n) lookup in a sorted array — faster than a linear scan for large datasets.

---

### `size_t sort_lower_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare)`

**Purpose**:  
Returns the index of the first element that is **not less than** `target` (C++ `std::lower_bound` semantics).

**Parameters**:  
- `array`: Sorted array of element pointers.
- `size`: Number of elements.
- `target`: The value to search for.
- `compare`: Comparator.

**Return Value**:  
Index in `[0, size]`. Returns `size` if all elements are less than `target`.

**Usage Case**:  
Finding the insertion point for `target` to keep the array sorted, or locating the start of a run of equal elements.

---

### `size_t sort_upper_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare)`

**Purpose**:  
Returns the index of the first element that is **greater than** `target` (C++ `std::upper_bound` semantics).

**Parameters**:  
- `array`: Sorted array of element pointers.
- `size`: Number of elements.
- `target`: The value to search for.
- `compare`: Comparator.

**Return Value**:  
Index in `[0, size]`. Returns `size` if all elements are ≤ `target`.

**Usage Case**:  
Locating the end of a run of equal elements, or computing the count of a value as `upper_bound − lower_bound`.

---

### `ElementType* sort_binary_search_range(ElementType* array, size_t size, ElementType target, CompareFunc compare, size_t* count)`

**Purpose**:  
Finds the contiguous range of elements equal to `target` in a sorted array (equivalent to C++ `std::equal_range`).

**Parameters**:  
- `array`: Sorted array of element pointers.
- `size`: Number of elements.
- `target`: Element to search for.
- `compare`: Comparator.
- `count`: Output — set to the number of matching elements (0 if none).

**Return Value**:  
Pointer to the first matching slot in `array`, or `NULL` if not found.

**Usage Case**:  
Counting duplicates, extracting all records matching a key, or implementing range queries.

---

### `void sort_merge(ElementType* array, size_t left, size_t mid, size_t right, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Merges the two sorted sub-arrays `array[left..mid]` and `array[mid+1..right]` in place using auxiliary storage.

**Parameters**:  
- `array`: Array containing both sorted halves.
- `left`: Start index of the left half.
- `mid`: End index of the left half.
- `right`: End index of the right half.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Building custom merge sort variants or merging two pre-sorted regions of a buffer.

---

### `void sort_merge_inplace(ElementType* array, size_t left, size_t mid, size_t right, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Merges two sorted sub-ranges in place without auxiliary storage. O(n²) worst case but zero extra memory.

**Parameters**:  
- `array`: Array containing both sorted halves.
- `left`, `mid`, `right`: Sub-range indices (same convention as `sort_merge`).
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Memory-constrained environments where heap allocation for a temporary buffer is not permitted.

---

### `ElementType* sort_merge_sorted_arrays(ElementType* arr1, size_t size1, ElementType* arr2, size_t size2, CompareFunc compare, SwapFunc swap, size_t* result_size)`

**Purpose**:  
Merges two independent sorted arrays into a newly allocated sorted array. The caller must `free()` the result.

**Parameters**:  
- `arr1`: First sorted array.
- `size1`: Length of `arr1`.
- `arr2`: Second sorted array.
- `size2`: Length of `arr2`.
- `compare`: Comparator.
- `swap`: Swap callback (used internally).
- `result_size`: Output — set to `size1 + size2`.

**Return Value**:  
Newly allocated `ElementType[]` containing the merged result, or `NULL` on failure. The caller must `free()` this pointer.

**Usage Case**:  
Combining two sorted lists — for example, merging two pages of sorted search results.

---

### `void sort_heapify(ElementType* array, size_t size, size_t index, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Restores the max-heap property at `index` by sifting down. Assumes all descendants of `index` already satisfy the heap property.

**Parameters**:  
- `array`: Array representing a binary heap.
- `size`: Logical size of the heap portion.
- `index`: Root of the sub-tree to heapify.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Called internally by heap sort and the heap-based utilities. Also useful when building custom priority-queue logic.

---

### `void sort_build_heap(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Converts an arbitrary array into a max-heap in O(n) using Floyd's algorithm.

**Parameters**:  
- `array`: Array of element pointers (reordered in place).
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Initial step for heap sort or before a sequence of `sort_heap_extract_max` calls.

---

### `void sort_heap_insert(ElementType* array, size_t* size, ElementType element, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Inserts `element` at the end of the heap and sifts it up to restore the max-heap property. Increments `*size`.

**Parameters**:  
- `array`: Max-heap array. Must have room for one more element.
- `size`: Pointer to the current heap size (incremented on success).
- `element`: The element pointer to insert.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
None.

**Usage Case**:  
Building a heap incrementally one element at a time (for example, a streaming top-K filter).

---

### `ElementType sort_heap_extract_max(ElementType* array, size_t* size, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Removes and returns the maximum element (the root) of a max-heap. Decrements `*size` and calls `sort_heapify` to restore the property.

**Parameters**:  
- `array`: Max-heap array.
- `size`: Pointer to the current heap size (decremented).
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
The maximum `ElementType`, or `NULL` if the heap is empty or arguments are invalid.

**Usage Case**:  
Implementing a priority queue, or extracting the k-largest elements from a max-heap.

---

### `size_t sort_partition(ElementType* array, size_t left, size_t right, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Lomuto partition scheme: picks `array[right]` as pivot, partitions, and returns the pivot's final index.

**Parameters**:  
- `array`: Array of element pointers.
- `left`: Start index of the region.
- `right`: End index (also the pivot index).
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
Final index of the pivot element.

**Usage Case**:  
Building block for custom quicksort or quickselect implementations.

---

### `size_t sort_partition_three_way(ElementType* array, size_t left, size_t right, ElementType pivot, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Dutch National Flag three-way partition: rearranges elements into `< pivot`, `== pivot`, `> pivot` sections. Returns the index of the first element equal to the pivot.

**Parameters**:  
- `array`: Array of element pointers.
- `left`, `right`: Region bounds.
- `pivot`: The pivot element to partition around.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
Index of the first element equal to `pivot`.

**Usage Case**:  
Optimising quicksort for arrays with many duplicate elements.

---

### `size_t sort_partition_median_of_three(ElementType* array, size_t left, size_t right, CompareFunc compare, SwapFunc swap)`

**Purpose**:  
Selects the median of `array[left]`, `array[mid]`, `array[right]` as pivot, places it at `array[right]`, then calls `sort_partition`.

**Parameters**:  
- `array`: Array of element pointers.
- `left`, `right`: Region bounds.
- `compare`: Comparator.
- `swap`: Swap callback.

**Return Value**:  
Final pivot index.

**Usage Case**:  
Avoiding the O(n²) worst case on sorted or reverse-sorted input.

---

### `void sort_print_array(ElementType* array, size_t size, PrintFunc print)`

**Purpose**:  
Prints the array as `[e0, e1, ..., eN]` followed by a newline, using the supplied per-element callback.

**Parameters**:  
- `array`: Array of element pointers.
- `size`: Number of elements.
- `print`: Per-element print callback (e.g. `sort_print_int`).

**Return Value**:  
None.

**Usage Case**:  
Quick debugging and logging of any `ElementType[]` array without writing a loop.

---

### `void sort_benchmark_algorithms(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap, PrintFunc print)`

**Purpose**:  
Runs a fixed set of algorithms (Insertion, QuickSort, MergeSort, HeapSort, BubbleSort, Selection, Shell, Introspective) over fresh copies of `array` and prints a timing / comparison / swap table. The original `array` is left unchanged.

**Parameters**:  
- `array`: Input array of element pointers (read-only).
- `size`: Number of elements.
- `compare`: Comparator.
- `swap`: Swap callback.
- `print`: Print callback (currently unused in output; kept for API symmetry).

**Return Value**:  
None.

**Usage Case**:  
One-shot performance comparison without writing any benchmarking boilerplate.

---

### `int sort_compare_int(const ElementType a, const ElementType b)`

**Purpose**:  
Dereferences both pointers as `int*` and returns a three-way comparison result.

**Parameters**:  
- `a`, `b`: `ElementType` values, each expected to be an `int*`.

**Return Value**:  
Negative if `*a < *b`, `0` if equal, positive if `*a > *b`. `NULL` is treated as the minimum value.

**Usage Case**:  
Pass as the `compare` argument when sorting `int*` elements.

---

### `int sort_compare_string(const ElementType a, const ElementType b)`

**Purpose**:  
Compares two C strings using `strcmp`.

**Parameters**:  
- `a`, `b`: `ElementType` values, each treated as a `char*`.

**Return Value**:  
Result of `strcmp(a, b)`.

**Usage Case**:  
Pass as `compare` when sorting `char*` elements.

---

### `int sort_compare_float(const ElementType a, const ElementType b)`

**Purpose**:  
Dereferences both pointers as `float*` and compares the values.

**Parameters**:  
- `a`, `b`: `ElementType` values, each expected to be a `float*`.

**Return Value**:  
Negative / 0 / positive.

**Usage Case**:  
Pass as `compare` when sorting `float*` elements.

---

### `int sort_compare_double(const ElementType a, const ElementType b)`

**Purpose**:  
Dereferences both pointers as `double*` and compares the values.

**Parameters**:  
- `a`, `b`: `ElementType` values, each expected to be a `double*`.

**Return Value**:  
Negative / 0 / positive.

**Usage Case**:  
Pass as `compare` when sorting `double*` elements.

---

### `int sort_compare_pointer(const ElementType a, const ElementType b)`

**Purpose**:  
Compares two element pointers by their raw address (identity ordering).

**Parameters**:  
- `a`, `b`: Any `ElementType` values.

**Return Value**:  
Negative / 0 / positive based on pointer magnitude.

**Usage Case**:  
Sorting arrays of pointers by address — useful for deduplication.

---

### `void sort_swap_generic(ElementType a, ElementType b)`

**Purpose**:  
Swaps the `ElementType` values held in two slots of an `ElementType[]` array by exchanging the pointers.

**Parameters**:  
- `a`: Address of the first slot (`&array[i]`).
- `b`: Address of the second slot (`&array[j]`).

**Return Value**:  
None.

**Usage Case**:  
The standard swap to pass to any sort function in this library.

---

### `void sort_print_int(const ElementType element)`

**Purpose**:  
Prints the `int` value pointed to by `element` using `printf("%d", ...)`.

**Parameters**:  
- `element`: `ElementType` expected to be an `int*`.

**Return Value**:  
None.

**Usage Case**:  
Pass as `print` to `sort_print_array` when the array holds `int*` elements.

---

### `void sort_print_string(const ElementType element)`

**Purpose**:  
Prints the `char*` string pointed to by `element` wrapped in double quotes.

**Parameters**:  
- `element`: `ElementType` expected to be a `char*`.

**Return Value**:  
None.

**Usage Case**:  
Pass as `print` to `sort_print_array` when the array holds `char*` elements.

---

### `void sort_print_float(const ElementType element)`

**Purpose**:  
Prints the `float` value pointed to by `element` with 2 decimal places.

**Parameters**:  
- `element`: `ElementType` expected to be a `float*`.

**Return Value**:  
None.

**Usage Case**:  
Pass as `print` to `sort_print_array` when the array holds `float*` elements.

---

### `void sort_print_double(const ElementType element)`

**Purpose**:  
Prints the `double` value pointed to by `element` with 2 decimal places.

**Parameters**:  
- `element`: `ElementType` expected to be a `double*`.

**Return Value**:  
None.

**Usage Case**:  
Pass as `print` to `sort_print_array` when the array holds `double*` elements.

---

### `void sort_print_pointer(const ElementType element)`

**Purpose**:  
Prints the raw pointer address of `element` using `printf("%p", ...)`.

**Parameters**:  
- `element`: Any `ElementType`.

**Return Value**:  
None.

**Usage Case**:  
Debugging pointer arrays or verifying that a pointer-based sort produced the expected ordering.

---

## Examples

---

### Example 1 : Insertion Sort on integers

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[9];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    fmt_printf("Before: ");
    sort_print_array(arr, n, sort_print_int);

    sort_insertion(arr, n, sort_compare_int, sort_swap_generic);

    fmt_printf("After:  ");
    sort_print_array(arr, n, sort_print_int);

    return 0;
}
```
**Result:**
```
Before: [5, 3, 8, 1, 9, 2, 7, 4, 6]
After:  [1, 2, 3, 4, 5, 6, 7, 8, 9]
```

---

### Example 2 : QuickSort on integers

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {64, 34, 25, 12, 22, 11, 90};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[7];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    fmt_printf("Before: ");
    sort_print_array(arr, n, sort_print_int);

    sort_quicksort(arr, n, sort_compare_int, sort_swap_generic);

    fmt_printf("After:  ");
    sort_print_array(arr, n, sort_print_int);

    return 0;
}
```
**Result:**
```
Before: [64, 34, 25, 12, 22, 11, 90]
After:  [11, 12, 22, 25, 34, 64, 90]
```

---

### Example 3 : MergeSort on strings

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    const char *words[] = {"banana", "apple", "cherry", "date", "elderberry"};
    size_t n = sizeof(words) / sizeof(words[0]);

    ElementType arr[5];
    for (size_t i = 0; i < n; i++) {
        arr[i] = (void*)words[i];
    }

    fmt_printf("Before: ");
    sort_print_array(arr, n, sort_print_string);

    sort_mergesort(arr, n, sort_compare_string, sort_swap_generic);

    fmt_printf("After:  ");
    sort_print_array(arr, n, sort_print_string);

    return 0;
}
```
**Result:**
```
Before: ["banana", "apple", "cherry", "date", "elderberry"]
After:  ["apple", "banana", "cherry", "date", "elderberry"]
```

---

### Example 4 : HeapSort on doubles

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    double vals[] = {3.14, 1.41, 2.71, 0.58, 1.73};
    size_t n = sizeof(vals) / sizeof(vals[0]);

    ElementType arr[5];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &vals[i];
    }

    fmt_printf("Before: ");
    sort_print_array(arr, n, sort_print_double);

    sort_heapsort(arr, n, sort_compare_double, sort_swap_generic);

    fmt_printf("After:  ");
    sort_print_array(arr, n, sort_print_double);

    return 0;
}
```
**Result:**
```
Before: [3.14, 1.41, 2.71, 0.58, 1.73]
After:  [0.58, 1.41, 1.73, 2.71, 3.14]
```

---

### Example 5 : `sort_generic` with `SortStats`

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {7, 2, 9, 4, 1, 6, 3, 8, 5};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[9];
    for (size_t i = 0; i < n; i++) arr[i] = &nums[i];

    SortStats stats;
    sort_generic(arr, n, sort_compare_int, sort_swap_generic, SORT_MERGESORT, &stats);

    fmt_printf("Sorted: ");
    sort_print_array(arr, n, sort_print_int);

    sort_print_stats(&stats);

    return 0;
}
```
**Result:**
```
Sorted: [1, 2, 3, 4, 5, 6, 7, 8, 9]
Sort Statistics:
  Comparisons: 20
  Swaps: 0
  Recursive calls: 0
  Execution time: 0.000 ms
  Stable: No
  In-place: No
```

---

### Example 6 : BubbleSort + `sort_is_sorted`

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {5, 1, 4, 2, 8};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[5];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    fmt_printf("Is sorted before: %s\n", sort_is_sorted(arr, n, sort_compare_int) ? "Yes" : "No");

    sort_bubblesort(arr, n, sort_compare_int, sort_swap_generic);

    fmt_printf("Sorted: ");
    sort_print_array(arr, n, sort_print_int);

    fmt_printf("Is sorted after:  %s\n", sort_is_sorted(arr, n, sort_compare_int) ? "Yes" : "No");

    return 0;
}
```
**Result:**
```
Is sorted before: No
Sorted: [1, 2, 4, 5, 8]
Is sorted after:  Yes
```

---

### Example 7 : Counting Sort and Radix Sort

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    /* Counting sort — small integer range */
    int c[] = {4, 2, 1, 4, 1, 4, 2, 1, 3, 2};
    size_t cn = sizeof(c) / sizeof(c[0]);
    ElementType ca[10];

    for (size_t i = 0; i < cn; i++) {
        ca[i] = &c[i];
    }

    fmt_printf("Counting sort before: ");
    sort_print_array(ca, cn, sort_print_int);
    sort_counting(ca, cn, sort_compare_int, sort_swap_generic);

    fmt_printf("Counting sort after:  ");
    sort_print_array(ca, cn, sort_print_int);

    /* Radix sort — larger numbers */
    int r[] = {170, 45, 75, 90, 802, 24, 2, 66};
    size_t rn = sizeof(r) / sizeof(r[0]);
    ElementType ra[8];

    for (size_t i = 0; i < rn; i++) {
        ra[i] = &r[i];
    }

    fmt_printf("Radix sort before: ");
    sort_print_array(ra, rn, sort_print_int);
    sort_radix(ra, rn, sort_compare_int, sort_swap_generic);

    fmt_printf("Radix sort after:  ");
    sort_print_array(ra, rn, sort_print_int);

    return 0;
}
```
**Result:**
```
Counting sort before: [4, 2, 1, 4, 1, 4, 2, 1, 3, 2]
Counting sort after:  [1, 1, 1, 2, 2, 2, 3, 4, 4, 4]
Radix sort before: [170, 45, 75, 90, 802, 24, 2, 66]
Radix sort after:  [2, 24, 45, 66, 75, 90, 170, 802]
```

---

### Example 8 : `sort_reverse`

```c
#include "sort/sort.h"
#include "fmt/fmt.h"


int main() {
    int nums[] = {1, 2, 3, 4, 5};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[5];
    for (size_t i = 0; i < n; i++) arr[i] = &nums[i];

    fmt_printf("Original:  ");
    sort_print_array(arr, n, sort_print_int);

    sort_reverse(arr, n, sort_swap_generic);

    fmt_printf("Reversed:  ");
    sort_print_array(arr, n, sort_print_int);

    return 0;
}
```
**Result:**
```
Original:  [1, 2, 3, 4, 5]
Reversed:  [5, 4, 3, 2, 1]
```

---

### Example 9 : `sort_binary_search`

```c
#include "sort/sort.h"
#include "fmt/fmt.h"
#include <stdint.h>


int main() {
    int nums[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[9];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    int t1 = 30, t2 = 75;
    size_t idx1 = sort_binary_search(arr, n, &t1, sort_compare_int);
    size_t idx2 = sort_binary_search(arr, n, &t2, sort_compare_int);

    fmt_printf("Search 30: found at index %zu\n", idx1);
    fmt_printf("Search 75: %s\n", idx2 == SIZE_MAX ? "not found" : "found");

    return 0;
}
```
**Result:**
```
Search 30: found at index 2
Search 75: not found
```

---

### Example 10 : `sort_lower_bound` and `sort_upper_bound`

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {10, 20, 20, 20, 30, 40, 50};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[7];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    int target = 20;
    size_t lo = sort_lower_bound(arr, n, &target, sort_compare_int);
    size_t hi = sort_upper_bound(arr, n, &target, sort_compare_int);

    fmt_printf("lower_bound(20) = %zu\n", lo);
    fmt_printf("upper_bound(20) = %zu\n", hi);
    fmt_printf("count of 20s    = %zu\n", hi - lo);

    return 0;
}
```
**Result:**
```
lower_bound(20) = 1
upper_bound(20) = 4
count of 20s    = 3
```

---

### Example 11 : `sort_partial_sort` — top-K smallest

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {50, 10, 40, 20, 30, 60, 70, 80, 90, 100};
    size_t n = sizeof(nums) / sizeof(nums[0]);
    size_t k = 4;

    ElementType arr[10];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    sort_partial_sort(arr, n, k, sort_compare_int, sort_swap_generic);

    fmt_printf("Smallest %zu elements (sorted):\n", k);
    for (size_t i = 0; i < k; i++) {
        fmt_printf("  arr[%zu] = %d\n", i, *(int*)arr[i]);
    }

    return 0;
}
```
**Result:**
```
Smallest 4 elements (sorted):
  arr[0] = 10
  arr[1] = 20
  arr[2] = 30
  arr[3] = 40
```

---

### Example 12 : `sort_nth_element` — kth smallest in O(n)

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {7, 2, 9, 4, 1, 6, 3, 8, 5};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[9];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    /* index 4 in a 9-element array -> 5th smallest */
    ElementType nth = sort_nth_element(arr, n, 4, sort_compare_int, sort_swap_generic);
    fmt_printf("5th smallest (index 4) = %d\n", *(int*)nth);

    return 0;
}
```
**Result:**
```
5th smallest (index 4) = 5
```

---

### Example 13 : `sort_median`

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    /* 9 elements; sorted: 1,1,2,3,4,5,5,6,9 -> median at index 4 = 4 */
    int nums[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[9];
    for (size_t i = 0; i < n; i++) arr[i] = &nums[i];

    ElementType med = sort_median(arr, n, sort_compare_int, sort_swap_generic);
    fmt_printf("Median = %d\n", *(int*)med);

    return 0;
}
```
**Result:**
```
Median = 4
```

---

### Example 14 : `sort_binary_search_range` — equal range

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main() {
    int nums[] = {5, 10, 10, 10, 15, 20, 20, 25};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[8];
    for (size_t i = 0; i < n; i++) arr[i] = &nums[i];

    int target = 10;
    size_t count = 0;
    ElementType *range = sort_binary_search_range(arr, n, &target, sort_compare_int, &count);

    if (range) {
        fmt_printf("Found %zu occurrence(s) of %d\n", count, target);
        for (size_t i = 0; i < count; i++) {
            fmt_printf("  range[%zu] = %d\n", i, *(int*)range[i]);
        }
    }

    int missing = 99;
    size_t miss_count = 0;
    ElementType *miss = sort_binary_search_range(arr, n, &missing, sort_compare_int, &miss_count);

    fmt_printf("Search for 99: %s\n", miss ? "found" : "not found");

    return 0;
}
```
**Result:**
```
Found 3 occurrence(s) of 10
  range[0] = 10
  range[1] = 10
  range[2] = 10
Search for 99: not found
```

---

### Example 15 : `sort_merge_sorted_arrays` — merge two sorted arrays

```c
#include "sort/sort.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    int a[] = {1, 3, 5, 7, 9};
    int b[] = {2, 4, 6, 8, 10};
    size_t na = sizeof(a) / sizeof(a[0]);
    size_t nb = sizeof(b) / sizeof(b[0]);

    ElementType arr_a[5], arr_b[5];
    for (size_t i = 0; i < na; i++) arr_a[i] = &a[i];
    for (size_t i = 0; i < nb; i++) arr_b[i] = &b[i];

    size_t result_size = 0;
    ElementType *merged = sort_merge_sorted_arrays(
        arr_a, na, arr_b, nb,
        sort_compare_int, sort_swap_generic,
        &result_size);

    fmt_printf("Merged (%zu elements): ", result_size);
    sort_print_array(merged, result_size, sort_print_int);

    free(merged);
    return 0;
}
```
**Result:**
```
Merged (10 elements): [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
```

---

### Example 16 : `sort_is_sorted_until` + `sort_unique` — diagnose order, then de-duplicate

`sort_is_sorted_until` reports exactly where ordering breaks (not just *whether* it
holds), and `sort_unique` completes the classic "sort, then unique" pipeline by
collapsing each run of equal elements to one — turning raw data into a sorted,
distinct set.

```c
#include "sort/sort.h"
#include "fmt/fmt.h"

int main(void) {
    /* Raw data with duplicates and an element out of order right away. */
    int nums[] = {5, 3, 8, 3, 1, 8, 5, 1, 9, 3};
    size_t n = sizeof(nums) / sizeof(nums[0]);

    ElementType arr[10];
    for (size_t i = 0; i < n; i++) {
        arr[i] = &nums[i];
    }

    /* Before sorting, report where the order first breaks. */
    fmt_printf("sorted prefix ends at index %zu of %zu\n",
               sort_is_sorted_until(arr, n, sort_compare_int), n);

    /* sort, then unique -> a sorted, de-duplicated set. */
    sort_quicksort(arr, n, sort_compare_int, sort_swap_generic);
    size_t m = sort_unique(arr, n, sort_compare_int);

    fmt_printf("distinct count: %zu\n", m);
    fmt_printf("distinct values:");
    for (size_t i = 0; i < m; i++) {
        fmt_printf(" %d", *(int*)arr[i]);
    }
    fmt_printf("\n");

    /* The de-duplicated front is now fully sorted. */
    fmt_printf("fully sorted now? %s\n",
               sort_is_sorted_until(arr, m, sort_compare_int) == m ? "yes" : "no");

    return 0;
}
```
**Result:**
```
sorted prefix ends at index 1 of 10
distinct count: 5
distinct values: 1 3 5 8 9
fully sorted now? yes
```

---

## License

This library is licensed under the ISC License.
