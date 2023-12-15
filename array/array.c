#include "array.h"
#include <string.h>
#include <stdlib.h>

static bool array_is_equal_impl(const Array* arr1, const Array* arr2);
static bool array_is_less_impl(const Array* arr1, const Array* arr2);
static bool array_is_greater_impl(const Array* arr1, const Array* arr2);
static bool array_is_not_equal_impl(const Array* arr1, const Array* arr2);
static bool array_is_less_or_equal_impl(const Array* arr1, const Array* arr2);
static bool array_is_greater_or_equal_impl(const Array* arr1, const Array* arr2);
static bool array_empty_impl(Array* arr);
static void array_deallocate_impl(Array* arr);
static void array_set_impl(Array* arr, size_t index, const void* value);
static void array_insert_impl(Array* mainArr, const Array* otherArr, size_t index);
static void array_fill_impl(Array* arr, const void* value);
static void array_swap_impl(Array* arr1, Array* arr2);
static void* array_at_impl(Array* arr, size_t index);
static void* array_begin_impl(Array* arr);
static void* array_end_impl(Array* arr);
static void* array_rbegin_impl(Array* arr);
static void* array_rend_impl(Array* arr);
static void* array_front_impl(Array* arr);
static void* array_back_impl(Array* arr);
static void* array_data_impl(Array* arr);
static size_t array_size_impl(Array* arr);
static size_t array_max_size_impl(Array* arr);
static const void* array_cbegin_impl(Array* arr);
static const void* array_cend_impl(Array* arr);
static const void* array_crbegin_impl(Array* arr);
static const void* array_crend_impl(Array* arr);

Array* array_create(size_t element_size, size_t size) 
{
    Array* arr = (Array*)malloc(sizeof(Array));

    if (!arr) 
        return NULL;

    arr->vec = vector_create(element_size);
    if (!arr->vec) 
    {
        free(arr);
        return NULL;
    }

    arr->vec->resize(arr->vec, size);

    arr->at = array_at_impl; 
    arr->fill = array_fill_impl;
    arr->back = array_back_impl;
    arr->begin = array_begin_impl;
    arr->cbegin = array_cbegin_impl;
    arr->cend = array_cend_impl;
    arr->crbegin = array_crbegin_impl;
    arr->crend = array_crend_impl;
    arr->data = array_data_impl;
    arr->deallocate = array_deallocate_impl;
    arr->set = array_set_impl;
    arr->insert = array_insert_impl;
    arr->empty = array_empty_impl;
    arr->end = array_end_impl;
    arr->front = array_front_impl;
    arr->is_equal = array_is_equal_impl;
    arr->is_greater = array_is_greater_impl;
    arr->is_greater_or_equal = array_is_greater_or_equal_impl;
    arr->is_less = array_is_less_impl;
    arr->is_less_or_equal = array_is_less_or_equal_impl;
    arr->is_not_equal = array_is_not_equal_impl;
    arr->max_size = array_max_size_impl;
    arr->rbegin = array_rbegin_impl;
    arr->size = array_size_impl;
    arr->swap = array_swap_impl;

    return arr;
}

static bool array_is_equal_impl(const Array* arr1, const Array* arr2) 
{
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) 
        return false; 
    
    if (arr1->vec->size != arr2->vec->size) 
        return false;  
    
    return memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) == 0;
}

static bool array_is_less_impl(const Array* arr1, const Array* arr2) 
{
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) 
        return false; 
    
    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    return cmp < 0 || (cmp == 0 && arr1->vec->size < arr2->vec->size);
}

static bool array_is_greater_impl(const Array* arr1, const Array* arr2)
{
    return array_is_less_impl(arr2, arr1);
}

static bool array_is_not_equal_impl(const Array* arr1, const Array* arr2) 
{
    if (arr1 == NULL || arr2 == NULL) 
        return arr1 != arr2; // If either is NULL, they're not equal
    
    if (arr1->vec->size != arr2->vec->size) 
        return true;  // Different sizes mean they're not equal
    
    return memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) != 0;
}

static bool array_is_less_or_equal_impl(const Array* arr1, const Array* arr2) 
{
    if (arr1 == NULL || arr2 == NULL) 
        return (arr1 == NULL) && (arr2 != NULL); // Only true if arr1 is NULL and arr2 is not
    
    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    return cmp < 0 || (cmp == 0 && arr1->vec->size <= arr2->vec->size);
}

static bool array_is_greater_or_equal_impl(const Array* arr1, const Array* arr2) 
{
    if (arr1 == NULL || arr2 == NULL) 
        return (arr1 != NULL) && (arr2 == NULL); // Only true if arr1 is not NULL and arr2 is NULL

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    return cmp > 0 || (cmp == 0 && arr1->vec->size >= arr2->vec->size);
}

static bool array_empty_impl(Array* arr)
{
    return arr == NULL || arr->vec == NULL || arr->vec->size == 0;
}

static void array_deallocate_impl(Array* arr)
{
    if (arr != NULL) 
    {
        if (arr->vec != NULL) 
            arr->vec->deallocate(arr->vec);  

        free(arr);  // Free the array structure itself
    }
}

static void array_set_impl(Array* arr, size_t index, const void* value) 
{
    if (arr == NULL || arr->vec == NULL || index >= arr->vec->size || value == NULL) 
        return;  // Invalid input
    
    memcpy((char*)arr->vec->items + (index * arr->vec->itemSize), value, arr->vec->itemSize);
}

static void array_insert_impl(Array* mainArr, const Array* otherArr, size_t index) 
{
    if (mainArr == NULL || mainArr->vec == NULL || otherArr == NULL || otherArr->vec == NULL) 
        return;  // Invalid input
    
    size_t newTotalSize = index + otherArr->vec->size;
    if (newTotalSize > mainArr->vec->size) 
        mainArr->vec->resize(mainArr->vec, newTotalSize);
    
    for (size_t i = 0; i < otherArr->vec->size; ++i) 
    {
        void* value = otherArr->vec->at(otherArr->vec, i);
        memcpy((char*)mainArr->vec->items + ((index + i) * mainArr->vec->itemSize), value, mainArr->vec->itemSize);
    }
}


static void array_fill_impl(Array* arr, const void* value)
{
    if (arr == NULL || arr->vec == NULL || value == NULL) 
        return; 
    
    for (size_t i = 0; i < arr->vec->size; ++i) 
        memcpy((char*)arr->vec->items + (i * arr->vec->itemSize), value, arr->vec->itemSize);
}

static void array_swap_impl(Array* arr1, Array* arr2)
{
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) 
        return;  
    
    Vector* temp = arr1->vec;
    arr1->vec = arr2->vec;
    arr2->vec = temp;
}

static void* array_at_impl(Array* arr, size_t index)
{
    if (arr == NULL || arr->vec == NULL || index >= arr->vec->size) 
        return NULL;  
    
    return arr->vec->at(arr->vec, index);
}

static void* array_begin_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL; 
    
    return arr->vec->items;  
}

static void* array_end_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL;  
    
    return (char*)arr->vec->items + (arr->vec->size * arr->vec->itemSize);  
}

static void* array_rbegin_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL; 
    
    return (char*)arr->vec->items + ((arr->vec->size - 1) * arr->vec->itemSize);  
}

static void* array_rend_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL) 
        return NULL; 
    
    return (char*)arr->vec->items - arr->vec->itemSize; 
}

static void* array_front_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL; 
    
    return arr->vec->at(arr->vec, 0);  // First element
}

static void* array_back_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL;
    
    return arr->vec->at(arr->vec, arr->vec->size - 1);
}

static void* array_data_impl(Array* arr)
{
    return (arr != NULL && arr->vec != NULL) ? arr->vec->items : NULL;
}

static size_t array_size_impl(Array* arr)
{
    return (arr != NULL && arr->vec != NULL) ? arr->vec->size : 0;
}

static size_t array_max_size_impl(Array* arr)
{
    return (arr != NULL && arr->vec != NULL) ? arr->vec->max_size(arr->vec) : 0;
}

static const void* array_cbegin_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL; 
    
    return arr->vec->items; 
}

static const void* array_cend_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL;  
    
    return (const char*)arr->vec->items + (arr->vec->size * arr->vec->itemSize); 
}

static const void* array_crbegin_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) 
        return NULL;  
    
    return (const char*)arr->vec->items + ((arr->vec->size - 1) * arr->vec->itemSize); 
}

static const void* array_crend_impl(Array* arr)
{
    if (arr == NULL || arr->vec == NULL) 
        return NULL;  

    return (const char*)arr->vec->items - arr->vec->itemSize;  
}