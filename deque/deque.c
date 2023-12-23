#include "deque.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



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
    deque->itemSize = itemSize;

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

    return deque;
}

bool deque_empty(const Deque* deque) 
{
    if (!deque) 
        return true; 

    return deque->size == 0;
}

size_t deque_length(const Deque* deque) 
{
    if (!deque) 
        return 0; 

    return deque->size;
}

void deque_push_front(Deque* deque, void* item) 
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

void deque_push_back(Deque* deque, const void* item) 
{
    if (!deque || !item) 
        return;

    // Check if a new block is needed at the back
    if (deque->backIndex == deque->blockSize - 1) 
    {
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) 
            return;

        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount])
        {
            free(newBlocks);
            return;
        }

        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->backIndex = -1;
    }

    // Allocate memory for the new item and copy the item contents
    void* newItem = malloc(deque->itemSize);
    if (!newItem)
        return;

    memcpy(newItem, item, deque->itemSize);

    // Insert the new item at the back
    deque->backIndex++;
    deque->blocks[deque->blockCount - 1][deque->backIndex] = newItem;
    deque->size++;
}

void* deque_front(const Deque* deque) 
{
    if (!deque || deque_empty(deque)) 
        return NULL; // Handle empty or NULL deque

    // Calculate the block and index for the front element
    size_t blockIndex = deque->frontIndex / deque->blockSize;
    size_t indexInBlock = deque->frontIndex % deque->blockSize;

    return deque->blocks[blockIndex][indexInBlock];
}

void* deque_back(const Deque* deque) 
{
    if (!deque || deque_empty(deque)) 
        return NULL; // Handle empty or NULL deque

    // Calculate the block and index for the back element
    size_t blockIndex = deque->backIndex / deque->blockSize;
    size_t indexInBlock = deque->backIndex % deque->blockSize;

    return deque->blocks[blockIndex][indexInBlock];
}

void deque_pop_front(Deque* deque) 
{
    if (!deque) 
        return;
    
    if (deque_empty(deque)) 
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

void deque_pop_back(Deque* deque) {
    if (!deque || deque_empty(deque)) 
        return;

    // Free the memory of the item being popped
    free(deque->blocks[deque->blockCount - 1][deque->backIndex]);

    // Normal decrement
    if (deque->backIndex > 0) {
        deque->backIndex--;
    } else if (deque->blockCount > 1) {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
        deque->backIndex = deque->blockSize - 1;
    }

    deque->size--;
}


void* deque_at(const Deque* deque, size_t index) 
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

void deque_clear(Deque* deque) 
{
    if (!deque) 
        return;

    for (size_t i = 0; i < deque->blockCount; ++i) 
    {
        for (size_t j = 0; j < (i == deque->blockCount - 1 ? deque->backIndex + 1 : deque->blockSize); ++j) 
            free(deque->blocks[i][j]);  // Free each item
        
        free(deque->blocks[i]);  // Free each block
    }

    // Reallocate one block to reset the deque
    deque->blocks = (void***)realloc(deque->blocks, sizeof(void**));
    deque->blocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize);
    deque->blockCount = 1;
    deque->size = 0;
    deque->frontIndex = deque->blockSize / 2;
    deque->backIndex = deque->frontIndex - 1;
}


void deque_deallocate(Deque* deque) {
    if (!deque) 
        return;

    for (size_t i = 0; i < deque->blockCount; ++i) {
        for (size_t j = 0; j < (i == deque->blockCount - 1 ? deque->backIndex + 1 : deque->blockSize); ++j) {
            free(deque->blocks[i][j]);  // Free each item
        }
        free(deque->blocks[i]);  // Free each block
    }
    free(deque->blocks);  // Free the blocks array
    free(deque);  // Free the deque structure
}

void deque_shrink_to_fit(Deque* deque) 
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

void deque_insert(Deque* deque, size_t index, void* item) 
{
    if (!deque || index > deque->size || !item) 
        return; // Validate input

    if (index == deque->size) 
    {
        deque_push_back(deque, item);
        return;
    }

    if (index == 0) 
    {
        deque_push_front(deque, item);
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

void deque_erase(Deque* deque, size_t index) 
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

void deque_resize(Deque* deque, size_t newSize) 
{
    if (!deque) 
        return; // Validate input
    
    while (deque->size < newSize)  // Resize larger: add default-initialized elements to the back
    {
        void* defaultItem = NULL; // Default item (could be customized as needed)
        deque_push_back(deque, defaultItem);
    }
    
    while (deque->size > newSize)  // Resize smaller: remove elements from the back
        deque_pop_back(deque);

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

void deque_swap(Deque* deque, Deque* otherDeque) 
{
    if (!deque || !otherDeque) 
        return; // Validate input

    // Swap the contents of the two deques
    Deque temp = *deque;
    *deque = *otherDeque;
    *otherDeque = temp;
}

void deque_assign(Deque* deque, size_t n, void* val) 
{
    if (!deque) 
        return; // Validate input

    deque_clear(deque); // Clear the current contents of the deque

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

void deque_emplace_back(Deque* deque, void* item) 
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

void deque_emplace_front(Deque* deque, void* item) 
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

void deque_emplace(Deque* deque, size_t index, void* item) 
{
    if (!deque || index > deque->size || !item) 
        return;

    if (index == deque->size) 
    {
        deque_emplace_back(deque, item);
        return;
    }

    if (index == 0) 
    {
        deque_emplace_front(deque, item);
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

size_t deque_max_size(const Deque* deque) 
{
    if (!deque) 
        return 0; 

    return SIZE_MAX;
}

bool deque_is_equal(const Deque* deque1, const Deque* deque2) 
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
        if (deque_at(deque1, i) != deque_at(deque2, i)) 
            return false;
    }

    return true;
}

bool deque_is_less(const Deque* deque1, const Deque* deque2) 
{
    if (!deque1 || !deque2) 
        return false; // Invalid input

    for (size_t i = 0; i < deque1->size && i < deque2->size; ++i) 
    {
        if (deque_at(deque1, i) < deque_at(deque2, i)) 
            return true;
        if (deque_at(deque1, i) > deque_at(deque2, i)) 
            return false;
    }

    return deque1->size < deque2->size;
}

bool deque_is_greater(const Deque* deque1, const Deque* deque2)
{
    return deque_is_less(deque2, deque1);
}

bool deque_is_not_equal(const Deque* deque1, const Deque* deque2)
{
    return !deque_is_equal(deque1, deque2);
}

bool deque_is_less_or_equal(const Deque* deque1, const Deque* deque2) 
{
    return deque_is_less(deque1, deque2) || deque_is_equal(deque1, deque2);
}

bool deque_is_greater_or_equal(const Deque* deque1, const Deque* deque2) 
{
    return deque_is_greater(deque1, deque2) || deque_is_equal(deque1, deque2);
}

DequeIterator deque_begin(const Deque* deque) 
{
    DequeIterator it = {0};

    if (deque && deque->size > 0) 
    {
        it.deque = (Deque*)deque;
        it.current = deque->blocks[0][deque->frontIndex];
        it.blockIndex = deque->frontIndex / deque->blockSize;
        it.indexInBlock = deque->frontIndex % deque->blockSize;

        // // Set function pointers for iterator
        // it.increment = iterator_increment_impl;
        // it.decrement = iterator_decrement_impl;
        // it.equals = iterator_equals_impl;
        // it.get = iterator_get_impl;
        // it.isReverse = false;
    }

    return it;
}

DequeIterator deque_end(const Deque* deque) 
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

        // // Set function pointers for iterator
        // it.increment = iterator_increment_impl;
        // it.decrement = iterator_decrement_impl;
        // it.equals = iterator_equals_impl;
        // it.get = iterator_get_impl;
        // it.isReverse = false;
    }

    return it;
}

DequeIterator deque_rbegin(const Deque* deque) 
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

        // it.increment = iterator_increment_impl;
        // it.decrement = iterator_decrement_impl;
        // it.equals = iterator_equals_impl;
        // it.get = iterator_get_impl;
    }

    return it;
}

DequeIterator deque_rend(const Deque* deque) 
{
    DequeIterator it = {0};

    if (deque) 
    {
        it.deque = (Deque*)deque;
        it.current = NULL;
        it.blockIndex = 0;
        it.indexInBlock = SIZE_MAX; // Representing an invalid position
        it.isReverse = true; // Set isReverse to true for reverse iterator

        // it.increment = iterator_increment_impl;
        // it.decrement = iterator_decrement_impl;
        // it.equals = iterator_equals_impl;
        // it.get = iterator_get_impl;
    }

    return it;
}

const DequeIterator* deque_cbegin(const Deque* deque) 
{
    static DequeIterator temp_iterator; // static to ensure it persists after the function returns

    if (deque) {
        temp_iterator = deque_begin(deque); // Copy the iterator into the temporary variable
        return &temp_iterator;
    }
    return NULL;
}


const DequeIterator* deque_cend(const Deque* deque) 
{
    static DequeIterator temp_iterator;
    if (deque) {
        temp_iterator = deque_end(deque);
        return &temp_iterator;
    }
    return NULL;
}

const DequeIterator* deque_crbegin(const Deque* deque) 
{
    static DequeIterator temp_iterator;
    if (deque) 
    {
        temp_iterator = deque_rbegin(deque);
        return &temp_iterator;
    }
    return NULL;
}

const DequeIterator* deque_crend(const Deque* deque) 
{
    static DequeIterator temp_iterator;
    if (deque) 
    {
        temp_iterator = deque_rend(deque);
        return &temp_iterator;
    }
    return NULL;
}


void iterator_increment(DequeIterator* it) 
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

void iterator_decrement(DequeIterator* it) 
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

bool iterator_equals(const DequeIterator* it1, const DequeIterator* it2) 
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

void* iterator_get(const DequeIterator* it) 
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