/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Array
*/

#include "array.h"
#include <string.h>
#include <stdlib.h>

Array* array_create(size_t element_size, size_t size) {
    Array* arr = (Array*)malloc(sizeof(Array));

    if (!arr) {
        perror("Can not allocate memory for Array object");
        exit(-1);
    }

    arr->vec = vector_create(element_size);
    if (!arr->vec) {
        free(arr);
        return NULL;
    }
    vector_resize(arr->vec, size);

    return arr;
}

bool array_is_equal(const Array* arr1, const Array* arr2) {
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) { 
        return false; 
    }
    if (arr1->vec->size != arr2->vec->size) {
        return false;  
    }
    return memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) == 0;
}

bool array_is_less(const Array* arr1, const Array* arr2) {
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) { 
        return false; 
    }

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    return cmp < 0 || (cmp == 0 && arr1->vec->size < arr2->vec->size);
}

bool array_is_greater(const Array* arr1, const Array* arr2) {
    return array_is_less(arr2, arr1);
}

bool array_is_not_equal(const Array* arr1, const Array* arr2) {
    if (arr1 == NULL || arr2 == NULL) { 
        return arr1 != arr2; // If either is NULL, they're not equal
    }
    if (arr1->vec->size != arr2->vec->size) { 
        return true;  // Different sizes mean they're not equal
    }
    return memcmp(arr1->vec->items, arr2->vec->items, arr1->vec->size * arr1->vec->itemSize) != 0;
}

bool array_is_less_or_equal(const Array* arr1, const Array* arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        return (arr1 == NULL) && (arr2 != NULL); // Only true if arr1 is NULL and arr2 is not
    }

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    return cmp < 0 || (cmp == 0 && arr1->vec->size <= arr2->vec->size);
}

bool array_is_greater_or_equal(const Array* arr1, const Array* arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        return (arr1 != NULL) && (arr2 == NULL); // Only true if arr1 is not NULL and arr2 is NULL
    }

    size_t minSize = arr1->vec->size < arr2->vec->size ? arr1->vec->size : arr2->vec->size;
    int cmp = memcmp(arr1->vec->items, arr2->vec->items, minSize * arr1->vec->itemSize);

    return cmp > 0 || (cmp == 0 && arr1->vec->size >= arr2->vec->size);
}

bool array_empty(Array* arr) {
    return arr == NULL || arr->vec == NULL || arr->vec->size == 0;
}

void array_deallocate(Array* arr) {
    if (arr != NULL) {
        if (arr->vec != NULL) {
            vector_deallocate(arr->vec);  
        }
        free(arr);  // Free the array structure itself
    }
}

void array_set(Array* arr, size_t index, const void* value) {
    if (arr == NULL || arr->vec == NULL || index >= arr->vec->size || value == NULL) {
        return;  // Invalid input
    }
    memcpy((char*)arr->vec->items + (index * arr->vec->itemSize), value, arr->vec->itemSize);
}

void array_insert(Array* mainArr, const Array* otherArr, size_t index) {
    if (mainArr == NULL || mainArr->vec == NULL || otherArr == NULL || otherArr->vec == NULL) {
        return;  // Invalid input
    }

    size_t newTotalSize = index + otherArr->vec->size;
    if (newTotalSize > mainArr->vec->size) {
        vector_resize(mainArr->vec, newTotalSize);
    }

    for (size_t i = 0; i < otherArr->vec->size; ++i) {
        void* value = vector_at(otherArr->vec, i);
        memcpy((char*)mainArr->vec->items + ((index + i) * mainArr->vec->itemSize), value, mainArr->vec->itemSize);
    }
}

void array_fill(Array* arr, const void* value) {
    if (arr == NULL || arr->vec == NULL || value == NULL) { 
        return; 
    }
    for (size_t i = 0; i < arr->vec->size; ++i) { 
        memcpy((char*)arr->vec->items + (i * arr->vec->itemSize), value, arr->vec->itemSize);
    }
}

void array_swap(Array* arr1, Array* arr2) {
    if (arr1 == NULL || arr2 == NULL || arr1->vec == NULL || arr2->vec == NULL) {
        return;  
    }

    Vector* temp = arr1->vec;
    arr1->vec = arr2->vec;
    arr2->vec = temp;
}

void* array_at(Array* arr, size_t index) {
    if (arr == NULL || arr->vec == NULL || index >= arr->vec->size) { 
        return NULL;  
    }
    return vector_at(arr->vec, index);
}

void* array_begin(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) {
        return NULL; 
    }
    return arr->vec->items;  
}

void* array_end(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) {
        return NULL;  
    }
    return (char*)arr->vec->items + (arr->vec->size * arr->vec->itemSize);  
}

void* array_rbegin(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) { 
        return NULL; 
    }
    return (char*)arr->vec->items + ((arr->vec->size - 1) * arr->vec->itemSize);  
}

void* array_rend(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        return NULL; 
    }
    return (char*)arr->vec->items - arr->vec->itemSize; 
}

void* array_front(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) { 
        return NULL; 
    }
    return vector_at(arr->vec, 0);  // First element
}

void* array_back(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) { 
        return NULL;
    }
    return vector_at(arr->vec, arr->vec->size - 1);
}

void* array_data(Array* arr) {
    return (arr != NULL && arr->vec != NULL) ? arr->vec->items : NULL;
}

size_t array_size(Array* arr) {
    return (arr != NULL && arr->vec != NULL) ? arr->vec->size : 0;
}

size_t array_max_size(Array* arr) {
    return (arr != NULL && arr->vec != NULL) ? vector_max_size(arr->vec) : 0;
}

const void* array_cbegin(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) { 
        return NULL; 
    }
    return arr->vec->items; 
}

const void* array_cend(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) { 
        return NULL;  
    }
    return (const char*)arr->vec->items + (arr->vec->size * arr->vec->itemSize); 
}

const void* array_crbegin(Array* arr) {
    if (arr == NULL || arr->vec == NULL || arr->vec->size == 0) { 
        return NULL;  
    }
    return (const char*)arr->vec->items + ((arr->vec->size - 1) * arr->vec->itemSize); 
}

const void* array_crend(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        return NULL;  
    }
    return (const char*)arr->vec->items - arr->vec->itemSize;  
}

void array_clear(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        return;
    }
    vector_clear(arr->vec);
}

void array_reverse(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        return;
    }
    size_t start = 0;
    size_t end = arr->vec->size - 1;

    // Allocate memory for the temporary storage
    char* temp = (char*)malloc(arr->vec->itemSize);
    if (!temp) {
        return; // Check for allocation failure
    }

    while (start < end) {
        memcpy(temp, (char*)arr->vec->items + (start * arr->vec->itemSize), arr->vec->itemSize);
        memcpy((char*)arr->vec->items + (start * arr->vec->itemSize), (char*)arr->vec->items + (end * arr->vec->itemSize), arr->vec->itemSize);
        memcpy((char*)arr->vec->items + (end * arr->vec->itemSize), temp, arr->vec->itemSize);
        start++;
        end--;
    }
    free(temp);
}

void array_sort(Array* arr, int (*compare)(const void*, const void*)) {
    if (arr == NULL || arr->vec == NULL || compare == NULL) {
        return;
    }
    qsort(arr->vec->items, arr->vec->size, arr->vec->itemSize, compare);
}

void array_copy(Array* dest, const Array* src) {
    if (src == NULL || dest == NULL) {
        return;
    }
    vector_resize(dest->vec, src->vec->size);
    memcpy(dest->vec->items, src->vec->items, src->vec->size * src->vec->itemSize);
}
