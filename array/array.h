#ifndef ARRAY_H_
#define ARRAY_H_

#include <stddef.h>   
#include <stdbool.h>
#include <string.h>   
#include <stdlib.h>   

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Declares a new fixed-size array type.
 *
 * This macro declares a new type named `name` that contains a fixed-size array
 * of N elements of the specified type. The storage is allocated on the stack.
 *
 * @param type The element type.
 * @param N The number of elements.
 * @param name The new type name.
 *
 * @note This is similar in spirit to C++'s std::array.
 *
 * @example
 *     array_create(int, 5, ArrayInt5);
 *     // Now you can declare: ArrayInt5 arr; // a fixed-size array of 5 ints.
 */
#define array_create(type, N, name) \
    typedef struct {                \
        type data[N];               \
    } name

/**
 * @brief Returns the number of elements in the array.
 *
 * Since the array is fixed-size, this value is computed at compile time.
 *
 * @param arr An instance of a fixed-size array type.
 * @return The number of elements in the array.
 */
#define array_size(arr) (sizeof((arr).data) / sizeof((arr).data[0]))

/**
 * @brief Returns the element at the specified index.
 *
 * No bounds checking is performed.
 *
 * @param arr An instance of a fixed-size array.
 * @param index The index of the element.
 * @return The element at that index.
 */
#define array_at(arr, index) ((arr).data[(index)])

/**
 * @brief Returns a pointer to the first element of the array.
 *
 * @param arr An instance of a fixed-size array.
 * @return A pointer to the first element.
 */
#define array_begin(arr) ((arr).data)

/**
 * @brief Returns a pointer to one past the last element of the array.
 *
 * @param arr An instance of a fixed-size array.
 * @return A pointer to one past the last element.
 */
#define array_end(arr) ((arr).data + array_size(arr))

/**
 * @brief Returns the first element (by value) of the array.
 *
 * @param arr An instance of a fixed-size array.
 * @return The first element.
 */
#define array_front(arr) ((arr).data[0])

/**
 * @brief Returns the last element (by value) of the array.
 *
 * @param arr An instance of a fixed-size array.
 * @return The last element.
 */
#define array_back(arr) ((arr).data[array_size(arr) - 1])

/**
 * @brief Returns a pointer to the underlying data of the array.
 *
 * Equivalent to array_begin.
 *
 * @param arr An instance of a fixed-size array.
 * @return A pointer to the array data.
 */
#define array_data(arr) ((arr).data)

/* --- Modification Functions --- */

/**
 * @brief Fills the array with the given value.
 *
 * This macro assigns the provided value to every element of the array.
 *
 * @param arr An instance of a fixed-size array.
 * @param value The value to fill in (assigned directly to each element).
 */
#define array_fill(arr, value)                      \
    do {                                            \
        size_t _i;                                  \
        for (_i = 0; _i < array_size(arr); ++_i) {   \
            (arr).data[_i] = (value);               \
        }                                           \
    } while (0)

/* --- Internal Helper Implementations --- */

/**
 * @brief Internal implementation of array_reverse.
 *
 * Reverses a block of memory in place.
 *
 * @param data Pointer to the data.
 * @param count Number of elements.
 * @param elem_size Size in bytes of one element.
 */
static inline void __array_reverse_impl(void *data, size_t count, size_t elem_size) {
    unsigned char *ptr = (unsigned char*)data;
    for (size_t i = 0; i < count / 2; i++) {
        size_t j = count - 1 - i;
        for (size_t k = 0; k < elem_size; k++) {
            unsigned char tmp = ptr[i * elem_size + k];
            ptr[i * elem_size + k] = ptr[j * elem_size + k];
            ptr[j * elem_size + k] = tmp;
        }
    }
}

/**
 * @brief Reverses the array in place.
 *
 * This macro reverses the order of elements in the fixed-size array.
 *
 * @param arr An instance of a fixed-size array.
 */
#define array_reverse(arr) \
    __array_reverse_impl((arr).data, array_size(arr), sizeof((arr).data[0]))

/**
 * @brief Internal implementation of array_sort.
 *
 * Sorts an array using the standard qsort function.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param compare Comparison function (same as for qsort).
 */
static inline void __array_sort_impl(void *data, size_t count, size_t elem_size,
                                       int (*compare)(const void *, const void *)) {
    qsort(data, count, elem_size, compare);
}

/**
 * @brief Sorts the array using the provided comparison function.
 *
 * @param arr An instance of a fixed-size array.
 * @param compare Function pointer to a comparison function.
 */
#define array_sort(arr, compare) \
    __array_sort_impl((arr).data, array_size(arr), sizeof((arr).data[0]), compare)

/**
 * @brief Internal implementation of array_copy.
 *
 * Copies the content from the source to destination.
 *
 * @param dest Pointer to the destination data.
 * @param src Pointer to the source data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 */
static inline void __array_copy_impl(void *dest, const void *src, size_t count, size_t elem_size) {
    memcpy(dest, src, count * elem_size);
}

/**
 * @brief Copies the contents of one array to another.
 *
 * Both arrays must be of the same type and size.
 *
 * @param dest An instance of the destination fixed-size array.
 * @param src An instance of the source fixed-size array.
 */
#define array_copy(dest, src) \
    __array_copy_impl((dest).data, (src).data, array_size(dest), sizeof((dest).data[0]))

/**
 * @brief Swaps the contents of two arrays.
 *
 * Both arrays must be of the same type and size.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 */
#define array_swap(arr1, arr2)                                \
    do {                                                    \
        char _temp[sizeof((arr1).data)];                    \
        memcpy(_temp, (arr1).data, sizeof((arr1).data));    \
        memcpy((arr1).data, (arr2).data, sizeof((arr1).data));\
        memcpy((arr2).data, _temp, sizeof((arr1).data));      \
    } while (0)

/**
 * @brief Clears the array by zeroing its memory.
 *
 * @param arr An instance of a fixed-size array.
 */
#define array_clear(arr) memset((arr).data, 0, sizeof((arr).data))

/* --- Search and Iteration Functions --- */

/**
 * @brief Internal implementation of array_find.
 *
 * Searches the array for an element equal to the key using the comparison function.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param key Pointer to the key to search for.
 * @param cmp Comparison function.
 * @return Pointer to the matching element, or NULL if not found.
 */
static inline void *__array_find_impl(void *data, size_t count, size_t elem_size,
                                        const void *key,
                                        int (*cmp)(const void *, const void *)) {
    unsigned char *ptr = (unsigned char*)data;
    for (size_t i = 0; i < count; i++) {
        if (cmp(ptr + i * elem_size, key) == 0)
            return ptr + i * elem_size;
    }
    return NULL;
}

/**
 * @brief Finds an element in the array using the provided comparison function.
 *
 * @param arr An instance of a fixed-size array.
 * @param key Pointer to the key to search for.
 * @param cmp Comparison function.
 * @return Pointer to the matching element, or NULL if not found.
 */
#define array_find(arr, key, cmp) \
    __array_find_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (key), (cmp))

/**
 * @brief Internal implementation of array_find_if.
 *
 * Searches the array for an element that satisfies the predicate.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param predicate Function pointer to a predicate.
 * @return Pointer to the matching element, or NULL if not found.
 */
static inline void *__array_find_if_impl(void *data, size_t count, size_t elem_size,
                                           bool (*predicate)(const void*)) {
    unsigned char *ptr = (unsigned char*)data;
    for (size_t i = 0; i < count; i++) {
         if (predicate(ptr + i * elem_size))
             return ptr + i * elem_size;
    }
    return NULL;
}

/**
 * @brief Finds an element in the array for which the predicate returns true.
 *
 * @param arr An instance of a fixed-size array.
 * @param predicate Function pointer to a predicate.
 * @return Pointer to the matching element, or NULL if none match.
 */
#define array_find_if(arr, predicate) \
    __array_find_if_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (predicate))

/**
 * @brief Internal implementation of array_for_each.
 *
 * Applies a function to every element in the array.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param func Function pointer to apply on each element.
 */
static inline void __array_for_each_impl(void *data, size_t count, size_t elem_size,
                                           void (*func)(void *)) {
    unsigned char *ptr = (unsigned char *)data;
    for (size_t i = 0; i < count; i++) {
         func(ptr + i * elem_size);
    }
}

/**
 * @brief Applies a function to each element of the array.
 *
 * The provided function must accept a pointer to an element.
 *
 * @param arr An instance of a fixed-size array.
 * @param func Function to apply to each element.
 */
#define array_for_each(arr, func) \
    __array_for_each_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (func))


/**
 * @brief Internal implementation of array_transform.
 *
 * Applies a transformation function to each element of the source array and writes
 * the result to the destination array.
 *
 * @param src Pointer to the source array data.
 * @param dest Pointer to the destination array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param func Transformation function (signature: void func(const void *, void *)).
 */
static inline void __array_transform_impl(const void *src, void *dest, size_t count,
                                            size_t elem_size,
                                            void (*func)(const void *, void *)) {
    const unsigned char *s = (const unsigned char *)src;
    unsigned char *d = (unsigned char *)dest;
    for (size_t i = 0; i < count; i++) {
        func(s + i * elem_size, d + i * elem_size);
    }
}

/**
 * @brief Transforms each element of the source array and stores the result in the destination array.
 *
 * Both arrays must be of the same type and size.
 *
 * @param src An instance of the source fixed-size array.
 * @param dest An instance of the destination fixed-size array.
 * @param func Transformation function (signature: void func(const void *, void *)).
 */
#define array_transform(src, dest, func) \
    __array_transform_impl((src).data, (dest).data, array_size(src), sizeof((src).data[0]), (func))

/**
 * @brief Internal implementation of array_reduce.
 *
 * Reduces the array to a single value by applying a binary reduction function.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param result Pointer to memory where the reduced value will be stored.
 * @param reduce_func Reduction function (signature: void reduce_func(const void *, const void *, void *)).
 */
static inline void __array_reduce_impl(const void *data, size_t count, size_t elem_size,
                                         void *result,
                                         void (*reduce_func)(const void *, const void *, void *)) {
    if (count == 0) return;
    memcpy(result, data, elem_size);
    const unsigned char *ptr = (const unsigned char *)data;
    for (size_t i = 1; i < count; i++) {
        unsigned char temp[elem_size];
        memcpy(temp, result, elem_size);
        reduce_func(temp, ptr + i * elem_size, result);
    }
}

/**
 * @brief Reduces the array to a single value using the provided reduction function.
 *
 * The destination memory pointed to by result must be large enough to hold one element.
 *
 * @param arr An instance of a fixed-size array.
 * @param result Pointer to memory where the result will be stored.
 * @param reduce_func Reduction function (signature: void reduce_func(const void *, const void *, void *)).
 */
#define array_reduce(arr, result, reduce_func) \
    __array_reduce_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (result), (reduce_func))

/**
 * @brief Internal implementation of array_count_if.
 *
 * Counts the number of elements that satisfy the given predicate.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param predicate Function pointer to a predicate (signature: bool predicate(const void *)).
 * @return The count of elements for which predicate returns true.
 */
static inline size_t __array_count_if_impl(const void *data, size_t count, size_t elem_size,
                                             bool (*predicate)(const void *)) {
    size_t result = 0;
    const unsigned char *ptr = (const unsigned char *)data;
    for (size_t i = 0; i < count; i++) {
        if (predicate(ptr + i * elem_size))
            result++;
    }
    return result;
}

/**
 * @brief Counts the number of elements in the array that satisfy the given predicate.
 *
 * @param arr An instance of a fixed-size array.
 * @param predicate Function pointer to a predicate.
 * @return The number of elements for which predicate returns true.
 */
#define array_count_if(arr, predicate) \
    __array_count_if_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (predicate))

/**
 * @brief Internal implementation of array_any_of.
 *
 * Returns true if any element in the array satisfies the given predicate.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param predicate Function pointer to a predicate.
 * @return true if at least one element satisfies the predicate, false otherwise.
 */
static inline bool __array_any_of_impl(const void *data, size_t count, size_t elem_size,
                                         bool (*predicate)(const void *)) {
    const unsigned char *ptr = (const unsigned char *)data;
    for (size_t i = 0; i < count; i++) {
        if (predicate(ptr + i * elem_size))
            return true;
    }
    return false;
}

/**
 * @brief Returns true if any element in the array satisfies the given predicate.
 *
 * @param arr An instance of a fixed-size array.
 * @param predicate Function pointer to a predicate.
 * @return true if any element satisfies the predicate, false otherwise.
 */
#define array_any_of(arr, predicate) \
    __array_any_of_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (predicate))

/**
 * @brief Internal implementation of array_all_of.
 *
 * Returns true if every element in the array satisfies the given predicate.
 *
 * @param data Pointer to the array data.
 * @param count Number of elements.
 * @param elem_size Size of each element.
 * @param predicate Function pointer to a predicate.
 * @return true if all elements satisfy the predicate, false otherwise.
 */
static inline bool __array_all_of_impl(const void *data, size_t count, size_t elem_size,
                                         bool (*predicate)(const void *)) {
    const unsigned char *ptr = (const unsigned char *)data;
    for (size_t i = 0; i < count; i++) {
        if (!predicate(ptr + i * elem_size))
            return false;
    }
    return true;
}

/**
 * @brief Returns true if every element in the array satisfies the given predicate.
 *
 * @param arr An instance of a fixed-size array.
 * @param predicate Function pointer to a predicate.
 * @return true if all elements satisfy the predicate, false otherwise.
 */
#define array_all_of(arr, predicate) \
    __array_all_of_impl((arr).data, array_size(arr), sizeof((arr).data[0]), (predicate))

/**
 * @brief Returns true if the two arrays are equal.
 *
 * The arrays are compared lexicographically.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 * @return true if the contents are identical, false otherwise.
 */
#define array_is_equal(arr1, arr2) \
    (memcmp((arr1).data, (arr2).data, sizeof((arr1).data)) == 0)

/**
 * @brief Returns true if the first array is lexicographically less than the second.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 * @return true if arr1 < arr2, false otherwise.
 */
#define array_is_less(arr1, arr2) \
    (memcmp((arr1).data, (arr2).data, sizeof((arr1).data)) < 0)

/**
 * @brief Returns true if the first array is lexicographically greater than the second.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 * @return true if arr1 > arr2, false otherwise.
 */
#define array_is_greater(arr1, arr2) \
    (memcmp((arr1).data, (arr2).data, sizeof((arr1).data)) > 0)

/**
 * @brief Returns true if the two arrays are not equal.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 * @return true if the arrays differ, false otherwise.
 */
#define array_is_not_equal(arr1, arr2) \
    (memcmp((arr1).data, (arr2).data, sizeof((arr1).data)) != 0)

/**
 * @brief Returns true if the first array is lexicographically less than or equal to the second.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 * @return true if arr1 <= arr2, false otherwise.
 */
#define array_is_less_or_equal(arr1, arr2) \
    (memcmp((arr1).data, (arr2).data, sizeof((arr1).data)) <= 0)

/**
 * @brief Returns true if the first array is lexicographically greater than or equal to the second.
 *
 * @param arr1 First fixed-size array.
 * @param arr2 Second fixed-size array.
 * @return true if arr1 >= arr2, false otherwise.
 */
#define array_is_greater_or_equal(arr1, arr2) \
    (memcmp((arr1).data, (arr2).data, sizeof((arr1).data)) >= 0)

#ifdef __cplusplus
}
#endif

#endif
