#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using crbegin and crend for reverse read-only iteration over the vector
    for (const int *it = (const int *)vector_crbegin(intVector); it != (const int *)vector_crend(intVector); --it) {
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
    return 0;
}