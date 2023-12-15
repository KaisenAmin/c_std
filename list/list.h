#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

typedef struct Node Node;
typedef struct List List;
typedef bool (*ConditionFunction)(void* value);
typedef int (*CompareFunction)(const void *, const void *);

struct Node 
{
    void *value;
    Node *next;
    Node *prev;  
};

struct List 
{
    Node *head;
    Node *tail;
    size_t size;
    size_t itemSize;
    ConditionFunction condition;
    CompareFunction compare;

    void *(*front)(const List *list);
    void *(*back)(const List *list);
    void (*push_front)(List *list, void *value);
    void (*push_back)(List *list, void *value);
    void (*pop_front)(List *list);
    void (*pop_back)(List *list);
    void (*clear)(List *list);
    void* (*insert)(List *list, size_t index, void *value);
    void* (*erase)(List *list, size_t index);
    void (*resize)(List *list, size_t newSize, void *defaultValue);
    void (*swap)(List *list1, List *list2);
    void (*reverse)(List *list);
    void (*sort)(List* list);
    bool (*empty)(const List *list);
    size_t (*length)(const List *list);
    void (*deallocate)(List *list);
    Node *(*begin)(const List *list);
    Node *(*end)(const List *list);
    Node *(*rbegin)(const List *list); 
    Node *(*rend)(const List *list); 
    const Node *(*cbegin)(const List *list);
    const Node *(*cend)(const List* list);
    const Node *(*crbegin)(const List* list);
    const Node *(*crend)(const List* list);
    void (*assign)(List *list, void *values, size_t numValues);
    void (*emplace_front)(List *list, void *value);
    void (*emplace_back)(List *list, void *value);
    void (*splice)(List *dest, List *src, Node *pos);
    void (*remove)(List *list, void *value);
    void (*remove_if)(List *list, ConditionFunction cond);
    void (*unique)(List *list);
    void (*merge)(List *list1, List *list2);
};

// Function declarations
List *list_create(size_t itemSize);


#endif // LIST_H_
