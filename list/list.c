/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class List
*/

#include <stdlib.h>
#include <string.h>
#include "list.h"


/**
 * @brief Creates a new list with the specified item size and comparison function.
 *
 * This function initializes a new list, allocating memory for it and setting up the head and tail pointers, size, and item size.
 * The comparison function is used for operations like sorting and removing elements.
 *
 * @param itemSize The size of each item in the list, in bytes. Must be greater than 0.
 * @param compare A function pointer used to compare items in the list. This can be NULL if no comparison is needed.
 * 
 * @return Pointer to the newly created list, or NULL if memory allocation fails.
 */
List *list_create(size_t itemSize, CompareFunction compare) {
    if (itemSize == 0) {
        LIST_LOG("[list_create] Error: Item size must be greater than 0.");
        exit(-1);
    }

    List *list = (List*) malloc(sizeof(List));
    if (!list) {
        LIST_LOG("[list_create] Error: Cannot allocate memory for list.");
        exit(-1);
    }

    list->head = list->tail = NULL;
    list->size = 0;
    list->itemSize = itemSize;
    list->compare = compare; 

    LIST_LOG("[list_create] List created successfully with item size %zu.", itemSize);
    return list;
}

/**
 * @brief Returns a pointer to the value at the front of the list.
 *
 * This function retrieves the value stored at the front of the list (the head node). If the list is empty or the list pointer is NULL,
 * an error message is printed and NULL is returned.
 *
 * @param list Pointer to the list.
 * @return Pointer to the value at the front of the list, or NULL if the list is empty or invalid.
 */
void *list_front(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_front] Error: Null pointer provided for list.");
        return NULL;
    }
    if (list->head == NULL) {
        LIST_LOG("[list_front] Error: The list is empty.");
        return NULL;
    }

    LIST_LOG("[list_front] Returning value at the front of the list.");
    return list->head->value;
}
/**
 * @brief Returns a pointer to the value at the back of the list.
 *
 * This function retrieves the value stored at the back of the list (the tail node). If the list is empty or the list pointer is NULL,
 * an error message is printed and NULL is returned.
 *
 * @param list Pointer to the list.
 * @return Pointer to the value at the back of the list, or NULL if the list is empty or invalid.
 */
void *list_back(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_back] : Error: Null pointer provided for list in list_back.");
        return NULL;
    }
    if (list->tail == NULL) {
        LIST_LOG("[list_back] : Error: The list is empty in list_back.");
        return NULL;
    }

    LIST_LOG("[list_back] : Returning value from the back of the list.");
    return list->tail->value;
}

/**
 * @brief Inserts a new element at the specified index in the list.
 *
 * This function inserts a new element at the specified index in the list. If the index is 0, the element is inserted at the front.
 * If the index is equal to the size of the list, the element is inserted at the back. The function returns a pointer to the newly inserted value.
 *
 * @param list Pointer to the list in which the element will be inserted.
 * @param index The index at which to insert the new element. Must be within the bounds of the list.
 * @param value Pointer to the value to be inserted.
 * 
 * @return Pointer to the newly inserted value, or NULL if the operation fails.
 */
void *list_insert(List *list, size_t index, void *value) {
    if (list == NULL) {
        LIST_LOG("[list_insert] : Error: Null pointer provided for list in list_insert.");
        return NULL;
    }
    if (index > list->size) {
        LIST_LOG("[list_insert] : Error: Index out of bounds in list_insert. Index: %zu, List size: %zu", index, list->size);
        return NULL;
    }
    if (index == 0) {
        LIST_LOG("[list_insert] : Inserting value at the front of the list.");
        list_push_front(list, value);
        LIST_LOG("[list_insert] : Successfully inserted value at the front.");

        return list->head->value;
    } 
    else if (index == list->size) {
        LIST_LOG("[list_insert] : Inserting value at the back of the list.");
        list_push_back(list, value);
        LIST_LOG("[list_insert] : Successfully inserted value at the back.");

        return list->tail->value;
    }

    Node *newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        LIST_LOG("[list_insert] : Error: Memory allocation failed for newNode in list_insert.");
        return NULL;
    }

    newNode->value = malloc(list->itemSize);
    if (!newNode->value) {
        LIST_LOG("[list_insert] : Error: Memory allocation failed for newNode value in list_insert.");
        free(newNode);
        return NULL;
    }

    memcpy(newNode->value, value, list->itemSize);
    LIST_LOG("[list_insert] : Allocated new node with copied value at index %zu.", index);

    // Traverse to the node just before the insertion point
    Node *current = list->head;
    for (size_t i = 0; i < index - 1; ++i) {
        current = current->next;
    }

    newNode->next = current->next;
    newNode->prev = current;

    if (newNode->next != NULL) {
        newNode->next->prev = newNode;  
    }

    current->next = newNode;
    list->size++;

    LIST_LOG("[list_insert] : Successfully inserted value at index %zu. New list size: %zu", index, list->size);
    return newNode->value;
}

/**
 * @brief Erases an element from the list at the specified index.
 *
 * This function removes the element at the specified index in the list. The removed element's memory is freed, and the list is updated accordingly.
 * If the list becomes empty after the operation, both the head and tail pointers are set to NULL.
 *
 * @param list Pointer to the list from which the element will be erased.
 * @param index The index of the element to remove. Must be within the bounds of the list
 * 
 * @return Pointer to the value of the removed element, or NULL if the operation fails.
 */
void *list_erase(List *list, size_t index) {
    if (list == NULL) {
        LIST_LOG("[list_erase] : Error: Null pointer provided for list in list_erase.");
        return NULL;
    }
    if (index >= list->size) {
        LIST_LOG("[list_erase] : Error: Index %zu out of bounds (list size: %zu) in list_erase.", index, list->size);
        return NULL;
    }

    Node *nodeToRemove;
    if (index == 0) {
        nodeToRemove = list->head;
        list->head = nodeToRemove->next;

        if (list->head) {
            list->head->prev = NULL;
        }
    } 
    else {
        Node *current = list->head;
        for (size_t i = 0; i < index - 1; ++i) {
            current = current->next;
        }
        nodeToRemove = current->next;
        current->next = nodeToRemove->next;

        if (nodeToRemove->next) {
            nodeToRemove->next->prev = current;
        }
    }

    if (nodeToRemove == list->tail) {
        list->tail = nodeToRemove->prev;
    }

    void *removedValue = nodeToRemove->value;
    free(nodeToRemove);
    list->size--;

    if (list->size == 0) {
        list->head = list->tail = NULL;
    }

    LIST_LOG("[list_erase] : Successfully erased element at index %zu. New size: %zu", index, list->size);
    return removedValue;
}

/**
 * @brief Resizes the list to the specified new size.
 *
 * This function changes the size of the list to the new size. If the new size is smaller than the current size, elements are removed from the back.
 * If the new size is larger, new elements are added to the back, initialized with the provided default value or zero if no default is provided.
 *
 * @param list Pointer to the list to be resized.
 * @param newSize The new size of the list.
 * @param defaultValue Pointer to the value used to initialize new elements if the list is expanded. If NULL, new elements are zero-initialized.
 */
void list_resize(List *list, size_t newSize, void *defaultValue) {
    if (list == NULL) {
        LIST_LOG("[list_resize] : Error: Null pointer provided for list in list_resize.");
        return;
    }
    LIST_LOG("[list_resize] : Resizing list from size %zu to %zu", list->size, newSize);

    while (list->size > newSize) {
        list_pop_back(list);
    }
    while (list->size < newSize) {
        void *newValue = malloc(list->itemSize);

        if (!newValue) {
            LIST_LOG("[list_resize] : Error: Memory allocation failed in list_resize.");
            return;
        }
        if (defaultValue != NULL) {
            memcpy(newValue, defaultValue, list->itemSize); // Use the provided default value
        } 
        else {
            memset(newValue, 0, list->itemSize); 
        }

        list_push_back(list, newValue);
        free(newValue); 
    }

    LIST_LOG("[list_resize] : List resized successfully. New size: %zu", list->size);
}

/**
 * @brief Swaps the contents of two lists.
 *
 * This function exchanges the contents of two lists, including their elements, sizes, and item sizes. After the swap, the two lists retain their original memory addresses but have swapped contents.
 *
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 */
void list_swap(List *list1, List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_swap] : Error: Null pointer provided for one or both lists in list_swap.");
        return;
    }
    LIST_LOG("[list_swap] : Swapping lists...");

    Node *tempHead = list1->head; 
    list1->head = list2->head;
    list2->head = tempHead;

    Node *tempTail = list1->tail; 
    list1->tail = list2->tail;
    list2->tail = tempTail;

    size_t tempSize = list1->size; 
    list1->size = list2->size;
    list2->size = tempSize;

    size_t tempItemSize = list1->itemSize; 
    list1->itemSize = list2->itemSize;
    list2->itemSize = tempItemSize;

    LIST_LOG("[list_swap] : Lists swapped successfully.");
}

/**
 * @brief Reverses the order of elements in the list.
 *
 * This function reverses the order of all elements in the list. The head becomes the tail, and the tail becomes the head. 
 * The operation is performed in-place, modifying the original list.
 *
 * @param list Pointer to the list to be reversed.
 */
void list_reverse(List *list) {
    if (list == NULL) {
        LIST_LOG("[list_reverse] : Error: Null pointer provided for list in list_reverse.");
        return;
    }
    if (list->head == NULL) {
        LIST_LOG("[list_reverse] : List is empty, no action needed in list_reverse.");
        return;
    }

    Node *current = list->head;
    Node *temp = NULL;

    while (current != NULL) {
        temp = current->prev; // Swap the next and prev pointers
        current->prev = current->next;
        current->next = temp;
        current = current->prev; 
    }

    temp = list->head; 
    list->head = list->tail;
    list->tail = temp;

    LIST_LOG("[list_reverse] : List reversed successfully.");
}

/**
 * @brief Sorts the elements of the list in ascending order.
 *
 * This function sorts the elements of the list in ascending order based on the comparison function provided during
 * the list's creation. It uses a simple bubble sort algorithm, which repeatedly swaps adjacent elements if they are in the wrong order.
 * The sorting is done in-place, modifying the original list.
 *
 * @param list Pointer to the list to be sorted.
 */
void list_sort(List* list) {
    if (list == NULL) {
        LIST_LOG("[list_sort] : Error: Null pointer provided for list in list_sort.");
        return;
    }
    if (list->size < 2 || list->compare == NULL) {
        LIST_LOG("[list_sort] : Error: Insufficient size or null compare function in list_sort.");
        return;
    }

    bool swapped;
    do {
        swapped = false;
        Node *current = list->head;

        while (current != NULL && current->next != NULL) {
            if (list->compare(current->value, current->next->value) > 0) {
                void *temp = current->value;
                current->value = current->next->value;
                current->next->value = temp;
                swapped = true;
            }
            current = current->next;
        }

    } while (swapped);

    LIST_LOG("[list_sort] : List sorted successfully.");
}

/**
 * @brief Adds a new element to the front of the list.
 *
 * This function creates a new node and adds it to the front of the list. The new node contains a copy of the provided value.
 * If the list is empty, the new node becomes both the head and the tail of the list.
 *
 * @param list Pointer to the list to which the element will be added.
 * @param value Pointer to the value to be added to the front of the list.
 */
void list_push_front(List *list, void *value) {
    if (list == NULL || value == NULL) {
        LIST_LOG("[list_push_front] : Error: Null pointer provided for list or value in list_push_front.");
        return;
    }

    Node *newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        LIST_LOG("[list_push_front] : Error: Memory allocation failed for newNode in list_push_front.");
        return;
    }
    newNode->value = malloc(list->itemSize);

    if (!newNode->value) {
        free(newNode);
        LIST_LOG("[list_push_front] : Error: Memory allocation failed for newNode->value in list_push_front.");
        return;
    }

    memcpy(newNode->value, value, list->itemSize);
    newNode->next = list->head;
    newNode->prev = NULL;  // As this will be the new first node

    if (list->head != NULL) { 
        list->head->prev = newNode;
    }

    list->head = newNode;

    if (list->tail == NULL) { 
        list->tail = newNode;
    }
    list->size++;

    LIST_LOG("[list_push_front] : Node added to the front of the list. New size: %zu", list->size);
}

/**
 * @brief Adds a new element to the back of the list.
 *
 * This function creates a new node and adds it to the back of the list. The new node contains a copy of the provided value.
 * If the list is empty, the new node becomes both the head and the tail of the list.
 *
 * @param list Pointer to the list to which the element will be added.
 * @param value Pointer to the value to be added to the back of the list.
 */
void list_push_back(List *list, void *value) {
    if (list == NULL || value == NULL) {
        LIST_LOG("[list_push_back] : Error: Null pointer provided for list or value in list_push_back.");
        return;
    }

    Node *newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        LIST_LOG("[list_push_back] : Error: Memory allocation failed for newNode in list_push_back.");
        return;
    }

    newNode->value = malloc(list->itemSize);
    if (!newNode->value) {
        free(newNode);
        LIST_LOG("[list_push_back] : Error: Memory allocation failed for newNode->value in list_push_back.");
        return;  
    }

    memcpy(newNode->value, value, list->itemSize);
    newNode->next = NULL; 
    newNode->prev = list->tail;  

    if (list->tail != NULL) {
        list->tail->next = newNode;  
    }
    list->tail = newNode;  
    
    if (list->head == NULL) {
        list->head = newNode;  
    }
    list->size++;

    LIST_LOG("[list_push_back] : Node successfully added to the list. New size: %zu", list->size);
}

/**
 * @brief Removes the first element from the list.
 *
 * This function removes the first element from the list. If the list is empty, it returns without performing any operation.
 * The memory associated with the removed element is freed.
 *
 * @param list Pointer to the list from which the element will be removed.
 */
void list_pop_front(List *list) {
    if (list == NULL) {
        LIST_LOG("[list_pop_front] : Error: Null pointer provided for list in list_pop_front.");
        return;
    }
    if (list->head == NULL) {
        LIST_LOG("[list_pop_front] : Error: Trying to pop from an empty list in list_pop_front.");
        return;  
    }

    Node *temp = list->head;
    list->head = list->head->next;  

    if (list->head != NULL) { 
        list->head->prev = NULL;  
    }
    else {
        list->tail = NULL;  
    }

    free(temp->value);  
    free(temp);
    list->size--;

    LIST_LOG("[list_pop_front] : First element removed from the list. New size: %zu", list->size);
}

/**
 * @brief Removes the last element from the list.
 *
 * This function removes the last element from the list. If the list is empty, it returns without 
 * performing any operation. If the list contains only one element, it removes that element and sets 
 * both the head and tail to NULL.
 * 
 * @param list Pointer to the list.
 */
void list_pop_back(List *list) {
    if (list == NULL) {
        LIST_LOG("[list_pop_back] : Error: Null pointer provided for list in list_pop_back.");
        return;
    }
    if (list->head == NULL) {
        LIST_LOG("[list_pop_back] : Error: Trying to pop from an empty list in list_pop_back.");
        return;  
    }
    
    // If there is only one node in the list
    if (list->head == list->tail) {
        free(list->head->value);
        free(list->head);
        list->head = list->tail = NULL;
    } 
    else {
        Node *lastNode = list->tail;
        list->tail = lastNode->prev;
        list->tail->next = NULL;

        free(lastNode->value);
        free(lastNode);
    }

    list->size--;
    LIST_LOG("[list_pop_back] : Last element removed from the list. New size: %zu", list->size);
}

/**
 * @brief Clears all elements from the list.
 *
 * This function removes all elements from the list and frees the memory associated with each node 
 * and its value. After this operation, the list is empty.
 * 
 * @param list Pointer to the list.
 */
void list_clear(List *list) {
    if (list == NULL) {
        LIST_LOG("[list_clear] : Error: Null pointer provided for list in list_clear.");
        return;
    }
    Node *current = list->head;

    while (current != NULL) {
        Node *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
    list->head = list->tail = NULL;
    list->size = 0;

    LIST_LOG("[list_clear] : List cleared successfully. Size is now 0.");
}

/**
 * @brief Checks if the list is empty.
 *
 * This function checks whether the list contains any elements. It returns true if the list is empty, 
 * and false otherwise.
 * 
 * @param list Pointer to the list.
 * 
 * @return bool True if the list is empty, false otherwise.
 */
bool list_empty(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_empty] : Error: Null pointer provided for list in list_empty.");
        return true; 
    }
    return list->head == NULL;
}

/**
 * @brief Returns the number of elements in the list.
 *
 * This function returns the number of elements currently stored in the list.
 * 
 * @param list Pointer to the list.
 * @return size_t The number of elements in the list.
 */
size_t list_length(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_length] : Error: Null pointer provided for list in list_length.");
        return 0;
    }
    return list->size;
}

/**
 * @brief Deallocates the list and frees all associated memory.
 *
 * This function clears all elements from the list and frees the memory associated with the list itself.
 * After calling this function, the list pointer should not be used unless it is reinitialized.
 * 
 * @param list Pointer to the list.
 */
void list_deallocate(List *list) {
    if (list == NULL) {
        LIST_LOG("[list_deallocate] : Error: Null pointer provided for list in list_deallocate.");
        return;
    }
    list_clear(list);
    free(list);

    LIST_LOG("[list_deallocate] : List deallocated successfully.");
}

/**
 * @brief Returns an iterator to the first element of the list.
 *
 * This function returns a pointer to the first element in the list. If the list is empty, 
 * it returns NULL.
 * 
 * @param list Pointer to the list.
 * 
 * @return Node* Pointer to the first element of the list, or NULL if the list is empty.
 */
Node *list_begin(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_begin] : Error: Null pointer provided for list in list_begin.");
        return NULL;
    }
    return list->head;
}

/**
 * @brief Returns an iterator to the element following the last element of the list.
 *
 * This function returns a pointer to the element following the last element in the list. 
 * Since the end of a singly linked list is represented by NULL, this function always returns NULL.
 * 
 * @param list Pointer to the list.
 * 
 * @return Node* Pointer to the end of the list (NULL).
 */
Node *list_end(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_end] : Error: Null pointer provided for list in list_end.");
        return NULL;
    }
    return NULL; 
}

/**
 * @brief Returns a reverse iterator to the last element of the list.
 *
 * This function returns a pointer to the last element in the list. It is useful for iterating 
 * over the list in reverse order.
 * 
 * @param list Pointer to the list.
 * @return Node* Pointer to the last element of the list, or NULL if the list is empty.
 */
Node *list_rbegin(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_rbegin] : Error: Null pointer provided for list in list_rbegin.");
        return NULL;
    }
    return list->tail;
}

/**
 * @brief Returns a reverse iterator to the element preceding the first element of the list.
 *
 * This function returns a pointer to the element preceding the first element in the list. 
 * Since the reverse end of a doubly linked list is represented by NULL, this function always returns NULL.
 * 
 * @param list Pointer to the list.
 * @return Node* Pointer to the reverse end of the list (NULL).
 */
Node *list_rend(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_rend] : Error: Null pointer provided for list in list_rend.");
        return NULL;
    }
    return NULL;  // In a doubly linked list, the end is represented by NULL
}

/**
 * @brief Returns a constant iterator to the first element of the list.
 *
 * This function returns a constant pointer to the first element in the list. If the list is empty, 
 * it returns NULL.
 * 
 * @param list Pointer to the list.
 * @return const Node* Constant pointer to the first element of the list, or NULL if the list is empty.
 */
const Node *list_cbegin(const List *list) {
    if (list == NULL) {
        LIST_LOG("[list_cbegin] : Error: Null pointer provided for list in list_cbegin.");
        return NULL;
    }
    return list->head;
}

/**
 * @brief Returns a constant iterator to the end of the list.
 *
 * This function returns a constant iterator (pointer) to the element following the last element 
 * in the list. Since the end is represented by NULL, this function will always return NULL.
 * 
 * @param list Pointer to the list.
 * @return const Node* Constant iterator pointing to the end of the list (NULL).
 */
const Node *list_cend(const List* list) {
    if (list == NULL) {
        LIST_LOG("[list_cend] : Error: Null pointer provided for list in list_cend.");
        return NULL;
    }
    return NULL;
}

/**
 * @brief Returns a constant reverse iterator to the last element of the list.
 *
 * This function returns a constant reverse iterator (pointer) to the last element in the list. 
 * It is useful for iterating over the list in reverse order.
 * 
 * @param list Pointer to the list.
 * @return const Node* Constant reverse iterator pointing to the last element of the list.
 */
const Node *list_crbegin(const List* list) {
    if (list == NULL) {
        LIST_LOG("[list_crbegin] : Error: Null pointer provided for list in list_crbegin.");
        return NULL;
    }
    return list->tail;
}

/**
 * @brief Returns a constant reverse iterator to the theoretical element preceding the first element of the list.
 *
 * This function returns a constant reverse iterator (pointer) to the element preceding the first element 
 * in the list. Since the reverse end is represented by NULL, this function will always return NULL.
 * 
 * @param list Pointer to the list.
 * @return const Node* Constant reverse iterator pointing to the theoretical element before the first element of the list (NULL).
 */
const Node *list_crend(const List* list) {
    if (list == NULL) {
        LIST_LOG("[list_crend] : Error: Null pointer provided for list in list_crend.");
        return NULL;
    }
    return NULL;
}

/**
 * @brief Assigns values to the list from an array.
 *
 * This function assigns values from a given array to the list, replacing its current contents. 
 * The list is cleared before the new values are inserted.
 * 
 * @param list Pointer to the list.
 * @param values Pointer to the array of values to be assigned to the list.
 * @param numValues Number of values in the array.
 */
void list_assign(List *list, void *values, size_t numValues) {
    if (list == NULL) {
        LIST_LOG("[list_assign] : Error: Null pointer provided for list in list_assign.");
        return;
    }
    if (values == NULL && numValues > 0) {
        LIST_LOG("[list_assign] : Error: Null pointer provided for values in list_assign.");
        return;
    }

    list_clear(list); // Clear the existing content

    for (size_t i = 0; i < numValues; ++i) {
        void *currentValue = (char *)values + i * list->itemSize;
        list_push_back(list, currentValue);
    }
    LIST_LOG("[list_assign] : List assigned with %zu values.", numValues);
}

/**
 * @brief Inserts an element at the front of the list without copying.
 *
 * This function inserts an element at the front of the list by directly using the provided value 
 * without making a copy. The list takes ownership of the value, and it should not be modified 
 * by the caller after this operation.
 * 
 * @param list Pointer to the list.
 * @param value Pointer to the value to be inserted.
 */
void list_emplace_front(List *list, void *value) {
    if (list == NULL || value == NULL) {
        LIST_LOG("[list_emplace_front] : Error: Null pointer provided for list or value in list_emplace_front.");
        return;
    }

    Node *newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        LIST_LOG("[list_emplace_front] : Error: Memory allocation failed for newNode in list_emplace_front.");
        return;
    }
    newNode->value = value;  // Directly use the provided value
    newNode->next = list->head;
    newNode->prev = NULL;

    if (list->head != NULL) { 
        list->head->prev = newNode;
    }
    list->head = newNode;

    if (list->tail == NULL) { 
        list->tail = newNode;
    }

    list->size++;
    LIST_LOG("[list_emplace_front] : Node emplaced at front. List size is now: %zu", list->size);
}


/**
 * @brief Inserts an element at the back of the list without copying.
 *
 * This function inserts an element at the back of the list by directly using the provided value 
 * without making a copy. The list takes ownership of the value, and it should not be modified 
 * by the caller after this operation.
 * 
 * @param list Pointer to the list.
 * @param value Pointer to the value to be inserted.
 */
void list_emplace_back(List *list, void *value) {
    if (list == NULL || value == NULL) {
        LIST_LOG("[list_emplace_back] : Error: Null pointer provided for list or value in list_emplace_back.");
        return;
    }

    Node *newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        LIST_LOG("[list_emplace_back] : Error: Memory allocation failed for newNode in list_emplace_back.");
        return;
    }

    newNode->value = value;  // Directly use the provided value
    newNode->next = NULL;
    newNode->prev = list->tail;

    if (list->tail != NULL) { 
        list->tail->next = newNode;
    }
    else {
        list->head = newNode;
    }
    list->tail = newNode;
    list->size++;

    LIST_LOG("[list_emplace_back] : Node emplaced at back. List size is now: %zu", list->size);
}

/**
 * @brief Splices elements from one list into another at a specified position.
 * 
 * This function moves the entire content of the source list (`src`) into the destination list (`dest`) at the 
 * specified position (`pos`). The source list will be empty after the operation. If `pos` is NULL, 
 * the elements are inserted at the end of the destination list.
 * 
 * @param dest Pointer to the destination list where elements will be spliced in.
 * @param src Pointer to the source list from which elements will be moved.
 * @param pos Pointer to the node in the destination list before which the source list will be inserted.
 */
void list_splice(List *dest, List *src, Node *pos) {
    if (dest == NULL || src == NULL) {
        LIST_LOG("[list_splice] : Error: Null pointer provided for one or both lists in list_splice.");
        return;
    }
    if (dest == src) {
        LIST_LOG("[list_splice] : Error: Cannot splice a list into itself in list_splice.");
        return;
    }
    if (src->head == NULL) {
        LIST_LOG("[list_splice] : Source list is empty, nothing to splice in list_splice.");
        return;
    }

    Node *srcFirst = src->head;
    Node *srcLast = src->tail;

    // If 'pos' is NULL, insert at the end of 'dest'
    if (pos == NULL) {
        LIST_LOG("[list_splice] : Splicing at the end of the destination list.");
        if (dest->tail != NULL) { 
            dest->tail->next = srcFirst;
        }
        else { 
            dest->head = srcFirst;
        }
        srcFirst->prev = dest->tail;
        dest->tail = srcLast;
    } 
    else {
        LIST_LOG("[list_splice] : Splicing at the specified position in the destination list.");
        if (pos->prev != NULL) { 
            pos->prev->next = srcFirst;
        }
        else {
            dest->head = srcFirst;
        }
        srcFirst->prev = pos->prev;
        srcLast->next = pos;
        pos->prev = srcLast;
    }

    dest->size += src->size;
    src->head = src->tail = NULL; // Clear 'src' list
    src->size = 0;

    LIST_LOG("[list_splice] : Splicing completed. Destination list size: %zu. Source list is now empty.", dest->size);
}

/**
 * @brief Removes elements equal to a specified value from the list.
 * 
 * This function removes all elements in the list that are equal to the given value, as determined by 
 * the comparison function provided during list creation. The memory for the removed elements is freed.
 * 
 * @param list Pointer to the list from which elements will be removed.
 * @param value Pointer to the value to compare against when removing elements.
 */
void list_remove(List *list, void *value) {
    if (list == NULL) {
        LIST_LOG("[list_remove] : Error: Null pointer provided for list in list_remove.");
        return;
    }
    if (value == NULL) {
        LIST_LOG("[list_remove] : Error: Null value provided for comparison in list_remove.");
        return;
    }
    if (list->compare == NULL) {
        LIST_LOG("[list_remove] : Error: Null compare function provided in list_remove.");
        return;
    }

    Node *current = list->head;

    LIST_LOG("[list_remove] : Starting removal of nodes matching the provided value.");
    while (current != NULL) {
        Node *next = current->next;

        if (list->compare(current->value, value) == 0) {
            LIST_LOG("[list_remove] : Removing a node with the matching value.");

            if (current->prev) {
                current->prev->next = next;
            }
            else {
                list->head = next;
            }
            if (next) { 
                next->prev = current->prev;
            }
            else { 
                list->tail = current->prev;
            }
            free(current->value);
            free(current);

            list->size--;
            LIST_LOG("[list_remove] : Node removed, new list size: %zu", list->size);
        }
        current = next;
    }

    LIST_LOG("[list_remove] : Finished removing nodes. Final list size: %zu", list->size);
}

/**
 * @brief Removes elements from the list that satisfy a given condition.
 * 
 * This function iterates through the list and removes all elements for which the condition function (`cond`) 
 * returns true. The memory for the removed elements is freed.
 * 
 * @param list Pointer to the list from which elements will be removed.
 * @param cond Function pointer to the condition function that determines whether an element should be removed.
 */
void list_remove_if(List *list, ConditionFunction cond) {
    if (list == NULL) {
        LIST_LOG("[list_remove_if] : Error: Null pointer provided for list in list_remove_if.");
        return;
    }
    if (cond == NULL) {
        LIST_LOG("[list_remove_if] : Error: Null condition function provided in list_remove_if.");
        return;
    }

    Node *current = list->head;

    LIST_LOG("[list_remove_if] : Starting conditional removal of nodes.");
    while (current != NULL) {
        Node *next = current->next;

        if (cond(current->value)) {
            LIST_LOG("[list_remove_if] : Removing a node that satisfies the condition.");

            if (current->prev) {
                current->prev->next = next;
            }
            else { 
                list->head = next;
            }
            if (next) { 
                next->prev = current->prev;
            }
            else {
                list->tail = current->prev;
            }
            free(current->value);
            free(current);
            list->size--;

            LIST_LOG("[list_remove_if] : Node removed, new list size: %zu", list->size);
        }
        current = next;
    }

    LIST_LOG("[list_remove_if] : Finished removing nodes based on condition. Final list size: %zu", list->size);
}

/**
 * @brief Removes consecutive duplicate elements from the list.
 * 
 * This function iterates through the list and removes consecutive elements that are equal 
 * according to the comparison function provided during list creation. 
 * It ensures that only unique elements remain in the list.
 * 
 * @param list Pointer to the list from which to remove duplicates.
 */
void list_unique(List *list) {
    if (list == NULL) {
        LIST_LOG("[list_unique] : Error: Null pointer provided for list in list_unique.");
        return;
    }
    if (list->compare == NULL) {
        LIST_LOG("[list_unique] : Error: Null compare function provided in list_unique.");
        return;
    }
    if (list->size < 2) {
        LIST_LOG("[list_unique] : No action needed, list has less than two elements.");
        return;
    }

    LIST_LOG("[list_unique] : Starting process to remove duplicate elements from the list.");
    Node *current = list->head;

    while (current != NULL && current->next != NULL) {
        if (list->compare(current->value, current->next->value) == 0) {
            Node *duplicate = current->next;
            current->next = duplicate->next;

            if (duplicate->next) {
                duplicate->next->prev = current;
            } 
            else { 
                list->tail = current;
            }

            free(duplicate->value);
            free(duplicate);
            list->size--;

            LIST_LOG("[list_unique] : Removed a duplicate node, new list size: %zu", list->size);
        } 
        else {
            current = current->next;
        }
    }

    LIST_LOG("[list_unique] : Finished removing duplicates, final list size: %zu", list->size);
}

/**
 * @brief Merges two sorted lists into the first list, maintaining sort order.
 * 
 * This function merges the second list into the first list. Both lists must be sorted before 
 * calling this function. The resulting list will contain all elements from both lists, 
 * sorted in non-decreasing order. The second list will be empty after the merge.
 * 
 * @param list1 Pointer to the first list, which will contain the merged elements.
 * @param list2 Pointer to the second list, which will be emptied after the merge.
 */
void list_merge(List *list1, List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_merge] : Error: Null pointer provided for one or both lists in list_merge.");
        return;
    }
    if (list1 == list2 || list2->size == 0) {
        LIST_LOG("[list_merge] : Error: No merge needed, lists are identical or second list is empty in list_merge.");
        return;
    }
    if (list1->size == 0) {
        list1->head = list2->head;
        list1->tail = list2->tail;
        list1->size = list2->size;
        list2->head = list2->tail = NULL;
        list2->size = 0;

        LIST_LOG("[list_merge] : List1 was empty, transferred all nodes from list2. New list1 size: %zu", list1->size);
        return;
    }

    Node *current1 = list1->head;
    Node *current2 = list2->head;
    LIST_LOG("[list_merge] : Merging list2 into list1.");

    while (current1 != NULL && current2 != NULL) {
        if (list1->compare && list1->compare(current1->value, current2->value) > 0) {
            Node *next2 = current2->next; // Insert current2 before current1    
            current2->prev = current1->prev;
            current2->next = current1;

            if (current1->prev) {
                current1->prev->next = current2;
            } 
            else { 
                list1->head = current2;
            }

            current1->prev = current2;
            current2 = next2;

            list2->size--;
            list1->size++;

            LIST_LOG("[list_merge] : Inserted node from list2 into list1. New list1 size: %zu", list1->size);
        } 
        else {
            current1 = current1->next;
        }
    }

    if (current2 != NULL) {
        LIST_LOG("[list_merge] : Attaching remaining nodes from list2 to the end of list1.");
        list1->tail->next = current2;
        current2->prev = list1->tail;
        list1->tail = list2->tail;
        list1->size += list2->size;
    }

    list2->head = list2->tail = NULL;
    list2->size = 0;

    LIST_LOG("[list_merge] : Merge completed. Final list1 size: %zu. List2 is now empty.", list1->size);
}

/**
 * @brief Checks if the first list is lexicographically less than the second list.
 * 
 * This function compares two lists element by element. It returns true if the first list 
 * is lexicographically less than the second list, and false otherwise. If the lists are of 
 * different lengths, the shorter list is considered smaller.
 * 
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 * 
 * @return true if list1 is less than list2, false otherwise.
 */
bool list_is_less(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_is_less] : Error: Null pointer provided for one or both lists in list_is_less.");
        return false;
    }
    LIST_LOG("[list_is_less] : Comparing if list1 is less than list2.");

    if (list1->size != list2->size) {
        LIST_LOG("[list_is_less] : Lists have different sizes: list1->size = %zu, list2->size = %zu", list1->size, list2->size);
        return list1->size < list2->size;
    }

    Node *node1 = list1->head;
    Node *node2 = list2->head;

    while (node1 != NULL && node2 != NULL) {
        int val1 = *(int *)(node1->value);
        int val2 = *(int *)(node2->value);

        LIST_LOG("[list_is_less] : Comparing node values: val1 = %d, val2 = %d", val1, val2);
        if (val1 != val2) {
            LIST_LOG("[list_is_less] : Returning comparison result: %d", val1 < val2);
            return val1 < val2;
        }
        node1 = node1->next;
        node2 = node2->next;
    }

    LIST_LOG("[list_is_less] : Lists are equal, returning false.");
    return false; // Lists are equal
}

/**
 * @brief Checks if the first list is lexicographically greater than the second list.
 * 
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 * 
 * @return true if list1 is greater than list2, false otherwise.
 */
bool list_is_greater(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_is_greater] : Error: Null pointer provided for one or both lists in list_is_greater.");
        return false;
    }

    LIST_LOG("[list_is_greater] : Comparing if list1 is greater than list2.");
    bool result = list_is_less(list2, list1);
    LIST_LOG("[list_is_greater] : Comparison result in list_is_greater: %d", result);

    return result;
}

/**
 * @brief Checks if two lists are lexicographically equal.
 * 
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 * 
 * @return true if both lists are equal, false otherwise.
 */
bool list_is_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_is_equal] : Error: Null pointer provided for one or both lists in list_is_equal.");
        return false;
    }
    if (list1->size != list2->size) {
        LIST_LOG("[list_is_equal] : Lists have different sizes: list1->size = %zu, list2->size = %zu", list1->size, list2->size);
        return false;
    }

    Node *node1 = list1->head;
    Node *node2 = list2->head;

    while (node1 != NULL && node2 != NULL) {
        int cmp_result = list1->compare(node1->value, node2->value);
        LIST_LOG("[list_is_equal] : Comparing node values using custom comparison function: result = %d", cmp_result);

        if (cmp_result != 0) {
            LIST_LOG("[list_is_equal] : Nodes are not equal, returning false.");
            return false;
        }

        node1 = node1->next;
        node2 = node2->next;
    }

    LIST_LOG("[list_is_equal] : Lists are equal, returning true.");
    return true;
}

/**
 * @brief Checks if the first list is lexicographically less than or equal to the second list.
 * 
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 * 
 * @return true if list1 is less than or equal to list2, false otherwise.
 */
bool list_is_less_or_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_is_less_or_equal] : Error: Null pointer provided for one or both lists in list_is_less_or_equal.");
        return false;
    }

    bool result = list_is_less(list1, list2) || list_is_equal(list1, list2);
    LIST_LOG("[list_is_less_or_equal] : Comparison result in list_is_less_or_equal: %d", result);

    return result;
}

/**
 * @brief Checks if the first list is lexicographically greater than or equal to the second list.
 * 
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 * 
 * @return true if list1 is greater than or equal to list2, false otherwise.
 */
bool list_is_greater_or_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_is_greater_or_equal] : Error: Null pointer provided for one or both lists in list_is_greater_or_equal.");
        return false;
    }
    bool result = list_is_greater(list1, list2) || list_is_equal(list1, list2);
    LIST_LOG("[list_is_greater_or_equal] : Comparison result in list_is_greater_or_equal: %d", result);

    return result;
}

/**
 * @brief Checks if two lists are not lexicographically equal.
 * 
 * @param list1 Pointer to the first list.
 * @param list2 Pointer to the second list.
 * 
 * @return true if the lists are not equal, false if they are equal.
 */
bool list_is_not_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        LIST_LOG("[list_is_not_equal] : Error: Null pointer provided for one or both lists in list_is_not_equal.");
        return true; 
    }
    bool result = !list_is_equal(list1, list2);
    LIST_LOG("[list_is_not_equal] : Comparison result in list_is_not_equal: %d", result);

    return result;
}