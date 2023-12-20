#include "string/string.h"
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Vector* vec = vector_create(sizeof(String*));

    String* myString1 = string_create("Hello");
    String* myString2 = string_create("World");
    String* myString3 = string_create("Example");
    String** strPtr = NULL;

    vector_push_back(vec, &myString1);
    vector_push_back(vec, &myString2);
    vector_push_back(vec, &myString3);

    for (size_t i = 0; i < vector_size(vec); ++i) 
    {
        strPtr = (String**) vector_at(vec, i);
        if (strPtr && *strPtr) 
            printf("%s\n", (*strPtr)->dataStr); 
    }

    string_deallocate(myString1);
    string_deallocate(myString2);
    string_deallocate(myString3);

    vector_deallocate(vec);

    // free(strPtr);

    return 0;
}