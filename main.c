#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{   
    Vector* intVector = vector_create(sizeof(int));
    int value = 10;
    vector_push_back(intVector, &value);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(intVector); ++i) 
    {
        int* item = (int*)vector_at(intVector, i);
        printf("%d\n", *item);
    }

    // Cleanup
    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}
