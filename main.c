#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello,World-This.Is;A-Test");
    int count = 0;
    String** tokens = string_tokenize(str, ",.-;", &count);

    for (int i = 0; i < count; i++) {
        fmt_printf("Token %d: %s\n", i, tokens[i]->dataStr);
        string_deallocate(tokens[i]);
    }
    free(tokens);

    string_deallocate(str);
    return 0;
}