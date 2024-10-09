#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str = string_create("amin 15adf");
    String* new_one = string_create("Hello1994");

    fmt_printf("'%s' is alnum %d\n", str->dataStr, string_is_alnum(str));
    fmt_printf("'%s' is alnum %d\n", new_one->dataStr, string_is_alnum(new_one));
    
    string_deallocate(new_one);
    string_deallocate(str);

    return 0;
}