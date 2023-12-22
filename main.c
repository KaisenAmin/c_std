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
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) 
        list_push_back(myList, &values[i]);

    list_deallocate(myList);

    list_deallocate(myList);
    return EXIT_SUCCESS;
}
