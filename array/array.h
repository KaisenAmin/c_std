#ifndef ARRAY_H_
#define ARRAY_H_

#include "../vector/vector.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct Array Array;

struct Array 
{
    Vector* vec;

    bool (*is_equal)(const Array* arr1, const Array* arr2);
    bool (*is_less)(const Array* arr1, const Array* arr2);
    bool (*is_greater)(const Array* arr1, const Array* arr2);
    bool (*is_not_equal)(const Array* arr1, const Array* arr2);
    bool (*is_less_or_equal)(const Array* arr1, const Array* arr2);
    bool (*is_greater_or_equal)(const Array* arr1, const Array* arr2);
    bool (*empty)(Array* arr);
    void (*fill)(Array* arr, const void* value);
    void (*swap)(Array* arr1, Array* arr2);
    void (*deallocate)(Array* arr);
    void (*set)(Array* arr, size_t index, const void* value);
    void (*insert)(Array* mainArr, const Array* otherArr, size_t index);
    void* (*at)(Array* arr, size_t index);
    void* (*begin)(Array* arr);
    void* (*end)(Array* arr);
    void* (*rbegin)(Array* arr);
    void* (*rend)(Array* arr);
    void* (*front)(Array* arr);
    void* (*back)(Array* arr);
    void* (*data)(Array* arr);
    size_t (*size)(Array* arr);
    size_t (*max_size)(Array* arr);
    const void* (*cbegin)(Array* arr);
    const void* (*cend)(Array* arr);
    const void* (*crbegin)(Array* arr);
    const void* (*crend)(Array* arr);
};

Array* array_create(size_t element_size, size_t size);

#endif 
