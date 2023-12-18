#include "vector.h"
#include <stdlib.h>
#include <string.h>

static bool vector_is_equal_impl(const Vector* vec1, const Vector* vec2);
static bool vector_is_less_impl(const Vector* vec1, const Vector* vec2);
static bool vector_is_greater_impl(const Vector* vec1, const Vector* vec2);
static bool vector_is_not_equal_impl(const Vector* vec1, const Vector* vec2);
static bool vector_is_greater_or_equal_impl(const Vector* vec1, const Vector* vec2);
static bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2);
static bool vector_is_empty_impl(Vector* vec);
static void vector_erase_impl(Vector* vec, size_t pos, size_t len);
static void vector_insert_impl(Vector* vec, size_t pos, void* item);
static void vector_reserve_impl(Vector* vec, size_t size);
static void vector_resize_impl(Vector* vec, size_t size);
static void vector_shrink_to_fit_impl(Vector* vec);
static void vector_clear_impl(Vector* vec);
static void vector_swap_impl(Vector* vec1, Vector* vec2);
static void vector_assign_impl(Vector* vec, size_t pos, void* item);
static void vector_emplace_impl(Vector* vec, size_t pos, void* item, size_t itemSize);
static void vector_emplace_back_impl(Vector *vec, void *item, size_t itemSize);
static void vector_push_back_impl(Vector* vec, void* item);
static void vector_deallocate_impl(Vector *vec);
static void* vector_at_impl(Vector* vec, size_t pos);
static void* vector_rbegin_impl(Vector* vec);
static void* vector_rend_impl(Vector* vec);
static const void* vector_cbegin_impl(Vector* vec);
static const void* vector_cend_impl(Vector* vec);
static const void* vector_crbegin_impl(Vector* vec);
static const void* vector_crend_impl(Vector* vec);
static void* vector_begin_impl(Vector* vec);
static void* vector_end_impl(Vector* vec);
static void* vector_pop_back_impl(Vector* vec);
static void* vector_front_impl(Vector* vec);
static void* vector_back_impl(Vector* vec);
static void* vector_data_impl(Vector* vec);
static size_t vector_size_impl(Vector* vec);
static size_t vector_capacity_impl(Vector* vec);
static size_t vector_max_size_impl(Vector* vec);

Vector* vector_create(size_t itemSize) 
{
    Vector* vec = (Vector*)malloc(sizeof(Vector));

    if (!vec) 
        return NULL;
    
    vec->size = 0;
    vec->capacitySize = 32; // Initial capacity
    vec->itemSize = itemSize;
    vec->items = malloc(vec->capacitySize * itemSize);

    if (!vec->items) 
    {
        free(vec);
        return NULL; 
    }

    vec->is_equal = vector_is_equal_impl;
    vec->is_less = vector_is_less_impl;
    vec->is_greater = vector_is_greater_impl;
    vec->is_greater_or_equal = vector_is_greater_or_equal_impl;
    vec->is_less_or_equal = vector_is_less_or_equal;
    vec->is_not_equal = vector_is_not_equal_impl;
    vec->is_empty = vector_is_empty_impl;
    vec->erase = vector_erase_impl;
    vec->insert = vector_insert_impl;
    vec->reserve = vector_reserve_impl;
    vec->resize = vector_resize_impl;
    vec->shrink_to_fit = vector_shrink_to_fit_impl;
    vec->swap = vector_swap_impl;
    vec->assign = vector_assign_impl;
    vec->emplace = vector_emplace_impl;
    vec->emplace_back = vector_emplace_back_impl;
    vec->push_back = vector_push_back_impl;
    vec->deallocate = vector_deallocate_impl;
    vec->at = vector_at_impl;
    vec->rbegin = vector_rbegin_impl;
    vec->rend = vector_rend_impl;
    vec->cbegin = vector_cbegin_impl;
    vec->cend = vector_cend_impl;
    vec->crbegin = vector_crbegin_impl;
    vec->crend = vector_crend_impl;
    vec->begin = vector_begin_impl;
    vec->end = vector_end_impl;
    vec->pop_back = vector_pop_back_impl;
    vec->clear = vector_clear_impl;
    vec->front = vector_front_impl;
    vec->back = vector_back_impl;
    vec->data = vector_data_impl;
    vec->length = vector_size_impl;
    vec->capacity = vector_capacity_impl;
    vec->max_size = vector_max_size_impl;

    return vec;
}

static bool vector_is_equal_impl(const Vector* vec1, const Vector* vec2) 
{
    if (vec1 == NULL || vec2 == NULL || vec1->size != vec2->size) 
        return false;

    return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) == 0;
}

static bool vector_is_less_impl(const Vector* vec1, const Vector* vec2) 
{
    if (!vec1 || !vec2) 
        return false;

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    return cmp < 0 || (cmp == 0 && vec1->size < vec2->size);
}

static bool vector_is_greater_impl(const Vector* vec1, const Vector* vec2) 
{
    if (!vec1 || !vec2) 
        return false;

    size_t minSize = vec1->size < vec2->size ? vec1->size : vec2->size;
    int cmp = memcmp(vec1->items, vec2->items, minSize * vec1->itemSize);

    return cmp > 0 || (cmp == 0 && vec1->size > vec2->size);
}

static bool vector_is_not_equal_impl(const Vector* vec1, const Vector* vec2) 
{
    if (!vec1 || !vec2) 
        return true;
    if (vec1->size != vec2->size) 
        return true;

    return memcmp(vec1->items, vec2->items, vec1->size * vec1->itemSize) != 0;
}

static bool vector_is_greater_or_equal_impl(const Vector* vec1, const Vector* vec2) 
{
    return !vector_is_less_impl(vec1, vec2);
}

static bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2) 
{
    return !vector_is_greater_impl(vec1, vec2);
}

static bool vector_is_empty_impl(Vector *vec) 
{
    if (vec == NULL) 
    {
        fprintf(stderr, "Error: Vector is NULL in vector_is_empty_impl.\n");
        return true; // or handle the error as per your application's needs
    }

    return vec->size == 0;
}

static void vector_erase_impl(Vector *vec, size_t pos, size_t len)
{
    if (vec == NULL || pos >= vec->size || pos + len > vec->size) 
        return; // Vector is NULL, position is out of bounds, or len is too large
    

    char *base = (char *)vec->items;

    memmove(base + pos * vec->itemSize, 
            base + (pos + len) * vec->itemSize, 
            (vec->size - pos - len) * vec->itemSize);

    vec->size -= len;
}

static void vector_insert_impl(Vector *vec, size_t pos, void *item)
{
    if (vec == NULL || pos > vec->size) 
        return;
    
    if (vec->size == vec->capacitySize) 
        vector_reserve_impl(vec, vec->capacitySize * 2); // Double the capacity
    
    char *base = (char *)vec->items;
    memmove(base + (pos + 1) * vec->itemSize, 
            base + pos * vec->itemSize, 
            (vec->size - pos) * vec->itemSize);

    memcpy(base + pos * vec->itemSize, item, vec->itemSize);

    vec->size++;
}

static void vector_reserve_impl(Vector *vec, size_t size)
{
    if (vec == NULL || vec->capacitySize >= size) 
        return; // No need to reserve if the capacity is already sufficient
    
    void *newItems = realloc(vec->items, size * vec->itemSize);
    if (newItems == NULL) 
        return;

    vec->items = newItems;
    vec->capacitySize = size;
}

static void vector_resize_impl(Vector *vec, size_t size) 
{
    if (vec == NULL) 
        return; // Vector is NULL
     
    if (size > vec->capacitySize) 
        vector_reserve_impl(vec, size); // Resize capacity if new size exceeds current capacity
    
    if (vec->size < size) 
        // Initialize new elements to 0 if size is increased
        memset((char *)vec->items + vec->size * vec->itemSize, 0, (size - vec->size) * vec->itemSize);
    
    vec->size = size;
}


static void vector_shrink_to_fit_impl(Vector *vec) 
{
    if (vec == NULL || vec->size == vec->capacitySize) 
        return;
    
    if (vec->size == 0) 
    {
        free(vec->items); // If the vector is empty, free the memory
        vec->items = NULL;
        vec->capacitySize = 0;

        return;
    }

    void *newItems = realloc(vec->items, vec->size * vec->itemSize);  // Reallocate the memory to match the size
    
    if (newItems == NULL) 
        return;
    
    vec->items = newItems;
    vec->capacitySize = vec->size;
}

static void vector_swap_impl(Vector *vec1, Vector *vec2) 
{
    if (vec1 == NULL || vec2 == NULL)
     {
        // One or both vectors are NULL
        fprintf(stderr, "Error: One or both vectors are NULL in vector_swap_impl.\n");
        return;
    }

    void *tempItems = vec1->items;
    vec1->items = vec2->items;
    vec2->items = tempItems;

    size_t tempSize = vec1->size;
    vec1->size = vec2->size;
    vec2->size = tempSize;

    size_t tempCapacity = vec1->capacitySize;
    vec1->capacitySize = vec2->capacitySize;
    vec2->capacitySize = tempCapacity;

    size_t tempItemSize = vec1->itemSize;
    vec1->itemSize = vec2->itemSize;
    vec2->itemSize = tempItemSize;
}

static void vector_assign_impl(Vector *vec, size_t pos, void *item) 
{
    if (vec == NULL || pos >= vec->size)
        return;
    
    memcpy((char *)vec->items + pos * vec->itemSize, item, vec->itemSize);
}

static void vector_emplace_impl(Vector *vec, size_t pos, void *item, size_t itemSize) 
{
    if (vec == NULL || pos > vec->size || itemSize != vec->itemSize) 
        return;
    
    if (vec->size == vec->capacitySize) 
        vector_reserve_impl(vec, vec->capacitySize * 2); // Double the capacity

    char *base = (char *)vec->items;

    memmove(base + (pos + 1) * vec->itemSize, 
            base + pos * vec->itemSize, 
            (vec->size - pos) * vec->itemSize);

    memcpy(base + pos * vec->itemSize, item, vec->itemSize);

    vec->size++;
}

static void vector_emplace_back_impl(Vector *vec, void *item, size_t itemSize) 
{
    if (vec == NULL || itemSize != vec->itemSize) 
        return;
    
    if (vec->size >= vec->capacitySize) 
        vector_reserve_impl(vec, vec->capacitySize * 2); // Double the capacity
    
    memcpy((char *)vec->items + vec->size * vec->itemSize, item, vec->itemSize);
    vec->size++;
}

static void vector_push_back_impl(Vector *vec, void *item) 
{
    if (vec->size >= vec->capacitySize) 
    {
        // Implement a more gradual growth strategy (1.5x)
        size_t newCapacity = vec->capacitySize + (vec->capacitySize * 2);
        if (newCapacity < vec->capacitySize + 1) {
            newCapacity = vec->capacitySize + 1; // Ensure at least one more item can fit
        }

        void *newItems = realloc(vec->items, newCapacity * vec->itemSize);
        if (!newItems) 
        {
            perror("Cannot push back new item");
            return;
        }

        vec->items = newItems;
        vec->capacitySize = newCapacity;
    }

    // Copy the item into the vector
    memcpy((char *)vec->items + (vec->size * vec->itemSize), item, vec->itemSize);
    vec->size++;
}

static void vector_deallocate_impl(Vector *vec) 
{
    if (vec == NULL) 
        return;  
    
    if (vec->items != NULL) 
    {
        free(vec->items);
        vec->items = NULL;  // Set to NULL to prevent use-after-free errors
        free(vec);
    }
}

static void *vector_at_impl(Vector *vec, size_t pos)
{
    if (pos < vec->size) 
        return (char *)vec->items + (pos * vec->itemSize); // Calculate the address of the item at position 'pos'
    else 
        return NULL; // Position is out of bounds
}

static void *vector_rbegin_impl(Vector *vec)
{
    if (vec == NULL || vec->size == 0)
        return NULL;
    
    return (void *)((char *)vec->items + (vec->size - 1) * vec->itemSize); // Last element
}

static void *vector_rend_impl(Vector *vec)
{
    if (vec == NULL) 
        return NULL;
    
    return (void *)((char *)vec->items - vec->itemSize); // One before the first element
}

static const void *vector_cbegin_impl(Vector *vec)
{
    if (vec == NULL || vec->size == 0)
        return NULL;
    
    return (const void *)vec->items;
}

static const void *vector_cend_impl(Vector *vec)
{
    if (vec == NULL || vec->size == 0) 
        return NULL;

    // One past the last element, as a read-only pointer
    return (const void *)((char *)vec->items + (vec->size * vec->itemSize));
}

static const void *vector_crbegin_impl(Vector *vec)
{
    if (vec == NULL || vec->size == 0) 
        return NULL;

    return (const void *)((char *)vec->items + (vec->size - 1) * vec->itemSize); // Last element, as a read-only pointer
}

static const void *vector_crend_impl(Vector *vec)
{
    if (vec == NULL) 
        return NULL;
    
    return (const void *)((char *)vec->items - vec->itemSize); // One before the first element, as a read-only pointer
}

static void *vector_begin_impl(Vector *vec) 
{
    if (vec == NULL || vec->size == 0) 
        return NULL;

    return vec->items; // Pointer to the first element
}

static void *vector_end_impl(Vector *vec)
{
    if (vec == NULL || vec->size == 0) 
        return NULL;
    
    return (char *)vec->items + (vec->size * vec->itemSize); // One past the last element
}

static void *vector_pop_back_impl(Vector *vec)
{
    if (vec == NULL || vec->size == 0) 
        return NULL;

    vec->size--;

    return (char *)vec->items + (vec->size * vec->itemSize);
}

static void vector_clear_impl(Vector *vec) 
{
    if (vec == NULL) 
    {
        fprintf(stderr, "Error: Vector is NULL in vector_clear_impl.\n");
        return;
    }

    vec->size = 0;
    size_t initialCapacity = 4;
    void *newItems = realloc(vec->items, initialCapacity * vec->itemSize);

    if (newItems != NULL || initialCapacity == 0) 
    {
        vec->items = newItems;
        vec->capacitySize = initialCapacity;
    } 
    else 
        fprintf(stderr, "Cannot reallocate the Vector\n");
    
}

static void *vector_front_impl(Vector *vec) 
{
    if (vec == NULL || vec->size == 0) 
        return NULL;
    
    return vec->items; // The first element is at the beginning of the items array
}

static void *vector_back_impl(Vector *vec) 
{
    if (vec == NULL || vec->size == 0) 
        return NULL;
    
    // The last element is at (size - 1) * itemSize offset from the beginning
    return (char *)vec->items + (vec->size - 1) * vec->itemSize;
}

static void *vector_data_impl(Vector *vec) 
{
    if (vec == NULL) 
        return NULL; 
    
    return vec->items; // The underlying array
}

size_t vector_size_impl(Vector *vec)
{
    return vec->size;
}

size_t vector_capacity_impl(Vector *vec)
{
    return vec->capacitySize;
}

size_t vector_max_size_impl(Vector *vec)
{
    return vec->itemSize;
}
