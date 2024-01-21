/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Array
*/

#ifndef ARRAY_H_
#define ARRAY_H_

#include "../vector/vector.h"

typedef struct Array Array;

struct Array {
    Vector* vec;
};

Array* array_create(size_t element_size, size_t size);
bool array_is_equal(const Array* arr1, const Array* arr2);
bool array_is_less(const Array* arr1, const Array* arr2);
bool array_is_greater(const Array* arr1, const Array* arr2);
bool array_is_not_equal(const Array* arr1, const Array* arr2);
bool array_is_less_or_equal(const Array* arr1, const Array* arr2);
bool array_is_greater_or_equal(const Array* arr1, const Array* arr2);
bool array_empty(Array* arr);
void array_deallocate(Array* arr);
void array_set(Array* arr, size_t index, const void* value);
void array_insert(Array* mainArr, const Array* otherArr, size_t index);
void array_fill(Array* arr, const void* value);
void array_swap(Array* arr1, Array* arr2);
void* array_at(Array* arr, size_t index);
void* array_begin(Array* arr);
void* array_end(Array* arr);
void* array_rbegin(Array* arr);
void* array_rend(Array* arr);
void* array_front(Array* arr);
void* array_back(Array* arr);
void* array_data(Array* arr);
size_t array_size(Array* arr);
size_t array_max_size(Array* arr);
const void* array_cbegin(Array* arr);
const void* array_cend(Array* arr);
const void* array_crbegin(Array* arr);
const void* array_crend(Array* arr);
void array_clear(Array* arr);
void array_reverse(Array* arr);
void array_sort(Array* arr, int (*compare)(const void*, const void*));
void array_copy(Array* dest, const Array* src);


#endif 
