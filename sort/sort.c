#include "sort.h"
// #include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>   /* SIZE_MAX (not guaranteed via <limits.h> on glibc/Linux) */




/* Statistics of the sort currently running on this thread. Thread-local: one
   process-wide struct made concurrent sorts (even of unrelated arrays) race
   and return each other's counters. */
#if defined(_MSC_VER) && !defined(__clang__)
#define SORT_THREAD_LOCAL __declspec(thread)
#else
#define SORT_THREAD_LOCAL _Thread_local
#endif
static SORT_THREAD_LOCAL SortStats global_stats;

/**
 * @brief Zero-initialize a SortStats counter structure.
 *
 * Resets `comparisons`, `swaps`, `recursive_calls`, and `execution_time_ms`
 * to zero and clears the `stable` / `in_place` flags. Safe to call with NULL
 * (no-op).
 *
 * @param stats Pointer to the stats block to clear.
 */
void sort_init_stats(SortStats* stats) {
    SORT_LOG("[sort_init_stats]: enter (stats=%p)", (void*)stats);
    if (!stats) {
        SORT_LOG("[sort_init_stats]: guard tripped (stats=NULL) -> early exit");
        return;
    }
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
    if (is_comparison) {
        global_stats.comparisons++;
    }
    if (is_swap) {
        global_stats.swaps++;
    }
    if (is_recursive) {
        global_stats.recursive_calls++;
    }
}

/* ------------------------------------------------------------------------
 * Introsort core shared by sort_quicksort, sort_quicksort_optimized and the
 * introspective sorts: median-of-three Hoare partitioning (balanced even with
 * many equal keys), recursion only into the smaller part (O(log n) stack),
 * a heapsort fallback once the depth exceeds 2*log2(n) (O(n log n) worst case)
 * and insertion sort for small ranges. The previous Lomuto quicksort recursed
 * n deep on sorted, reversed or all-equal input and overflowed the stack.
 * ---------------------------------------------------------------------- */
#define SORT_SMALL_RANGE 16

static size_t sort_floor_log2(size_t n) {
    size_t r = 0;
    while (n > 1) {
        n >>= 1;
        r++;
    }
    return r;
}

/* Stable insertion sort of array[lo, hi) (no stats reset). */
static void sort_insertion_range(ElementType* array, size_t lo, size_t hi, CompareFunc compare) {
    for (size_t i = lo + 1; i < hi; i++) {
        ElementType key = array[i];
        size_t j = i;
        while (j > lo && compare(array[j - 1], key) > 0) {
            array[j] = array[j - 1];
            j--;
        }
        array[j] = key;
    }
}

/* Hoare partition of array[lo..hi] (inclusive, hi > lo) around the median of
   three. Returns p with lo <= p < hi such that array[lo..p] <= pivot <=
   array[p+1..hi]. */
static size_t sort_hoare_partition(ElementType* array, size_t lo, size_t hi, CompareFunc compare, SwapFunc swap) {
    size_t mid = lo + (hi - lo) / 2;
    if (compare(array[mid], array[lo]) < 0) {
        swap(&array[mid], &array[lo]);
    }
    if (compare(array[hi], array[lo]) < 0) {
        swap(&array[hi], &array[lo]);
    }
    if (compare(array[hi], array[mid]) < 0) {
        swap(&array[hi], &array[mid]);
    }
    ElementType pivot = array[mid];
    size_t i = lo - 1;   /* unsigned wrap is well defined; the first ++ brings it to lo */
    size_t j = hi + 1;
    for (;;) {
        do {
            i++;
        } while (compare(array[i], pivot) < 0);
        do {
            j--;
        } while (compare(array[j], pivot) > 0);
        if (i >= j) {
            return j;
        }
        swap(&array[i], &array[j]);
    }
}

static void sort_heapsort_range(ElementType* array, size_t n, CompareFunc compare, SwapFunc swap) {
    sort_build_heap(array, n, compare, swap);
    for (size_t i = n - 1; i > 0; i--) {
        swap(&array[0], &array[i]);
        sort_heapify(array, i, 0, compare, swap);
    }
}

static void sort_introsort_loop(ElementType* array, size_t lo, size_t hi, size_t depth, CompareFunc compare, SwapFunc swap) {
    while (hi > lo && hi - lo >= SORT_SMALL_RANGE) {
        if (depth == 0) {
            sort_heapsort_range(array + lo, hi - lo + 1, compare, swap);
            return;
        }
        depth--;
        update_stats(false, false, true);
        size_t p = sort_hoare_partition(array, lo, hi, compare, swap);
        if (p - lo < hi - p) {
            sort_introsort_loop(array, lo, p, depth, compare, swap);
            lo = p + 1;
        }
        else {
            sort_introsort_loop(array, p + 1, hi, depth, compare, swap);
            hi = p;
        }
    }
}

/* Full introsort of array[0, size): small ranges are left for one final
   insertion pass, which is linear because every element is then near its
   place. */
static void sort_introsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    if (size < 2) {
        return;
    }
    sort_introsort_loop(array, 0, size - 1, 2 * sort_floor_log2(size), compare, swap);
    sort_insertion_range(array, 0, size, compare);
}

/* ------------------------------------------------------------------------
 * Stable natural merge sort (used by sort_timsort): finds the existing
 * ascending runs (strictly descending runs are reversed, which keeps
 * stability), extends short runs with insertion sort and merges neighbours
 * bottom-up. O(n) on sorted input, O(n log n) worst case, stable.
 * ---------------------------------------------------------------------- */
#define SORT_MIN_RUN 32

static void sort_merge_runs(ElementType* array, ElementType* tmp, size_t lo, size_t mid, size_t hi, CompareFunc compare) {
    /* array[lo, mid) and array[mid, hi) are sorted; merge stably via tmp. */
    if (mid <= lo || mid >= hi || compare(array[mid - 1], array[mid]) <= 0) {
        return; /* already in order */
    }
    size_t n1 = mid - lo;
    memcpy(tmp, array + lo, n1 * sizeof(ElementType));
    size_t i = 0, j = mid, k = lo;
    while (i < n1 && j < hi) {
        if (compare(array[j], tmp[i]) < 0) {
            array[k++] = array[j++];
        }
        else {
            array[k++] = tmp[i++];   /* ties keep the left (earlier) element first */
        }
    }
    while (i < n1) {
        array[k++] = tmp[i++];
    }
}

static bool sort_natural_mergesort(ElementType* array, size_t size, CompareFunc compare) {
    if (size < 2) {
        return true;
    }
    size_t* runs = (size_t*)malloc((size / SORT_MIN_RUN + 2) * sizeof(size_t));
    ElementType* tmp = (ElementType*)malloc(size * sizeof(ElementType));
    if (!runs || !tmp) {
        free(runs);
        free(tmp);
        return false;
    }

    size_t nruns = 0;
    size_t pos = 0;
    while (pos < size) {
        size_t start = pos++;
        if (pos < size) {
            if (compare(array[pos], array[pos - 1]) < 0) {           /* strictly descending */
                while (pos < size && compare(array[pos], array[pos - 1]) < 0) {
                    pos++;
                }
                for (size_t a = start, b = pos - 1; a < b; a++, b--) { /* reverse in place */
                    ElementType t = array[a];
                    array[a] = array[b];
                    array[b] = t;
                }
            }
            else {
                while (pos < size && compare(array[pos], array[pos - 1]) >= 0) {
                    pos++;
                }
            }
        }
        if (pos - start < SORT_MIN_RUN) {                            /* extend a short run */
            size_t end = (start + SORT_MIN_RUN < size) ? start + SORT_MIN_RUN : size;
            sort_insertion_range(array, start, end, compare);
            pos = end;
        }
        runs[nruns++] = start;
    }
    runs[nruns] = size;

    /* Merge neighbouring runs until one is left. */
    while (nruns > 1) {
        size_t out = 0;
        for (size_t r = 0; r < nruns; r += 2) {
            if (r + 1 < nruns) {
                sort_merge_runs(array, tmp, runs[r], runs[r + 1], runs[r + 2], compare);
            }
            runs[out++] = runs[r];
        }
        runs[out] = size;
        nruns = out;
    }

    free(runs);
    free(tmp);
    return true;
}

/* 64-bit generator for sort_shuffle (splitmix64): no global state and no
   srand()/rand(), whose RAND_MAX of 32767 on Windows biased large shuffles. */
static uint64_t sort_rng_next(uint64_t* state) {
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

/* Uniform index in [0, bound) by rejection (no modulo bias). */
static size_t sort_rng_below(uint64_t* state, size_t bound) {
    uint64_t b = (uint64_t)bound;
    uint64_t limit = UINT64_MAX - (UINT64_MAX % b);
    uint64_t x;
    do {
        x = sort_rng_next(state);
    } while (x >= limit);
    return (size_t)(x % b);
}



/**
 * @brief Default comparator for `int*` elements.
 *
 * Dereferences both arguments as `int*` and returns standard three-way
 * comparison. NULL is treated as the minimum value (NULL < any).
 *
 * @param a Pointer to the first int (treated as `int*`).
 * @param b Pointer to the second int.
 * @return Negative if `*a < *b`, zero if equal, positive if `*a > *b`.
 */
int sort_compare_int(const void* a, const void* b) {
    SORT_LOG("[sort_compare_int]: enter (a=%p, b=%p)", (void*)a, (void*)b);
    if (!a && !b) {
        return 0;
    }
    if (!a) {
        return -1;
    }
    if (!b) {
        return 1;
    }

    int val_a = *(int*)a;
    int val_b = *(int*)b;

    update_stats(true, false, false);

    if (val_a < val_b) {
        return -1;
    }
    if (val_a > val_b) {
        return 1;
    }
    return 0;
}


/**
 * @brief Default comparator for null-terminated strings (`char*` elements).
 *
 * Delegates to `strcmp`. NULL is treated as the minimum value.
 *
 * @param a Pointer to the first string.
 * @param b Pointer to the second string.
 * @return Result of `strcmp(a, b)` — negative / 0 / positive.
 */
int sort_compare_string(const void* a, const void* b) {
    SORT_LOG("[sort_compare_string]: enter (a=%p, b=%p)", (void*)a, (void*)b);
    if (!a && !b) {
        return 0;
    }
    if (!a) {
        return -1;
    }
    if (!b) {
        return 1;
    }

    update_stats(true, false, false);
    return strcmp((const char*)a, (const char*)b);
}


/**
 * @brief Default comparator for `float*` elements.
 *
 * @param a Pointer to the first float.
 * @param b Pointer to the second float.
 * @return Negative if `*a < *b`, zero if equal, positive if `*a > *b`.
 */
int sort_compare_float(const void* a, const void* b) {
    SORT_LOG("[sort_compare_float]: enter (a=%p, b=%p)", (void*)a, (void*)b);
    if (!a && !b) {
        return 0;
    }
    if (!a) {
        return -1;
    }
    if (!b) {
        return 1;
    }

    float val_a = *(float*)a;
    float val_b = *(float*)b;

    update_stats(true, false, false);

    if (val_a < val_b) {
        return -1;
    }
    if (val_a > val_b) {
        return 1;
    }
    return 0;
}


/**
 * @brief Default comparator for `double*` elements.
 *
 * @param a Pointer to the first double.
 * @param b Pointer to the second double.
 * @return Negative if `*a < *b`, zero if equal, positive if `*a > *b`.
 */
int sort_compare_double(const void* a, const void* b) {
    SORT_LOG("[sort_compare_double]: enter (a=%p, b=%p)", (void*)a, (void*)b);
    if (!a && !b) {
        return 0;
    }
    if (!a) {
        return -1;
    }
    if (!b) {
        return 1;
    }

    double val_a = *(double*)a;
    double val_b = *(double*)b;

    update_stats(true, false, false);

    if (val_a < val_b) {
        return -1;
    }
    if (val_a > val_b) {
        return 1;
    }
    return 0;
}


/**
 * @brief Comparator that orders elements by their raw pointer address.
 *
 * Useful when the user wants a stable but arbitrary ordering keyed on
 * identity rather than value (for example, deduplicating an array of
 * pointers).
 *
 * @param a First pointer.
 * @param b Second pointer.
 * @return Negative if `a < b`, zero if equal, positive if `a > b`.
 */
int sort_compare_pointer(const void* a, const void* b) {
    SORT_LOG("[sort_compare_pointer]: enter (a=%p, b=%p)", (void*)a, (void*)b);
    if (!a && !b) {
        return 0;
    }
    if (!a) {
        return -1;
    }
    if (!b) {
        return 1;
    }

    update_stats(true, false, false);

    if (a < b) {
        return -1;
    }
    if (a > b) {
        return 1;
    }
    return 0;
}


/**
 * @brief Default swap function for slot-based element arrays.
 *
 * Treats both arguments as `ElementType*` (i.e. addresses of slots in an
 * `ElementType[]` array) and swaps the pointers they hold. This is what
 * every sort algorithm in this library passes as its `swap` argument when
 * the caller doesn't supply a custom one.
 *
 * @param a Address of the first slot (e.g. `&array[i]`).
 * @param b Address of the second slot (e.g. `&array[j]`).
 */
void sort_swap_generic(ElementType a, ElementType b) {
    SORT_LOG("[sort_swap_generic]: enter (a=%p, b=%p)", (void*)a, (void*)b);
    if (!a || !b) {
        SORT_LOG("[sort_swap_generic]: guard tripped (a=%p, b=%p) -> early exit", (void*)a, (void*)b);
        return;
    }

    ElementType temp = *(ElementType*)a;
    *(ElementType*)a = *(ElementType*)b;
    *(ElementType*)b = temp;
    
    update_stats(false, true, false);
}


/**
 * @brief Default print callback for `int*` elements.
 *        Used by `sort_print_array`. Prints "NULL" for null pointers.
 * @param element Pointer to the int to print.
 */
void sort_print_int(const void* element) {
    SORT_LOG("[sort_print_int]: enter (element=%p)", (void*)element);
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%d", *(int*)element);
}

/**
 * @brief Default print callback for null-terminated string elements.
 *        Wraps the string in double quotes.
 * @param element Pointer to a C string.
 */
void sort_print_string(const void* element) {
    SORT_LOG("[sort_print_string]: enter (element=%p)", (void*)element);
    if (!element) {
        printf("NULL");
        return;
    }
    printf("\"%s\"", (const char*)element);
}


/**
 * @brief Default print callback for `float*` elements (2 decimal places).
 * @param element Pointer to the float to print.
 */
void sort_print_float(const void* element) {
    SORT_LOG("[sort_print_float]: enter (element=%p)", (void*)element);
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%.2f", *(float*)element);
}


/**
 * @brief Default print callback for `double*` elements (2 decimal places).
 * @param element Pointer to the double to print.
 */
void sort_print_double(const void* element) {
    SORT_LOG("[sort_print_double]: enter (element=%p)", (void*)element);
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%.2f", *(double*)element);
}


/**
 * @brief Default print callback that emits the raw pointer address.
 * @param element The pointer to print.
 */
void sort_print_pointer(const void* element) {
    SORT_LOG("[sort_print_pointer]: enter (element=%p)", (void*)element);
    if (!element) {
        printf("NULL");
        return;
    }
    printf("%p", (void*)element);
}


/**
 * @brief In-place insertion sort.
 *
 * Builds the sorted portion of the array one element at a time by sliding
 * each new element into its correct position. **Stable.** Best on small
 * inputs or arrays that are already nearly sorted (~O(n) in that case).
 *
 * Time complexity: O(n²) worst/average, O(n) best.
 * Space complexity: O(1) extra.
 *
 * @param array   Array of element pointers (e.g. `int*[]` whose `array[i]`
 *                each point at a real value).
 * @param size    Number of elements in `array`.
 * @param compare User comparator that receives `array[i]` (user pointers).
 * @param swap    User swap that receives `&array[i]` (slot addresses).
 */
void sort_insertion(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_insertion]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_insertion]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    sort_insertion_range(array, 0, size, compare);   /* size_t indexes: no int truncation */

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = true;

    SORT_LOG("[sort_insertion]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}



/**
 * @brief In-place quicksort (introsort).
 *
 * Median-of-three Hoare partitioning, recursion into the smaller side only,
 * a heapsort fallback past 2*log2(n) levels and insertion sort for small
 * ranges. **Not stable.** Sorted, reversed and all-equal inputs are fast.
 *
 * Time complexity: O(n log n) average and worst case.
 * Space complexity: O(log n) recursion stack.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_quicksort]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_quicksort]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    sort_introsort(array, size, compare, swap);

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;

    SORT_LOG("[sort_quicksort]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
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


/**
 * @brief Stable, divide-and-conquer merge sort.
 *
 * Recursively splits the array in half, sorts each side, then merges.
 * **Stable.** Always O(n log n), but allocates O(n) temporary storage
 * during each merge.
 *
 * Time complexity: O(n log n) all cases.
 * Space complexity: O(n) extra.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap (unused by the merge step itself, but required
 *                by the framework signature).
 */
void sort_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_mergesort]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_mergesort]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    mergesort_recursive(array, 0, size - 1, compare, swap);

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;

    SORT_LOG("[sort_mergesort]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief In-place heap sort.
 *
 * Builds a max-heap then repeatedly extracts the root to the end of the
 * array. **Not stable.** Worst-case guarantee of O(n log n) with zero
 * extra memory — preferable to quicksort when worst-case performance
 * matters.
 *
 * Time complexity: O(n log n) all cases.
 * Space complexity: O(1) extra.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_heapsort]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_heapsort]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

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

    SORT_LOG("[sort_heapsort]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Bubble sort with early-exit optimization.
 *
 * Repeatedly steps through the array, swapping adjacent out-of-order
 * elements. Early-exits when a full pass makes no swaps (so an already
 * sorted input runs in O(n)). **Stable** but inefficient; intended for
 * teaching / correctness baselines rather than production use.
 *
 * Time complexity: O(n²) worst/average, O(n) best.
 * Space complexity: O(1) extra.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_bubblesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_bubblesort]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_bubblesort]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    for (size_t i = 0; i < size - 1; i++) {
        bool swapped = false;
        for (size_t j = 0; j < size - i - 1; j++) {
            // compare receives user pointers (array[j]) not slot addresses (&array[j]).
            if (compare(array[j], array[j + 1]) > 0) {
                swap(&array[j], &array[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            SORT_LOG("[sort_bubblesort]: early exit at pass %zu (no swaps)", i);
            break; // Optimized bubble sort
        }
    }

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = true;

    SORT_LOG("[sort_bubblesort]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Selection sort.
 *
 * Finds the minimum of the unsorted suffix and swaps it into position.
 * **Not stable.** Performs exactly `n - 1` swaps (useful when the cost of
 * swapping is high), but always does O(n²) comparisons regardless of
 * input order.
 *
 * Time complexity: O(n²) all cases.
 * Space complexity: O(1) extra.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_selection(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_selection]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_selection]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    for (size_t i = 0; i < size - 1; i++) {
        size_t min_index = i;
        for (size_t j = i + 1; j < size; j++) {
            if (compare(array[j], array[min_index]) < 0) {
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

    SORT_LOG("[sort_selection]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Shell sort with Knuth's halving gap sequence.
 *
 * Generalized insertion sort: starts with a large gap (`size/2`), halves it
 * each pass, and ends with a final gap-1 insertion sort. **Not stable.**
 * Performance depends heavily on the gap sequence; Knuth's is a reasonable
 * middle-ground.
 *
 * Time complexity: ~O(n^1.3) typical, O(n²) worst with this sequence.
 * Space complexity: O(1) extra.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap (unused — element moves happen via direct
 *                assignment rather than full swaps).
 */
void sort_shell(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_shell]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_shell]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    /* Ciura's gap sequence (1, 4, 10, 23, 57, 132, 301, 701), extended by
       x2.25 beyond 701; the old size/2, size/4, ... gaps are notably slower. */
    size_t gaps[64];
    size_t ngaps = 0;
    static const size_t ciura[] = {1, 4, 10, 23, 57, 132, 301, 701};
    for (size_t k = 0; k < sizeof(ciura) / sizeof(ciura[0]) && ciura[k] < size; ++k) {
        gaps[ngaps++] = ciura[k];
    }
    if (ngaps == sizeof(ciura) / sizeof(ciura[0])) {
        size_t g = 701;
        while (ngaps < 64 && g <= SIZE_MAX / 9) {
            g = g * 9 / 4;
            if (g >= size) {
                break;
            }
            gaps[ngaps++] = g;
        }
    }
    while (ngaps-- > 0) {
        size_t gap = gaps[ngaps];
        for (size_t i = gap; i < size; i++) {
            ElementType temp = array[i];
            size_t j;
            for (j = i; j >= gap && compare(array[j - gap], temp) > 0; j -= gap) {
                array[j] = array[j - gap];
            }
            array[j] = temp;
        }
    }

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;

    SORT_LOG("[sort_shell]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Counting sort over integer keys.
 *
 * Tallies how many times each distinct integer value appears (after shifting
 * by `min`), then writes them out in order. **Stable.** Beats comparison-based
 * sorts when the value range `k = max - min + 1` is small relative to `n`.
 *
 * @warning Only valid for arrays whose elements are `int*`. The `compare`
 *          and `swap` parameters are unused and exist only for API symmetry.
 *
 * Time complexity: O(n + k).
 * Space complexity: O(n + k) extra.
 *
 * @param array   Array of `int*` element pointers.
 * @param size    Number of elements.
 * @param compare Unused; pass `sort_compare_int` for consistency.
 * @param swap    Unused.
 */
void sort_counting(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_counting]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_counting]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    // Find min and max values
    int min_val = INT_MAX, max_val = INT_MIN;
    for (size_t i = 0; i < size; i++) {
        int val = *(int*)array[i];
        if (val < min_val) {
            min_val = val;
        }
        if (val > max_val) {
            max_val = val;
        }
    }

    /* The range is computed in 64 bits: max - min + 1 in int overflowed for
       wide inputs (e.g. INT_MIN..INT_MAX) and wrapped to a tiny count array
       that was then written out of bounds. A range much larger than the
       input is better served by a comparison sort (stable, like this one). */
    uint64_t range = (uint64_t)((int64_t)max_val - (int64_t)min_val) + 1;
    if (range > (uint64_t)size * 4 + 1024 || range > SIZE_MAX / sizeof(size_t)) {
        SORT_LOG("[sort_counting]: value range %llu too wide for counting -> stable merge sort", (unsigned long long)range);
        sort_mergesort(array, size, compare, swap);
        return;
    }

    size_t* count = (size_t*)calloc((size_t)range, sizeof(size_t));
    ElementType* output = (ElementType*)malloc(size * sizeof(ElementType));

    if (!count || !output) {
        SORT_LOG("[sort_counting]: allocation failed (count=%p, output=%p) -> early exit", (void*)count, (void*)output);
        free(count);
        free(output);
        return;
    }

    // Count occurrences
    for (size_t i = 0; i < size; i++) {
        count[(size_t)((int64_t)*(int*)array[i] - min_val)]++;
    }

    // Calculate positions
    for (size_t i = 1; i < (size_t)range; i++) {
        count[i] += count[i - 1];
    }

    // Build output array (backwards keeps equal keys in order: stable)
    for (size_t i = size; i-- > 0; ) {
        size_t slot = (size_t)((int64_t)*(int*)array[i] - min_val);
        output[--count[slot]] = array[i];
    }

    // Copy back to original array
    memcpy(array, output, size * sizeof(ElementType));

    free(count);
    free(output);

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;

    SORT_LOG("[sort_counting]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief LSD (least-significant-digit) radix sort over 32-bit integers.
 *
 * Runs a stable counting pass for each byte of the key, from least to most
 * significant (4 passes of 256 buckets). Negative values are supported: the
 * sign bit is flipped so the unsigned key order equals the int order.
 * **Stable.** Beats comparison-based sorts for large integer arrays.
 *
 * @warning Only valid for arrays whose elements are `int*`.
 *          The `compare` and `swap` parameters are unused.
 *
 * Time complexity: O(4 · (n + 256)).
 * Space complexity: O(n) extra.
 *
 * @param array   Array of `int*` element pointers.
 * @param size    Number of elements.
 * @param compare Unused.
 * @param swap    Unused.
 */
void sort_radix(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_radix]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_radix]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    /* Byte-wise LSD passes over (value XOR sign bit): the old base-10 version
       indexed count[] with negative digits for negative keys (stack
       corruption) and overflowed `exp *= 10` for keys >= 1e9. */
    ElementType* output = (ElementType*)malloc(size * sizeof(ElementType));
    if (!output) {
        SORT_LOG("[sort_radix]: allocation failed -> early exit");
        return;
    }

    ElementType* src = array;
    ElementType* dst = output;
    for (unsigned shift = 0; shift < 32; shift += 8) {
        size_t count[257] = {0};
        for (size_t i = 0; i < size; i++) {
            uint32_t key = (uint32_t)*(int*)src[i] ^ 0x80000000u;
            count[((key >> shift) & 0xFFu) + 1]++;
        }
        for (size_t b = 1; b < 257; b++) {
            count[b] += count[b - 1];
        }
        for (size_t i = 0; i < size; i++) {
            uint32_t key = (uint32_t)*(int*)src[i] ^ 0x80000000u;
            dst[count[(key >> shift) & 0xFFu]++] = src[i];
        }
        ElementType* t = src;
        src = dst;
        dst = t;
    }
    /* After an even number (4) of passes the result is back in `array`. */

    free(output);

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;

    SORT_LOG("[sort_radix]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Bucket sort optimized for uniformly-distributed floating-point keys.
 *
 * Distributes elements into `n` buckets based on a linear projection of
 * `(value - min) / (max - min)`, sorts each bucket with insertion sort,
 * then concatenates. **Stable when bucket sort is stable** (it is here).
 *
 * @warning Only valid for arrays whose elements are `float*`. Performance
 *          degrades to O(n²) on heavily skewed distributions.
 *
 * Time complexity: O(n) expected for uniform input; O(n²) worst.
 * Space complexity: O(n) extra.
 *
 * @param array   Array of `float*` element pointers.
 * @param size    Number of elements.
 * @param compare User comparator (passed through to bucket insertion sort).
 * @param swap    User swap.
 */
void sort_bucket(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_bucket]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_bucket]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    // Find min and max for bucket range
    float min_val = *(float*)array[0], max_val = *(float*)array[0];
    bool finite = true;
    for (size_t i = 0; i < size; i++) {
        float val = *(float*)array[i];
        if (!isfinite(val)) {
            finite = false;
            break;
        }
        if (val < min_val) {
            min_val = val;
        }
        if (val > max_val) {
            max_val = val;
        }
    }
    if (finite && max_val == min_val) {
        SORT_LOG("[sort_bucket]: all keys equal -> already sorted");
        return; /* (the old code divided 0/0 here and cast NaN to size_t) */
    }
    if (!finite || !isfinite(max_val - min_val)) {
        /* NaN or infinite keys cannot be projected onto buckets. */
        SORT_LOG("[sort_bucket]: non-finite keys -> stable merge sort");
        sort_mergesort(array, size, compare, swap);
        return;
    }

    /* One counting pass sizes every bucket exactly, so all buckets share a
       single n-element array (the old version allocated n buckets of n slots:
       O(n^2) memory, never NULL-checked). */
    size_t num_buckets = size;
    size_t* starts = (size_t*)calloc(num_buckets + 1, sizeof(size_t));
    size_t* index_of = (size_t*)malloc(size * sizeof(size_t));
    ElementType* output = (ElementType*)malloc(size * sizeof(ElementType));
    if (!starts || !index_of || !output) {
        SORT_LOG("[sort_bucket]: allocation failed -> early exit");
        free(starts);
        free(index_of);
        free(output);
        return;
    }

    double span = (double)max_val - (double)min_val;
    for (size_t i = 0; i < size; i++) {
        double pos = ((double)*(float*)array[i] - (double)min_val) / span * (double)(num_buckets - 1);
        size_t b = (pos <= 0.0) ? 0 : (pos >= (double)(num_buckets - 1)) ? num_buckets - 1 : (size_t)pos;
        index_of[i] = b;
        starts[b + 1]++;
    }
    for (size_t b = 1; b <= num_buckets; b++) {
        starts[b] += starts[b - 1];
    }
    size_t* fill = (size_t*)malloc(num_buckets * sizeof(size_t));
    if (!fill) {
        free(starts);
        free(index_of);
        free(output);
        return;
    }
    memcpy(fill, starts, num_buckets * sizeof(size_t));
    for (size_t i = 0; i < size; i++) {          /* in input order: stable */
        output[fill[index_of[i]]++] = array[i];
    }
    for (size_t b = 0; b < num_buckets; b++) {   /* sort each bucket (stable insertion sort) */
        if (starts[b + 1] - starts[b] > 1) {
            sort_insertion_range(output, starts[b], starts[b + 1], compare);
        }
    }
    memcpy(array, output, size * sizeof(ElementType));

    free(fill);
    free(starts);
    free(index_of);
    free(output);

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;

    SORT_LOG("[sort_bucket]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Introspective sort — hybrid of quicksort, heapsort, and insertion sort.
 *
 * Starts as quicksort; switches to heapsort when the recursion depth exceeds
 * `2·log2(n)` (to guarantee O(n log n) worst case) and to insertion sort for
 * small partitions (≤16 elements, where the overhead of the better algorithms
 * doesn't pay off). **Not stable.**
 *
 * Time complexity: O(n log n) worst-case.
 * Space complexity: O(log n) recursion stack.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_introspective(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_introspective]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_introspective]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    sort_introsort(array, size, compare, swap);   /* depth limit 2*log2(n), heapsort fallback */

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;

    SORT_LOG("[sort_introspective]: Sorted %zu elements in %.3f ms", size, global_stats.execution_time_ms);
}


/**
 * @brief Dispatch to a specific sort algorithm by enum tag.
 *
 * Convenience wrapper that picks the named algorithm from `SortAlgorithm`
 * and optionally captures execution statistics. Falls back to quicksort
 * for unknown enum values.
 *
 * @param array     Array of element pointers.
 * @param size      Number of elements.
 * @param compare   User comparator.
 * @param swap      User swap.
 * @param algorithm Which sort to run (e.g. `SORT_QUICKSORT`).
 * @param stats     Optional out-param filled with comparison / swap counts
 *                  and execution time; may be NULL to skip.
 */
void sort_generic(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap, SortAlgorithm algorithm, SortStats* stats) {
    SORT_LOG("[sort_generic]: enter (array=%p, size=%zu, algorithm=%d)", (void*)array, size, (int)algorithm);

    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_generic]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    switch (algorithm) {
        case SORT_INSERTION:
            SORT_LOG("[sort_generic]: dispatching to insertion sort");
            sort_insertion(array, size, compare, swap);
            break;
        case SORT_QUICKSORT:
            SORT_LOG("[sort_generic]: dispatching to quicksort");
            sort_quicksort(array, size, compare, swap);
            break;
        case SORT_MERGESORT:
            SORT_LOG("[sort_generic]: dispatching to mergesort");
            sort_mergesort(array, size, compare, swap);
            break;
        case SORT_HEAPSORT:
            SORT_LOG("[sort_generic]: dispatching to heapsort");
            sort_heapsort(array, size, compare, swap);
            break;
        case SORT_BUBBLESORT:
            SORT_LOG("[sort_generic]: dispatching to bubblesort");
            sort_bubblesort(array, size, compare, swap);
            break;
        case SORT_SELECTION:
            SORT_LOG("[sort_generic]: dispatching to selection sort");
            sort_selection(array, size, compare, swap);
            break;
        case SORT_SHELL:
            SORT_LOG("[sort_generic]: dispatching to shell sort");
            sort_shell(array, size, compare, swap);
            break;
        case SORT_COUNTING:
            SORT_LOG("[sort_generic]: dispatching to counting sort");
            sort_counting(array, size, compare, swap);
            break;
        case SORT_RADIX:
            SORT_LOG("[sort_generic]: dispatching to radix sort");
            sort_radix(array, size, compare, swap);
            break;
        case SORT_BUCKET:
            SORT_LOG("[sort_generic]: dispatching to bucket sort");
            sort_bucket(array, size, compare, swap);
            break;
        case SORT_INTROSPECTIVE:
            SORT_LOG("[sort_generic]: dispatching to introspective sort");
            sort_introspective(array, size, compare, swap);
            break;
        default:
            SORT_LOG("[sort_generic]: unknown algorithm %d, falling back to quicksort", (int)algorithm);
            sort_quicksort(array, size, compare, swap);
            break;
    }

    if (stats) {
        *stats = global_stats;
        stats->execution_time_ms = get_time_ms() - start_time;
    }
}


/**
 * @brief Check whether an array is sorted in non-decreasing order.
 *
 * Empty arrays and single-element arrays are considered sorted.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @return `true` if `compare(array[i-1], array[i]) <= 0` for every adjacent
 *         pair, otherwise `false`. NULL/empty arrays return `true`.
 */
bool sort_is_sorted(ElementType* array, size_t size, CompareFunc compare) {
    SORT_LOG("[sort_is_sorted]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare) {
        SORT_LOG("[sort_is_sorted]: guard tripped (array=%p, size=%zu) -> early exit (true)", (void*)array, size);
        return true;
    }

    for (size_t i = 1; i < size; i++) {
        if (compare(array[i - 1], array[i]) > 0) {
            return false;
        }
    }
    return true;
}


/**
 * @brief Return the index of the first element that is out of order — the end
 *        of the longest sorted prefix (analog of `std::is_sorted_until`).
 *
 * Scans left to right and returns the index of the first element that is
 * strictly smaller than its predecessor. If the whole array is sorted (or has
 * fewer than two elements), `size` is returned. The relationship
 * `sort_is_sorted(a, n, cmp) == (sort_is_sorted_until(a, n, cmp) == n)` always
 * holds, so this is the diagnostic companion to `sort_is_sorted`: it tells you
 * not just whether the data is sorted, but exactly where the order breaks.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator (returns < 0 / 0 / > 0).
 * @return Index in `[0, size]` of the first out-of-order element, or `size`
 *         if the array is fully sorted / degenerate / NULL.
 */
size_t sort_is_sorted_until(ElementType* array, size_t size, CompareFunc compare) {
    SORT_LOG("[sort_is_sorted_until]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare) {
        SORT_LOG("[sort_is_sorted_until]: guard tripped (array=%p, size=%zu) -> size", (void*)array, size);
        return size;
    }

    for (size_t i = 1; i < size; i++) {
        if (compare(array[i - 1], array[i]) > 0) {
            return i;
        }
    }
    return size;
}


/**
 * @brief Remove consecutive duplicate elements in-place, keeping the first of
 *        each run (analog of `std::unique`).
 *
 * This is the second half of the ubiquitous "sort, then unique" idiom: run a
 * sort first, then call this to collapse each group of equal elements down to
 * one. Only the element *pointers* are moved (the function never frees or
 * copies the pointed-to data), so it is safe regardless of who owns the
 * elements. Because it compares each element to the previous *kept* one, on a
 * sorted array it removes every duplicate; on an unsorted array it removes
 * only adjacent duplicates (exactly like `std::unique`).
 *
 * The first `return value` slots hold the distinct elements in their original
 * relative order. Slots beyond the returned count keep their previous
 * (now-duplicated) pointer values — like `std::unique`, they are left in an
 * unspecified-but-valid state, not cleared.
 *
 * @param array   Array of element pointers (typically already sorted).
 * @param size    Number of elements.
 * @param compare User comparator (returns < 0 / 0 / > 0).
 * @return The number of distinct elements now packed at the front of `array`
 *         (the new logical length). Returns 0 for a NULL array / NULL
 *         comparator / empty input.
 *
 * @code
 * sort_quicksort(arr, n, sort_compare_int, sort_swap_generic);
 * size_t m = sort_unique(arr, n, sort_compare_int);   // arr[0..m) is sorted + distinct
 * @endcode
 */
size_t sort_unique(ElementType* array, size_t size, CompareFunc compare) {
    SORT_LOG("[sort_unique]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || !compare || size == 0) {
        SORT_LOG("[sort_unique]: guard tripped (array=%p, size=%zu) -> 0", (void*)array, size);
        return 0;
    }

    size_t write = 1;   /* the first element is always kept */
    for (size_t read = 1; read < size; read++) {
        if (compare(array[write - 1], array[read]) != 0) {
            if (write != read) {
                array[write] = array[read];
            }
            write++;
        }
    }

    SORT_LOG("[sort_unique]: exit -> %zu distinct", write);
    return write;
}


/**
 * @brief Reverse the order of elements in-place.
 *
 * Swaps element `i` with element `size - 1 - i` for the first half of the
 * array. NULL or single-element arrays are no-ops.
 *
 * @param array Array of element pointers.
 * @param size  Number of elements.
 * @param swap  User swap.
 */
void sort_reverse(ElementType* array, size_t size, SwapFunc swap) {
    SORT_LOG("[sort_reverse]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !swap) {
        SORT_LOG("[sort_reverse]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    for (size_t i = 0; i < size / 2; i++) {
        swap(&array[i], &array[size - 1 - i]);
    }
}


/**
 * @brief Randomly permute the array in place via Fisher-Yates shuffle.
 *
 * Seeds `rand()` with the current time on each call, then performs `n-1`
 * swaps. For cryptographic randomness or reproducible shuffles use a
 * higher-quality RNG.
 *
 * @param array Array of element pointers.
 * @param size  Number of elements.
 * @param swap  User swap.
 */
void sort_shuffle(ElementType* array, size_t size, SwapFunc swap) {
    SORT_LOG("[sort_shuffle]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !swap) {
        SORT_LOG("[sort_shuffle]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    /* Local generator seeded from the clock and addresses: uniform for any size
       (rand() is limited to 32767 on Windows), no shared state, and the caller's
       rand() sequence is not reseeded. */
    uint64_t state = (uint64_t)time(NULL) ^ ((uint64_t)clock() << 20) ^ (uint64_t)(uintptr_t)array
                     ^ ((uint64_t)(uintptr_t)&state << 7);
    for (size_t i = size - 1; i > 0; i--) {
        size_t j = sort_rng_below(&state, i + 1);
        swap(&array[i], &array[j]);
    }
}


/**
 * @brief Lomuto partition around `array[right]` as pivot.
 *
 * Reorders `array[left..right]` so that elements `<=` pivot come first and
 * elements `>` pivot come after. The pivot ends up at its final sorted
 * position; the return value is its index.
 *
 * Used internally by quicksort, but exposed for users who want to build
 * their own partition-based algorithms.
 *
 * @param array   Array of element pointers.
 * @param left    Inclusive lower bound of the sub-range.
 * @param right   Inclusive upper bound (also where the pivot starts).
 * @param compare User comparator.
 * @param swap    User swap.
 * @return Final index of the pivot after partitioning.
 */
size_t sort_partition(ElementType* array, size_t left, size_t right,
                     CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_partition]: enter (array=%p, left=%zu, right=%zu)", (void*)array, left, right);
    ElementType pivot = array[right];
    size_t i = left;

    for (size_t j = left; j < right; j++) {
        if (compare(array[j], pivot) <= 0) {
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


/**
 * @brief Merge two adjacent sorted sub-ranges into one sorted range.
 *
 * Treats `array[left..mid]` and `array[mid+1..right]` as independently
 * sorted, then merges them stably back into `array[left..right]`.
 *
 * Allocates two temporary arrays of size `mid-left+1` and `right-mid`.
 * On allocation failure the array is left untouched and the function
 * returns silently.
 *
 * @param array   Combined array (both halves must already be sorted).
 * @param left    Inclusive lower bound.
 * @param mid     End of the first sub-range (inclusive).
 * @param right   End of the second sub-range (inclusive).
 * @param compare User comparator.
 * @param swap    Unused (merge writes through direct assignment).
 */
void sort_merge(ElementType* array, size_t left, size_t mid, size_t right, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_merge]: enter (array=%p, left=%zu, mid=%zu, right=%zu)", (void*)array, left, mid, right);
    (void)swap;

    size_t left_size = mid - left + 1;
    size_t right_size = right - mid;

    ElementType* left_array = (ElementType*)malloc(left_size * sizeof(ElementType));
    ElementType* right_array = (ElementType*)malloc(right_size * sizeof(ElementType));

    if (!left_array || !right_array) {
        SORT_LOG("[sort_merge]: allocation failed -> early exit");
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
        if (compare(left_array[i], right_array[j]) <= 0) {
            array[k] = left_array[i];
            i++;
        } 
        else {
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


/**
 * @brief Restore the max-heap property at a single node by sifting down.
 *
 * Assumes the binary trees rooted at `index`'s two children already satisfy
 * the heap property and fixes the subtree rooted at `index`.
 *
 * @param array   Heap-array of element pointers.
 * @param size    Number of valid elements in the heap.
 * @param index   Index of the node to heapify down from.
 * @param compare User comparator (heap is a MAX-heap: largest at root).
 * @param swap    User swap.
 */
void sort_heapify(ElementType* array, size_t size, size_t index, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_heapify]: enter (array=%p, size=%zu, index=%zu)", (void*)array, size, index);
    size_t largest = index;
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;

    if (left < size && compare(array[left], array[largest]) > 0) {
        largest = left;
    }

    if (right < size && compare(array[right], array[largest]) > 0) {
        largest = right;
    }
    if (largest != index) {
        swap(&array[index], &array[largest]);
        sort_heapify(array, size, largest, compare, swap);
    }
}


/**
 * @brief Convert an arbitrary array into a max-heap in place (Floyd's method).
 *
 * Runs in O(n) by heapifying from the last non-leaf node downward.
 *
 * @param array   Array of element pointers (becomes a heap on return).
 * @param size    Number of elements.
 * @param compare User comparator (heap is MAX-heap).
 * @param swap    User swap.
 */
void sort_build_heap(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_build_heap]: enter (array=%p, size=%zu)", (void*)array, size);
    for (size_t i = size / 2; i-- > 0; ) {   /* size_t: no int truncation for big arrays */
        sort_heapify(array, size, i, compare, swap);
    }
}


/**
 * @brief Print an array as `[e0, e1, ...]` using a per-element callback.
 *
 * The callback is one of `sort_print_int` / `sort_print_string` / etc., or
 * any user-supplied function with the same signature.
 *
 * @param array Array of element pointers.
 * @param size  Number of elements.
 * @param print Per-element print callback.
 */
void sort_print_array(ElementType* array, size_t size, PrintFunc print) {
    SORT_LOG("[sort_print_array]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || !print) {
        SORT_LOG("[sort_print_array]: guard tripped (array=%p, print=%p) -> early exit", (void*)array, (void*)print);
        return;
    }

    printf("[");
    for (size_t i = 0; i < size; i++) {
        print(array[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}


/**
 * @brief Print a `SortStats` block as a human-readable multi-line block.
 *
 * Shows comparison count, swap count, recursive-call count, execution time,
 * and the stability/in-place flags. No-op on NULL.
 *
 * @param stats Stats block to display.
 */
void sort_print_stats(const SortStats* stats) {
    SORT_LOG("[sort_print_stats]: enter (stats=%p)", (const void*)stats);
    if (!stats) {
        SORT_LOG("[sort_print_stats]: guard tripped (stats=NULL) -> early exit");
        return;
    }

    printf("Sort Statistics:\n");
    printf("  Comparisons: %zu\n", stats->comparisons);
    printf("  Swaps: %zu\n", stats->swaps);
    printf("  Recursive calls: %zu\n", stats->recursive_calls);
    printf("  Execution time: %.3f ms\n", stats->execution_time_ms);
    printf("  Stable: %s\n", stats->stable ? "Yes" : "No");
    printf("  In-place: %s\n", stats->in_place ? "Yes" : "No");
}


/**
 * @brief Reset a `SortStats` block back to zero. Alias for `sort_init_stats`.
 * @param stats Stats block to reset.
 */
void sort_reset_stats(SortStats* stats) {
    SORT_LOG("[sort_reset_stats]: enter (stats=%p)", (void*)stats);
    sort_init_stats(stats);
}


/**
 * @brief Quicksort with median-of-three pivot selection and small-array cutoff.
 *
 * For arrays of ≤10 elements, falls back to insertion sort (which has
 * lower constant overhead). Otherwise picks the median of `array[0]`,
 * `array[mid]`, `array[size-1]` as pivot to avoid the O(n²) worst case
 * on already-sorted input.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_quicksort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_quicksort_optimized]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_quicksort_optimized]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    /* sort_quicksort is the median-of-three introsort with an insertion-sort
       cutoff, i.e. exactly what this variant promises. (The old version
       picked a median once and then ran the plain O(n^2)-worst Lomuto sort.) */
    sort_quicksort(array, size, compare, swap);
}


/**
 * @brief Mergesort with small-array fallback to insertion sort.
 *
 * Arrays of ≤10 elements run through insertion sort (faster constant
 * factors), everything larger uses the standard mergesort.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_mergesort_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_mergesort_optimized]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_mergesort_optimized]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    // Use insertion sort for small arrays
    if (size <= 10) {
        SORT_LOG("[sort_mergesort_optimized]: size<=10, falling back to insertion sort");
        sort_insertion(array, size, compare, swap);
        return;
    }

    sort_mergesort(array, size, compare, swap);
}


/**
 * @brief Introspective sort variant with cutoffs and depth-limited recursion.
 *
 * - Arrays of ≤16 elements: insertion sort.
 * - Otherwise: median-of-three quicksort, but switches to heapsort if the
 *   depth-bound `2·log2(n)` is exceeded (preventing O(n²) blow-up).
 *
 * Combines the best practical behavior of three algorithms: insertion
 * sort's tiny constant for small inputs, quicksort's cache-friendly
 * partitioning, and heapsort's worst-case guarantee.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_introspective_optimized(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_introspective_optimized]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_introspective_optimized]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    double start_time = get_time_ms();
    reset_global_stats();

    /* Real depth-limited introsort (the old code computed the depth bound
       and never enforced it). */
    sort_introsort(array, size, compare, swap);

    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = false;
    global_stats.in_place = true;
}


/**
 * @brief Single-threaded placeholder for a parallel mergesort.
 *
 * The current implementation simply delegates to `sort_mergesort_optimized`.
 * Reserved for a future task-based parallel implementation; users can
 * substitute their own without breaking source compatibility.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_parallel_mergesort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_parallel_mergesort]: enter (array=%p, size=%zu)", (void*)array, size);
    // Simplified parallel implementation
    sort_mergesort_optimized(array, size, compare, swap);
}


/**
 * @brief Single-threaded placeholder for a parallel quicksort.
 *
 * Delegates to `sort_quicksort_optimized`. Reserved for a future task-based
 * parallel implementation.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_parallel_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_parallel_quicksort]: enter (array=%p, size=%zu)", (void*)array, size);
    // Simplified parallel implementation
    sort_quicksort_optimized(array, size, compare, swap);
}


/**
 * @brief Stable sort with quicksort-like API.
 *
 * Because quicksort itself isn't stable, this routes to `sort_mergesort`,
 * which is. Useful as a drop-in replacement when callers want a stable
 * sort but coded against the quicksort signature.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_stable_quicksort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_stable_quicksort]: enter (array=%p, size=%zu)", (void*)array, size);
    // Use mergesort for stable sorting
    sort_mergesort(array, size, compare, swap);
}


/**
 * @brief Stable sort with heapsort-like API.
 *
 * Heapsort isn't stable, so this also delegates to `sort_mergesort`.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_stable_heapsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_stable_heapsort]: enter (array=%p, size=%zu)", (void*)array, size);
    // Use mergesort for stable sorting
    sort_mergesort(array, size, compare, swap);
}


/**
 * @brief Adaptive sort that picks the best algorithm based on array size.
 *
 * - `size ≤ 10` → insertion sort.
 * - `10 < size ≤ 100` → shell sort.
 * - Everything larger → introspective sort.
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_adaptive(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_adaptive]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        SORT_LOG("[sort_adaptive]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    // Choose algorithm based on array characteristics
    if (size <= 10) {
        SORT_LOG("[sort_adaptive]: size<=10, dispatching to insertion sort");
        sort_insertion(array, size, compare, swap);
    } 
    else if (size <= 100) {
        SORT_LOG("[sort_adaptive]: 10<size<=100, dispatching to shell sort");
        sort_shell(array, size, compare, swap);
    } 
    else {
        SORT_LOG("[sort_adaptive]: size>100, dispatching to introspective sort");
        sort_introspective_optimized(array, size, compare, swap);
    }
}


/**
 * @brief Timsort-style stable sort (natural merge sort).
 *
 * Detects the natural ascending/descending runs, extends short runs with
 * insertion sort and merges neighbouring runs stably. **Stable.** O(n) on
 * already-sorted (or reversed) input, O(n log n) worst case, O(n) extra memory.
 * (It previously delegated to sort_adaptive, i.e. an unstable shell/quick sort.)
 *
 * @param array   Array of element pointers.
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 */
void sort_timsort(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_timsort]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size <= 1 || !compare || !swap) {
        return;
    }
    double start_time = get_time_ms();
    reset_global_stats();
    if (!sort_natural_mergesort(array, size, compare)) {
        SORT_LOG("[sort_timsort]: allocation failed -> falling back to stable merge sort");
        sort_mergesort(array, size, compare, swap);
        return;
    }
    global_stats.execution_time_ms = get_time_ms() - start_time;
    global_stats.stable = true;
    global_stats.in_place = false;
}


/**
 * @brief Locate `target` in a sorted array via binary search.
 *
 * The array must already be sorted according to `compare`.
 *
 * @param array   Sorted array of element pointers.
 * @param size    Number of elements.
 * @param target  Element to look for (user pointer).
 * @param compare User comparator.
 * @return Index of a matching element, or `SIZE_MAX` if not found / on
 *         invalid input. When multiple equal elements exist, any one of
 *         their indices may be returned — use `sort_lower_bound` /
 *         `sort_upper_bound` for deterministic positions.
 */
size_t sort_binary_search(ElementType* array, size_t size, ElementType target, CompareFunc compare) {
    SORT_LOG("[sort_binary_search]: enter (array=%p, size=%zu, target=%p)", (void*)array, size, (void*)target);
    if (!array || size == 0 || !compare) {
        SORT_LOG("[sort_binary_search]: guard tripped (array=%p, size=%zu) -> early exit (SIZE_MAX)", (void*)array, size);
        return SIZE_MAX;
    }

    /* Half-open [left, right) so we never decrement past zero. The old
       closed-interval version (`right = size - 1; while (left <= right);
       right = mid - 1;`) underflowed `right` to SIZE_MAX when the target
       was smaller than every element, then dereferenced array[mid] far
       out of bounds — a guaranteed segfault under any sanitizer. */
    size_t left = 0, right = size;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        int cmp = compare(array[mid], target);

        if (cmp == 0) {
            return mid;
        }
        if (cmp < 0) {
            left = mid + 1;
        }
        else {
            right = mid;
        }
    }

    return SIZE_MAX;
}


/**
 * @brief Return the first index where `target` could be inserted to keep
 *        `array` sorted (analog of `std::lower_bound`).
 *
 * Equivalent to the smallest `i` such that `compare(array[i], target) >= 0`.
 * For an existing run of elements equal to `target`, this returns the
 * index of the first one.
 *
 * @param array   Sorted array of element pointers.
 * @param size    Number of elements.
 * @param target  Element to compare against.
 * @param compare User comparator.
 * @return Insertion index in `[0, size]` (returns `size` if every existing
 *         element is `< target`).
 */
size_t sort_lower_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare) {
    SORT_LOG("[sort_lower_bound]: enter (array=%p, size=%zu, target=%p)", (void*)array, size, (void*)target);
    if (!array || size == 0 || !compare) {
        SORT_LOG("[sort_lower_bound]: guard tripped (array=%p, size=%zu) -> early exit (0)", (void*)array, size);
        return 0;
    }

    size_t left = 0, right = size;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (compare(array[mid], target) < 0) {
            left = mid + 1;
        } 
        else {
            right = mid;
        }
    }

    return left;
}


/**
 * @brief Return one-past-the-last index where `target` appears in a sorted
 *        array (analog of `std::upper_bound`).
 *
 * Equivalent to the smallest `i` such that `compare(array[i], target) > 0`.
 * `upper - lower` gives the count of elements equal to `target`.
 *
 * @param array   Sorted array of element pointers.
 * @param size    Number of elements.
 * @param target  Element to compare against.
 * @param compare User comparator.
 * @return Insertion index in `[0, size]`.
 */
size_t sort_upper_bound(ElementType* array, size_t size, ElementType target, CompareFunc compare) {
    SORT_LOG("[sort_upper_bound]: enter (array=%p, size=%zu, target=%p)", (void*)array, size, (void*)target);
    if (!array || size == 0 || !compare) {
        SORT_LOG("[sort_upper_bound]: guard tripped (array=%p, size=%zu) -> early exit (0)", (void*)array, size);
        return 0;
    }

    size_t left = 0, right = size;

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (compare(array[mid], target) <= 0) {
            left = mid + 1;
        } 
        else {
            right = mid;
        }
    }

    return left;
}


/**
 * @brief Three-way partition (Dutch national flag) around `pivot`.
 *
 * Reorders elements in `array[left..right]` so that elements less than `pivot`
 * come first, equal elements next, and greater elements last. The return value
 * is the start index of the "equal" region — useful for quicksort variants
 * that need to skip duplicates of the pivot.
 *
 * @param array  Array of generic element pointers.
 * @param left   Inclusive lower bound of the sub-range to partition.
 * @param right  Inclusive upper bound of the sub-range to partition.
 * @param pivot  Pivot value (user pointer; compared via `compare`).
 * @param compare User-provided comparator that takes the user pointers and
 *                returns negative/zero/positive.
 * @param swap   User-provided swap; receives slot addresses (`&array[i]`).
 * @return Index of the first element in the "equal-to-pivot" region.
 */
size_t sort_partition_three_way(ElementType* array, size_t left, size_t right, ElementType pivot, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_partition_three_way]: enter (array=%p, left=%zu, right=%zu)", (void*)array, left, right);
    if (!array || !compare || !swap || left > right) {
        SORT_LOG("[sort_partition_three_way]: guard tripped (array=%p, left=%zu, right=%zu) -> early exit", (void*)array, left, right);
        return left;
    }

    size_t lt = left;     // array[left..lt-1] < pivot
    size_t i  = left;     // current scan position
    size_t gt = right;    // array[gt+1..right] > pivot
    
    while (i <= gt) {
        int cmp = compare(array[i], pivot);
        if (cmp < 0) {
            swap(&array[lt], &array[i]);
            lt++; i++;
        } 
        else if (cmp > 0) {
            swap(&array[i], &array[gt]);
            if (gt == 0) {
                break;
            }
            gt--;
        } 
        else {
            i++;
        }
    }
    return lt;
}


/**
 * @brief Median-of-three partition for quicksort.
 *
 * Picks the median of `array[left]`, `array[mid]`, `array[right]` as the
 * pivot, moves it to `array[right]`, and then runs the standard
 * Lomuto-style partition. This avoids quicksort's worst-case O(n²) on
 * already-sorted or reverse-sorted inputs.
 *
 * @param array  Array of generic element pointers.
 * @param left   Inclusive lower bound.
 * @param right  Inclusive upper bound.
 * @param compare User-provided comparator.
 * @param swap   User-provided swap.
 * @return Pivot's final index after partitioning.
 */
size_t sort_partition_median_of_three(ElementType* array, size_t left, size_t right, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_partition_median_of_three]: enter (array=%p, left=%zu, right=%zu)", (void*)array, left, right);
    if (!array || !compare || !swap || left >= right) {
        SORT_LOG("[sort_partition_median_of_three]: guard tripped (array=%p, left=%zu, right=%zu) -> early exit", (void*)array, left, right);
        return left;
    }
    size_t mid = left + (right - left) / 2;

    // Sort left/mid/right so the median ends up at `mid`, then put it at `right`.
    if (compare(array[left], array[mid])   > 0) {
        swap(&array[left], &array[mid]);
    }
    if (compare(array[left], array[right]) > 0) {
        swap(&array[left], &array[right]);
    }
    if (compare(array[mid],  array[right]) > 0) {
        swap(&array[mid],  &array[right]);
    }
    swap(&array[mid], &array[right]);

    return sort_partition(array, left, right, compare, swap);
}


/**
 * @brief In-place merge of two adjacent sorted sub-ranges.
 *
 * Merges `array[left..mid]` and `array[mid+1..right]` (both assumed sorted)
 * back into `array[left..right]` without allocating a temporary buffer.
 * Uses element-by-element rotation — O((n)·(left-right)) worst case, slower
 * than `sort_merge` but uses zero extra memory.
 *
 * @param array   Array of element pointers (each sub-range already sorted).
 * @param left    Inclusive lower bound.
 * @param mid     End of first sub-range (inclusive).
 * @param right   End of second sub-range (inclusive).
 * @param compare User-provided comparator.
 * @param swap    User-provided swap.
 */
void sort_merge_inplace(ElementType* array, size_t left, size_t mid, size_t right, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_merge_inplace]: enter (array=%p, left=%zu, mid=%zu, right=%zu)", (void*)array, left, mid, right);
    if (!array || !compare || !swap || left >= right) {
        SORT_LOG("[sort_merge_inplace]: guard tripped (array=%p, left=%zu, right=%zu) -> early exit", (void*)array, left, right);
        return;
    }

    size_t i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        if (compare(array[i], array[j]) <= 0) {
            i++;
        } 
        else {
            // Rotate array[j] into position i, shifting array[i..j-1] right.
            ElementType v = array[j];
            for (size_t k = j; k > i; k--) {
                array[k] = array[k - 1];
            }

            array[i] = v;
            i++;
            mid++;
            j++;
        }
    }
}


/**
 * @brief Merge two already-sorted external arrays into one sorted array.
 *
 * Allocates a brand-new array of `size1 + size2` element pointers and fills
 * it with the merged result. The pointers are NOT cloned — `arr1` and `arr2`
 * still own the underlying data.
 *
 * @param arr1   First sorted input array.
 * @param size1  Length of `arr1`.
 * @param arr2   Second sorted input array.
 * @param size2  Length of `arr2`.
 * @param compare User-provided comparator.
 * @param swap   Unused (provided for API consistency).
 * @param[out] result_size Filled with `size1 + size2` on success.
 * @return Newly malloc'd merged array, or NULL on OOM / bad arguments.
 *         Caller frees with `free()`.
 */
ElementType* sort_merge_sorted_arrays(ElementType* arr1, size_t size1, ElementType* arr2, size_t size2, CompareFunc compare, SwapFunc swap, size_t* result_size) {
    SORT_LOG("[sort_merge_sorted_arrays]: enter (arr1=%p, size1=%zu, arr2=%p, size2=%zu)", (void*)arr1, size1, (void*)arr2, size2);
    (void)swap;
    if (!compare || !result_size) {
        SORT_LOG("[sort_merge_sorted_arrays]: guard tripped (compare=%p, result_size=%p) -> early exit (NULL)", (void*)compare, (void*)result_size);
        return NULL;
    }

    *result_size = 0;
    size_t total = size1 + size2;
    if (total == 0) {
        SORT_LOG("[sort_merge_sorted_arrays]: total==0 -> early exit (NULL)");
        return NULL;
    }

    ElementType* out = (ElementType*)malloc(total * sizeof(ElementType));
    if (!out) {
        SORT_LOG("[sort_merge_sorted_arrays]: allocation failed -> early exit (NULL)");
        return NULL;
    }

    size_t i = 0, j = 0, k = 0;
    while (i < size1 && j < size2) {
        if (compare(arr1[i], arr2[j]) <= 0) {
            out[k++] = arr1[i++];
        }
        else {
            out[k++] = arr2[j++];
        }
    }
    while (i < size1) {
        out[k++] = arr1[i++];
    }
    while (j < size2) {
        out[k++] = arr2[j++];
    }

    *result_size = total;
    return out;
}


/**
 * @brief Insert an element into a max-heap stored as an array.
 *
 * Appends `element` at the next free slot (`array[*size]`), then sifts it up
 * to restore the max-heap property. `*size` is incremented in place.
 *
 * The caller must ensure `array` has capacity for `*size + 1` elements.
 *
 * @param array     Heap-array of element pointers.
 * @param[in,out] size  Pointer to current heap size; incremented on success.
 * @param element   Element pointer to insert.
 * @param compare   User-provided comparator.
 * @param swap      User-provided swap.
 */
void sort_heap_insert(ElementType* array, size_t* size, ElementType element, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_heap_insert]: enter (array=%p, size=%p, element=%p)", (void*)array, (void*)size, (void*)element);
    if (!array || !size || !compare || !swap) {
        SORT_LOG("[sort_heap_insert]: guard tripped (array=%p, size=%p) -> early exit", (void*)array, (void*)size);
        return;
    }
    size_t i = *size;
    array[i] = element;
    (*size)++;
    // Sift up
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (compare(array[i], array[parent]) > 0) {
            swap(&array[i], &array[parent]);
            i = parent;
        } 
        else {
            break;
        }
    }
}


/**
 * @brief Remove and return the maximum element from a max-heap.
 *
 * Swaps the root with the last element, decrements `*size`, and re-heapifies
 * from the root. Returns the previously-root element.
 *
 * @param array     Heap-array of element pointers.
 * @param[in,out] size  Pointer to current heap size; decremented on success.
 * @param compare   User-provided comparator.
 * @param swap      User-provided swap.
 * @return The removed maximum, or NULL if the heap is empty.
 */
ElementType sort_heap_extract_max(ElementType* array, size_t* size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_heap_extract_max]: enter (array=%p, size=%p)", (void*)array, (void*)size);
    if (!array || !size || *size == 0 || !compare || !swap) {
        SORT_LOG("[sort_heap_extract_max]: guard tripped (array=%p, size=%p) -> early exit (NULL)", (void*)array, (void*)size);
        return NULL;
    }

    ElementType max = array[0];
    (*size)--;
    if (*size > 0) {
        array[0] = array[*size];
        sort_heapify(array, *size, 0, compare, swap);
    }

    return max;
}


/**
 * @brief Partially sort an array so the smallest `k` elements occupy the first
 *        `k` positions in sorted order. The rest of the array is left unsorted.
 *
 * Implementation: build a max-heap over the first `k` elements, then scan the
 * remaining elements; each one that is smaller than the heap's root replaces
 * it. Finally heap-sort the first `k` to put them in ascending order.
 *
 * @param array   Array of element pointers (mutated in place).
 * @param size    Total array length.
 * @param k       Number of smallest elements to expose at the front.
 * @param compare User-provided comparator.
 * @param swap    User-provided swap.
 * @return Pointer to the start of the sorted prefix (= `array`).
 */
ElementType* sort_partial_sort(ElementType* array, size_t size, size_t k, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_partial_sort]: enter (array=%p, size=%zu, k=%zu)", (void*)array, size, k);
    if (!array || !compare || !swap || k == 0) {
        SORT_LOG("[sort_partial_sort]: guard tripped (array=%p, k=%zu) -> early exit", (void*)array, k);
        return array;
    }
    if (k >= size) {
        SORT_LOG("[sort_partial_sort]: k>=size, falling back to full quicksort");
        sort_quicksort(array, size, compare, swap);
        return array;
    }

    sort_build_heap(array, k, compare, swap);

    // Push every later element through the heap if it's smaller than the root.
    for (size_t i = k; i < size; i++) {
        if (compare(array[i], array[0]) < 0) {
            ElementType tmp = array[0];
            array[0] = array[i];
            array[i] = tmp;
            sort_heapify(array, k, 0, compare, swap);
        }
    }

    // Sort the prefix ascending (heap-sort: repeatedly extract max).
    for (size_t i = k; i > 1; i--) {
        ElementType tmp = array[0];
        array[0] = array[i - 1];
        array[i - 1] = tmp;
        sort_heapify(array, i - 1, 0, compare, swap);
    }
    return array;
}


/**
 * @brief Quickselect-style nth-element: rearrange `array` so the element that
 *        would be at sorted index `n` ends up at `array[n]`.
 *
 * After the call, all elements before index `n` compare less than or equal to
 * `array[n]`, and all elements after are greater than or equal. The two
 * sub-ranges themselves are NOT sorted.
 *
 * Average O(n). Each round splits the range three ways (< pivot, == pivot,
 * > pivot) around a median-of-three pivot, so inputs with many equal keys
 * stay linear (the old two-way partition put every equal key on one side and
 * was quadratic on them).
 *
 * @param array   Array of element pointers.
 * @param size    Length of the array.
 * @param n       Target index (must be < `size`).
 * @param compare User-provided comparator.
 * @param swap    User-provided swap.
 * @return The element that ends up at `array[n]`, or NULL on bad args.
 */
ElementType sort_nth_element(ElementType* array, size_t size, size_t n, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_nth_element]: enter (array=%p, size=%zu, n=%zu)", (void*)array, size, n);
    if (!array || !compare || !swap || n >= size) {
        SORT_LOG("[sort_nth_element]: guard tripped (array=%p, size=%zu, n=%zu) -> early exit (NULL)", (void*)array, size, n);
        return NULL;
    }

    size_t left = 0, right = size - 1;
    while (left < right) {
        /* median of three as the pivot value (the element pointer is copied:
           swaps only move array slots) */
        size_t mid = left + (right - left) / 2;
        ElementType a = array[left], b = array[mid], c = array[right];
        ElementType pivot;
        if (compare(a, b) < 0) {
            pivot = (compare(b, c) < 0) ? b : (compare(a, c) < 0) ? c : a;
        }
        else {
            pivot = (compare(a, c) < 0) ? a : (compare(b, c) < 0) ? c : b;
        }
        /* [left, lt) < pivot, [lt, gt] == pivot, (gt, right] > pivot */
        size_t lt = left, i = left, gt = right;
        bool gtBelowZero = false;
        while (i <= gt && !gtBelowZero) {
            int r = compare(array[i], pivot);
            if (r < 0) {
                if (lt != i) {
                    swap(&array[lt], &array[i]);
                }
                lt++;
                i++;
            }
            else if (r > 0) {
                if (i != gt) {
                    swap(&array[i], &array[gt]);
                }
                if (gt == 0) {
                    gtBelowZero = true;
                }
                else {
                    gt--;
                }
            }
            else {
                i++;
            }
        }
        if (n < lt) {
            right = lt - 1;      /* lt > left here: the pivot value is in the range */
        }
        else if (gtBelowZero || n <= gt) {
            return array[n];     /* n falls among the keys equal to the pivot */
        }
        else {
            left = gt + 1;
        }
    }
    return array[n];
}


/**
 * @brief Returns the median element of the array.
 *
 * Computes the element at position `size/2` after a quickselect partitioning.
 * For even-length arrays, returns the *upper* median (the n/2-th element in
 * 0-indexed terms).
 *
 * @param array   Array of element pointers (will be partially rearranged).
 * @param size    Length of the array (must be ≥ 1).
 * @param compare User-provided comparator.
 * @param swap    User-provided swap.
 * @return The median element, or NULL on bad args.
 */
ElementType sort_median(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap) {
    SORT_LOG("[sort_median]: enter (array=%p, size=%zu)", (void*)array, size);
    if (!array || size == 0 || !compare || !swap) {
        SORT_LOG("[sort_median]: guard tripped (array=%p, size=%zu) -> early exit (NULL)", (void*)array, size);
        return NULL;
    }

    return sort_nth_element(array, size, size / 2, compare, swap);
}


/**
 * @brief Find the contiguous range of elements equal to `target` in a sorted
 *        array. Equivalent to C++'s `std::equal_range`.
 *
 * @param array   Sorted array of element pointers.
 * @param size    Length of the array.
 * @param target  Element to search for.
 * @param compare User-provided comparator.
 * @param[out] count  Filled with the number of matching elements (0 if none).
 * @return Pointer to the first matching element in `array`, or NULL if not
 *         present. The matching range is contiguous of length `*count`.
 */
ElementType* sort_binary_search_range(ElementType* array, size_t size, ElementType target, CompareFunc compare, size_t* count) {
    SORT_LOG("[sort_binary_search_range]: enter (array=%p, size=%zu, target=%p)", (void*)array, size, (void*)target);
    if (count) {
        *count = 0;
    }
    if (!array || size == 0 || !compare) {
        SORT_LOG("[sort_binary_search_range]: guard tripped (array=%p, size=%zu) -> early exit (NULL)", (void*)array, size);
        return NULL;
    }

    size_t lo = sort_lower_bound(array, size, target, compare);
    size_t hi = sort_upper_bound(array, size, target, compare);

    if (lo == hi) {
        return NULL;
    }
    if (count) {
        *count = hi - lo;
    }
    return &array[lo];
}


/**
 * @brief Run a fixed suite of sorting algorithms over the same input and
 *        print a per-algorithm timing / comparison / swap table.
 *
 * Each algorithm sorts a fresh copy of the input so the comparisons are
 * fair. Algorithms exercised: Insertion, QuickSort, MergeSort, HeapSort,
 * BubbleSort, Selection, Shell, Introspective.
 *
 * The original `array` is left unchanged.
 *
 * @param array   Input array of element pointers (read-only).
 * @param size    Number of elements.
 * @param compare User comparator.
 * @param swap    User swap.
 * @param print   Per-element print callback (currently unused — accepted
 *                for API symmetry with related functions).
 */
void sort_benchmark_algorithms(ElementType* array, size_t size, CompareFunc compare, SwapFunc swap, PrintFunc print) {
    SORT_LOG("[sort_benchmark_algorithms]: enter (array=%p, size=%zu)", (void*)array, size);
    (void)print;

    if (!array || size == 0 || !compare || !swap) {
        SORT_LOG("[sort_benchmark_algorithms]: guard tripped (array=%p, size=%zu) -> early exit", (void*)array, size);
        return;
    }

    SortAlgorithm algorithms[] = {
        SORT_INSERTION, SORT_QUICKSORT, SORT_MERGESORT, SORT_HEAPSORT,
        SORT_BUBBLESORT, SORT_SELECTION, SORT_SHELL, SORT_INTROSPECTIVE
    };

    const char* algorithm_names[] = {
        "Insertion", "QuickSort", "MergeSort", "HeapSort",
        "BubbleSort", "Selection", "Shell", "Introspective"
    };

    ElementType* test_array = (ElementType*)malloc(size * sizeof(ElementType));
    if (!test_array) {
        SORT_LOG("[sort_benchmark_algorithms]: allocation failed -> early exit");
        return;
    }

    for (size_t i = 0; i < sizeof(algorithms) / sizeof(algorithms[0]); i++) {
        // Copy original array
        memcpy(test_array, array, size * sizeof(ElementType));

        SortStats stats;
        sort_init_stats(&stats);

        double start_time = get_time_ms();
        sort_generic(test_array, size, compare, swap, algorithms[i], &stats);
        double end_time = get_time_ms();

        printf("%-12s: %.3f ms, %zu comparisons, %zu swaps\n", algorithm_names[i], end_time - start_time, stats.comparisons, stats.swaps);
    }

    free(test_array);
} 
