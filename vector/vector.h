#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct Vector Vector;

struct Vector 
{
    void* items;
    size_t size;
    size_t capacitySize;
    size_t itemSize;

    bool (*is_empty)(Vector* vec);
    bool (*is_equal)(const Vector* vec1, const Vector* vec2);
    bool (*is_less)(const Vector* vec1, const Vector* vec2);
    bool (*is_greater)(const Vector* vec1, const Vector* vec2);
    bool (*is_not_equal)(const Vector* vec1, const Vector* vec2);
    bool (*is_greater_or_equal)(const Vector* vec1, const Vector* vec2);
    bool (*is_less_or_equal)(const Vector* vec1, const Vector* vec2);
    void (*erase)(Vector* vec, size_t pos, size_t len);
    void (*insert)(Vector* vec, size_t pos, void* item);
    void (*reserve)(Vector* vec, size_t size);
    void (*resize)(Vector* vec, size_t size);
    void (*shrink_to_fit)(Vector* vec);
    void (*swap)(Vector* vec1, Vector* vec2);
    void (*assign)(Vector* vec, size_t pos, void* item);
    void (*emplace)(Vector* vec, size_t pos, void* item, size_t sizeItem);
    void (*emplace_back)(Vector* vec, void*, size_t);
    void (*push_back)(Vector*, void*);
    void (*clear)(Vector* vec);
    void (*deallocate)(Vector* vec);
    void* (*at)(Vector* vec, size_t pos);
    void* (*rbegin)(Vector* vec);
    void* (*rend)(Vector* vec);
    const void* (*cbegin)(Vector* vec);
    const void* (*cend)(Vector* vec);
    const void* (*crbegin)(Vector* vec);
    const void* (*crend)(Vector* vec);
    void* (*begin)(Vector* vec);
    void* (*end)(Vector* vec);
    void* (*pop_back)(Vector* vec);
    void* (*front)(Vector* vec);
    void* (*back)(Vector* vec);
    void* (*data)(Vector* vec);
    size_t (*length)(Vector* vec);
    size_t (*capacity)(Vector* vec);
    size_t (*max_size)(Vector* vec);
};

Vector* vector_create(size_t itemSize);

#endif 