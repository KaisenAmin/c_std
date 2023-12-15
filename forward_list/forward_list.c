#include "forward_list.h"
#include <stdlib.h>
#include <string.h>

static void forward_list_push_front_impl(ForwardList *list, void *value);
static void forward_list_pop_front_impl(ForwardList *list);
static void *forward_list_front_impl(const ForwardList *list);
static void forward_list_clear_impl(ForwardList *list);
static bool forward_list_empty_impl(const ForwardList *list);
static size_t forward_list_length_impl(const ForwardList *list);
static void forward_list_deallocate_impl(ForwardList *list);
static void forward_list_assign_impl(ForwardList *list, void *values, size_t numValues);
static ForwardListNode *forward_list_before_begin_impl(ForwardList *list);
static ForwardListNode *forward_list_begin_impl(ForwardList *list);
static ForwardListNode *forward_list_end_impl(ForwardList *list);
static size_t forward_list_max_size_impl(const ForwardList *list);
static void forward_list_emplace_front_impl(ForwardList *list, void *value);
static void forward_list_emplace_after_impl(ForwardList *list, ForwardListNode *pos, void *value);
static void forward_list_insert_after_impl(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues);
static void forward_list_erase_after_impl(ForwardList *list, ForwardListNode *pos);
static void forward_list_swap_impl(ForwardList *list1, ForwardList *list2);
static void forward_list_resize_impl(ForwardList *list, size_t newSize);
static void forward_list_splice_after_impl(ForwardList *list, ForwardListNode *pos, ForwardList *other);
static void forward_list_remove_impl(ForwardList *list, void *value);
static void forward_list_remove_if_impl(ForwardList *list, bool (*condition)(void*));
static void forward_list_unique_impl(ForwardList *list);
static void forward_list_merge_impl(ForwardList *list1, ForwardList *list2);
static void forward_list_sort_impl(ForwardList *list);
static void forward_list_reverse_impl(ForwardList *list);

ForwardList *forward_list_create(size_t itemSize) 
{
    ForwardList *list = malloc(sizeof(ForwardList));
    if (list == NULL) 
        return NULL;

    // Initialize the list
    list->head = NULL;
    list->itemSize = itemSize;
    list->size = 0;

    list->push_front = forward_list_push_front_impl;
    list->pop_front = forward_list_pop_front_impl;
    list->front = forward_list_front_impl;
    list->clear = forward_list_clear_impl;
    list->empty = forward_list_empty_impl;
    list->length = forward_list_length_impl;
    list->deallocate = forward_list_deallocate_impl;
    list->assign = forward_list_assign_impl;
    list->before_begin = forward_list_before_begin_impl;
    list->begin = forward_list_begin_impl;
    list->end = forward_list_end_impl;
    list->max_size = forward_list_max_size_impl;
    list->emplace_front = forward_list_emplace_front_impl;
    list->emplace_after = forward_list_emplace_after_impl;
    list->insert_after = forward_list_insert_after_impl;
    list->erase_after = forward_list_erase_after_impl;
    list->swap = forward_list_swap_impl;
    list->resize = forward_list_resize_impl;
    list->splice_after = forward_list_splice_after_impl;
    list->remove = forward_list_remove_impl;
    list->remove_if = forward_list_remove_if_impl;
    list->unique = forward_list_unique_impl;
    list->merge = forward_list_merge_impl;
    list->sort = forward_list_sort_impl;
    list->reverse = forward_list_reverse_impl;

    return list;
}

// Helper function to split the list into two halves
static ForwardListNode *split_list_for_sort(ForwardListNode *head) 
{
    ForwardListNode *fast = head, *slow = head, *prev = NULL;

    while (fast != NULL && fast->next != NULL) 
    {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    if (prev != NULL) 
        prev->next = NULL;

    return slow;
}

// Merge two sorted lists
static ForwardListNode *merge_sorted_lists(ForwardListNode *a, ForwardListNode *b, size_t itemSize) 
{
    ForwardListNode dummy;
    ForwardListNode *tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL) 
    {
        if (memcmp(a->value, b->value, itemSize) <= 0) 
        {
            tail->next = a;
            a = a->next;
        } 
        else 
        {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;

    return dummy.next;
}

// Recursive merge sort implementation
static ForwardListNode *merge_sort_impl(ForwardListNode *head, size_t itemSize) 
{
    if (head == NULL || head->next == NULL) 
        return head;

    ForwardListNode *middle = split_list_for_sort(head);
    ForwardListNode *left = merge_sort_impl(head, itemSize);
    ForwardListNode *right = merge_sort_impl(middle, itemSize);

    return merge_sorted_lists(left, right, itemSize);
}

static void forward_list_push_front_impl(ForwardList *list, void *value) 
{
    if (list == NULL || value == NULL) 
        return;

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));

    if (newNode == NULL) 
        return;

    newNode->value = malloc(list->itemSize);

    if (newNode->value == NULL) 
    {
        free(newNode);
        return;
    }
    memcpy(newNode->value, value, list->itemSize);

    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

static void forward_list_pop_front_impl(ForwardList *list) 
{
    if (list == NULL || list->head == NULL) 
        return;

    ForwardListNode *temp = list->head;
    list->head = list->head->next;

    free(temp->value);
    free(temp);
    list->size--;
}

static void *forward_list_front_impl(const ForwardList *list)
{
    if (list == NULL || list->head == NULL) 
        return NULL;

    return list->head->value;
}

static void forward_list_clear_impl(ForwardList *list)
{
    if (list == NULL) 
        return;

    ForwardListNode *current = list->head;

    while (current != NULL) 
    {
        ForwardListNode *next = current->next;

        free(current->value);
        free(current);

        current = next;
    }

    list->head = NULL;
    list->size = 0;
}

static bool forward_list_empty_impl(const ForwardList *list)
{
    if (list == NULL) 
        return true;  // Consider a NULL list as empty

    return list->head == NULL;
}

static size_t forward_list_length_impl(const ForwardList *list)
{
    if (list == NULL) 
        return 0;

    return list->size;
}

static void forward_list_deallocate_impl(ForwardList *list)
{
    if (list == NULL) 
        return;

    forward_list_clear_impl(list);  // Clear all nodes
    free(list);
}

static void forward_list_assign_impl(ForwardList *list, void *values, size_t numValues) {
    if (list == NULL || values == NULL) 
        return;

    forward_list_clear_impl(list);  // Clear existing contents
    for (size_t i = 0; i < numValues; ++i) 
    {
        void *value = (char *)values + i * list->itemSize; // Calculate the address of the value to be copied
        forward_list_push_front_impl(list, value);  // Add each new value to the front
    }
    
    forward_list_reverse_impl(list); // Reverse the list to maintain the correct order
}

static ForwardListNode *forward_list_before_begin_impl(ForwardList *list) 
{
    return NULL; // In a singly linked list, there is no node before the beginning
}

static ForwardListNode *forward_list_begin_impl(ForwardList *list) 
{
    if (list == NULL) 
        return NULL;

    return list->head;
}

static ForwardListNode *forward_list_end_impl(ForwardList *list) 
{
    return NULL; // In a singly linked list, the end is represented by NULL
}

static size_t forward_list_max_size_impl(const ForwardList *list) 
{
    return (size_t)-1;
}

static void forward_list_emplace_front_impl(ForwardList *list, void *value) 
{
    if (list == NULL || value == NULL) 
        return;

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));

    if (newNode == NULL) 
        return;

    newNode->value = value;
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

static void forward_list_emplace_after_impl(ForwardList *list, ForwardListNode *pos, void *value) 
{
    if (list == NULL || pos == NULL || value == NULL) 
        return;

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));

    if (newNode == NULL) 
        return;

    newNode->value = value;
    newNode->next = pos->next;
    pos->next = newNode;
    list->size++;
}

static void forward_list_insert_after_impl(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues) 
{
    if (list == NULL || pos == NULL || value == NULL) 
        return;

    for (size_t i = 0; i < numValues; ++i) 
    {
        void *currentValue = (char *)value + i * list->itemSize;
        ForwardListNode *newNode = malloc(sizeof(ForwardListNode));

        if (newNode == NULL) 
            return;

        newNode->value = malloc(list->itemSize);

        if (newNode->value == NULL) 
        {
            free(newNode);
            return;
        }
        memcpy(newNode->value, currentValue, list->itemSize);

        newNode->next = pos->next;
        pos->next = newNode;
        pos = newNode;  // Update pos to the last inserted node
        list->size++;
    }
}

static void forward_list_erase_after_impl(ForwardList *list, ForwardListNode *pos) 
{
    if (list == NULL || pos == NULL || pos->next == NULL) 
        return;

    ForwardListNode *temp = pos->next;
    pos->next = temp->next;

    free(temp->value);
    free(temp);
    list->size--;
}

static void forward_list_swap_impl(ForwardList *list1, ForwardList *list2) 
{
    if (list1 == NULL || list2 == NULL) 
        return;

    // Swap heads
    ForwardListNode *tempHead = list1->head;
    list1->head = list2->head;
    list2->head = tempHead;

    // Swap sizes
    size_t tempSize = list1->size;
    list1->size = list2->size;
    list2->size = tempSize;
}

static void forward_list_resize_impl(ForwardList *list, size_t newSize) 
{
    if (list == NULL) 
        return;

    while (list->size > newSize) 
        forward_list_pop_front_impl(list);
    
    while (list->size < newSize) 
    {
        void *nullValue = NULL; // Assuming that a null value is acceptable to add
        forward_list_emplace_front_impl(list, nullValue);
    }
}

static void forward_list_splice_after_impl(ForwardList *list, ForwardListNode *pos, ForwardList *other) 
{
    if (list == NULL || other == NULL || pos == NULL) 
        return;

    ForwardListNode *otherCurrent = other->head;

    if (otherCurrent == NULL) 
        return;

    while (otherCurrent->next != NULL)  // Find the last node of the other list
        otherCurrent = otherCurrent->next;

    // Splice
    otherCurrent->next = pos->next;
    pos->next = other->head;
    
    // Adjust size
    list->size += other->size;
    other->head = NULL;
    other->size = 0;
}

static void forward_list_remove_impl(ForwardList *list, void *value) 
{
    if (list == NULL || value == NULL) 
        return;

    while (list->head != NULL && memcmp(list->head->value, value, list->itemSize) == 0) 
        forward_list_pop_front_impl(list);
    
    ForwardListNode *current = list->head;

    while (current != NULL && current->next != NULL) 
    {
        if (memcmp(current->next->value, value, list->itemSize) == 0) 
        {
            ForwardListNode *temp = current->next;
            current->next = temp->next;
            free(temp->value);
            free(temp);
            list->size--;
        } 
        else 
            current = current->next;
    }
}

static void forward_list_remove_if_impl(ForwardList *list, bool (*condition)(void*)) 
{
    if (list == NULL || condition == NULL) 
        return;

    while (list->head != NULL && condition(list->head->value)) 
        forward_list_pop_front_impl(list);
    
    ForwardListNode *current = list->head;

    while (current != NULL && current->next != NULL) 
    {
        if (condition(current->next->value)) 
        {
            ForwardListNode *temp = current->next;
            current->next = temp->next;
            free(temp->value);
            free(temp);
            list->size--;
        } 
        else 
            current = current->next;
    }
}

static void forward_list_unique_impl(ForwardList *list) 
{
    if (list == NULL || list->head == NULL || list->head->next == NULL) 
        return;

    ForwardListNode *current = list->head;

    while (current->next != NULL) 
    {
        if (memcmp(current->value, current->next->value, list->itemSize) == 0) 
        {
            ForwardListNode *duplicate = current->next;
            current->next = duplicate->next;
            free(duplicate->value);
            free(duplicate);
            list->size--;
        } 
        else 
            current = current->next;
    }
}

static void forward_list_merge_impl(ForwardList *list1, ForwardList *list2) 
{
    if (list1 == NULL || list2 == NULL || list2->head == NULL) 
        return;

    ForwardListNode dummy;
    ForwardListNode *tail = &dummy;
    dummy.next = NULL;

    while (list1->head != NULL && list2->head != NULL) 
    {
        if (memcmp(list1->head->value, list2->head->value, list1->itemSize) <= 0) 
        {
            tail->next = list1->head;
            list1->head = list1->head->next;
        } 
        else 
        {
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
}

static void forward_list_sort_impl(ForwardList *list) 
{
    if (list == NULL || list->head == NULL) 
        return;

    list->head = merge_sort_impl(list->head, list->itemSize);
}

static void forward_list_reverse_impl(ForwardList *list) 
{
    if (list == NULL || list->head == NULL || list->head->next == NULL) 
        return;

    ForwardListNode *prev = NULL, *current = list->head, *next = NULL;

    while (current != NULL) 
    {
        next = current->next;     // Store next node
        current->next = prev;     // Reverse current node's pointer
        prev = current;           // Move pointers one position ahead
        current = next;
    }

    list->head = prev; // Update the head to the new front
}