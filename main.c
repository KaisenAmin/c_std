#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_front(myList, &value);
    list_pop_front(myList);

    list_deallocate(myList);
    return 0;
}