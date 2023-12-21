#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"


static int compare_strings(const void* a, const void* b) 
{
    String* strA = *(String**)a;
    String* strB = *(String**)b;

    return string_is_less(strA, strB) ? -1 : string_is_greater(strA, strB);
}

int main() 
{
    List* stringList = list_create(sizeof(String*), compare_strings);

    // Add strings to the list
    String* str1 = string_create("Apple");
    String* str2 = string_create("Banana");
    String* str3 = string_create("Cherry");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Sort the list of strings
    list_sort(stringList);

    // Iterate and print strings
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) 
    {
        String* str = *(String**)node->value;
        printf("%s\n", string_c_str(str));
    }

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);

    list_deallocate(stringList);

    return EXIT_SUCCESS;
}