/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class List
*/

#include "list.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>


List *list_create(size_t itemSize, CompareFunction compare) {
    if (itemSize == 0) {
        fmt_fprintf(stderr, "Error: Item size must be greater than 0 in list_create.\n");
        exit(-1);
    }

    List *list = malloc(sizeof(List));
    if (!list) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for list in list_create.\n");
        exit(-1);
    }

    list->head = list->tail = NULL;
    list->size = 0;
    list->itemSize = itemSize;
    list->compare = compare; 

    return list;
}

void *list_front(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_front.\n");
        return NULL;
    }
    if (list->head == NULL) {
        fmt_fprintf(stderr, "Error: The list is empty in list_front.\n");
        return NULL;
    }
    return list->head->value;
}

void *list_back(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_back.\n");
        return NULL;
    }
    if (list->tail == NULL) {
        fmt_fprintf(stderr, "Error: The list is empty in list_back.\n");
        return NULL;
    }
    return list->tail->value;
}

void *list_insert(List *list, size_t index, void *value) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_insert.\n");
        return NULL;
    }
    if (index > list->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in list_insert.\n");
        return NULL;
    }
    if (index == 0) {
        list_push_front(list, value);
        return list->head->value;
    } 
    else if (index == list->size) {
        list_push_back(list, value);
        return list->tail->value;
    }

    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        return NULL;
    }

    newNode->value = malloc(list->itemSize);
    if (!newNode->value) {
        free(newNode);
        return NULL;
    }

    memcpy(newNode->value, value, list->itemSize);
    Node *current = list->head;

    for (size_t i = 0; i < index - 1; ++i) { 
        current = current->next;
    }

    newNode->next = current->next;
    newNode->prev = current;  // Set the prev pointer to the current node

    if (newNode->next != NULL) {
        newNode->next->prev = newNode;  // Update the prev pointer of the next node
    }
    current->next = newNode;
    list->size++;

    return newNode->value;
}

void *list_erase(List *list, size_t index) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_erase.\n");
        return NULL;
    }
    if (index >= list->size) {
        fmt_fprintf(stderr, "Error: Index out of bounds in list_erase.\n");
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
    return removedValue;
}

void list_resize(List *list, size_t newSize, void *defaultValue) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_resize.\n");
        return;
    }

    while (list->size > newSize) {
        list_pop_back(list);
    }

    while (list->size < newSize) {
        void *newValue = malloc(list->itemSize);
        if (!newValue) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in list_resize.\n");
            return;
        }

        if (defaultValue != NULL) {
            // Use the provided default value
            memcpy(newValue, defaultValue, list->itemSize);
        } 
        else {
            // If defaultValue is NULL, initialize the space to zero
            memset(newValue, 0, list->itemSize);
        }

        list_push_back(list, newValue);
        free(newValue); // Free the temporary allocation after pushing back
    }
}

void list_swap(List *list1, List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_swap.\n");
        return;
    }
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

void list_reverse(List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_reverse.\n");
        return;
    }
    if (list->head == NULL) {
        // No action needed for empty list, but you might want to log it
        return;
    }

    Node *current = list->head;
    Node *temp = NULL;

    while (current != NULL) {
        temp = current->prev; // Swap the next and prev pointers
        current->prev = current->next;
        current->next = temp;
        current = current->prev; // Move to the next node in the original list
    }

    temp = list->head; // Swap the head and tail of the list
    list->head = list->tail;
    list->tail = temp;
}

void list_sort(List* list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_sort.\n");
        return;
    }
    if (list->size < 2 || list->compare == NULL) {
        fmt_fprintf(stderr, "Error: Insufficient size or null compare function in list_sort.\n");
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
}

void list_push_front(List *list, void *value) {
    if (list == NULL || value == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list or value in list_push_front.\n");
        return;
    }

    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for newNode in list_push_front.\n");
        return;
    }
    newNode->value = malloc(list->itemSize);

    if (!newNode->value) {
        free(newNode);
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
}

void list_push_back(List *list, void *value) {
    if (list == NULL || value == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list or value in list_push_back.\n");
        return;
    }

    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for newNode in list_push_back.\n");
        return;
    }

    newNode->value = malloc(list->itemSize);
    if (!newNode->value) {
        free(newNode);
        return;  // Memory allocation failure
    }

    memcpy(newNode->value, value, list->itemSize);
    newNode->next = NULL;  // As this is the last node
    newNode->prev = list->tail;  // Set the prev pointer to the current tail

    if (list->tail != NULL) {
        list->tail->next = newNode;  // Update the next pointer of the current tail
    }
    list->tail = newNode;  // Update the tail of the list
    
    if (list->head == NULL) {
        list->head = newNode;  // If the list was empty, set the head to the new node
    }
    list->size++;
}

void list_pop_front(List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_pop_front.\n");
        return;
    }
    if (list->head == NULL) {
        fmt_fprintf(stderr, "Error: Trying to pop from an empty list in list_pop_front.\n");
        return;  // The list is empty, nothing to pop
    }

    Node *temp = list->head;
    list->head = list->head->next;  // Update the head to the next node

    if (list->head != NULL) { 
        list->head->prev = NULL;  // Set the prev pointer of the new head to NULL
    }
    else {
        list->tail = NULL;  // If the list becomes empty, set the tail to NULL
    }
    free(temp->value);  // Free the value and the node
    free(temp);

    list->size--;
}

void list_pop_back(List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_pop_back.\n");
        return;
    }
    if (list->head == NULL) {
        fmt_fprintf(stderr, "Error: Trying to pop from an empty list in list_pop_back.\n");
        return;  // The list is empty, nothing to pop
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
}

void list_clear(List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_clear.\n");
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
}

bool list_empty(const List *list) {
    return list->head == NULL;
}

size_t list_length(const List *list) {
    return list->size;
}

void list_deallocate(List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_deallocate.\n");
        return;
    }
    list_clear(list);
    free(list);
}

Node *list_begin(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_begin.\n");
        return NULL;
    }
    return list->head;
}

Node *list_end(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_end.\n");
        return NULL;
    }
    return NULL; // In a singly linked list, the end is represented by NULL
}

Node *list_rbegin(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_rbegin.\n");
        return NULL;
    }
    return list->tail;
}

Node *list_rend(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_rend.\n");
        return NULL;
    }
    return NULL;  // In a doubly linked list, the end is still represented by NULL
}

const Node *list_cbegin(const List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_cbegin.\n");
        return NULL;
    }
    return list->head;
}

const Node *list_cend(const List* list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_cend.\n");
        return NULL;
    }
    return NULL;
}

const Node *list_crbegin(const List* list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_crbegin.\n");
        return NULL;
    }
    return list->tail;
}

const Node *list_crend(const List* list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_crend.\n");
        return NULL;
    }
    return NULL;
}

void list_assign(List *list, void *values, size_t numValues) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_assign.\n");
        return;
    }
    if (values == NULL && numValues > 0) {
        fmt_fprintf(stderr, "Error: Null pointer provided for values in list_assign.\n");
        return;
    }

    list_clear(list); // Clear the existing content

    for (size_t i = 0; i < numValues; ++i) {
        void *currentValue = (char *)values + i * list->itemSize;
        list_push_back(list, currentValue);
    }
}

void list_emplace_front(List *list, void *value) {
    if (list == NULL || value == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list or value in list_emplace_front.\n");
        return;
    }
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for newNode in list_emplace_front.\n");
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
}

void list_emplace_back(List *list, void *value) {
    if (list == NULL || value == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list or value in list_emplace_back.\n");
        return;
    }
    Node *newNode = malloc(sizeof(Node));
    if (!newNode) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for newNode in list_emplace_back.\n");
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
}

void list_splice(List *dest, List *src, Node *pos) {
    if (dest == NULL || src == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_splice.\n");
        return;
    }
    if (dest == src) {
        fmt_fprintf(stderr, "Error: Cannot splice a list into itself in list_splice.\n");
        return;
    }
    if (src->head == NULL) {
        // The source list is empty; no action needed
        return;
    }
    Node *srcFirst = src->head;
    Node *srcLast = src->tail;

    // If 'pos' is NULL, insert at the end of 'dest'
    if (pos == NULL) {
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
}

void list_remove(List *list, void *value) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_remove.\n");
        return;
    }
    if (value == NULL) {
        fmt_fprintf(stderr, "Error: Null value provided for comparison in list_remove.\n");
        return;
    }
    if (list->compare == NULL) {
        fmt_fprintf(stderr, "Error: Null compare function provided in list_remove.\n");
        return;
    }

    Node *current = list->head;

    while (current != NULL) {
        Node *next = current->next;

        if (list->compare(current->value, value) == 0) {
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
        }
        current = next;
    }
}

void list_remove_if(List *list, ConditionFunction cond) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_remove_if.\n");
        return;
    }
    if (cond == NULL) {
        fmt_fprintf(stderr, "Error: Null condition function provided in list_remove_if.\n");
        return;
    }
    Node *current = list->head;

    while (current != NULL) {
        Node *next = current->next;

        if (cond(current->value)) {
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
        }
        current = next;
    }
}

void list_unique(List *list) {
    if (list == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for list in list_unique.\n");
        return;
    }
    if (list->compare == NULL) {
        fmt_fprintf(stderr, "Error: Null compare function provided in list_unique.\n");
        return;
    }
    if (list->size < 2) {
        // No action needed for lists with less than two elements
        return;
    }
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
        }
        else {
            current = current->next;
        }
    }
}

void list_merge(List *list1, List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_merge.\n");
        return;
    }
    if (list1 == list2 || list2->size == 0) {
        fmt_fprintf(stderr, "Error: No merge needed, lists are identical or second list is empty in list_merge.\n");
        return;
    }
    if (list1->size == 0) {
        list1->head = list2->head;
        list1->tail = list2->tail;
        list1->size = list2->size;
        list2->head = list2->tail = NULL;
        list2->size = 0;
        perror("List1 was empty, transferred all nodes from list2.\n");

        return;
    }
    Node *current1 = list1->head;
    Node *current2 = list2->head;

    while (current1 != NULL && current2 != NULL){

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
        } 
        else {
            current1 = current1->next;
        }
    }

    if (current2 != NULL) {
        // perror("Attaching remaining nodes from list2 to the end of list1...\n");
        list1->tail->next = current2;
        current2->prev = list1->tail;
        list1->tail = list2->tail;
        list1->size += list2->size;
    }
    list2->head = list2->tail = NULL;
    list2->size = 0;
}

bool list_is_less(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_is_less.\n");
        return false;
    }
    if (list1->size != list2->size) {
        return list1->size < list2->size;
    }
    Node *node1 = list1->head;
    Node *node2 = list2->head;

    while (node1 != NULL && node2 != NULL) {
        int val1 = *(int *)(node1->value);
        int val2 = *(int *)(node2->value);

        if (val1 != val2) {
            return val1 < val2;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    return false; // Lists are equal
}

bool list_is_greater(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_is_greater.\n");
        return false;
    }
    return list_is_less(list2, list1);
}

bool list_is_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_is_equal.\n");
        return false;
    }
    if (list1->size != list2->size) {
        return false;
    }
    Node *node1 = list1->head;
    Node *node2 = list2->head;

    while (node1 != NULL && node2 != NULL) {
        
        if (list1->compare(node1->value, node2->value) != 0) {
            return false;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    return true;
}

bool list_is_less_or_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_is_less_or_equal.\n");
        return false;
    }
    return list_is_less(list1, list2) || list_is_equal(list1, list2);
}

bool list_is_greater_or_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_is_greater_or_equal.\n");
        return false;
    }
    return list_is_greater(list1, list2) || list_is_equal(list1, list2);
}

bool list_is_not_equal(const List *list1, const List *list2) {
    if (list1 == NULL || list2 == NULL) {
        fmt_fprintf(stderr, "Error: Null pointer provided for one or both lists in list_is_not_equal.\n");
        return true;  // If one of the lists is null, they are not equal
    }
    return !list_is_equal(list1, list2);
}

