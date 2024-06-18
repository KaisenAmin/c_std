#include "vector/vector.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

typedef struct {
    String* name;
    int age;
} Person;

Person* create_person(int group, int index) {
    Person* p = (Person*)malloc(sizeof(Person));
    p->name = string_create("");
    string_format(p->name, "Person_%d_%d", group, index);
    p->age = group * 10 + index;

    return p;
}

void deallocate_person(Person* p) {
    if (p) {
        string_deallocate(p->name);
        free(p);
    }
}

int main() {
    Vector* vec2D = vector_create(sizeof(Vector*));

    // Populate the 2D vector with vectors of Persons
    for (int i = 0; i < 2; i++) {
        Vector* peopleVec = vector_create(sizeof(Person*));

        for (int j = 0; j < 3; j++) {
            Person* person = create_person(i, j);
            vector_push_back(peopleVec, &person);
        }
        vector_push_back(vec2D, &peopleVec);
    }

    // Iterate and print each person's details
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** peopleVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* peopleVec = *peopleVecPtr;

        for (size_t j = 0; j < vector_size(peopleVec); j++) {
            Person** personPtr = (Person**)vector_at(peopleVec, j);
            Person* person = *personPtr;

            fmt_printf("Name: %s, Age: %d\n", string_c_str(person->name), person->age);
        }
    }

    // Cleanup
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** peopleVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* peopleVec = *peopleVecPtr;

        for (size_t j = 0; j < vector_size(peopleVec); j++) {
            Person** personPtr = (Person**)vector_at(peopleVec, j);
            deallocate_person(*personPtr);
        }
        vector_deallocate(peopleVec);
    }

    vector_deallocate(vec2D);
    return 0;
}