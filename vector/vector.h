
/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Vector
*/

#ifndef VECTOR_H_
#define VECTOR_H_

#define VECTOR_LOGGING_ENABLE

#include <stdbool.h>
#include <stddef.h>


typedef struct Vector Vector;

typedef struct MemoryPoolVector {
    void *pool;        // Pointer to the memory pool
    size_t poolSize;   // Total size of the pool
    size_t used;       // Memory used so far
} MemoryPoolVector;

struct Vector {
    void* items;
    size_t size;
    size_t capacitySize;
    size_t itemSize;
    MemoryPoolVector *pool;
};

Vector* vector_create(size_t itemSize);

bool vector_is_equal(const Vector* vec1, const Vector* vec2);
bool vector_is_less(const Vector* vec1, const Vector* vec2);
bool vector_is_greater(const Vector* vec1, const Vector* vec2);
bool vector_is_not_equal(const Vector* vec1, const Vector* vec2);
bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2);
bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2);
bool vector_is_empty(Vector* vec);
bool vector_reserve(Vector* vec, size_t size);
bool vector_push_back(Vector* vec, const void* item);
bool vector_emplace_back(Vector *vec, void *item, size_t itemSize);

void vector_erase(Vector* vec, size_t pos, size_t len);
void vector_insert(Vector* vec, size_t pos, void* item);
void vector_resize(Vector* vec, size_t size);
void vector_shrink_to_fit(Vector* vec);
void vector_clear(Vector* vec);
void vector_swap(Vector* vec1, Vector* vec2);
void vector_assign(Vector* vec, size_t pos, void* item);
void vector_emplace(Vector* vec, size_t pos, void* item, size_t itemSize);
void vector_deallocate(Vector *vec);

const void* vector_cbegin(Vector* vec);
const void* vector_cend(Vector* vec);
const void* vector_crbegin(Vector* vec);
const void* vector_crend(Vector* vec);

void* vector_begin(Vector* vec);
void* vector_end(Vector* vec);
void* vector_pop_back(Vector* vec);
void* vector_front(Vector* vec);
void* vector_back(Vector* vec);
void* vector_data(Vector* vec);
void* vector_at(const Vector* vec, size_t pos);
void* vector_rbegin(Vector* vec);
void* vector_rend(Vector* vec);

size_t vector_size(const Vector* vec);
size_t vector_capacity(Vector* vec);
size_t vector_max_size(Vector* vec);

#endif 