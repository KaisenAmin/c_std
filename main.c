#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* a = string_create("HELLO, AND WELCOME TO MY WORLD");
    fmt_printf("%s is title: %d\n", string_c_str(a), string_is_title(a)); 

    String* b = string_create("Hello World");
    fmt_printf("%s is title: %d\n", string_c_str(b), string_is_title(b)); 

    String* c = string_create("22 Names");
    fmt_printf("%s is title: %d\n", string_c_str(c), string_is_title(c)); 

    String* d = string_create("This Is %'!?");
    fmt_printf("%s is title: %d\n", string_c_str(d), string_is_title(d)); 

    String* e = string_create("Hello world");
    fmt_printf("%s is title: %d\n", string_c_str(e), string_is_title(e)); 

    string_deallocate(a);
    string_deallocate(b);
    string_deallocate(c);
    string_deallocate(d);
    string_deallocate(e);

    return 0;
}