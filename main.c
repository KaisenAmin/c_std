#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    int age;
} Person;

void print_people_array(Person *people, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf("Name: %s, Age: %d\n", people[i].name, people[i].age);
    }
}

int main() {
    Person people[] = {
        {"Alice", 30},
        {"Bob", 25},
        {"Carol", 27},
        {"Dave", 32},
        {"Eve", 29}
    };
    size_t people_size = sizeof(people) / sizeof(people[0]);
    Person destination[5];

    memset(destination, 0, sizeof(destination));  // Initialize to zero
    // Copy people to destination in reverse order
    algorithm_copy_backward(people, people + people_size, sizeof(Person), destination + people_size);

    // Print the destination array
    printf("Destination array after copy_backward:\n");
    print_people_array(destination, people_size);

    return 0;
}
