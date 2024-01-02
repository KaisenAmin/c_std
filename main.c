


// #include "algorithm/algorithm.h"
// #include "vector/vector.h"
// #include <stdio.h>
// #include <stdlib.h>

// int compare_ints(const void* a, const void* b) {
//     int arg1 = *(const int*)a;
//     int arg2 = *(const int*)b;

//     if (arg1 < arg2) {
//          return -1;
//     }  
//     if (arg1 > arg2) {
//         return 1;
//     } 
//     return 0;
// }

// int main() {
//     Vector* intVector = vector_create(sizeof(int));
//     int values[] = {4, 3, 2, 10, 12, 1, 5, 6};

//     for (int i = 0; i < 8; i++) {
//         vector_push_back(intVector, &values[i]);
//     }

//     algorithm_sort(vector_data(intVector), vector_size(intVector), sizeof(int), compare_ints);

//     for (size_t i = 0; i < vector_size(intVector); i++) {
//         int* item = (int*)vector_at(intVector, i);
//         printf("%d ", *item);
//     }
//     printf("\n");

//     vector_deallocate(intVector);
//     return EXIT_SUCCESS;
// }


#include "algorithm/algorithm.h"
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    double value;
} MyStruct;

// Comparison function for MyStruct by id
int compare_structs(const void* a, const void* b) {
    MyStruct arg1 = *(const MyStruct*)a;
    MyStruct arg2 = *(const MyStruct*)b;
    if (arg1.id < arg2.id) return -1;
    if (arg1.id > arg2.id) return 1;
    return 0;
}

int main() {
    Vector* structVector = vector_create(sizeof(MyStruct));

    // Adding elements to the vector
    MyStruct items[] = {{2, 20.5}, {1, 10.5}, {3, 30.5}};
    for (int i = 0; i < 3; i++) {
        vector_push_back(structVector, &items[i]);
    }

    // Sorting the vector
    algorithm_sort(vector_data(structVector), vector_size(structVector), sizeof(MyStruct), compare_structs);

    // Printing sorted vector
    for (size_t i = 0; i < vector_size(structVector); i++) {
        MyStruct* item = (MyStruct*)vector_at(structVector, i);
        printf("ID: %d, Value: %.2f\n", item->id, item->value);
    }

    vector_deallocate(structVector);

    return EXIT_SUCCESS;
}
