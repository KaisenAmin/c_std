/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Deque
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "deque.h"


/**
 * @brief Creates a new deque with a specified item size.
 *
 * This function initializes a new deque with the given item size. It allocates memory for the deque structure and
 * sets up the initial block of memory for storing elements. The front and back indices are initialized to the middle
 * of the block to allow for efficient push operations at both ends.
 *
 * @param itemSize The size of each item in the deque. Must be greater than 0.
 * @return Pointer to the newly created deque.
 * @warning The function will exit the program if memory allocation fails or if itemSize is zero.
 */
Deque* deque_create(size_t itemSize) {
    if (itemSize == 0) {
        DEQUE_LOG("[deque_create] Error: itemSize cannot be zero.");
        return NULL;
    }

    // Allocate memory for the Deque structure
    Deque* deque = (Deque*)malloc(sizeof(Deque)); 
    if (!deque) {
        DEQUE_LOG("[deque_create] Error: Memory allocation failed for Deque structure.");
        return NULL; 
    }

    // Initial values for the Deque structure
    deque->blockSize = DEFAULT_BLOCK_SIZE;
    deque->size = 0;
    deque->blockCount = 1; // Start with one block
    deque->frontIndex = DEFAULT_BLOCK_SIZE / 2; // Middle of the block
    deque->backIndex = deque->frontIndex - 1;
    deque->itemSize = itemSize;

    // Allocate memory for the blocks array
    deque->blocks = (void***)malloc(sizeof(void**) * deque->blockCount);
    if (!deque->blocks) {
        DEQUE_LOG("[deque_create] Error: Memory allocation failed for blocks array.");
        free(deque);
        return NULL; 
    }

    // Allocate memory for the first block
    deque->blocks[0] = (void**)malloc(sizeof(void*) * DEFAULT_BLOCK_SIZE);
    if (!deque->blocks[0]) {
        DEQUE_LOG("[deque_create] Error: Memory allocation failed for the first block.");
        free(deque->blocks);
        free(deque);
        return NULL; 
    }

    DEQUE_LOG("[deque_create] Deque created successfully with blockSize: %zu, frontIndex: %zu, backIndex: %zu.", deque->blockSize, deque->frontIndex, deque->backIndex);
    return deque;
}

/**
 * @brief Checks if the deque is empty.
 *
 * This function returns true if the deque is empty (i.e., contains no elements) or if the deque is NULL.
 *
 * @param deque Pointer to the deque.
 * @return True if the deque is empty or NULL, otherwise false.
 */
bool deque_empty(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_empty] Error: Deque is NULL.");
        return true; 
    }
    
    bool isEmpty = deque->size == 0;
    DEQUE_LOG("[deque_empty] Deque is %s.", isEmpty ? "empty" : "not empty");
    
    return isEmpty;
}

/**
 * @brief Returns the number of elements in the deque.
 *
 * This function returns the current number of elements stored in the deque. If the deque is NULL,
 * the function returns 0.
 *
 * @param deque Pointer to the deque.
 * @return The number of elements in the deque, or 0 if the deque is NULL.
 */
size_t deque_length(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_length] Error: Deque is NULL.");
        return 0; 
    }

    DEQUE_LOG("[deque_length] Deque size: %zu.", deque->size);
    return deque->size;
}

/**
 * @brief Inserts an item at the front of the deque.
 *
 * This function inserts a new item at the front of the deque. If necessary, it allocates a new block
 * at the front to accommodate the new item. If the deque or item is NULL, the function prints an error
 * message and returns without making any changes.
 *
 * @param deque Pointer to the deque.
 * @param item Pointer to the item to be inserted.
 */
void deque_push_front(Deque* deque, void* item) {
    if (!deque) {
        DEQUE_LOG("[deque_push_front] Error: Deque is NULL.");
        return; 
    }
    if (!item) {
        DEQUE_LOG("[deque_push_front] Error: Item is NULL.");
        return; 
    }
    // Check if a new block is needed at the front
    if (deque->frontIndex == 0) {
        DEQUE_LOG("[deque_push_front] Allocating new block at the front.");
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1)); // Allocate a new block at the front
        if (!newBlocks) {
            DEQUE_LOG("[deque_push_front] Error: Memory allocation failed.");
            return; 
        }
        memmove(newBlocks + 1, newBlocks, sizeof(void**) * deque->blockCount); // Shift existing blocks to the right

        newBlocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[0]) { 
            DEQUE_LOG("[deque_push_front] Error: Memory allocation failed for the new block.");
            return; 
        }
        // Update deque properties
        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->frontIndex = deque->blockSize; 
        DEQUE_LOG("[deque_push_front] New block allocated. blockCount: %zu.", deque->blockCount);
    }
    // Insert item at the front
    deque->frontIndex--;
    deque->blocks[0][deque->frontIndex] = item;
    deque->size++;
    DEQUE_LOG("[deque_push_front] Item inserted at the front. New frontIndex: %zu, size: %zu.", deque->frontIndex, deque->size);
}

/**
 * @brief Inserts an item at the back of the deque.
 *
 * This function inserts a new item at the back of the deque. If necessary, it allocates a new block
 * at the back to accommodate the new item. If the deque or item is NULL, the function prints an error
 * message and returns without making any changes.
 *
 * @param deque Pointer to the deque.
 * @param item Pointer to the item to be inserted.
 */
void deque_push_back(Deque* deque, const void* item) {
    if (!deque) {
        DEQUE_LOG("[deque_push_back] Error: Deque is NULL.");
        return; 
    }
    if (!item) {
        DEQUE_LOG("[deque_push_back] Error: Item is NULL.");
        return; 
    }

    // Check if a new block is needed at the back
    if (deque->backIndex == deque->blockSize - 1) {
        DEQUE_LOG("[deque_push_back] Allocating new block at the back.");
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            DEQUE_LOG("[deque_push_back] Error: Memory allocation failed.");
            return;
        }
        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) {
            DEQUE_LOG("[deque_push_back] Error: Memory allocation failed for the new block.");
            free(newBlocks);
            return;
        }
        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->backIndex = -1;
        DEQUE_LOG("[deque_push_back] New block allocated. blockCount: %zu.", deque->blockCount);
    }

    // Allocate memory for the new item and copy the item contents
    void* newItem = malloc(deque->itemSize);
    if (!newItem) {
        DEQUE_LOG("[deque_push_back] Error: Memory allocation failed for new item.");
        return;
    }
    memcpy(newItem, item, deque->itemSize);

    // Insert the new item at the back
    deque->backIndex++;
    deque->blocks[deque->blockCount - 1][deque->backIndex] = newItem;
    deque->size++;
    DEQUE_LOG("[deque_push_back] Item inserted at the back. New backIndex: %zu, size: %zu.", deque->backIndex, deque->size);
}

/**
 * @brief Returns a pointer to the front element of the deque.
 *
 * This function retrieves the front element of the deque without removing it.
 * If the deque is empty or NULL, an error message is printed and NULL is returned.
 *
 * @param deque Pointer to the deque.
 * @return Pointer to the front element, or NULL if the deque is empty or NULL.
 */
void* deque_front(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_front] Error: Deque is NULL.");
        return NULL; 
    }
    if (deque_empty(deque)) {
        DEQUE_LOG("[deque_front] Error: Deque is empty.");
        return NULL; 
    }

    // Calculate the block and index for the front element
    size_t blockIndex = deque->frontIndex / deque->blockSize;
    size_t indexInBlock = deque->frontIndex % deque->blockSize;

    DEQUE_LOG("[deque_front] Returning front element from blockIndex: %zu, indexInBlock: %zu.", blockIndex, indexInBlock);
    return deque->blocks[blockIndex][indexInBlock];
}


/**
 * @brief Returns a pointer to the back element of the deque.
 *
 * This function retrieves the back element of the deque without removing it.
 * If the deque is empty or NULL, an error message is printed and NULL is returned.
 *
 * @param deque Pointer to the deque.
 * @return Pointer to the back element, or NULL if the deque is empty or NULL.
 */
void* deque_back(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_back] Error: Deque is NULL.");
        return NULL; 
    }
    if (deque_empty(deque)) { 
        DEQUE_LOG("[deque_back] Error: Deque is empty.");
        return NULL; 
    }

    // Calculate the block and index for the back element
    size_t blockIndex = deque->backIndex / deque->blockSize;
    size_t indexInBlock = deque->backIndex % deque->blockSize;

    DEQUE_LOG("[deque_back] Returning back element from blockIndex: %zu, indexInBlock: %zu.", blockIndex, indexInBlock);
    return deque->blocks[blockIndex][indexInBlock];
}

/**
 * @brief Removes the front element from the deque.
 *
 * This function removes the front element from the deque. 
 * If the deque becomes empty, the front index is adjusted and 
 * the memory for the blocks is managed accordingly.
 * 
 * @param deque Pointer to the deque.
 */
void deque_pop_front(Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_pop_front] Error: Deque is NULL.");
        return; 
    }
    if (deque_empty(deque)) {
        DEQUE_LOG("[deque_pop_front] Error: Deque is empty.");
        return; 
    }

    DEQUE_LOG("[deque_pop_front] Popping front element. Current frontIndex: %zu.", deque->frontIndex);
    deque->frontIndex++;

    if (deque->frontIndex / deque->blockSize > 0) {
        free(deque->blocks[0]);
        memmove(deque->blocks, deque->blocks + 1, sizeof(void**) * (deque->blockCount - 1));
        deque->blockCount--;
        deque->frontIndex -= deque->blockSize;

        DEQUE_LOG("[deque_pop_front] Removed first block. New blockCount: %zu.", deque->blockCount);
    }

    deque->size--;
    DEQUE_LOG("[deque_pop_front] Front element popped. New size: %zu.", deque->size);
}

/**
 * @brief Removes the back element from the deque.
 *
 * This function removes the back element from the deque.
 * If the deque becomes empty, the back index is adjusted and
 * the memory for the blocks is managed accordingly.
 * 
 * @param deque Pointer to the deque.
 */
void deque_pop_back(Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_pop_back] Error: Deque is NULL.");
        return; 
    }
    if (deque_empty(deque)) {
        DEQUE_LOG("[deque_pop_back] Error: Deque is empty.");
        return; 
    }

    DEQUE_LOG("[deque_pop_back] Popping back element. Current backIndex: %zu.", deque->backIndex);
    free(deque->blocks[deque->blockCount - 1][deque->backIndex]);

    // Normal decrement
    if (deque->backIndex > 0) {
        deque->backIndex--;
    } 
    else if (deque->blockCount > 1) {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
        deque->backIndex = deque->blockSize - 1;

        DEQUE_LOG("[deque_pop_back] Removed last block. New blockCount: %zu.", deque->blockCount);
    }

    deque->size--;
    DEQUE_LOG("[deque_pop_back] Back element popped. New size: %zu.", deque->size);
}

/**
 * @brief Returns a pointer to the element at the specified index in the deque.
 *
 * This function retrieves the element at the specified index in the deque.
 * If the index is out of bounds or the deque is NULL, an error message is printed and NULL is returned.
 *
 * @param deque Pointer to the deque.
 * @param index The index of the element to retrieve.
 * @return Pointer to the element at the specified index, or NULL if the index is out of bounds or the deque is NULL.
 */
void* deque_at(const Deque* deque, size_t index) {
    if (!deque) {
        DEQUE_LOG("[deque_at] Error: Deque is NULL.");
        return NULL; 
    }
    if (index >= deque->size) {
        DEQUE_LOG("[deque_at] Error: Index out of bounds. index: %zu, deque size: %zu", index, deque->size);
        return NULL; 
    }

    // Calculate the block and index within the block
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;
    size_t indexInBlock = (deque->frontIndex + index) % deque->blockSize;

    // Adjusting the calculation for blockIndex when crossing block boundaries
    if (blockIndex >= deque->blockCount) {
        blockIndex = deque->blockCount - 1;
        indexInBlock = deque->backIndex - ((deque->size - 1) - index);
    }

    DEQUE_LOG("[deque_at] Returning element at blockIndex: %zu, indexInBlock: %zu.", blockIndex, indexInBlock);
    return deque->blocks[blockIndex][indexInBlock];
}

/**
 * @brief Clears all elements from the deque.
 *
 * This function removes all elements from the deque and frees the associated memory.
 * After clearing, the deque is reset to its initial state with one allocated block.
 *
 * @param deque Pointer to the deque.
 */
void deque_clear(Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_clear] Error: Deque is NULL.");
        return;
    }

    DEQUE_LOG("[deque_clear] Clearing the deque. Current size: %zu, blockCount: %zu.", deque->size, deque->blockCount);

    for (size_t i = 0; i < deque->blockCount; ++i) {
        size_t startIndex = (i == 0) ? deque->frontIndex : 0;
        size_t endIndex = (i == deque->blockCount - 1) ? deque->backIndex + 1 : deque->blockSize;

        for (size_t j = startIndex; j < endIndex; ++j) {
            if (deque->blocks[i][j] != NULL) {
                DEQUE_LOG("[deque_clear] Freeing element at block %zu, index %zu.", i, j);
                free(deque->blocks[i][j]);  // Free the element
                deque->blocks[i][j] = NULL;  // Mark as freed
            }
        }
        free(deque->blocks[i]);  // Free the block
        deque->blocks[i] = NULL;
        DEQUE_LOG("[deque_clear] Freed block at index: %zu.", i);
    }

    // Free the blocks array and reset metadata
    free(deque->blocks);
    deque->blocks = NULL;
    deque->blockCount = 0;
    deque->size = 0;
    deque->frontIndex = 0;
    deque->backIndex = -1;

    DEQUE_LOG("[deque_clear] Deque cleared and reset.");
}



/**
 * @brief Deallocates all memory associated with the deque.
 *
 * This function frees all elements and blocks associated with the deque,
 * and then deallocates the deque structure itself.
 *
 * @param deque Pointer to the deque.
 */
void deque_deallocate(Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_deallocate] Error: Deque is NULL.");
        return;
    }
    DEQUE_LOG("[deque_deallocate] Deallocating deque.");

    // First clear the deque to free all elements and blocks
    deque_clear(deque);

    // Then free the deque structure itself
    free(deque);
    DEQUE_LOG("[deque_deallocate] Deque deallocated.");
}

/**
 * @brief Shrinks the deque's memory usage to fit its current size.
 *
 * This function reduces the memory used by the deque to match its current size.
 * It deallocates any excess memory blocks that are no longer needed, keeping only 
 * the blocks that are required to store the current elements in the deque.
 *
 * @param deque Pointer to the deque.
 */
void deque_shrink_to_fit(Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_shrink_to_fit] Error: Deque is NULL.");
        return; 
    }
    if (deque->size == 0) {
        DEQUE_LOG("[deque_shrink_to_fit] Deque is empty, no need to shrink.");
        return; 
    }

    // Calculate the optimal number of blocks needed
    size_t optimalBlockCount = (deque->size / deque->blockSize) + ((deque->size % deque->blockSize) ? 1 : 0);
    DEQUE_LOG("[deque_shrink_to_fit] Shrinking deque. Current blockCount: %zu, optimalBlockCount: %zu.", deque->blockCount, optimalBlockCount);

    // If the current block count is greater than the optimal, reduce it
    if (deque->blockCount > optimalBlockCount) {
        // Free the excess blocks
        for (size_t i = optimalBlockCount; i < deque->blockCount; ++i) { 
            free(deque->blocks[i]);
            DEQUE_LOG("[deque_shrink_to_fit] Freed block at index: %zu.", i);
        }

        // Attempt to reallocate the blocks array to the new size
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * optimalBlockCount);
        if (!newBlocks) {
            DEQUE_LOG("[deque_shrink_to_fit] Error: Memory allocation failed during block shrink.");
            return; 
        }

        deque->blocks = newBlocks;
        deque->blockCount = optimalBlockCount;
        DEQUE_LOG("[deque_shrink_to_fit] Successfully shrunk deque. New blockCount: %zu.", deque->blockCount);
    } 
    else {
        DEQUE_LOG("[deque_shrink_to_fit] No shrinking needed. Deque is already optimal.");
    }
}

/**
 * @brief Inserts an element at a specified index in the deque.
 *
 * This function inserts a new element at the specified index in the deque.
 * If necessary, it reallocates memory to accommodate the new element, and shifts
 * existing elements to the right to make space for the insertion.
 *
 * @param deque Pointer to the deque.
 * @param index The position at which to insert the new element.
 * @param item Pointer to the item to insert.
 */
void deque_insert(Deque* deque, size_t index, void* item) {
    if (!deque || !item) {
        DEQUE_LOG("[deque_insert] Error: Deque or item is NULL.");
        return;
    }
    if (index > deque->size) {
        DEQUE_LOG("[deque_insert] Error: Index out of bounds.");
        return;
    }

    // Reallocate if more blocks are needed
    if (deque->size == deque->blockSize * deque->blockCount) {
        DEQUE_LOG("[deque_insert] Allocating new block.");
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            DEQUE_LOG("[deque_insert] Error: Memory allocation failed.");
            return;
        }
        deque->blocks = newBlocks;
        deque->blocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!deque->blocks[deque->blockCount]) {
            DEQUE_LOG("[deque_insert] Error: Block memory allocation failed.");
            return;
        }
        deque->blockCount++;
    }

    // Shift elements to make space for the new item
    for (size_t i = deque->size; i > index; --i) {
        size_t srcBlockIndex = (deque->frontIndex + i - 1) / deque->blockSize;
        size_t srcIndexInBlock = (deque->frontIndex + i - 1) % deque->blockSize;
        size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
        size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

        deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
    }

    // Insert the item
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;
    size_t indexInBlock = (deque->frontIndex + index) % deque->blockSize;
    deque->blocks[blockIndex][indexInBlock] = item;
    deque->size++;
}



/**
 * @brief Removes an element at a specified index in the deque.
 *
 * This function removes the element at the specified index in the deque.
 * It shifts elements to the left to fill the gap left by the removed element,
 * and may deallocate memory blocks if they are no longer needed.
 *
 * @param deque Pointer to the deque.
 * @param index The position of the element to remove.
 */
// void deque_erase(Deque* deque, size_t index) {
//     if (!deque) {
//         DEQUE_LOG("[deque_erase] Error: Deque is NULL.");
//         return;
//     }
//     if (index >= deque->size) {
//         DEQUE_LOG("[deque_erase] Error: Index out of bounds. index: %zu, deque size: %zu", index, deque->size);
//         return; 
//     }

//     DEQUE_LOG("[deque_erase] Removing element at index: %zu.", index);

//     // Shift elements to fill the gap left by the removed element
//     for (size_t i = index; i < deque->size - 1; ++i) {
//         size_t srcBlockIndex = (deque->frontIndex + i + 1) / deque->blockSize;
//         size_t srcIndexInBlock = (deque->frontIndex + i + 1) % deque->blockSize;
//         size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
//         size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

//         deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
//         DEQUE_LOG("[deque_erase] Shifted element from blockIndex: %zu, indexInBlock: %zu to blockIndex: %zu, indexInBlock: %zu.",
//                   srcBlockIndex, srcIndexInBlock, destBlockIndex, destIndexInBlock);
//     }

//     deque->size--;
//     DEQUE_LOG("[deque_erase] Element removed. New size: %zu.", deque->size);

//     if (deque->size % deque->blockSize == 0 && deque->blockCount > 1) {
//         free(deque->blocks[deque->blockCount - 1]);
//         deque->blockCount--;
//         DEQUE_LOG("[deque_erase] Freed unused block. New blockCount: %zu.", deque->blockCount);
//     }
// }

void deque_erase(Deque* deque, size_t index) {
    if (!deque || index >= deque_length(deque)) {
        DEQUE_LOG("[deque_erase] Error: Invalid deque or index.");
        return;
    }

    DEQUE_LOG("[deque_erase] Removing element at index: %zu.", index);

    // Iterate over all elements after the erased element and shift them one position left
    for (size_t i = index + 1; i < deque_length(deque); ++i) {
        void* currentElement = deque_at(deque, i);
        void* previousElement = deque_at(deque, i - 1);

        if (currentElement && previousElement) {
            memcpy(previousElement, currentElement, deque->itemSize);  // Copy the element to its new location
        }

        DEQUE_LOG("[deque_erase] Shifted element from index %zu to index %zu.", i, i - 1);
    }

    // Free the last element after shifting
    void* lastElement = deque_at(deque, deque_length(deque) - 1);
    if (lastElement) {
        DEQUE_LOG("[deque_erase] Freeing last element.");
        free(lastElement);
    }

    deque->size--;  // Reduce the size
    DEQUE_LOG("[deque_erase] Element removed. New size: %zu.", deque_length(deque));
}

/**
 * @brief Resizes the deque to the specified size.
 *
 * This function changes the size of the deque to the specified new size. 
 * If the new size is larger than the current size, the deque is expanded with 
 * default-initialized elements. If the new size is smaller, elements are removed 
 * from the back of the deque. The function also optimizes memory usage if the new 
 * size is significantly smaller than the current size.
 *
 * @param deque Pointer to the deque.
 * @param newSize The new size of the deque.
 */
void deque_resize(Deque* deque, size_t newSize) {
    if (!deque) {
        DEQUE_LOG("[deque_resize] Error: Deque is NULL.");
        return; 
    }

    DEQUE_LOG("[deque_resize] Resizing deque from size: %zu to new size: %zu.", deque->size, newSize);

    // Resize larger: add default-initialized elements to the back
    while (deque->size < newSize) {
        void* defaultItem = NULL;
        deque_push_back(deque, defaultItem);
        DEQUE_LOG("[deque_resize] Added default item. New size: %zu.", deque->size);
    }
    while (deque->size > newSize) {
        deque_pop_back(deque); 
        DEQUE_LOG("[deque_resize] Removed item from the back. New size: %zu.", deque->size);
    }

    // Optimize memory usage if the new size is much smaller than the current size
    size_t optimalBlockCount = (newSize / deque->blockSize) + ((newSize % deque->blockSize) ? 1 : 0);
    if (optimalBlockCount < deque->blockCount) {
        DEQUE_LOG("[deque_resize] Reducing block count from %zu to optimal block count: %zu.", deque->blockCount, optimalBlockCount);
        
        for (size_t i = optimalBlockCount; i < deque->blockCount; ++i) {
            free(deque->blocks[i]);
            DEQUE_LOG("[deque_resize] Freed block at index: %zu.", i);
        }

        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * optimalBlockCount);
        if (!newBlocks) {
            DEQUE_LOG("[deque_resize] Error: Memory allocation failed during block reduction.");
            return; 
        }
        deque->blocks = newBlocks;
        deque->blockCount = optimalBlockCount;
        DEQUE_LOG("[deque_resize] Successfully reduced block count to: %zu.", deque->blockCount);
    }
}

/**
 * @brief Swaps the contents of two deques.
 *
 * This function swaps the contents of the current deque with another deque. 
 * The two deques will exchange their contents, sizes, and internal structures.
 *
 * @param deque Pointer to the first deque.
 * @param otherDeque Pointer to the second deque.
 */
void deque_swap(Deque* deque, Deque* otherDeque) {
    if (!deque || !otherDeque) {
        DEQUE_LOG("[deque_swap] Error: One or both deques are NULL.");
        return; 
    }
    DEQUE_LOG("[deque_swap] Swapping contents of deque1 (size: %zu) and deque2 (size: %zu).", deque->size, otherDeque->size);
    
    Deque temp = *deque;
    *deque = *otherDeque;
    *otherDeque = temp;
    DEQUE_LOG("[deque_swap] Swap complete. New deque1 size: %zu, new deque2 size: %zu.", deque->size, otherDeque->size);
}

/**
 * @brief Assigns the specified value to a range of elements in the deque.
 *
 * This function clears the current contents of the deque and then resizes the deque
 * to hold 'n' elements, each initialized to the specified value. Memory is allocated 
 * as needed to accommodate the new size.
 *
 * @param deque Pointer to the deque.
 * @param n The number of elements to assign to the deque.
 * @param val Pointer to the value to assign to each element.
 */
void deque_assign(Deque* deque, size_t n, void* val) {
    if (!deque) {
        DEQUE_LOG("[deque_assign] Error: Deque is NULL.");
        return; 
    }

    DEQUE_LOG("[deque_assign] Assigning %zu elements to the deque.", n);

    deque_clear(deque); 
    DEQUE_LOG("[deque_assign] Cleared deque, now resizing to hold %zu elements.", n);

    // Resize the deque to have 'n' elements
    for (size_t i = 0; i < n; ++i) {
        // If expanding beyond current block count, allocate new blocks as needed
        if (deque->size == deque->blockSize * deque->blockCount) {
            DEQUE_LOG("[deque_assign] Allocating new block as current blocks are full.");

            void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
            if (!newBlocks) {
                DEQUE_LOG("[deque_assign] Error: Memory allocation failed during block expansion.");
                return; 
            }

            newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
            if (!newBlocks[deque->blockCount]) {
                DEQUE_LOG("[deque_assign] Error: Memory allocation failed for new block.");
                return; 
            }
            deque->blocks = newBlocks;
            deque->blockCount++;
            DEQUE_LOG("[deque_assign] New block allocated. Total blocks: %zu.", deque->blockCount);
        }

        // Calculate the block index and position within the block
        size_t blockIndex = deque->size / deque->blockSize;
        size_t indexInBlock = deque->size % deque->blockSize;

        deque->blocks[blockIndex][indexInBlock] = val;
        deque->size++;
        DEQUE_LOG("[deque_assign] Assigned value to blockIndex: %zu, indexInBlock: %zu. New size: %zu.", blockIndex, indexInBlock, deque->size);
    }
}


/**
 * @brief Inserts an element at the end of the deque.
 *
 * This function inserts a new element at the back of the deque. If the deque's 
 * current block is full, it allocates a new block and inserts the element there.
 *
 * @param deque Pointer to the deque.
 * @param item Pointer to the item to insert.
 */
void deque_emplace_back(Deque* deque, void* item) {
    if (!deque || !item) {
        DEQUE_LOG("[deque_emplace_back] Error: Deque or item is NULL.");
        return;
    }

    // Dynamically allocate memory for the new item
    void* newItem = malloc(deque->itemSize);
    if (!newItem) {
        DEQUE_LOG("[deque_emplace_back] Error: Memory allocation failed for new item.");
        return;
    }

    // Copy the content of 'item' into the newly allocated memory
    memcpy(newItem, item, deque->itemSize);

    // Handle inserting the item in a new block if necessary
    if (deque->backIndex == deque->blockSize - 1) {
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            free(newItem);  
            DEQUE_LOG("[deque_emplace_back] Error: Memory allocation for new block failed.");
            return;
        }
        deque->blocks = newBlocks;
        deque->blocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!deque->blocks[deque->blockCount]) {
            free(newItem);
            free(newBlocks); 
            DEQUE_LOG("[deque_emplace_back] Error: Memory allocation for new block element failed.");
            return;
        }
        deque->blockCount++;
        deque->backIndex = -1;
    }

    // Insert the new item at the back
    deque->backIndex++;
    deque->blocks[deque->blockCount - 1][deque->backIndex] = newItem;
    deque->size++;
    DEQUE_LOG("[deque_emplace_back] Item inserted at blockIndex: %zu, indexInBlock: %zu.", deque->blockCount - 1, deque->backIndex);
}

/**
 * @brief Inserts an element at the front of the deque.
 *
 * This function inserts a new element at the front of the deque. If the deque's 
 * current block is full at the front, it allocates a new block and inserts the 
 * element there.
 *
 * @param deque Pointer to the deque.
 * @param item Pointer to the item to insert.
 */
void deque_emplace_front(Deque* deque, void* item) {
    if (!deque) {
        DEQUE_LOG("[deque_emplace_front] Error: Deque is NULL.");
        return; 
    }
    if (!item) {
        DEQUE_LOG("[deque_emplace_front] Error: Item is NULL.");
        return; 
    }

    // Check if a new block is needed at the front
    if (deque->frontIndex == 0) {
        DEQUE_LOG("[deque_emplace_front] Allocating new block for deque at the front.");
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            DEQUE_LOG("[deque_emplace_front] Error: Memory allocation failed.");
            return; 
        }

        memmove(newBlocks + 1, newBlocks, sizeof(void**) * deque->blockCount);
        newBlocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[0]) {
            DEQUE_LOG("[deque_emplace_front] Error: Memory allocation failed for new block.");
            return; 
        }

        deque->blocks = newBlocks;
        deque->blockCount++;
        DEQUE_LOG("[deque_emplace_front] New block allocated at the front. blockCount: %zu", deque->blockCount);
        deque->frontIndex = deque->blockSize;
    }

    deque->frontIndex--;
    size_t blockIndex = deque->frontIndex / deque->blockSize;
    size_t indexInBlock = deque->frontIndex % deque->blockSize;
    deque->blocks[blockIndex][indexInBlock] = item;
    deque->size++;
    DEQUE_LOG("[deque_emplace_front] Item emplaced at blockIndex: %zu, indexInBlock: %zu. New size: %zu", blockIndex, indexInBlock, deque->size);
}

/**
 * @brief Inserts an element at the specified position in the deque.
 *
 * This function inserts an element at the given index in the deque. If the index is
 * at the front or back, it calls the appropriate emplace function. It handles the 
 * necessary memory management, including allocating new blocks if needed.
 *
 * @param deque Pointer to the deque in which to insert the element.
 * @param index The position at which to insert the element.
 * @param item Pointer to the item to be inserted.
 */
void deque_emplace(Deque* deque, size_t index, void* item) {
    if (!deque) {
        DEQUE_LOG("[deque_emplace] Error: Deque is NULL.");
        return; 
    }
    if (index > deque->size) {
        DEQUE_LOG("[deque_emplace] Error: Index out of bounds. index: %zu, deque size: %zu", index, deque->size);
        return; 
    }
    if (!item) {
        DEQUE_LOG("[deque_emplace] Error: Item is NULL.");
        return; 
    }
    if (index == deque->size) {
        DEQUE_LOG("[deque_emplace] Inserting item at the back.");
        deque_emplace_back(deque, item);
        return;
    }
    if (index == 0) {
        DEQUE_LOG("[deque_emplace] Inserting item at the front.");
        deque_emplace_front(deque, item);
        return;
    }

    // Check if a new block is needed
    if (deque->size == deque->blockSize * deque->blockCount) {
        DEQUE_LOG("[deque_emplace] Allocating new block for the deque.");
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            DEQUE_LOG("[deque_emplace] Error: Memory allocation failed.");
            return; 
        }

        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) {
            DEQUE_LOG("[deque_emplace] Error: Memory allocation failed for new block.");
            return;
        }

        deque->blocks = newBlocks;
        deque->blockCount++;
        DEQUE_LOG("[deque_emplace] New block allocated. blockCount: %zu", deque->blockCount);
    }

    // Shift elements to the right to make space for the new item
    for (size_t i = deque->size; i > index; --i) {
        size_t srcBlockIndex = (deque->frontIndex + i - 1) / deque->blockSize;
        size_t srcIndexInBlock = (deque->frontIndex + i - 1) % deque->blockSize;
        size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
        size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

        deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
        DEQUE_LOG("[deque_emplace] Shifted element from blockIndex: %zu, indexInBlock: %zu to blockIndex: %zu, indexInBlock: %zu",
                  srcBlockIndex, srcIndexInBlock, destBlockIndex, destIndexInBlock);
    }

    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;
    size_t indexInBlock = (deque->frontIndex + index) % deque->blockSize;
    deque->blocks[blockIndex][indexInBlock] = item;
    deque->size++;
    DEQUE_LOG("[deque_emplace] Item inserted at blockIndex: %zu, indexInBlock: %zu, new size: %zu", blockIndex, indexInBlock, deque->size);
}

/**
 * @brief Returns the maximum size of the deque.
 *
 * This function returns the maximum possible number of elements that the deque
 * can hold. This is typically a very large number, depending on system limits.
 *
 * @param deque Pointer to the deque.
 * @return The maximum size of the deque.
 */
size_t deque_max_size(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_max_size] Error: Deque is NULL.");
        return 0; 
    }
    DEQUE_LOG("[deque_max_size] Returning maximum size of the deque.");
    return SIZE_MAX;
}

/**
 * @brief Compares two deques for equality.
 *
 * This function checks whether two deques are equal, which means they have the
 * same number of elements and each corresponding element is equal.
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * 
 * @return true if the deques are equal, false otherwise.
 */
bool deque_is_equal(const Deque* deque1, const Deque* deque2) {
    if (deque1 == deque2) {
        DEQUE_LOG("[deque_is_equal] Both deques are the same or both are NULL.");
        return true; 
    }
    if (!deque1 || !deque2) {
        DEQUE_LOG("[deque_is_equal] Error: One or both deques are NULL.");
        return false; 
    }
    if (deque1->size != deque2->size) { 
        DEQUE_LOG("[deque_is_equal] Deques have different sizes. deque1 size: %zu, deque2 size: %zu", deque1->size, deque2->size);
        return false;
    }
    // Compare elements
    for (size_t i = 0; i < deque1->size; ++i) {
        if (deque_at(deque1, i) != deque_at(deque2, i)) {
            DEQUE_LOG("[deque_is_equal] Deques differ at index: %zu.", i);
            return false;
        }
    }
    DEQUE_LOG("[deque_is_equal] Deques are equal.");
    return true;
}

/**
 * @brief Compares two deques to determine if the first is less than the second.
 *
 * This function performs a lexicographical comparison of two deques. It checks
 * each element in sequence until it finds a pair that differs, or until one deque
 * runs out of elements.
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * 
 * @return true if the first deque is less than the second, false otherwise.
 */
bool deque_is_less(const Deque* deque1, const Deque* deque2) {
    if (!deque1 || !deque2) {
        DEQUE_LOG("[deque_is_less] Error: One or both deques are NULL.");
        return false; 
    }

    for (size_t i = 0; i < deque1->size && i < deque2->size; ++i) {
        if (deque_at(deque1, i) < deque_at(deque2, i)) { 
            DEQUE_LOG("[deque_is_less] Deque1 is less at index: %zu.", i);
            return true;
        }
        if (deque_at(deque1, i) > deque_at(deque2, i)) { 
            DEQUE_LOG("[deque_is_less] Deque1 is greater at index: %zu.", i);
            return false;
        }
    }
    bool result = deque1->size < deque2->size;
    DEQUE_LOG("[deque_is_less] Comparison result: %d. Deque1 size: %zu, Deque2 size: %zu", result, deque1->size, deque2->size);
    
    return result;
}

/**
 * @brief Compares two deques to determine if the first is greater than the second.
 *
 * This function checks whether the first deque is greater than the second
 * by comparing them using deque_is_less with the arguments reversed.
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * 
 * @return true if the first deque is greater than the second, false otherwise.
 */
bool deque_is_greater(const Deque* deque1, const Deque* deque2) {
    DEQUE_LOG("[deque_is_greater] Comparing if deque1 is greater than deque2.");
    return deque_is_less(deque2, deque1);
}

/**
 * @brief Compares two deques for inequality.
 *
 * This function checks whether two deques are not equal, which means either
 * they have different sizes or at least one pair of corresponding elements is not equal.
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * 
 * @return true if the deques are not equal, false otherwise.
 */
bool deque_is_not_equal(const Deque* deque1, const Deque* deque2) {
    DEQUE_LOG("[deque_is_not_equal] Comparing if deque1 is not equal to deque2.");
    return !deque_is_equal(deque1, deque2);
}

/**
 * @brief Compares two deques to determine if the first is less than or equal to the second.
 *
 * This function checks if the first deque is less than or equal to the second
 * by using the deque_is_less and deque_is_equal functions.
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * 
 * @return true if the first deque is less than or equal to the second, false otherwise.
 */
bool deque_is_less_or_equal(const Deque* deque1, const Deque* deque2) {
    DEQUE_LOG("[deque_is_less_or_equal] Comparing if deque1 is less than or equal to deque2.");
    return deque_is_less(deque1, deque2) || deque_is_equal(deque1, deque2);
}

/**
 * @brief Compares two deques to determine if the first is greater than or equal to the second.
 *
 * This function checks if the first deque is greater than or equal to the second
 * by using the deque_is_greater and deque_is_equal functions.
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * 
 * @return true if the first deque is greater than or equal to the second, false otherwise.
 */
bool deque_is_greater_or_equal(const Deque* deque1, const Deque* deque2) {
    DEQUE_LOG("[deque_is_greater_or_equal] Comparing if deque1 is greater than or equal to deque2.");
    return deque_is_greater(deque1, deque2) || deque_is_equal(deque1, deque2);
}

/**
 * @brief Returns an iterator to the beginning of the deque.
 *
 * This function returns an iterator pointing to the first element in the deque.
 * If the deque is empty, the iterator will be set to the end position.
 *
 * @param deque Pointer to the deque.
 * @return An iterator pointing to the first element of the deque.
 */
DequeIterator deque_begin(const Deque* deque) {
    DequeIterator it = {0};
    if (!deque) {
        DEQUE_LOG("[deque_begin] Error: Deque is NULL.");
        return it; // Return default iterator on error
    }
    if (deque->size > 0) {
        it.deque = (Deque*)deque;
        it.current = deque->blocks[0][deque->frontIndex];
        it.blockIndex = deque->frontIndex / deque->blockSize;
        it.indexInBlock = deque->frontIndex % deque->blockSize;
        DEQUE_LOG("[deque_begin] Iterator to first element created. blockIndex: %zu, indexInBlock: %zu",
                  it.blockIndex, it.indexInBlock);
    } 
    else {
        DEQUE_LOG("[deque_begin] Deque is empty.");
    }

    return it;
}

/**
 * @brief Returns an iterator to the end of the deque.
 *
 * This function returns an iterator pointing to the position just past the last element
 * in the deque. This is typically used as a sentinel in loops.
 *
 * @param deque Pointer to the deque.
 * @return An iterator pointing just past the last element of the deque.
 */
DequeIterator deque_end(const Deque* deque) {
    DequeIterator it = {0};
    if (!deque) {
        DEQUE_LOG("[deque_end] Error: Deque is NULL.");
        return it; 
    }

    it.deque = (Deque*)deque;
    it.current = NULL;  // End iterator is past the last element

    size_t totalElements = deque->frontIndex + deque->size;
    it.blockIndex = totalElements / deque->blockSize;
    it.indexInBlock = totalElements % deque->blockSize;

    DEQUE_LOG("[deque_end] Iterator to past-the-end element created. blockIndex: %zu, indexInBlock: %zu",
              it.blockIndex, it.indexInBlock);
    
    return it;
}

/**
 * @brief Returns a reverse iterator pointing to the last element in the deque.
 *
 * This function initializes a reverse iterator that points to the last element of the deque.
 * The reverse iterator can be used to iterate backward through the deque.
 *
 * @param deque Pointer to the deque for which the reverse iterator is created.
 * @return A DequeIterator pointing to the last element of the deque.
 */
DequeIterator deque_rbegin(const Deque* deque) {
    DequeIterator it = {0};
    if (!deque) {
        DEQUE_LOG("[deque_rbegin] Error: Deque is NULL.");
        return it; 
    }

    if (deque->size > 0) {
        it.deque = (Deque*)deque;
        size_t lastBlock = (deque->frontIndex + deque->size - 1) / deque->blockSize;
        size_t indexInLastBlock = (deque->frontIndex + deque->size - 1) % deque->blockSize;

        it.current = deque->blocks[lastBlock][indexInLastBlock];
        it.blockIndex = lastBlock;
        it.indexInBlock = indexInLastBlock;
        it.isReverse = true; // Set isReverse to true for reverse iterator
        DEQUE_LOG("[deque_rbegin] Reverse iterator created at blockIndex: %zu, indexInBlock: %zu", lastBlock, indexInLastBlock);
    }
    else {
        DEQUE_LOG("[deque_rbegin] Deque is empty.");
    }

    return it;
}

/**
 * @brief Returns a reverse iterator pointing to the position before the first element in the deque.
 *
 * This function initializes a reverse iterator that represents the position before
 * the first element in the deque, effectively the "rend" position for reverse iteration.
 *
 * @param deque Pointer to the deque for which the reverse end iterator is created.
 * @return A DequeIterator representing the reverse end position of the deque.
 */
DequeIterator deque_rend(const Deque* deque) {
    DequeIterator it = {0};
    if (!deque) {
        DEQUE_LOG("[deque_rend] Error: Deque is NULL.");
        return it; 
    }

    it.deque = (Deque*)deque;
    it.current = NULL;
    it.blockIndex = 0;
    it.indexInBlock = SIZE_MAX; // Representing an invalid position
    it.isReverse = true; // Set isReverse to true for reverse iterator
    
    DEQUE_LOG("[deque_rend] Reverse end iterator created at invalid blockIndex.");
    return it;
}

/**
 * @brief Returns a constant iterator to the first element of the deque.
 *
 * This function provides a constant iterator (read-only) that points to the first element
 * in the deque. The iterator can be used to traverse the deque from the beginning.
 *
 * @param deque Pointer to the deque for which the constant begin iterator is created.
 * @return A constant DequeIterator pointing to the first element of the deque.
 */
DequeIterator deque_cbegin(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_cbegin] Error: Deque is NULL.");
        return (DequeIterator){0};  // Return an empty iterator
    }

    DequeIterator it = deque_begin(deque);
    it.deque = deque;  // Ensure the iterator refers to the correct deque
    it.current = deque->blocks[it.blockIndex] + it.indexInBlock;  // Point to the first element
    DEQUE_LOG("[deque_cbegin] Constant begin iterator created at blockIndex: %zu, indexInBlock: %zu", it.blockIndex, it.indexInBlock);
    return it;
}

/**
 * @brief Returns a constant iterator to the position after the last element of the deque.
 *
 * This function provides a constant iterator (read-only) that represents the position
 * just after the last element in the deque, effectively the "end" position.
 *
 * @param deque Pointer to the deque for which the constant end iterator is created.
 * @return A constant DequeIterator representing the end position of the deque.
 */
DequeIterator deque_cend(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_cend] Error: Deque is NULL.");
        return (DequeIterator){0};  // Return an empty iterator
    }

    DequeIterator it = deque_end(deque);
    it.deque = deque;  // Ensure the iterator refers to the correct deque
    it.current = NULL;  // End iterator doesn't point to a valid element
    DEQUE_LOG("[deque_cend] Constant end iterator created.");
    return it;
}

/**
 * @brief Returns a constant reverse iterator to the last element of the deque.
 *
 * This function provides a constant reverse iterator (read-only) that points to the last element
 * in the deque. The iterator can be used to traverse the deque in reverse order.
 *
 * @param deque Pointer to the deque for which the constant reverse begin iterator is created.
 * @return A pointer to a constant DequeIterator pointing to the last element of the deque.
 */
DequeIterator deque_crbegin(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_crbegin] Error: Deque is NULL.");
        return (DequeIterator){0};  // Return an empty iterator
    }

    DequeIterator it = deque_rbegin(deque);
    DEQUE_LOG("[deque_crbegin] Created constant reverse iterator at blockIndex: %zu, indexInBlock: %zu",
              it.blockIndex, it.indexInBlock);
    return it;
}

/**
 * @brief Returns a constant reverse iterator to the position before the first element of the deque.
 *
 * This function provides a constant reverse iterator (read-only) that represents the position
 * before the first element in the deque, effectively the "rend" position.
 *
 * @param deque Pointer to the deque for which the constant reverse end iterator is created.
 * @return A constant DequeIterator representing the reverse end position of the deque.
 */
DequeIterator deque_crend(const Deque* deque) {
    if (!deque) {
        DEQUE_LOG("[deque_crend] Error: Deque is NULL.");
        return (DequeIterator){0};  // Return an empty iterator
    }

    DequeIterator it = deque_rend(deque);
    DEQUE_LOG("[deque_crend] Created constant reverse end iterator at blockIndex: %zu, indexInBlock: %zu",
              it.blockIndex, it.indexInBlock);
    return it;
}

/**
 * @brief Moves the iterator to the previous position for reverse iteration.
 *
 * For reverse iterators, this function decrements the iterator's position, moving backwards
 * through the deque. It properly handles moving between blocks and invalidates the iterator
 * when it reaches the reverse end.
 *
 * @param it Pointer to the iterator to increment (move backward).
 */
void iterator_increment(DequeIterator* it) {
    if (!it || !it->deque) {
        DEQUE_LOG("[iterator_increment] Error: Iterator or Deque is NULL.");
        return;
    }

    it->indexInBlock++;
    if (it->indexInBlock >= it->deque->blockSize) {  // Move to the next block
        it->blockIndex++;
        it->indexInBlock = 0;
    }

    if (it->blockIndex >= it->deque->blockCount ||  // Check if out of bounds
        (it->blockIndex == it->deque->blockCount - 1 && it->indexInBlock > it->deque->backIndex)) {
        it->current = NULL;
        DEQUE_LOG("[iterator_increment] Reached end of deque, iterator is now NULL.");
    } else {
        it->current = it->deque->blocks[it->blockIndex] + it->indexInBlock;
        DEQUE_LOG("[iterator_increment] Updated iterator position: blockIndex: %zu, indexInBlock: %zu", it->blockIndex, it->indexInBlock);
    }
}


/**
 * @brief Decrements the position of the iterator.
 *
 * This function moves the iterator one position backward in the deque. If the iterator
 * is a reverse iterator, it moves forward instead. It handles boundary conditions,
 * such as moving between blocks of the deque.
 *
 * @param it Pointer to the iterator to decrement.
 */
void iterator_decrement(DequeIterator* it) {
    if (!it || !it->deque) {
        DEQUE_LOG("[iterator_decrement] Error: Iterator or Deque is NULL.");
        return;
    }

    if (it->indexInBlock == 0) {  // Move to the previous block
        if (it->blockIndex == 0) {
            it->current = NULL;  // Reached the reverse end
            DEQUE_LOG("[iterator_decrement] Reached reverse end of deque, iterator is now NULL.");
            return;
        }
        it->blockIndex--;
        it->indexInBlock = it->deque->blockSize - 1;
    } else {
        it->indexInBlock--;
    }

    // Update current pointer
    if (it->blockIndex < it->deque->blockCount) {
        it->current = it->deque->blocks[it->blockIndex] + it->indexInBlock;
        DEQUE_LOG("[iterator_decrement] Reverse iteration - Updated iterator position: blockIndex: %zu, indexInBlock: %zu", it->blockIndex, it->indexInBlock);
    } else {
        it->current = NULL;
        DEQUE_LOG("[iterator_decrement] Reverse iteration - Iterator is now invalid.");
    }
}

/**
 * @brief Compares two iterators for equality.
 *
 * This function checks if two iterators point to the same position in the deque.
 * It also handles cases where one or both iterators are at the end (or rend) position.
 *
 * @param it1 Pointer to the first iterator.
 * @param it2 Pointer to the second iterator.
 * 
 * @return true if the iterators are equal, false otherwise.
 */
bool iterator_equals(const DequeIterator* it1, const DequeIterator* it2) {
    if (!it1 || !it2) {
        DEQUE_LOG("[iterator_equals] Error: One or both iterators are NULL.");
        return false; // Null pointer error
    }
    
    // Check if either iterator is at the end (or rend) position
    if (it1->current == NULL || it2->current == NULL) {
        bool isEqual = it1->current == it2->current;
        DEQUE_LOG("[iterator_equals] One or both iterators are at end/rend. Equality result: %d", isEqual);

        return isEqual;
    }
    
    bool isEqual = it1->blockIndex == it2->blockIndex && it1->indexInBlock == it2->indexInBlock;
    DEQUE_LOG("[iterator_equals] Comparing iterators. blockIndex1: %zu, indexInBlock1: %zu, blockIndex2: %zu, indexInBlock2: %zu. Equality result: %d",
              it1->blockIndex, it1->indexInBlock, it2->blockIndex, it2->indexInBlock, isEqual);
    
    return isEqual;
}

/**
 * @brief Retrieves the value pointed to by the iterator.
 *
 * This function returns the value at the current position of the iterator. It handles
 * both forward and reverse iteration, ensuring that the correct element is returned.
 *
 * @param it Pointer to the iterator from which to retrieve the value.
 * 
 * @return A pointer to the value at the current position of the iterator, or NULL if the iterator is out of bounds.
 */
void* iterator_get(const DequeIterator* it) {
    if (!it || !it->deque) {
        DEQUE_LOG("[iterator_get] Error: Iterator or Deque is NULL.");
        return NULL; 
    }

    // Handle out-of-bounds situation
    if (it->blockIndex >= it->deque->blockCount || it->indexInBlock >= it->deque->blockSize) {
        DEQUE_LOG("[iterator_get] Error: Iterator out of bounds.");
        return NULL;
    }

    DEQUE_LOG("[iterator_get] Returning element at blockIndex: %zu, indexInBlock: %zu", it->blockIndex, it->indexInBlock);
    return it->deque->blocks[it->blockIndex][it->indexInBlock];
}
