#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* myDeque = deque_create(sizeof(int));
    int insertVal = 99;

    for (int i = 0; i < 5; ++i) {
        int* ptr = malloc(sizeof(int)); 
        if (ptr == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory\n");
            free(ptr);
            break; 
        }

        *ptr = i; 
        deque_push_back(myDeque, ptr); 
    }

    deque_insert(myDeque, 2, &insertVal);
    // deque_erase(myDeque, 1);

    fmt_printf("Deque after insert and erase:\n");
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        fmt_printf("%d ", *(int*)deque_at(myDeque, i));
    }
    fmt_printf("\n");

    deque_deallocate(myDeque);
    return 0;
}