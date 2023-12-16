#ifndef FORWARD_LIST_H_
#define FORWARD_LIST_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct ForwardListNode ForwardListNode;
typedef struct ForwardList ForwardList;

struct ForwardListNode 
{
    void *value;
    ForwardListNode *next;
};

struct ForwardList 
{
    ForwardListNode *head;
    size_t itemSize;
    size_t size;

    void (*push_front)(ForwardList *list, void *value);
    void (*pop_front)(ForwardList *list);
    void *(*front)(const ForwardList *list);
    void (*clear)(ForwardList *list);
    bool (*empty)(const ForwardList *list);
    size_t (*length)(const ForwardList *list);
    void (*deallocate)(ForwardList *list);
    void (*assign)(ForwardList *list, void *values, size_t numValues);
    ForwardListNode *(*before_begin)(ForwardList *list);
    ForwardListNode *(*begin)(ForwardList *list);
    ForwardListNode *(*end)(ForwardList *list);
    size_t (*max_size)(const ForwardList *list);
    void (*emplace_front)(ForwardList *list, void *value);
    void (*emplace_after)(ForwardList *list, ForwardListNode *pos, void *value);
    void (*insert_after)(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues);
    void (*erase_after)(ForwardList *list, ForwardListNode *pos);
    void (*swap)(ForwardList *list1, ForwardList *list2);
    void (*resize)(ForwardList *list, size_t newSize);
    void (*splice_after)(ForwardList *list, ForwardListNode *pos, ForwardList *other);
    void (*remove)(ForwardList *list, void *value);
    void (*remove_if)(ForwardList *list, bool (*condition)(void*));
    void (*unique)(ForwardList *list);
    void (*merge)(ForwardList *list1, ForwardList *list2);
    void (*sort)(ForwardList *list);
    void (*reverse)(ForwardList *list);

    bool (*is_less)(const ForwardList *list1, const ForwardList *list2);
    bool (*is_greater)(const ForwardList *list1, const ForwardList *list2);
    bool (*is_equal)(const ForwardList *list1, const ForwardList *list2);
    bool (*is_less_or_equal)(const ForwardList *list1, const ForwardList *list2);
    bool (*is_greater_or_equal)(const ForwardList *list1, const ForwardList *list2);
    bool (*is_not_equal)(const ForwardList *list1, const ForwardList *list2);

};

ForwardList* forward_list_create(size_t itemSize);

#endif 