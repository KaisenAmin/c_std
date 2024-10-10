#include "stack/stack.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* myString = string_create("");
    Stack* stack = stack_create(sizeof(char*));

    char* value1 = "Amin";
    string_append(myString, value1);
    stack_push(stack, &myString);

    char* value2 = "Tahmasebi";
    string_append(myString, value2);
    stack_push(stack, &myString);

    char* value3 = "C Programming";
    string_append(myString, value3);
    stack_push(stack, &myString);

    fmt_printf("Size of Stack is %zu\n", stack_size(stack));

    String** str1 = (String**)stack_pop(stack);
    fmt_printf("%s", string_c_str(*str1));

    string_deallocate(*str1);
    stack_deallocate(stack);
    
    return 0;
}