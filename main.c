#include "string/string.h"
#include "vector/vector.h"
#include <stdio.h>

int main() 
{
    Vector *vec = vector_create(sizeof(String*));
    String *fruits[5] = {
        string_create("Apple"),
        string_create("Banana"),
        string_create("Cherry"),
        string_create("Lemon"),
        string_create("Watermelon")
    };

    for (size_t index = 0; index < 5; index++)
        vector_push_back(vec, &fruits[index]);

    for (String **it = (String**)vector_begin(vec); it != (String**)vector_end(vec); it++) 
    {
        String *currentString = *it;
        if (currentString) 
            printf("%s\n", string_c_str(currentString));
    }
    
    for (size_t index = 0; index < 5; index++) 
        string_deallocate(fruits[index]);

    vector_deallocate(vec);
    return 0;
}