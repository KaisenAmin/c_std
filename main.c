#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    String* str = string_create("   Hello World   ");

    string_trim_start(str);
    printf("Trimmed Start: '%s'\n", string_c_str(str));

    string_trim_end(str);
    printf("Trimmed End: '%s'\n", string_c_str(str));

    string_clear(str);
    string_assign(str, "   Hello World   ");
    string_trim(str);
    
    printf("Trimmed Both: '%s'\n", string_c_str(str));

    string_deallocate(str);
    return 0;
}