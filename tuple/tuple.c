/**
 * @author Amin Tahmasebi
 * @date 2024 
 * @class Tuple
*/

#include <stdlib.h>
#include <string.h>
#include "tuple.h"


/**
 * @brief This function allocates memory for a tuple and its elements based on the given size.
 * Each element is initialized to `NULL`.
 * 
 * @param size The number of elements the tuple will contain.
 * @return A pointer to the newly created `Tuple`, or `NULL` if memory allocation fails.
 * 
 * @note The function will terminate the program if memory allocation fails and `TUPLE_LOGGING_ENABLE` is defined.
 */
Tuple* tuple_create(size_t size) {
    TUPLE_LOG("[tuple_create]: Entering with size: %zu", size);

    Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));
    if (!tuple) {
        TUPLE_LOG("[tuple_create]: Error: Memory allocation failed for tuple.");
        exit(-1);
    }

    tuple->size = size;
    TUPLE_LOG("[tuple_create]: Allocating memory for %zu tuple elements.", size);

    tuple->elements = (TupleElement*)calloc(size, sizeof(TupleElement));
    if (!tuple->elements) {
        TUPLE_LOG("[tuple_create]: Error: Memory allocation failed for tuple elements.");
        free(tuple);
        exit(-1);
    }

    TUPLE_LOG("[tuple_create]: Tuple created successfully with size %zu.", size);
    return tuple;
}

/**
 * @brief This function frees all memory associated with the tuple, including the memory used by each element.
 * 
 * @param tuple A pointer to the `Tuple` to be deallocated.
 * 
 * @note If the `tuple` pointer is `NULL`, the function will return immediately without performing any action.
 */
void tuple_deallocate(Tuple* tuple) {
    TUPLE_LOG("[tuple_deallocate]: Entering with tuple: %p", (void*)tuple);

    if (!tuple) {
        TUPLE_LOG("[tuple_deallocate]: Info: Tuple is NULL, can't deallocate.");
        return;
    }

    for (size_t i = 0; i < tuple->size; ++i) {
        TUPLE_LOG("[tuple_deallocate]: Freeing data at index %zu", i);
        free(tuple->elements[i].data);
    }

    TUPLE_LOG("[tuple_deallocate]: Freeing tuple elements and tuple structure.");
    free(tuple->elements);
    free(tuple);
    
    TUPLE_LOG("[tuple_deallocate]: Deallocated successfully.");
}
/**
 * @brief This function assigns new data to the specified element of the tuple. 
 * It allocates memory for the new data and copies it into the tuple, replacing any existing data.
 * 
 * @param tuple A pointer to the `Tuple` in which to set the data.
 * @param index The index of the element to be set.
 * @param data A pointer to the data to be set in the tuple.
 * @param size The size of the data to be set.
 * @return `true` if the data is successfully set, `false` otherwise.
 * 
 * @note The function will return `false` if the `tuple` is `NULL`, if the `index` is out of bounds, 
 *       or if the `data` is `NULL`.
 */
bool tuple_set(Tuple* tuple, size_t index, void* data, size_t size) {
    TUPLE_LOG("[tuple_set]: Entering with tuple: %p, index: %zu, size: %zu", (void*)tuple, index, size);

    if (!tuple) {
        TUPLE_LOG("[tuple_set]: Error: Tuple is NULL.");
        return false;
    }
    if (index >= tuple->size) {
        TUPLE_LOG("[tuple_set]: Error: Index %zu is out of bounds (size: %zu).", index, tuple->size);
        return false;
    }
    if (!data) {
        TUPLE_LOG("[tuple_set]: Error: Data is NULL.");
        return false;
    }

    void* newData = malloc(size);
    if (!newData) {
        TUPLE_LOG("[tuple_set]: Error: Memory allocation failed.");
        return false;
    }

    TUPLE_LOG("[tuple_set]: Copying new data to index %zu.", index);
    memcpy(newData, data, size);
    free(tuple->elements[index].data);

    tuple->elements[index].data = newData;
    tuple->elements[index].size = size;
    TUPLE_LOG("[tuple_set]: Data set successfully at index %zu.", index);

    return true;
}

/**
 * @brief This function returns a pointer to the data stored at the given index in the tuple.
 * It also provides the size of the data through the `outSize` parameter.
 * 
 * @param tuple Pointer to the Tuple from which data is to be retrieved.
 * @param index The index of the element to retrieve.
 * @param outSize Pointer to a `size_t` variable where the size of the retrieved data will be stored.
 * @return A pointer to the data at the specified index, or `NULL` if an error occurs (e.g., the index is out of bounds, or the tuple or outSize is `NULL`).
 * 
 * @note It is the caller's responsibility to ensure that `outSize` is a valid pointer.
 */
void* tuple_get(const Tuple* tuple, size_t index, size_t* outSize) {
    TUPLE_LOG("[tuple_get]: Entering with tuple: %p, index: %zu", (void*)tuple, index);

    if (!tuple) {
        TUPLE_LOG("[tuple_get]: Error: Tuple is NULL.");
        return NULL;
    }
    if (index >= tuple->size) {
        TUPLE_LOG("[tuple_get]: Error: Index %zu is out of bounds (size: %zu).", index, tuple->size);
        return NULL;
    }
    if (!outSize) {
        TUPLE_LOG("[tuple_get]: Error: outSize is NULL.");
        return NULL;
    }

    *outSize = tuple->elements[index].size;
    TUPLE_LOG("[tuple_get]: Returning data from index %zu, size: %zu", index, *outSize);

    return tuple->elements[index].data;
}

/**
 * @brief This function creates a new tuple with the specified number of elements. 
 * Each element in the tuple is set using the provided data and its corresponding size.
 * 
 * @param num The number of elements to store in the tuple.
 * @param ... A variable number of arguments, each consisting of a pointer to data followed by a `size_t` indicating the size of the data.
 * @return A pointer to the newly created Tuple, or `NULL` if the creation or any element setting fails.
 * 
 * @note The data provided is copied into the tuple, and the caller is responsible for managing the memory of the original data.
 */
Tuple* tuple_make_tuple(size_t num, ...) {
    TUPLE_LOG("[tuple_make_tuple]: Entering with num: %zu", num);

    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        TUPLE_LOG("[tuple_make_tuple]: Error: Creation of tuple failed.");
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void* data = va_arg(args, void*);
        size_t size = va_arg(args, size_t);
        if (!tuple_set(tuple, i, data, size)) {
            TUPLE_LOG("[tuple_make_tuple]: Error: Failed to set data at index %zu.", i);
            tuple_deallocate(tuple);
            va_end(args);

            return NULL;
        }
        TUPLE_LOG("[tuple_make_tuple]: Set data at index %zu.", i);
    }
    va_end(args);

    TUPLE_LOG("[tuple_make_tuple]: Returning tuple: %p", (void*)tuple);
    return tuple;
}

/**
 * @brief This function takes a variable number of pointers and stores them in a newly created tuple. 
 * Each element in the tuple is a pointer to the original data. This is useful for creating a 
 * tuple that references existing variables without copying their values.
 * 
 * @param num The number of pointers to be tied together into the tuple.
 * @param ... The variable number of pointers to be tied together.
 * @return A pointer to the newly created Tuple, or `NULL` if the creation fails.
 * 
 * @note The caller is responsible for managing the memory of the original data pointed to by these pointers.
 */
Tuple* tuple_tie(size_t num, ...) {
    TUPLE_LOG("[tuple_tie]: Entering with num: %zu", num);

    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        TUPLE_LOG("[tuple_tie]: Error: Creation of tuple failed.");
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void** pointer = va_arg(args, void**);
        if (!tuple_set(tuple, i, &pointer, sizeof(void*))) {
            TUPLE_LOG("[tuple_tie]: Error: Failed to set data at index %zu.", i);
            tuple_deallocate(tuple);
            va_end(args);

            return NULL;
        }
        TUPLE_LOG("[tuple_tie]: Set pointer at index %zu.", i);
    }
    va_end(args);

    TUPLE_LOG("[tuple_tie]: Returning tuple: %p", (void*)tuple);
    return tuple;
}

/**
 * @brief This function exchanges the contents of the two provided tuples. 
 * After the swap, the first tuple will contain the elements of the second tuple and vice versa.
 * 
 * @param a Pointer to the first Tuple object.
 * @param b Pointer to the second Tuple object.
 * 
 * @note If either `a` or `b` is `NULL`, the function does nothing.
 */
void tuple_swap(Tuple* a, Tuple* b) {
    TUPLE_LOG("[tuple_swap]: Entering with tuple a: %p, tuple b: %p", (void*)a, (void*)b);

    if (a == NULL) {
        TUPLE_LOG("[tuple_swap]: Error: Param 'a' is NULL.");
        return;
    }
    if (b == NULL) {
        TUPLE_LOG("[tuple_swap]: Error: Param 'b' is NULL.");
        return;
    }

    // Swap the sizes
    size_t tempSize = a->size;
    a->size = b->size;
    b->size = tempSize;
    TUPLE_LOG("[tuple_swap]: Swapped sizes, new sizes - a: %zu, b: %zu", a->size, b->size);

    // Swap the elements
    TupleElement* tempElements = a->elements;
    a->elements = b->elements;
    b->elements = tempElements;
    TUPLE_LOG("[tuple_swap]: Swapped elements.");
}

/**
 * @brief This function takes a variable number of arguments and stores them in a newly created tuple. 
 * The pointers to the original arguments are stored directly in the tuple elements without allocating 
 * new memory. This function is typically used when you want to pass a set of arguments as a tuple.
 * 
 * @param num The number of arguments to be stored in the tuple.
 * @param ... The variable number of arguments to be forwarded as a tuple.
 * @return A pointer to the newly created Tuple, or `NULL` if the creation fails.
 * 
 * @note The caller is responsible for managing the memory of the original arguments since only their pointers are stored.
 */
Tuple* tuple_forward_as_tuple(size_t num, ...) {
    TUPLE_LOG("[tuple_forward_as_tuple]: Entering with num: %zu", num);

    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        TUPLE_LOG("[tuple_forward_as_tuple]: Error: Creation of tuple failed.");
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void* pointer = va_arg(args, void*);
        tuple->elements[i].data = pointer; // Store the pointer directly
        tuple->elements[i].size = sizeof(pointer);
        TUPLE_LOG("[tuple_forward_as_tuple]: Element %zu set with data: %p", i, pointer);
    }

    va_end(args);
    TUPLE_LOG("[tuple_forward_as_tuple]: Returning tuple: %p", (void*)tuple);

    return tuple;
}

/**
 * @brief This function returns the number of elements stored in the given tuple.
 * 
 * @param tuple Pointer to the Tuple object.
 * @return The number of elements in the tuple, or 0 if the tuple is `NULL`.
 */
size_t tuple_size(const Tuple* tuple) {
    TUPLE_LOG("[tuple_size]: Entering with tuple: %p", (void*)tuple);

    if (tuple == NULL) {
        TUPLE_LOG("[tuple_size]: Error: Tuple is NULL.");
        return 0;
    }

    TUPLE_LOG("[tuple_size]: Returning size: %zu", tuple->size);
    return tuple->size;
}

/**
 * @brief This function compares two tuples element by element. It returns `true` if the tuples 
 * have the same size and their corresponding elements are equal in both size and content.
 * 
 * @param t1 Pointer to the first Tuple object.
 * @param t2 Pointer to the second Tuple object.
 * @return `true` if the tuples are equal, `false` otherwise.
 * 
 * @note If either `t1` or `t2` is `NULL`, the function returns `false`.
 */
bool tuple_is_equal(const Tuple* t1, const Tuple* t2) {
    TUPLE_LOG("[tuple_is_equal]: Entering with t1: %p, t2: %p", (void*)t1, (void*)t2);

    if (t1 == NULL) {
        TUPLE_LOG("[tuple_is_equal]: Error: Param 't1' is NULL.");
        return false;
    }
    if (t2 == NULL) {
        TUPLE_LOG("[tuple_is_equal]: Error: Param 't2' is NULL.");
        return false;
    }

    if (tuple_size(t1) != tuple_size(t2)) {
        TUPLE_LOG("[tuple_is_equal]: Tuples have different sizes: t1 = %zu, t2 = %zu", tuple_size(t1), tuple_size(t2));
        return false;
    }

    for (size_t i = 0; i < tuple_size(t1); ++i) {
        if (t1->elements[i].size != t2->elements[i].size ||
            memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size) != 0) {
            TUPLE_LOG("[tuple_is_equal]: Elements at index %zu are not equal.", i);
            return false;
        }
    }

    TUPLE_LOG("[tuple_is_equal]: Tuples are equal.");
    return true;
}

/**
 * @brief This function compares two tuples element by element. It returns `true` if the first tuple 
 * is lexicographically less than the second tuple. The comparison is done based on the size 
 * and content of each element.
 * 
 * @param t1 Pointer to the first Tuple object.
 * @param t2 Pointer to the second Tuple object.
 * @return `true` if `t1` is less than `t2`, `false` otherwise.
 * 
 * @note If either `t1` or `t2` is `NULL`, the function returns `false`.
 */
bool tuple_is_less(const Tuple* t1, const Tuple* t2) {
    TUPLE_LOG("[tuple_is_less]: Entering with t1: %p, t2: %p", (void*)t1, (void*)t2);

    if (t1 == NULL) {
        TUPLE_LOG("[tuple_is_less]: Error: Param 't1' is null.");
        return false;
    }
    if (t2 == NULL) {
        TUPLE_LOG("[tuple_is_less]: Error: Param 't2' is null.");
        return false;
    }

    size_t minSize = (tuple_size(t1) < tuple_size(t2)) ? tuple_size(t1) : tuple_size(t2);

    for (size_t i = 0; i < minSize; ++i) {
        if (t1->elements[i].size < t2->elements[i].size) {
            TUPLE_LOG("[tuple_is_less]: Element %zu of t1 is smaller than t2.", i);
            return true;
        }
        if (t1->elements[i].size > t2->elements[i].size) {
            TUPLE_LOG("[tuple_is_less]: Element %zu of t1 is larger than t2.", i);
            return false;
        }

        int cmp = memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size);
        if (cmp < 0) {
            TUPLE_LOG("[tuple_is_less]: Element %zu of t1 is lexicographically less than t2.", i);
            return true;
        }
        if (cmp > 0) {
            TUPLE_LOG("[tuple_is_less]: Element %zu of t1 is lexicographically greater than t2.", i);
            return false;
        }
    }

    bool result = tuple_size(t1) < tuple_size(t2);
    TUPLE_LOG("[tuple_is_less]: Returning result: %d", result);

    return result;
}

/**
 * @brief This function compares two tuples element by element. It returns `true` if the first tuple 
 * is lexicographically greater than the second tuple. The comparison is done based on the size 
 * and content of each element.
 * 
 * @param t1 Pointer to the first Tuple object.
 * @param t2 Pointer to the second Tuple object.
 * @return `true` if `t1` is greater than `t2`, `false` otherwise.
 * 
 * @note If either `t1` or `t2` is `NULL`, the function returns `false`.
 */
bool tuple_is_greater(const Tuple* t1, const Tuple* t2) {
    TUPLE_LOG("[tuple_is_greater]: Entering with t1: %p, t2: %p", (void*)t1, (void*)t2);

    if (t1 == NULL) {
        TUPLE_LOG("[tuple_is_greater]: Error: Param 't1' is null.");
        return false;
    }
    if (t2 == NULL) {
        TUPLE_LOG("[tuple_is_greater]: Error: Param 't2' is null.");
        return false;
    }

    size_t minSize = (tuple_size(t1) < tuple_size(t2)) ? tuple_size(t1) : tuple_size(t2);

    for (size_t i = 0; i < minSize; ++i) {
        if (t1->elements[i].size > t2->elements[i].size) {
            TUPLE_LOG("[tuple_is_greater]: Element %zu of t1 is larger than t2.", i);
            return true;
        }
        if (t1->elements[i].size < t2->elements[i].size) {
            TUPLE_LOG("[tuple_is_greater]: Element %zu of t1 is smaller than t2.", i);
            return false;
        }

        int cmp = memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size);
        if (cmp > 0) {
            TUPLE_LOG("[tuple_is_greater]: Element %zu of t1 is lexicographically greater than t2.", i);
            return true;
        }
        if (cmp < 0) {
            TUPLE_LOG("[tuple_is_greater]: Element %zu of t1 is lexicographically less than t2.", i);
            return false;
        }
    }

    bool result = tuple_size(t1) > tuple_size(t2);
    TUPLE_LOG("[tuple_is_greater]: Returning result: %d", result);

    return result;
}

/**
 * @brief This function compares two tuples element by element. It returns `true` if the tuples are 
 * not equal, i.e., if they differ in size or content.
 * 
 * @param t1 Pointer to the first Tuple object.
 * @param t2 Pointer to the second Tuple object.
 * @return `true` if `t1` and `t2` are not equal, `false` otherwise.
 * 
 * @note If either `t1` or `t2` is `NULL`, the function returns `false`.
 */
bool tuple_is_not_equal(const Tuple* t1, const Tuple* t2) {
    TUPLE_LOG("[tuple_is_not_equal]: Entering with t1: %p, t2: %p", (void*)t1, (void*)t2);

    if (t1 == NULL) {
        TUPLE_LOG("[tuple_is_not_equal]: Error: Param 't1' is null.");
        return false;
    }
    if (t2 == NULL) {
        TUPLE_LOG("[tuple_is_not_equal]: Error: Param 't2' is null.");
        return false;
    }

    bool result = !tuple_is_equal(t1, t2);
    TUPLE_LOG("[tuple_is_not_equal]: Returning result: %d", result);

    return result;
}

/**
 * @brief This function compares two tuples element by element. It returns `true` if the first tuple 
 * is greater than or equal to the second tuple according to lexicographical order.
 * 
 * @param t1 Pointer to the first Tuple object.
 * @param t2 Pointer to the second Tuple object.
 * @return `true` if `t1` is greater than or equal to `t2`, `false` otherwise.
 * 
 * @note If either `t1` or `t2` is `NULL`, the function returns `false`.
 */
bool tuple_is_greater_or_equal(const Tuple* t1, const Tuple* t2) {
    TUPLE_LOG("[tuple_is_greater_or_equal]: Entering with t1: %p, t2: %p", (void*)t1, (void*)t2);

    if (t1 == NULL) {
        TUPLE_LOG("[tuple_is_greater_or_equal]: Error: Param 't1' is null.");
        return false;
    }
    if (t2 == NULL) {
        TUPLE_LOG("[tuple_is_greater_or_equal]: Error: Param 't2' is null.");
        return false;
    }

    bool result = tuple_is_greater(t1, t2) || tuple_is_equal(t1, t2);
    TUPLE_LOG("[tuple_is_greater_or_equal]: Returning result: %d", result);

    return result;
}

/**
 * @brief This function compares two tuples element by element. It returns `true` if the first tuple 
 * is less than or equal to the second tuple according to lexicographical order.
 * 
 * @param t1 Pointer to the first Tuple object.
 * @param t2 Pointer to the second Tuple object.
 * @return `true` if `t1` is less than or equal to `t2`, `false` otherwise.
 * 
 * @note If either `t1` or `t2` is `NULL`, the function returns `false`.
 */
bool tuple_is_less_or_equal(const Tuple* t1, const Tuple* t2) {
    TUPLE_LOG("[tuple_is_less_or_equal]: Entering with t1: %p, t2: %p", (void*)t1, (void*)t2);

    if (t1 == NULL) {
        TUPLE_LOG("[tuple_is_less_or_equal]: Error: Param 't1' is null.");
        return false;
    }
    if (t2 == NULL) {
        TUPLE_LOG("[tuple_is_less_or_equal]: Error: Param 't2' is null.");
        return false;
    }
    bool result = tuple_is_less(t1, t2) || tuple_is_equal(t1, t2);
    TUPLE_LOG("[tuple_is_less_or_equal]: Returning result: %d", result);

    return result;
}

/**
 * @brief Checks if the tuple is empty.
 * 
 * This function checks whether a given tuple contains any elements. A tuple is considered empty 
 * if its size is zero.
 * 
 * @param t Pointer to the Tuple object.
 * @return `true` if the tuple is empty or if `t` is `NULL`, `false` otherwise.
 */
bool tuple_is_empty(Tuple* t) {
    TUPLE_LOG("[tuple_is_empty]: Entering with tuple: %p", (void*)t);

    if (!t) {
        TUPLE_LOG("[tuple_is_empty]: Tuple is NULL, returning true.");
        return true;
    }

    bool result = (t->size == 0);
    TUPLE_LOG("[tuple_is_empty]: Returning result: %d", result);

    return result;
}