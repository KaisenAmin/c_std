#include "list.h"
#include <stdlib.h>
#include <string.h>

static void *list_front_impl(const List *list);
static void *list_back_impl(const List *list);
static void *list_insert_impl(List *list, size_t index, void *value);
static void *list_erase_impl(List *list, size_t index);
static void list_resize_impl(List *list, size_t newSize, void *defaultValue);
static void list_swap_impl(List *list1, List *list2);
static void list_reverse_impl(List *list);
static void list_sort_impl(List* list);
static void list_push_front_impl(List *list, void *value);
static void list_push_back_impl(List *list, void *value);
static void list_pop_front_impl(List *list);
static void list_pop_back_impl(List *list);
static void list_clear_impl(List *list);
static bool list_empty_impl(const List *list);
static size_t list_length_impl(const List *list);
static void list_deallocate_impl(List *list);
static Node *list_begin_impl(const List *list);
static Node *list_end_impl(const List *list);
static Node *list_rbegin_impl(const List *list);  // Only for doubly linked lists
static Node *list_rend_impl(const List *list); 
static const Node *list_cbegin_impl(const List *list);
static const Node *list_cend_impl(const List* list);
static const Node *list_crbegin_impl(const List* list);
static const Node *list_crend_impl(const List* list);
static void list_assign_impl(List *list, void *values, size_t numValues);
static void list_emplace_front_impl(List *list, void *value);
static void list_emplace_back_impl(List *list, void *value);
static void list_splice_impl(List *dest, List *src, Node *pos);
static void list_remove_impl(List *list, void *value);
static void list_remove_if_impl(List *list, ConditionFunction cond);
static void list_unique_impl(List *list);
static void list_merge_impl(List *list1, List *list2);
static bool list_is_less_impl(const List *list1, const List *list2);
static bool list_is_greater_impl(const List *list1, const List *list2);
static bool list_is_equal_impl(const List *list1, const List *list2);
static bool list_is_less_or_equal_impl(const List *list1, const List *list2);
static bool list_is_greater_or_equal_impl(const List *list1, const List *list2);
static bool list_is_not_equal_impl(const List *list1, const List *list2);

List *list_create(size_t itemSize, CompareFunction compare) 
{
    List *list = malloc(sizeof(List));
    if (!list) return NULL;

    list->head = list->tail = NULL;
    list->size = 0;
    list->itemSize = itemSize;
    list->compare = compare; 

    // Assign function pointers
    list->assign = list_assign_impl;
    list->back = list_back_impl;
    list->begin = list_begin_impl;
    list->cbegin = list_cbegin_impl;
    list->cend = list_cend_impl;
    list->clear = list_clear_impl;
    list->crbegin = list_crbegin_impl;
    list->crend = list_crend_impl;
    list->deallocate = list_deallocate_impl;
    list->emplace_back = list_emplace_back_impl;
    list->emplace_front = list_emplace_front_impl;
    list->empty = list_empty_impl;
    list->end = list_end_impl;
    list->erase = list_erase_impl;
    list->front = list_front_impl;
    list->push_front = list_push_front_impl;
    list->push_back = list_push_back_impl;
    list->pop_front = list_pop_front_impl;
    list->pop_back = list_pop_back_impl;
    list->length = list_length_impl;
    list->insert = list_insert_impl;
    list->merge = list_merge_impl;
    list->rbegin = list_rbegin_impl;
    list->remove = list_remove_impl;
    list->remove_if = list_remove_if_impl;
    list->rend = list_rend_impl;
    list->resize = list_resize_impl;
    list->reverse = list_reverse_impl;
    list->sort = list_sort_impl;
    list->splice = list_splice_impl;
    list->swap = list_swap_impl;
    list->unique = list_unique_impl;
    list->is_less = list_is_less_impl;
    list->is_greater = list_is_greater_impl;
    list->is_equal = list_is_equal_impl;
    list->is_less_or_equal = list_is_less_or_equal_impl;
    list->is_greater_or_equal = list_is_greater_or_equal_impl;
    list->is_not_equal = list_is_not_equal_impl;

    return list;
}

static void *list_front_impl(const List *list) 
{
    if (list->head == NULL) 
        return NULL; // The list is empty
    
    return list->head->value;
}

static void *list_back_impl(const List *list) 
{
    if (list->tail == NULL) 
        return NULL;

    return list->tail->value;
}

static void *list_insert_impl(List *list, size_t index, void *value) 
{
    if (index > list->size) 
        return NULL; // Index out of bounds
    
    if (index == 0) 
    {
        list_push_front_impl(list, value);
        return list->head->value;
    } 
    else if (index == list->size) 
    {
        list_push_back_impl(list, value);
        return list->tail->value;
    }

    Node *newNode = malloc(sizeof(Node));
    if (!newNode) 
        return NULL;
    
    newNode->value = malloc(list->itemSize);

    if (!newNode->value) 
    {
        free(newNode);
        return NULL;
    }

    memcpy(newNode->value, value, list->itemSize);
    Node *current = list->head;

    for (size_t i = 0; i < index - 1; ++i) 
        current = current->next;
    
    newNode->next = current->next;
    newNode->prev = current;  // Set the prev pointer to the current node

    if (newNode->next != NULL) 
        newNode->next->prev = newNode;  // Update the prev pointer of the next node
    
    current->next = newNode;
    list->size++;

    return newNode->value;
}

static void *list_erase_impl(List *list, size_t index) 
{
    if (index >= list->size) 
        return NULL;

    Node *nodeToRemove;
    if (index == 0) 
    {
        nodeToRemove = list->head;
        list->head = nodeToRemove->next;

        if (list->head) 
            list->head->prev = NULL; 
    } 
    else 
    {
        Node *current = list->head;
        for (size_t i = 0; i < index - 1; ++i) 
            current = current->next;
        
        nodeToRemove = current->next;
        current->next = nodeToRemove->next;

        if (nodeToRemove->next) 
            nodeToRemove->next->prev = current;
    }

    if (nodeToRemove == list->tail) 
        list->tail = nodeToRemove->prev;

    void *removedValue = nodeToRemove->value;
    free(nodeToRemove);
    list->size--;

    if (list->size == 0) 
        list->head = list->tail = NULL;
    
    return removedValue;
}

static void list_resize_impl(List *list, size_t newSize, void *defaultValue) 
{
    while (list->size > newSize) 
        list_pop_back_impl(list);  

    while (list->size < newSize) 
        list_push_back_impl(list, defaultValue);
}

static void list_swap_impl(List *list1, List *list2) 
{
    Node *tempHead = list1->head; // Swap the heads
    list1->head = list2->head;
    list2->head = tempHead;

    Node *tempTail = list1->tail; // Swap the tails
    list1->tail = list2->tail;
    list2->tail = tempTail;

    size_t tempSize = list1->size; // Swap the sizes
    list1->size = list2->size;
    list2->size = tempSize;

    size_t tempItemSize = list1->itemSize; // Swap the item sizes
    list1->itemSize = list2->itemSize;
    list2->itemSize = tempItemSize;
}

static void list_reverse_impl(List *list) 
{
    Node *current = list->head;
    Node *temp = NULL;

    while (current != NULL) 
    {
        temp = current->prev; // Swap the next and prev pointers
        current->prev = current->next;
        current->next = temp;
        current = current->prev; // Move to the next node in the original list
    }

    temp = list->head; // Swap the head and tail of the list
    list->head = list->tail;
    list->tail = temp;
}

static void list_sort_impl(List* list) 
{
    if (list->size < 2 || list->compare == NULL) 
        return;
    
    bool swapped;

    do 
    {
        swapped = false;
        Node *current = list->head;

        while (current != NULL && current->next != NULL) 
        {
            if (list->compare(current->value, current->next->value) > 0) 
            {
                void *temp = current->value;
                current->value = current->next->value;
                current->next->value = temp;
                swapped = true;
            }
            current = current->next;
        }

    } while (swapped);
}

static void list_push_front_impl(List *list, void *value) 
{
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) 
        return;

    newNode->value = malloc(list->itemSize);

    if (!newNode->value) 
    {
        free(newNode);
        return;
    }

    memcpy(newNode->value, value, list->itemSize);
    newNode->next = list->head;
    newNode->prev = NULL;  // As this will be the new first node

    if (list->head != NULL) 
        list->head->prev = newNode;

    list->head = newNode;

    if (list->tail == NULL) 
        list->tail = newNode;
    
    list->size++;
}

static void list_push_back_impl(List *list, void *value) 
{
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) 
        return;  // Memory allocation failure

    newNode->value = malloc(list->itemSize);

    if (!newNode->value) 
    {
        free(newNode);
        return;  // Memory allocation failure
    }

    memcpy(newNode->value, value, list->itemSize);
    newNode->next = NULL;  // As this is the last node
    newNode->prev = list->tail;  // Set the prev pointer to the current tail

    if (list->tail != NULL) 
        list->tail->next = newNode;  // Update the next pointer of the current tail

    list->tail = newNode;  // Update the tail of the list

    if (list->head == NULL) 
        list->head = newNode;  // If the list was empty, set the head to the new node
    
    list->size++;
}

static void list_pop_front_impl(List *list) 
{
    if (list->head == NULL) 
        return;  // The list is empty, nothing to pop

    Node *temp = list->head;
    list->head = list->head->next;  // Update the head to the next node

    if (list->head != NULL) 
        list->head->prev = NULL;  // Set the prev pointer of the new head to NULL
    else 
        list->tail = NULL;  // If the list becomes empty, set the tail to NULL
    
    free(temp->value);  // Free the value and the node
    free(temp);

    list->size--;
}

static void list_pop_back_impl(List *list) 
{
    if (list->head == NULL) 
        return;  // The list is empty, nothing to pop
    
    
    if (list->head == list->tail) // If there is only one node in the list
    {
        free(list->head->value);
        free(list->head);
        list->head = list->tail = NULL;
    } 
    else 
    {
        Node *lastNode = list->tail;
        list->tail = lastNode->prev;
        list->tail->next = NULL;

        free(lastNode->value);
        free(lastNode);
    }

    list->size--;
}

static void list_clear_impl(List *list) 
{
    Node *current = list->head;

    while (current != NULL) 
    {
        Node *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }

    list->head = list->tail = NULL;
    list->size = 0;
}

static bool list_empty_impl(const List *list) 
{
    return list->head == NULL;
}

static size_t list_length_impl(const List *list) 
{
    return list->size;
}

static void list_deallocate_impl(List *list) 
{
    if (list == NULL) 
        return;
    
    list_clear_impl(list); // First, clear the list
    free(list);            // Then, free the list structure itself
}

static Node *list_begin_impl(const List *list) 
{
    return list->head;
}

static Node *list_end_impl(const List *list) 
{   
    return NULL; // In a singly linked list, the end is represented by NULL
}

static Node *list_rbegin_impl(const List *list) 
{
    return list->tail;
}

static Node *list_rend_impl(const List *list) 
{
    return NULL;  // In a doubly linked list, the end is still represented by NULL
}

static const Node *list_cbegin_impl(const List *list)
{
    return list->head;
}

static const Node *list_cend_impl(const List* list)
{
    return NULL;
}

static const Node *list_crbegin_impl(const List* list)
{
    return list->tail;
}

static const Node *list_crend_impl(const List* list)
{
    return NULL;
}

static void list_assign_impl(List *list, void *values, size_t numValues) 
{
    list_clear_impl(list); // Clear the existing content

    for (size_t i = 0; i < numValues; ++i) 
    {
        void *currentValue = (char *)values + i * list->itemSize;
        list_push_back_impl(list, currentValue);
    }
}

static void list_emplace_front_impl(List *list, void *value) 
{
    Node *newNode = malloc(sizeof(Node));

    if (!newNode) 
        return;

    newNode->value = value;  // Directly use the provided value
    newNode->next = list->head;
    newNode->prev = NULL;

    if (list->head != NULL) 
        list->head->prev = newNode;

    list->head = newNode;

    if (list->tail == NULL) 
        list->tail = newNode;
    
    list->size++;
}

static void list_emplace_back_impl(List *list, void *value) 
{
    Node *newNode = malloc(sizeof(Node));

    if (!newNode) 
        return;

    newNode->value = value;  // Directly use the provided value
    newNode->next = NULL;
    newNode->prev = list->tail;

    if (list->tail != NULL) 
        list->tail->next = newNode;
     else 
        list->head = newNode;

    list->tail = newNode;
    list->size++;
}

static void list_splice_impl(List *dest, List *src, Node *pos) 
{
    if (src->head == NULL || dest == src) 
        return;

    Node *srcFirst = src->head;
    Node *srcLast = src->tail;

    if (pos == NULL) // If 'pos' is NULL, insert at the end of 'dest'
    {
        if (dest->tail != NULL) 
            dest->tail->next = srcFirst;
        else 
            dest->head = srcFirst;
    
        srcFirst->prev = dest->tail;
        dest->tail = srcLast;
    } 
    else 
    {
        if (pos->prev != NULL) 
            pos->prev->next = srcFirst;
        else 
            dest->head = srcFirst;
        
        srcFirst->prev = pos->prev;
        srcLast->next = pos;
        pos->prev = srcLast;
    }

    dest->size += src->size;

    src->head = src->tail = NULL; // Clear 'src' list
    src->size = 0;
}

static void list_remove_impl(List *list, void *value) 
{
    Node *current = list->head;

    while (current != NULL) 
    {
        Node *next = current->next;

        if (list->compare(current->value, value) == 0) 
        {
            if (current->prev) 
                current->prev->next = next;
            else 
                list->head = next;
    
            if (next) 
                next->prev = current->prev;
            else 
                list->tail = current->prev;

            free(current->value);
            free(current);

            list->size--;
        }
        current = next;
    }
}

static void list_remove_if_impl(List *list, ConditionFunction cond) 
{
    Node *current = list->head;

    while (current != NULL) 
    {
        Node *next = current->next;

        if (cond(current->value)) 
        {
            if (current->prev) 
                current->prev->next = next;
            else 
                list->head = next;
            
            if (next) 
                next->prev = current->prev;
            else 
                list->tail = current->prev;

            free(current->value);
            free(current);
            list->size--;
        }

        current = next;
    }
}

static void list_unique_impl(List *list) 
{
    if (list->size < 2) 
        return;

    Node *current = list->head;

    while (current != NULL && current->next != NULL) 
    {
        if (list->compare(current->value, current->next->value) == 0) 
        {
            Node *duplicate = current->next;
            current->next = duplicate->next;

            if (duplicate->next) 
                duplicate->next->prev = current;
            else 
                list->tail = current;

            free(duplicate->value);
            free(duplicate);
            list->size--;

        }
        else 
            current = current->next;
    }
}

static void list_merge_impl(List *list1, List *list2) 
{
    if (list1 == list2 || list2->size == 0) 
        return;

    Node *current1 = list1->head;
    Node *current2 = list2->head;

    while (current1 != NULL && current2 != NULL) 
    {
        if (list1->compare(current1->value, current2->value) > 0) 
        {
            Node *next2 = current2->next; // Insert current2 before current1
            
            current2->prev = current1->prev;
            current2->next = current1;

            if (current1->prev) 
                current1->prev->next = current2;
            else 
                list1->head = current2;
        
            current1->prev = current2;
            current2 = next2;

            list2->size--;
            list1->size++;
        } 
        else 
            current1 = current1->next;
    }

    if (current2 != NULL) 
    {
        list1->tail->next = current2;
        current2->prev = list1->tail;
        list1->tail = list2->tail;
        list1->size += list2->size;
    }

    list2->head = list2->tail = NULL;
    list2->size = 0;
}

static bool list_is_less_impl(const List *list1, const List *list2) 
{
    if (list1->size != list2->size)
        return list1->size < list2->size;

    Node *node1 = list1->head;
    Node *node2 = list2->head;

    while (node1 != NULL && node2 != NULL) 
    {
        int val1 = *(int *)(node1->value);
        int val2 = *(int *)(node2->value);

        if (val1 != val2)
            return val1 < val2;

        node1 = node1->next;
        node2 = node2->next;
    }

    return false; // Lists are equal
}

static bool list_is_greater_impl(const List *list1, const List *list2) 
{
    return list_is_less_impl(list2, list1);
}

static bool list_is_equal_impl(const List *list1, const List *list2) 
{
    if (list1->size != list2->size)
        return false;

    Node *node1 = list1->head;
    Node *node2 = list2->head;

    while (node1 != NULL && node2 != NULL) 
    {
        if (list1->compare(node1->value, node2->value) != 0)
            return false;

        node1 = node1->next;
        node2 = node2->next;
    }

    return true;
}

static bool list_is_less_or_equal_impl(const List *list1, const List *list2) 
{
    return list1->is_less(list1, list2) || list1->is_equal(list1, list2);
}

static bool list_is_greater_or_equal_impl(const List *list1, const List *list2) 
{
    return list1->is_greater(list1, list2) || list1->is_equal(list1, list2);
}

static bool list_is_not_equal_impl(const List *list1, const List *list2) 
{
    return !list1->is_equal(list1, list2);
}
