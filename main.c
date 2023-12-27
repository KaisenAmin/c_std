#include "string/string.h"
#include <stdio.h>

int main() 
{
    float value = 123.456;
    String* str = string_from_float(value);

    printf("Float %.3f converted to string: %s\n", value, str->dataStr);

    string_deallocate(str);
    return 0;
}
