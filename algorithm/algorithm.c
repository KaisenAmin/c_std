#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "algorithm.h"

/**
 * Swaps the values of two memory locations.
 *
 * This function swaps the values of two memory locations, given their addresses and the size of the elements.
 * It uses a stack-allocated buffer for small elements to perform the swap efficiently.
 *
 * @param a     Pointer to the first memory location.
 * @param b     Pointer to the second memory location.
 * @param size  Size of the elements in bytes.
 */
static void swap(void *a, void *b, size_t size) {
    unsigned char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

/**
 * Reverses the order of elements in a range.
 *
 * This function takes a range defined by the pointers `first` and `last`,
 * and reverses the order of the elements within that range. The `size`
 * parameter specifies the size of each element in the range.
 *
 * @param first Pointer to the first element in the range.
 * @param last Pointer to the last element in the range.
 * @param size Size of each element in the range.
 */
static void reverse(void *first, void *last, size_t size) {
    char *a = (char *)first;
    char *b = (char *)last - size;

    while (a < b) {
        swap(a, b, size);
        a += size;
        b -= size;
    }
}

/**
 * Sorts the elements in the specified range of the array using the QuickSort algorithm.
 *
 * @param base The pointer to the start of the array.
 * @param low The index of the first element in the range to be sorted.
 * @param high The index of the last element in the range to be sorted.
 * @param size The size of each element in the array.
 * @param comp The comparison function used to determine the order of the elements.
 * @param temp The temporary buffer used for swapping elements during sorting.
 */
static void quickSortInternal(void *base, size_t low, size_t high, size_t size, CompareFunc comp, void *temp) {
    if (low < high) {
        char* pivot = (char*)base + high * size;
        size_t i = low;

        for (size_t j = low; j < high; j++) {
            if (comp((char*)base + j * size, pivot) < 0) {
                memcpy(temp, (char*)base + j * size, size);
                memcpy((char*)base + j * size, (char*)base + i * size, size);
                memcpy((char*)base + i * size, temp, size);
                i++;
            }
        }

        memcpy(temp, (char*)base + i * size, size);
        memcpy((char*)base + i * size, pivot, size);
        memcpy(pivot, temp, size);

        if (i > 0) 
            quickSortInternal(base, low, i - 1, size, comp, temp);
        quickSortInternal(base, i + 1, high, size, comp, temp);
    }
}

/**
 * Merges two sorted subarrays within the given array.
 *
 * @param base The base address of the array.
 * @param low The starting index of the first subarray.
 * @param mid The ending index of the first subarray and the starting index of the second subarray.
 * @param high The ending index of the second subarray.
 * @param size The size of each element in the array.
 * @param comp The comparison function used to compare elements.
 * @param temp The temporary array used for merging.
 */
static void merge(void *base, size_t low, size_t mid, size_t high, size_t size, CompareFunc comp, void *temp) {
    size_t i = low, j = mid, k = 0;

    while (i < mid && j < high) {
        if (comp((char*)base + i * size, (char*)base + j * size) <= 0) {
            memcpy((char*)temp + k * size, (char*)base + i * size, size);
            i++;
        } else {
            memcpy((char*)temp + k * size, (char*)base + j * size, size);
            j++;
        }
        k++;
    }

    while (i < mid) {
        memcpy((char*)temp + k * size, (char*)base + i * size, size);
        i++; k++;
    }

    while (j < high) {
        memcpy((char*)temp + k * size, (char*)base + j * size, size);
        j++; k++;
    }

    memcpy((char*)base + low * size, temp, k * size);
}

/**
 * @brief Performs an internal merge sort on a given array.
 *
 * This function recursively divides the array into smaller subarrays and then merges them back together in sorted order.
 *
 * @param base The base address of the array.
 * @param low The lower index of the subarray to be sorted.
 * @param high The higher index of the subarray to be sorted.
 * @param size The size of each element in the array.
 * @param comp The comparison function used to determine the order of the elements.
 * @param temp A temporary array used for merging the subarrays.
 */
static void mergeSortInternal(void *base, size_t low, size_t high, size_t size, CompareFunc comp, void *temp) {
    if (high - low > 1) {
        size_t mid = low + (high - low) / 2;
        mergeSortInternal(base, low, mid, size, comp, temp);
        mergeSortInternal(base, mid, high, size, comp, temp);
        merge(base, low, mid, high, size, comp, temp);
    }
}

/**
 * Sorts the elements in the specified array in a stable manner using the merge sort algorithm.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param comp Pointer to the comparison function that determines the order of the elements.
 *             The function should return a negative value if the first element is less than the second element,
 *             a positive value if the first element is greater than the second element,
 *             and zero if the elements are equal.
 */
void algorithm_stable_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num > 1) {
        void* temp = malloc(num * size);
        if (temp) {
            mergeSortInternal(base, 0, num, size, comp, temp);
            free(temp);
        } else {
            perror("Failed to allocate memory for stable sorting");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Sorts an array using the quicksort algorithm.
 *
 * @param base Pointer to the array to be sorted.
 * @param num Number of elements in the array.
 * @param size Size of each element in the array.
 * @param comp Pointer to the comparison function used to determine the order of the elements.
 *             The function should return a negative value if the first element is less than the second,
 *             a positive value if the first element is greater than the second, and zero if they are equal.
 * @return void
 */
void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num > 1) {
        void* temp = malloc(size);
        if (temp) {
            quickSortInternal(base, 0, num - 1, size, comp, temp);
            free(temp);
        } 
        else {
            perror("Failed to allocate memory for sorting");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Finds the first occurrence of a value in an array using a specified comparison function.
 *
 * This function searches for the first occurrence of a value in the given array. It compares each element
 * of the array with the specified value using the provided comparison function. The comparison function
 * should return 0 if the two values are considered equal.
 *
 * @param base The base address of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param val The value to search for.
 * @param comp The comparison function to use for comparing elements.
 * @return A pointer to the first occurrence of the value in the array, or NULL if the value is not found.
 */
void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    const char *ptr = (const char*)base;

    for (size_t i = 0; i < num; i++) {
        if (comp(ptr + i * size, val) == 0) {
            return (void *)(ptr + i * size);
        }
    }

    return NULL;
}

/**
 * Finds the first element in the given array that satisfies the specified predicate function.
 *
 * @param base The pointer to the start of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param pred The predicate function that determines if an element satisfies the condition.
 * @return A pointer to the first element that satisfies the predicate, or NULL if no such element is found.
 */
void *algorithm_find_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char*)base;

    for (size_t i = 0; i < num; i++) {
        if (pred(ptr + i * size)) {
            return (void *)(ptr + i * size);
        }
    }

    return NULL;
}

/**
 * Finds the first element in the range [base, base + num * size) that does not satisfy the predicate function.
 *
 * @param base The pointer to the start of the range.
 * @param num The number of elements in the range.
 * @param size The size of each element in the range.
 * @param pred The predicate function that takes a pointer to an element and returns a boolean value.
 * @return A pointer to the first element that does not satisfy the predicate function, or NULL if all elements satisfy the predicate.
 */
void *algorithm_find_if_not(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char*)base;

    for (size_t i = 0; i < num; i++) {
        if (!pred(ptr + i * size)) {
            return (void *)(ptr + i * size);
        }
    }

    return NULL;
}

/**
 * Finds the last occurrence of a subsequence within a given sequence.
 *
 * This function searches for the last occurrence of the subsequence specified by `base2` within the sequence specified by `base1`.
 * The search is performed by comparing elements using the provided comparison function `comp`.
 *
 * @param base1   Pointer to the start of the sequence to search within.
 * @param num1    Number of elements in the sequence `base1`.
 * @param size1   Size of each element in the sequence `base1`.
 * @param base2   Pointer to the start of the subsequence to search for.
 * @param num2    Number of elements in the subsequence `base2`.
 * @param size2   Size of each element in the subsequence `base2`.
 * @param comp    Comparison function used to compare elements.
 *
 * @return        Pointer to the last occurrence of the subsequence within the sequence, or NULL if not found.
 */
void *algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    if (num2 == 0) {
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
        }
    }
    return ret;
}

/**
 * Finds the first occurrence of any element from `base2` in `base1` using the specified comparison function.
 *
 * @param base1 The pointer to the first element of the array to search.
 * @param num1 The number of elements in `base1`.
 * @param size1 The size of each element in `base1`.
 * @param base2 The pointer to the first element of the array containing the elements to search for.
 * @param num2 The number of elements in `base2`.
 * @param size2 The size of each element in `base2`.
 * @param comp The comparison function to use for comparing elements.
 * @return A pointer to the first occurrence of an element from `base2` in `base1`, or `NULL` if no match is found.
 */
void *algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    for (size_t i = 0; i < num1; ++i) {
        for (size_t j = 0; j < num2; ++j) {
            if (comp(ptr1 + i * size1, ptr2 + j * size2) == 0) {
                return (void *)(ptr1 + i * size1);
            }
        }
    }

    return NULL;
}

/**
 * Performs binary search on a sorted array.
 *
 * This function searches for a specific value in a sorted array using the binary search algorithm.
 * It returns the index of the found element if it exists, or -1 if the element is not found.
 *
 * @param base The base address of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param val The value to search for.
 * @param comp A function pointer to the comparison function used to compare elements.
 *             The comparison function should return a negative value if the first element is less than the second,
 *             a positive value if the first element is greater than the second, and 0 if the elements are equal.
 *
 * @return The index of the found element, or -1 if the element is not found.
 */
size_t algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
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
            return (size_t)mid; // Element found, return index
        }
    }
    return (size_t)-1; // Element not found
}

/**
 * Finds the maximum element in an array.
 *
 * This function iterates through the elements of the array pointed to by `base` and returns a pointer to the maximum element.
 * The `num` parameter specifies the number of elements in the array.
 * The `size` parameter specifies the size of each element in bytes.
 * The `comp` parameter is a function pointer to a comparison function that takes two elements as arguments and returns an integer indicating their order.
 * The comparison function should return a negative value if the first element is less than the second, a positive value if the first element is greater than the second, and zero if the elements are equal.
 *
 * @param base The pointer to the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in bytes.
 * @param comp The comparison function.
 * @return A pointer to the maximum element, or NULL if the array is empty.
 */
void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        return NULL;
    }

    const char *max_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(max_element, element) < 0) {
            max_element = element;
        }
    }
    return (void *)max_element;
}

/**
 * Finds the minimum element in an array.
 *
 * This function iterates over the elements in the array pointed to by `base` and returns a pointer to the minimum element.
 * The `num` parameter specifies the number of elements in the array.
 * The `size` parameter specifies the size of each element in bytes.
 * The `comp` parameter is a function pointer to a comparison function that takes two elements as arguments and returns an integer value indicating their order.
 * The comparison function should return a negative value if the first element is less than the second element, a positive value if the first element is greater than the second element, and zero if the elements are equal.
 *
 * @param base The pointer to the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in bytes.
 * @param comp The comparison function.
 * @return A pointer to the minimum element in the array, or NULL if the array is empty.
 */
void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) { 
        return NULL;
    }

    const char *min_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(min_element, element) > 0) {
            min_element = element;
        }
    }
    return (void *)min_element;
}

/**
 * Applies the given operation function to each element in the array.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param op Function pointer to the operation function.
 */
void algorithm_for_each(void *base, size_t num, size_t size, ForEachOpFunc op) {
    char *ptr = (char *)base;
    for (size_t i = 0; i < num; ++i) {
        op(ptr + i * size);
    }
}

/**
 * Copies a block of memory from the source to the destination.
 *
 * This function copies `num` elements of size `size` from the memory block pointed to by `source`
 * to the memory block pointed to by `dest`. The memory areas must not overlap.
 *
 * @param source Pointer to the source memory block.
 * @param num    Number of elements to be copied.
 * @param size   Size of each element in bytes.
 * @param dest   Pointer to the destination memory block.
 */
void algorithm_copy(const void *source, size_t num, size_t size, void *dest) {
    const char *src_ptr = (const char *)source;
    char *dest_ptr = (char *)dest;

    for (size_t i = 0; i < num; ++i) {
        memcpy(dest_ptr + i * size, src_ptr + i * size, size);
    }
}

/**
 * Applies the specified accumulation operation to a range of elements in an array.
 *
 * This function iterates over the elements in the range [base, base + num * size) and applies the accumulation operation
 * specified by the function pointer `op` to each element. The initial value for the accumulation is provided by the `init`
 * parameter. The result of the accumulation is stored in the memory pointed to by `init`.
 *
 * @param base Pointer to the start of the range of elements.
 * @param num Number of elements in the range.
 * @param size Size of each element in bytes.
 * @param init Pointer to the initial value for the accumulation.
 * @param op Function pointer to the accumulation operation.
 * @return Pointer to the memory where the result of the accumulation is stored.
 */
void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, AccumulateOpFunc op) {
    char *result = (char *)init;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        op(result, ptr + i * size);
    }
    return result;
}

/**
 * Checks if a predicate function returns true for all elements in an array.
 *
 * This function iterates over the elements in the array pointed to by `base` and applies the predicate function `pred` to each element.
 * If the predicate function returns false for any element, the function immediately returns false.
 * If the predicate function returns true for all elements, the function returns true.
 *
 * @param base The pointer to the start of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param pred The predicate function that takes a pointer to an element and returns a boolean value.
 * @return true if the predicate function returns true for all elements, false otherwise.
 */
bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (!pred(ptr + i * size)) {
            return false;
        }
    }

    return true;
}

/**
 * Checks if any element in the given array satisfies the specified predicate function.
 *
 * @param base The base address of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param pred The predicate function that determines if an element satisfies a condition.
 *             The function should return true if the element satisfies the condition, false otherwise.
 * @return true if any element satisfies the predicate function, false otherwise.
 */
bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            return true;
        }
    }

    return false;
}

/**
 * Checks if none of the elements in the given array satisfy the given predicate function.
 *
 * @param base The base address of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param pred The predicate function that takes an element as input and returns a boolean value.
 * @return Returns true if none of the elements satisfy the predicate, false otherwise.
 */
bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            return false;
        }
    }

    return true;
}

/**
 * Fills a range of elements with a specified value.
 *
 * This function copies the value pointed to by `val` to every element in the range
 * `[first, last)`, where `first` and `last` are pointers to the beginning and end
 * of the range, respectively. The size of each element is specified by `size`.
 *
 * @param first Pointer to the beginning of the range.
 * @param last Pointer to the end of the range (exclusive).
 * @param size Size of each element in the range.
 * @param val Pointer to the value to be copied.
 */
void algorithm_fill(void *first, void *last, size_t size, const void *val) {
    for (char *ptr = first; ptr != last; ptr += size) {
        memcpy(ptr, val, size);
    }
}

/**
 * Fills a range of elements with a specified value.
 *
 * This function copies the value pointed to by `val` to each element in the range
 * defined by the `first` and `n` parameters. The size of each element is specified
 * by the `size` parameter.
 *
 * @param first Pointer to the first element in the range.
 * @param n     Number of elements to fill.
 * @param size  Size of each element in bytes.
 * @param val   Pointer to the value to be copied.
 */
void algorithm_fill_n(void *first, size_t n, size_t size, const void *val) {
    for (char *ptr = first; n > 0; ptr += size, n--) {
        memcpy(ptr, val, size);
    }
}

/**
 * Counts the number of occurrences of a value in an array.
 *
 * This function iterates over the elements of the array pointed to by `base` and compares each element
 * with the value pointed to by `val` using the comparison function `comp`. It returns the count of
 * elements that match the value.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in the array.
 * @param val Pointer to the value to be counted.
 * @param comp Comparison function used to compare elements.
 *
 * @return The number of occurrences of the value in the array.
 */
size_t algorithm_count(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    size_t count = 0;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) == 0) {
            ++count;
        }
    }

    return count;
}

/**
 * Counts the number of elements in an array that satisfy a given predicate function.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in the array.
 * @param pred Predicate function that takes a pointer to an element and returns a boolean value.
 *             The function should return true if the element satisfies the condition, and false otherwise.
 * @return The number of elements that satisfy the predicate function.
 */
size_t algorithm_count_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    size_t count = 0;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ++count;
        }
    }

    return count;
}

/**
 * Shuffles the elements in an array.
 *
 * This function shuffles the elements in the specified array using the Fisher-Yates algorithm.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param rng Function pointer to a uniform random number generator.
 */
void algorithm_shuffle(void *base, size_t num, size_t size, UniformRandomNumberGenerator rng) {
    if (num > 1) {
        char *arr = (char *)base;
        char *temp = malloc(size);
        if (!temp) {
            perror("Failed to allocate memory for shuffle");
            exit(EXIT_FAILURE);
        }

        for (size_t i = num - 1; i > 0; i--) {
            size_t j = rng() % (i + 1);

            // Swap arr[i] and arr[j]
            memcpy(temp, arr + i * size, size);
            memcpy(arr + i * size, arr + j * size, size);
            memcpy(arr + j * size, temp, size);
        }

        free(temp);
    }
}

/**
 * Finds the lower bound of a value in a sorted array.
 *
 * This function performs a binary search on the sorted array to find the first element that is not less than the given value.
 * The array is assumed to be sorted in ascending order according to the comparison function `comp`.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in the array.
 * @param val Pointer to the value to search for.
 * @param comp Comparison function used to compare elements.
 * @return Pointer to the lower bound of the value in the array, or a pointer to the end of the array if the value is not found.
 */
void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
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
    return (void *)((const char *)base + low * size);
}

/**
 * Finds the upper bound of a value in a sorted array.
 *
 * This function performs a binary search to find the first element in the array
 * that is greater than the given value. It assumes that the array is sorted in
 * ascending order according to the comparison function `comp`.
 *
 * @param base The base address of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param val The value to search for.
 * @param comp The comparison function used to compare elements.
 * @return A pointer to the first element greater than `val`, or a pointer to the
 *         end of the array if no such element is found.
 */
void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
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
    return (void *)((const char *)base + low * size);
}

/**
 * Applies a transformation function to each element in an array.
 *
 * This function applies the transformation function `op` to each element in the input array `base`.
 * The transformed elements are stored in the output array `result`.
 *
 * @param base    Pointer to the input array.
 * @param num     Number of elements in the array.
 * @param size    Size of each element in bytes.
 * @param result  Pointer to the output array.
 * @param op      Pointer to the transformation function.
 */
void algorithm_transform(const void *base, size_t num, size_t size, void *result, TransformFunc op) {
    const char *input_ptr = (const char *)base;
    char *output_ptr = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        op(output_ptr + i * size, input_ptr + i * size);
    }
}

/**
 * Reduces an array of elements to a single value using a specified reduction function.
 *
 * This function applies the reduction function `op` to each element in the array `base`,
 * starting with the initial value `init`. The `op` function takes two arguments: the current
 * result value and the next element in the array. The result of each reduction is stored in
 * the `result` variable.
 *
 * @param base  Pointer to the array of elements.
 * @param num   Number of elements in the array.
 * @param size  Size of each element in bytes.
 * @param init  Pointer to the initial value.
 * @param op    Pointer to the reduction function.
 * @return      Pointer to the final reduced value.
 */
void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, ReduceFunc op) {
    const char *ptr = (const char *)base;
    char *result = (char *)init;

    for (size_t i = 0; i < num; ++i) {
        op(result, ptr + i * size);
    }
    return result;
}

/**
 * Removes duplicate elements from an array.
 *
 * This function takes an array and removes any duplicate elements, keeping only the unique elements.
 * The uniqueness of elements is determined by the provided comparison function.
 *
 * @param base The pointer to the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in bytes.
 * @param comp The comparison function used to determine the uniqueness of elements.
 *             The function should return 0 if the elements are considered equal, and a non-zero value otherwise.
 *
 * @return The number of unique elements in the array.
 */
size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        return 0;
    }

    size_t uniqueCount = 1; // First element is always unique
    char *array = (char *)base;

    for (size_t i = 1; i < num; ++i) {
        // Compare current element with the last unique element
        if (comp(array + (uniqueCount - 1) * size, array + i * size) != 0) {
            // If different, move it to the next unique position
            if (uniqueCount != i) {
                memcpy(array + uniqueCount * size, array + i * size, size);
            }
            uniqueCount++;
        }
    }
    return uniqueCount;
}

/**
 * @brief Checks if two ranges of elements are equal.
 *
 * This function compares two ranges of elements specified by `base1` and `base2`.
 * The number of elements in each range is specified by `num1` and `num2`, respectively.
 * The size of each element in bytes is specified by `size1` and `size2`, respectively.
 * The comparison is performed using the provided `comp` function, which should return 0 if two elements are considered equal.
 *
 * @param base1 Pointer to the first range of elements.
 * @param num1 Number of elements in the first range.
 * @param size1 Size of each element in the first range.
 * @param base2 Pointer to the second range of elements.
 * @param num2 Number of elements in the second range.
 * @param size2 Size of each element in the second range.
 * @param comp Function used to compare two elements.
 * @return true if the ranges are equal, false otherwise.
 */
bool algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    // If the number of elements or sizes differ, the ranges are not equal
    if (num1 != num2 || size1 != size2) {
        return false;
    }

    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;

    for (size_t i = 0; i < num1; ++i) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            return false;
        }
    }

    return true;
}

/**
 * Generates the next lexicographically greater permutation of a range of elements.
 *
 * This function rearranges the elements in the range [first, last) into the next lexicographically greater permutation.
 * The function returns true if such permutation exists, and false otherwise.
 *
 * @param first     Pointer to the first element in the range.
 * @param last      Pointer to the past-the-end element in the range.
 * @param size      Size of each element in the range.
 * @param comp      Pointer to the comparison function that determines the order of the elements.
 *                  The function should return true if the first argument is less than the second argument, and false otherwise.
 *
 * @return          true if the next permutation exists, false otherwise.
 */
bool algorithm_next_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    if (first == last) {
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
            swap(i, k, size);
            reverse(j, last, size);

            return true;
        }

        if (i == (char *)first) {
            reverse(first, last, size);
            return false;
        }
    }
    return false; // This should not be reached
}


/**
 * Reverses the order of the elements in the range [first, last) using the previous permutation algorithm.
 * 
 * This function takes a range defined by the pointers `first` and `last`, and reverses the order of the elements
 * in that range using the previous permutation algorithm. The elements are compared using the provided comparison
 * function `comp`.
 * 
 * The function returns `true` if a previous permutation was found and applied successfully, and `false` otherwise.
 * 
 * @param first     Pointer to the first element in the range.
 * @param last      Pointer to the element past the last element in the range.
 * @param size      Size of each element in the range.
 * @param comp      Comparison function used to compare the elements.
 * @return          `true` if a previous permutation was found and applied successfully, `false` otherwise.
 */
bool algorithm_prev_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    if (first == last) {
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
            swap(i, k, size);
            reverse(j, last, size);
            return true;
        }

        if (i == (char *)first) {
            reverse(first, last, size);
            return false;
        }
    }
    return false;
}

/**
 * Partitions the elements in the given array based on the provided predicate function.
 *
 * This function rearranges the elements in the array such that all elements for which the predicate
 * function returns true come before all elements for which the predicate function returns false.
 *
 * @param base The pointer to the start of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param pred The predicate function used to determine the partitioning.
 *             The function should take a pointer to an element as input and return a boolean value.
 *             It should return true if the element satisfies the partitioning condition, and false otherwise.
 * @return A pointer to the first element after the partition.
 */
void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    char *first = (char *)base;
    char *last = first + num * size;

    while (first != last) {
        while (first != last && pred(first)) {
            first += size;
        }
        do {
            last -= size;
            if (first == last) 
                break;
        } while (!pred(last));

        if (first != last) {
            swap(first, last, size);
            first += size;
        }
    }
    return first;
}

/**
 * Generates values using a generator function and stores them in a range of memory.
 *
 * This function iterates over the memory range defined by the pointers `first` and `last`,
 * generating values using the provided `gen` function and storing them in consecutive memory locations.
 * The `size` parameter specifies the size of each element in the range.
 *
 * @param first Pointer to the beginning of the range.
 * @param last Pointer to the end of the range (one past the last element).
 * @param size Size of each element in the range.
 * @param gen Generator function that generates values to be stored in the range.
 */
void algorithm_generate(void *first, void *last, size_t size, GeneratorFunc gen) {
    char *current = (char *)first;
    while (current != last) {
        gen(current);
        current += size;
    }
}

/**
 * Generates n elements using a generator function and stores them in the specified memory location.
 *
 * @param first The pointer to the first element in the memory location.
 * @param n The number of elements to generate.
 * @param size The size of each element in bytes.
 * @param gen The generator function that generates each element.
 */
void algorithm_generate_n(void *first, size_t n, size_t size, GeneratorFunc gen) {
    char *current = (char *)first;
    for (size_t i = 0; i < n; ++i) {
        gen(current);
        current += size;
    }
}

/**
 * Copies a range of elements in reverse order.
 *
 * This function copies the elements in the range [first, last) to the range [result - (last - first), result) in reverse order.
 * The size parameter specifies the size of each element in bytes.
 *
 * @param first   Pointer to the first element of the range to be copied.
 * @param last    Pointer to the element past the last element of the range to be copied.
 * @param size    Size of each element in bytes.
 * @param result  Pointer to the first element of the destination range.
 */
void algorithm_copy_backward(const void *first, const void *last, size_t size, void *result) {
    const char *src = (const char *)last;
    char *dest = (char *)result;

    while (src != (const char *)first) {
        src -= size;  // Move source pointer backwards
        dest -= size; // Move destination pointer backwards
        memcpy(dest, src, size); // Copy element
    }
}

/**
 * Copies elements from the range [first, last) to the range starting at result,
 * but only if the element satisfies the given predicate function.
 *
 * @param first     Pointer to the beginning of the input range.
 * @param last      Pointer to the end of the input range.
 * @param size      Size of each element in bytes.
 * @param result    Pointer to the beginning of the output range.
 * @param pred      Unary predicate function that determines whether an element should be copied.
 */
void algorithm_copy_if(const void *first, const void *last, size_t size, void *result, UnaryPredicateFunc pred) {
    const char *src = (const char *)first;
    char *dest = (char *)result;

    while (src != (const char *)last) {
        if (pred(src)) {
            memcpy(dest, src, size);
            dest += size;
        }
        src += size;
    }
}

/**
 * Copies a specified number of elements from the source array to the destination array.
 *
 * @param first   Pointer to the first element of the source array.
 * @param n       Number of elements to be copied.
 * @param size    Size of each element in bytes.
 * @param result  Pointer to the first element of the destination array.
 */
void algorithm_copy_n(const void *first, size_t n, size_t size, void *result) {
    const char *src = (const char *)first;
    char *dest = (char *)result;

    for (size_t i = 0; i < n; ++i) {
        memcpy(dest, src, size);
        src += size;
        dest += size;
    }
}

/**
 * Finds the range of elements in a sorted array that are equal to a given value.
 *
 * This function searches for the lower and upper bounds of the range of elements in the sorted array
 * `base` that are equal to the value pointed to by `val`. The search is performed using the comparison
 * function `comp`.
 *
 * @param base  Pointer to the first element of the array.
 * @param num   Number of elements in the array.
 * @param size  Size of each element in the array.
 * @param val   Pointer to the value to search for.
 * @param comp  Pointer to the comparison function.
 *
 * @return      A `Pair` structure containing the lower and upper bounds of the range.
 */
Pair algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    Pair range;
    range.first = algorithm_lower_bound(base, num, size, val, comp);
    range.second = algorithm_upper_bound(range.first, num, size, val, comp);
    return range;
}

/**
 * Checks if a range of elements in the first array includes all the elements in the second array.
 *
 * This function compares elements in the first array with elements in the second array using the provided comparison function.
 * The comparison function should return a negative value if the first element is less than the second element, a positive value if the first element is greater than the second element, and zero if the elements are equal.
 *
 * @param first1 Pointer to the first element of the first array.
 * @param num1 Number of elements in the first array.
 * @param size1 Size of each element in the first array.
 * @param first2 Pointer to the first element of the second array.
 * @param num2 Number of elements in the second array.
 * @param size2 Size of each element in the second array.
 * @param comp Comparison function used to compare elements.
 * @return true if the range of elements in the first array includes all the elements in the second array, false otherwise.
 */
bool algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, 
                        CompareFunc comp) {
    const char *ptr1 = (const char *)first1;
    const char *ptr2 = (const char *)first2;
    const char *last1 = ptr1 + num1 * size1;
    const char *last2 = ptr2 + num2 * size2;

    while (ptr2 != last2) {
        if ((ptr1 == last1) || comp(ptr2, ptr1) < 0) {
            return false;
        }
        if (!comp(ptr1, ptr2)) {
            ptr2 += size2;
        }
        ptr1 += size1;
    }
    return true;
}

/**
 * Copies unique elements from the input array to the output array based on a comparison function.
 *
 * @param first     Pointer to the first element of the input array.
 * @param num       Number of elements in the input array.
 * @param size      Size of each element in bytes.
 * @param result    Pointer to the first element of the output array.
 * @param comp      Comparison function used to determine uniqueness.
 * @return          The number of unique elements copied to the output array.
 */
size_t algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, CompareFunc comp) {
    if (num == 0) {
        return 0;
    }

    const char *src = (const char *)first;
    char *dst = (char *)result;
    memcpy(dst, src, size); // Copy first element
    size_t count = 1;

    for (size_t i = 1; i < num; ++i) {
        if (comp(dst + (count - 1) * size, src + i * size) != 0) {
            memcpy(dst + count * size, src + i * size, size);
            ++count;
        }
    }
    return count;
}

/**
 * Swaps the values of two memory locations.
 *
 * This function swaps the values of two memory locations specified by the pointers `a` and `b`.
 * The `size` parameter specifies the size of the memory block to be swapped.
 *
 * @param a     Pointer to the first memory location.
 * @param b     Pointer to the second memory location.
 * @param size  Size of the memory block to be swapped.
 */
void algorithm_swap(void *a, void *b, size_t size) {
    void *temp = malloc(size);
    if (!temp) {
        perror("Failed to allocate memory for swap");
        exit(EXIT_FAILURE);
    }

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);

    free(temp);
}

/**
 * Swaps ranges of elements between two memory locations.
 *
 * This function swaps ranges of elements between two memory locations.
 * It takes two pointers to the first elements of the ranges, the number
 * of elements to swap, and the size of each element. The function swaps
 * each element in the ranges by calling the `algorithm_swap` function.
 *
 * @param first1 Pointer to the first element of the first range.
 * @param first2 Pointer to the first element of the second range.
 * @param num Number of elements to swap.
 * @param size Size of each element in bytes.
 */
void algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size) {
    char *ptr1 = (char *)first1;
    char *ptr2 = (char *)first2;

    for (size_t i = 0; i < num; ++i) {
        algorithm_swap(ptr1 + i * size, ptr2 + i * size, size);
    }
}

/**
 * Checks if the elements in a range are sorted in ascending order.
 *
 * This function compares adjacent elements in the range using the provided comparison function.
 * The range is considered sorted if each element is less than or equal to the next element.
 *
 * @param base Pointer to the start of the range.
 * @param num Number of elements in the range.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to compare elements.
 * @return true if the range is sorted, false otherwise.
 */
bool algorithm_is_sorted(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num < 2) {
        return true; // A range with less than two elements is always sorted.
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            return false; // Found an element that is greater than the next element.
        }
    }
    return true; // All elements are in sorted order.
}

/**
 * Finds the first unsorted element in a range of elements.
 *
 * This function checks if the elements in the range [base, base + num * size) are sorted in ascending order
 * according to the comparison function comp. It returns a pointer to the first unsorted element, or a pointer
 * to the end of the range if the entire range is sorted.
 *
 * @param base Pointer to the start of the range.
 * @param num Number of elements in the range.
 * @param size Size of each element in bytes.
 * @param comp Comparison function used to determine the order of the elements.
 * @return Pointer to the first unsorted element, or a pointer to the end of the range if the entire range is sorted.
 */
void *algorithm_is_sorted_until(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num < 2) {
        return (void *)((char *)base + num * size); // Entire range is sorted
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            return (void *)(ptr + (i + 1) * size); // Found unsorted element
        }
    }
    return (void *)((char *)base + num * size); // Entire range is sorted
}

/**
 * Rotates the elements in the range [first, last) in a circular manner.
 *
 * This function swaps the elements in the range [first, middle) with the elements in the range [middle, last),
 * effectively rotating the elements in a circular manner. The size of each element is specified by the parameter 'size'.
 *
 * @param first Pointer to the first element of the range.
 * @param middle Pointer to the middle element of the range.
 * @param last Pointer to the element past the last element of the range.
 * @param size Size of each element in bytes.
 */
void algorithm_rotate(void *first, void *middle, void *last, size_t size) {
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
}

/**
 * Copies the elements from the range [first, last) to the range [result, result + (last - first)),
 * with the element at the middle position being the first element in the resulting range.
 *
 * @param first   Pointer to the beginning of the input range.
 * @param middle  Pointer to the middle position in the input range.
 * @param last    Pointer to the end of the input range.
 * @param size    Size of each element in bytes.
 * @param result  Pointer to the beginning of the output range.
 */
void algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result) {
    const char *first_ptr = (const char *)first;
    const char *middle_ptr = (const char *)middle;
    const char *last_ptr = (const char *)last;
    char *result_ptr = (char *)result;

    // Copy from middle to last into result
    while (middle_ptr != last_ptr) {
        memcpy(result_ptr, middle_ptr, size);
        result_ptr += size;
        middle_ptr += size;
    }

    // Copy from first to middle into result
    while (first_ptr != (const char *)middle) {
        memcpy(result_ptr, first_ptr, size);
        result_ptr += size;
        first_ptr += size;
    }
}

/**
 * Merges two sorted arrays into a single sorted array.
 *
 * This function takes two arrays, `base1` and `base2`, each containing `num1` and `num2` elements respectively.
 * The elements in both arrays are assumed to be of size `size` bytes.
 * The function merges the elements from both arrays into a single sorted array `result`, using the provided comparison function `comp`.
 *
 * @param base1 The pointer to the first array.
 * @param num1 The number of elements in the first array.
 * @param base2 The pointer to the second array.
 * @param num2 The number of elements in the second array.
 * @param size The size of each element in bytes.
 * @param result The pointer to the resulting merged array.
 * @param comp The comparison function used to determine the order of the elements.
 */
void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp) {
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
}

/**
 * Merges two sorted subarrays within the given array in-place.
 *
 * This function merges the subarray [0, middle) and the subarray [middle, num)
 * within the given array. The elements are compared using the provided comparison
 * function `comp`. The merged subarray is sorted in ascending order.
 *
 * @param base The base address of the array.
 * @param middle The index separating the two subarrays.
 * @param num The total number of elements in the array.
 * @param size The size of each element in the array.
 * @param comp The comparison function used to compare elements.
 */
void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp) {
    size_t i = 0, j = middle, k;
    char *arr = (char *)base;
    char temp[size];

    while (i < middle && j < num) {
        if (comp(arr + i * size, arr + j * size) <= 0) {
            i++;
        } else {
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
}

/**
 * Finds the first pair of adjacent elements in an array that satisfy a given comparison function.
 *
 * This function searches for the first pair of adjacent elements in the array pointed to by 'base'
 * that satisfy the comparison function 'comp'. The array contains 'num' elements, each of size 'size'.
 *
 * @param base The pointer to the start of the array.
 * @param num The number of elements in the array.
 * @param size The size of each element in the array.
 * @param comp The comparison function used to compare elements.
 * @return A pointer to the first element of the pair that satisfies the comparison function,
 *         or NULL if no such pair is found.
 */
void *algorithm_adjacent_find(const void *base, size_t num, size_t size, CompareFunc comp) {
    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) == 0) {
            return (void *)(ptr + i * size);
        }
    }
    return NULL;
}

/**
 * Finds the first mismatch between two sequences.
 *
 * This function compares the elements of two sequences, starting from the beginning, until a mismatch is found or one of the sequences ends.
 * The comparison is performed using the provided comparison function.
 *
 * @param base1 Pointer to the first element of the first sequence.
 * @param num1 Number of elements in the first sequence.
 * @param size1 Size of each element in the first sequence.
 * @param base2 Pointer to the first element of the second sequence.
 * @param num2 Number of elements in the second sequence.
 * @param size2 Size of each element in the second sequence.
 * @param comp Comparison function used to compare elements.
 * @return A Pair struct containing pointers to the first mismatched elements, or NULL if no mismatch is found.
 */
Pair algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFuncBool comp) {
    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;

    size_t min_num = num1 < num2 ? num1 : num2;

    for (size_t i = 0; i < min_num; i++) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            Pair mismatch;
            mismatch.first = (void *)(ptr1 + i * size1);
            mismatch.second = (void *)(ptr2 + i * size2);
            return mismatch;
        }
    }

    Pair mismatch = {NULL, NULL};
    return mismatch;
}

/**
 * Checks if two arrays are permutations of each other.
 *
 * This function compares two arrays, `base1` and `base2`, to determine if they are permutations of each other.
 * The comparison is done based on the number of elements (`num1` and `num2`), the size of each element (`size1` and `size2`),
 * and a comparison function (`comp`) that is used to compare individual elements.
 *
 * @param base1     Pointer to the first array.
 * @param num1      Number of elements in the first array.
 * @param size1     Size of each element in the first array.
 * @param base2     Pointer to the second array.
 * @param num2      Number of elements in the second array.
 * @param size2     Size of each element in the second array.
 * @param comp      Comparison function used to compare individual elements.
 * @return          Returns `true` if the arrays are permutations of each other, `false` otherwise.
 */
bool algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    // Check if both arrays have the same number of elements
    if (num1 != num2 || size1 != size2) {
        return false;
    }

    // Temporary arrays to mark elements as found
    bool found1[num1], found2[num2];
    memset(found1, 0, sizeof(found1));
    memset(found2, 0, sizeof(found2));

    for (size_t i = 0; i < num1; ++i) {
        for (size_t j = 0; j < num2; ++j) {
            // Check if elements match and neither has been paired before
            if (!found1[i] && !found2[j] && comp((char *)base1 + i * size1, (char *)base2 + j * size2)) {
                found1[i] = true;
                found2[j] = true;
                break;
            }
        }
        // If an element in base1 does not have a match in base2
        if (!found1[i]) {
            return false;
        }
    }

    return true;
}

/**
 * Searches for a subsequence within a sequence using a custom comparison function.
 *
 * This function searches for the subsequence defined by the range [first2, last2)
 * within the sequence defined by the range [first1, last1). The size of each element
 * in the sequences is specified by the parameters size1 and size2. The comparison
 * function comp is used to compare elements.
 *
 * @param first1 Pointer to the beginning of the first sequence.
 * @param last1 Pointer to the end of the first sequence.
 * @param size1 Size of each element in the first sequence.
 * @param first2 Pointer to the beginning of the second sequence (subsequence).
 * @param last2 Pointer to the end of the second sequence (subsequence).
 * @param size2 Size of each element in the second sequence (subsequence).
 * @param comp Comparison function used to compare elements.
 * @return Pointer to the first occurrence of the subsequence within the sequence,
 *         or pointer to the end of the sequence if no match is found.
 */
const void* algorithm_search(const void* first1, const void* last1, size_t size1,const void* first2, const void* last2, size_t size2, CompareFuncBool comp) {
    const char* ptr1 = (const char*)first1;
    const char* ptr2 = (const char*)first2;
    const char* end1 = (const char*)last1;

    if (ptr2 == last2) {
        return first1; // If the second sequence is empty, return the start of the first
    }

    while (ptr1 != end1) {
        const char* it1 = ptr1;
        const char* it2 = ptr2;

        while (comp(it1, it2)) { // Compare elements
            it1 += size1;
            it2 += size2;
            if (it2 == last2) {
                return ptr1; // Found a match
            }
            if (it1 == end1) {
                return last1; // Reached the end of the first sequence
            }
        }
        ptr1 += size1;
    }
    return last1; // No match found
}

/**
 * Searches for a subsequence of 'count' elements in the range [first, last) that matches the value 'val',
 * using the provided comparison function 'comp'.
 *
 * @param first     Pointer to the beginning of the range.
 * @param last      Pointer to the end of the range (exclusive).
 * @param size      Size of each element in the range.
 * @param count     Number of elements in the subsequence.
 * @param val       Pointer to the value to search for.
 * @param comp      Comparison function to determine if two elements are equal.
 * @return          Pointer to the first element of the found subsequence, or 'last' if not found.
 */
const void *algorithm_search_n(const void *first, const void* last, size_t size, size_t count, const void *val, CompareFuncBool comp) {
    const char* ptr = (const char*)first;
    const char* end = (const char*)last;

    while (ptr + size * count <= end) { // Ensure there's enough room for 'count' elements
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
            return ptr; // Found the subsequence
        }
        ptr += size; // Move to the next element
    }
    return last; // Subsequence not found
}

/**
 * Removes elements from an array based on a given value.
 *
 * This function removes elements from the array pointed to by 'base' that are equal to the value pointed to by 'val'.
 * The number of elements in the array is specified by 'num', and each element has a size of 'size' bytes.
 * The comparison between elements is performed using the 'comp' function, which should return 0 if the elements are equal.
 *
 * @param base Pointer to the start of the array.
 * @param num Number of elements in the array.
 * @param size Size of each element in bytes.
 * @param val Pointer to the value to be removed.
 * @param comp Pointer to the comparison function.
 * @return Pointer to the new end of the array after removing the elements.
 */
void *algorithm_remove(void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    char *ptr = (char *)base;
    size_t new_num = 0;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) != 0) {
            if (i != new_num) {
                memcpy(ptr + new_num * size, ptr + i * size, size);
            }
            ++new_num;
        }
    }
    return ptr + new_num * size; // Pointer to the new end of the array
}

/**
 * Removes elements from the source array that match a given value and copies the remaining elements to the result array.
 *
 * @param source The pointer to the source array.
 * @param num The number of elements in the source array.
 * @param size The size of each element in bytes.
 * @param result The pointer to the result array where the remaining elements will be copied.
 * @param val The pointer to the value to be removed from the source array.
 * @param comp The function pointer to the comparison function used to compare elements.
 */
void algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, CompareFunc comp) {
    const char *src = (const char *)source;
    char *dst = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        if (comp(src + i * size, val) != 0) {
            memcpy(dst, src + i * size, size);
            dst += size;
        }
    }
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
    if (!source || !result || !pred || size == 0 || num == 0) {
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
        }
    }

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
void algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, CompareFunc comp){
    if (!base || !old_val || !new_val || !comp || size == 0 || num == 0) {
        return;
    }

    char *ptr = (char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, old_val) == 0) {
            memcpy(ptr + i * size, new_val, size);
        }
    }
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
    if (!base || !new_val || !pred || size == 0 || num == 0) {
        return;
    }

    char *ptr = (char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            memcpy(ptr + i * size, new_val, size);
        }
    }
}
