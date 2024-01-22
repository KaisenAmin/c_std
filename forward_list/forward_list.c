/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class ForwardList
*/

#include "forward_list.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

ForwardList *forward_list_create(size_t itemSize) {
    if (itemSize == 0) {
        fmt_fprintf(stderr, "Error: Item size cannot be zero in forward_list_create.\n");
        exit(-1);
    }

    ForwardList *list = malloc(sizeof(ForwardList));
    if (!list) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for ForwardList in forward_list_create.\n");
        exit(-1);
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
    if (!a || !b) {
        fmt_fprintf(stderr, "Error: NULL list node(s) in merge_sorted_lists.\n");
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

    return dummy.next;
}

// Recursive merge sort implementation
static ForwardListNode *merge_sort(ForwardListNode *head, size_t itemSize) {
    if (!head) {
        fmt_fprintf(stderr, "Error: NULL head in merge_sort.\n");
        return NULL;
    }

    ForwardListNode *middle = split_list_for_sort(head);
    ForwardListNode *left = merge_sort(head, itemSize);
    ForwardListNode *right = merge_sort(middle, itemSize);

    return merge_sorted_lists(left, right, itemSize);
}

void forward_list_push_front(ForwardList *list, void *value) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_push_front.\n");
        return;
    }
    if (!value) {
        fmt_fprintf(stderr, "Error: NULL value pointer in forward_list_push_front.\n");
        return;
    }

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));
    if (!newNode) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for new node in forward_list_push_front.\n");
        return;
    }

    newNode->value = malloc(list->itemSize);
    if (!newNode->value) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for node value in forward_list_push_front.\n");
        free(newNode);
        return;
    }
    memcpy(newNode->value, value, list->itemSize);

    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

void forward_list_pop_front(ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_pop_front.\n");
        return;
    }
    if (!list->head) {
        fmt_fprintf(stderr, "Warning: Attempting to pop from an empty list in forward_list_pop_front.\n");
        return;
    }

    ForwardListNode *temp = list->head;
    list->head = list->head->next;

    free(temp->value);
    free(temp);
    list->size--;
}

void *forward_list_front(const ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_front.\n");
        return NULL;
    }
    if (list->head == NULL) {
        fmt_fprintf(stderr, "Warning: Attempting to access the front of an empty ForwardList in forward_list_front.\n");
        return NULL;
    }
    return list->head->value;
}

void forward_list_clear(ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_clear.\n");
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_empty.\n");
        return true;  // Consider a NULL list as empty
    }
    return list->head == NULL;
}

size_t forward_list_length(const ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_length.\n");
        return 0;
    }
    return list->size;
}

void forward_list_deallocate(ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_deallocate.\n");
        return;
    }
    forward_list_clear(list);  // Clear all nodes
    free(list);
}

void forward_list_assign(ForwardList *list, void *values, size_t numValues) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_assign.\n");
        return;
    }
    if (!values) {
        fmt_fprintf(stderr, "Error: NULL values pointer in forward_list_assign.\n");
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
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in forward_list_before_begin.\n");
        return NULL;
    }
    return NULL; // In a singly linked list, there is no node before the beginning
}

ForwardListNode *forward_list_begin(ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_begin.\n");
        return NULL;
    }
    return list->head;
}

ForwardListNode *forward_list_end(ForwardList *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in forward_list_end.\n");
        return NULL;
    }
    return NULL; // In a singly linked list, the end is represented by NULL
}

size_t forward_list_max_size(const ForwardList *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in forward_list_max_size.\n");
        return (size_t)-1; // Or a specific error code or size_t max value
    }
    return (size_t)-1; // You might want to replace this with a more meaningful value
}


void forward_list_emplace_front(ForwardList *list, void *value) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_emplace_front.\n");
        return;
    }
    if (!value) {
        fmt_fprintf(stderr, "Error: NULL value pointer in forward_list_emplace_front.\n");
        return;
    }

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));
    if (newNode == NULL) { 
        fmt_fprintf(stderr, "Error: Can not allocate memory for newNode in forward_list_emplace_front.\n");
        return;
    }

    newNode->value = value;
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

void forward_list_emplace_after(ForwardList *list, ForwardListNode *pos, void *value) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_emplace_after.\n");
        return;
    }
    if (!value) {
        fmt_fprintf(stderr, "Error: NULL value pointer in forward_list_emplace_after.\n");
        return;
    }
    if (pos == NULL) {
        // Special case: if pos is NULL, insert at the beginning
        forward_list_emplace_front(list, value);
        return;
    }

    ForwardListNode *newNode = malloc(sizeof(ForwardListNode));
    if (newNode == NULL) { 
        fmt_fprintf(stderr, "Error: Can not allocate memory for newNode in forward_list_emplace_after.\n");
        return;
    }
    newNode->value = value;
    newNode->next = pos->next;
    pos->next = newNode;
    list->size++;
}

void forward_list_insert_after(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_insert_after.\n");
        return;
    }
    if (!value) {
        fmt_fprintf(stderr, "Error: NULL value pointer in forward_list_insert_after.\n");
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_erase_after.\n");
        return;
    }
    if (!pos) {
        fmt_fprintf(stderr, "Error: NULL position node in forward_list_erase_after.\n");
        return;
    }
    if (!pos->next) {
        fmt_fprintf(stderr, "Warning: Attempting to erase after the end of the list in forward_list_erase_after.\n");
        return;
    }
    ForwardListNode *temp = pos->next;
    pos->next = temp->next;

    free(temp->value);
    free(temp);
    list->size--;
}

void forward_list_swap(ForwardList *list1, ForwardList *list2) {
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL list pointer(s) in forward_list_swap.\n");
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_resize.\n");
        return;
    }
    while (list->size > newSize) { 
        forward_list_pop_front(list);
    }
    while (list->size < newSize) {
        // Allocate memory for a new value and initialize it to zero
        void *newValue = calloc(1, list->itemSize);  // Use calloc to zero-initialize
        if (!newValue) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in forward_list_resize.\n");
            break;
        }
        // Add the new value to the front of the list
        forward_list_push_front(list, newValue);
        free(newValue);
    }
}

void forward_list_splice_after(ForwardList *list, ForwardListNode *pos, ForwardList *other) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_splice_after.\n");
        return;
    }
    if (!other) {
        fmt_fprintf(stderr, "Error: NULL other list pointer in forward_list_splice_after.\n");
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_remove.\n");
        return;
    }
    if (!value) {
        fmt_fprintf(stderr, "Error: NULL value pointer in forward_list_remove.\n");
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_remove_if.\n");
        return;
    }
    if (!condition) {
        fmt_fprintf(stderr, "Error: NULL condition function pointer in forward_list_remove_if.\n");
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL list pointer in forward_list_unique.\n");
        return;
    }
    if (list->head == NULL || list->head->next == NULL) {
        // List is empty or has a single element, no action required
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
    if (!list1) {
        fmt_fprintf(stderr, "Error: NULL list1 pointer in forward_list_merge.\n");
        return;
    }
    if (!list2) {
        fmt_fprintf(stderr, "Error: NULL list2 pointer in forward_list_merge.\n");
        return;
    }
    if (list2->head == NULL) {
        return;  // list2 is empty, nothing to merge
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
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer in forward_list_sort.\n");
        return;
    }
    if (list->head == NULL) {
        return;  // List is empty, nothing to sort
    }
    list->head = merge_sort(list->head, list->itemSize);
}

void forward_list_reverse(ForwardList *list) {
    if (!list) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer in forward_list_reverse.\n");
        return;
    }
    if (list->head == NULL || list->head->next == NULL) {
        return;  // List is empty or has only one element, no need to reverse
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
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer(s) in forward_list_is_less.\n");
        return false;
    }
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
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer(s) in forward_list_is_greater.\n");
        return false;
    }
    return forward_list_is_less(list2, list1); // Just invert list1 and list2 for is_greater
}

bool forward_list_is_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer(s) in forward_list_is_equal.\n");
        return false;
    }
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
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer(s) in forward_list_is_less_or_equal.\n");
        return false;
    }
    return forward_list_is_less(list1, list2) || forward_list_is_equal(list1, list2);
}

bool forward_list_is_greater_or_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer(s) in forward_list_is_greater_or_equal.\n");
        return false;
    }
    return forward_list_is_greater(list1, list2) || forward_list_is_equal(list1, list2);
}

bool forward_list_is_not_equal(const ForwardList *list1, const ForwardList *list2) {
    if (!list1 || !list2) {
        fmt_fprintf(stderr, "Error: NULL ForwardList pointer(s) in forward_list_is_not_equal.\n");
        return true;  // If one is NULL and the other is not, they are not equal.
    }
    return !forward_list_is_equal(list1, list2);
}
