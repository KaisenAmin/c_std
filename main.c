#include "forward_list/forward_list.h"
#include <stdio.h>

int main() {
    ForwardList *list1 = forward_list_create(sizeof(int));
    ForwardList *list2 = forward_list_create(sizeof(int));
    
    int values1[] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < sizeof(values1) / sizeof(values1[0]); i++) {
        forward_list_push_front(list1, &values1[i]);
    }
    
    int values2[] = {1, 2, 3, 4, 5, 6}; 
    for (size_t i = 0; i < sizeof(values2) / sizeof(values2[0]); i++) {
        forward_list_push_front(list2, &values2[i]);
    }
    
    if (forward_list_is_less(list1, list2)) { 
        printf("List1 is less than List2\n");
    }
    else if (forward_list_is_greater(list1, list2)) {
        printf("List1 is greater than List2\n");
    }
    else if (forward_list_is_equal(list1, list2)) {
        printf("List1 is equal to List2\n");
    }
    if (forward_list_is_not_equal(list1, list2)) { 
        printf("List1 is not equal to List2\n");
    }

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);

    return 0;
}
