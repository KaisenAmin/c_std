#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello World");

    if (string_contains(str, "Hello")) { 
        fmt_printf("Yes 'Hello' exists in str\n");
    }
    
    string_reverse(str); // reverse String 
    fmt_printf("Reverse String is %s\n", string_c_str(str));

    string_deallocate(str);
    return 0;
}