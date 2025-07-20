# Sort Library

A comprehensive, generic sorting library for C that provides multiple sorting algorithms with performance statistics, benchmarking, and search capabilities.

## Features

- **Multiple Sorting Algorithms**: Insertion, QuickSort, MergeSort, HeapSort, BubbleSort, Selection, Shell, Introspective
- **Specialized Algorithms**: Counting Sort, Radix Sort, Bucket Sort
- **Optimized Variants**: Optimized versions of QuickSort, MergeSort, and Introspective Sort
- **Adaptive Sorting**: Automatically chooses the best algorithm based on data characteristics
- **Performance Statistics**: Tracks comparisons, swaps, execution time
- **Search Functions**: Binary search, lower bound, upper bound
- **Utility Functions**: Array validation, reverse, shuffle
- **Benchmarking**: Compare algorithm performance
- **Generic Design**: Works with any data type using function pointers

## API Overview

### Core Functions

```c
// Generic sorting with algorithm selection
void sort_generic(ElementType* array, size_t size, CompareFunc compare, 
                  SwapFunc swap, SortAlgorithm algorithm, SortStats* stats);

// Individual sorting algorithms
void sort_insertion(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_bubblesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_selection(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_shell(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_introspective(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Specialized algorithms
void sort_counting(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_radix(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_bucket(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Optimized variants
void sort_quicksort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_mergesort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);
void sort_introspective_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Adaptive sorting
void sort_adaptive(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap);

// Search functions
size_t sort_binary_search(ElementType* array, size_t size, ElementType target, CompareFunc compare);
size_t sort_lower_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare);
size_t sort_upper_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare);

// Utility functions
bool sort_is_sorted(ElementType* array, size_t size, CompareFunc compare);
void sort_reverse(ElementType* array, size_t size, SwapFunc swap);
void sort_shuffle(ElementType* array, size_t size, SwapFunc swap);
void sort_print_array(ElementType* array, size_t size, PrintFunc print);

// Benchmarking
void sort_benchmark_algorithms(ElementType* array, size_t size, CompareFunc compare, 
                              SwapFunc swap, PrintFunc print);
```

### Built-in Comparison and Utility Functions

```c
// Comparison functions
int sort_compare_int(const ElementType a, const ElementType b);
int sort_compare_string(const ElementType a, const ElementType b);
int sort_compare_float(const ElementType a, const ElementType b);
int sort_compare_double(const ElementType a, const ElementType b);
int sort_compare_pointer(const ElementType a, const ElementType b);

// Utility functions
void sort_swap_generic(ElementType a, ElementType b);
void sort_print_int(const ElementType element);
void sort_print_string(const ElementType element);
void sort_print_float(const ElementType element);
void sort_print_double(const ElementType element);
void sort_print_pointer(const ElementType element);
```

## Complete Example

Here's a comprehensive example demonstrating all the sort library functionality:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fmt/fmt.h"
#include "sort/sort.h"

void print_separator(const char* title) {
    int len = strlen(title) + 4;
    fmt_printf("\n");
    for (int i = 0; i < len; i++) fmt_printf("=");
    fmt_printf("\n");
    fmt_printf("= %s =\n", title);
    for (int i = 0; i < len; i++) fmt_printf("=");
    fmt_printf("\n");
}

// Basic Algorithms Demo
void demo_sort_basic_algorithms() {
    print_separator("Sort Library - Basic Algorithms Demo");
    
    // Create test arrays
    int numbers[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 54};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    ElementType* array = malloc(size * sizeof(ElementType));
    for (size_t i = 0; i < size; i++) {
        array[i] = &numbers[i];
    }
    
    fmt_printf("Original array: ");
    sort_print_array(array, size, sort_print_int);
    
    // Test different sorting algorithms
    SortAlgorithm algorithms[] = {
        SORT_INSERTION, SORT_QUICKSORT, SORT_MERGESORT, SORT_HEAPSORT,
        SORT_BUBBLESORT, SORT_SELECTION, SORT_SHELL, SORT_INTROSPECTIVE
    };
    
    const char* algorithm_names[] = {
        "Insertion", "QuickSort", "MergeSort", "HeapSort",
        "BubbleSort", "Selection", "Shell", "Introspective"
    };
    
    for (size_t i = 0; i < sizeof(algorithms) / sizeof(algorithms[0]); i++) {
        // Reset array to original state
        for (size_t j = 0; j < size; j++) {
            numbers[j] = (int[]){64, 34, 25, 12, 22, 11, 90, 88, 76, 54}[j];
        }
        
        SortStats stats;
        sort_generic(array, size, sort_compare_int, sort_swap_generic, 
                     algorithms[i], &stats);
        
        fmt_printf("%s Sort: ", algorithm_names[i]);
        sort_print_array(array, size, sort_print_int);
        fmt_printf("  Comparisons: %zu, Swaps: %zu, Time: %.3f ms\n", 
                   stats.comparisons, stats.swaps, stats.execution_time_ms);
    }
    
    free(array);
}

// Advanced Features Demo
void demo_sort_advanced_features() {
    print_separator("Sort Library - Advanced Features Demo");
    
    // String sorting
    char* strings[] = {"banana", "apple", "cherry", "date", "elderberry"};
    size_t string_size = sizeof(strings) / sizeof(strings[0]);
    
    ElementType* string_array = malloc(string_size * sizeof(ElementType));
    for (size_t i = 0; i < string_size; i++) {
        string_array[i] = strings[i];
    }
    
    fmt_printf("Original strings: ");
    sort_print_array(string_array, string_size, sort_print_string);
    
    sort_mergesort(string_array, string_size, sort_compare_string, sort_swap_generic);
    
    fmt_printf("Sorted strings: ");
    sort_print_array(string_array, string_size, sort_print_string);
    
    // Float sorting
    float floats[] = {3.14f, 2.71f, 1.41f, 0.58f, 2.23f};
    size_t float_size = sizeof(floats) / sizeof(floats[0]);
    
    ElementType* float_array = malloc(float_size * sizeof(ElementType));
    for (size_t i = 0; i < float_size; i++) {
        float_array[i] = &floats[i];
    }
    
    fmt_printf("Original floats: ");
    sort_print_array(float_array, float_size, sort_print_float);
    
    sort_quicksort(float_array, float_size, sort_compare_float, sort_swap_generic);
    
    fmt_printf("Sorted floats: ");
    sort_print_array(float_array, float_size, sort_print_float);
    
    free(string_array);
    free(float_array);
}

// Benchmarking Demo
void demo_sort_benchmarking() {
    print_separator("Sort Library - Benchmarking Demo");
    
    // Create a larger array for benchmarking
    size_t size = 1000;
    ElementType* array = malloc(size * sizeof(ElementType));
    int* numbers = malloc(size * sizeof(int));
    
    // Initialize with random data
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < size; i++) {
        numbers[i] = rand() % 10000;
        array[i] = &numbers[i];
    }
    
    fmt_printf("Benchmarking sorting algorithms with %zu elements:\n", size);
    fmt_printf("================================================\n");
    
    sort_benchmark_algorithms(array, size, sort_compare_int, 
                             sort_swap_generic, sort_print_int);
    
    free(array);
    free(numbers);
}

// Search Functions Demo
void demo_sort_search_functions() {
    print_separator("Sort Library - Search Functions Demo");
    
    // Create a sorted array for searching
    int numbers[] = {11, 12, 22, 25, 34, 64, 90, 100, 150, 200};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    ElementType* array = malloc(size * sizeof(ElementType));
    for (size_t i = 0; i < size; i++) {
        array[i] = &numbers[i];
    }
    
    fmt_printf("Sorted array: ");
    sort_print_array(array, size, sort_print_int);
    
    // Test binary search
    int targets[] = {25, 64, 99, 200};
    for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
        size_t index = sort_binary_search(array, size, &targets[i], sort_compare_int);
        if (index != SIZE_MAX) {
            fmt_printf("Binary search: Found %d at index %zu\n", targets[i], index);
        } else {
            fmt_printf("Binary search: %d not found\n", targets[i]);
        }
    }
    
    // Test lower and upper bounds
    int search_value = 25;
    size_t lower = sort_lower_bound(array, size, &search_value, sort_compare_int);
    size_t upper = sort_upper_bound(array, size, &search_value, sort_compare_int);
    
    fmt_printf("Lower bound for %d: %zu\n", search_value, lower);
    fmt_printf("Upper bound for %d: %zu\n", search_value, upper);
    
    free(array);
}

// Utility Functions Demo
void demo_sort_utility_functions() {
    print_separator("Sort Library - Utility Functions Demo");
    
    // Test array operations
    int numbers[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 54};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    ElementType* array = malloc(size * sizeof(ElementType));
    for (size_t i = 0; i < size; i++) {
        array[i] = &numbers[i];
    }
    
    fmt_printf("Original array: ");
    sort_print_array(array, size, sort_print_int);
    
    // Test is_sorted
    fmt_printf("Is sorted: %s\n", sort_is_sorted(array, size, sort_compare_int) ? "Yes" : "No");
    
    // Sort the array
    sort_quicksort(array, size, sort_compare_int, sort_swap_generic);
    fmt_printf("After sorting: ");
    sort_print_array(array, size, sort_print_int);
    fmt_printf("Is sorted: %s\n", sort_is_sorted(array, size, sort_compare_int) ? "Yes" : "No");
    
    // Test reverse
    sort_reverse(array, size, sort_swap_generic);
    fmt_printf("After reverse: ");
    sort_print_array(array, size, sort_print_int);
    
    // Test shuffle
    sort_shuffle(array, size, sort_swap_generic);
    fmt_printf("After shuffle: ");
    sort_print_array(array, size, sort_print_int);
    
    free(array);
}

// Specialized Algorithms Demo
void demo_sort_specialized_algorithms() {
    print_separator("Sort Library - Specialized Algorithms Demo");
    
    // Test Counting Sort with small range integers
    int numbers[] = {4, 2, 1, 4, 1, 4, 2, 1, 3, 2};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    ElementType* array = malloc(size * sizeof(ElementType));
    for (size_t i = 0; i < size; i++) {
        array[i] = &numbers[i];
    }
    
    fmt_printf("Original array (for counting sort): ");
    sort_print_array(array, size, sort_print_int);
    
    sort_counting(array, size, sort_compare_int, sort_swap_generic);
    fmt_printf("After counting sort: ");
    sort_print_array(array, size, sort_print_int);
    
    free(array);
    
    // Test Radix Sort with larger numbers
    int radix_numbers[] = {170, 45, 75, 90, 802, 24, 2, 66};
    size_t radix_size = sizeof(radix_numbers) / sizeof(radix_numbers[0]);
    
    ElementType* radix_array = malloc(radix_size * sizeof(ElementType));
    for (size_t i = 0; i < radix_size; i++) {
        radix_array[i] = &radix_numbers[i];
    }
    
    fmt_printf("Original array (for radix sort): ");
    sort_print_array(radix_array, radix_size, sort_print_int);
    
    sort_radix(radix_array, radix_size, sort_compare_int, sort_swap_generic);
    fmt_printf("After radix sort: ");
    sort_print_array(radix_array, radix_size, sort_print_int);
    
    free(radix_array);
}

// Optimized Variants Demo
void demo_sort_optimized_variants() {
    print_separator("Sort Library - Optimized Variants Demo");
    
    // Test optimized algorithms
    int numbers[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 54, 33, 44, 55, 66, 77};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    ElementType* array = malloc(size * sizeof(ElementType));
    for (size_t i = 0; i < size; i++) {
        array[i] = &numbers[i];
    }
    
    fmt_printf("Original array: ");
    sort_print_array(array, size, sort_print_int);
    
    // Test optimized quicksort
    sort_quicksort_optimized(array, size, sort_compare_int, sort_swap_generic);
    fmt_printf("After optimized quicksort: ");
    sort_print_array(array, size, sort_print_int);
    
    // Reset array
    for (size_t i = 0; i < size; i++) {
        numbers[i] = (int[]){64, 34, 25, 12, 22, 11, 90, 88, 76, 54, 33, 44, 55, 66, 77}[i];
    }
    
    // Test optimized mergesort
    sort_mergesort_optimized(array, size, sort_compare_int, sort_swap_generic);
    fmt_printf("After optimized mergesort: ");
    sort_print_array(array, size, sort_print_int);
    
    // Reset array
    for (size_t i = 0; i < size; i++) {
        numbers[i] = (int[]){64, 34, 25, 12, 22, 11, 90, 88, 76, 54, 33, 44, 55, 66, 77}[i];
    }
    
    // Test introspective sort
    sort_introspective_optimized(array, size, sort_compare_int, sort_swap_generic);
    fmt_printf("After introspective sort: ");
    sort_print_array(array, size, sort_print_int);
    
    free(array);
}

// Adaptive Sorting Demo
void demo_sort_adaptive_sorting() {
    print_separator("Sort Library - Adaptive Sorting Demo");
    
    // Test adaptive sorting with different array sizes
    int small_numbers[] = {5, 2, 8, 1, 9};
    int medium_numbers[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 54, 33, 44, 55, 66, 77};
    int large_numbers[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000,
                          110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
                          210, 220, 230, 240, 250, 260, 270, 280, 290, 300};
    
    size_t small_size = sizeof(small_numbers) / sizeof(small_numbers[0]);
    size_t medium_size = sizeof(medium_numbers) / sizeof(medium_numbers[0]);
    size_t large_size = sizeof(large_numbers) / sizeof(large_numbers[0]);
    
    // Test small array
    ElementType* small_array = malloc(small_size * sizeof(ElementType));
    for (size_t i = 0; i < small_size; i++) {
        small_array[i] = &small_numbers[i];
    }
    
    fmt_printf("Small array (adaptive sort): ");
    sort_print_array(small_array, small_size, sort_print_int);
    sort_adaptive(small_array, small_size, sort_compare_int, sort_swap_generic);
    fmt_printf("After adaptive sort: ");
    sort_print_array(small_array, small_size, sort_print_int);
    
    // Test medium array
    ElementType* medium_array = malloc(medium_size * sizeof(ElementType));
    for (size_t i = 0; i < medium_size; i++) {
        medium_array[i] = &medium_numbers[i];
    }
    
    fmt_printf("Medium array (adaptive sort): ");
    sort_print_array(medium_array, medium_size, sort_print_int);
    sort_adaptive(medium_array, medium_size, sort_compare_int, sort_swap_generic);
    fmt_printf("After adaptive sort: ");
    sort_print_array(medium_array, medium_size, sort_print_int);
    
    // Test large array
    ElementType* large_array = malloc(large_size * sizeof(ElementType));
    for (size_t i = 0; i < large_size; i++) {
        large_array[i] = &large_numbers[i];
    }
    
    fmt_printf("Large array (adaptive sort): ");
    sort_print_array(large_array, large_size, sort_print_int);
    sort_adaptive(large_array, large_size, sort_compare_int, sort_swap_generic);
    fmt_printf("After adaptive sort: ");
    sort_print_array(large_array, large_size, sort_print_int);
    
    free(small_array);
    free(medium_array);
    free(large_array);
}

int main() {
    fmt_printf("Sort Library Complete Demo\n");
    fmt_printf("=========================\n\n");
    
    // Run all demos
    demo_sort_basic_algorithms();
    demo_sort_advanced_features();
    demo_sort_benchmarking();
    demo_sort_search_functions();
    demo_sort_utility_functions();
    demo_sort_specialized_algorithms();
    demo_sort_optimized_variants();
    demo_sort_adaptive_sorting();
    
    fmt_printf("\nDemo completed successfully!\n");
    return 0;
}
```

## Build Instructions

```bash
# Build the project
mkdir build
cd build
cmake -G "Ninja" ..
cmake --build .

# Run the demo
./main.exe
```

## Output Example

The demo will show:
- All sorting algorithms working with different data types
- Performance statistics for each algorithm
- Search functions (binary search, lower/upper bounds)
- Utility functions (reverse, shuffle, validation)
- Specialized algorithms (counting sort, radix sort)
- Optimized variants and adaptive sorting
- Benchmarking results

## Key Features

1. **Generic Design**: Works with any data type using function pointers
2. **Performance Tracking**: Monitors comparisons, swaps, and execution time
3. **Multiple Algorithms**: 8+ different sorting algorithms
4. **Search Capabilities**: Binary search with bounds
5. **Utility Functions**: Array manipulation and validation
6. **Benchmarking**: Compare algorithm performance
7. **Adaptive Sorting**: Automatically chooses optimal algorithm
8. **Optimized Variants**: Enhanced versions of core algorithms

This comprehensive sort library provides everything needed for efficient data sorting and analysis in C applications. 