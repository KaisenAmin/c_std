#include "string/string.h"
#include "vector/vector.h"
#include <stdio.h>
#include <time.h>

int main() 
{
    struct timespec start, end;
    double time_elapsed;

    Vector *vec = vector_create(sizeof(String*));
    String *fruits[5] = {
        string_create("Apple"),
        string_create("Banana"),
        string_create("Cherry"),
        string_create("Lemon"),
        string_create("Watermelon")
    };
    String *concat = string_create("");

    for (size_t index = 0; index < 5; index++)
        vector_push_back(vec, &fruits[index]);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t index = 0; index < vector_size(vec); ++index) 
    {
        String **strPtr = (String**)vector_at(vec, index);
        string_push_back(*strPtr, '\n');
        string_concatenate(concat, *strPtr);
    }
    
    for (size_t index = 0; index < 5; index++) 
        string_deallocate(fruits[index]);

    vector_deallocate(vec);

    clock_gettime(CLOCK_MONOTONIC, &end);
    time_elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("%s", string_c_str(concat));
    printf("Time taken: %f seconds\n", time_elapsed);

    string_deallocate(concat);
    return 0;
}
