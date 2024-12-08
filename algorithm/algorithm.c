/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Algorithm
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "algorithm.h"

static void swap(void *a, void *b, size_t size) {
    ALGORITHM_LOG("[swap] Swapping elements of size %zu bytes.", size);
    
    unsigned char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);

    ALGORITHM_LOG("[swap] Swapped elements at memory locations %p and %p.", a, b);
}

static void reverse(void *first, void *last, size_t size) {
    ALGORITHM_LOG("[reverse] Reversing elements between memory locations %p and %p, each of size %zu bytes.", first, last, size);

    char *a = (char *)first;
    char *b = (char *)last - size;

    while (a < b) {
        ALGORITHM_LOG("[reverse] Swapping elements at positions %p and %p.", a, b);
        swap(a, b, size);
        a += size;
        b -= size;
    }

    ALGORITHM_LOG("[reverse] Completed reversing elements.");
}

static void quickSortInternal(void *base, size_t low, size_t high, size_t size, CompareFunc comp, void *temp) {
    if (low < high) {
        ALGORITHM_LOG("[quickSortInternal] Sorting range [%zu, %zu], pivot element at index %zu.", low, high, high);

        char* pivot = (char*)base + high * size;
        size_t i = low;

        for (size_t j = low; j < high; j++) {
            if (comp((char*)base + j * size, pivot) < 0) {
                ALGORITHM_LOG("[quickSortInternal] Swapping elements at indices %zu and %zu.", i, j);
                memcpy(temp, (char*)base + j * size, size);
                memcpy((char*)base + j * size, (char*)base + i * size, size);
                memcpy((char*)base + i * size, temp, size);
                i++;
            }
        }

        ALGORITHM_LOG("[quickSortInternal] Placing pivot element at index %zu.", i);
        memcpy(temp, (char*)base + i * size, size);
        memcpy((char*)base + i * size, pivot, size);
        memcpy(pivot, temp, size);

        if (i > 0) {
            ALGORITHM_LOG("[quickSortInternal] Recursively sorting left part: [%zu, %zu].", low, i - 1);
            quickSortInternal(base, low, i - 1, size, comp, temp);
        }

        ALGORITHM_LOG("[quickSortInternal] Recursively sorting right part: [%zu, %zu].", i + 1, high);
        quickSortInternal(base, i + 1, high, size, comp, temp);
    }
}

static void merge(void *base, size_t low, size_t mid, size_t high, size_t size, CompareFunc comp, void *temp) {
    ALGORITHM_LOG("[merge] Merging two halves: [%zu, %zu) and [%zu, %zu).", low, mid, mid, high);

    size_t i = low, j = mid, k = 0;

    while (i < mid && j < high) {
        if (comp((char*)base + i * size, (char*)base + j * size) <= 0) {
            memcpy((char*)temp + k * size, (char*)base + i * size, size);
            i++;
        } 
        else {
            memcpy((char*)temp + k * size, (char*)base + j * size, size);
            j++;
        }
        k++;
    }
    while (i < mid) {
        memcpy((char*)temp + k * size, (char*)base + i * size, size);
        i++;
        k++;
    }
    while (j < high) {
        memcpy((char*)temp + k * size, (char*)base + j * size, size);
        j++;
        k++;
    }

    ALGORITHM_LOG("[merge] Merged result copied back to base array, range [%zu, %zu).", low, high);
    memcpy((char*)base + low * size, temp, k * size);
}

static void mergeSortInternal(void *base, size_t low, size_t high, size_t size, CompareFunc comp, void *temp) {
    if (high - low > 1) {
        size_t mid = low + (high - low) / 2;
        ALGORITHM_LOG("[mergeSortInternal] Splitting range [%zu, %zu) at midpoint %zu.", low, high, mid);

        mergeSortInternal(base, low, mid, size, comp, temp);
        mergeSortInternal(base, mid, high, size, comp, temp);

        ALGORITHM_LOG("[mergeSortInternal] Merging sorted halves [%zu, %zu) and [%zu, %zu).", low, mid, mid, high);
        merge(base, low, mid, high, size, comp, temp);
    }
}

/**
 * @brief Sorts an array using a stable merge sort algorithm.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to order the elements.
 */
void algorithm_stable_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num > 1) {
        ALGORITHM_LOG("[algorithm_stable_sort] Starting stable sort for %zu elements.", num);
        void* temp = malloc(num * size);

        if (temp) {
            ALGORITHM_LOG("[algorithm_stable_sort] Memory allocation successful. Proceeding with merge sort.");
            mergeSortInternal(base, 0, num, size, comp, temp);
            free(temp);
            ALGORITHM_LOG("[algorithm_stable_sort] Stable sort completed.");
        } 
        else {
            perror("Failed to allocate memory for stable sorting");
            exit(EXIT_FAILURE);
        }
    } 
    else {
        ALGORITHM_LOG("[algorithm_stable_sort] No sorting needed for %zu elements.", num);
    }
}

/**
 * @brief Sorts an array using a non-stable quicksort algorithm.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to order the elements.
 */
void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num > 1) {
        ALGORITHM_LOG("[algorithm_sort] Starting quicksort for %zu elements.", num);
        void* temp = malloc(size);

        if (temp) {
            ALGORITHM_LOG("[algorithm_sort] Memory allocation successful. Proceeding with quicksort.");
            quickSortInternal(base, 0, num - 1, size, comp, temp);
            free(temp);
            ALGORITHM_LOG("[algorithm_sort] Quicksort completed.");
        } 
        else {
            perror("Failed to allocate memory for sorting");
            exit(EXIT_FAILURE);
        }
    } 
    else {
        ALGORITHM_LOG("[algorithm_sort] No sorting needed for %zu elements.", num);
    }
}

/**
 * @brief Finds the first element in a range that matches a specified value.
 *
 * This function searches for the first element in the range `[base, base + num * size)` that is equal to `val`
 * based on the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to find.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return Pointer to the first matching element, or `NULL` if no match is found.
 */
void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    const char *ptr = (const char*)base;
    ALGORITHM_LOG("[algorithm_find] Searching for the element in an array of %zu elements.", num);

    for (size_t i = 0; i < num; i++) {
        if (comp(ptr + i * size, val) == 0) {
            ALGORITHM_LOG("[algorithm_find] Element found at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_find] Element not found in the array.");
    return NULL;
}

/**
 * @brief Finds the first element in a range that satisfies a predicate.
 *
 * This function searches for the first element in the range `[base, base + num * size)` that satisfies
 * the predicate function `pred`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for the element to be found.
 * 
 * @return Pointer to the first element that satisfies the predicate, or `NULL` if no such element is found.
 */
void *algorithm_find_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char*)base;
    ALGORITHM_LOG("[algorithm_find_if] Starting search in array of %zu elements.", num);

    for (size_t i = 0; i < num; i++) {
        if (pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_find_if] Element matching predicate found at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_find_if] No element matching the predicate found.");
    return NULL;
}

/**
 * @brief Finds the first element in a range that does not satisfy a predicate.
 *
 * This function searches for the first element in the range `[base, base + num * size)` that does not satisfy
 * the predicate function `pred`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for elements to be skipped.
 * 
 * @return Pointer to the first element that does not satisfy the predicate, or `NULL` if all elements satisfy it.
 */
void *algorithm_find_if_not(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_find_if_not] Info: Searching for the first element that does not satisfy the predicate in %zu elements.", num);

    const char *ptr = (const char*)base;
    for (size_t i = 0; i < num; i++) {
        if (!pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_find_if_not] Success: Element found at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_find_if_not] Info: No element found that does not satisfy the predicate.");
    return NULL;
}

/**
 * @brief Finds the last occurrence of a subsequence in a range.
 *
 * This function searches for the last occurrence of the sequence `[base2, base2 + num2 * size2)` in the range
 * `[base1, base1 + num1 * size1)`.
 *
 * @param base1 Pointer to the start of the main array.
 * @param num1 Number of elements in the main array.
 * @param size1 Size of each element in the main array in bytes.
 * @param base2 Pointer to the start of the subsequence array.
 * @param num2 Number of elements in the subsequence array.
 * @param size2 Size of each element in the subsequence array in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return Pointer to the start of the last occurrence of the subsequence, or `NULL` if not found.
 */
void *algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_find_end] Info: Searching for the last occurrence of the subsequence in the main array.");

    if (num2 == 0) {
        ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence is empty, returning pointer to the end of the main array.");
        return (void*)((char*)base1 + num1 * size1);
    }

    void *ret = NULL;
    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    for (size_t i = 0; i <= num1 - num2; ++i) {
        bool found = true;
        for (size_t j = 0; j < num2; ++j) {
            if (!comp(ptr1 + (i + j) * size1, ptr2 + j * size2)) {
                found = false;
                break;
            }
        }
        if (found) {
            ret = (void *)(ptr1 + i * size1);
            ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence found starting at index %zu.", i);
        }
    }

    if (ret == NULL) {
        ALGORITHM_LOG("[algorithm_find_end] Info: Subsequence not found.");
    }
    return ret;
}

/**
 * @brief Finds the first element in a range that matches any element in another range.
 *
 * This function searches for the first element in the range `[base1, base1 + num1 * size1)` that matches
 * any element in the range `[base2, base2 + num2 * size2)`.
 *
 * @param base1 Pointer to the start of the main array.
 * @param num1 Number of elements in the main array.
 * @param size1 Size of each element in the main array in bytes.
 * @param base2 Pointer to the start of the array to search for matches.
 * @param num2 Number of elements in the array to search for matches.
 * @param size2 Size of each element in the array to search for matches in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return Pointer to the first matching element in `base1`, or `NULL` if no match is found.
 */
void *algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_find_first_of] Info: Searching for the first occurrence of any element from the second array in the main array.");

    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    for (size_t i = 0; i < num1; ++i) {
        for (size_t j = 0; j < num2; ++j) {
            if (comp(ptr1 + i * size1, ptr2 + j * size2) == 0) {
                ALGORITHM_LOG("[algorithm_find_first_of] Success: Match found at index %zu in the main array.", i);
                return (void *)(ptr1 + i * size1);
            }
        }
    }

    ALGORITHM_LOG("[algorithm_find_first_of] Info: No match found.");
    return NULL;
}

/**
 * @brief Performs a binary search to find the index of a value in a sorted range.
 *
 * This function performs a binary search on a sorted range `[base, base + num * size)` to find the index
 * of the value `val`. The range must be sorted according to the same comparison function `comp`.
 *
 * @param base Pointer to the start of the sorted array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to find.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return The index of the found element, or `(size_t)-1` if the value is not found.
 */
size_t algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_binary_search] Info: Performing binary search on a sorted array of %zu elements.", num);

    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        int result = comp(mid_elem, val);
        if (result < 0) {
            low = mid + 1;
        } 
        else if (result > 0) {
            high = mid;
        } 
        else {
            ALGORITHM_LOG("[algorithm_binary_search] Success: Element found at index %zu.", mid);
            return (size_t)mid;
        }
    }

    ALGORITHM_LOG("[algorithm_binary_search] Info: Element not found.");
    return (size_t)-1;
}

/**
 * @brief Finds the maximum element in a range.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` to find the maximum
 * element, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the maximum element, or `NULL` if the array is empty.
 */
void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        ALGORITHM_LOG("[algorithm_max_element] Error: Array is empty, returning NULL.");
        return NULL;
    }
    ALGORITHM_LOG("[algorithm_max_element] Info: Searching for the maximum element in an array of %zu elements.", num);

    const char *max_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(max_element, element) < 0) {
            max_element = element;
            ALGORITHM_LOG("[algorithm_max_element] Info: New maximum element found at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_max_element] Success: Maximum element found.");
    return (void *)max_element;
}

/**
 * @brief Finds the minimum element in a range.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` to find the minimum
 * element, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the minimum element, or `NULL` if the array is empty.
 */
void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        ALGORITHM_LOG("[algorithm_min_element] Error: Array is empty, returning NULL.");
        return NULL;
    }
    ALGORITHM_LOG("[algorithm_min_element] Info: Searching for the minimum element in an array of %zu elements.", num);

    const char *min_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(min_element, element) > 0) {
            min_element = element;
            ALGORITHM_LOG("[algorithm_min_element] Info: New minimum element found at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_min_element] Success: Minimum element found.");
    return (void *)min_element;
}

/**
 * @brief Applies a function to each element in an array.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param op Function to apply to each element.
 */
void algorithm_for_each(void *base, size_t num, size_t size, ForEachOpFunc op) {
    ALGORITHM_LOG("[algorithm_for_each] Info: Applying operation to each of %zu elements.", num);

    char *ptr = (char *)base;
    for (size_t i = 0; i < num; ++i) {
        op(ptr + i * size);
    }

    ALGORITHM_LOG("[algorithm_for_each] Success: Operation applied to all elements.");
}

/**
 * @brief Copies elements from a source array to a destination array.
 *
 * @param source Pointer to the source array.
 * @param num Number of elements to copy.
 * @param size Size of each element in bytes.
 * @param dest Pointer to the destination array.
 */
void algorithm_copy(const void *source, size_t num, size_t size, void *dest) {
    ALGORITHM_LOG("[algorithm_copy] Info: Copying %zu elements.", num);

    const char *src_ptr = (const char *)source;
    char *dest_ptr = (char *)dest;

    for (size_t i = 0; i < num; ++i) {
        memcpy(dest_ptr + i * size, src_ptr + i * size, size);
        ALGORITHM_LOG("[algorithm_copy] Info: Copied element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_copy] Success: Copy completed.");
}

/**
 * @brief Accumulates a result by applying an operation sequentially to the elements in a range.
 *
 * This function applies the operation `op` to the initial value `init` and each element in the range
 * `[base, base + num * size)` to accumulate a result. The result is stored in `init`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param init Pointer to the initial value and where the result will be stored.
 * @param op Function that defines how to accumulate the values.
 * 
 * @return Pointer to the result, which is the same as the `init` parameter.
 */
void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, AccumulateOpFunc op) {
    ALGORITHM_LOG("[algorithm_accumulate] Info: Accumulating %zu elements.", num);

    char *result = (char *)init;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        op(result, ptr + i * size);
        ALGORITHM_LOG("[algorithm_accumulate] Info: Accumulated element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_accumulate] Success: Accumulation completed.");
    return result;
}

/**
 * @brief Checks if all elements in a range satisfy a predicate.
 *
 * This function returns `true` if the predicate `pred` returns `true` for all elements in the range
 * `[base, base + num * size)`. Otherwise, it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function to check each element.
 * 
 * @return `true` if all elements satisfy the predicate, `false` otherwise.
 */
bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_all_of] Info: Checking if all of %zu elements satisfy the predicate.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (!pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_all_of] Info: Element at index %zu does not satisfy the predicate.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_all_of] Success: All elements satisfy the predicate.");
    return true;
}

/**
 * @brief Checks if any element in a range satisfies a predicate.
 *
 * This function returns `true` if the predicate `pred` returns `true` for any element in the range
 * `[base, base + num * size)`. Otherwise, it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function to check each element.
 * 
 * @return `true` if any element satisfies the predicate, `false` otherwise.
 */
bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_any_of] Info: Checking if any element of %zu satisfies the predicate.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_any_of] Success: Element at index %zu satisfies the predicate.", i);
            return true;
        }
    }

    ALGORITHM_LOG("[algorithm_any_of] Info: No elements satisfy the predicate.");
    return false;
}

/**
 * @brief Checks if none of the elements in a range satisfy a predicate.
 *
 * This function returns `true` if the predicate `pred` returns `false` for all elements in the range
 * `[base, base + num * size)`. Otherwise, it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function to check each element.
 * 
 * @return `true` if none of the elements satisfy the predicate, `false` otherwise.
 */
bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_none_of] Info: Checking if none of the elements of %zu satisfy the predicate.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ALGORITHM_LOG("[algorithm_none_of] Info: Element at index %zu satisfies the predicate, returning false.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_none_of] Success: None of the elements satisfy the predicate.");
    return true;
}

/**
 * @brief Fills a range with a specified value.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to fill the range with.
 */
void algorithm_fill(void *first, void *last, size_t size, const void *val) {
    ALGORITHM_LOG("[algorithm_fill] Info: Filling range with value.");

    for (char *ptr = (char*)first; ptr != last; ptr += size) {
        memcpy(ptr, val, size);
    }

    ALGORITHM_LOG("[algorithm_fill] Success: Range filled with value.");
}

/**
 * @brief Fills the first n elements of a range with a specified value.
 *
 * @param first Pointer to the start of the range.
 * @param n Number of elements to fill.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to fill the range with.
 */
void algorithm_fill_n(void *first, size_t n, size_t size, const void *val) {
    ALGORITHM_LOG("[algorithm_fill_n] Info: Filling first %zu elements with value.", n);

    for (char *ptr = (char*)first; n > 0; ptr += size, n--) {
        memcpy(ptr, val, size);
    }

    ALGORITHM_LOG("[algorithm_fill_n] Success: %zu elements filled with value.", n);
}

/**
 * @brief Counts the number of occurrences of a value in a range.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` and counts
 * how many times the value `val` appears, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to be counted.
 * @param comp Comparison function used to compare elements.
 * It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return The number of times `val` appears in the range.
 */
size_t algorithm_count(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_count] Info: Counting occurrences of value in array of %zu elements.", num);

    size_t count = 0;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) == 0) {
            ++count;
            ALGORITHM_LOG("[algorithm_count] Info: Found match at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_count] Success: Total occurrences found: %zu.", count);
    return count;
}

/**
 * @brief Counts the number of elements in a range that satisfy a predicate function.
 *
 * This function iterates over the elements in the range `[base, base + num * size)` and counts
 * how many of them satisfy the predicate function `pred`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for elements that should be counted.
 * 
 * @return The number of elements that satisfy the predicate function `pred`.
 */
size_t algorithm_count_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_count_if] Info: Counting elements that satisfy the predicate in array of %zu elements.", num);

    size_t count = 0;
    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ++count;
            ALGORITHM_LOG("[algorithm_count_if] Info: Predicate satisfied at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_count_if] Success: Total elements that satisfy the predicate: %zu.", count);
    return count;
}

/**
 * @brief Randomly shuffles elements in an array.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param rng Random number generator function.
 */
void algorithm_shuffle(void *base, size_t num, size_t size, UniformRandomNumberGenerator rng) {
    ALGORITHM_LOG("[algorithm_shuffle] Info: Shuffling array of %zu elements.", num);

    if (num > 1) {
        char *arr = (char *)base;
        char *temp = (char*) malloc(size);

        if (!temp) {
            ALGORITHM_LOG("[algorithm_shuffle] Error: Memory allocation failed for temp.");
            exit(EXIT_FAILURE);
        }

        for (size_t i = num - 1; i > 0; i--) {
            size_t j = rng() % (i + 1);
            ALGORITHM_LOG("[algorithm_shuffle] Info: Swapping elements at index %zu and %zu.", i, j);

            // Swap arr[i] and arr[j]
            memcpy(temp, arr + i * size, size);
            memcpy(arr + i * size, arr + j * size, size);
            memcpy(arr + j * size, temp, size);
        }

        free(temp);
        ALGORITHM_LOG("[algorithm_shuffle] Success: Shuffling completed.");
    }
}

/**
 * @brief Finds the first position in a sorted range where a value can be inserted without violating the order.
 *
 * This function performs a binary search on a sorted range `[base, base + num * size)` to find the first
 * position where `val` can be inserted while maintaining the sorted order. The range must be sorted
 * according to the same comparison function `comp`.
 *
 * @param base Pointer to the start of the sorted array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to be inserted.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the first position where `val` can be inserted without violating the order.
 */
void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_lower_bound] Info: Performing lower bound search in array of %zu elements.", num);

    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        if (comp(mid_elem, val) < 0) {
            low = mid + 1;
        } 
        else {
            high = mid;
        }
    }

    ALGORITHM_LOG("[algorithm_lower_bound] Success: Found lower bound at index %zu.", low);
    return (void *)((const char *)base + low * size);
}

/**
 * @brief Finds the first position in a sorted range where a value is greater than a specified value.
 *
 * This function performs a binary search on a sorted range `[base, base + num * size)` to find the first
 * position where the elements are greater than `val`. The range must be sorted according to the same comparison function `comp`.
 *
 * @param base Pointer to the start of the sorted array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to compare against.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return Pointer to the first position where elements are greater than `val`.
 */
void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_upper_bound] Info: Performing upper bound search in array of %zu elements.", num);

    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        if (comp(mid_elem, val) <= 0) {
            low = mid + 1;
        } 
        else {
            high = mid;
        }
    }

    ALGORITHM_LOG("[algorithm_upper_bound] Success: Found upper bound at index %zu.", low);
    return (void *)((const char *)base + low * size);
}

/**
 * @brief Transforms each element in an array and stores the result in another array.
 *
 * @param base Pointer to the start of the input array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the output array where the results will be stored.
 * @param op Transformation function to apply to each element.
 */
void algorithm_transform(const void *base, size_t num, size_t size, void *result, TransformFunc op) {
    ALGORITHM_LOG("[algorithm_transform] Info: Applying transformation to %zu elements.", num);

    const char *input_ptr = (const char *)base;
    char *output_ptr = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        ALGORITHM_LOG("[algorithm_transform] Info: Transforming element at index %zu.", i);
        op(output_ptr + i * size, input_ptr + i * size);
    }

    ALGORITHM_LOG("[algorithm_transform] Success: Transformation completed.");
}

/**
 * @brief Reduces a range of elements to a single value using a specified binary operation.
 *
 * This function applies the binary operation `op` to each element in the range `[base, base + num * size)`
 * and accumulates the result, starting with the initial value `init`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param init Pointer to the initial value, which is also used to store the accumulated result.
 * @param op Binary operation function that takes two arguments: the accumulated result and the current element.
 * 
 * @return Pointer to the accumulated result (same as `init`).
 */
void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, ReduceFunc op) {
    ALGORITHM_LOG("[algorithm_reduce] Info: Reducing array of %zu elements.", num);

    const char *ptr = (const char *)base;
    char *result = (char *)init;

    for (size_t i = 0; i < num; ++i) {
        ALGORITHM_LOG("[algorithm_reduce] Info: Reducing element at index %zu.", i);
        op(result, ptr + i * size);
    }

    ALGORITHM_LOG("[algorithm_reduce] Success: Reduction completed.");
    return result;
}

/**
 * @brief Removes consecutive duplicate elements from a sorted array.
 *
 * This function removes consecutive duplicate elements from a sorted array `base` as determined
 * by the comparison function `comp`. The array must be sorted before calling this function.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return The number of unique elements remaining in the array.
 */
size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_unique] Info: Removing duplicates from array of %zu elements.", num);

    if (num == 0) {
        ALGORITHM_LOG("[algorithm_unique] Warning: Empty array, returning 0.");
        return 0;
    }

    size_t uniqueCount = 1; 
    char *array = (char *)base;

    for (size_t i = 1; i < num; ++i) {
        if (comp(array + (uniqueCount - 1) * size, array + i * size) != 0) {
            if (uniqueCount != i) {
                memcpy(array + uniqueCount * size, array + i * size, size);
                ALGORITHM_LOG("[algorithm_unique] Info: Moving element at index %zu to position %zu.", i, uniqueCount);
            }
            uniqueCount++;
        }
    }

    ALGORITHM_LOG("[algorithm_unique] Success: Number of unique elements is %zu.", uniqueCount);
    return uniqueCount;
}

/**
 * @brief Checks if two ranges are equal.
 *
 * This function compares two ranges `[base1, base1 + num1 * size1)` and `[base2, base2 + num2 * size2)`
 * to determine if they contain the same elements in the same order, as determined by the comparison function `comp`.
 *
 * @param base1 Pointer to the start of the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array in bytes.
 * @param base2 Pointer to the start of the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array in bytes.
 * @param comp Comparison function used to compare elements.It should return 0 if the elements are equal, and a non-zero value otherwise.
 * 
 * @return `true` if the two ranges are equal, `false` otherwise.
 */
bool algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, 
                     CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_equal] Info: Comparing two arrays of %zu and %zu elements.", num1, num2);
    if (num1 != num2 || size1 != size2) {
        ALGORITHM_LOG("[algorithm_equal] Warning: Arrays have different sizes, returning false.");
        return false;
    }

    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;

    for (size_t i = 0; i < num1; ++i) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            ALGORITHM_LOG("[algorithm_equal] Info: Mismatch found at index %zu.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_equal] Success: Arrays are equal.");
    return true;
}

/**
 * @brief Generates the next lexicographical permutation of a range.
 *
 * This function rearranges the elements in the range `[first, last)` into the next lexicographical
 * permutation, as determined by the comparison function `comp`. If such a permutation is not possible,
 * the range is rearranged into the smallest lexicographical order (sorted in ascending order).
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to one past the end of the range.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return `true` if the first element is less than the second, `false` otherwise.
 * 
 * @return `true` if the next permutation was generated, `false` if the range was reset to the smallest permutation.
 */
bool algorithm_next_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_next_permutation] Info: Generating next permutation.");
    if (first == last) {
        ALGORITHM_LOG("[algorithm_next_permutation] Warning: Empty range, returning false.");
        return false;
    }

    char *i = (char *)last - size;
    while (i != (char *)first) {
        char *j = i;
        i -= size;

        if (comp(i, j)) {
            char *k = (char *)last - size;
            while (!comp(i, k)) {
                k -= size;
            }

            ALGORITHM_LOG("[algorithm_next_permutation] Info: Swapping elements.");
            swap(i, k, size);
            reverse(j, last, size);

            ALGORITHM_LOG("[algorithm_next_permutation] Success: Next permutation generated.");
            return true;
        }

        if (i == (char *)first) {
            ALGORITHM_LOG("[algorithm_next_permutation] Info: Resetting to smallest permutation.");
            reverse(first, last, size);
            return false;
        }
    }

    return false; // This should not be reached
}

/**
 * @brief Generates the previous lexicographical permutation of a range.
 *
 * This function rearranges the elements in the range `[first, last)` into the previous lexicographical
 * permutation, as determined by the comparison function `comp`. If such a permutation is not possible,
 * the range is rearranged into the largest lexicographical order (sorted in descending order).
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to one past the end of the range.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * It should return `true` if the first element is less than the second, `false` otherwise.
 * 
 * @return `true` if the previous permutation was generated, `false` if the range was reset to the largest permutation.
 */
bool algorithm_prev_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_prev_permutation] Info: Generating previous permutation.");
    if (first == last) {
        ALGORITHM_LOG("[algorithm_prev_permutation] Warning: Empty range, returning false.");
        return false;
    }

    char *i = (char *)last - size;
    while (i != (char *)first) {
        char *j = i;
        i -= size;

        if (comp(j, i)) {
            char *k = (char *)last - size;
            while (!comp(k, i)) {
                k -= size;
            }

            ALGORITHM_LOG("[algorithm_prev_permutation] Info: Swapping elements.");
            swap(i, k, size);
            reverse(j, last, size);

            ALGORITHM_LOG("[algorithm_prev_permutation] Success: Previous permutation generated.");
            return true;
        }
        if (i == (char *)first) {
            ALGORITHM_LOG("[algorithm_prev_permutation] Info: Resetting to largest permutation.");
            
            reverse(first, last, size);
            return false;
        }
    }

    return false; 
}

/**
 * @brief Partitions a range of elements based on a predicate function.
 *
 * This function rearranges the elements in the range `[base, base + num * size)` so that all elements
 * for which the predicate function `pred` returns `true` appear before those for which it returns `false`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param pred Predicate function that returns `true` for elements that should appear before others.
 * 
 * @return Pointer to the first element in the second group (where `pred` returns `false`).
 */
void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_partition] Info: Partitioning array of %zu elements.", num);

    char *first = (char *)base;
    char *last = first + num * size;

    while (first != last) {
        while (first != last && pred(first)) {
            first += size;
        }
        do {
            last -= size;
            if (first == last) {
                break;
            }
        } while (!pred(last));

        if (first != last) {
            ALGORITHM_LOG("[algorithm_partition] Info: Swapping elements.");
            swap(first, last, size);
            first += size;
        }
    }

    ALGORITHM_LOG("[algorithm_partition] Success: Partitioning completed.");
    return first;
}

/**
 * @brief Generates values for a range using a generator function.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param gen Generator function that produces values.
 */
void algorithm_generate(void *first, void *last, size_t size, GeneratorFunc gen) {
    ALGORITHM_LOG("[algorithm_generate] Info: Generating values for range.");

    char *current = (char *)first;
    while (current != last) {
        gen(current);
        current += size;
        ALGORITHM_LOG("[algorithm_generate] Info: Value generated at position.");
    }

    ALGORITHM_LOG("[algorithm_generate] Success: Generation completed.");
}

/**
 * @brief Generates values for the first n elements of a range using a generator function.
 *
 * @param first Pointer to the start of the range.
 * @param n Number of elements to generate.
 * @param size Size of each element in bytes.
 * @param gen Generator function that produces values.
 */
void algorithm_generate_n(void *first, size_t n, size_t size, GeneratorFunc gen) {
    ALGORITHM_LOG("[algorithm_generate_n] Info: Generating values for %zu elements.", n);

    char *current = (char *)first;
    for (size_t i = 0; i < n; ++i) {
        gen(current);
        current += size;
        ALGORITHM_LOG("[algorithm_generate_n] Info: Value generated for element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_generate_n] Success: Generation completed.");
}

/**
 * @brief Copies a range of elements in reverse order.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array where elements will be copied.
 */
void algorithm_copy_backward(const void *first, const void *last, size_t size, void *result) {
    ALGORITHM_LOG("[algorithm_copy_backward] Info: Copying elements in reverse order.");
    const char *src = (const char *)last;
    char *dest = (char *)result;

    while (src != (const char *)first) {
        src -= size;  // Move source pointer backwards
        dest -= size; // Move destination pointer backwards
        memcpy(dest, src, size); 
        ALGORITHM_LOG("[algorithm_copy_backward] Info: Element copied.");
    }

    ALGORITHM_LOG("[algorithm_copy_backward] Success: Copying completed.");
}

/**
 * @brief Copies elements from a source array to a destination array if they satisfy a predicate function.
 *
 * @param first Pointer to the start of the source array.
 * @param last Pointer to the end of the source array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 * @param pred Predicate function that determines if an element should be copied.
 */
void algorithm_copy_if(const void *first, const void *last, size_t size, void *result, UnaryPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_copy_if] Info: Copying elements based on predicate.");

    const char *src = (const char *)first;
    char *dest = (char *)result;

    while (src != (const char *)last) {
        if (pred(src)) {
            memcpy(dest, src, size);
            dest += size;
            ALGORITHM_LOG("[algorithm_copy_if] Info: Element satisfying predicate copied.");
        }
        src += size;
    }

    ALGORITHM_LOG("[algorithm_copy_if] Success: Copying completed.");
}

/**
 * @brief Copies the first n elements from a source array to a destination array.
 *
 * @param first Pointer to the start of the source array.
 * @param n Number of elements to copy.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 */
void algorithm_copy_n(const void *first, size_t n, size_t size, void *result) {
    ALGORITHM_LOG("[algorithm_copy_n] Info: Copying %zu elements of size %zu.", n, size);

    const char *src = (const char *)first;
    char *dest = (char *)result;

    for (size_t i = 0; i < n; ++i) {
        memcpy(dest, src, size);
        src += size;
        dest += size;

        ALGORITHM_LOG("[algorithm_copy_n] Info: Copied element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_copy_n] Success: Copy completed.");
}

/**
 * @brief Finds the range of elements that are equal to a specified value in a sorted range.
 *
 * This function returns a `Pair` representing the range of elements in the sorted array `base`
 * that are equal to the value `val`, as determined by the comparison function `comp`. The
 * `first` element in the `Pair` is the first element in the range, and the `second` element
 * is one past the last element in the range.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to search for.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return A `Pair` representing the range of elements equal to `val`.
 *         `first` points to the first element in the range, and `second` points to one past the last element.
 */
Pair algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_equal_range] Info: Finding equal range for value in array with %zu elements.", num);

    Pair range;
    range.first = algorithm_lower_bound(base, num, size, val, comp);
    range.second = algorithm_upper_bound(range.first, num, size, val, comp);

    ALGORITHM_LOG("[algorithm_equal_range] Success: Equal range found.");
    return range;
}

/**
 * @brief Determines if a sorted range includes all the elements of another sorted range.
 *
 * This function checks if all elements of the sorted range `[first2, first2 + num2)` are contained
 * within the sorted range `[first1, first1 + num1)` as determined by the comparison function `comp`.
 *
 * @param first1 Pointer to the start of the first sorted array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array.
 * @param first2 Pointer to the start of the second sorted array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return `true` if all elements of the second range are contained within the first range, `false` otherwise.
 */
bool algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, 
                        CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_includes] Info: Checking if array1 includes array2 with %zu and %zu elements respectively.", num1, num2);

    const char *ptr1 = (const char *)first1;
    const char *ptr2 = (const char *)first2;
    const char *last1 = ptr1 + num1 * size1;
    const char *last2 = ptr2 + num2 * size2;

    while (ptr2 != last2) {
        if ((ptr1 == last1) || comp(ptr2, ptr1) < 0) {
            ALGORITHM_LOG("[algorithm_includes] Info: Array1 does not include Array2.");
            return false;
        }
        if (!comp(ptr1, ptr2)) {
            ptr2 += size2;
        }
        ptr1 += size1;
    }

    ALGORITHM_LOG("[algorithm_includes] Success: Array1 includes Array2.");
    return true;
}

/**
 * @brief Copies unique elements from a sorted range to a new range.
 *
 * This function copies unique elements from the sorted array `first` to the array `result`,
 * as determined by the comparison function `comp`. Elements that are equal (according to `comp`)
 * are only copied once.
 *
 * @param first Pointer to the start of the sorted array.
 * @param num Number of elements in the source array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the start of the destination array.
 * @param comp Comparison function used to compare elements.
 * It should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first element is greater than the second.
 * 
 * @return The number of unique elements copied to the destination array.
 */
size_t algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_unique_copy] Info: Copying unique elements from an array with %zu elements.", num);

    if (num == 0) {
        ALGORITHM_LOG("[algorithm_unique_copy] Info: No elements to copy.");
        return 0;
    }

    const char *src = (const char *)first;
    char *dst = (char *)result;
    memcpy(dst, src, size); 
    size_t count = 1;

    for (size_t i = 1; i < num; ++i) {
        if (comp(dst + (count - 1) * size, src + i * size) != 0) {
            memcpy(dst + count * size, src + i * size, size);
            ++count;
            ALGORITHM_LOG("[algorithm_unique_copy] Info: Unique element %zu copied.", count);
        }
    }

    ALGORITHM_LOG("[algorithm_unique_copy] Success: Copied %zu unique elements.", count);
    return count;
}

/**
 * @brief Swaps the values of two elements.
 *
 * @param a Pointer to the first element.
 * @param b Pointer to the second element.
 * @param size Size of each element in bytes.
 */
void algorithm_swap(void *a, void *b, size_t size) {
    ALGORITHM_LOG("[algorithm_swap] Info: Swapping two elements of size %zu.", size);

    void *temp = malloc(size);
    if (!temp) {
        ALGORITHM_LOG("[algorithm_swap] Error: Failed to allocate memory for temporary swap buffer.");
        exit(EXIT_FAILURE);
    }

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);

    free(temp);
    ALGORITHM_LOG("[algorithm_swap] Success: Swap completed.");
}

/**
 * @brief Swaps ranges of elements between two arrays.
 *
 * @param first1 Pointer to the start of the first array.
 * @param first2 Pointer to the start of the second array.
 * @param num Number of elements to swap.
 * @param size Size of each element in bytes.
 */
void algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size) {
    ALGORITHM_LOG("[algorithm_swap_ranges] Info: Swapping %zu elements between two arrays of element size %zu.", num, size);

    char *ptr1 = (char *)first1;
    char *ptr2 = (char *)first2;

    for (size_t i = 0; i < num; ++i) {
        algorithm_swap(ptr1 + i * size, ptr2 + i * size, size);
        ALGORITHM_LOG("[algorithm_swap_ranges] Info: Swapped element %zu.", i);
    }

    ALGORITHM_LOG("[algorithm_swap_ranges] Success: Swap ranges completed.");
}

/**
 * @brief Checks if the elements in a range are sorted in non-decreasing order.
 *
 * This function determines whether the elements in the array pointed to by `base` are sorted
 * in non-decreasing order, as defined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements. It should return a negative value
 *             if the first element is less than the second, zero if they are equal, and a positive
 *             value if the first element is greater than the second.
 * 
 * @return `true` if the elements are sorted in non-decreasing order, `false` otherwise.
 */
bool algorithm_is_sorted(const void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_is_sorted] Info: Checking if array of size %zu with element size %zu is sorted.", num, size);

    if (num < 2) {
        ALGORITHM_LOG("[algorithm_is_sorted] Success: Array with less than two elements is always sorted.");
        return true; 
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            ALGORITHM_LOG("[algorithm_is_sorted] Info: Array is not sorted at element %zu.", i);
            return false; 
        }
    }

    ALGORITHM_LOG("[algorithm_is_sorted] Success: Array is sorted.");
    return true; 
}

/**
 * @brief Finds the first position in a range where the elements are no longer sorted.
 *
 * This function searches through the array pointed to by `base` and finds the first element
 * where the sequence is no longer sorted in non-decreasing order, as defined by the comparison
 * function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements. It should return a negative value
 *             if the first element is less than the second, zero if they are equal, and a positive
 *             value if the first element is greater than the second.
 * 
 * @return Pointer to the first element where the sequence is no longer sorted.
 *         If the entire range is sorted, returns a pointer to the end of the array (i.e., `base + num * size`).
 */
void *algorithm_is_sorted_until(const void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_is_sorted_until] Info: Checking if the array is sorted until a specific point in an array of size %zu.", num);
    if (num < 2) {
        ALGORITHM_LOG("[algorithm_is_sorted_until] Success: Entire array is sorted (size < 2).");
        return (void *)((char *)base + num * size);
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            ALGORITHM_LOG("[algorithm_is_sorted_until] Info: Unsorted element found at index %zu.", i + 1);
            return (void *)(ptr + (i + 1) * size);
        }
    }

    ALGORITHM_LOG("[algorithm_is_sorted_until] Success: Entire array is sorted.");
    return (void *)((char *)base + num * size);
}

/**
 * @brief Rotates elements in a range.
 *
 * @param first Pointer to the start of the range.
 * @param middle Pointer to the middle of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 */
void algorithm_rotate(void *first, void *middle, void *last, size_t size) {
    ALGORITHM_LOG("[algorithm_rotate] Info: Rotating array elements in range [first, last) with middle at position %zu.", ((char *)middle - (char *)first) / size);
    char *next = (char *)middle;

    while ((char *)first != next) {
        swap(first, next, size);
        first = (char *)first + size;
        next = (char *)next + size;

        if (next == (char *)last) {
            next = (char *)middle;
        } 
        else if (first == (char *)middle) {
            middle = next;
        }
    }

    ALGORITHM_LOG("[algorithm_rotate] Success: Array rotation completed.");
}

/**
 * @brief Rotates elements in a range and copies them to a destination array.
 *
 * @param first Pointer to the start of the range.
 * @param middle Pointer to the middle of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 */
void algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result) {
    ALGORITHM_LOG("[algorithm_rotate_copy] Info: Copying rotated range [first, last) to result with middle at position %zu.", ((char *)middle - (char *)first) / size);

    const char *first_ptr = (const char *)first;
    const char *middle_ptr = (const char *)middle;
    const char *last_ptr = (const char *)last;
    char *result_ptr = (char *)result;

    while (middle_ptr != last_ptr) {
        memcpy(result_ptr, middle_ptr, size);
        result_ptr += size;
        middle_ptr += size;
    }
    while (first_ptr != (const char *)middle) {
        memcpy(result_ptr, first_ptr, size);
        result_ptr += size;
        first_ptr += size;
    }

    ALGORITHM_LOG("[algorithm_rotate_copy] Success: Rotated copy completed.");
}

/**
 * @brief Merges two sorted arrays into a single sorted array.
 *
 * @param base1 Pointer to the first sorted array.
 * @param num1 Number of elements in the first array.
 * @param base2 Pointer to the second sorted array.
 * @param num2 Number of elements in the second array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the array where the merged result will be stored.
 * @param comp Comparison function used to order the elements.
 */
void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_merge] Info: Merging two sorted arrays of sizes %zu and %zu.", num1, num2);

    size_t i = 0, j = 0, k = 0;
    const char *a = (const char *)base1;
    const char *b = (const char *)base2;
    char *res = (char *)result;

    while (i < num1 && j < num2) {
        if (comp(a + i * size, b + j * size) <= 0) {
            memcpy(res + k * size, a + i * size, size);
            i++;
        } 
        else {
            memcpy(res + k * size, b + j * size, size);
            j++;
        }
        k++;
    }
    while (i < num1) {
        memcpy(res + k * size, a + i * size, size);
        i++;
        k++;
    }
    while (j < num2) {
        memcpy(res + k * size, b + j * size, size);
        j++;
        k++;
    }

    ALGORITHM_LOG("[algorithm_merge] Success: Merging completed.");
}

/**
 * @brief Merges two sorted halves of an array in place.
 *
 * @param base Pointer to the start of the array.
 * @param middle Index of the middle element.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to order the elements.
 */
void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_inplace_merge] Info: Merging array segments with middle at %zu and total number of elements %zu.", middle, num);

    size_t i = 0, j = middle, k;
    char *arr = (char *)base;
    char temp[size];

    while (i < middle && j < num) {
        if (comp(arr + i * size, arr + j * size) <= 0) {
            i++;
        } 
        else {
            memcpy(temp, arr + j * size, size);
            for (k = j; k > i; k--) {
                memcpy(arr + k * size, arr + (k - 1) * size, size);
            }
            memcpy(arr + i * size, temp, size);

            i++;
            middle++;
            j++;
        }
    }

    ALGORITHM_LOG("[algorithm_inplace_merge] Success: In-place merge completed.");
}

/**
 * @brief Finds the first occurrence of two consecutive elements that are equal.
 *
 * This function searches through the array pointed to by `base` and finds the first pair of consecutive 
 * elements that are equal, as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * 
 * @return Pointer to the first element of the first pair of consecutive equal elements found.
 * If no such pair is found, returns `NULL`.
 */
void *algorithm_adjacent_find(const void *base, size_t num, size_t size, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_adjacent_find] Info: Searching for adjacent equal elements in an array of size %zu.", num);

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) == 0) {
            ALGORITHM_LOG("[algorithm_adjacent_find] Success: Found adjacent equal elements at index %zu.", i);
            return (void *)(ptr + i * size);
        }
    }

    ALGORITHM_LOG("[algorithm_adjacent_find] Info: No adjacent equal elements found.");
    return NULL;
}

/**
 * @brief Finds the first position where two ranges differ.
 *
 * This function compares two arrays element by element and returns the first position where the elements differ.
 * The comparison is done using the comparison function `comp`.
 *
 * @param base1 Pointer to the start of the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array in bytes.
 * @param base2 Pointer to the start of the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array in bytes.
 * @param comp Comparison function used to compare elements between the arrays.
 * 
 * @return A Pair structure:
 *      - `first`: Pointer to the first differing element in the first array.
 *      - `second`: Pointer to the first differing element in the second array.
 *      If the ranges are equal, both pointers in the Pair will be `NULL`.
 */
Pair algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_mismatch] Info: Searching for mismatch between two arrays of size %zu and %zu.", num1, num2);

    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;
    size_t min_num = num1 < num2 ? num1 : num2;

    for (size_t i = 0; i < min_num; i++) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            ALGORITHM_LOG("[algorithm_mismatch] Success: Mismatch found at index %zu.", i);
            Pair mismatch = {(void *)(ptr1 + i * size1), (void *)(ptr2 + i * size2)};
            
            return mismatch;
        }
    }

    ALGORITHM_LOG("[algorithm_mismatch] Info: No mismatch found. Arrays are identical within the compared range.");
    Pair mismatch = {NULL, NULL};

    return mismatch;
}

/**
 * @brief Checks if two arrays are permutations of each other.
 *
 * This function checks whether the elements of the two arrays `base1` and `base2` are permutations of each other. 
 * It does so by ensuring that each element in `base1` has a corresponding element in `base2` and vice versa, 
 * with all elements being matched exactly once.
 *
 * @param base1 Pointer to the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array in bytes.
 * @param base2 Pointer to the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array in bytes.
 * @param comp Comparison function used to compare elements between the arrays.
 * 
 * @return true if the arrays are permutations of each other; false otherwise.
 */
bool algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_is_permutation] Info: Checking if two arrays of size %zu and %zu are permutations of each other.", num1, num2);
    if (num1 != num2 || size1 != size2) {
        ALGORITHM_LOG("[algorithm_is_permutation] Info: Arrays have different sizes. They cannot be permutations.");
        return false;
    }

    bool found1[num1], found2[num2];
    memset(found1, 0, sizeof(found1));
    memset(found2, 0, sizeof(found2));

    for (size_t i = 0; i < num1; ++i) {
        bool matched = false;
        for (size_t j = 0; j < num2; ++j) {
            if (!found1[i] && !found2[j] && comp((char *)base1 + i * size1, (char *)base2 + j * size2) == 0) {
                found1[i] = true;
                found2[j] = true;
                matched = true;
                break;
            }
        }
        if (!matched) {
            ALGORITHM_LOG("[algorithm_is_permutation] Info: Element at index %zu in the first array does not have a match in the second array.", i);
            return false;
        }
    }

    ALGORITHM_LOG("[algorithm_is_permutation] Success: The arrays are permutations of each other.");
    return true;
}

/**
 * @brief Searches for the first occurrence of a sequence within another sequence.
 *
 * This function searches for the first occurrence of the sequence starting at `first2` and ending at `last2` 
 * within the sequence starting at `first1` and ending at `last1`.
 *
 * @param first1 Pointer to the start of the first sequence.
 * @param last1 Pointer to the end of the first sequence.
 * @param size1 Size of each element in the first sequence in bytes.
 * @param first2 Pointer to the start of the second sequence.
 * @param last2 Pointer to the end of the second sequence.
 * @param size2 Size of each element in the second sequence in bytes.
 * @param comp Comparison function used to compare elements between the sequences.
 * 
 * @return Pointer to the first element of the first occurrence of `first2` within `first1`. If not found, returns `last1`.
 */
const void* algorithm_search(const void* first1, const void* last1, size_t size1, const void* first2, const void* last2, size_t size2, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_search] Info: Starting search between two ranges.");
    
    const char* ptr1 = (const char*)first1;
    const char* ptr2 = (const char*)first2;
    const char* end1 = (const char*)last1;

    if (ptr2 == last2) {
        ALGORITHM_LOG("[algorithm_search] Info: Second range is empty, returning first.");
        return first1; 
    }

    while (ptr1 != end1) {
        const char* it1 = ptr1;
        const char* it2 = ptr2;

        while (comp(it1, it2)) { 
            it1 += size1;
            it2 += size2;

            if (it2 == last2) {
                ALGORITHM_LOG("[algorithm_search] Success: Found a match starting at element %zu.", (ptr1 - (const char*)first1) / size1);
                return ptr1; 
            }
            if (it1 == end1) {
                ALGORITHM_LOG("[algorithm_search] Info: Reached the end of the first sequence without a full match.");
                return last1; 
            }
        }
        ptr1 += size1;
    }

    ALGORITHM_LOG("[algorithm_search] Info: No match found, returning last.");
    return last1;
}

/**
 * @brief Searches for a sequence of identical elements within a range.
 *
 * This function searches for the first occurrence of a sequence of `count` identical elements 
 * (each element matching `val`) within the range `[first, last)`.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to the end of the range.
 * @param size Size of each element in bytes.
 * @param count Number of consecutive elements to search for.
 * @param val Pointer to the value that elements should match.
 * @param comp Comparison function used to compare elements with the value.
 * 
 * @return Pointer to the first element of the found sequence. If not found, returns `last`.
 */
const void *algorithm_search_n(const void *first, const void* last, size_t size, size_t count, const void *val, CompareFuncBool comp) {
    ALGORITHM_LOG("[algorithm_search_n] Info: Searching for %zu consecutive elements in a sequence.", count);

    const char* ptr = (const char*)first;
    const char* end = (const char*)last;

    while (ptr + size * count <= end) {
        size_t matched = 0;
        for (size_t i = 0; i < count; ++i) {
            if (comp(ptr + i * size, val)) {
                matched++;
            } 
            else {
                break;
            }
        }

        if (matched == count) {
            ALGORITHM_LOG("[algorithm_search_n] Success: Found %zu consecutive elements.", count);
            return ptr; 
        }
        ptr += size;
    }

    ALGORITHM_LOG("[algorithm_search_n] Info: Did not find %zu consecutive elements.", count);
    return last;
}

/**
 * @brief Removes elements that match a specified value from an array.
 *
 * This function removes all elements from the array `base` that match the value pointed to by `val`, 
 * as determined by the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to remove from the array.
 * @param comp Comparison function used to compare elements with the value.
 * @return Pointer to the new end of the array, after removing the specified elements.
 */
void *algorithm_remove(void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_remove] Info: Removing elements matching a value from %zu elements.", num);

    char *ptr = (char *)base;
    size_t new_num = 0;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) != 0) {
            if (i != new_num) {
                memcpy(ptr + new_num * size, ptr + i * size, size);
                ALGORITHM_LOG("[algorithm_remove] Info: Moved element from index %zu to index %zu.", i, new_num);
            }
            ++new_num;
        }
    }

    ALGORITHM_LOG("[algorithm_remove] Success: Removed elements, new size is %zu.", new_num);
    return ptr + new_num * size;
}

/**
 * @brief Copies elements from a source array to a destination array, excluding elements that match a given value.
 *
 * @param source Pointer to the source array.
 * @param num Number of elements in the source array.
 * @param size Size of each element in bytes.
 * @param result Pointer to the destination array.
 * @param val Pointer to the value to exclude.
 * @param comp Comparison function used to compare elements with the value.
 */
void algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_remove_copy] Info: Starting with %zu elements.", num);

    const char *src = (const char *)source;
    char *dst = (char *)result;
    size_t copied = 0;

    for (size_t i = 0; i < num; ++i) {
        if (comp(src + i * size, val) != 0) {
            memcpy(dst, src + i * size, size);
            dst += size;
            copied++;
            ALGORITHM_LOG("[algorithm_remove_copy] Info: Copied element at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_remove_copy] Success: Copied %zu elements.", copied);
}

/**
 * Removes elements from the source array based on a given predicate function and copies the remaining elements to the result array.
 *
 * @param source The source array.
 * @param num The number of elements in the source array.
 * @param size The size of each element in bytes.
 * @param result The result array where the remaining elements will be copied.
 * @param pred The predicate function that determines whether an element should be removed or not.
 * @return The number of elements copied to the result array.
 */
size_t algorithm_remove_copy_if(const void *source, size_t num, size_t size, void *result, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_remove_copy_if] Info: Starting with %zu elements.", num);

    if (!source || !result || !pred || size == 0 || num == 0) {
        ALGORITHM_LOG("[algorithm_remove_copy_if] Error: Invalid input parameters.");
        return 0;
    }

    const char *src = (const char *)source;
    char *dst = (char *)result;
    size_t count = 0;

    for (size_t i = 0; i < num; ++i) {
        if (!pred(src + i * size)) {
            memcpy(dst, src + i * size, size);
            dst += size;
            count++;
            ALGORITHM_LOG("[algorithm_remove_copy_if] Info: Copied element at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_remove_copy_if] Success: Copied %zu elements.", count);
    return count;
}

/**
 * Replaces occurrences of a value in an array.
 *
 * This function replaces occurrences of the value `old_val` with the value `new_val` in the array `base`.
 * The number of elements in the array is specified by `num`, and the size of each element is specified by `size`.
 * The comparison function `comp` is used to compare elements in the array.
 *
 * @param base     Pointer to the start of the array.
 * @param num      Number of elements in the array.
 * @param size     Size of each element in bytes.
 * @param old_val  Pointer to the value to be replaced.
 * @param new_val  Pointer to the new value.
 * @param comp     Comparison function used to compare elements.
 */
void algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, CompareFunc comp) {
    ALGORITHM_LOG("[algorithm_replace] Info: Starting with %zu elements.", num);

    if (!base || !old_val || !new_val || !comp || size == 0 || num == 0) {
        ALGORITHM_LOG("[algorithm_replace] Error: Invalid input parameters.");
        return;
    }

    char *ptr = (char *)base;
    size_t replaced = 0;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, old_val) == 0) {
            memcpy(ptr + i * size, new_val, size);
            replaced++;
            ALGORITHM_LOG("[algorithm_replace] Info: Replaced element at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_replace] Success: Replaced %zu elements.", replaced);
}

/**
 * Replaces elements in an array based on a given predicate function.
 *
 * This function iterates over the elements in the array pointed to by `base` and replaces
 * elements that satisfy the predicate function `pred` with the value pointed to by `new_val`.
 *
 * @param base      Pointer to the start of the array.
 * @param num       Number of elements in the array.
 * @param size      Size of each element in bytes.
 * @param new_val   Pointer to the value to replace the elements with.
 * @param pred      Predicate function that determines if an element should be replaced.
 *
 * @return None.
 */
void algorithm_replace_if(void *base, size_t num, size_t size, const void *new_val, BoolPredicateFunc pred) {
    ALGORITHM_LOG("[algorithm_replace_if] Info: Starting replace_if with %zu elements.", num);

    if (!base) {
        ALGORITHM_LOG("[algorithm_replace_if] Error: base pointer is NULL.");
        return;
    }
    if (!new_val) {
        ALGORITHM_LOG("[algorithm_replace_if] Error: new_val pointer is NULL.");
        return;
    }
    if (!pred) {
        ALGORITHM_LOG("[algorithm_replace_if] Error: Predicate function is NULL.");
        return;
    }
    if (size == 0 || num == 0) {
        ALGORITHM_LOG("[algorithm_replace_if] Error: size or num is zero.");
        return;
    }

    char *ptr = (char *)base;
    size_t replace_count = 0;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            memcpy(ptr + i * size, new_val, size);
            replace_count++;
            ALGORITHM_LOG("[algorithm_replace_if] Info: Replaced element at index %zu.", i);
        }
    }

    ALGORITHM_LOG("[algorithm_replace_if] Success: Replaced %zu elements.", replace_count);
}

/**
 * @brief Returns a pointer to the first element of an array.
 *
 * @param base Pointer to the start of the array.
 * @return Pointer to the first element of the array.
 */
void* algorithm_begin(void* base) {
    ALGORITHM_LOG("[algorithm_begin] Info: Returning pointer to the first element.");
    return base;
}

/**
 * @brief Returns a pointer to one past the last element of an array.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @return Pointer to one past the last element of the array.
 */
void* algorithm_end(void* base, size_t num, size_t size) {
    ALGORITHM_LOG("[algorithm_end] Info: Returning pointer to one past the last element.");
    return (char*)base + num * size;
}

/**
 * @brief Fills a range with successive values, starting from the given value.
 *
 * This function assigns successive values to the range `[first, last)`, starting from the value `val`
 * and incrementing it for each element. It works for integers, characters, floating-point, and several other types.
 *
 * @param first Pointer to the start of the range.
 * @param last Pointer to one past the end of the range.
 * @param val Pointer to the initial value to assign to the first element.
 * @param size Size of each element in bytes.
 * @param type The type of the data being processed (specified as DataType).
 */
void algorithm_iota(void* first, void* last, void* val, size_t size, DataType type) {
    char* ptr = (char*)first;

    switch (type) {
        case TYPE_INT: {
            ALGORITHM_LOG("Handling int\n");
            int current = *(int*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_CHAR: {
            ALGORITHM_LOG("Handling char\n");
            char current = *(char*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_FLOAT: {
            ALGORITHM_LOG("Handling float\n");
            float current = *(float*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1.0f;
                ptr += size;
            }
            break;
        }
        case TYPE_DOUBLE: {
            ALGORITHM_LOG("Handling double\n");
            double current = *(double*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1.0;
                ptr += size;
            }
            break;
        }
        case TYPE_SHORT: {
            ALGORITHM_LOG("Handling short\n");
            short current = *(short*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_LONG: {
            ALGORITHM_LOG("Handling long\n");
            long current = *(long*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_LONG_LONG: {
            ALGORITHM_LOG("Handling long long\n");
            long long current = *(long long*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_UNSIGNED_LONG: {
            ALGORITHM_LOG("Handling unsigned long\n");
            unsigned long current = *(unsigned long*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_UNSIGNED_CHAR: {
            ALGORITHM_LOG("Handling unsigned char\n");
            unsigned char current = *(unsigned char*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_UNSIGNED_LONG_LONG: {
            ALGORITHM_LOG("Handling unsigned long long\n");
            unsigned long long current = *(unsigned long long*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_LONG_DOUBLE: {
            ALGORITHM_LOG("Handling long double\n");
            long double current = *(long double*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1.0;
                ptr += size;
            }
            break;
        }
        case TYPE_UNSIGNED_INT: {
            ALGORITHM_LOG("Handling unsignedint\n");
            unsigned int current = *(unsigned int*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        case TYPE_UNSIGNED_SHORT: {
            ALGORITHM_LOG("Handling unsigned short\n");
            unsigned short current = *(unsigned short*)val;
            while (ptr != (char*)last) {
                memcpy(ptr, &current, size);
                current += 1;
                ptr += size;
            }
            break;
        }
        default: {
            ALGORITHM_LOG("Unsupported type.\n");
            break;
        }
    }
}