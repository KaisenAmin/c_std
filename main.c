#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));

    // Adding elements
    for (int i = 0; i < 5; ++i) 
    {
        int* newInt = malloc(sizeof(int));
        *newInt = i;
        deque_push_back(myDeque, newInt);
    }

    // Constant reverse iteration
    const DequeIterator crit = *deque_crbegin(myDeque);
    const DequeIterator crend = *deque_crend(myDeque);

    printf("Constant reverse iteration: ");
    for (DequeIterator it = crit; !iterator_equals(&it, &crend); iterator_increment(&it)) 
        printf("%d ", *(const int*)iterator_get(&it));
    
    printf("\n");

    // Freeing allocated memory
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        free(deque_at(myDeque, i));

    deque_deallocate(myDeque);

    return EXIT_SUCCESS;
}