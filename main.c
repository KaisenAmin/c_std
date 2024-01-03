#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int id;
    char name[10];
} Person;

uint32_t simple_rng() {
    return rand();  // Simple RNG, replace with a better one if needed
}

int main() {
    srand((unsigned int)time(NULL)); 

    Person people[] = {
        {1, "Alice"},
        {2, "Bob"},
        {3, "Charlie"},
        {4, "Dave"},
        {5, "Eve"}
    };
    size_t size = sizeof(people) / sizeof(people[0]);
    
    algorithm_shuffle(people, size, sizeof(Person), simple_rng);

    printf("Shuffled People:\n");
    for (size_t i = 0; i < size; ++i) {
        printf("ID: %d, Name: %s\n", people[i].id, people[i].name);
    }

    return 0;
}
