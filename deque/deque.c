#include "deque.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Core Deque Functions
static bool deque_empty_impl(const Deque* deque);
static size_t deque_length_impl(const Deque* deque);
static void deque_push_front_impl(Deque* deque, void* item);
static void deque_push_back_impl(Deque* deque, void* item);
static void* deque_front_impl(const Deque* deque);
static void* deque_back_impl(const Deque* deque);
static void deque_pop_front_impl(Deque* deque);
static void deque_pop_back_impl(Deque* deque);
static void* deque_at_impl(const Deque* deque, size_t index);
static void deque_clear_impl(Deque* deque);
static void deque_deallocate_impl(Deque* deque);

// Additional Deque Functions
static void deque_shrink_to_fit_impl(Deque* deque);
static void deque_insert_impl(Deque* deque, size_t index, void* item);
static void deque_erase_impl(Deque* deque, size_t index);
static void deque_resize_impl(Deque* deque, size_t newSize);
static void deque_swap_impl(Deque* deque, Deque* otherDeque);
static void deque_assign_impl(Deque* deque, size_t n, void* val);
static void deque_emplace_back_impl(Deque* deque, void* item);
static void deque_emplace_front_impl(Deque* deque, void* item);
static void deque_emplace_impl(Deque* deque, size_t index, void* item);
static size_t deque_max_size_impl(const Deque* deque);

// Relational Operators
static bool deque_is_equal_impl(const Deque* deque1, const Deque* deque2);
static bool deque_is_less_impl(const Deque* deque1, const Deque* deque2);
static bool deque_is_greater_impl(const Deque* deque1, const Deque* deque2);
static bool deque_is_not_equal_impl(const Deque* deque1, const Deque* deque2);
static bool deque_is_less_or_equal_impl(const Deque* deque1, const Deque* deque2);
static bool deque_is_greater_or_equal_impl(const Deque* deque1, const Deque* deque2);

// Iterator Functions
static DequeIterator deque_begin_impl(const Deque* deque);
static DequeIterator deque_end_impl(const Deque* deque);
static DequeIterator deque_rbegin_impl(const Deque* deque);
static DequeIterator deque_rend_impl(const Deque* deque);
static const DequeIterator deque_cbegin_impl(const Deque* deque);
static const DequeIterator deque_cend_impl(const Deque* deque);
static const DequeIterator deque_crbegin_impl(const Deque* deque);
static const DequeIterator deque_crend_impl(const Deque* deque);

// Iterator Operations
static void iterator_increment_impl(DequeIterator* it);
static void iterator_decrement_impl(DequeIterator* it);
static bool iterator_equals_impl(const DequeIterator* it1, const DequeIterator* it2);
static void* iterator_get_impl(const DequeIterator* it);


Deque* deque_create(size_t itemSize) 
{
    // Allocate memory for the Deque structure
    Deque* deque = (Deque*)malloc(sizeof(Deque));
    if (!deque) 
        return NULL; // Handle allocation failure

    // Initial values for the Deque structure
    deque->blockSize = DEFAULT_BLOCK_SIZE;
    deque->size = 0;
    deque->blockCount = 1; // Start with one block
    deque->frontIndex = DEFAULT_BLOCK_SIZE / 2; // Middle of the block
    deque->backIndex = deque->frontIndex - 1;
    
    // Allocate memory for the blocks array
    deque->blocks = (void***)malloc(sizeof(void**) * deque->blockCount);
    if (!deque->blocks) 
    {
        free(deque);
        return NULL; // Handle allocation failure
    }

    // Allocate memory for the first block
    deque->blocks[0] = (void**)malloc(sizeof(void*) * DEFAULT_BLOCK_SIZE);
    if (!deque->blocks[0]) 
    {
        free(deque->blocks);
        free(deque);
        return NULL; // Handle allocation failure
    }

    // Set function pointers
    deque->empty = deque_empty_impl;
    deque->length = deque_length_impl;
    deque->push_front = deque_push_front_impl;
    deque->push_back = deque_push_back_impl;
    deque->front = deque_front_impl;
    deque->back = deque_back_impl;
    deque->pop_front = deque_pop_front_impl;
    deque->pop_back = deque_pop_back_impl;
    deque->at = deque_at_impl;
    deque->clear = deque_clear_impl;
    deque->deallocate = deque_deallocate_impl;
    deque->shrink_to_fit = deque_shrink_to_fit_impl;
    deque->insert = deque_insert_impl;
    deque->erase = deque_erase_impl;
    deque->resize = deque_resize_impl;
    deque->swap = deque_swap_impl;
    deque->assign = deque_assign_impl;
    deque->emplace_back = deque_emplace_back_impl;
    deque->emplace_front = deque_emplace_front_impl;
    deque->emplace = deque_emplace_impl;
    deque->max_size = deque_max_size_impl;
    deque->is_equal = deque_is_equal_impl;
    deque->is_less = deque_is_less_impl;
    deque->is_greater = deque_is_greater_impl;
    deque->is_not_equal = deque_is_not_equal_impl;
    deque->is_less_or_equal = deque_is_less_or_equal_impl;
    deque->is_greater_or_equal = deque_is_greater_or_equal_impl;
    deque->begin = deque_begin_impl;
    deque->end = deque_end_impl;
    deque->rbegin = deque_rbegin_impl;
    deque->rend = deque_rend_impl;
    deque->cbegin = deque_cbegin_impl;
    deque->cend = deque_cend_impl;
    deque->crbegin = deque_crbegin_impl;
    deque->crend = deque_crend_impl;

    return deque;
}

static bool deque_empty_impl(const Deque* deque) 
{
    if (!deque) 
        return true; 

    return deque->size == 0;
}

static size_t deque_length_impl(const Deque* deque) 
{
    if (!deque) 
        return 0; 

    return deque->size;
}

static void deque_push_front_impl(Deque* deque, void* item) 
{
    if (!deque || !item) 
        return; 

    // Check if a new block is needed at the front
    if (deque->frontIndex == 0) 
    {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1)); // Allocate a new block at the front
        if (!newBlocks) 
            return; // Handle allocation failure

        memmove(newBlocks + 1, newBlocks, sizeof(void**) * deque->blockCount); // Shift existing blocks to the right

        newBlocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[0]) 
            return; // Handle allocation failure

        // Update deque properties
        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->frontIndex = deque->blockSize; // Reset front index to new block end
    }

    // Insert item at the front
    deque->frontIndex--;
    deque->blocks[0][deque->frontIndex] = item;
    deque->size++;
}

static void deque_push_back_impl(Deque* deque, void* item) 
{
    if (!deque || !item) 
        return; 

    // Check if a new block is needed at the back
    if (deque->backIndex == deque->blockSize - 1) 
    {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1)); 
        if (!newBlocks) 
            return; // Handle allocation failure

        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize); // Allocate the new back block
        if (!newBlocks[deque->blockCount]) 
            return; // Handle allocation failure

        // Update deque properties
        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->backIndex = -1; // Reset back index to new block start
    }

    // Insert item at the back
    deque->backIndex++;
    deque->blocks[deque->blockCount - 1][deque->backIndex] = item;
    deque->size++;

    // printf("Pushed %d to deque.\n", *(int*)item);
}

static void* deque_front_impl(const Deque* deque) 
{
    if (!deque || deque_empty_impl(deque)) 
        return NULL; // Handle empty or NULL deque

    // Calculate the block and index for the front element
    size_t blockIndex = deque->frontIndex / deque->blockSize;
    size_t indexInBlock = deque->frontIndex % deque->blockSize;

    return deque->blocks[blockIndex][indexInBlock];
}

static void* deque_back_impl(const Deque* deque) 
{
    if (!deque || deque_empty_impl(deque)) 
        return NULL; // Handle empty or NULL deque

    // Calculate the block and index for the back element
    size_t blockIndex = deque->backIndex / deque->blockSize;
    size_t indexInBlock = deque->backIndex % deque->blockSize;

    return deque->blocks[blockIndex][indexInBlock];
}

static void deque_pop_front_impl(Deque* deque) 
{
    if (!deque) 
        return;
    
    if (deque_empty_impl(deque)) 
        return;

    deque->frontIndex++;

    if (deque->frontIndex / deque->blockSize > 0) 
    {
        free(deque->blocks[0]);

        memmove(deque->blocks, deque->blocks + 1, sizeof(void**) * (deque->blockCount - 1));
        deque->blockCount--;
        deque->frontIndex -= deque->blockSize;
    }

    deque->size--;
}

static void deque_pop_back_impl(Deque* deque) 
{
    if (!deque || deque_empty_impl(deque)) 
        return; // Handle empty or NULL deque

    deque->size--;
    deque->backIndex--;

    // Adjusting the backIndex when crossing block boundaries
    if (deque->backIndex < 0 && deque->blockCount > 1) 
    {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
        deque->backIndex = deque->blockSize - 1; // Set backIndex to the end of the new last block
    }
}

static void* deque_at_impl(const Deque* deque, size_t index) 
{
    if (!deque || index >= deque->size) 
        return NULL; // Validate input

    // Calculating the block and the index within the block
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;
    size_t indexInBlock = (deque->frontIndex + index) % deque->blockSize;

    if (blockIndex >= deque->blockCount)  // Adjusting the calculation for blockIndex when crossing block boundaries
    {
        blockIndex = deque->blockCount - 1;
        indexInBlock = deque->backIndex - ((deque->size - 1) - index);
    }

    return deque->blocks[blockIndex][indexInBlock];
}

static void deque_clear_impl(Deque* deque) 
{
    if (!deque) 
        return; 

    for (size_t i = 0; i < deque->blockCount; ++i)  // Iterate over each block and free it
        free(deque->blocks[i]);
    
    free(deque->blocks);  // Reset the block array and deque properties

    deque->blocks = (void***)malloc(sizeof(void**) * 1); // Allocate one block
    deque->blocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize); // Allocate the first block
    deque->blockCount = 1;
    deque->size = 0;
    deque->frontIndex = deque->blockSize / 2;
    deque->backIndex = deque->frontIndex - 1;

}

static void deque_deallocate_impl(Deque* deque) 
{
    if (!deque) 
        return; // Validate input

    for (size_t i = 0; i < deque->blockCount; ++i)   // Iterate over each block and free it
        free(deque->blocks[i]);
    
    // Free the block array and the deque itself
    free(deque->blocks);
    free(deque);
}

static void deque_shrink_to_fit_impl(Deque* deque) 
{
    if (!deque || deque->size == 0) 
        return; // Validate input or handle empty deque

    // Calculate the optimal number of blocks needed
    size_t optimalBlockCount = (deque->size / deque->blockSize) + ((deque->size % deque->blockSize) ? 1 : 0);

    // If the current block count is greater than the optimal, reduce it
    if (deque->blockCount > optimalBlockCount) 
    {
        // Free the excess blocks
        for (size_t i = optimalBlockCount; i < deque->blockCount; ++i) 
            free(deque->blocks[i]);
        
        // Attempt to reallocate the blocks array to the new size
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * optimalBlockCount);
        if (newBlocks) 
        {
            deque->blocks = newBlocks;
            deque->blockCount = optimalBlockCount;
        }
        // If realloc fails, the excess blocks have already been freed,
        // and the remaining blocks are still valid. The deque continues to function
        // with potentially unused allocated space in the blocks array.
    }
}

static void deque_insert_impl(Deque* deque, size_t index, void* item) 
{
    if (!deque || index > deque->size || !item) 
        return; // Validate input

    if (index == deque->size) 
    {
        deque_push_back_impl(deque, item);
        return;
    }

    if (index == 0) 
    {
        deque_push_front_impl(deque, item);
        return;
    }

    size_t actualIndex = (deque->frontIndex + index) % deque->blockSize;
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;

    if (deque->size == deque->blockSize * deque->blockCount) 
    {
        // Reallocate to add a new block
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) return; // Allocation failure

        // Initialize the new block at the end
        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) return; // Allocation failure

        deque->blocks = newBlocks;
        deque->blockCount++;
    }

    // Shift elements to make space for the new item
    // Starting from the last element, shift elements to the right
    for (size_t i = deque->size; i > index; --i) 
    {
        size_t srcBlockIndex = (deque->frontIndex + i - 1) / deque->blockSize;
        size_t srcIndexInBlock = (deque->frontIndex + i - 1) % deque->blockSize;
        size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
        size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

        deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
    }

    // Insert the item at the specified index
    deque->blocks[blockIndex][actualIndex] = item;
    deque->size++;
}

static void deque_erase_impl(Deque* deque, size_t index) 
{
    if (!deque) 
        return;
    
    if (index >= deque->size) 
        return;
    
    for (size_t i = index; i < deque->size - 1; ++i) 
    {
        size_t srcBlockIndex = (deque->frontIndex + i + 1) / deque->blockSize;
        size_t srcIndexInBlock = (deque->frontIndex + i + 1) % deque->blockSize;
        size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
        size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

        deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
    }

    deque->size--;

    if (deque->size % deque->blockSize == 0 && deque->blockCount > 1) 
    {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
    }
}

static void deque_resize_impl(Deque* deque, size_t newSize) 
{
    if (!deque) 
        return; // Validate input
    
    while (deque->size < newSize)  // Resize larger: add default-initialized elements to the back
    {
        void* defaultItem = NULL; // Default item (could be customized as needed)
        deque_push_back_impl(deque, defaultItem);
    }
    
    while (deque->size > newSize)  // Resize smaller: remove elements from the back
        deque_pop_back_impl(deque);

    // Optimize memory usage if the new size is much smaller than the current size
    // This could involve reallocating the blocks array to a smaller size
    size_t optimalBlockCount = (newSize / deque->blockSize) + ((newSize % deque->blockSize) ? 1 : 0);
    if (optimalBlockCount < deque->blockCount) 
    {
        for (size_t i = optimalBlockCount; i < deque->blockCount; ++i) 
            free(deque->blocks[i]);
        
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * optimalBlockCount);
        if (newBlocks) 
        { // If realloc fails, retain the original blocks array
            deque->blocks = newBlocks;
            deque->blockCount = optimalBlockCount;
        }
    }
}

static void deque_swap_impl(Deque* deque, Deque* otherDeque) 
{
    if (!deque || !otherDeque) 
        return; // Validate input

    // Swap the contents of the two deques
    Deque temp = *deque;
    *deque = *otherDeque;
    *otherDeque = temp;
}

static void deque_assign_impl(Deque* deque, size_t n, void* val) 
{
    if (!deque) 
        return; // Validate input

    deque_clear_impl(deque); // Clear the current contents of the deque

    // Resize the deque to have 'n' elements
    for (size_t i = 0; i < n; ++i) {
        // If expanding beyond current block count, allocate new blocks as needed
        if (deque->size == deque->blockSize * deque->blockCount) 
        {
            void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
            if (!newBlocks) 
                return; // Handle allocation failure

            newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
            if (!newBlocks[deque->blockCount]) 
                return; // Handle allocation failure

            deque->blocks = newBlocks;
            deque->blockCount++;
        }

        // Calculate the block index and position within the block
        size_t blockIndex = deque->size / deque->blockSize;
        size_t indexInBlock = deque->size % deque->blockSize;

        // Assign the value
        deque->blocks[blockIndex][indexInBlock] = val;

        // Increase the size
        deque->size++;
    }
}

static void deque_emplace_back_impl(Deque* deque, void* item) 
{
    if (!deque || !item) 
        return;

    if (deque->backIndex == deque->blockSize - 1)  // Check if a new block is needed at the back
    {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) return;

        newBlocks[deque->blockCount] = malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) 
            return;

        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->backIndex = -1;
    }

    deque->backIndex++;
    size_t blockIndex = deque->backIndex / deque->blockSize;
    size_t indexInBlock = deque->backIndex % deque->blockSize;
    deque->blocks[blockIndex][indexInBlock] = item;
    deque->size++;
}

static void deque_emplace_front_impl(Deque* deque, void* item) 
{
    if (!deque || !item) 
        return;

    if (deque->frontIndex == 0)  // Check if a new block is needed at the front
    {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) 
            return;

        memmove(newBlocks + 1, newBlocks, sizeof(void**) * deque->blockCount);

        newBlocks[0] = malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[0]) 
            return;

        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->frontIndex = deque->blockSize;
    }

    deque->frontIndex--;
    size_t blockIndex = deque->frontIndex / deque->blockSize;
    size_t indexInBlock = deque->frontIndex % deque->blockSize;
    deque->blocks[blockIndex][indexInBlock] = item;
    deque->size++;
}

static void deque_emplace_impl(Deque* deque, size_t index, void* item) 
{
    if (!deque || index > deque->size || !item) 
        return;

    if (index == deque->size) 
    {
        deque_emplace_back_impl(deque, item);
        return;
    }

    if (index == 0) 
    {
        deque_emplace_front_impl(deque, item);
        return;
    }

    // Check if a new block is needed
    if (deque->size == deque->blockSize * deque->blockCount) 
    {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) 
            return;

        newBlocks[deque->blockCount] = malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) 
            return;

        deque->blocks = newBlocks;
        deque->blockCount++;
    }

    // Shift elements to the right to make space for the new item
    for (size_t i = deque->size; i > index; --i) 
    {
        size_t srcBlockIndex = (deque->frontIndex + i - 1) / deque->blockSize;
        size_t srcIndexInBlock = (deque->frontIndex + i - 1) % deque->blockSize;
        size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
        size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

        deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
    }

    // Insert the new item
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;
    size_t indexInBlock = (deque->frontIndex + index) % deque->blockSize;
    deque->blocks[blockIndex][indexInBlock] = item;
    deque->size++;
}

static size_t deque_max_size_impl(const Deque* deque) 
{
    if (!deque) 
        return 0; 

    return SIZE_MAX;
}

static bool deque_is_equal_impl(const Deque* deque1, const Deque* deque2) 
{
    if (deque1 == deque2) 
        return true; // Same deque or both NULL
    if (!deque1 || !deque2) 
        return false; // One is NULL, the other is not
    if (deque1->size != deque2->size) 
        return false; // Different sizes

    // Compare elements
    for (size_t i = 0; i < deque1->size; ++i) 
    {
        if (deque_at_impl(deque1, i) != deque_at_impl(deque2, i)) 
            return false;
    }

    return true;
}

static bool deque_is_less_impl(const Deque* deque1, const Deque* deque2) 
{
    if (!deque1 || !deque2) 
        return false; // Invalid input

    for (size_t i = 0; i < deque1->size && i < deque2->size; ++i) 
    {
        if (deque_at_impl(deque1, i) < deque_at_impl(deque2, i)) 
            return true;
        if (deque_at_impl(deque1, i) > deque_at_impl(deque2, i)) 
            return false;
    }

    return deque1->size < deque2->size;
}

static bool deque_is_greater_impl(const Deque* deque1, const Deque* deque2)
{
    return deque_is_less_impl(deque2, deque1);
}

static bool deque_is_not_equal_impl(const Deque* deque1, const Deque* deque2)
{
    return !deque_is_equal_impl(deque1, deque2);
}

static bool deque_is_less_or_equal_impl(const Deque* deque1, const Deque* deque2) 
{
    return deque_is_less_impl(deque1, deque2) || deque_is_equal_impl(deque1, deque2);
}

static bool deque_is_greater_or_equal_impl(const Deque* deque1, const Deque* deque2) 
{
    return deque_is_greater_impl(deque1, deque2) || deque_is_equal_impl(deque1, deque2);
}

static DequeIterator deque_begin_impl(const Deque* deque) 
{
    DequeIterator it = {0};

    if (deque && deque->size > 0) 
    {
        it.deque = (Deque*)deque;
        it.current = deque->blocks[0][deque->frontIndex];
        it.blockIndex = deque->frontIndex / deque->blockSize;
        it.indexInBlock = deque->frontIndex % deque->blockSize;

        // Set function pointers for iterator
        it.increment = iterator_increment_impl;
        it.decrement = iterator_decrement_impl;
        it.equals = iterator_equals_impl;
        it.get = iterator_get_impl;
        it.isReverse = false;
    }

    return it;
}

static DequeIterator deque_end_impl(const Deque* deque) 
{
    DequeIterator it = {0};

    if (deque) 
    {
        it.deque = (Deque*)deque;
        it.current = NULL;  // End iterator is past the last element

        // Calculate blockIndex and indexInBlock for the position just past the last element
        size_t totalElements = deque->frontIndex + deque->size;
        it.blockIndex = totalElements / deque->blockSize;
        it.indexInBlock = totalElements % deque->blockSize;

        // Set function pointers for iterator
        it.increment = iterator_increment_impl;
        it.decrement = iterator_decrement_impl;
        it.equals = iterator_equals_impl;
        it.get = iterator_get_impl;
        it.isReverse = false;
    }

    return it;
}

static DequeIterator deque_rbegin_impl(const Deque* deque) 
{
    DequeIterator it = {0};

    if (deque && deque->size > 0) 
    {
        it.deque = (Deque*)deque;
        size_t lastBlock = (deque->frontIndex + deque->size - 1) / deque->blockSize;
        size_t indexInLastBlock = (deque->frontIndex + deque->size - 1) % deque->blockSize;

        it.current = deque->blocks[lastBlock][indexInLastBlock];
        it.blockIndex = lastBlock;
        it.indexInBlock = indexInLastBlock;
        it.isReverse = true; // Set isReverse to true for reverse iterator

        it.increment = iterator_increment_impl;
        it.decrement = iterator_decrement_impl;
        it.equals = iterator_equals_impl;
        it.get = iterator_get_impl;
    }

    return it;
}

static DequeIterator deque_rend_impl(const Deque* deque) 
{
    DequeIterator it = {0};

    if (deque) 
    {
        it.deque = (Deque*)deque;
        it.current = NULL;
        it.blockIndex = 0;
        it.indexInBlock = SIZE_MAX; // Representing an invalid position
        it.isReverse = true; // Set isReverse to true for reverse iterator

        it.increment = iterator_increment_impl;
        it.decrement = iterator_decrement_impl;
        it.equals = iterator_equals_impl;
        it.get = iterator_get_impl;
    }

    return it;
}

static const DequeIterator deque_cbegin_impl(const Deque* deque) 
{
    return deque_begin_impl(deque);
}

static const DequeIterator deque_cend_impl(const Deque* deque) 
{
    return deque_end_impl(deque);
}

static const DequeIterator deque_crbegin_impl(const Deque* deque) 
{
    return deque_rbegin_impl(deque);
}

static const DequeIterator deque_crend_impl(const Deque* deque) 
{
    return deque_rend_impl(deque);
}

static void iterator_increment_impl(DequeIterator* it) 
{
    if (!it || !it->deque) 
        return;

    if (it->isReverse) 
    {
        if (it->indexInBlock == 0)  // Handle reverse iteration
        {
            if (it->blockIndex > 0) 
            {
                it->blockIndex--;
                it->indexInBlock = it->deque->blockSize - 1;
            } 
            else 
                it->current = NULL; // Reached the beginning of the deque
        }
        else 
            it->indexInBlock--;
    
        if (it->blockIndex < it->deque->blockCount && it->indexInBlock != SIZE_MAX) 
            it->current = it->deque->blocks[it->blockIndex][it->indexInBlock];
        else 
            it->current = NULL;
    } 
    else 
    {
        
        it->indexInBlock++; // Handle forward iteration
        if (it->indexInBlock >= it->deque->blockSize) 
        {
            it->blockIndex++;
            it->indexInBlock = 0;
        }

        if (it->blockIndex < it->deque->blockCount) 
            it->current = it->deque->blocks[it->blockIndex][it->indexInBlock];
        else 
            it->current = NULL;
        
    }
}

static void iterator_decrement_impl(DequeIterator* it) 
{
    if (!it || !it->deque || it->deque->size == 0) 
        return;

    if (it->isReverse) 
    {
        if (it->indexInBlock < it->deque->blockSize - 1) // Handle decrement for reverse iterator
            it->indexInBlock++;
        else 
        {
            if (it->blockIndex < it->deque->blockCount - 1) 
            {
                it->blockIndex++;
                it->indexInBlock = 0;
            } 
            else 
            {
                it->current = NULL; // Reached the end of the deque
                return;
            }
        }
    } 
    else 
    {
        if (it->indexInBlock == 0) // Handle decrement for forward iterator
        {
            if (it->blockIndex > 0) 
            {
                it->blockIndex--;
                it->indexInBlock = it->deque->blockSize - 1;
            } 
            else 
            {
                it->current = NULL; // Reached the beginning of the deque
                return;
            }
        } 
        else 
            it->indexInBlock--;
        
    }

    it->current = it->deque->blocks[it->blockIndex][it->indexInBlock]; // Update the current pointer
}

static bool iterator_equals_impl(const DequeIterator* it1, const DequeIterator* it2) 
{
    if (!it1 || !it2) 
        return false;
    
    if (it1->current == NULL || it2->current == NULL) // Check if either iterator is at the end (or rend) position
    {
        bool isEqual = it1->current == it2->current;
        return isEqual;
    }

    // Standard comparison for other cases
    bool isEqual = it1->blockIndex == it2->blockIndex && it1->indexInBlock == it2->indexInBlock;

    return isEqual;
}

static void* iterator_get_impl(const DequeIterator* it) 
{
    if (!it || !it->deque) 
        return NULL;

    if (it->isReverse) 
    {
        if (it->blockIndex < it->deque->blockCount && it->indexInBlock != SIZE_MAX) // Handle reverse iteration
            return it->deque->blocks[it->blockIndex][it->indexInBlock];
        else 
            return NULL;
    } 
    else 
    {
        if (it->blockIndex < it->deque->blockCount) // Handle forward iteration
            return it->deque->blocks[it->blockIndex][it->indexInBlock];
        else 
            return NULL;
    }
}