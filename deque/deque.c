/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Deque
*/

#include "deque.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
        fmt_fprintf(stderr, "Error: itemSize can not be Zero in deque_create.\n");
        exit(-1);
    }
    // Allocate memory for the Deque structure
    Deque* deque = (Deque*)malloc(sizeof(Deque)); 
    if (!deque) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in deque_create.\n");
        exit(-1); // Memory allocation failure
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
        free(deque);
        fmt_fprintf(stderr, "Error: Memory allocation failed in deque_create.\n");
        exit(-1); // Memory allocation failure
    }

    // Allocate memory for the first block
    deque->blocks[0] = (void**)malloc(sizeof(void*) * DEFAULT_BLOCK_SIZE);
    if (!deque->blocks[0]) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for the first block in deque_create.\n");
        free(deque->blocks);
        free(deque);
        exit(-1); // Handle allocation failure
    }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_empty.\n");
        return true; // Treat NULL deque as empty
    }
    return deque->size == 0;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_length.\n");
        return 0; // Return 0 for NULL deque
    }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_push_front.\n");
        return; // Null deque error
    }
    if (!item) {
        fmt_fprintf(stderr, "Error: Item is NULL in deque_push_front.\n");
        return; // Null item error
    }
    // Check if a new block is needed at the front
    if (deque->frontIndex == 0) {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1)); // Allocate a new block at the front
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_push_front.\n");
            return; // Memory allocation failure
        }
        memmove(newBlocks + 1, newBlocks, sizeof(void**) * deque->blockCount); // Shift existing blocks to the right

        newBlocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[0]) { 
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_push_front.\n");
            return; // Handle allocation failure
        }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_push_back.\n");
        return; // Null deque error
    }
    if (!item) {
        fmt_fprintf(stderr, "Error: Item is NULL in deque_push_back.\n");
        return; // Null item error
    }
    // Check if a new block is needed at the back
    if (deque->backIndex == deque->blockSize - 1) {
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_push_back.\n");
            return;
        }
        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        
        if (!newBlocks[deque->blockCount]) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_push_back.\n");
            free(newBlocks);
            return;
        }
        deque->blocks = newBlocks;
        deque->blockCount++;
        deque->backIndex = -1;
    }

    // Allocate memory for the new item and copy the item contents
    void* newItem = malloc(deque->itemSize);
    if (!newItem) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in deque_push_back.\n");
        return;
    }
    memcpy(newItem, item, deque->itemSize);

    // Insert the new item at the back
    deque->backIndex++;
    deque->blocks[deque->blockCount - 1][deque->backIndex] = newItem;
    deque->size++;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_front.\n");
        return NULL; // Null deque error
    }
    if (deque_empty(deque)) {
        fmt_fprintf(stderr, "Error: Deque is empty in deque_front.\n");
        return NULL; // Empty deque error
    }
    // Calculate the block and index for the front element
    size_t blockIndex = deque->frontIndex / deque->blockSize;
    size_t indexInBlock = deque->frontIndex % deque->blockSize;

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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_back.\n");
        return NULL; // Null deque error
    }
    if (deque_empty(deque)) { 
        fmt_fprintf(stderr, "Error: Deque is empty in deque_back.\n");
        return NULL; // Empty deque error
    }
    // Calculate the block and index for the back element
    size_t blockIndex = deque->backIndex / deque->blockSize;
    size_t indexInBlock = deque->backIndex % deque->blockSize;

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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_pop_front.\n");
        return; // Null deque error
    }
    if (deque_empty(deque)) {
        fmt_fprintf(stderr, "Error: Deque is empty in deque_pop_front.\n");
        return; // Empty deque error
    }
    deque->frontIndex++;

    if (deque->frontIndex / deque->blockSize > 0) {
        free(deque->blocks[0]);

        memmove(deque->blocks, deque->blocks + 1, sizeof(void**) * (deque->blockCount - 1));
        deque->blockCount--;
        deque->frontIndex -= deque->blockSize;
    }
    deque->size--;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_pop_back.\n");
        return; // Null deque error
    }
    if (deque_empty(deque)) {
        fmt_fprintf(stderr, "Error: Deque is empty in deque_pop_back.\n");
        return; // Empty deque error
    }
    // Free the memory of the item being popped
    free(deque->blocks[deque->blockCount - 1][deque->backIndex]);

    // Normal decrement
    if (deque->backIndex > 0) {
        deque->backIndex--;
    } 
    else if (deque->blockCount > 1) {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
        deque->backIndex = deque->blockSize - 1;
    }
    deque->size--;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_at.\n");
        return NULL; // Null deque error
    }
    if (index >= deque->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in deque_at.\n");
        return NULL; // Index out of bounds error
    }
    // Calculating the block and the index within the block
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;
    size_t indexInBlock = (deque->frontIndex + index) % deque->blockSize;

    // Adjusting the calculation for blockIndex when crossing block boundaries
    if (blockIndex >= deque->blockCount) {
        blockIndex = deque->blockCount - 1;
        indexInBlock = deque->backIndex - ((deque->size - 1) - index);
    }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_clear.\n");
        return; // Null deque error
    }

    for (size_t i = 0; i < deque->blockCount; ++i) {
        for (size_t j = 0; j < (i == deque->blockCount - 1 ? deque->backIndex + 1 : deque->blockSize); ++j) { 
            free(deque->blocks[i][j]);  // Free each item
        }
        free(deque->blocks[i]);  // Free each block
    }
    // Reallocate one block to reset the deque
    deque->blocks = (void***)realloc(deque->blocks, sizeof(void**));
    deque->blocks[0] = (void**)malloc(sizeof(void*) * deque->blockSize);
    if (!deque->blocks[0]) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in deque_clear.\n");
        return;
    }

    deque->blockCount = 1;
    deque->size = 0;
    deque->frontIndex = deque->blockSize / 2;
    deque->backIndex = deque->frontIndex - 1;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_deallocate.\n");
        return; // Null deque error
    }

    for (size_t i = 0; i < deque->blockCount; ++i) {
        for (size_t j = 0; j < (i == deque->blockCount - 1 ? deque->backIndex + 1 : deque->blockSize); ++j) {
            free(deque->blocks[i][j]);  // Free each item
        }
        free(deque->blocks[i]);  // Free each block
    }
    free(deque->blocks);  // Free the blocks array
    free(deque);  // Free the deque structure
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_shrink_to_fit.\n");
        return; // Null deque error
    }
    if (deque->size == 0) {
        return; // Nothing to shrink in an empty deque
    }
    // Calculate the optimal number of blocks needed
    size_t optimalBlockCount = (deque->size / deque->blockSize) + ((deque->size % deque->blockSize) ? 1 : 0);

    // If the current block count is greater than the optimal, reduce it
    if (deque->blockCount > optimalBlockCount) {
        // Free the excess blocks
        for (size_t i = optimalBlockCount; i < deque->blockCount; ++i) { 
            free(deque->blocks[i]);
        }
        
        // Attempt to reallocate the blocks array to the new size
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * optimalBlockCount);
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_shrink_to_fit.\n");
            return; // Memory allocation error
        }
        
        deque->blocks = newBlocks;
        deque->blockCount = optimalBlockCount;
        
        // If realloc fails, the excess blocks have already been freed,
        // and the remaining blocks are still valid. The deque continues to function
        // with potentially unused allocated space in the blocks array.
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
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_insert.\n");
        return; // Null deque error
    }
    if (index > deque->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in deque_insert.\n");
        return; // Index out of bounds error
    }
    if (!item) {
        fmt_fprintf(stderr, "Error: Item is NULL in deque_insert.\n");
        return; // Null item error
    }
    if (index == deque->size) {
        deque_push_back(deque, item);
        return;
    }
    if (index == 0) {
        deque_push_front(deque, item);
        return;
    }

    size_t actualIndex = (deque->frontIndex + index) % deque->blockSize;
    size_t blockIndex = (deque->frontIndex + index) / deque->blockSize;

    if (deque->size == deque->blockSize * deque->blockCount) {
        // Reallocate to add a new block
        void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_insert.\n");
            return; // Memory allocation error
        }
        // Initialize the new block at the end
        newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) { 
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_insert.\n");
            return; // Allocation failure
        }

        deque->blocks = newBlocks;
        deque->blockCount++;
    }

    // Shift elements to make space for the new item
    // Starting from the last element, shift elements to the right
    for (size_t i = deque->size; i > index; --i) {
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
void deque_erase(Deque* deque, size_t index) {
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_erase.\n");
        return; // Null deque error
    }
    if (index >= deque->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in deque_erase.\n");
        return; // Index out of bounds error
    }
    
    for (size_t i = index; i < deque->size - 1; ++i) {
        size_t srcBlockIndex = (deque->frontIndex + i + 1) / deque->blockSize;
        size_t srcIndexInBlock = (deque->frontIndex + i + 1) % deque->blockSize;
        size_t destBlockIndex = (deque->frontIndex + i) / deque->blockSize;
        size_t destIndexInBlock = (deque->frontIndex + i) % deque->blockSize;

        deque->blocks[destBlockIndex][destIndexInBlock] = deque->blocks[srcBlockIndex][srcIndexInBlock];
    }
    deque->size--;

    if (deque->size % deque->blockSize == 0 && deque->blockCount > 1) {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
    }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_resize.\n");
        return; // Null deque error
    }

    // Resize larger: add default-initialized elements to the back
    while (deque->size < newSize)  {
        void* defaultItem = NULL; // Default item (could be customized as needed)
        deque_push_back(deque, defaultItem);
    }
    while (deque->size > newSize) {
        // Resize smaller: remove elements from the back
        deque_pop_back(deque);
    }

    // Optimize memory usage if the new size is much smaller than the current size
    // This could involve reallocating the blocks array to a smaller size
    size_t optimalBlockCount = (newSize / deque->blockSize) + ((newSize % deque->blockSize) ? 1 : 0);
    if (optimalBlockCount < deque->blockCount) {
        for (size_t i = optimalBlockCount; i < deque->blockCount; ++i) { 
            free(deque->blocks[i]);
        }
        
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * optimalBlockCount);
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_resize.\n");
            return; // Memory allocation error
        }
        deque->blocks = newBlocks;
        deque->blockCount = optimalBlockCount;
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
        fmt_fprintf(stderr, "Error: One or both deques are NULL in deque_swap.\n");
        return; // Validate input
    }
    // Swap the contents of the two deques
    Deque temp = *deque;
    *deque = *otherDeque;
    *otherDeque = temp;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_assign.\n");
        return; // Null deque error
    }
    deque_clear(deque); // Clear the current contents of the deque

    // Resize the deque to have 'n' elements
    for (size_t i = 0; i < n; ++i) {
        // If expanding beyond current block count, allocate new blocks as needed
        if (deque->size == deque->blockSize * deque->blockCount) {
            void*** newBlocks = (void***)realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
            if (!newBlocks) {
                fmt_fprintf(stderr, "Error: Memory allocation failed in deque_assign.\n");
                return; // Memory allocation error
            } 
            newBlocks[deque->blockCount] = (void**)malloc(sizeof(void*) * deque->blockSize);
            if (!newBlocks[deque->blockCount]) {
                fmt_fprintf(stderr, "Error: Memory allocation failed in deque_assign.\n");
                return; // Memory allocation error
            }
            deque->blocks = newBlocks;
            deque->blockCount++;
        }

        // Calculate the block index and position within the block
        size_t blockIndex = deque->size / deque->blockSize;
        size_t indexInBlock = deque->size % deque->blockSize;

        deque->blocks[blockIndex][indexInBlock] = val;
        deque->size++; 
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
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_emplace_back.\n");
        return; // Null deque error
    }
    if (!item) {
        fmt_fprintf(stderr, "Error: Item is NULL in deque_emplace_back.\n");
        return; // Null item error
    }

    // Check if a new block is needed at the back
    if (deque->backIndex == deque->blockSize - 1)  {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) { 
            return;
        }

        newBlocks[deque->blockCount] = malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_emplace_back.\n");
            return; // Memory allocation error
        }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_emplace_front.\n");
        return; // Null deque error
    }
    if (!item) {
        fmt_fprintf(stderr, "Error: Item is NULL in deque_emplace_front.\n");
        return; // Null item error
    }

    // Check if a new block is needed at the front
    if (deque->frontIndex == 0)  {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_emplace_front.\n");
            return; // Memory allocation error
        }
        memmove(newBlocks + 1, newBlocks, sizeof(void**) * deque->blockCount);
        newBlocks[0] = malloc(sizeof(void*) * deque->blockSize);

        if (!newBlocks[0]) { 
            return;
        }
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_emplace.\n");
        return; // Null deque error
    }
    if (index > deque->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in deque_emplace.\n");
        return; // Index out of bounds error
    }
    if (!item) {
        fmt_fprintf(stderr, "Error: Item is NULL in deque_emplace.\n");
        return; // Null item error
    }
    if (index == deque->size) {
        deque_emplace_back(deque, item);
        return;
    }
    if (index == 0) {
        deque_emplace_front(deque, item);
        return;
    }

    // Check if a new block is needed
    if (deque->size == deque->blockSize * deque->blockCount) {
        void*** newBlocks = realloc(deque->blocks, sizeof(void**) * (deque->blockCount + 1));
        if (!newBlocks) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in deque_emplace.\n");
            return; // Memory allocation error
        }

        newBlocks[deque->blockCount] = malloc(sizeof(void*) * deque->blockSize);
        if (!newBlocks[deque->blockCount]) {
            return;
        }

        deque->blocks = newBlocks;
        deque->blockCount++;
    }

    // Shift elements to the right to make space for the new item
    for (size_t i = deque->size; i > index; --i) {
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_max_size.\n");
        return 0; // Return 0 to indicate error
    }
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
        return true; // Same deque or both NULL
    }
    if (!deque1 || !deque2) {
        fmt_fprintf(stderr, "Error: One or both deques are NULL in deque_is_equal.\n");
        return false; // One is NULL, the other is not
    }
    if (deque1->size != deque2->size) { 
        return false; // Different sizes
    }
    // Compare elements
    for (size_t i = 0; i < deque1->size; ++i) {
        if (deque_at(deque1, i) != deque_at(deque2, i)) {
            return false;
        }
    }
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
        fmt_fprintf(stderr, "Error: One or both deques are NULL in deque_is_less.\n");
        return false; // Invalid input
    }

    for (size_t i = 0; i < deque1->size && i < deque2->size; ++i) {
        if (deque_at(deque1, i) < deque_at(deque2, i)) { 
            return true;
        }
        if (deque_at(deque1, i) > deque_at(deque2, i)) { 
            return false;
        }
    }
    return deque1->size < deque2->size;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_begin.\n");
        return it; // Return default iterator on error
    }
    if (deque->size > 0) {
        it.deque = (Deque*)deque;
        it.current = deque->blocks[0][deque->frontIndex];
        it.blockIndex = deque->frontIndex / deque->blockSize;
        it.indexInBlock = deque->frontIndex % deque->blockSize;
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_end.\n");
        return it; // Return default iterator on error
    }

    it.deque = (Deque*)deque;
    it.current = NULL;  // End iterator is past the last element

    // Calculate blockIndex and indexInBlock for the position just past the last element
    size_t totalElements = deque->frontIndex + deque->size;
    it.blockIndex = totalElements / deque->blockSize;
    it.indexInBlock = totalElements % deque->blockSize;
    
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_rbegin.\n");
        return it; // Return default iterator on error
    }

    if (deque && deque->size > 0) {
        it.deque = (Deque*)deque;
        size_t lastBlock = (deque->frontIndex + deque->size - 1) / deque->blockSize;
        size_t indexInLastBlock = (deque->frontIndex + deque->size - 1) % deque->blockSize;

        it.current = deque->blocks[lastBlock][indexInLastBlock];
        it.blockIndex = lastBlock;
        it.indexInBlock = indexInLastBlock;
        it.isReverse = true; // Set isReverse to true for reverse iterator
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
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_rend.\n");
        return it; // Return default iterator on error
    }

    it.deque = (Deque*)deque;
    it.current = NULL;
    it.blockIndex = 0;
    it.indexInBlock = SIZE_MAX; // Representing an invalid position
    it.isReverse = true; // Set isReverse to true for reverse iterator
    
    return it;
}

/**
 * @brief Returns a constant iterator to the first element of the deque.
 *
 * This function provides a constant iterator (read-only) that points to the first element
 * in the deque. The iterator can be used to traverse the deque from the beginning.
 *
 * @param deque Pointer to the deque for which the constant begin iterator is created.
 * @return A pointer to a constant DequeIterator pointing to the first element of the deque.
 */
const DequeIterator* deque_cbegin(const Deque* deque) {
    static DequeIterator temp_iterator;
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_cbegin.\n");
        return NULL; // Return NULL on error
    }

    temp_iterator = deque_begin(deque);
    return &temp_iterator;
}

/**
 * @brief Returns a constant iterator to the position after the last element of the deque.
 *
 * This function provides a constant iterator (read-only) that represents the position
 * just after the last element in the deque, effectively the "end" position.
 *
 * @param deque Pointer to the deque for which the constant end iterator is created.
 * @return A pointer to a constant DequeIterator representing the end position of the deque.
 */
const DequeIterator* deque_cend(const Deque* deque) {
    static DequeIterator temp_iterator;
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_cend.\n");
        return NULL; // Return NULL on error
    }

    temp_iterator = deque_end(deque);
    return &temp_iterator;
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
const DequeIterator* deque_crbegin(const Deque* deque) {
    static DequeIterator temp_iterator;
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_crbegin.\n");
        return NULL; // Return NULL on error
    }

    temp_iterator = deque_rbegin(deque);
    return &temp_iterator;
}

/**
 * @brief Returns a constant reverse iterator to the position before the first element of the deque.
 *
 * This function provides a constant reverse iterator (read-only) that represents the position
 * before the first element in the deque, effectively the "rend" position.
 *
 * @param deque Pointer to the deque for which the constant reverse end iterator is created.
 * @return A pointer to a constant DequeIterator representing the reverse end position of the deque.
 */
const DequeIterator* deque_crend(const Deque* deque) {
    static DequeIterator temp_iterator;
    if (!deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in deque_crend.\n");
        return NULL; // Return NULL on error
    }

    temp_iterator = deque_rend(deque);
    return &temp_iterator;
}

/**
 * @brief Increments the position of the iterator.
 *
 * This function moves the iterator one position forward in the deque. If the iterator
 * is a reverse iterator, it moves backward instead. It handles boundary conditions,
 * such as moving between blocks of the deque.
 *
 * @param it Pointer to the iterator to increment.
 */
void iterator_increment(DequeIterator* it) {
    if (!it) {
        fmt_fprintf(stderr, "Error: Iterator is NULL in iterator_increment.\n");
        return; // Null iterator error
    }
    if (!it->deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in iterator_increment.\n");
        return; // Null deque error
    }
    if (it->isReverse) {
        // Handle reverse iteration
        if (it->indexInBlock == 0) {
            if (it->blockIndex > 0) {
                it->blockIndex--;
                it->indexInBlock = it->deque->blockSize - 1;
            } 
            else {
                it->current = NULL; // Reached the beginning of the deque
            }
        }
        else {
            it->indexInBlock--;
        }
        if (it->blockIndex < it->deque->blockCount && it->indexInBlock != SIZE_MAX) {
            it->current = it->deque->blocks[it->blockIndex][it->indexInBlock];
        }
        else { 
            it->current = NULL;
        }
    } 
    else {
        it->indexInBlock++; // Handle forward iteration
        if (it->indexInBlock >= it->deque->blockSize) {
            it->blockIndex++;
            it->indexInBlock = 0;
        }
        if (it->blockIndex < it->deque->blockCount) { 
            it->current = it->deque->blocks[it->blockIndex][it->indexInBlock];
        }
        else { 
            it->current = NULL;
        }
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
    if (!it) {
        fmt_fprintf(stderr, "Error: Iterator is NULL in iterator_decrement.\n");
        return; // Null iterator error
    }
    if (!it->deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in iterator_decrement.\n");
        return; // Null deque error
    }
    if (it->deque->size == 0) {
        fmt_fprintf(stderr, "Error: Deque is empty in iterator_decrement.\n");
        return; // Empty deque error
    }
    if (it->isReverse) {
        // Handle decrement for reverse iterator
        if (it->indexInBlock < it->deque->blockSize - 1) { 
            it->indexInBlock++;
        }
        else {
            if (it->blockIndex < it->deque->blockCount - 1) {
                it->blockIndex++;
                it->indexInBlock = 0;
            } 
            else {
                it->current = NULL; // Reached the end of the deque
                return;
            }
        }
    } 
    else {
        // Handle decrement for forward iterator
        if (it->indexInBlock == 0) {
            if (it->blockIndex > 0) {
                it->blockIndex--;
                it->indexInBlock = it->deque->blockSize - 1;
            } 
            else {
                it->current = NULL; // Reached the beginning of the deque
                return;
            }
        } 
        else {
            it->indexInBlock--;
        }
    }
    it->current = it->deque->blocks[it->blockIndex][it->indexInBlock]; // Update the current pointer
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
        fmt_fprintf(stderr, "Error: One or both iterators are NULL in iterator_equals.\n");
        return false; // Null pointer error
    }
    // Check if either iterator is at the end (or rend) position
    if (it1->current == NULL || it2->current == NULL) {
        bool isEqual = it1->current == it2->current;
        return isEqual;
    }
    // Standard comparison for other cases
    bool isEqual = it1->blockIndex == it2->blockIndex && it1->indexInBlock == it2->indexInBlock;
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
    if (!it) {
        fmt_fprintf(stderr, "Error: Iterator is NULL in iterator_get.\n");
        return NULL; // Null iterator error
    }
    if (!it->deque) {
        fmt_fprintf(stderr, "Error: Deque is NULL in iterator_get.\n");
        return NULL; // Null deque error
    }
    if (it->isReverse) {
        // Handle reverse iteration
        if (it->blockIndex < it->deque->blockCount && it->indexInBlock != SIZE_MAX) { 
            return it->deque->blocks[it->blockIndex][it->indexInBlock];
        }
        else { 
            fmt_fprintf(stderr, "Error: Iterator out of bounds in iterator_get.\n");
            return NULL;
        }
    } 
    else {
        // Handle forward iteration
        if (it->blockIndex < it->deque->blockCount) { 
            return it->deque->blocks[it->blockIndex][it->indexInBlock];
        }
        else {
            fmt_fprintf(stderr, "Error: Iterator out of bounds in iterator_get.\n");
            return NULL;
        }
    }
}