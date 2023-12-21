#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *list1 = list_create(sizeof(int), compare_ints);

    int values[] = {50, 40, 30, 20, 10};
    for (int i = 0; i < 5; ++i) 
        list_push_back(list1, &values[i]);

    list_resize(list1, 10, 0);

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next)
    {
        int* value = (int*)node->value;
        printf("%d\n", *value);
    }


    list_deallocate(list1);

    return EXIT_SUCCESS;
}