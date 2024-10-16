/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Array
*/

#include "array.h"
#include "../fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Creates a new Array with a specified element size and initial size.
 *
 * This function allocates memory for a new Array and initializes it with a specified
 * element size and initial size. It also creates an internal vector to hold the elements.
 * If memory allocation fails, an error message is printed and the program exits.
 *
 * @param element_size The size of each element in the Array.
 * @param size The initial number of elements in the Array.
 * 
 * @return Array* A pointer to the newly created Array.
 *         The caller is responsible for deallocating the Array using array_deallocate.
 */
Array* array_create(size_t element_size, size_t size) {
    ARRAY_LOG("[array_create] Info: Creating an array with element size %zu and initial size %zu.", element_size, size);

    Array* arr = (Array*)malloc(sizeof(Array));
    if (!arr) {
        ARRAY_LOG("[array_create] Error: Cannot allocate memory for Array object.");
        exit(-1);
    }

    arr->vec = vector_create(element_size);
    if (!arr->vec) {
        ARRAY_LOG("[array_create] Error: Cannot allocate memory for internal Vector.");
        free(arr);
        exit(-1);
    }

    vector_resize(arr->vec, size);
    ARRAY_LOG("[array_create] Success: Array created successfully.");
    
    return arr;
}

/**
 * @brief Checks if two Arrays are equal.
 *
 * This function compares two Arrays to determine if they are equal. Two Arrays are
 * considered equal if they have the same size and their elements are identical.
 * If either Array is NULL or its vector is NULL, the function returns false.
 *
 * @param arr1 The first Array to compare.
 * @param arr2 The second Array to compare.
 * @return bool True if the Arrays are equal, false otherwise.
 */
bool array_is_equal(const Array* arr1, const Array* arr2) {
    ARRAY_LOG("[array_is_equal] Info: Checking if two arrays are equal.");
    
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) {
        ARRAY_LOG("[array_is_equal] Warning: One or both arrays (or their vectors) are NULL.");
        return false;
    }
    if (arr1->vec->size != arr2->vec->size) {
        ARRAY_LOG("[array_is_equal] Info: Arrays have different sizes. They are not equal.");
        return false;
    }

    bool result = memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) == 0;
    ARRAY_LOG("[array_is_equal] Result: %d", result);

    return result;
}

/**
 * @brief Determines if the first Array is less than the second Array.
 *
 * This function compares two Arrays to determine if the first Array is less than the second.
 * An Array is considered less than another if its elements, when compared lexicographically,
 * are less than those of the other Array. If either Array is NULL or its vector is NULL,
 * the function returns false.
 *
 * @param arr1 The first Array to compare.
 * @param arr2 The second Array to compare.
 * 
 * @return bool True if the first Array is less than the second, false otherwise.
 */
bool array_is_less(const Array* arr1, const Array* arr2) {
    ARRAY_LOG("[array_is_less] Info: Comparing two arrays to check if the first is less than the second.");
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) {
        ARRAY_LOG("[array_is_less] Error: One or both arrays are NULL or their vectors are NULL.");
        return false;
    }

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    bool result = cmp < 0 || (cmp == 0 && arr1->vec->size < arr2->vec->size);
    ARRAY_LOG("[array_is_less] Result: %d", result);

    return result;
}

/**
 * @brief Determines if the first Array is greater than the second Array.
 *
 * This function compares two Arrays to determine if the first Array is greater than the second.
 * It is equivalent to calling `array_is_less` with the arguments reversed.
 *
 * @param arr1 The first Array to compare.
 * @param arr2 The second Array to compare.
 * 
 * @return bool True if the first Array is greater than the second, false otherwise.
 */
bool array_is_greater(const Array* arr1, const Array* arr2) {
    ARRAY_LOG("[array_is_greater] Info: Checking if the first array is greater than the second.");
    return array_is_less(arr2, arr1);
}

/**
 * @brief Checks if two Arrays are not equal.
 *
 * This function compares two Arrays to determine if they are not equal. Two Arrays are
 * considered not equal if they have different sizes or if any of their elements differ.
 * If either Array is NULL, the function returns true if one is NULL and the other is not.
 *
 * @param arr1 The first Array to compare.
 * @param arr2 The second Array to compare.
 * @return bool True if the Arrays are not equal, false otherwise.
 */
bool array_is_not_equal(const Array* arr1, const Array* arr2) {
    ARRAY_LOG("[array_is_not_equal] Info: Checking if the two arrays are not equal.");

    if (arr1 == NULL || arr2 == NULL) {
        ARRAY_LOG("[array_is_not_equal] Warning: One or both arrays are NULL. They are considered not equal.");
        return arr1 != arr2;
    }
    if (arr1->vec->size != arr2->vec->size) {
        ARRAY_LOG("[array_is_not_equal] Result: Arrays have different sizes.");
        return true;
    }

    bool result = memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) != 0;
    ARRAY_LOG("[array_is_not_equal] Result: %d", result);

    return result;
}

/**
 * @brief Checks if the first Array is less than or equal to the second Array.
 *
 * This function compares two Arrays to determine if the first Array is less than or equal to the second.
 * It first compares the elements lexicographically, and if they are equal, it checks the size.
 * If either Array is NULL, the function returns true if the first Array is NULL and the second is not.
 *
 * @param arr1 The first Array to compare.
 * @param arr2 The second Array to compare.
 * 
 * @return bool True if the first Array is less than or equal to the second, false otherwise.
 */
bool array_is_less_or_equal(const Array* arr1, const Array* arr2) {
    ARRAY_LOG("[array_is_less_or_equal] Info: Checking if the first array is less than or equal to the second.");

    if (arr1 == NULL || arr2 == NULL) {
        ARRAY_LOG("[array_is_less_or_equal] Warning: One or both arrays are NULL.");
        return (arr1 == NULL) && (arr2 != NULL);
    }

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    bool result = cmp < 0 || (cmp == 0 && arr1->vec->size <= arr2->vec->size);
    ARRAY_LOG("[array_is_less_or_equal] Result: %d", result);

    return result;
}

/**
 * @brief Checks if the first Array is greater than or equal to the second Array.
 *
 * This function compares two Arrays to determine if the first Array is greater than or equal to the second.
 * It is equivalent to checking if the second Array is less than or equal to the first.
 * If both Arrays are NULL, the function returns true.
 *
 * @param arr1 The first Array to compare.
 * @param arr2 The second Array to compare.
 * 
 * @return bool True if the first Array is greater than or equal to the second, false otherwise.
 */
bool array_is_greater_or_equal(const Array* arr1, const Array* arr2) {
    ARRAY_LOG("[array_is_greater_or_equal] Info: Checking if the first array is greater than or equal to the second.");

    if (arr1 == NULL && arr2 == NULL) {
        ARRAY_LOG("[array_is_greater_or_equal] Both arrays are NULL, they are considered equal.");
        return true;
    }
    if (arr1 == NULL || arr2 == NULL) {
        ARRAY_LOG("[array_is_greater_or_equal] Warning: One of the arrays is NULL.");
        return (arr1 != NULL) && (arr2 == NULL);
    }

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    bool result = cmp > 0 || (cmp == 0 && arr1->vec->size >= arr2->vec->size);
    ARRAY_LOG("[array_is_greater_or_equal] Result: %d", result);

    return result;
}
/**
 * @brief Checks if the Array is empty.
 *
 * This function checks if the Array is empty by verifying if the Array or its internal vector is NULL,
 * or if the vector's size is zero.
 *
 * @param arr The Array to check.
 * @return bool True if the Array is empty, false otherwise.
 */
bool array_empty(Array* arr) {
    ARRAY_LOG("[array_empty] Info: Checking if the array is empty.");

    bool result = arr == NULL || arr->vec == NULL || arr->vec->size == 0;
    ARRAY_LOG("[array_empty] Result: %d", result);

    return result;
}
/**
 * @brief Deallocates the memory used by the Array.
 *
 * This function deallocates the memory used by the Array, including its internal vector.
 * If the Array or its vector is NULL, the function simply returns without action.
 *
 * @param arr The Array to deallocate.
 */
void array_deallocate(Array* arr) {
    ARRAY_LOG("[array_deallocate] Info: Deallocating the array.");

    if (arr != NULL) {
        if (arr->vec != NULL) {
            vector_deallocate(arr->vec);
            ARRAY_LOG("[array_deallocate] Info: Deallocated the vector.");
        }
        free(arr);
        ARRAY_LOG("[array_deallocate] Info: Deallocated the array structure.");
    } 
    else {
        ARRAY_LOG("[array_deallocate] Warning: Array is NULL. No action taken.");
    }
}

/**
 * @brief Sets the value at a specific index in the Array.
 *
 * This function sets the value of an element at the specified index in the Array.
 * If the Array, its vector, the index, or the value is invalid, an error message is printed
 * and the function returns without action.
 *
 * @param arr The Array in which to set the value.
 * @param index The index at which to set the value.
 * @param value The value to set at the specified index.
 */
void array_set(Array* arr, size_t index, const void* value) {
    ARRAY_LOG("[array_set] Info: Setting value at index %zu.", index);

    if (arr == NULL) {
        ARRAY_LOG("[array_set] Error: Array is NULL.");
        return;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_set] Error: Array's vector is NULL.");
        return;
    }
    if (index >= arr->vec->size) {
        ARRAY_LOG("[array_set] Error: Index %zu out of bounds. Vector size is %zu.", index, arr->vec->size);
        return;
    }
    if (value == NULL) {
        ARRAY_LOG("[array_set] Error: Value is NULL.");
        return;
    }

    memcpy((char*)arr->vec->items + (index * arr->vec->itemSize), value, arr->vec->itemSize);
    ARRAY_LOG("[array_set] Success: Value set at index %zu.", index);
}

/**
 * @brief Inserts elements of another Array into the main Array at a specified index.
 *
 * This function inserts the elements of another Array into the main Array starting at the specified index.
 * If the main Array, its vector, the other Array, or the index is invalid, an error message is printed
 * and the function returns without action.
 *
 * @param mainArr The main Array into which the elements will be inserted.
 * @param otherArr The Array whose elements will be inserted into the main Array.
 * @param index The index in the main Array where the elements will be inserted.
 */
void array_insert(Array* mainArr, const Array* otherArr, size_t index) {
    ARRAY_LOG("[array_insert] Info: Inserting elements into main array at index %zu.", index);

    if (mainArr == NULL) {
        ARRAY_LOG("[array_insert] Error: Main array is NULL.");
        return;
    }
    if (mainArr->vec == NULL) {
        ARRAY_LOG("[array_insert] Error: Main array's vector is NULL.");
        return;
    }
    if (otherArr == NULL) {
        ARRAY_LOG("[array_insert] Error: Other array is NULL.");
        return;
    }
    if (otherArr->vec == NULL) {
        ARRAY_LOG("[array_insert] Error: Other array's vector is NULL.");
        return;
    }

    size_t newTotalSize = index + otherArr->vec->size;
    if (newTotalSize > mainArr->vec->size) {
        ARRAY_LOG("[array_insert] Info: Resizing main array's vector to size %zu.", newTotalSize);
        vector_resize(mainArr->vec, newTotalSize);
    }

    for (size_t i = 0; i < otherArr->vec->size; ++i) {
        void* value = vector_at(otherArr->vec, i);
        memcpy((char*)mainArr->vec->items + ((index + i) * mainArr->vec->itemSize), value, mainArr->vec->itemSize);
    }
    ARRAY_LOG("[array_insert] Success: Elements from other array inserted at index %zu.", index);
}

/**
 * @brief Fills the Array with the specified value.
 *
 * This function fills the entire Array with the provided value by copying
 * the value into each element of the Array. If the Array, its vector, or
 * the value is NULL, an error message is printed and the function returns without action.
 *
 * @param arr The Array to be filled.
 * @param value The value to fill the Array with.
 */
void array_fill(Array* arr, const void* value) {
    ARRAY_LOG("[array_fill] Info: Filling array with specified value.");

    if (arr == NULL) {
        ARRAY_LOG("[array_fill] Error: Array is NULL.");
        return;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_fill] Error: Array's vector is NULL.");
        return;
    }
    if (value == NULL) {
        ARRAY_LOG("[array_fill] Error: 'value' is NULL.");
        return;
    }

    for (size_t i = 0; i < arr->vec->size; ++i) { 
        memcpy((char*)arr->vec->items + (i * arr->vec->itemSize), value, arr->vec->itemSize);
    }
    ARRAY_LOG("[array_fill] Success: Array filled with value.");
}

/**
 * @brief Swaps the contents of two Arrays.
 *
 * This function swaps the contents of two Arrays by exchanging their internal vectors.
 * If either Array or its vector is NULL, an error message is printed and the function
 * returns without action.
 *
 * @param arr1 The first Array.
 * @param arr2 The second Array.
 */
void array_swap(Array* arr1, Array* arr2) {
    ARRAY_LOG("[array_swap] Info: Swapping two arrays.");

    if (arr1 == NULL) {
        ARRAY_LOG("[array_swap] Error: arr1 is NULL.");
        return;
    }
    if (arr2 == NULL) {
        ARRAY_LOG("[array_swap] Error: arr2 is NULL.");
        return;
    }
    if (arr1->vec == NULL) {
        ARRAY_LOG("[array_swap] Error: arr1's vector is NULL.");
        return;
    }
    if (arr2->vec == NULL) {
        ARRAY_LOG("[array_swap] Error: arr2's vector is NULL.");
        return;
    }

    Vector* temp = arr1->vec;
    arr1->vec = arr2->vec;
    arr2->vec = temp;

    ARRAY_LOG("[array_swap] Success: Arrays swapped.");
}

/**
 * @brief Returns a pointer to the element at the specified index in the Array.
 *
 * This function returns a pointer to the element at the given index. If the Array,
 * its vector is NULL, or the index is out of bounds, an error message is printed
 * and the function returns NULL.
 *
 * @param arr The Array from which to retrieve the element.
 * @param index The index of the element to retrieve.
 * 
 * @return void* A pointer to the element at the specified index.
 *         Returns NULL if the Array, its vector is NULL, or the index is out of bounds.
 */
void* array_at(Array* arr, size_t index) {
    ARRAY_LOG("[array_at] Info: Accessing element at index %zu.", index);

    if (arr == NULL) {
        ARRAY_LOG("[array_at] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_at] Error: Array's vector is NULL.");
        return NULL;
    }
    if (index >= arr->vec->size) {
        ARRAY_LOG("[array_at] Error: Index %zu out of bounds. Vector size is %zu.", index, arr->vec->size);
        return NULL;
    }

    ARRAY_LOG("[array_at] Success: Element at index %zu accessed.", index);
    return vector_at(arr->vec, index);
}

/**
 * @brief Returns a pointer to the beginning of the Array.
 *
 * This function returns a pointer to the first element of the Array. If the Array,
 * its vector is NULL, or the Array is empty, an error message is printed and the
 * function returns NULL.
 *
 * @param arr The Array whose beginning pointer is to be retrieved.
 * @return void* A pointer to the beginning of the Array.
 *         Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
void* array_begin(Array* arr) {
    ARRAY_LOG("[array_begin] Info: Accessing beginning of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_begin] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_begin] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_begin] Error: Array is empty.");
        return NULL;
    }

    ARRAY_LOG("[array_begin] Success: Pointer to beginning of the array returned.");
    return arr->vec->items;
}

/**
 * @brief Returns a pointer to the end of the Array.
 *
 * This function returns a pointer to one element past the last element of the Array.
 * If the Array, its vector is NULL, or the Array is empty, an error message is printed
 * and the function returns NULL.
 *
 * @param arr The Array whose end pointer is to be retrieved.
 * @return void* A pointer to the end of the Array.
 *         Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
void* array_end(Array* arr) {
    ARRAY_LOG("[array_end] Info: Accessing end of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_end] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_end] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_end] Error: Array is empty.");
        return NULL;
    }

    ARRAY_LOG("[array_end] Success: Pointer to end of the array returned.");
    return vector_end(arr->vec);
}

/**
 * @brief Returns a reverse iterator to the beginning of the Array.
 *
 * This function returns a reverse iterator pointing to the last element of the Array.
 * If the Array, its vector is NULL, or the Array is empty, an error message is printed
 * and the function returns NULL.
 *
 * @param arr The Array whose reverse beginning iterator is to be retrieved.
 * @return void* A reverse iterator to the beginning of the Array.
 *         Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
void* array_rbegin(Array* arr) {
    ARRAY_LOG("[array_rbegin] Info: Accessing reverse beginning of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_rbegin] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_rbegin] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_rbegin] Error: Array is empty.");
        return NULL;
    }

    ARRAY_LOG("[array_rbegin] Success: Reverse iterator to the beginning returned.");
    return vector_rbegin(arr->vec);
}

/**
 * @brief Returns a reverse iterator to the end of the Array.
 *
 * This function returns a reverse iterator pointing to one element before the first
 * element of the Array. If the Array, its vector is NULL, or the Array is empty,
 * an error message is printed and the function returns NULL.
 *
 * @param arr The Array whose reverse end iterator is to be retrieved.
 * @return void* A reverse iterator to the end of the Array.
 *         Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
void* array_rend(Array* arr) {
    ARRAY_LOG("[array_rend] Info: Accessing reverse end of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_rend] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_rend] Error: Vector is NULL.");
        return NULL;
    }

    ARRAY_LOG("[array_rend] Success: Reverse iterator to the end returned.");
    return vector_rend(arr->vec);
}

/**
 * @brief Returns a pointer to the first element of the Array.
 *
 * This function returns a pointer to the first element of the Array. If the Array is NULL,
 * or if the internal vector is NULL, or if the Array is empty, it returns NULL and prints
 * an error message to stderr.
 *
 * @param arr The Array from which to retrieve the first element.
 * @return void* A pointer to the first element of the Array, or NULL if an error occurs.
 */
void* array_front(Array* arr) {
    ARRAY_LOG("[array_front] Info: Retrieving pointer to the first element of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_front] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_front] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_front] Error: Array is empty.");
        return NULL;
    }

    ARRAY_LOG("[array_front] Success: Pointer to the first element returned.");
    return vector_at(arr->vec, 0);  // First element
}

/**
 * @brief Returns a pointer to the last element of the Array.
 *
 * This function returns a pointer to the last element of the Array. If the Array is NULL,
 * or if the internal vector is NULL, or if the Array is empty, it returns NULL and prints
 * an error message to stderr.
 *
 * @param arr The Array from which to retrieve the last element.
 * @return void* A pointer to the last element of the Array, or NULL if an error occurs.
 */
void* array_back(Array* arr) {
    ARRAY_LOG("[array_back] Info: Retrieving pointer to the last element of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_back] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_back] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_back] Error: Array is empty.");
        return NULL;
    }

    ARRAY_LOG("[array_back] Success: Pointer to the last element returned.");
    return vector_at(arr->vec, arr->vec->size - 1);
}

/**
 * @brief Returns a pointer to the underlying data of the Array.
 *
 * This function returns a pointer to the underlying data of the Array. If the Array is NULL,
 * or if the internal vector is NULL, it returns NULL and prints an error message to stderr.
 *
 * @param arr The Array from which to retrieve the data pointer.
 * @return void* A pointer to the underlying data of the Array, or NULL if an error occurs.
 */
void* array_data(Array* arr) {
    ARRAY_LOG("[array_data] Info: Retrieving pointer to the underlying data of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_data] Error: Array is NULL.");
        return NULL;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_data] Error: Vector is NULL.");
        return NULL;
    }

    ARRAY_LOG("[array_data] Success: Pointer to the underlying data returned.");
    return arr->vec->items;
}

/**
 * @brief Returns the number of elements in the Array.
 *
 * This function returns the number of elements currently stored in the Array.
 * If the Array or its internal vector is NULL, the function returns 0.
 *
 * @param arr The Array whose size is to be retrieved.
 * @return size_t The number of elements in the Array. Returns 0 if the Array or its vector is NULL.
 */
size_t array_size(Array* arr) {
    ARRAY_LOG("[array_size] Info: Retrieving size of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_size] Error: Array is NULL.");
        return 0;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_size] Error: Vector is NULL.");
        return 0;
    }

    ARRAY_LOG("[array_size] Success: Size of the array returned: %zu.", arr->vec->size);
    return arr->vec->size;
}

/**
 * @brief Returns the maximum number of elements the Array can hold.
 *
 * This function returns the maximum number of elements that the Array can hold.
 * If the Array or its internal vector is NULL, the function returns 0.
 *
 * @param arr The Array whose maximum size is to be retrieved.
 * @return size_t The maximum number of elements the Array can hold. Returns 0 if the Array or its vector is NULL.
 */
size_t array_max_size(Array* arr) {
    ARRAY_LOG("[array_max_size] Info: Retrieving maximum size of the array.");

    if (arr == NULL) {
        ARRAY_LOG("[array_max_size] Error: Array is NULL.");
        return 0;
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_max_size] Error: Vector is NULL.");
        return 0;
    }

    ARRAY_LOG("[array_max_size] Success: Maximum size of the array returned: %zu.", vector_max_size(arr->vec));
    return vector_max_size(arr->vec);
}

/**
 * @brief Returns a constant iterator to the beginning of the Array.
 *
 * This function provides a constant iterator pointing to the first element
 * of the Array. If the Array, its vector is NULL, or the Array is empty, 
 * the function returns NULL.
 *
 * @param arr The Array whose constant beginning iterator is to be retrieved.
 * 
 * @return const void* A constant iterator to the beginning of the Array.
 * Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
const void* array_cbegin(Array* arr) {
    ARRAY_LOG("[array_cbegin] Info: Retrieving constant iterator to the beginning of the array.");

    if (arr == NULL) { 
        ARRAY_LOG("[array_cbegin] Error: Array is NULL.");
        return NULL; 
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_cbegin] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_cbegin] Error: Vector size is 0.");
        return NULL;
    }

    ARRAY_LOG("[array_cbegin] Success: Constant iterator to the beginning returned.");
    return vector_cbegin(arr->vec); 
}

/**
 * @brief Returns a constant iterator to the end of the Array.
 *
 * This function provides a constant iterator pointing to one element past the
 * last element of the Array. If the Array, its vector is NULL, or the Array is empty, 
 * the function returns NULL.
 *
 * @param arr The Array whose constant end iterator is to be retrieved.
 * 
 * @return const void* A constant iterator to the end of the Array.
 * Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
const void* array_cend(Array* arr) {
    ARRAY_LOG("[array_cend] Info: Retrieving constant iterator to the end of the array.");

    if (arr == NULL) { 
        ARRAY_LOG("[array_cend] Error: Array is NULL.");
        return NULL;  
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_cend] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_cend] Error: Vector size is 0.");
        return NULL;
    }

    ARRAY_LOG("[array_cend] Success: Constant iterator to the end returned.");
    return vector_cend(arr->vec);
}

/**
 * @brief Returns a constant reverse iterator to the last element of the Array.
 *
 * This function provides a constant reverse iterator pointing to the last element
 * of the Array. If the Array, its vector is NULL, or the Array is empty, 
 * the function returns NULL.
 *
 * @param arr The Array whose constant reverse beginning iterator is to be retrieved.
 * 
 * @return const void* A constant reverse iterator to the last element of the Array.
 * Returns NULL if the Array, its vector is NULL, or the Array is empty.
 */
const void* array_crbegin(Array* arr) {
    ARRAY_LOG("[array_crbegin] Info: Retrieving constant reverse iterator to the beginning of the array.");

    if (arr == NULL) { 
        ARRAY_LOG("[array_crbegin] Error: Array is NULL.");
        return NULL;  
    }
    if (arr->vec == NULL) {
        ARRAY_LOG("[array_crbegin] Error: Vector is NULL.");
        return NULL;
    }
    if (arr->vec->size == 0) {
        ARRAY_LOG("[array_crbegin] Error: Vector size is 0.");
        return NULL;
    }

    ARRAY_LOG("[array_crbegin] Success: Constant reverse iterator to the beginning returned.");
    return vector_crbegin(arr->vec);
}

/**
 * @brief Returns a constant reverse iterator to the end of the Array.
 *
 * This function provides a constant reverse iterator pointing to one element past
 * the first element of the Array, effectively marking the end of the reversed Array.
 *
 * @param arr The Array whose reverse end iterator is to be retrieved.
 * 
 * @return const void* A constant reverse iterator to the end of the Array.
 *         Returns NULL if the Array or its internal vector is NULL.
 */
const void* array_crend(Array* arr) {
    ARRAY_LOG("[array_crend] Info: Retrieving constant reverse iterator to the end of the array.");

    if (arr == NULL || arr->vec == NULL) { 
        ARRAY_LOG("[array_crend] Error: Invalid input (Array or Vector is NULL).");
        return NULL;  
    }

    ARRAY_LOG("[array_crend] Success: Constant reverse iterator to the end returned.");
    return vector_crend(arr->vec);
}

/**
 * @brief Clears all elements from the Array.
 *
 * This function removes all elements from the Array without deallocating the memory.
 * The size of the Array is set to zero after this operation.
 *
 * @param arr The Array to be cleared.
 */
void array_clear(Array* arr) {
    ARRAY_LOG("[array_clear] Info: Clearing array.");
    if (arr == NULL || arr->vec == NULL) { 
        ARRAY_LOG("[array_clear] Error: Invalid array or vector pointer.");
        return;
    }

    ARRAY_LOG("[array_clear] Info: Clearing array with size %zu.", arr->vec->size);
    vector_clear(arr->vec);
    ARRAY_LOG("[array_clear] Success: Array cleared.");
}

/**
 * @brief Reverses the order of elements in the Array.
 *
 * This function reverses the order of the elements in the Array in place. If the Array has 
 * zero or one element, the function returns immediately as there's nothing to reverse.
 *
 * @param arr The Array to be reversed.
 */
void array_reverse(Array* arr) {
    ARRAY_LOG("[array_reverse] Info: Reversing array.");

    if (arr == NULL || arr->vec == NULL) { 
        ARRAY_LOG("[array_reverse] Error: Invalid array or vector pointer.");
        return;
    }
    if (arr->vec->size <= 1) {
        ARRAY_LOG("[array_reverse] Info: Nothing to reverse (array size is %zu).", arr->vec->size);
        return;
    }
    ARRAY_LOG("[array_reverse] Info: Reversing array with size %zu.", arr->vec->size);

    size_t start = 0;
    size_t end = arr->vec->size - 1;

    char* temp = (char*)malloc(arr->vec->itemSize);
    if (!temp) {
        ARRAY_LOG("[array_reverse] Error: Memory allocation failed.");
        return;
    }

    while (start < end) {
        memcpy(temp, (char*)arr->vec->items + (start * arr->vec->itemSize), arr->vec->itemSize);
        memcpy((char*)arr->vec->items + (start * arr->vec->itemSize), (char*)arr->vec->items + (end * arr->vec->itemSize), arr->vec->itemSize);
        memcpy((char*)arr->vec->items + (end * arr->vec->itemSize), temp, arr->vec->itemSize);
        start++;
        end--;
    }

    free(temp);
    ARRAY_LOG("[array_reverse] Success: Array reversed.");
}

/**
 * @brief Sorts the elements in the Array.
 *
 * This function sorts the elements in the Array in place using the provided comparison function.
 * The comparison function should return a negative value if the first element is less than the second,
 * zero if they are equal, and a positive value if the first is greater than the second.
 *
 * @param arr The Array to be sorted.
 * @param compare A pointer to a comparison function that takes two const void* arguments and returns an int.
 */
void array_sort(Array* arr, int (*compare)(const void*, const void*)) {
    ARRAY_LOG("[array_sort] Info: Sorting array.");

    if (arr == NULL || arr->vec == NULL || compare == NULL) {
        ARRAY_LOG("[array_sort] Error: Invalid array, vector pointer, or compare function.");
        return;
    }
    if (arr->vec->size <= 1) {
        ARRAY_LOG("[array_sort] Info: Nothing to sort (array size is %zu).", arr->vec->size);
        return;
    }

    ARRAY_LOG("[array_sort] Info: Sorting array with size %zu.", arr->vec->size);
    qsort(arr->vec->items, arr->vec->size, arr->vec->itemSize, compare);
    ARRAY_LOG("[array_sort] Success: Array sorted.");
}

/**
 * @brief Copies the contents of one Array to another.
 *
 * This function copies the elements from the source Array to the destination Array. 
 * The destination Array is resized to match the size of the source Array before the copy.
 *
 * @param dest The destination Array where the contents will be copied.
 * @param src The source Array from which to copy the contents.
 */
void array_copy(Array* dest, const Array* src) {
    ARRAY_LOG("[array_copy] Info: Starting array copy.");

    if (src == NULL) {
        ARRAY_LOG("[array_copy] Error: Source array (src) is NULL.");
        return;
    }
    if (dest == NULL) {
        ARRAY_LOG("[array_copy] Error: Destination array (dest) is NULL.");
        return;
    }
    ARRAY_LOG("[array_copy] Info: Copying from source array of size %zu to destination array of size %zu.",
              src->vec->size, dest->vec->size);

    vector_resize(dest->vec, src->vec->size);
    memcpy(dest->vec->items, src->vec->items, src->vec->size * src->vec->itemSize);
    ARRAY_LOG("[array_copy] Success: Array copied successfully. Destination array size is now %zu.", dest->vec->size);
}
