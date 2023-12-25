#include "string/string.h"
#include <stdio.h>

int main() 
{
    String *hexStr = string_create("48656c6c6f"); // Hexadecimal representation of "Hello"
    String *convertedStr = hex_to_string(hexStr);

    printf("Hexadecimal String: %s\n", hexStr->dataStr);
    printf("Converted String: %s\n", convertedStr->dataStr);

    string_deallocate(hexStr);
    string_deallocate(convertedStr);
    return 0;
}



