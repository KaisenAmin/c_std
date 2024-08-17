/**
 * @author Amin Tahmasebi
 * @date 2024 
 * @class Tuple
*/

#include "tuple.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

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
    Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for tuple in tuple_create\n");
        #endif 
        exit(-1);
    }

    tuple->size = size;
    tuple->elements = (TupleElement*)calloc(size, sizeof(TupleElement));
    if (!tuple->elements) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for tuple elements in tuple_create\n");
        #endif 
        free(tuple);
        exit(-1);
    }
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
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Info: tuple is NULL can't deallocate\n");
        #endif 
        return;
    }
    for (size_t i = 0; i < tuple->size; ++i) {
        free(tuple->elements[i].data);
    }
    free(tuple->elements);
    free(tuple);
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
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: data is null in tuple_set\n");
        #endif 
        return false;
    }
    if (index >= tuple->size) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: index is bigger or equal than tuple->size in tuple_set");
        #endif 
        return false;
    }
    if (!data) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: date is null in tuple_set\n");
        #endif 
        return false;
    }

    void* newData = malloc(size);
    if (!newData) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocattion failed for newData in tuple_set\n");
        #endif 
        return false;
    }

    memcpy(newData, data, size);
    free(tuple->elements[index].data);

    tuple->elements[index].data = newData;
    tuple->elements[index].size = size;
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
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: tuple is null in tuple_get\n");
        #endif 
        return NULL;
    }
    if (index >= tuple->size) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: index is bigger or equal than tuple->size in tuple_get\n");
        #endif 
        return NULL;
    }
    if(!outSize) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: outSize is null in tuple_get please give valid address\n");
        #endif 
        return NULL;
    }

    *outSize = tuple->elements[index].size;
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
    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Creation of tuple failed in tuple_make_tuple\n");
        #endif 
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void* data = va_arg(args, void*);
        size_t size = va_arg(args, size_t);
        if (!tuple_set(tuple, i, data, size)) {
            #ifdef TUPLE_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: failed to set data in tuple_make_tuple\n");
            #endif 
            tuple_deallocate(tuple);
            va_end(args);
            return NULL;
        }
    }
    va_end(args);
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
    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Creation of tuple failed in tuple_tie\n");
        #endif 
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void** pointer = va_arg(args, void**);
        if (!tuple_set(tuple, i, &pointer, sizeof(void*))) {
            #ifdef TUPLE_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: failed to set data in tuple_tie\n");
            #endif 
            tuple_deallocate(tuple);
            va_end(args);
            return NULL;
        }
    }
    va_end(args);
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
    if (a == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 'a' is null in tuple_swap\n");
        #endif 
        return;
    }
    if (b == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 'b' is null in tuple_swap\n");
        #endif 
        return;
    }
    // Swap the sizes
    size_t tempSize = a->size;
    a->size = b->size;
    b->size = tempSize;

    // Swap the elements
    TupleElement* tempElements = a->elements;
    a->elements = b->elements;
    b->elements = tempElements;
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
    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Creation of tuple failed in tuple_forward_as_tuple\n");
        #endif 
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void* pointer = va_arg(args, void*);
        // Store the pointer directly, without allocating new memory
        tuple->elements[i].data = pointer;
        tuple->elements[i].size = sizeof(pointer);
    }
    va_end(args);
    return tuple;
}

/**
 * @brief This function returns the number of elements stored in the given tuple.
 * 
 * @param tuple Pointer to the Tuple object.
 * @return The number of elements in the tuple, or 0 if the tuple is `NULL`.
 */
size_t tuple_size(const Tuple* tuple) {
    if (tuple == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: tuple is null in tuple_size\n");
        #endif 
        return 0;
    }
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
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_equal\n");
        #endif 
        return false;
    }
    if (tuple_size(t1) != tuple_size(t2)) {
        return false;
    }
    for (size_t i = 0; i < tuple_size(t1); ++i) {
        if (t1->elements[i].size != t2->elements[i].size ||
            memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size) != 0) {
            return false;
        }
    }
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
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_less\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_less\n");
        #endif 
        return false;
    }
    size_t minSize = (tuple_size(t1) < tuple_size(t2)) ? tuple_size(t1) : tuple_size(t2);

    for (size_t i = 0; i < minSize; ++i) {
        if (t1->elements[i].size < t2->elements[i].size) {
            return true;
        }
        if (t1->elements[i].size > t2->elements[i].size) { 
            return false;
        }

        int cmp = memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size);
        if (cmp < 0) {
            return true;
        }
        if (cmp > 0) { 
            return false;
        }
    }

    return tuple_size(t1) < tuple_size(t2);
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
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_greater\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_greater\n");
        #endif 
        return false;
    }

    size_t minSize = (tuple_size(t1) < tuple_size(t2)) ? tuple_size(t1) : tuple_size(t2);

    for (size_t i = 0; i < minSize; ++i) {
        if (t1->elements[i].size > t2->elements[i].size) {
            return true;
        }
        if (t1->elements[i].size < t2->elements[i].size) { 
            return false;
        }

        int cmp = memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size);
        if (cmp > 0) { 
            return true;
        }
        if (cmp < 0) { 
            return false;
        }
    }
    return tuple_size(t1) > tuple_size(t2);
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
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_not_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_not_equal\n");
        #endif 
        return false;
    }
    return !tuple_is_equal(t1, t2);
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
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_greater_or_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_greater_or_equal\n");
        #endif 
        return false;
    }
    return tuple_is_greater(t1, t2) || tuple_is_equal(t1, t2);
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
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_less_or_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_less_or_equal\n");
        #endif 
        return false;
    }
    return tuple_is_less(t1, t2) || tuple_is_equal(t1, t2);
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
    if (!t) { 
        return true;
    }
    return t->size == 0;
}
