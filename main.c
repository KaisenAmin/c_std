#include "fmt/fmt.h"
#include "list/list.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    List *list1 = list_create(sizeof(int), compare_ints);
    List *list2 = list_create(sizeof(int), compare_ints);
    int values[] = {50, 40, 30, 20, 10};
    int values2[] = {100, 200, 300, 400, 500};

    for (int i = 0; i < 5; ++i) { 
        list_push_back(list1, &values[i]);
    }
    for (int i = 0; i < 5; ++i) { 
        list_push_back(list2, &values2[i]);
    }
    list_swap(list1, list2); // Swap list1 and list2

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next){
        int* value = (int*)node->value;
        fmt_printf("%d\n", *value);
    }

    list_deallocate(list1);
    list_deallocate(list2);

    return 0;
}