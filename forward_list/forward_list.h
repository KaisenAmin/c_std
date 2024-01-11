/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class ForwardList
*/

#ifndef FORWARD_LIST_H_
#define FORWARD_LIST_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct ForwardListNode ForwardListNode;
typedef struct ForwardList ForwardList;

struct ForwardListNode {
    void *value;
    ForwardListNode *next;
};

struct ForwardList {
    ForwardListNode *head;
    size_t itemSize;
    size_t size;
};

ForwardList* forward_list_create(size_t itemSize);
void* forward_list_front(const ForwardList *list);

void forward_list_push_front(ForwardList *list, void *value);
void forward_list_pop_front(ForwardList *list);
void forward_list_clear(ForwardList *list);
void forward_list_deallocate(ForwardList *list);
void forward_list_assign(ForwardList *list, void *values, size_t numValues);
void forward_list_emplace_front(ForwardList *list, void *value);
void forward_list_emplace_after(ForwardList *list, ForwardListNode *pos, void *value);
void forward_list_insert_after(ForwardList *list, ForwardListNode *pos, void *value, size_t numValues);
void forward_list_erase_after(ForwardList *list, ForwardListNode *pos);
void forward_list_swap(ForwardList *list1, ForwardList *list2);
void forward_list_resize(ForwardList *list, size_t newSize);
void forward_list_splice_after(ForwardList *list, ForwardListNode *pos, ForwardList *other);
void forward_list_remove(ForwardList *list, void *value);
void forward_list_remove_if(ForwardList *list, bool (*condition)(void*));
void forward_list_unique(ForwardList *list);
void forward_list_merge(ForwardList *list1, ForwardList *list2);
void forward_list_sort(ForwardList *list);
void forward_list_reverse(ForwardList *list);

size_t forward_list_length(const ForwardList *list);
size_t forward_list_max_size(const ForwardList *list);

ForwardListNode* forward_list_before_begin(ForwardList *list);
ForwardListNode* forward_list_begin(ForwardList *list);
ForwardListNode* forward_list_end(ForwardList *list);

bool forward_list_is_less(const ForwardList *list1, const ForwardList *list2);
bool forward_list_is_greater(const ForwardList *list1, const ForwardList *list2);
bool forward_list_is_equal(const ForwardList *list1, const ForwardList *list2);
bool forward_list_is_less_or_equal(const ForwardList *list1, const ForwardList *list2);
bool forward_list_is_greater_or_equal(const ForwardList *list1, const ForwardList *list2);
bool forward_list_is_not_equal(const ForwardList *list1, const ForwardList *list2);
bool forward_list_empty(const ForwardList *list);

#endif 