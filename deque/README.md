
# Deque Library in C

Author: amin tahmasebi
Release Date: 2023
License: GNU General Public License v3.0


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
#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>



int main() 
{
    Deque* myDeque = deque_create(sizeof(int));
    int val1 = 10, val2 = 20;

    // Pushing elements
    deque_push_back(myDeque, &val1); // Add to back
    printf("Pushed %d to back\n", val1);

    deque_push_front(myDeque, &val2); // Add to front
    printf("Pushed %d to front\n", val2);

    // Accessing elements
    int* frontVal = (int*)deque_front(myDeque); // Access front element
    int* backVal = (int*)deque_back(myDeque); // Access back element

    printf("Front: %d, Back: %d\n", *frontVal, *backVal);

    // Popping elements
    deque_pop_front(myDeque); // Remove from front
    printf("Popped from front\n");

    deque_pop_back(myDeque); // Remove from back
    printf("Popped from back\n");

    deque_deallocate(myDeque);

    return 0;
}
```

### Example 2: Using `deque_at` to Access Elements
```c
#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>



int main() 
{
    Deque* myDeque = deque_create(sizeof(int));
    int nums[] = {1, 2, 3, 4, 5};

    // Pushing elements
    for (int i = 0; i < 5; ++i) 
    {
        deque_push_back(myDeque, &nums[i]);
        printf("Pushed %d to back\n", nums[i]);
    }

    // Accessing elements using deque_at
    printf("Deque elements: ");
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        printf("%d ", *(int*)deque_at(myDeque, i));

    printf("\n");

    deque_deallocate(myDeque);

    return 0;
}
```

### Example 3: Resizing Deque
```c
#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main() 
{
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
        deque_push_back(myDeque, ptr); // Push the pointer onto the deque

        printf("Pushed %d to back\n", i);
    }

    printf("Size before resize: %zu\n", deque_length(myDeque));

    // Resizing the deque
    deque_resize(myDeque, 5); // Resize to smaller size
    printf("Resized deque to 5 elements\n");
    printf("Size after resize: %zu\n", deque_length(myDeque));

    // Printing resized deque
    printf("Elements in resized deque: ");
    for (size_t i = 0; i < deque_length(myDeque); ++i) {
        printf("%d ", *(int*)deque_at(myDeque, i));
    }
    printf("\n");

    deque_deallocate(myDeque);
    return 0;
}


```


## Example 4: Using Deque with Structs

```c
#include "deque/deque.h"
#include <stdio.h>

#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



typedef struct 
{
    int id;
    char name[50];

} Person;


int main()
{
    Deque* personDeque = deque_create(sizeof(Person));
    Person p1 = {1, "Alice"}, p2 = {2, "Bob"};

    deque_push_back(personDeque, &p1);
    deque_push_back(personDeque, &p2);

    Person* frontPerson = (Person*)deque_front(personDeque);
    printf("Front person: %d, %s\n", frontPerson->id, frontPerson->name);

    deque_deallocate(personDeque);

    return EXIT_SUCCESS;
}

```

## Example 5: Storing char Elements

```c
#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



typedef struct 
{
    int id;
    char name[50];

} Person;


int main()
{
    Deque* charDeque = deque_create(sizeof(char));
    char a = 'A', b = 'B';

    deque_push_back(charDeque, &a);
    deque_push_back(charDeque, &b);

    char* frontChar = (char*)deque_front(charDeque);
    printf("Front char: %c\n", *frontChar);

    deque_deallocate(charDeque);

    return EXIT_SUCCESS;
}

```

## Example 6 : Storing float Elements

```c

#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    Deque* floatDeque = deque_create(sizeof(float));
    float num1 = 1.5f, num2 = 2.5f;

    deque_push_back(floatDeque, &num1);
    deque_push_back(floatDeque, &num2);

    float* backFloat = (float*)deque_back(floatDeque);
    printf("Back float: %f\n", *backFloat);

    deque_deallocate(floatDeque);
    
    return EXIT_SUCCESS;
}

```

## Example 7 : Reversing Elements in a Deque

```c

#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
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
        deque_push_back(reverseDeque, ptr);
    }

    printf("Reversed Deque: ");

    // Iterate using the length of the deque
    for (size_t i = 0; i < deque_length(reverseDeque); ++i)
    {
        int* valPtr = (int*)deque_at(reverseDeque, deque_length(reverseDeque) - 1 - i);

        printf("%d ", *valPtr);
        free(valPtr); // Free the memory
    }

    printf("\n");

    deque_deallocate(reverseDeque);

    getchar();
    return EXIT_SUCCESS;
}
```

## Example 8 : Nested Deque 

```c

#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>


int main()
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
            deque_push_back(innerDeque, valPtr);
        }

        deque_push_back(outerDeque, &innerDeque);
    }

    // Access and print elements
    for (size_t i = 0; i < deque_length(outerDeque); ++i) 
    {
        Deque* innerDeque = *(Deque**)deque_at(outerDeque, i);

        for (size_t j = 0; j < deque_length(innerDeque); ++j) 
        {
            int* valPtr = (int*)deque_at(innerDeque, j);

            printf("%d ", *valPtr);
            free(valPtr); // Free the memory allocated for each integer
        }
        printf("\n");
        deque_deallocate(innerDeque);
    }

    deque_deallocate(outerDeque);

    return EXIT_SUCCESS;
}

```

## Example 9 : Copying Deque 

```c

#include "deque/deque.h"
#include <stdio.h>
#include <stdlib.h>


#include "deque/deque.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
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
        deque_push_back(original, ptr);
    }

    // Copying the deque
    Deque* copy = deque_create(sizeof(int));
    for (size_t i = 0; i < deque_length(original); ++i) 
    {
        int* val = (int*)deque_at(original, i);
        deque_push_back(copy, val);
    }

    // Print copied deque
    printf("Copied Deque: ");
    for (size_t i = 0; i < deque_length(copy); ++i) 
        printf("%d ", *(int*)deque_at(copy, i));
    
    printf("\n");

    deque_deallocate(original);
    deque_deallocate(copy);

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

        deque_push_back(people, p);  // Add the Person pointer to the deque
    }

    // Access and print people
    for (size_t i = 0; i < deque_length(people); ++i) 
    {
        Person* p = (Person*)deque_at(people, i);
        printf("Name: %s, Age: %d\n", p->name, p->age);

        free(p->name);  // Free the dynamically allocated name
        free(p);
    }

    deque_deallocate(people);

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
        deque_push_back(myDeque, ptr); // Push the pointer onto the deque
    }

    // Shrinking the deque
    deque_resize(myDeque, 10);
    deque_shrink_to_fit(myDeque);

    printf("Deque after shrink to fit (Size: %zu):\n", deque_length(myDeque));
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        printf("%d ", *(int*)deque_at(myDeque, i));
    
    printf("\n");

    deque_deallocate(myDeque);
    
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
        deque_push_back(myDeque, ptr); // Push the pointer onto the deque
    }

    // Inserting an element and erase
    deque_insert(myDeque, 2, &insertVal);
    deque_erase(myDeque, 1);

    printf("Deque after insert and erase:\n");
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        printf("%d ", *(int*)deque_at(myDeque, i));
    
    printf("\n");

    deque_deallocate(myDeque);
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
    deque_emplace_back(myDeque, &val);
    deque_emplace_front(myDeque, &val);
    deque_emplace(myDeque, 1, &val);

    printf("Size of myQueue %zu\n", deque_length(myDeque));
    printf("Deque after emplace operations:\n");
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        printf("%d ", *(int*)deque_at(myDeque, i));
    
    printf("\n");

    deque_deallocate(myDeque);
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
        deque_push_back(deque1, newInt1);

        int *newInt2 = malloc(sizeof(int));
        *newInt2 = val;
        deque_push_back(deque2, newInt2);
    }

    // Comparing deques
    if (deque_is_less(deque1, deque2)) 
        printf("Deque1 is less than Deque2\n");
    
    for (size_t i = 0; i < 3; ++i) 
    {
        free(deque_at(deque1, i));
        free(deque_at(deque2, i));
    }

    deque_deallocate(deque1);
    deque_deallocate(deque2);
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

        deque_push_back(myDeque, newInt);
    }

    DequeIterator it = deque_begin(myDeque);
    DequeIterator end = deque_end(myDeque);

    printf("Deque elements: ");
    
    while (!iterator_equals(&it, &end)) 
    {
        printf("%d ", *(int*)iterator_get(&it));
        iterator_increment(&it);
    }
    printf("\n");

    // Free the dynamically allocated integers
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        free(deque_at(myDeque, i));
    
    deque_deallocate(myDeque);

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
        deque_push_back(myDeque, newInt);
    }

    // Reverse iteration
    DequeIterator rit = deque_rbegin(myDeque);
    DequeIterator rend = deque_rend(myDeque);

    printf("Deque elements in reverse: ");
    while (!iterator_equals(&rit, &rend)) 
    {
        printf("%d ", *(int*)iterator_get(&rit));
        iterator_decrement(&rit);
    }
    
    printf("\n");

    // Freeing allocated memory
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        free(deque_at(myDeque, i));

    deque_deallocate(myDeque);

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

    deque_emplace_back(myDeque, &val);
    deque_emplace_back(myDeque, &val);

    printf("Deque after emplacing elements: ");
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        printf("%d ", *(int*)deque_at(myDeque, i));

    printf("\n");

    deque_clear(myDeque);
    printf("Deque after clearing: Length = %zu\n", deque_length(myDeque));

    deque_deallocate(myDeque);

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
    for (size_t i = 0; i < deque_length(dq); ++i) 
    {
        int* elem = (int*)deque_at(dq, i);
        printf("\t%d", *elem);
    }
    
    printf("\n");
}

int main() 
{
    Deque* gquiz = deque_create(sizeof(int));

    // Push elements
    int val1 = 10, val2 = 20, val3 = 30, val4 = 15;
    deque_push_back(gquiz, &val1);
    deque_push_front(gquiz, &val2);
    deque_push_back(gquiz, &val3);
    deque_push_front(gquiz, &val4);

    printf("The deque gquiz is : ");
    showdq(gquiz);

    printf("\ngquiz.max_size() : %zu", deque_max_size(gquiz));
    printf("\ngquiz.at(2) : %d", *(int*)deque_at(gquiz, 2));
    printf("\ngquiz.front() : %d", *(int*)deque_front(gquiz));
    printf("\ngquiz.back() : %d", *(int*)deque_back(gquiz));

    printf("\ngquiz.pop_front() : ");
    deque_pop_front(gquiz);
    showdq(gquiz);

    printf("\ngquiz.pop_back() : ");
    deque_pop_back(gquiz);
    showdq(gquiz);

    // Free the deque
    deque_deallocate(gquiz);

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
        deque_push_back(myDeque, newInt);
    }

    // Constant reverse iteration
    const DequeIterator crit = *deque_crbegin(myDeque);
    const DequeIterator crend = *deque_crend(myDeque);

    printf("Constant reverse iteration: ");
    for (DequeIterator it = crit; !iterator_equals(&it, &crend); iterator_increment(&it)) 
        printf("%d ", *(const int*)iterator_get(&it));
    
    printf("\n");

    // Freeing allocated memory
    for (size_t i = 0; i < deque_length(myDeque); ++i) 
        free(deque_at(myDeque, i));

    deque_deallocate(myDeque);

    return EXIT_SUCCESS;
}

```