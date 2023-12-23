#include "csv/csv.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    String* str = string_create("Hello World");
    char* toUpper = string_to_upper(str);
    char* toLower = string_to_lower(str);

    if (toUpper)
    {   
        printf("To Upper is -> %s\n", toUpper);
        free(toUpper);
    }
    if (toLower)
    {
        printf("To Lower is -> %s\n", toLower);
        free(toLower);
    }
    string_deallocate(str);
   
    return 0;
}
