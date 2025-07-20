/**
 * @author Amin Tahmasebi
 * @date 2025
 * @class Sort
*/

#ifndef SORT_H_
#define SORT_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif 

#define SORT_LOGGING_ENABLE

#ifdef SORT_LOGGING_ENABLE 
    #define SORT_LOG(fmt, ...) \
        do { fprintf(stderr, "[SORT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SORT_LOG(fmt, ...) do { } while (0)
#endif

// Generic element type
typedef void* ElementType;

// Function pointer types
typedef int (*CompareFunc)(const ElementType, const ElementType);
typedef void (*SwapFunc)(ElementType, ElementType);
typedef void (*PrintFunc)(const ElementType);

// Sorting algorithms enum
typedef enum {
    SORT_INSERTION,
    SORT_QUICKSORT,
    SORT_MERGESORT,
    SORT_HEAPSORT,
    SORT_BUBBLESORT,
    SORT_SELECTION,
    SORT_SHELL,
    SORT_COUNTING,
    SORT_RADIX,
    SORT_BUCKET,
    SORT_INTROSPECTIVE
} SortAlgorithm;

// Sort statistics structure
typedef struct {
    size_t comparisons;
    size_t swaps;
    size_t recursive_calls;
    double execution_time_ms;
    bool stable;
    bool in_place;
} SortStats;

// Constructor and destructor
void sort_init_stats(SortStats* stats);

// Basic sorting functions
void sort_insertion(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_bubblesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_selection(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_shell(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_counting(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_radix(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_bucket(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_introspective(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Generic sort function with algorithm selection
void sort_generic(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap, 
                  SortAlgorithm algorithm, SortStats* stats);

// Utility functions
bool sort_is_sorted(ElementType* array, size_t size, CompareFunc compare);
void sort_reverse(ElementType* array, size_t size, SwapFunc swap);
void sort_shuffle(ElementType* array, size_t size, SwapFunc swap);
ElementType* sort_partial_sort(ElementType* array, size_t size, size_t k, 
                              CompareFunc compare, SwapFunc swap);
ElementType sort_median(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
ElementType sort_nth_element(ElementType* array, size_t size, size_t n, 
                           CompareFunc compare, SwapFunc swap);

// Search functions
size_t sort_binary_search(ElementType* array, size_t size, ElementType target, CompareFunc compare);
size_t sort_lower_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare);
size_t sort_upper_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare);
ElementType* sort_binary_search_range(ElementType* array, size_t size, ElementType target, 
                                    CompareFunc compare, size_t* count);

// Merge functions
void sort_merge(ElementType* array, size_t left, size_t mid, size_t right, 
                CompareFunc compare, SwapFunc swap);
void sort_merge_inplace(ElementType* array, size_t left, size_t mid, size_t right, 
                       CompareFunc compare, SwapFunc swap);
ElementType* sort_merge_sorted_arrays(ElementType* arr1, size_t size1, 
                                    ElementType* arr2, size_t size2,
                                    CompareFunc compare, SwapFunc swap, size_t* result_size);

// Heap functions
void sort_heapify(ElementType* array, size_t size, size_t index, CompareFunc compare, SwapFunc swap);
void sort_build_heap(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_heap_insert(ElementType* array, size_t* size, ElementType element, 
                     CompareFunc compare, SwapFunc swap);
ElementType sort_heap_extract_max(ElementType* array, size_t* size, 
                                CompareFunc compare, SwapFunc swap);

// Partition functions
size_t sort_partition(ElementType* array, size_t left, size_t right, 
                     CompareFunc compare, SwapFunc swap);
size_t sort_partition_three_way(ElementType* array, size_t left, size_t right, 
                               ElementType pivot, CompareFunc compare, SwapFunc swap);
size_t sort_partition_median_of_three(ElementType* array, size_t left, size_t right, 
                                    CompareFunc compare, SwapFunc swap);

// Utility and helper functions
void sort_print_array(ElementType* array, size_t size, PrintFunc print);
void sort_print_stats(const SortStats* stats);
void sort_reset_stats(SortStats* stats);
void sort_benchmark_algorithms(ElementType* array, size_t size, CompareFunc compare, 
                              SwapFunc swap, PrintFunc print);

// Default comparison functions for common types
int sort_compare_int(const ElementType a, const ElementType b);
int sort_compare_string(const ElementType a, const ElementType b);
int sort_compare_float(const ElementType a, const ElementType b);
int sort_compare_double(const ElementType a, const ElementType b);
int sort_compare_pointer(const ElementType a, const ElementType b);

// Default swap function
void sort_swap_generic(ElementType a, ElementType b);

// Default print functions
void sort_print_int(const ElementType element);
void sort_print_string(const ElementType element);
void sort_print_float(const ElementType element);
void sort_print_double(const ElementType element);
void sort_print_pointer(const ElementType element);

// Algorithm-specific optimizations
void sort_quicksort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_mergesort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_introspective_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Parallel sorting (basic implementation)
void sort_parallel_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_parallel_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Stable sorting variants
void sort_stable_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_stable_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Adaptive sorting
void sort_adaptive(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_timsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

#ifdef __cplusplus 
}
#endif 

#endif 