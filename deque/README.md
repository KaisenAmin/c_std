
# Deque Library

The Deque library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size Deques, offering similar functionality to `std::deque` in C++.

## Compilation

To compile the Deque library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./deque/deque.c 
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Deque library in your project, include the `Deque.h` header file in your source code.

```c
#include "deque/deque.h"
```

### Example 1: Basic Push and Pop Operations
```c
Deque* myDeque = deque_create(sizeof(int));
int val1 = 10, val2 = 20;

// Pushing elements
myDeque->push_back(myDeque, &val1); // Add to back
printf("Pushed %d to back\n", val1);

myDeque->push_front(myDeque, &val2); // Add to front
printf("Pushed %d to front\n", val2);

// Accessing elements
int* frontVal = (int*)myDeque->front(myDeque); // Access front element
int* backVal = (int*)myDeque->back(myDeque); // Access back element

printf("Front: %d, Back: %d\n", *frontVal, *backVal);

// Popping elements
myDeque->pop_front(myDeque); // Remove from front
printf("Popped from front\n");

myDeque->pop_back(myDeque); // Remove from back
printf("Popped from back\n");

myDeque->deallocate(myDeque);
```

### Example 2: Using `deque_at` to Access Elements
```c
Deque* myDeque = deque_create(sizeof(int));
int nums[] = {1, 2, 3, 4, 5};

// Pushing elements
for (int i = 0; i < 5; ++i) 
{
    myDeque->push_back(myDeque, &nums[i]);
    printf("Pushed %d to back\n", nums[i]);
}

// Accessing elements using deque_at
printf("Deque elements: ");
for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
    printf("%d ", *(int*)myDeque->at(myDeque, i));

printf("\n");

myDeque->deallocate(myDeque);
```

### Example 3: Resizing Deque
```c

#include <stdlib.h>
#include <stdio.h>

Deque* myDeque = deque_create(sizeof(int));

for (int i = 0; i < 10; ++i) 
{
    int* ptr = malloc(sizeof(int)); 
    if (ptr == NULL) 
    {
        fprintf(stderr, "Failed to allocate memory\n");
        free(ptr);
        break; // Exit the loop or return from function
    }

    *ptr = i; // Set the allocated integer to the value of i
    myDeque->push_back(myDeque, ptr); // Push the pointer onto the deque

    printf("Pushed %d to back\n", i);
}

printf("Size before resize: %zu\n", myDeque->length(myDeque));

// Resizing the deque
myDeque->resize(myDeque, 5); // Resize to smaller size
printf("Resized deque to 5 elements\n");
printf("Size after resize: %zu\n", myDeque->length(myDeque));

// Printing resized deque
printf("Elements in resized deque: ");
for (size_t i = 0; i < myDeque->length(myDeque); ++i) {
    printf("%d ", *(int*)myDeque->at(myDeque, i));
}
printf("\n");

myDeque->deallocate(myDeque);

```


## Example 4: Using Deque with Structs

```c
#include "deque/deque.h"
#include <stdio.h>

typedef struct 
{
    int id;
    char name[50];

} Person;


int main(int argc, char** argv)
{
    Deque* personDeque = deque_create(sizeof(Person));
    Person p1 = {1, "Alice"}, p2 = {2, "Bob"};

    personDeque->push_back(personDeque, &p1);
    personDeque->push_back(personDeque, &p2);

    Person* frontPerson = (Person*)personDeque->front(personDeque);
    printf("Front person: %d, %s\n", frontPerson->id, frontPerson->name);

    personDeque->deallocate(personDeque);

    return EXIT_SUCCESS;
}

```

## Example 5: Storing char Elements

```c

Deque* charDeque = deque_create(sizeof(char));
char a = 'A', b = 'B';

charDeque->push_back(charDeque, &a);
charDeque->push_back(charDeque, &b);

char* frontChar = (char*)charDeque->front(charDeque);
printf("Front char: %c\n", *frontChar);

charDeque->deallocate(charDeque);

```

## Example 6 : Storing float Elements

```c

Deque* floatDeque = deque_create(sizeof(float));
float num1 = 1.5f, num2 = 2.5f;

floatDeque->push_back(floatDeque, &num1);
floatDeque->push_back(floatDeque, &num2);

float* backFloat = (float*)floatDeque->back(floatDeque);
printf("Back float: %f\n", *backFloat);

floatDeque->deallocate(floatDeque);

```

## Example 7 : Reversing Elements in a Deque

```c

#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    Deque* reverseDeque = deque_create(sizeof(int));

    // Allocate and push back elements
    for (int i = 0; i < 5; ++i) 
    {
        int* ptr = malloc(sizeof(int));
        if (ptr == NULL) 
        {
            fprintf(stderr, "Failed to allocate memory\n");

            free(ptr);
            exit(EXIT_FAILURE);
        }

        *ptr = i;
        reverseDeque->push_back(reverseDeque, ptr);
    }

    printf("Reversed Deque: ");

    // Iterate using the length of the deque
    for (size_t i = 0; i < reverseDeque->length(reverseDeque); ++i)
    {
        int* valPtr = (int*)reverseDeque->at(reverseDeque, reverseDeque->length(reverseDeque) - 1 - i);

        printf("%d ", *valPtr);
        free(valPtr); // Free the memory
    }

    printf("\n");

    reverseDeque->deallocate(reverseDeque);

    getchar();
    return EXIT_SUCCESS;
}
```

## Example 8 : Nested Deque 

```c

#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
    Deque* outerDeque = deque_create(sizeof(Deque*));

    for (int i = 0; i < 3; ++i) 
    {
        Deque* innerDeque = deque_create(sizeof(int*));

        for (int j = 0; j < 3; ++j) 
        {
            int* valPtr = malloc(sizeof(int));

            if (valPtr == NULL) 
            {
                fprintf(stderr, "Failed to allocate memory\n");
                
                free(valPtr);
                exit(EXIT_FAILURE);
            }
            *valPtr = i * 3 + j;
            innerDeque->push_back(innerDeque, valPtr);
        }

        outerDeque->push_back(outerDeque, &innerDeque);
    }

    // Access and print elements
    for (size_t i = 0; i < outerDeque->length(outerDeque); ++i) 
    {
        Deque* innerDeque = *(Deque**)outerDeque->at(outerDeque, i);

        for (size_t j = 0; j < innerDeque->length(innerDeque); ++j) 
        {
            int* valPtr = (int*)innerDeque->at(innerDeque, j);

            printf("%d ", *valPtr);
            free(valPtr); // Free the memory allocated for each integer
        }
        printf("\n");
        innerDeque->deallocate(innerDeque);
    }

    outerDeque->deallocate(outerDeque);

    return EXIT_SUCCESS;
}

```

## Example 9 : Copying Deque 

```c

#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
    Deque* original = deque_create(sizeof(int));

    for (int i = 0; i < 5; ++i) 
    {
        int* ptr = malloc(sizeof(int));
        if (ptr == NULL) 
        {
            fprintf(stderr, "Failed to allocate memory\n");

            free(ptr);
            exit(EXIT_FAILURE);
        }

        *ptr = i;
        original->push_back(original, ptr);
    }

    // Copying the deque
    Deque* copy = deque_create(sizeof(int));
    for (size_t i = 0; i < original->length(original); ++i) 
    {
        int* val = (int*)original->at(original, i);
        copy->push_back(copy, val);
    }

    // Print copied deque
    printf("Copied Deque: ");
    for (size_t i = 0; i < copy->length(copy); ++i) 
        printf("%d ", *(int*)copy->at(copy, i));
    
    printf("\n");

    original->deallocate(original);
    copy->deallocate(copy);

    getchar();
    return EXIT_SUCCESS;
}

```

## Example 10 : Store Copies of Struct in the Deque

```c
#include "deque/deque.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
    char* name;
    int age;

} Person;

int main() 
{
    Deque* people = deque_create(sizeof(Person));

    char* names[] = {"Alice", "Bob", "Charlie"};
    int ages[] = {25, 30, 35};
    
    for (size_t i = 0; i < 3; ++i) 
    {
        Person* p = malloc(sizeof(Person));  // Dynamically allocate a Person
        p->name = strdup(names[i]);  // Duplicate the name string
        p->age = ages[i];

        people->push_back(people, p);  // Add the Person pointer to the deque
    }

    // Access and print people
    for (size_t i = 0; i < people->length(people); ++i) 
    {
        Person* p = (Person*)people->at(people, i);
        printf("Name: %s, Age: %d\n", p->name, p->age);

        free(p->name);  // Free the dynamically allocated name
        free(p);
    }

    people->deallocate(people);

    return EXIT_SUCCESS;
}

```

### Example 11: Shrink to Fit Deque

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));

    // Adding elements
    for (int i = 0; i < 100; ++i) 
    {
        int* ptr = malloc(sizeof(int)); 
        if (ptr == NULL) 
        {
            fprintf(stderr, "Failed to allocate memory\n");
            free(ptr);
            break; // Exit the loop or return from function
        }

        *ptr = i; // Set the allocated integer to the value of i
        myDeque->push_back(myDeque, ptr); // Push the pointer onto the deque
    }

    // Shrinking the deque
    myDeque->resize(myDeque, 10);
    myDeque->shrink_to_fit(myDeque);

    printf("Deque after shrink to fit (Size: %zu):\n", myDeque->length(myDeque));
    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        printf("%d ", *(int*)myDeque->at(myDeque, i));
    
    printf("\n");

    myDeque->deallocate(myDeque);
    
    return EXIT_SUCCESS;
}

```

### Example 12: Inserting and Erasing Elements

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));
    int insertVal = 99;

    // Adding elements
    for (int i = 0; i < 5; ++i) 
    {
        int* ptr = malloc(sizeof(int)); 
        if (ptr == NULL) 
        {
            fprintf(stderr, "Failed to allocate memory\n");
            free(ptr);
            break; // Exit the loop or return from function
        }

        *ptr = i; // Set the allocated integer to the value of i
        myDeque->push_back(myDeque, ptr); // Push the pointer onto the deque
    }

    // Inserting an element and erase
    myDeque->insert(myDeque, 2, &insertVal);
    myDeque->erase(myDeque, 1);

    printf("Deque after insert and erase:\n");
    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        printf("%d ", *(int*)myDeque->at(myDeque, i));
    
    printf("\n");

    myDeque->deallocate(myDeque);
    return EXIT_SUCCESS;
}

```

### Example 13: Using Emplace Methods
This example demonstrates the use of `emplace_back`, `emplace_front`, and `emplace` methods for in-place construction.

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));
    int val = 10;

    // Emplacing elements
    myDeque->emplace_back(myDeque, &val);
    myDeque->emplace_front(myDeque, &val);
    myDeque->emplace(myDeque, 1, &val);

    printf("Size of myQueue %zu\n", myDeque->length(myDeque));
    printf("Deque after emplace operations:\n");
    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        printf("%d ", *(int*)myDeque->at(myDeque, i));
    
    printf("\n");

    myDeque->deallocate(myDeque);
    return EXIT_SUCCESS;
}
```

### Example 14: Using Relational Operators
This example illustrates how to compare two deques using relational operators.

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* deque1 = deque_create(sizeof(int));
    Deque* deque2 = deque_create(sizeof(int));
    int val = 5;

    // Adding elements
    for (int i = 0; i < 3; ++i) 
    {
        int *newInt1 = malloc(sizeof(int));
        *newInt1 = i;
        deque1->push_back(deque1, newInt1);

        int *newInt2 = malloc(sizeof(int));
        *newInt2 = val;
        deque2->push_back(deque2, newInt2);
    }

    // Comparing deques
    if (deque1->is_less(deque1, deque2)) 
        printf("Deque1 is less than Deque2\n");
    
    for (size_t i = 0; i < 3; ++i) 
    {
        free(deque1->at(deque1, i));
        free(deque2->at(deque2, i));
    }

    deque1->deallocate(deque1);
    deque2->deallocate(deque2);
    return EXIT_SUCCESS;
}
```

### Example 15: Iterating Using Deque Iterators
This example demonstrates iterating over a deque using its iterators.

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));

    for (int i = 0; i < 3; ++i) 
    {
        int* newInt = malloc(sizeof(int));
        *newInt = i;

        myDeque->push_back(myDeque, newInt);
    }

    DequeIterator it = myDeque->begin(myDeque);
    DequeIterator end = myDeque->end(myDeque);

    printf("Deque elements: ");
    
    while (!end.equals(&it, &end)) 
    {
        printf("%d ", *(int*)it.get(&it));
        it.increment(&it);
    }
    printf("\n");

    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        free(myDeque->at(myDeque, i));
    

    myDeque->deallocate(myDeque);

    return EXIT_SUCCESS;
}

```

## Example 16 : Reverse Iteration 

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));

    // Adding elements
    for (int i = 0; i < 3; ++i) 
    {
        int* newInt = malloc(sizeof(int));

        *newInt = i;
        myDeque->push_back(myDeque, newInt);
    }

    // Reverse iteration
    DequeIterator rit = myDeque->rbegin(myDeque);
    DequeIterator rend = myDeque->rend(myDeque);

    printf("Deque elements in reverse: ");
    while (!rend.equals(&rit, &rend)) 
    {
        printf("%d ", *(int*)rit.get(&rit));
        rit.decrement(&rit);
    }
    
    printf("\n");

    // Freeing allocated memory
    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        free(myDeque->at(myDeque, i));

    myDeque->deallocate(myDeque);

    return EXIT_SUCCESS;
}

```

## Example 17 : 'clear' and 'emplace_back'

```c

#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));
    int val = 42;

    myDeque->emplace_back(myDeque, &val);
    myDeque->emplace_back(myDeque, &val);

    printf("Deque after emplacing elements: ");
    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        printf("%d ", *(int*)myDeque->at(myDeque, i));

    printf("\n");

    myDeque->clear(myDeque);
    printf("Deque after clearing: Length = %zu\n", myDeque->length(myDeque));

    myDeque->deallocate(myDeque);

    return EXIT_SUCCESS;
}

```

## Example 18 : Convert example in this url 'https://www.geeksforgeeks.org/deque-cpp-stl/' to Deque 

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

void showdq(Deque* dq) 
{
    for (size_t i = 0; i < dq->length(dq); ++i) 
    {
        int* elem = (int*)dq->at(dq, i);
        printf("\t%d", *elem);
    }
    
    printf("\n");
}

int main() 
{
    Deque* gquiz = deque_create(sizeof(int));

    // Push elements
    int val1 = 10, val2 = 20, val3 = 30, val4 = 15;
    gquiz->push_back(gquiz, &val1);
    gquiz->push_front(gquiz, &val2);
    gquiz->push_back(gquiz, &val3);
    gquiz->push_front(gquiz, &val4);

    printf("The deque gquiz is : ");
    showdq(gquiz);

    printf("\ngquiz.max_size() : %zu", gquiz->max_size(gquiz));
    printf("\ngquiz.at(2) : %d", *(int*)gquiz->at(gquiz, 2));
    printf("\ngquiz.front() : %d", *(int*)gquiz->front(gquiz));
    printf("\ngquiz.back() : %d", *(int*)gquiz->back(gquiz));

    printf("\ngquiz.pop_front() : ");
    gquiz->pop_front(gquiz);
    showdq(gquiz);

    printf("\ngquiz.pop_back() : ");
    gquiz->pop_back(gquiz);
    showdq(gquiz);

    // Free the deque
    gquiz->deallocate(gquiz);

    return 0;
}

```

## Example 19 : 'crbegin', 'crend'

```c
#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Deque* myDeque = deque_create(sizeof(int));

    // Adding elements
    for (int i = 0; i < 5; ++i) 
    {
        int* newInt = malloc(sizeof(int));
        *newInt = i;
        myDeque->push_back(myDeque, newInt);
    }

    // Constant reverse iteration
    const DequeIterator crit = myDeque->crbegin(myDeque);
    const DequeIterator crend = myDeque->crend(myDeque);

    printf("Constant reverse iteration: ");
    for (DequeIterator it = crit; !crend.equals(&it, &crend); it.increment(&it)) 
        printf("%d ", *(const int*)it.get(&it));
    
    printf("\n");

    // Freeing allocated memory
    for (size_t i = 0; i < myDeque->length(myDeque); ++i) 
        free(myDeque->at(myDeque, i));

    myDeque->deallocate(myDeque);

    return EXIT_SUCCESS;
}

```