#include "stack/stack.h"
#include "fmt/fmt.h"

int main() {
    Stack* stack = stack_create(sizeof(int));
    int arr[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        stack_push(stack, &arr[i]);
    }
    fmt_printf("Size of stack is %zu\n", stack_size(stack));

    if (!stack_empty(stack)) {
        int topValue = *(int*)stack_top(stack);
        fmt_printf("Top Element is %d\n", topValue);

        int pop = *(int*)stack_pop(stack);
        fmt_printf("Pop value is %d\n", pop);
        fmt_printf("After Pop size is %zu\n", stack_size(stack));
    }

    stack_deallocate(stack);
    return 0;
}