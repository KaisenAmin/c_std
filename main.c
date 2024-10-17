#include "deque/deque.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Deque* deque1 = deque_create(sizeof(int));
    Deque* deque2 = deque_create(sizeof(int));
    int val = 5;

    // Adding elements
    for (int i = 0; i < 3; ++i) {
        int *newInt1 = malloc(sizeof(int));
        *newInt1 = i;
        deque_push_back(deque1, newInt1);

        int *newInt2 = malloc(sizeof(int));
        *newInt2 = val;
        deque_push_back(deque2, newInt2);
    }

    if (deque_is_less(deque1, deque2)) {
        fmt_printf("Deque1 is less than Deque2\n");
    }

    for (size_t i = 0; i < 3; ++i) {
        free(deque_at(deque1, i));
        free(deque_at(deque2, i));
    }

    deque_deallocate(deque1);
    deque_deallocate(deque2);
    return 0;
}