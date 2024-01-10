#include "algorithm/algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char name[50];
    int age;
}Person;

bool is_equal(const void* a, const void* b) {
    const Person* arg1 = (const Person*)a;
    const Person* arg2 = (const Person*)b;
    return (strcmp(arg1->name, arg2->name) == 0) && (arg1->age == arg2->age);
}

int main() {
    Person information[] = {{"amin", 27}, {"omid", 28}, {"ali", 25}, {"milad", 27}};
    Person person[] = {{"omid", 28}, {"ali", 25}};

    if (algorithm_search(information, information + 4, sizeof(person), 
                        person, person + 2, sizeof(person), is_equal)) {
        printf("its equal");
    }
    else {
        printf("its not equal");
    }
    
    return 0;
}