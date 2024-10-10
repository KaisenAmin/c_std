#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello World");
    char* toUpper = string_to_upper(str); // convert str to upper case and return as char*
    char* toLower = string_to_lower(str); // convert str to lower case and return as char* 

    if (toUpper) {   
        fmt_printf("To Upper is -> %s\n", toUpper);
        free(toUpper);
    }
    if (toLower) {
        fmt_printf("To Lower is -> %s\n", toLower);
        free(toLower);
    }

    string_deallocate(str);
    return 0;
}