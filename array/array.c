/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Array
*/

#include "array.h"
#include "../fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

Array* array_create(size_t element_size, size_t size) {
    Array* arr = (Array*)malloc(sizeof(Array));

    if (!arr) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for Array object in array_create\n");
        exit(-1);
    }

    arr->vec = vector_create(element_size);
    if (!arr->vec) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for internal Vector in array_create\n");
        free(arr);
        exit(-1);
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
    if (arr1 == NULL && arr2 == NULL) {
        // Both arrays are NULL, which means they are equal
        return true;
    }
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
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_set.\n");
        return;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Array's vector is NULL in array_set.\n");
        return;
    }
    if (index >= arr->vec->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in array_set.\n");
        return;
    }
    if (value == NULL) {
        fmt_fprintf(stderr, "Error: Value is NULL in array_set.\n");
        return;
    }

    memcpy((char*)arr->vec->items + (index * arr->vec->itemSize), value, arr->vec->itemSize);
}

void array_insert(Array* mainArr, const Array* otherArr, size_t index) {
    if (mainArr == NULL) {
        fmt_fprintf(stderr, "Error: Main array is NULL in array_insert.\n");
        return;
    }
    if (mainArr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Main array's vector is NULL in array_insert.\n");
        return;
    }
    if (otherArr == NULL) {
        fmt_fprintf(stderr, "Error: Other array is NULL in array_insert.\n");
        return;
    }
    if (otherArr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Other array's vector is NULL in array_insert.\n");
        return;
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
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_fill.\n");
        return;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Array's vector is NULL in array_fill.\n");
        return;
    }
    if (value == NULL) {
        fmt_fprintf(stderr, "Error: 'value' is NULL in array_fill.\n");
        return;
    }

    for (size_t i = 0; i < arr->vec->size; ++i) { 
        memcpy((char*)arr->vec->items + (i * arr->vec->itemSize), value, arr->vec->itemSize);
    }
}

void array_swap(Array* arr1, Array* arr2) {
    if (arr1 == NULL) {
        fmt_fprintf(stderr, "Error: arr1 is NULL in array_swap.\n");
        return;
    }
    if (arr2 == NULL) {
        fmt_fprintf(stderr, "Error: arr2 is NULL in array_swap.\n");
        return;
    }
    if (arr1->vec == NULL) {
        fmt_fprintf(stderr, "Error: arr1's vector is NULL in array_swap.\n");
        return;
    }
    if (arr2->vec == NULL) {
        fmt_fprintf(stderr, "Error: arr2's vector is NULL in array_swap.\n");
        return;
    }

    Vector* temp = arr1->vec;
    arr1->vec = arr2->vec;
    arr2->vec = temp;
}

void* array_at(Array* arr, size_t index) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_at.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Array's vector is NULL in array_at.\n");
        return NULL;
    }
    if (index >= arr->vec->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in array_at.\n");
        return NULL;
    }
    return vector_at(arr->vec, index);
}

void* array_begin(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_begin.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_begin.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Array is empty in array_begin.\n");
        return NULL;
    }
    return arr->vec->items;
}

void* array_end(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_end.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_end.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Array is empty in array_end.\n");
        return NULL;
    }
    return vector_end(arr->vec);
}

void* array_rbegin(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_rbegin.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_rbegin.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Array is empty in array_rbegin.\n");
        return NULL;
    }
    return vector_rbegin(arr->vec);
}

void* array_rend(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_rend.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_rend.\n");
        return NULL;
    }
    return vector_rend(arr->vec);
}

void* array_front(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_front.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_front.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Array is empty in array_front.\n");
        return NULL;
    }
    return vector_at(arr->vec, 0);  // First element
}

void* array_back(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_back.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_back.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Array is empty in array_back.\n");
        return NULL;
    }
    return vector_at(arr->vec, arr->vec->size - 1);
}

void* array_data(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_data.\n");
        return NULL;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_data.\n");
        return NULL;
    }
    return arr->vec->items;
}

size_t array_size(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_size.\n");
        return 0;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_size.\n");
        return 0;
    }
    return arr->vec->size;
}

size_t array_max_size(Array* arr) {
    if (arr == NULL) {
        fmt_fprintf(stderr, "Error: Array is NULL in array_max_size.\n");
        return 0;
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_max_size.\n");
        return 0;
    }
    return vector_max_size(arr->vec);
}

const void* array_cbegin(Array* arr) {
    if (arr == NULL) { 
        fmt_fprintf(stderr, "Error: Array is NULL in array_cbegin.\n");
        return NULL; 
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_cbegin.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Vector size is 0 in array_cbegin.\n");
        return NULL;
    }
    // return arr->vec->items;
    return vector_cbegin(arr->vec); 
}

const void* array_cend(Array* arr) {
    if (arr == NULL) { 
        fmt_fprintf(stderr, "Error: Array is NULL in array_cend.\n");
        return NULL;  
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_cend.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Vector size is 0 in array_cend.\n");
        return NULL;
    }
    // return (const char*)arr->vec->items + (arr->vec->size * arr->vec->itemSize); 
    return vector_cend(arr->vec);
}

const void* array_crbegin(Array* arr) {
    if (arr == NULL) { 
        fmt_fprintf(stderr, "Error: Array is NULL in array_crbegin.\n");
        return NULL;  
    }
    if (arr->vec == NULL) {
        fmt_fprintf(stderr, "Error: Vector is NULL in array_crbegin.\n");
        return NULL;
    }
    if (arr->vec->size == 0) {
        fmt_fprintf(stderr, "Error: Vector size is 0 in array_crbegin.\n");
        return NULL;
    }
    // return (const char*)arr->vec->items + ((arr->vec->size - 1) * arr->vec->itemSize); 
    return vector_crbegin(arr->vec);
}

const void* array_crend(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input in array_crend.\n");
        return NULL;  
    }
    // return (const char*)arr->vec->items - arr->vec->itemSize;  
    return vector_crend(arr->vec);
}

void array_clear(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input in array_clear.\n");
        return;
    }
    vector_clear(arr->vec);
}

void array_reverse(Array* arr) {
    if (arr == NULL || arr->vec == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input in array_reverse.\n");
        return;
    }
    if (arr->vec->size <= 1) {
        // Nothing to reverse for an empty or single-element array
        return;
    }
    size_t start = 0;
    size_t end = arr->vec->size - 1;

    // Allocate memory for the temporary storage
    char* temp = (char*)malloc(arr->vec->itemSize);
    if (!temp) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in array_reverse.\n");
        return; // Exit the function if memory allocation fails
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
        fmt_fprintf(stderr, "Error: Invalid input in array_sort.\n");
        return;
    }
    if (arr->vec->size <= 1) {
        // Nothing to sort for an empty or single-element array
        return;
    }
    qsort(arr->vec->items, arr->vec->size, arr->vec->itemSize, compare);
}

void array_copy(Array* dest, const Array* src) {
    if (src == NULL || dest == NULL) {
        fmt_fprintf(stderr, "Error: One or both of them are null in array_copy.\n");
        return;
    }
    vector_resize(dest->vec, src->vec->size);
    memcpy(dest->vec->items, src->vec->items, src->vec->size * src->vec->itemSize);
}
