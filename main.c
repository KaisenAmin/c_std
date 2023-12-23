#include "csv/csv.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    String* str = string_create("Hello World");

    if (string_contains(str, "Hello"))
        printf("Yes 'Hello' exists in str\n");
    
    string_reverse(str); // reverse String 
    printf("Reverse String is %s\n", string_c_str(str));

    string_deallocate(str);
   
    return 0;
}
