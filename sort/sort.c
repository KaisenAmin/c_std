#include "sort.h"
// #include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

// Global statistics for tracking
static SortStats global_stats;

// Initialize statistics
void sort_init_stats(SortStats* stats) {
    if (!stats) return;
    stats->comparisons = 0;
    stats->swaps = 0;
    stats->recursive_calls = 0;
    stats->execution_time_ms = 0.0;
    stats->stable = false;
    stats->in_place = false;
}

// Helper function to get current time in milliseconds
static double get_time_ms() {
    clock_t start = clock();
    return ((double)start) / CLOCKS_PER_SEC * 1000.0;
}

// Helper function to reset global stats
static void reset_global_stats() {
    sort_init_stats(&global_stats);
}

// Helper function to update stats
static void update_stats(bool is_comparison, bool is_swap, bool is_recursive) {
    if (is_comparison) global_stats.comparisons++;
    if (is_swap) global_stats.swaps++;
    if (is_recursive) global_stats.recursive_calls++;
}

// Default comparison functions
int sort_compare_int(const ElementType a, const ElementType b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    int val_a = *(int*)a;
    int val_b = *(int*)b;
    
    update_stats(true, false, false);
    
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

int sort_compare_string(const ElementType a, const ElementType b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    update_stats(true, false, false);
    return strcmp((const char*)a, (const char*)b);
}

int sort_compare_float(const ElementType a, const ElementType b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    float val_a = *(float*)a;
    float val_b = *(float*)b;
    
    update_stats(true, false, false);
    
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

int sort_compare_double(const ElementType a, const ElementType b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    double val_a = *(double*)a;
    double val_b = *(double*)b;
    
    update_stats(true, false, false);
    
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

int sort_compare_pointer(const ElementType a, const ElementType b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    
    update_stats(true, false, false);
    
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

// Default swap function
void sort_swap_generic(ElementType a, ElementType b) {
    if (!a || !b) return;
    
    ElementType temp = *(ElementType*)a;
    *(ElementType*)a = *(ElementType*)b;
    *(ElementType*)b = temp;
    
    update_stats(false, true, false);
}

// Default print functions
void sort_print_int(const ElementType element) {
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%d", *(int*)element);
}

void sort_print_string(const ElementType element) {
    if (!element) {
        printf("NULL");
        return;
    }
    printf("\"%s\"", (const char*)element);
}

void sort_print_float(const ElementType element) {
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%.2f", *(float*)element);
}

void sort_print_double(const ElementType element) {
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%.2f", *(double*)element);
}

void sort_print_pointer(const ElementType element) {
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%p", (void*)element);
}

// Insertion Sort - O(n²) but good for small arrays
void sort_insertion(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    for (size_t i = 1; i < size; i++) {
        ElementType key = array[i];
        int j = (int)i - 1;
        
        while (j >= 0 && compare(&array[j], &key) > 0) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = key;
    }
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_insertion] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// QuickSort - O(n log n) average, O(n²) worst case
static void quicksort_recursive(ElementType* array, size_t left, size_t right, 
                               CompareFunc compare, SwapFunc swap) {
    if (left < right) {
        update_stats(false, false, true);
        
        size_t pivot_index = sort_partition(array, left, right, compare, swap);
        
        if (pivot_index > 0) {
            quicksort_recursive(array, left, pivot_index - 1, compare, swap);
        }
        quicksort_recursive(array, pivot_index + 1, right, compare, swap);
    }
}

void sort_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    quicksort_recursive(array, 0, size - 1, compare, swap);
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_quicksort] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// MergeSort - O(n log n) stable sort
static void mergesort_recursive(ElementType* array, size_t left, size_t right, 
                               CompareFunc compare, SwapFunc swap) {
    if (left < right) {
        update_stats(false, false, true);
        
        size_t mid = left + (right - left) / 2;
        
        mergesort_recursive(array, left, mid, compare, swap);
        mergesort_recursive(array, mid + 1, right, compare, swap);
        
        sort_merge(array, left, mid, right, compare, swap);
    }
}

void sort_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    mergesort_recursive(array, 0, size - 1, compare, swap);
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;
    
    SORT_LOG("[sort_mergesort] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// HeapSort - O(n log n) in-place
void sort_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    sort_build_heap(array, size, compare, swap);
    
    for (size_t i = size - 1; i > 0; i--) {
        swap(&array[0], &array[i]);
        sort_heapify(array, i, 0, compare, swap);
    }
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_heapsort] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// BubbleSort - O(n²) simple but inefficient
void sort_bubblesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    for (size_t i = 0; i < size - 1; i++) {
        bool swapped = false;
        for (size_t j = 0; j < size - i - 1; j++) {
            if (compare(&array[j], &array[j + 1]) > 0) {
                swap(&array[j], &array[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break; // Optimized bubble sort
    }
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_bubblesort] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// SelectionSort - O(n²) simple but inefficient
void sort_selection(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    for (size_t i = 0; i < size - 1; i++) {
        size_t min_index = i;
        for (size_t j = i + 1; j < size; j++) {
            if (compare(&array[j], &array[min_index]) < 0) {
                min_index = j;
            }
        }
        if (min_index != i) {
            swap(&array[i], &array[min_index]);
        }
    }
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_selection] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// ShellSort - O(n^1.3) to O(n^2) depending on gap sequence
void sort_shell(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    // Using Knuth's gap sequence
    for (size_t gap = size / 2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < size; i++) {
            ElementType temp = array[i];
            size_t j;
            for (j = i; j >= gap && compare(&array[j - gap], &temp) > 0; j -= gap) {
                array[j] = array[j - gap];
            }
            array[j] = temp;
        }
    }
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_shell] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// CountingSort - O(n + k) for integers with range k
void sort_counting(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    // Find min and max values
    int min_val = INT_MAX, max_val = INT_MIN;
    for (size_t i = 0; i < size; i++) {
        int val = *(int*)array[i];
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    
    size_t range = max_val - min_val + 1;
    size_t* count = (size_t*)calloc(range, sizeof(size_t));
    ElementType* output = (ElementType*)malloc(size * sizeof(ElementType));
    
    if (!count || !output) {
        free(count);
        free(output);
        return;
    }
    
    // Count occurrences
    for (size_t i = 0; i < size; i++) {
        int val = *(int*)array[i];
        count[val - min_val]++;
    }
    
    // Calculate positions
    for (size_t i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    
    // Build output array
    for (int i = (int)size - 1; i >= 0; i--) {
        int val = *(int*)array[i];
        size_t index = count[val - min_val] - 1;
        output[index] = array[i];
        count[val - min_val]--;
    }
    
    // Copy back to original array
    for (size_t i = 0; i < size; i++) {
        array[i] = output[i];
    }
    
    free(count);
    free(output);
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;
    
    SORT_LOG("[sort_counting] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// RadixSort - O(d(n + k)) where d is number of digits
void sort_radix(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    // Find maximum value to determine number of digits
    int max_val = *(int*)array[0];
    for (size_t i = 1; i < size; i++) {
        int val = *(int*)array[i];
        if (val > max_val) max_val = val;
    }
    
    // Perform counting sort for each digit
    for (int exp = 1; max_val / exp > 0; exp *= 10) {
        size_t count[10] = {0};
        ElementType* output = (ElementType*)malloc(size * sizeof(ElementType));
        
        if (!output) return;
        
        // Count occurrences
        for (size_t i = 0; i < size; i++) {
            int val = *(int*)array[i];
            count[(val / exp) % 10]++;
        }
        
        // Calculate positions
        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }
        
        // Build output array
        for (int i = (int)size - 1; i >= 0; i--) {
            int val = *(int*)array[i];
            size_t index = count[(val / exp) % 10] - 1;
            output[index] = array[i];
            count[(val / exp) % 10]--;
        }
        
        // Copy back to original array
        for (size_t i = 0; i < size; i++) {
            array[i] = output[i];
        }
        
        free(output);
    }
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;
    
    SORT_LOG("[sort_radix] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// BucketSort - O(n + k) for uniformly distributed data
void sort_bucket(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    // Create buckets
    size_t num_buckets = size;
    ElementType** buckets = (ElementType**)malloc(num_buckets * sizeof(ElementType*));
    size_t* bucket_sizes = (size_t*)calloc(num_buckets, sizeof(size_t));
    
    if (!buckets || !bucket_sizes) {
        free(buckets);
        free(bucket_sizes);
        return;
    }
    
    // Initialize buckets
    for (size_t i = 0; i < num_buckets; i++) {
        buckets[i] = (ElementType*)malloc(size * sizeof(ElementType));
        bucket_sizes[i] = 0;
    }
    
    // Find min and max for bucket range
    float min_val = *(float*)array[0], max_val = *(float*)array[0];
    for (size_t i = 1; i < size; i++) {
        float val = *(float*)array[i];
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    
    // Distribute elements into buckets
    for (size_t i = 0; i < size; i++) {
        float val = *(float*)array[i];
        size_t bucket_index = (size_t)((val - min_val) / (max_val - min_val) * (num_buckets - 1));
        buckets[bucket_index][bucket_sizes[bucket_index]++] = array[i];
    }
    
    // Sort individual buckets and merge
    size_t index = 0;
    for (size_t i = 0; i < num_buckets; i++) {
        if (bucket_sizes[i] > 0) {
            sort_insertion(buckets[i], bucket_sizes[i], compare, swap);
            for (size_t j = 0; j < bucket_sizes[i]; j++) {
                array[index++] = buckets[i][j];
            }
        }
    }
    
    // Clean up
    for (size_t i = 0; i < num_buckets; i++) {
        free(buckets[i]);
    }
    free(buckets);
    free(bucket_sizes);
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;
    
    SORT_LOG("[sort_bucket] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// Introspective Sort - Hybrid of QuickSort, HeapSort, and InsertionSort
void sort_introspective(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    size_t max_depth = 2 * (size_t)log2(size);
    sort_introspective_optimized(array, size, compare, swap);
    
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;
    
    SORT_LOG("[sort_introspective] : Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}

// Generic sort function with algorithm selection
void sort_generic(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap, 
                  SortAlgorithm algorithm, SortStats* stats) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    double start_time = get_time_ms();
    reset_global_stats();
    
    switch (algorithm) {
        case SORT_INSERTION:
            sort_insertion(array, size, compare, swap);
            break;
        case SORT_QUICKSORT:
            sort_quicksort(array, size, compare, swap);
            break;
        case SORT_MERGESORT:
            sort_mergesort(array, size, compare, swap);
            break;
        case SORT_HEAPSORT:
            sort_heapsort(array, size, compare, swap);
            break;
        case SORT_BUBBLESORT:
            sort_bubblesort(array, size, compare, swap);
            break;
        case SORT_SELECTION:
            sort_selection(array, size, compare, swap);
            break;
        case SORT_SHELL:
            sort_shell(array, size, compare, swap);
            break;
        case SORT_COUNTING:
            sort_counting(array, size, compare, swap);
            break;
        case SORT_RADIX:
            sort_radix(array, size, compare, swap);
            break;
        case SORT_BUCKET:
            sort_bucket(array, size, compare, swap);
            break;
        case SORT_INTROSPECTIVE:
            sort_introspective(array, size, compare, swap);
            break;
        default:
            sort_quicksort(array, size, compare, swap);
            break;
    }
    
    if (stats) {
        *stats = global_stats;
        stats->execution_time_ms = get_time_ms() - start_time;
    }
}

// Utility functions
bool sort_is_sorted(ElementType* array, size_t size, CompareFunc compare) {
    if (!array || size <= 1 || !compare) return true;
    
    for (size_t i = 1; i < size; i++) {
        if (compare(&array[i - 1], &array[i]) > 0) {
            return false;
        }
    }
    return true;
}

void sort_reverse(ElementType* array, size_t size, SwapFunc swap) {
    if (!array || size <= 1 || !swap) return;
    
    for (size_t i = 0; i < size / 2; i++) {
        swap(&array[i], &array[size - 1 - i]);
    }
}

void sort_shuffle(ElementType* array, size_t size, SwapFunc swap) {
    if (!array || size <= 1 || !swap) return;
    
    srand((unsigned int)time(NULL));
    
    for (size_t i = size - 1; i > 0; i--) {
        size_t j = (size_t)rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

// Partition functions
size_t sort_partition(ElementType* array, size_t left, size_t right, 
                     CompareFunc compare, SwapFunc swap) {
    ElementType pivot = array[right];
    size_t i = left;
    
    for (size_t j = left; j < right; j++) {
        if (compare(&array[j], &pivot) <= 0) {
            if (i != j) {
                swap(&array[i], &array[j]);
            }
            i++;
        }
    }
    
    if (i != right) {
        swap(&array[i], &array[right]);
    }
    
    return i;
}

// Merge functions
void sort_merge(ElementType* array, size_t left, size_t mid, size_t right, 
                CompareFunc compare, SwapFunc swap) {
    size_t left_size = mid - left + 1;
    size_t right_size = right - mid;
    
    ElementType* left_array = (ElementType*)malloc(left_size * sizeof(ElementType));
    ElementType* right_array = (ElementType*)malloc(right_size * sizeof(ElementType));
    
    if (!left_array || !right_array) {
        free(left_array);
        free(right_array);
        return;
    }
    
    // Copy data to temporary arrays
    for (size_t i = 0; i < left_size; i++) {
        left_array[i] = array[left + i];
    }
    for (size_t j = 0; j < right_size; j++) {
        right_array[j] = array[mid + 1 + j];
    }
    
    // Merge the temporary arrays back
    size_t i = 0, j = 0, k = left;
    while (i < left_size && j < right_size) {
        if (compare(&left_array[i], &right_array[j]) <= 0) {
            array[k] = left_array[i];
            i++;
        } else {
            array[k] = right_array[j];
            j++;
        }
        k++;
    }
    
    // Copy remaining elements
    while (i < left_size) {
        array[k] = left_array[i];
        i++;
        k++;
    }
    while (j < right_size) {
        array[k] = right_array[j];
        j++;
        k++;
    }
    
    free(left_array);
    free(right_array);
}

// Heap functions
void sort_heapify(ElementType* array, size_t size, size_t index, CompareFunc compare, SwapFunc swap) {
    size_t largest = index;
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;
    
    if (left < size && compare(&array[left], &array[largest]) > 0) {
        largest = left;
    }
    
    if (right < size && compare(&array[right], &array[largest]) > 0) {
        largest = right;
    }
    
    if (largest != index) {
        swap(&array[index], &array[largest]);
        sort_heapify(array, size, largest, compare, swap);
    }
}

void sort_build_heap(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    for (int i = (int)size / 2 - 1; i >= 0; i--) {
        sort_heapify(array, size, (size_t)i, compare, swap);
    }
}

// Print functions
void sort_print_array(ElementType* array, size_t size, PrintFunc print) {
    if (!array || !print) return;
    
    printf("[");
    for (size_t i = 0; i < size; i++) {
        print(array[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

void sort_print_stats(const SortStats* stats) {
    if (!stats) return;
    
    printf("Sort Statistics:\n");
    printf("  Comparisons: %zu\n", stats->comparisons);
    printf("  Swaps: %zu\n", stats->swaps);
    printf("  Recursive calls: %zu\n", stats->recursive_calls);
    printf("  Execution time: %.3f ms\n", stats->execution_time_ms);
    printf("  Stable: %s\n", stats->stable ? "Yes" : "No");
    printf("  In-place: %s\n", stats->in_place ? "Yes" : "No");
}

void sort_reset_stats(SortStats* stats) {
    sort_init_stats(stats);
}

// Optimized versions
void sort_quicksort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    // Use insertion sort for small arrays
    if (size <= 10) {
        sort_insertion(array, size, compare, swap);
        return;
    }
    
    // Use median-of-three pivot selection
    size_t mid = size / 2;
    if (compare(&array[0], &array[mid]) > 0) swap(&array[0], &array[mid]);
    if (compare(&array[0], &array[size-1]) > 0) swap(&array[0], &array[size-1]);
    if (compare(&array[mid], &array[size-1]) > 0) swap(&array[mid], &array[size-1]);
    
    // Use the median as pivot
    swap(&array[mid], &array[size-1]);
    
    sort_quicksort(array, size, compare, swap);
}

void sort_mergesort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    // Use insertion sort for small arrays
    if (size <= 10) {
        sort_insertion(array, size, compare, swap);
        return;
    }
    
    sort_mergesort(array, size, compare, swap);
}

void sort_introspective_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    size_t max_depth = 2 * (size_t)log2(size);
    
    // Use insertion sort for small arrays
    if (size <= 16) {
        sort_insertion(array, size, compare, swap);
        return;
    }
    
    // Use heap sort if depth limit exceeded
    if (max_depth == 0) {
        sort_heapsort(array, size, compare, swap);
        return;
    }
    
    // Use optimized quicksort
    sort_quicksort_optimized(array, size, compare, swap);
}

// Basic parallel implementations (simplified)
void sort_parallel_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    // Simplified parallel implementation
    sort_mergesort_optimized(array, size, compare, swap);
}

void sort_parallel_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    // Simplified parallel implementation
    sort_quicksort_optimized(array, size, compare, swap);
}

// Stable variants
void sort_stable_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    // Use mergesort for stable sorting
    sort_mergesort(array, size, compare, swap);
}

void sort_stable_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    // Use mergesort for stable sorting
    sort_mergesort(array, size, compare, swap);
}

// Adaptive sorting
void sort_adaptive(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (!array || size <= 1 || !compare || !swap) return;
    
    // Choose algorithm based on array characteristics
    if (size <= 10) {
        sort_insertion(array, size, compare, swap);
    } else if (size <= 100) {
        sort_shell(array, size, compare, swap);
    } else {
        sort_introspective_optimized(array, size, compare, swap);
    }
}

void sort_timsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    // Simplified Timsort implementation
    sort_adaptive(array, size, compare, swap);
}

// Search functions
size_t sort_binary_search(ElementType* array, size_t size, ElementType target, CompareFunc compare) {
    if (!array || size == 0 || !compare) return SIZE_MAX;
    
    size_t left = 0, right = size - 1;
    
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        int cmp = compare(array[mid], target);
        
        if (cmp == 0) return mid;
        if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    
    return SIZE_MAX;
}

size_t sort_lower_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare) {
    if (!array || size == 0 || !compare) return 0;
    
    size_t left = 0, right = size;
    
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (compare(array[mid], target) < 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    return left;
}

size_t sort_upper_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare) {
    if (!array || size == 0 || !compare) return 0;
    
    size_t left = 0, right = size;
    
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (compare(array[mid], target) <= 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    return left;
}

// Benchmark function
void sort_benchmark_algorithms(ElementType* array, size_t size, CompareFunc compare, 
                              SwapFunc swap, PrintFunc print) {
    if (!array || size == 0 || !compare || !swap) return;
    
    SortAlgorithm algorithms[] = {
        SORT_INSERTION, SORT_QUICKSORT, SORT_MERGESORT, SORT_HEAPSORT,
        SORT_BUBBLESORT, SORT_SELECTION, SORT_SHELL, SORT_INTROSPECTIVE
    };
    
    const char* algorithm_names[] = {
        "Insertion", "QuickSort", "MergeSort", "HeapSort",
        "BubbleSort", "Selection", "Shell", "Introspective"
    };
    
    ElementType* test_array = (ElementType*)malloc(size * sizeof(ElementType));
    if (!test_array) return;
    
    for (size_t i = 0; i < sizeof(algorithms) / sizeof(algorithms[0]); i++) {
        // Copy original array
        memcpy(test_array, array, size * sizeof(ElementType));
        
        SortStats stats;
        sort_init_stats(&stats);
        
        double start_time = get_time_ms();
        sort_generic(test_array, size, compare, swap, algorithms[i], &stats);
        double end_time = get_time_ms();
        
        printf("%-12s: %.3f ms, %zu comparisons, %zu swaps\n", 
               algorithm_names[i], end_time - start_time, 
               stats.comparisons, stats.swaps);
    }
    
    free(test_array);
} 