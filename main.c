#include "string/string.h"
#include <stdio.h>

int main() 
{
    String* str = string_create("Hello World");

    string_swap_case(str);
    printf("Swapped Case: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}