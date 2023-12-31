#include "span/span.h"
#include <stdio.h>

int main() 
{
    int array[] = {10, 20, 30, 40, 50};
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    Span *span = span_create(array, arraySize, sizeof(int));

    Span firstThree = span_first(span, 3);
    printf("First three elements: ");
    for (size_t i = 0; i < firstThree.size / sizeof(int); ++i) 
        printf("%d ", ((int*)firstThree.data)[i]);

    printf("\n");
    span_destroy(span);

    return 0;
}
