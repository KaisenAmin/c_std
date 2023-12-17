#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdbool.h>
#include <stddef.h>

#define DEFAULT_BLOCK_SIZE 64

typedef struct Deque Deque;
typedef struct DequeIterator DequeIterator;

struct DequeIterator 
{
    Deque* deque; 
    bool isReverse;
    // Iterator state and functions
    void* current;         // Pointer to the current element
    size_t blockIndex;     // Current block index
    size_t indexInBlock;   // Index within the current block

    // Iterator operations
    void (*increment)(DequeIterator* it);   // Move to next element
    void (*decrement)(DequeIterator* it);   // Move to previous element
    bool (*equals)(const DequeIterator* it1, const DequeIterator* it2); // Compare iterators
    void* (*get)(const DequeIterator* it);  // Get current element
};

struct Deque 
{
    size_t blockSize;     // Size of each block
    size_t size;          // Total number of elements
    size_t blockCount;    // Number of blocks
    size_t frontIndex;    // Index of the front element
    size_t backIndex;     // Index of the back element
    void*** blocks;       // Pointer to blocks of elements

    // Function pointers for basic operations
    bool (*empty)(const Deque* deque);
    size_t (*length)(const Deque* deque);
    void (*push_front)(Deque* deque, void* item);
    void (*push_back)(Deque* deque, void* item);
    void* (*front)(const Deque* deque);
    void* (*back)(const Deque* deque);
    void (*pop_front)(Deque* deque);
    void (*pop_back)(Deque* deque);
    void* (*at)(const Deque* deque, size_t index);
    void (*clear)(Deque* deque);
    void (*deallocate)(Deque* deque);

    // Function pointers for additional operations
    void (*shrink_to_fit)(Deque* deque);
    void (*insert)(Deque* deque, size_t index, void* item);
    void (*erase)(Deque* deque, size_t index);
    void (*resize)(Deque* deque, size_t newSize);
    void (*swap)(Deque* deque, Deque* otherDeque);
    void (*assign)(Deque* deque, size_t n, void* val);
    void (*emplace_back)(Deque* deque, void* item);
    void (*emplace_front)(Deque* deque, void* item);
    void (*emplace)(Deque* deque, size_t index, void* item);
    size_t (*max_size)(const Deque* deque);
    // Function pointers for relational operators
    bool (*is_equal)(const Deque* deque1, const Deque* deque2);
    bool (*is_less)(const Deque* deque1, const Deque* deque2);
    bool (*is_greater)(const Deque* deque1, const Deque* deque2);
    bool (*is_not_equal)(const Deque* deque1, const Deque* deque2);
    bool (*is_less_or_equal)(const Deque* deque1, const Deque* deque2);
    bool (*is_greater_or_equal)(const Deque* deque1, const Deque* deque2);

    // Iterator related operations
    DequeIterator (*begin)(const Deque* deque);
    DequeIterator (*end)(const Deque* deque);
    DequeIterator (*rbegin)(const Deque* deque);
    DequeIterator (*rend)(const Deque* deque);
    const DequeIterator (*cbegin)(const Deque* deque);
    const DequeIterator (*cend)(const Deque* deque);
    const DequeIterator (*crbegin)(const Deque* deque);
    const DequeIterator (*crend)(const Deque* deque);
};

Deque* deque_create(size_t itemSize);


#endif 