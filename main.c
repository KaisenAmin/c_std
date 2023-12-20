#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() 
{
    Vector *stringVector = vector_create(sizeof(char*));
    char* values[] = {"Hello", "World", "Vector", "Example"};

    for (int i = 0; i < 4; ++i) 
        vector_push_back(stringVector, &values[i]);
        
    char* newValue = "NewString";
    vector_assign(stringVector, 1, &newValue);

    char* emplaceValue = "EmplacedString";
    vector_emplace(stringVector, 2, &emplaceValue, sizeof(char*));

    char* emplaceBackValue = "EmplacedBackString";
    vector_emplace_back(stringVector, &emplaceBackValue, sizeof(char*));

    for (size_t i = 0; i < vector_size(stringVector); ++i) 
    {
        char** item = (char **)vector_at(stringVector, i);
        printf("%s\n", *item);
    }

    vector_deallocate(stringVector);

    return EXIT_SUCCESS;
}