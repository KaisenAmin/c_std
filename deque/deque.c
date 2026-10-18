/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Deque
*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "deque.h"


/* ------------------------------------------------------------------ */
/* Inline-storage helpers                                             */
/* ------------------------------------------------------------------ */
/*
 * Elements live INLINE inside the block buffers (like std::deque): each block
 * is one contiguous malloc of blockSize*itemSize bytes, NOT an array of pointers
 * to individually-heap-allocated elements. That turns a push into a single copy
 * into the block (no per-element malloc) and a pop into an index update (no
 * per-element free), and makes random access one indirection instead of two.
 *
 * dq_slot() maps a flat position (frontIndex + logical index) to the address of
 * that element's bytes inside its block. dq_store() copies one element with a
 * size-specialized branch so the common element sizes compile to a single
 * load/store move instead of a memcpy call with a runtime length. Source and
 * destination are interior buffer pointers (no compile-time object-size info),
 * so no -Warray-bounds pragma is needed. The `dq_` prefix keeps these distinct
 * from the public `deque_` API.
 */
static inline void* dq_slot(const Deque* d, size_t flatPos) {
    return (char*)d->blocks[flatPos >> d->blockShift] + (flatPos & (d->blockSize - 1)) * d->itemSize;
}

/* The block pointers live in `map`, a window of which is in use: blocks ==
 * map + offset. Spare slots are kept on BOTH sides (like std::deque's map),
 * so a new block at either end usually takes a free slot in O(1). Only when
 * that side has none left is the window recentred in place (when at least
 * half of the map is free) or moved to a map twice the size: amortized O(1).
 * (The array used to grow by exactly one slot per block, and push_front
 * memmoved every pointer each time, O(n) per block.) Returns false when out
 * of memory; nothing is changed then. */
static bool dq_map_make_room(Deque* d, bool atFront) {
    size_t off = (size_t)(d->blocks - d->map);
    size_t frontSpare = off;
    size_t backSpare = d->mapCapacity - off - d->blockCount;
    if (atFront ? frontSpare > 0 : backSpare > 0) {
        return true;
    }
    size_t need = d->blockCount + 1;
    if (need <= d->mapCapacity / 2) {
        size_t newOff = (d->mapCapacity - need) / 2 + (atFront ? 1 : 0);
        memmove(d->map + newOff, d->blocks, sizeof(void*) * d->blockCount);
        d->blocks = d->map + newOff;
        return true;
    }
    if (d->mapCapacity > SIZE_MAX / (4 * sizeof(void*))) {
        return false;
    }
    size_t newCap = d->mapCapacity * 2;
    while (newCap < need * 2) {
        newCap *= 2;
    }
    void** nm = (void**)malloc(newCap * sizeof(void*));
    if (!nm) {
        return false;
    }
    size_t newOff = (newCap - need) / 2 + (atFront ? 1 : 0);
    memcpy(nm + newOff, d->blocks, sizeof(void*) * d->blockCount);
    free(d->map);
    d->map = nm;
    d->mapCapacity = newCap;
    d->blocks = nm + newOff;
    return true;
}

/* Appends a fresh block after the last one; false when out of memory. */
static bool dq_add_back_block(Deque* d) {
    if (!dq_map_make_room(d, false)) {
        return false;
    }
    void* nb = malloc(d->blockSize * d->itemSize);
    if (!nb) {
        return false;
    }
    d->blocks[d->blockCount] = nb;
    d->blockCount++;
    return true;
}

/* The canonical empty state: one block (the current first one), placed in the
   middle of the map so both ends have spare slots, front/back in its middle. */
static void dq_reset_empty(Deque* d) {
    for (size_t i = 1; i < d->blockCount; ++i) {
        free(d->blocks[i]);
        d->blocks[i] = NULL;
    }
    void* keep = d->blocks[0];
    d->blocks = d->map + d->mapCapacity / 2;
    d->blocks[0] = keep;
    d->blockCount = 1;
    d->size = 0;
    d->frontIndex = d->blockSize / 2;
    d->backIndex = d->frontIndex - 1;
}

static inline void dq_store(void* dst, const void* src, size_t n) {
    switch (n) {
        case 1:  memcpy(dst, src, 1);  break;
        case 2:  memcpy(dst, src, 2);  break;
        case 4:  memcpy(dst, src, 4);  break;
        case 8:  memcpy(dst, src, 8);  break;
        case 16: memcpy(dst, src, 16); break;
        default: memcpy(dst, src, n);  break;
    }
}


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

    deque->itemSize = itemSize;
    /* Elements per block: the largest power of two whose block stays within a
       ~2KB byte budget. Small elements therefore share big blocks (far fewer
       allocations, like std::deque's byte-budgeted nodes), while large elements
       drop to two elements per block instead of over-allocating. Always >= 2:
       with a single slot per block the empty state (front = middle, back =
       front - 1) has no valid middle and backIndex underflowed to SIZE_MAX,
       which later corrupted the heap for elements larger than 1 KiB.
       A power-of-two count makes block index/offset a shift/mask, not a divide. */
    {
        size_t fit = (size_t)2048 / itemSize;   /* whole elements within the budget */
        size_t bs = 2, shift = 1;
        while ((bs << 1) <= fit) {
            bs <<= 1;
            shift++;
        }
        deque->blockSize  = bs;
        deque->blockShift = shift;
    }
    deque->compare = NULL;
    deque->size = 0;
    deque->blockCount = 1; // Start with one block
    deque->frontIndex = deque->blockSize / 2; // Middle of the block
    deque->backIndex = deque->frontIndex - 1;

    deque->mapCapacity = 8;
    deque->map = (void**)malloc(sizeof(void*) * deque->mapCapacity);
    if (!deque->map) {
        DEQUE_LOG("[deque_create] Error: Memory allocation failed for blocks array.");
        free(deque);
        return NULL;
    }
    deque->blocks = deque->map + deque->mapCapacity / 2;   /* spare slots on both sides */

    deque->blocks[0] = malloc(deque->blockSize * deque->itemSize);
    if (!deque->blocks[0]) {
        DEQUE_LOG("[deque_create] Error: Memory allocation failed for the first block.");
        free(deque->map);
        free(deque);
        return NULL;
    }

    DEQUE_LOG("[deque_create] Deque created successfully with blockSize: %zu, frontIndex: %zu, backIndex: %zu.", deque->blockSize, deque->frontIndex, deque->backIndex);
    return deque;
}


/* deque_empty() and deque_length() are defined static inline in deque.h. */


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
    if (!deque || !item) {
        DEQUE_LOG("[deque_push_front] Error: Deque or item is NULL.");
        return;
    }

    // If there is no room at the front of block 0, prepend a new block (in a
    // spare map slot: no pointer moves, amortized O(1)).
    if (deque->frontIndex == 0) {
        if (!dq_map_make_room(deque, true)) {
            DEQUE_LOG("[deque_push_front] Error: Memory allocation failed for blocks array.");
            return;
        }
        void* newBlock = malloc(deque->blockSize * deque->itemSize);
        if (!newBlock) {
            DEQUE_LOG("[deque_push_front] Error: Memory allocation failed for new block.");
            return;
        }
        deque->blocks--;
        deque->blocks[0] = newBlock;
        deque->blockCount++;
        deque->frontIndex = deque->blockSize;
        DEQUE_LOG("[deque_push_front] New block allocated. blockCount: %zu.", deque->blockCount);
    }

    deque->frontIndex--;
    dq_store(dq_slot(deque, deque->frontIndex), item, deque->itemSize);
    deque->size++;

    DEQUE_LOG("[deque_push_front] Item copied at the front. frontIndex: %zu, size: %zu.", deque->frontIndex, deque->size);
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
        if (!dq_add_back_block(deque)) {
            DEQUE_LOG("[deque_push_back] Error: Memory allocation failed for the new block.");
            return;
        }
        deque->backIndex = (size_t)-1;
        DEQUE_LOG("[deque_push_back] New block allocated. blockCount: %zu.", deque->blockCount);
    }

    // Insert the new item at the back (copied inline into the block; no per-element malloc)
    deque->backIndex++;
    dq_store((char*)deque->blocks[deque->blockCount - 1] + deque->backIndex * deque->itemSize,
             item, deque->itemSize);
    deque->size++;
    DEQUE_LOG("[deque_push_back] Item inserted at the back. New backIndex: %zu, size: %zu.", deque->backIndex, deque->size);
}


/* deque_front() and deque_back() are defined static inline in deque.h. */


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

    // Elements live inline in the block buffers, so popping is just an index
    // bump — there is no per-element heap copy to free (dead front BLOCKS are
    // reclaimed in bulk by the amortized compaction below).
    deque->frontIndex++;
    deque->size--;

    if (deque->size == 0) {
        dq_reset_empty(deque);
        DEQUE_LOG("[deque_pop_front] Deque is now empty.");
    }
    else if (deque->frontIndex == deque->blockSize) {
        // The front block is used up: free it and move the window of used map
        // slots one forward. O(1), and no other block pointer moves, so
        // iterators to the remaining elements stay valid (a bulk compaction
        // memmoved the pointer array and invalidated all of them).
        free(deque->blocks[0]);
        deque->blocks[0] = NULL;
        deque->blocks++;
        deque->blockCount--;
        deque->frontIndex = 0;
        DEQUE_LOG("[deque_pop_front] Released the front block. New blockCount: %zu.", deque->blockCount);
    }
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
    // Inline storage: nothing to free per element; just drop the back index.
    deque->size--;

    if (deque->size == 0) {
        dq_reset_empty(deque);
        DEQUE_LOG("[deque_pop_back] Deque is now empty.");
    }
    else if (deque->backIndex > 0) {
        deque->backIndex--;
    }
    else {
        // backIndex == 0 and there are still elements
        if (deque->blockCount > 1) {
            free(deque->blocks[deque->blockCount - 1]);
            deque->blockCount--;
        }
        deque->backIndex = deque->blockSize - 1;
        DEQUE_LOG("[deque_pop_back] Removed last block. New blockCount: %zu.", deque->blockCount);
    }
    DEQUE_LOG("[deque_pop_back] Back element popped. New size: %zu.", deque->size);
}


/* deque_at() is defined static inline in deque.h. */


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

    // Elements are stored inline, so there are no per-element allocations to
    // free — just release the block buffers (keeping block 0 for reuse).
    if (deque->blocks != NULL) {
        dq_reset_empty(deque);
    }

    DEQUE_LOG("[deque_clear] Deque cleared. Ready for new pushes.");
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

    deque_clear(deque);

    if (deque->blocks) {
        for (size_t i = 0; i < deque->blockCount; ++i) {
            free(deque->blocks[i]);
        }
        free(deque->map);
        deque->map = NULL;
        deque->blocks = NULL;
    }
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

    size_t firstBlock = deque->frontIndex / deque->blockSize;
    size_t lastBlock  = (deque->frontIndex + deque->size - 1) / deque->blockSize;
    size_t liveBlocks = lastBlock - firstBlock + 1;
    DEQUE_LOG("[deque_shrink_to_fit] blockCount: %zu, live blocks: %zu (first: %zu, last: %zu).", deque->blockCount, liveBlocks, firstBlock, lastBlock);

    if (firstBlock == 0 && liveBlocks == deque->blockCount) {
        DEQUE_LOG("[deque_shrink_to_fit] No shrinking needed. Deque is already optimal.");
        return;
    }

    // Release dead front blocks, then dead back blocks.
    for (size_t i = 0; i < firstBlock; ++i) {
        free(deque->blocks[i]);
    }
    for (size_t i = lastBlock + 1; i < deque->blockCount; ++i) {
        free(deque->blocks[i]);
    }

    // Move the live block pointers into a map of exactly liveBlocks slots
    // (keep the old, still valid map, with the live window, if that fails).
    void** fitted = (void**)malloc(sizeof(void*) * liveBlocks);
    if (fitted) {
        memcpy(fitted, deque->blocks + firstBlock, sizeof(void*) * liveBlocks);
        free(deque->map);
        deque->map = fitted;
        deque->mapCapacity = liveBlocks;
        deque->blocks = fitted;
    }
    else {
        deque->blocks += firstBlock;
    }
    deque->blockCount = liveBlocks;

    // The front element now lives in block 0; rebase the global front/back offsets.
    deque->frontIndex = deque->frontIndex % deque->blockSize;
    deque->backIndex  = (deque->frontIndex + deque->size - 1) % deque->blockSize;
    DEQUE_LOG("[deque_shrink_to_fit] Shrunk to %zu blocks. frontIndex: %zu, backIndex: %zu.", deque->blockCount, deque->frontIndex, deque->backIndex);
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
/* If `p` points into one of the deque's blocks, returns the flat slot of the
   element it points into and stores the byte offset inside that element;
   otherwise returns SIZE_MAX. */
static size_t dq_flat_slot_of(const Deque* deque, const void* p, size_t* inner) {
    uintptr_t up = (uintptr_t)p;
    size_t blockBytes = deque->blockSize * deque->itemSize;
    for (size_t b = 0; b < deque->blockCount; ++b) {
        uintptr_t base = (uintptr_t)deque->blocks[b];
        if (deque->blocks[b] && up >= base && up < base + blockBytes) {
            size_t off = (size_t)(up - base);
            *inner = off % deque->itemSize;
            return b * deque->blockSize + off / deque->itemSize;
        }
    }
    return SIZE_MAX;
}

void deque_insert(Deque* deque, size_t index, void* item) {
    if (!deque || !item) {
        DEQUE_LOG("[deque_insert] Error: Deque or item is NULL.");
        return;
    }
    if (index > deque->size) {
        DEQUE_LOG("[deque_insert] Error: Index out of bounds.");
        return;
    }
    if (index == 0) {
        deque_push_front(deque, item);
        return;
    }
    if (index == deque->size) {
        deque_push_back(deque, item);
        return;
    }

    // Reallocate if more blocks are needed.
    // The shift loop below writes the new back element to flat index
    // (frontIndex + size); a new block is required once that index reaches the
    // current capacity. Because frontIndex starts at blockSize/2 (> 0), this
    // boundary is hit before `size` alone equals the capacity, so the guard
    // must include frontIndex — otherwise the shift writes to blocks[blockCount]
    // (one past the array) and corrupts the heap.
    if (deque->frontIndex + deque->size >= deque->blockSize * deque->blockCount) {
        DEQUE_LOG("[deque_insert] Allocating new block.");
        if (!dq_add_back_block(deque)) {
            DEQUE_LOG("[deque_insert] Error: Memory allocation failed.");
            return;
        }
    }

    /* `item` may be an element of this deque (d.insert(i, d[j])): the shift
       below moves element j >= index one slot right, so read it from there
       (it used to insert the shifted neighbour's value instead). */
    const void* src = item;
    size_t inner = 0;
    size_t flat = dq_flat_slot_of(deque, item, &inner);
    if (flat != SIZE_MAX && flat >= deque->frontIndex + index && flat < deque->frontIndex + deque->size) {
        src = (const char*)dq_slot(deque, flat + 1) + inner;
    }

    // Shift elements right by one (copying their bytes) to open a hole at index.
    for (size_t i = deque->size; i > index; --i) {
        dq_store(dq_slot(deque, deque->frontIndex + i),
                 dq_slot(deque, deque->frontIndex + i - 1),
                 deque->itemSize);
    }

    // Insert the item inline (no per-element malloc).
    dq_store(dq_slot(deque, deque->frontIndex + index), src, deque->itemSize);
    deque->size++;
    deque->backIndex = (deque->frontIndex + deque->size - 1) % deque->blockSize;
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
    if (!deque || index >= deque->size) {
        DEQUE_LOG("[deque_erase] Error: Invalid deque or index.");
        return;
    }

    DEQUE_LOG("[deque_erase] Removing element at index: %zu.", index);

    for (size_t i = index + 1; i < deque->size; ++i) {
        void* dst = deque_at(deque, i - 1);
        void* src = deque_at(deque, i);
        if (dst && src) {
            memcpy(dst, src, deque->itemSize);
        }
    }

    // Elements are stored inline; the shifted-over tail slot holds a leftover
    // duplicate that is simply dropped by the size decrement — nothing to free.
    deque->size--;

    if (deque->size == 0) {
        /* Drop any trailing blocks and reset to canonical empty state so
           future pushes have headroom at both ends. */
        dq_reset_empty(deque);
        DEQUE_LOG("[deque_erase] Deque is now empty.");
        return;
    }

    /* Recompute backIndex / blockCount from the (contiguous-index) layout. */
    size_t newLastAbs   = deque->frontIndex + deque->size - 1;
    size_t newLastBlock = newLastAbs / deque->blockSize;
    deque->backIndex    = newLastAbs % deque->blockSize;

    while (deque->blockCount > newLastBlock + 1) {
        free(deque->blocks[deque->blockCount - 1]);
        deque->blockCount--;
    }

    DEQUE_LOG("[deque_erase] Element removed. New size: %zu, backIndex: %zu, blockCount: %zu.", deque->size, deque->backIndex, deque->blockCount);
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
 *
 * @note If memory runs out while growing, the deque keeps the elements added
 * so far (check deque_length); it used to retry forever.
 */
void deque_resize(Deque* deque, size_t newSize) {
    if (!deque) {
        DEQUE_LOG("[deque_resize] Error: Deque is NULL.");
        return; 
    }

    DEQUE_LOG("[deque_resize] Resizing deque from size: %zu to new size: %zu.", deque->size, newSize);

    // Expand: append zero-initialized items at the back.
    if (newSize > deque->size) {
        char* zero = (char*)calloc(1, deque->itemSize);
        if (!zero) {
            return;
        }
        while (deque->size < newSize) {
            size_t before = deque->size;
            deque_push_back(deque, zero);
            if (deque->size == before) {
                break;   /* allocation failed: stop instead of retrying forever */
            }
        }
        free(zero);
    }
    // Shrink: pop from the back (free heap copies).
    while (deque->size > newSize) {
        deque_pop_back(deque);
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
    if (!deque || !val) {
        DEQUE_LOG("[deque_assign] Error: Deque or val is NULL.");
        return;
    }

    DEQUE_LOG("[deque_assign] Assigning %zu elements to the deque.", n);

    deque_clear(deque);

    for (size_t i = 0; i < n; ++i) {
        size_t before = deque->size;
        deque_push_back(deque, val);
        if (deque->size == before) {
            break;   /* out of memory */
        }
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

    if (deque->backIndex == deque->blockSize - 1) {
        if (!dq_add_back_block(deque)) {
            DEQUE_LOG("[deque_emplace_back] Error: Memory allocation for new block failed.");
            return;
        }
        deque->backIndex = (size_t)-1;
    }

    deque->backIndex++;
    dq_store((char*)deque->blocks[deque->blockCount - 1] + deque->backIndex * deque->itemSize,
             item, deque->itemSize);
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

    deque_push_front(deque, item);
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
    /* `emplace` in this C deque has the same observable semantics as
       `insert`: it copies the bytes pointed to by `item` into a freshly
       owned heap slot.  Delegating keeps both paths identical and
       avoids duplicating the ownership/bookkeeping rules. */
    deque_insert(deque, index, item);
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
 * @brief Sets the element comparator used by deque_is_equal / is_less / ... .
 *
 * Without one the elements' bytes are compared with memcmp, which is not the
 * numeric order of int, negative or floating-point elements ({256} < {1} on
 * little-endian). When only one of two deques has a comparator, it is used.
 *
 * @param deque   The deque (NULL is a no-op).
 * @param compare qsort-style comparator, or NULL for byte comparison.
 */
void deque_set_compare(Deque* deque, DequeCompareFunc compare) {
    if (deque) {
        deque->compare = compare;
    }
}


/**
 * @brief Single-pass lexicographic comparison of two deques' element sequences.
 *
 * Returns a negative value if deque1 < deque2, zero if they are equal, and a
 * positive value if deque1 > deque2. Callers guarantee both pointers are
 * non-NULL and share the same itemSize. The ordering predicates use this so a
 * query like is_less_or_equal walks the elements ONCE instead of calling
 * deque_is_less and deque_is_equal back to back (which scanned twice).
 *
 * @param deque1 Pointer to the first deque.
 * @param deque2 Pointer to the second deque.
 * @return <0, 0, or >0 per lexicographic order (length breaks ties).
 */
static int dq_lex_compare(const Deque* deque1, const Deque* deque2) {
    size_t n1 = deque1->size;
    size_t n2 = deque2->size;
    size_t minN = (n1 < n2) ? n1 : n2;
    DequeCompareFunc compare = deque1->compare ? deque1->compare : deque2->compare;

    for (size_t i = 0; i < minN; ++i) {
        const void* a = deque_at(deque1, i);
        const void* b = deque_at(deque2, i);
        if (!a || !b) {
            return 0;
        }
        int cmp = compare ? compare(a, b) : memcmp(a, b, deque1->itemSize);
        if (cmp != 0) {
            return (cmp < 0) ? -1 : 1;
        }
    }
    if (n1 != n2) {
        return (n1 < n2) ? -1 : 1;
    }
    return 0;
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
    if (deque1->itemSize != deque2->itemSize) {
        return false;
    }

    if (deque1->compare || deque2->compare) {
        return dq_lex_compare(deque1, deque2) == 0;
    }
    for (size_t i = 0; i < deque1->size; ++i) {
        void* a = deque_at(deque1, i);
        void* b = deque_at(deque2, i);
        if (!a || !b || memcmp(a, b, deque1->itemSize) != 0) {
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

    if (deque1->itemSize != deque2->itemSize) {
        return false;
    }

    bool result = dq_lex_compare(deque1, deque2) < 0;
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
    if (!deque1 || !deque2) {
        return false;
    }
    if (deque1->itemSize != deque2->itemSize) {
        return false;
    }
    return dq_lex_compare(deque1, deque2) > 0;
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
    if (deque1 == deque2) {
        return true;
    }
    if (!deque1 || !deque2) {
        return false;
    }
    if (deque1->itemSize != deque2->itemSize) {
        return false;
    }
    return dq_lex_compare(deque1, deque2) <= 0;
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
    if (deque1 == deque2) {
        return true;
    }
    if (!deque1 || !deque2) {
        return false;
    }
    if (deque1->itemSize != deque2->itemSize) {
        return false;
    }
    return dq_lex_compare(deque1, deque2) >= 0;
}


/* Iterator positions are flat indices counted from the start of the block
 * MAP, not from blocks[0]. pop_front / push_front / pop_back / push_back only
 * move the window of used map slots, so they leave these positions alone and
 * an iterator to an element that is still there stays valid (pop_front used
 * to compact the pointer array and push_front to shift it, which moved every
 * iterator to another element). What does invalidate iterators: a push that
 * needs a new map (or recentres it), insert / erase / emplace (elements
 * shift), resize, assign, clear, shrink_to_fit, swap, and the deque becoming
 * empty. */
static size_t dq_abs_front(const Deque* d) {
    return ((size_t)(d->blocks - d->map) << d->blockShift) + d->frontIndex;
}

/* Position relative to the front element: 0 .. size-1 are elements, size is
   the end; anything else (e.g. SIZE_MAX) is the position before the front. */
static size_t dq_iter_rel(const DequeIterator* it) {
    size_t abs = (it->blockIndex << it->deque->blockShift) | it->indexInBlock;
    size_t rel = abs - dq_abs_front(it->deque);   /* wraps below the front */
    if (rel > it->deque->size && rel <= SIZE_MAX / 2) {
        rel = it->deque->size;                    /* past the back: the end */
    }
    return rel;
}

#define DQ_BEFORE_FRONT SIZE_MAX

static void dq_iter_place(DequeIterator* it, size_t rel) {
    const Deque* d = it->deque;
    size_t abs = dq_abs_front(d) + rel;           /* wraps for DQ_BEFORE_FRONT */
    it->blockIndex = abs >> d->blockShift;
    it->indexInBlock = abs & (d->blockSize - 1);
    it->current = (rel < d->size) ? (char*)d->map[it->blockIndex] + it->indexInBlock * d->itemSize : NULL;
}

/* One step toward the back / toward the front, stopping at the end and at
   the position before the front. */
static void dq_iter_step_back(DequeIterator* it) {
    size_t rel = dq_iter_rel(it);
    if (rel > it->deque->size) {
        rel = 0;                                  /* before the front -> first */
    }
    else if (rel < it->deque->size) {
        rel++;
    }
    dq_iter_place(it, rel);
}

static void dq_iter_step_front(DequeIterator* it) {
    size_t rel = dq_iter_rel(it);
    if (rel > it->deque->size) {
        rel = DQ_BEFORE_FRONT;                    /* stays before the front */
    }
    else {
        rel = (rel == 0) ? DQ_BEFORE_FRONT : rel - 1;
    }
    dq_iter_place(it, rel);
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
    it.deque = deque;
    dq_iter_place(&it, 0);   /* the end when empty */
    DEQUE_LOG("[deque_begin] Iterator to first element created. blockIndex: %zu, indexInBlock: %zu", it.blockIndex, it.indexInBlock);
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
    it.deque = deque;
    dq_iter_place(&it, deque->size);
    DEQUE_LOG("[deque_end] Iterator to past-the-end element created. blockIndex: %zu, indexInBlock: %zu", it.blockIndex, it.indexInBlock);
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
    it.deque = deque;
    it.isReverse = true;
    dq_iter_place(&it, deque->size > 0 ? deque->size - 1 : DQ_BEFORE_FRONT);
    DEQUE_LOG("[deque_rbegin] Reverse iterator created at blockIndex: %zu, indexInBlock: %zu", it.blockIndex, it.indexInBlock);
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
    it.deque = deque;
    it.isReverse = true;
    dq_iter_place(&it, DQ_BEFORE_FRONT);
    DEQUE_LOG("[deque_rend] Reverse end iterator created.");
    return it;
}


/**
 * @brief Returns a constant iterator to the first element of the deque.
 *
 * @param deque Pointer to the deque for which the constant begin iterator is created.
 * @return A constant DequeIterator pointing to the first element of the deque.
 */
DequeIterator deque_cbegin(const Deque* deque) {
    return deque_begin(deque);
}


/**
 * @brief Returns a constant iterator to the position after the last element of the deque.
 *
 * @param deque Pointer to the deque for which the constant end iterator is created.
 * @return A constant DequeIterator representing the end position of the deque.
 */
DequeIterator deque_cend(const Deque* deque) {
    return deque_end(deque);
}


/**
 * @brief Returns a constant reverse iterator to the last element of the deque.
 *
 * @param deque Pointer to the deque for which the constant reverse begin iterator is created.
 * @return A constant DequeIterator pointing to the last element of the deque.
 */
DequeIterator deque_crbegin(const Deque* deque) {
    return deque_rbegin(deque);
}


/**
 * @brief Returns a constant reverse iterator to the position before the first element of the deque.
 *
 * @param deque Pointer to the deque for which the constant reverse end iterator is created.
 * @return A constant DequeIterator representing the reverse end position of the deque.
 */
DequeIterator deque_crend(const Deque* deque) {
    return deque_rend(deque);
}


/**
 * @brief Advances the iterator: one element toward the back for a forward
 *        iterator, one toward the front for a reverse iterator.
 *
 * At the end (forward) or at rend (reverse) it stays there, with
 * `current == NULL`.
 *
 * @param it Pointer to the iterator to advance.
 */
void iterator_increment(DequeIterator* it) {
    if (!it || !it->deque) {
        DEQUE_LOG("[iterator_increment] Error: Iterator or Deque is NULL.");
        return;
    }
    if (it->isReverse) {
        dq_iter_step_front(it);
    }
    else {
        dq_iter_step_back(it);
    }
}


/**
 * @brief Moves the iterator back: one element toward the front for a forward
 *        iterator, one toward the back for a reverse iterator.
 *
 * Decrementing the end gives the last element, and decrementing rend (a
 * reverse iterator) gives the first. Moving past the first element (forward)
 * or past the last one (reverse) yields the matching sentinel with
 * `current == NULL` (it used to ignore isReverse, and decrementing rend left
 * a wild non-NULL `current`).
 *
 * @param it Pointer to the iterator to decrement.
 */
void iterator_decrement(DequeIterator* it) {
    if (!it || !it->deque) {
        DEQUE_LOG("[iterator_decrement] Error: Iterator or Deque is NULL.");
        return;
    }
    if (it->isReverse) {
        dq_iter_step_back(it);
    }
    else {
        dq_iter_step_front(it);
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
 * @param it Pointer to the iterator from which to retrieve the value.
 *
 * @return A pointer to the value at the current position of the iterator, or NULL if the
 *         iterator is at the end / rend or no longer addresses an element.
 */
void* iterator_get(const DequeIterator* it) {
    if (!it || !it->deque) {
        DEQUE_LOG("[iterator_get] Error: Iterator or Deque is NULL.");
        return NULL;
    }
    size_t rel = dq_iter_rel(it);
    if (rel >= it->deque->size) {
        DEQUE_LOG("[iterator_get] Iterator is not on an element.");
        return NULL;
    }
    const Deque* d = it->deque;
    return (char*)d->map[it->blockIndex] + it->indexInBlock * d->itemSize;
}
