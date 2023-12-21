#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"
#include "array/array.h"

int main() 
{
    Array* intArray = array_create(sizeof(int), 5);

    if (intArray == NULL) 
    {
        printf("Failed to create array.\n");
        return 1;
    }

    int valueToFill = 42;
    array_fill(intArray, &valueToFill);

    for (int i = 0; i < array_size(intArray); i++)
        printf("Number is %d\n", *(int*)array_at(intArray, i));

    printf("-----------\n");

    for (size_t i = 0; i < array_size(intArray); ++i) 
    {
        int* element = (int*)array_at(intArray, i);

        if (element != NULL) 
            printf("Element %zu: %d\n", i, *element);
    }

    array_deallocate(intArray);


    return EXIT_SUCCESS;
}
