#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"


int main() 
{
    List* list1 = list_create(sizeof(String*), NULL);
    List* list2 = list_create(sizeof(String*), NULL);

    String* str1 = string_create("Hello");
    String* str2 = string_create("World");
    String* str3 = string_create("Example");
    String* str4 = string_create("Text");

    list_push_back(list1, &str1);
    list_push_back(list1, &str2);
    list_push_back(list2, &str3);
    list_push_back(list2, &str4);

    // Merge list2 into list1
    list_merge(list1, list2);

    // Iterate and print strings from merged list
    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next) 
    {
        String* str = *(String**)node->value;
        printf("%s\n", string_c_str(str));
    }

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    string_deallocate(str4);

    list_deallocate(list1);
    list_deallocate(list2);

    return EXIT_SUCCESS;
}