#include "tuple/tuple.h"
#include "string/string.h"
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"
#include "random/random.h"
#include <stdlib.h>

void process_person_data(Tuple* personData) {
    if (!personData) {
        fmt_printf("Invalid tuple.\n");
        return;
    }
    size_t size;
    int* age = (int*)tuple_get(personData, 0, &size);
    float* grade = (float*)tuple_get(personData, 1, &size);
    Vector** vector = (Vector**)tuple_get(personData, 2, &size);
    String** name = (String**)tuple_get(personData, 3, &size);

    if (age) {
        fmt_printf("Age: %d\n", *age);
    }
    if (grade) { 
        fmt_printf("Grade: %.2f\n", *grade);
    }
    if (vector && *vector) { 
        fmt_printf("Vector size: %zu\n", vector_size(*vector));
    }
    if (name && *name) { 
        fmt_printf("Name: %s\n", string_c_str(*name));
    }
}

void random_int_generator(void *output) {
    *(int *)output = random_randint(1, 100);
}

int main() {
    int age = 27;
    float grade = 19.32f;
    Vector* vec = vector_create(sizeof(int));
    String* name = string_create("amin");
    Tuple* information = NULL;

    vector_resize(vec, 10);
    algorithm_generate(vector_begin(vec), vector_end(vec), sizeof(int), random_int_generator);
    
    information = tuple_make_tuple(4, &age, sizeof(int), &grade, sizeof(float), &vec, sizeof(Vector*), &name, sizeof(String*));
    process_person_data(information);

    vector_deallocate(vec);
    string_deallocate(name);
    tuple_deallocate(information);

    return 0;
}