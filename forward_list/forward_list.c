/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class ForwardList
*/

#include "forward_list.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


ForwardList *forward_list_create(size_t itemSize) {
    ForwardList *list = malloc(sizeof(ForwardList));
    if (list == NULL) {
        return NULL;
    }
    // Initialize the list
    list->head = NULL;
    list->itemSize = itemSize;
    list->size = 0;

    return list;
}

// Helper function to split the list into two halves
static ForwardListNode *split_list_for_sort(ForwardListNode *head) {
    ForwardListNode *fast = head, *slow = head, *prev = NULL;

    while (fast != NULL && fast->next != NULL) {
        prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }
    if (prev != NULL) {
        prev->next = NULL;
    }

    return slow;
}

// Merge two sorted lists
static ForwardListNode *merge_sorted_lists(ForwardListNode *a, ForwardListNode *b, size_t itemSize) {
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

    return dummy.next;
}

// Recursive merge sort implementation
static ForwardListNode *merge_sort(ForwardListNode *head, size_t itemSize) {
    if (head == NULL || head->next == NULL) {
        return head;
    }

    ForwardListNode *middle = split_list_for_sort(head);
    ForwardListNode *left = merge_sort(head, itemSize);
    ForwardListNode *right = merge_sort(middle, itemSize);

    return merge_sorted_lists(left, right, itemSize);
}

void forward_list_push_front(ForwardList *list, void *value) {
    if (list == NULL || value == NULL) {
        return;
    }

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));

    if (newNode == NULL) { 
        return;
    }
    newNode->value = malloc(list->itemSize);

    if (newNode->value == NULL) {
        free(newNode);
        return;
    }
    memcpy(newNode->value, value, list->itemSize);

    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

void forward_list_pop_front(ForwardList *list) {
    if (list == NULL || list->head == NULL) {
        return;
    }

    ForwardListNode *temp = list->head;
    list->head = list->head->next;

    free(temp->value);
    free(temp);
    list->size--;
}

void *forward_list_front(const ForwardList *list) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }
    return list->head->value;
}

void forward_list_clear(ForwardList *list) {
    if (list == NULL) {
        return;
    }

    ForwardListNode *current = list->head;
    while (current != NULL) {
        ForwardListNode *next = current->next;

        free(current->value);
        free(current);
        current = next;
    }
    list->head = NULL;
    list->size = 0;
}

bool forward_list_empty(const ForwardList *list) {
    if (list == NULL) {
        return true;  // Consider a NULL list as empty
    }
    return list->head == NULL;
}

size_t forward_list_length(const ForwardList *list) {
    if (list == NULL) {
        return 0;
    }
    return list->size;
}

void forward_list_deallocate(ForwardList *list) {
    if (list == NULL) {
        return;
    }
    forward_list_clear(list);  // Clear all nodes
    free(list);
}

void forward_list_assign(ForwardList *list, void *values, size_t numValues) {
    if (list == NULL || values == NULL) { 
        return;
    }
    forward_list_clear(list);  // Clear existing contents
    for (size_t i = 0; i < numValues; ++i) {
        void *value = (char *)values + i * list->itemSize; // Calculate the address of the value to be copied
        forward_list_push_front(list, value);  // Add each new value to the front
    }
    forward_list_reverse(list); // Reverse the list to maintain the correct order
}

ForwardListNode *forward_list_before_begin(ForwardList *list) {
    assert(list != NULL);
    return NULL; // In a singly linked list, there is no node before the beginning
}

ForwardListNode *forward_list_begin(ForwardList *list) {
    if (list == NULL) {
        return NULL;
    }
    return list->head;
}

ForwardListNode *forward_list_end(ForwardList *list) {
    assert(list != NULL);
    return NULL; // In a singly linked list, the end is represented by NULL
}

size_t forward_list_max_size(const ForwardList *list) {
    assert(list != NULL);
    return (size_t)-1;
}

void forward_list_emplace_front(ForwardList *list, void *value) {
    if (list == NULL || value == NULL) {
        return;
    }
    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));

    if (newNode == NULL) { 
        return;
    }
    newNode->value = value;
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

void forward_list_emplace_after(ForwardList *list, ForwardListNode *pos, void *value) {
    if (list == NULL || value == NULL) {
        return;
    }
    if (pos == NULL) {
        // Special case: if pos is NULL, insert at the beginning
        forward_list_emplace_front(list, value);
        return;
    }

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));
    if (newNode == NULL) { 
        return;
    }
    newNode->value = value;
    newNode->next = pos->next;
    pos->next = newNode;
    list->size++;
}

void forward_list_insert_after(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues) {
    if (list == NULL || value == NULL) {
        return;
    }
    if (pos == NULL) {
        // Special case: insert at the beginning if pos is NULL
        for (size_t i = 0; i < numValues; ++i) {
            void *currentValue = (char *)value + i * list->itemSize;
            forward_list_push_front(list, currentValue);
        }
        return;
    }

    // Regular insertion after a given node
    for (size_t i = 0; i < numValues; ++i) {
        void *currentValue = (char *)value + i * list->itemSize;
        ForwardListNode *newNode = malloc(sizeof(ForwardListNode));
        if (newNode == NULL) {
            return;
        }
        newNode->value = malloc(list->itemSize);
        if (newNode->value == NULL) {
            free(newNode);
            return;
        }
        memcpy(newNode->value, currentValue, list->itemSize);

        newNode->next = pos->next;
        pos->next = newNode;
        pos = newNode;
        list->size++;
    }
}

void forward_list_erase_after(ForwardList *list, ForwardListNode *pos) {
    if (list == NULL || pos == NULL || pos->next == NULL) {
        return;
    }
    ForwardListNode *temp = pos->next;
    pos->next = temp->next;

    free(temp->value);
    free(temp);
    list->size--;
}

void forward_list_swap(ForwardList *list1, ForwardList *list2) {
    if (list1 == NULL || list2 == NULL) {
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
}

void forward_list_resize(ForwardList *list, size_t newSize) {
    if (list == NULL) {
        return;
    }
    while (list->size > newSize) { 
        forward_list_pop_front(list);
    }
    while (list->size < newSize) {
        // Allocate memory for a new value and initialize it to zero
        void *newValue = calloc(1, list->itemSize);  // Use calloc to zero-initialize
        if (newValue == NULL) {
            break; // In case of allocation failure, exit the loop
        }
        // Add the new value to the front of the list
        forward_list_push_front(list, newValue);
        free(newValue);
    }
}

void forward_list_splice_after(ForwardList *list, ForwardListNode *pos, ForwardList *other) {
    if (list == NULL || other == NULL) {
        return;
    }
    if (pos == NULL) {
        // Special case: if pos is NULL, splice at the beginning
        if (other->head != NULL) {
            ForwardListNode *otherCurrent = other->head;
            while (otherCurrent->next != NULL) {  // Find the last node of the other list
                otherCurrent = otherCurrent->next;
            }
            otherCurrent->next = list->head;
            list->head = other->head;
            list->size += other->size;
            other->head = NULL;
            other->size = 0;
        }
        return;
    }

    // Regular splicing after a given node
    if (other->head != NULL) {
        ForwardListNode *otherCurrent = other->head;
        while (otherCurrent->next != NULL) {  // Find the last node of the other list
            otherCurrent = otherCurrent->next;
        }
        otherCurrent->next = pos->next;
        pos->next = other->head;
        list->size += other->size;
        other->head = NULL;
        other->size = 0;
    }
}

void forward_list_remove(ForwardList *list, void *value) {
    if (list == NULL || value == NULL) {
        return;
    }
    while (list->head != NULL && memcmp(list->head->value, value, list->itemSize) == 0) { 
        forward_list_pop_front(list);
    }
    ForwardListNode *current = list->head;

    while (current != NULL && current->next != NULL) {
        if (memcmp(current->next->value, value, list->itemSize) == 0) {
            ForwardListNode *temp = current->next;
            current->next = temp->next;
            free(temp->value);
            free(temp);
            list->size--;
        } 
        else {
            current = current->next;
        }
    }
}

void forward_list_remove_if(ForwardList *list, bool (*condition)(void*)) {
    if (list == NULL || condition == NULL) {
        return;
    }
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
        } 
        else {
            current = current->next;
        }
    }
}

void forward_list_unique(ForwardList *list) {
    if (list == NULL || list->head == NULL || list->head->next == NULL) { 
        return;
    }

    ForwardListNode *current = list->head;
    while (current->next != NULL) {
        if (memcmp(current->value, current->next->value, list->itemSize) == 0) {
            ForwardListNode *duplicate = current->next;
            current->next = duplicate->next;
            free(duplicate->value);
            free(duplicate);
            list->size--;
        } 
        else {
            current = current->next;
        }
    }
}

void forward_list_merge(ForwardList *list1, ForwardList *list2) {
    if (list1 == NULL || list2 == NULL || list2->head == NULL) {
        return;
    }
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
}

void forward_list_sort(ForwardList *list) {
    if (list == NULL || list->head == NULL) { 
        return;
    }
    list->head = merge_sort(list->head, list->itemSize);
}

void forward_list_reverse(ForwardList *list) {
    if (list == NULL || list->head == NULL || list->head->next == NULL) { 
        return;
    }

    ForwardListNode *prev = NULL, *current = list->head, *next = NULL;
    while (current != NULL) {
        next = current->next;     // Store next node
        current->next = prev;     // Reverse current node's pointer
        prev = current;           // Move pointers one position ahead
        current = next;
    }
    list->head = prev; // Update the head to the new front
}

// Helper function for comparing node values
static int compare_node_values(const void *a, const void *b, size_t size) {
    return memcmp(a, b, size);
}

bool forward_list_is_less(const ForwardList *list1, const ForwardList *list2) {
    ForwardListNode *node1 = list1->head, *node2 = list2->head;

    while (node1 && node2) {
        if (compare_node_values(node1->value, node2->value, list1->itemSize) >= 0) {
            return false;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    return node1 == NULL && node2 != NULL;
}

bool forward_list_is_greater(const ForwardList *list1, const ForwardList *list2) {
    return forward_list_is_less(list2, list1); // Just invert list1 and list2 for is_greater
}

bool forward_list_is_equal(const ForwardList *list1, const ForwardList *list2) {
    ForwardListNode *node1 = list1->head, *node2 = list2->head;

    while (node1 && node2) {
        if (compare_node_values(node1->value, node2->value, list1->itemSize) != 0) { 
            return false;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    return node1 == NULL && node2 == NULL;
}

bool forward_list_is_less_or_equal(const ForwardList *list1, const ForwardList *list2) {
    return forward_list_is_less(list1, list2) || forward_list_is_equal(list1, list2);
}

bool forward_list_is_greater_or_equal(const ForwardList *list1, const ForwardList *list2) {
    return forward_list_is_greater(list1, list2) || forward_list_is_equal(list1, list2);
}

bool forward_list_is_not_equal(const ForwardList *list1, const ForwardList *list2) {
    return !forward_list_is_equal(list1, list2);
}
