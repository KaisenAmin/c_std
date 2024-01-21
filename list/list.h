/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class List
*/

#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct Node Node;
typedef struct List List;
typedef bool (*ConditionFunction)(void* value);
typedef int (*CompareFunction)(const void *, const void *);

struct Node {
    void *value;
    Node *next;
    Node *prev;  
};

struct List {
    Node *head;
    Node *tail;
    size_t size;
    size_t itemSize;
    ConditionFunction condition;
    CompareFunction compare;
};

// Function declarations
List *list_create(size_t itemSize, CompareFunction compare);
size_t list_length(const List *list);

void *list_front(const List *list);
void *list_back(const List *list);
void *list_insert(List *list, size_t index, void *value);
void *list_erase(List *list, size_t index);

void list_resize(List *list, size_t newSize, void *defaultValue);
void list_swap(List *list1, List *list2);
void list_reverse(List *list);
void list_sort(List* list);
void list_push_front(List *list, void *value);
void list_push_back(List *list, void *value);
void list_pop_front(List *list);
void list_pop_back(List *list);
void list_clear(List *list);
void list_assign(List *list, void *values, size_t numValues);
void list_emplace_front(List *list, void *value);
void list_emplace_back(List *list, void *value);
void list_splice(List *dest, List *src, Node *pos);
void list_remove(List *list, void *value);
void list_remove_if(List *list, ConditionFunction cond);
void list_unique(List *list);
void list_merge(List *list1, List *list2);
void list_deallocate(List *list);

Node *list_begin(const List *list);
Node *list_end(const List *list);
Node *list_rbegin(const List *list);  // Only for doubly linked lists
Node *list_rend(const List *list); 

const Node *list_cbegin(const List *list);
const Node *list_cend(const List* list);
const Node *list_crbegin(const List* list);
const Node *list_crend(const List* list);

bool list_is_less(const List *list1, const List *list2);
bool list_is_greater(const List *list1, const List *list2);
bool list_is_equal(const List *list1, const List *list2);
bool list_is_less_or_equal(const List *list1, const List *list2);
bool list_is_greater_or_equal(const List *list1, const List *list2);
bool list_is_not_equal(const List *list1, const List *list2);
bool list_empty(const List *list);

#endif // LIST_H_
