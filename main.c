#include "forward_list/forward_list.h"
#include "fmt/fmt.h"

int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) {
        fmt_printf("%d ", *(int*)(node->value));
    }
    fmt_printf("\n");
    
    forward_list_deallocate(list);
    return 0;
}