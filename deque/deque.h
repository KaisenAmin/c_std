/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Deque
*/

#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdbool.h>
#include <stddef.h>

#define DEFAULT_BLOCK_SIZE 64

typedef struct Deque Deque;
typedef struct DequeIterator DequeIterator;

struct DequeIterator {
    Deque* deque; 
    bool isReverse;
    // Iterator state and functions
    void* current;         // Pointer to the current element
    size_t blockIndex;     // Current block index
    size_t indexInBlock;   // Index within the current block
};

struct Deque {
    size_t itemSize; 
    size_t blockSize;     // Size of each block
    size_t size;          // Total number of elements
    size_t blockCount;    // Number of blocks
    size_t frontIndex;    // Index of the front element
    size_t backIndex;     // Index of the back element
    void*** blocks;       // Pointer to blocks of elements
};

Deque* deque_create(size_t itemSize);
// Core Deque Functions

size_t deque_length(const Deque* deque);
size_t deque_max_size(const Deque* deque);

// Additional Deque Functions
void deque_push_front(Deque* deque, void* item);
void deque_push_back(Deque* deque, const void* item);
void deque_pop_front(Deque* deque);
void deque_pop_back(Deque* deque);
void deque_shrink_to_fit(Deque* deque);
void deque_insert(Deque* deque, size_t index, void* item);
void deque_erase(Deque* deque, size_t index);
void deque_resize(Deque* deque, size_t newSize);
void deque_swap(Deque* deque, Deque* otherDeque);
void deque_assign(Deque* deque, size_t n, void* val);
void deque_emplace_back(Deque* deque, void* item);
void deque_emplace_front(Deque* deque, void* item);
void deque_emplace(Deque* deque, size_t index, void* item);
void iterator_increment(DequeIterator* it);
void iterator_decrement(DequeIterator* it);
void deque_clear(Deque* deque);
void deque_deallocate(Deque* deque);

// Relational Operators
bool deque_is_equal(const Deque* deque1, const Deque* deque2);
bool deque_is_less(const Deque* deque1, const Deque* deque2);
bool deque_is_greater(const Deque* deque1, const Deque* deque2);
bool deque_is_not_equal(const Deque* deque1, const Deque* deque2);
bool deque_is_less_or_equal(const Deque* deque1, const Deque* deque2);
bool deque_is_greater_or_equal(const Deque* deque1, const Deque* deque2);
bool iterator_equals(const DequeIterator* it1, const DequeIterator* it2);
bool deque_empty(const Deque* deque);

// Iterator Functions
DequeIterator deque_begin(const Deque* deque);
DequeIterator deque_end(const Deque* deque);
DequeIterator deque_rbegin(const Deque* deque);
DequeIterator deque_rend(const Deque* deque);

const DequeIterator* deque_cbegin(const Deque* deque);
const DequeIterator* deque_cend(const Deque* deque);
const DequeIterator* deque_crbegin(const Deque* deque);
const DequeIterator* deque_crend(const Deque* deque);

void* deque_front(const Deque* deque);
void* deque_back(const Deque* deque);
void* iterator_get(const DequeIterator* it);
void* deque_at(const Deque* deque, size_t index);

#endif 