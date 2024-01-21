#include "fmt/fmt.h"
#include "list/list.h"
#include "string/string.h"

int main() {
    List* stringList = list_create(sizeof(String*), NULL);
    String* str1 = string_create("Hello");
    String* str2 = string_create("World");
    String* str3 = string_create("Example");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Reverse each string
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) {
        String* str = *(String**)node->value;
        String* reversed = string_create(""); 

        for (int i = string_length(str) - 1; i >= 0; --i) {
            string_push_back(reversed, string_c_str(str)[i]);
        }
        fmt_printf("Reversed String: %s\n", string_c_str(reversed));
        string_deallocate(reversed);
    }

    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    list_deallocate(stringList);

    return 0;
}