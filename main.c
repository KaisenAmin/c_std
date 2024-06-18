#include "stack/stack.h"
#include "string/string.h"
#include "fmt/fmt.h"

bool isBalanced(String* input) {
    Stack* stack = stack_create(sizeof(char));
    size_t length = string_length(input);

    for (size_t i = 0; i < length; i++) {
        char currentChar = string_at(input, i);

        if (currentChar == '(' || currentChar == '[' || currentChar == '{') {
            stack_push(stack, &currentChar);
        }
        else if (currentChar == ')' || currentChar == ']' || currentChar == '}') {
            if (stack_empty(stack)) {
                stack_deallocate(stack);
                return false;
            }

            char topChar = *(char*)stack_top(stack);
            stack_pop(stack);

            if ((currentChar == ')' && topChar != '(') ||
                (currentChar == ']' && topChar != '[') ||
                (currentChar == '}' && topChar != '{')) {
                stack_deallocate(stack);
                return false;
            }
        }
    }

    bool balanced = stack_empty(stack);
    stack_deallocate(stack);

    return balanced;
}

int main() {
    String* str = string_create("{[()]}");

    if (isBalanced(str)) {
        fmt_printf("The string %s is balanced.\n", str->dataStr);
    }
    else {
        fmt_printf("The string %s is not balanced.\n", str->dataStr);
    }

    string_deallocate(str);
    return 0;
}