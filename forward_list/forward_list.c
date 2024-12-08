/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class ForwardList
*/

#include <stdlib.h>
#include <string.h>
#include "forward_list.h"


/**
 * @brief Creates a new ForwardList.
 *
 * This function allocates memory for a new ForwardList structure and initializes it.
 * The list is created to hold elements of size `itemSize`. If `itemSize` is zero or
 * if memory allocation fails, the function prints an error message and terminates
 * the program.
 *
 * @param itemSize The size, in bytes, of each element that will be stored in the list.
 * @return ForwardList* Pointer to the newly created ForwardList.
 * @note If `itemSize` is zero or if memory allocation fails, the function will exit the program with an error.
 */
ForwardList *forward_list_create(size_t itemSize) {
    if (itemSize == 0) {
        FORWARD_LIST_LOG("[forward_list_create] Error: Item size cannot be zero.");
        exit(-1);
    }

    ForwardList *list = (ForwardList*) malloc(sizeof(ForwardList));
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_create] Error: Memory allocation failed for ForwardList.");
        exit(-1);
    }

    // Initialize the list
    list->head = NULL;
    list->itemSize = itemSize;
    list->size = 0;

    FORWARD_LIST_LOG("[forward_list_create] ForwardList created with item size: %zu", itemSize);
    return list;
}

// Helper function to split the list into two halves
static ForwardListNode *split_list_for_sort(ForwardListNode *head) {
    if (!head) {
        FORWARD_LIST_LOG("[split_list_for_sort] Error: NULL head.");
        return NULL;
    }

    ForwardListNode *fast = head, *slow = head, *prev = NULL;
    while (fast != NULL && fast->next != NULL) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    if (prev != NULL) {
        prev->next = NULL;  // Split the list into two halves
    }

    FORWARD_LIST_LOG("[split_list_for_sort] List split for sorting.");
    return slow;
}

// Merge two sorted lists
static ForwardListNode *merge_sorted_lists(ForwardListNode *a, ForwardListNode *b, size_t itemSize) {
    if (!a || !b) {
        FORWARD_LIST_LOG("[merge_sorted_lists] NULL list node(s).");
        return NULL;
    }
    ForwardListNode dummy;
    ForwardListNode *tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL) {
        if (memcmp(a->value, b->value, itemSize) <= 0) {
            tail->next = a;
            a = a->next;
        } 
        else {
            tail->next = b;
            b = b->next;
        }

        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;

    FORWARD_LIST_LOG("[merge_sorted_lists] Merged two lists.");
    return dummy.next;
}

// Recursive merge sort implementation
static ForwardListNode *merge_sort(ForwardListNode *head, size_t itemSize) {
    if (!head) {
        FORWARD_LIST_LOG("[merge_sort] NULL head.");
        return NULL;
    }

    ForwardListNode *middle = split_list_for_sort(head);
    ForwardListNode *left = merge_sort(head, itemSize);
    ForwardListNode *right = merge_sort(middle, itemSize);

    FORWARD_LIST_LOG("[merge_sort] Sorted and merged two halves.");
    return merge_sorted_lists(left, right, itemSize);
}

/**
 * @brief Adds a new element to the front of the ForwardList.
 *
 * This function creates a new node with the provided `value` and inserts it at the
 * front of the `list`. The list's size is increased by one.
 *
 * @param list Pointer to the ForwardList where the element will be added.
 * @param value Pointer to the value to be added at the front of the list.
 */
void forward_list_push_front(ForwardList *list, void *value) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_push_front] NULL list pointer.");
        return;
    }
    if (!value) {
        FORWARD_LIST_LOG("[forward_list_push_front] NULL value pointer.");
        return;
    }

    ForwardListNode *newNode = (ForwardListNode*) malloc(sizeof(ForwardListNode));
    if (!newNode) {
        FORWARD_LIST_LOG("[forward_list_push_front] Memory allocation failed for new node.");
        return;
    }

    newNode->value = malloc(list->itemSize);
    if (!newNode->value) {
        FORWARD_LIST_LOG("[forward_list_push_front] Memory allocation failed for node value.");
        free(newNode);
        return;
    }
    memcpy(newNode->value, value, list->itemSize);

    newNode->next = list->head;
    list->head = newNode;
    list->size++;

    FORWARD_LIST_LOG("[forward_list_push_front] Added new element to the front. New size: %zu", list->size);
}

/**
 * @brief Adds a new element to the front of the ForwardList.
 *
 * This function creates a new node with the provided `value` and inserts it at the
 * front of the `list`. The list's size is increased by one.
 *
 * @param list Pointer to the ForwardList where the element will be added.
 * @param value Pointer to the value to be added at the front of the list.
 */
void forward_list_pop_front(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_pop_front] NULL list pointer.");
        return;
    }
    if (!list->head) {
        FORWARD_LIST_LOG("[forward_list_pop_front] Warning: Attempting to pop from an empty list.");
        return;
    }

    ForwardListNode *temp = list->head;
    list->head = list->head->next;

    free(temp->value);
    free(temp);
    list->size--;
    FORWARD_LIST_LOG("[forward_list_pop_front] Popped element. New size: %zu", list->size);
}

/**
 * @brief Retrieves the first element of the ForwardList.
 *
 * This function returns a pointer to the value stored in the first node of the `list`.
 * If the list is empty, it returns `NULL`.
 *
 * @param list Pointer to the ForwardList.
 * @return void* Pointer to the value at the front of the list, or `NULL` if the list is empty.
 */
void *forward_list_front(const ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_front] NULL list pointer.");
        return NULL;
    }
    if (list->head == NULL) {
        FORWARD_LIST_LOG("[forward_list_front] Warning: Attempting to access the front of an empty list.");
        return NULL;
    }

    FORWARD_LIST_LOG("[forward_list_front] Returning the front element.");
    return list->head->value;
}

/**
 * @brief Removes all elements from the ForwardList.
 *
 * This function clears the `list` by deallocating the memory of all its nodes, effectively
 * making the list empty. The list's size is set to zero.
 *
 * @param list Pointer to the ForwardList to be cleared.
 */
void forward_list_clear(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_clear] NULL list pointer.");
        return;
    }

    ForwardListNode *current = list->head;
    size_t nodesCleared = 0;

    while (current != NULL) {
        ForwardListNode *next = current->next;

        free(current->value);
        free(current);
        current = next;
        nodesCleared++;
    }

    list->head = NULL;
    list->size = 0;
    FORWARD_LIST_LOG("[forward_list_clear] Cleared %zu nodes. List is now empty.", nodesCleared);
}

/**
 * @brief Checks whether the ForwardList is empty.
 *
 * This function returns `true` if the `list` is empty (i.e., contains no elements),
 * otherwise it returns `false`. If the list pointer is `NULL`, it is considered empty.
 *
 * @param list Pointer to the ForwardList.
 * @return bool `true` if the list is empty, `false` otherwise.
 */
bool forward_list_empty(const ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_empty] NULL list pointer.");
        return true;  // Consider a NULL list as empty
    }
    FORWARD_LIST_LOG("[forward_list_empty] List is %s.", list->head == NULL ? "empty" : "not empty");
    return list->head == NULL;
}

/**
 * @brief Returns the number of elements in the ForwardList.
 *
 * This function returns the size of the `list`, representing the number of elements
 * currently stored in the list.
 *
 * @param list Pointer to the ForwardList.
 * @return size_t The number of elements in the list.
 */
size_t forward_list_length(const ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_length] NULL list pointer.");
        return 0;
    }
    FORWARD_LIST_LOG("[forward_list_length] List size: %zu", list->size);
    return list->size;
}

/**
 * @brief Deallocates the memory associated with the ForwardList.
 *
 * This function deallocates all the memory associated with the `list`, including all its nodes.
 * It also clears the list before deallocating the list structure itself.
 *
 * @param list Pointer to the ForwardList to be deallocated.
 */
void forward_list_deallocate(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_deallocate] NULL list pointer.");
        return;
    }
    FORWARD_LIST_LOG("[forward_list_deallocate] Deallocating list.");
    forward_list_clear(list);  // Clear all nodes
    free(list);
    FORWARD_LIST_LOG("[forward_list_deallocate] List deallocated.");
}

/**
 * @brief Assigns new values to the ForwardList, replacing its current contents.
 *
 * This function replaces the current contents of the `list` with `numValues` elements
 * from the array pointed to by `values`. The list is cleared before the new values are added.
 * The order of elements in the list is maintained as per the order in the input array.
 *
 * @param list Pointer to the ForwardList to be assigned new values.
 * @param values Pointer to the array containing new values to be assigned.
 * @param numValues The number of values in the array.
 */
void forward_list_assign(ForwardList *list, void *values, size_t numValues) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_assign] NULL list pointer.");
        return;
    }
    if (!values) {
        FORWARD_LIST_LOG("[forward_list_assign] NULL values pointer.");
        return;
    }
    FORWARD_LIST_LOG("[forward_list_assign] Assigning %zu new values to the list.", numValues);
    forward_list_clear(list);  // Clear existing contents
    for (size_t i = 0; i < numValues; ++i) {
        void *value = (char *)values + i * list->itemSize; // Calculate the address of the value to be copied
        forward_list_push_front(list, value);  
    }

    forward_list_reverse(list); // Reverse the list to maintain the correct order
    FORWARD_LIST_LOG("[forward_list_assign] New values assigned and list order maintained.");
}

/**
 * @brief Returns a pointer to the node before the beginning of the ForwardList.
 *
 * This function returns a pointer to the node before the beginning of the `list`. In a singly
 * linked list, there is no node before the beginning, so this function returns `NULL`.
 *
 * @param list Pointer to the ForwardList.
 * @return ForwardListNode* A pointer to the node before the beginning, which is `NULL`.
 */
ForwardListNode *forward_list_before_begin(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_before_begin] NULL list pointer.");
        return NULL;
    }
    FORWARD_LIST_LOG("[forward_list_before_begin] Returning NULL as there is no node before the beginning.");
    return NULL; // In a singly linked list, there is no node before the beginning
}

/**
 * @brief Returns a pointer to the first node in the ForwardList.
 *
 * This function returns a pointer to the first node (head) in the `list`.
 *
 * @param list Pointer to the ForwardList.
 * @return ForwardListNode* A pointer to the first node in the list.
 */
ForwardListNode *forward_list_begin(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_begin] NULL list pointer.");
        return NULL;
    }
    FORWARD_LIST_LOG("[forward_list_begin] Returning head of the list.");
    return list->head;
}

/**
 * @brief Returns a pointer to the end of the ForwardList.
 *
 * This function returns `NULL` to represent the end of the `list`, as the end of a singly linked list
 * is represented by a `NULL` pointer.
 *
 * @param list Pointer to the ForwardList.
 * @return ForwardListNode* A `NULL` pointer representing the end of the list.
 */
ForwardListNode *forward_list_end(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_end] NULL list pointer.");
        return NULL;
    }
    FORWARD_LIST_LOG("[forward_list_end] Returning NULL to represent end of list.");
    return NULL; 
}

/**
 * @brief Returns a pointer to the end of the ForwardList.
 *
 * This function returns `NULL` to represent the end of the `list`, as the end of a singly linked list
 * is represented by a `NULL` pointer.
 *
 * @param list Pointer to the ForwardList.
 * @return ForwardListNode* A `NULL` pointer representing the end of the list.
 */
size_t forward_list_max_size(const ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_max_size] NULL list pointer.");
        return (size_t)-1; // Or a specific error code or size_t max value
    }

    FORWARD_LIST_LOG("[forward_list_max_size] Returning max size of the list.");
    return (size_t)-1; // You might want to replace this with a more meaningful value
}

/**
 * @brief Constructs an element in place after the specified position in the ForwardList.
 *
 * This function constructs a new element in place after the node specified by `pos` in the `list`,
 * using the provided `value` without making a copy. If `pos` is `NULL`, the element is emplaced
 * at the front of the list. The list's size is increased by one.
 *
 * @param list Pointer to the ForwardList where the element will be emplaced.
 * @param pos Pointer to the node after which the element will be emplaced. If `NULL`, the element is emplaced at the front.
 * @param value Pointer to the value to be emplaced after the specified position.
 */
void forward_list_emplace_front(ForwardList *list, void *value) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_emplace_front] NULL list pointer.");
        return;
    }
    if (!value) {
        FORWARD_LIST_LOG("[forward_list_emplace_front] NULL value pointer.");
        return;
    }

    // Allocate memory for the new node
    ForwardListNode *newNode = (ForwardListNode*) malloc(sizeof(ForwardListNode));
    if (newNode == NULL) { 
        FORWARD_LIST_LOG("[forward_list_emplace_front] Memory allocation failed for newNode.");
        return;
    }

    // Allocate memory for the value and copy it
    newNode->value = malloc(list->itemSize);
    if (newNode->value == NULL) {
        FORWARD_LIST_LOG("[forward_list_emplace_front] Memory allocation failed for node value.");
        free(newNode);
        return;
    }
    memcpy(newNode->value, value, list->itemSize);

    // Insert the new node at the front of the list
    newNode->next = list->head;
    list->head = newNode;
    list->size++;

    FORWARD_LIST_LOG("[forward_list_emplace_front] Emplaced new value at the front. New size: %zu.", list->size);
}


/**
 * @brief Inserts a new element after the specified position in the ForwardList.
 * 
 * This function creates a new node and places it immediately after the given `pos` node.
 * If `pos` is NULL, the element is inserted at the beginning of the list. The element 
 * to be inserted is passed by pointer and becomes the value of the newly created node.
 * 
 * @param list Pointer to the ForwardList where the new element will be inserted.
 * @param pos Pointer to the node after which the new element will be inserted. 
 *            If `NULL`, the element will be inserted at the beginning of the list.
 * @param value Pointer to the value to be inserted into the list. This pointer must not be NULL.
 * 
 * @note The size of the inserted element should match the size specified by the ForwardList at creation.
 * 
 * @warning If memory allocation fails, the function returns without modifying the list. 
 *          Be sure to check for null pointers when calling this function.
 * 
 * @return void
 */
void forward_list_emplace_after(ForwardList *list, ForwardListNode *pos, void *value) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_emplace_after] Error: NULL list pointer.");
        return;
    }
    if (!value) {
        FORWARD_LIST_LOG("[forward_list_emplace_after] Error: NULL value pointer.");
        return;
    }
    if (pos == NULL) {
        FORWARD_LIST_LOG("[forward_list_emplace_after] Special case: inserting at the beginning.");
        forward_list_emplace_front(list, value);
        return;
    }

    // Allocate memory for the new node
    ForwardListNode *newNode = (ForwardListNode*) malloc(sizeof(ForwardListNode));
    if (newNode == NULL) { 
        FORWARD_LIST_LOG("[forward_list_emplace_after] Error: Unable to allocate memory for new node.");
        return;
    }

    // Allocate memory for the value and copy it
    newNode->value = malloc(list->itemSize);
    if (newNode->value == NULL) {
        FORWARD_LIST_LOG("[forward_list_emplace_after] Error: Memory allocation failed for node value.");
        free(newNode);  // Free the node itself before returning
        return;
    }
    memcpy(newNode->value, value, list->itemSize);  // Deep copy the value

    // Insert the new node after the specified position
    newNode->next = pos->next;
    pos->next = newNode;
    list->size++;

    FORWARD_LIST_LOG("[forward_list_emplace_after] Inserted value after the specified position. New size: %zu.", list->size);
}

/**
 * @brief Inserts one or more elements into the ForwardList after the specified position.
 *
 * This function inserts `numValues` elements into the `list` after the node specified by `pos`.
 * If `pos` is `NULL`, the elements are inserted at the beginning of the list. The values to be
 * inserted are provided through a pointer `value`, and the size of each element is determined by
 * the list's `itemSize`.
 *
 * @param list Pointer to the ForwardList where elements will be inserted.
 * @param pos Pointer to the node after which the elements will be inserted. If `NULL`, elements are inserted at the beginning.
 * @param value Pointer to the values to be inserted.
 * @param numValues The number of values to be inserted.
 */
void forward_list_insert_after(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_insert_after] Error: NULL list pointer.");
        return;
    }
    if (!value) {
        FORWARD_LIST_LOG("[forward_list_insert_after] Error: NULL value pointer.");
        return;
    }
    if (pos == NULL) {
        FORWARD_LIST_LOG("[forward_list_insert_after] Special case: inserting at the beginning.");
        for (size_t i = 0; i < numValues; ++i) {
            void *currentValue = (char *)value + i * list->itemSize;
            forward_list_push_front(list, currentValue);
        }
        return;
    }

    // Regular insertion after a given node
    for (size_t i = 0; i < numValues; ++i) {
        void *currentValue = (char *)value + i * list->itemSize;
        ForwardListNode *newNode = (ForwardListNode*) malloc(sizeof(ForwardListNode));
        if (newNode == NULL) {
            FORWARD_LIST_LOG("[forward_list_insert_after] Error: Memory allocation failed for new node.");
            return;
        }
        newNode->value = malloc(list->itemSize);
        if (newNode->value == NULL) {
            FORWARD_LIST_LOG("[forward_list_insert_after] Error: Memory allocation failed for node value.");
            free(newNode);
            return;
        }
        memcpy(newNode->value, currentValue, list->itemSize);

        newNode->next = pos->next;
        pos->next = newNode;
        pos = newNode;
        list->size++;
        FORWARD_LIST_LOG("[forward_list_insert_after] Inserted value after the specified position.");
    }
}

/**
 * @brief Inserts one or more elements into the ForwardList after the specified position.
 *
 * This function inserts `numValues` elements into the `list` after the node specified by `pos`.
 * If `pos` is `NULL`, the elements are inserted at the beginning of the list. The values to be
 * inserted are provided through a pointer `value`, and the size of each element is determined by
 * the list's `itemSize`.
 *
 * @param list Pointer to the ForwardList where elements will be inserted.
 * @param pos Pointer to the node after which the elements will be inserted. If `NULL`, elements are inserted at the beginning.
 * @param value Pointer to the values to be inserted.
 * @param numValues The number of values to be inserted.
 */
void forward_list_erase_after(ForwardList *list, ForwardListNode *pos) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_erase_after] Error: NULL list pointer.");
        return;
    }
    if (!pos) {
        FORWARD_LIST_LOG("[forward_list_erase_after] Error: NULL position node.");
        return;
    }
    if (!pos->next) {
        FORWARD_LIST_LOG("[forward_list_erase_after] Warning: Attempting to erase after the end of the list.");
        return;
    }
    ForwardListNode *temp = pos->next;
    pos->next = temp->next;

    free(temp->value);
    free(temp);
    list->size--;
    FORWARD_LIST_LOG("[forward_list_erase_after] Erased node after the specified position.");
}

/**
 * @brief Swaps the contents of two ForwardLists.
 *
 * This function swaps the contents (nodes and sizes) of the two ForwardLists, `list1` and `list2`.
 * After the swap, `list1` will contain what was originally in `list2` and vice versa.
 *
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 */
void forward_list_swap(ForwardList *list1, ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_swap] Error: NULL list pointer(s).");
        return;
    }

    // Swap heads
    ForwardListNode *tempHead = list1->head;
    list1->head = list2->head;
    list2->head = tempHead;

    // Swap sizes
    size_t tempSize = list1->size;
    list1->size = list2->size;
    list2->size = tempSize;

    FORWARD_LIST_LOG("[forward_list_swap] Swapped the contents of two lists.");
}

/**
 * @brief Resizes the ForwardList to the specified size.
 *
 * This function resizes the `list` to contain `newSize` elements. If the current size of the list
 * is greater than `newSize`, elements are removed from the front of the list. If the current size is
 * less than `newSize`, new elements are added to the front of the list, initialized to zero.
 *
 * @param list Pointer to the ForwardList to be resized.
 * @param newSize The new size of the list.
 */
void forward_list_resize(ForwardList *list, size_t newSize) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_resize] Error: NULL list pointer.");
        return;
    }

    while (list->size > newSize) { 
        forward_list_pop_front(list);
        FORWARD_LIST_LOG("[forward_list_resize] Reduced size by popping front.");
    }
    while (list->size < newSize) {
        // Allocate memory for a new value and initialize it to zero
        void *newValue = calloc(1, list->itemSize);  
        if (!newValue) {
            FORWARD_LIST_LOG("[forward_list_resize] Error: Memory allocation failed.");
            break;
        }
        // Add the new value to the front of the list
        forward_list_push_front(list, newValue);
        free(newValue);  
        FORWARD_LIST_LOG("[forward_list_resize] Increased size by adding new element.");
    }

    FORWARD_LIST_LOG("[forward_list_resize] Resizing completed.");
}

/**
 * @brief Splices the elements from another ForwardList into this list after the specified position.
 * 
 * This function moves all elements from the `other` ForwardList into the `list` 
 * after the specified position `pos`. The `other` list becomes empty after this 
 * operation. If `pos` is `NULL`, the elements are spliced at the beginning of the list.
 * 
 * @param list Pointer to the ForwardList where elements will be spliced.
 * @param pos Pointer to the node after which the elements will be inserted. If `NULL`, elements are spliced at the beginning.
 * @param other Pointer to the ForwardList whose elements will be spliced into `list`.
 */
void forward_list_splice_after(ForwardList *list, ForwardListNode *pos, ForwardList *other) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_splice_after] Error: NULL list pointer.");
        return;
    }
    if (!other) {
        FORWARD_LIST_LOG("[forward_list_splice_after] Error: NULL other list pointer.");
        return;
    }
    if (pos == NULL) {
        // Special case: if pos is NULL, splice at the beginning
        FORWARD_LIST_LOG("[forward_list_splice_after] Splicing at the beginning as pos is NULL.");
        if (other->head != NULL) {
            ForwardListNode *otherCurrent = other->head;
            while (otherCurrent->next != NULL) {  // Find the last node of the other list
                otherCurrent = otherCurrent->next;
            }
            otherCurrent->next = list->head;
            list->head = other->head;
            list->size += other->size;
            FORWARD_LIST_LOG("[forward_list_splice_after] Spliced %zu elements from other list to the beginning.", other->size);
            other->head = NULL;
            other->size = 0;
        }
        return;
    }

    // Regular splicing after a given node
    FORWARD_LIST_LOG("[forward_list_splice_after] Splicing after a given node.");
    if (other->head != NULL) {
        ForwardListNode *otherCurrent = other->head;
        while (otherCurrent->next != NULL) {  // Find the last node of the other list
            otherCurrent = otherCurrent->next;
        }
        otherCurrent->next = pos->next;
        pos->next = other->head;
        list->size += other->size;
        FORWARD_LIST_LOG("[forward_list_splice_after] Spliced %zu elements after the given node.", other->size);
        other->head = NULL;
        other->size = 0;
    }
}

/**
 * @brief Removes all elements equal to the specified value from the ForwardList.
 * 
 * This function iterates through the ForwardList and removes all elements that 
 * are equal to the specified `value`. Memory for the removed nodes is freed, 
 * and the list's size is updated accordingly.
 * 
 * @param list Pointer to the ForwardList from which elements are to be removed.
 * @param value Pointer to the value to be removed from the list.
 */
void forward_list_remove(ForwardList *list, void *value) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_remove] Error: NULL list pointer.");
        return;
    }
    if (!value) {
        FORWARD_LIST_LOG("[forward_list_remove] Error: NULL value pointer.");
        return;
    }

    FORWARD_LIST_LOG("[forward_list_remove] Removing elements equal to the specified value.");
    
    while (list->head != NULL && memcmp(list->head->value, value, list->itemSize) == 0) { 
        forward_list_pop_front(list);
        FORWARD_LIST_LOG("[forward_list_remove] Removed element from the front.");
    }

    ForwardListNode *current = list->head;
    while (current != NULL && current->next != NULL) {
        if (memcmp(current->next->value, value, list->itemSize) == 0) {
            ForwardListNode *temp = current->next;
            current->next = temp->next;
            free(temp->value);
            free(temp);
            list->size--;
            FORWARD_LIST_LOG("[forward_list_remove] Removed element from the list.");
        } 
        else {
            current = current->next;
        }
    }

    FORWARD_LIST_LOG("[forward_list_remove] Removal of elements completed.");
}

/**
 * @brief Removes all elements that satisfy the specified condition from the ForwardList.
 * 
 * This function iterates through the ForwardList and removes all elements that 
 * satisfy the specified condition. The condition is provided as a function pointer 
 * that takes a pointer to the element and returns `true` if the element should be removed.
 * 
 * @param list Pointer to the ForwardList from which elements are to be removed.
 * @param condition Function pointer to a function that determines if an element should be removed.
 */
void forward_list_remove_if(ForwardList *list, bool (*condition)(void*)) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_remove_if] Error: NULL list pointer.");
        return;
    }
    if (!condition) {
        FORWARD_LIST_LOG("[forward_list_remove_if] Error: NULL condition function pointer.");
        return;
    }

    FORWARD_LIST_LOG("[forward_list_remove_if] Removing elements based on condition.");
    
    // Remove elements at the front that satisfy the condition
    while (list->head != NULL && condition(list->head->value)) { 
        forward_list_pop_front(list);
    }

    ForwardListNode *current = list->head;
    while (current != NULL && current->next != NULL) {
        if (condition(current->next->value)) {
            ForwardListNode *temp = current->next;
            current->next = temp->next;
            free(temp->value);
            free(temp);
            list->size--;
            FORWARD_LIST_LOG("[forward_list_remove_if] Removed element.");
        } 
        else {
            current = current->next;
        }
    }

    FORWARD_LIST_LOG("[forward_list_remove_if] Removal based on condition completed.");
}

/**
 * @brief Removes consecutive duplicate elements from the ForwardList.
 * 
 * This function iterates through the ForwardList and removes consecutive elements 
 * that are duplicates, retaining only the first occurrence. The list is modified 
 * in place, and memory for the removed nodes is freed.
 * 
 * @param list Pointer to the ForwardList from which duplicates are to be removed.
 */
void forward_list_unique(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_unique] Error: NULL list pointer.");
        return;
    }
    if (list->head == NULL || list->head->next == NULL) {
        FORWARD_LIST_LOG("[forward_list_unique] List is empty or has a single element, no action required.");
        return; 
    }

    FORWARD_LIST_LOG("[forward_list_unique] Removing consecutive duplicate elements.");

    ForwardListNode *current = list->head;
    while (current->next != NULL) {
        if (memcmp(current->value, current->next->value, list->itemSize) == 0) {
            ForwardListNode *duplicate = current->next;
            current->next = duplicate->next;
            free(duplicate->value);
            free(duplicate);
            list->size--;
            FORWARD_LIST_LOG("[forward_list_unique] Removed duplicate element.");
        } 
        else {
            current = current->next;
        }
    }

    FORWARD_LIST_LOG("[forward_list_unique] Removal of consecutive duplicates completed.");
}

/**
 * @brief Merges two sorted ForwardLists into a single sorted list.
 * 
 * This function takes two sorted ForwardLists, `list1` and `list2`, and merges 
 * them into a single sorted list stored in `list1`. After the merge, `list2` 
 * becomes empty. The elements are merged in a way that the resulting list is 
 * sorted in ascending order.
 * 
 * @param list1 Pointer to the first sorted ForwardList that will store the merged list.
 * @param list2 Pointer to the second sorted ForwardList, which will be emptied after the merge.
 */
void forward_list_merge(ForwardList *list1, ForwardList *list2) {
    if (!list1) {
        FORWARD_LIST_LOG("[forward_list_merge] Error: NULL list1 pointer.");
        return;
    }
    if (!list2) {
        FORWARD_LIST_LOG("[forward_list_merge] Error: NULL list2 pointer.");
        return;
    }
    if (list2->head == NULL) {
        FORWARD_LIST_LOG("[forward_list_merge] list2 is empty, nothing to merge.");
        return;  
    }

    FORWARD_LIST_LOG("[forward_list_merge] Merging list1 and list2.");
    ForwardListNode dummy;
    ForwardListNode *tail = &dummy;
    dummy.next = NULL;

    while (list1->head != NULL && list2->head != NULL) {
        if (memcmp(list1->head->value, list2->head->value, list1->itemSize) <= 0) {
            tail->next = list1->head;
            list1->head = list1->head->next;
        } 
        else {
            tail->next = list2->head;
            list2->head = list2->head->next;
        }
        tail = tail->next;
    }

    tail->next = (list1->head != NULL) ? list1->head : list2->head;
    
    // Reset list2
    list2->head = NULL;
    list2->size = 0;
    list1->head = dummy.next; // Update list1's head

    FORWARD_LIST_LOG("[forward_list_merge] Merging completed.");
}

/**
 * @brief Sorts the elements of the ForwardList in ascending order.
 * 
 * This function sorts the elements of the ForwardList using the merge sort algorithm. 
 * The sorting is done in place, and the list is modified to reflect the new order.
 * 
 * @param list Pointer to the ForwardList to be sorted.
 */
void forward_list_sort(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_sort] Error: NULL ForwardList pointer.");
        return;
    }
    if (list->head == NULL) {
        FORWARD_LIST_LOG("[forward_list_sort] List is empty, nothing to sort.");
        return;  
    }

    FORWARD_LIST_LOG("[forward_list_sort] Sorting the ForwardList.");
    list->head = merge_sort(list->head, list->itemSize);
    FORWARD_LIST_LOG("[forward_list_sort] Sorting completed.");
}

/**
 * @brief Reverses the order of elements in the ForwardList.
 * 
 * This function reverses the order of elements in the ForwardList. The list is 
 * modified in place, and the head of the list is updated to reflect the new order.
 * 
 * @param list Pointer to the ForwardList to be reversed.
 */
void forward_list_reverse(ForwardList *list) {
    if (!list) {
        FORWARD_LIST_LOG("[forward_list_reverse] Error: NULL ForwardList pointer.");
        return;
    }
    if (list->head == NULL || list->head->next == NULL) {
        FORWARD_LIST_LOG("[forward_list_reverse] No need to reverse. List is empty or has only one element.");
        return;  // List is empty or has only one element, no need to reverse
    }

    FORWARD_LIST_LOG("[forward_list_reverse] Reversing the ForwardList.");
    ForwardListNode *prev = NULL, *current = list->head, *next = NULL;
    while (current != NULL) {
        next = current->next;     // Store next node
        current->next = prev;     // Reverse current node's pointer
        prev = current;           // Move pointers one position ahead
        current = next;
    }
    list->head = prev; // Update the head to the new front
    FORWARD_LIST_LOG("[forward_list_reverse] ForwardList reversed successfully.");
}

// Helper function for comparing node values
static int compare_node_values(const void *a, const void *b, size_t size) {
    return memcmp(a, b, size);
}

/**
 * @brief Compares two ForwardLists to determine if the first list is lexicographically less than the second.
 * 
 * This function checks if the elements in `list1` are lexicographically less than those in `list2`.
 * The comparison is done element by element, and the function stops as soon as a non-equal element
 * is found. If all compared elements are equal but `list1` is shorter than `list2`, the function returns true.
 * 
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 * 
 * @return true if `list1` is lexicographically less than `list2`, false otherwise.
 */
bool forward_list_is_less(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_is_less] Error: NULL ForwardList pointer(s).");
        return false;
    }
    
    FORWARD_LIST_LOG("[forward_list_is_less] Comparing if list1 is less than list2.");
    ForwardListNode *node1 = list1->head, *node2 = list2->head;

    while (node1 && node2) {
        if (compare_node_values(node1->value, node2->value, list1->itemSize) >= 0) {
            FORWARD_LIST_LOG("[forward_list_is_less] list1 is not less than list2.");
            return false;
        }
        node1 = node1->next;
        node2 = node2->next;
    }

    bool result = (node1 == NULL && node2 != NULL);
    FORWARD_LIST_LOG("[forward_list_is_less] Comparison result: %s", result ? "True" : "False");

    return result;
}

/**
 * @brief Compares two ForwardLists to determine if the first list is lexicographically greater than the second.
 * 
 * This function checks if the elements in `list1` are lexicographically greater than those in `list2`.
 * The comparison is done element by element, and the function stops as soon as a non-equal element
 * is found. If all compared elements are equal but `list1` is longer than `list2`, the function returns true.
 * 
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 * 
 * @return true if `list1` is lexicographically greater than `list2`, false otherwise.
 */
bool forward_list_is_greater(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_is_greater] Error: NULL ForwardList pointer(s).");
        return false;
    }
    FORWARD_LIST_LOG("[forward_list_is_greater] Comparing if list1 is greater than list2.");
    return forward_list_is_less(list2, list1);
}

/**
 * @brief Compares two ForwardLists to determine if they are equal.
 * 
 * This function checks if the elements in `list1` are equal to those in `list2`.
 * The comparison is done element by element. The function returns true if all elements
 * are equal and both lists have the same length.
 * 
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 * 
 * @return true if `list1` is equal to `list2`, false otherwise.
 */
bool forward_list_is_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_is_equal] Error: NULL ForwardList pointer(s).");
        return false;
    }
    FORWARD_LIST_LOG("[forward_list_is_equal] Checking if list1 is equal to list2.");

    ForwardListNode *node1 = list1->head, *node2 = list2->head;

    while (node1 && node2) {
        if (compare_node_values(node1->value, node2->value, list1->itemSize) != 0) {
            FORWARD_LIST_LOG("[forward_list_is_equal] Lists are not equal.");
            return false;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    bool result = (node1 == NULL && node2 == NULL);
    FORWARD_LIST_LOG("[forward_list_is_equal] Lists comparison result: %s", result ? "Equal" : "Not equal");

    return result;
}

/**
 * @brief Compares two ForwardLists to determine if the first list is lexicographically less than or equal to the second.
 * 
 * This function checks if the elements in `list1` are lexicographically less than or equal to those in `list2`.
 * The comparison is done element by element. The function returns true if `list1` is less than or equal to `list2`.
 * 
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 * 
 * @return true if `list1` is less than or equal to `list2`, false otherwise.
 */
bool forward_list_is_less_or_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_is_less_or_equal] Error: NULL ForwardList pointer(s).");
        return false;
    }
    FORWARD_LIST_LOG("[forward_list_is_less_or_equal] Checking if list1 is less than or equal to list2.");
    bool result = forward_list_is_less(list1, list2) || forward_list_is_equal(list1, list2);
    FORWARD_LIST_LOG("[forward_list_is_less_or_equal] Comparison result: %s", result ? "True" : "False");

    return result;
}

/**
 * @brief Compares two ForwardLists to determine if the first list is lexicographically greater than or equal to the second.
 * 
 * This function checks if the elements in `list1` are lexicographically greater than or equal to those in `list2`.
 * The comparison is done element by element. The function returns true if `list1` is greater than or equal to `list2`.
 * 
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 * 
 * @return true if `list1` is greater than or equal to `list2`, false otherwise.
 */
bool forward_list_is_greater_or_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_is_greater_or_equal] Error: NULL ForwardList pointer(s).");
        return false;
    }
    FORWARD_LIST_LOG("[forward_list_is_greater_or_equal] Comparing if list1 is greater than or equal to list2.");
    return forward_list_is_greater(list1, list2) || forward_list_is_equal(list1, list2);
}

/**
 * @brief Compares two ForwardLists to determine if they are not equal.
 * 
 * This function checks if the elements in `list1` are not equal to those in `list2`.
 * The comparison is done element by element. The function returns true if the lists differ in any element
 * or if they have different lengths.
 * 
 * @param list1 Pointer to the first ForwardList.
 * @param list2 Pointer to the second ForwardList.
 * @return true if `list1` is not equal to `list2`, false otherwise.
 */
bool forward_list_is_not_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        FORWARD_LIST_LOG("[forward_list_is_not_equal] Error: NULL ForwardList pointer(s).");
        return true;  // If one is NULL and the other is not, they are not equal.
    }
    FORWARD_LIST_LOG("[forward_list_is_not_equal] Checking if list1 is not equal to list2.");
    return !forward_list_is_equal(list1, list2);
}
