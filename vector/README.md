
# Vector Library in C

The Vector library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::vector` in C++.

## Compilation

To compile the Vector library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./vector/vector.c ./string/string.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Vector library in your project, include the `vector.h` header file in your source code.

```c
#include "vector/vector.h"
```

### Example 1: Integers

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{   
    Vector* intVector = vector_create(sizeof(int));
    int value = 10;
    vector_push_back(intVector, &value);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(intVector); ++i) 
    {
        int* item = (int*)vector_at(intVector, i);
        printf("%d\n", *item);
    }

    // Cleanup
    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}


```

### Example 2: Strings

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{   
    Vector* stringVector = vector_create(sizeof(char*));
    char *str = "Hello, World!";

    vector_push_back(stringVector, &str);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(stringVector); ++i) 
    {
        char** item = (char**)vector_at(stringVector, i);
        printf("%s\n", *item);
    }

    // Cleanup
    vector_deallocate(stringVector);

    return EXIT_SUCCESS;
}

```

### Example 3: User-Defined Structures

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int id;
    double value;

} MyStruct;

int main() 
{   
    Vector* structVector = vector_create(sizeof(MyStruct));
    MyStruct s = {1, 10.5};

    vector_push_back(structVector, &s);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(structVector); ++i) 
    {
        MyStruct* item = (MyStruct*)vector_at(structVector, i);
        printf("ID: %d, Value: %.2f\n", item->id, item->value);
    }

        
    return EXIT_SUCCESS;
}

```

### Example 4: Use relational operators

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector* vec1 = vector_create(sizeof(int));
    Vector* vec2 = vector_create(sizeof(int));

    int items1[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) 
        vector_push_back(vec1, &items1[i]);

    int items2[] = {1, 2, 4};
    for (int i = 0; i < 3; i++) 
        vector_push_back(vec2, &items2[i]);

    if (vector_is_equal(vec1, vec2)) 
        printf("vec1 is equal to vec2\n");
    else 
        printf("vec1 is not equal to vec2\n");

    if (vector_is_less(vec1, vec2)) 
        printf("vec1 is less than vec2\n");
    if (vector_is_greater(vec2, vec1)) 
        printf("vec2 is greater than vec2\n");

    // Cleanup
    vector_deallocate(vec1);
    vector_deallocate(vec2);
        
    return EXIT_SUCCESS;
}

```

## Example 5: Use at and char*

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    int value1 = 10, value2 = 20, value3 = 30;
    vector_push_back(intVector, &value1);
    vector_push_back(intVector, &value2);
    vector_push_back(intVector, &value3);

    for (size_t i = 0; i < (vector_size(intVector)); ++i) 
    {
        int* item = (int*)vector_at(intVector, i);
        printf("%d\n", *item);
    }

    Vector *stringVector = vector_create(sizeof(char*));

    char *str1 = "Hello";
    char *str2 = "World";
    char *str3 = "Example";

    vector_push_back(stringVector, &str1);
    vector_push_back(stringVector, &str2);
    vector_push_back(stringVector, &str3);

    for (size_t i = 0; i < vector_size(stringVector); ++i) 
    {
        char **item = (char**) vector_at(stringVector, i);
        printf("%s\n", *item);
    }

    vector_clear(intVector);
    vector_deallocate(intVector);
    vector_clear(stringVector);
    vector_deallocate(stringVector);
    
    return EXIT_SUCCESS;
}

```
## Example 6: how to use 'swap'

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *vector1 = vector_create(sizeof(int));
    Vector *vector2 = vector_create(sizeof(int));

    int value1 = 10, value2 = 20;
    vector_push_back(vector1, &value1);
    vector_push_back(vector1, &value2);

    int value3 = 30, value4 = 40;
    vector_push_back(vector2, &value3);
    vector_push_back(vector2, &value4);

    vector_swap(vector1, vector2);

    printf("Contents of vector1 after swap:\n");
    for (size_t i = 0; i < vector_size(vector1); ++i) 
    {
        int* item = (int*) vector_at(vector1, i);
        printf("%d\n", *item);
    }

    printf("Contents of vector2 after swap:\n");
    for (size_t i = 0; i < vector_size(vector2); ++i) 
    {
        int* item = (int*)vector_at(vector2, i);
        printf("%d\n", *item);
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);
                
    return EXIT_SUCCESS;
}


```

## Example 6: how to use 'pop_back' 

```c 
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements to the vector
    int values[] = {10, 20, 30, 40};
    for (int i = 0; i < 4; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Pop the last element and access it
    int *poppedItem = (int *)vector_pop_back(intVector);
    if (poppedItem) 
        printf("Popped item: %d\n", *poppedItem);
        
    vector_deallocate(intVector);
            
    return EXIT_SUCCESS;
}

```

## Example 7: how to use 'front' 

```c 

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    int *frontItem = (int *)vector_front(intVector);
    if (frontItem) 
        printf("First item: %d\n", *frontItem);
        
    vector_deallocate(intVector);
            
    return EXIT_SUCCESS;
}


```

## Example 8: how to use 'back'

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    int *backItem = (int *)vector_back(intVector);
    if (backItem) 
        printf("Last item: %d\n", *backItem);
        
    vector_deallocate(intVector);
            
    return EXIT_SUCCESS;
}

```

## Example 9: use 'data' Returns pointer to the underlying array serving as element storage
```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements to the vector
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Accessing the underlying array
    int *data = (int *)vector_data(intVector);
    if (data)
    {
        for (size_t i = 0; i < vector_size(intVector); ++i) 
            printf("Item %zu: %d\n", i, data[i]);
    }
        
    vector_deallocate(intVector);
        
    return EXIT_SUCCESS;
}


```

## Example 10: 'begin', 'end' Return pointer to the first and last element

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Using begin and end to iterate over the vector
    for (int *it = (int *)vector_begin(intVector); it != (int *)vector_end(intVector); ++it) 
        printf("%d\n", *it);
        
    vector_deallocate(intVector);
    
    
    return EXIT_SUCCESS;
}

```

## Example 11: 'cbegin', 'cend' Return const pointer to first and last element

```c 
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Using cbegin and cend for read-only iteration over the vector
    for (const int *it = (const int *)vector_cbegin(intVector); it != (const int *)vector_cend(intVector); ++it)
        printf("%d\n", *it);
        
    vector_deallocate(intVector);
    
    
    return EXIT_SUCCESS;
}

```

## Example 12 : 'crbegin', 'crend' Return reverse read-only iteration over the vector
```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Using crbegin and crend for reverse read-only iteration over the vector
    for (const int *it = (const int *)vector_crbegin(intVector); it != (const int *)vector_crend(intVector); --it) 
        printf("%d\n", *it);
        
    vector_deallocate(intVector);
    
    return EXIT_SUCCESS;
}


```

## Example 13: Using 'rbegin' and 'rend' for reverse iteration over the vector

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);

    // Using rbegin and rend for reverse iteration over the vector
    for (int *it = (int *)vector_rbegin(intVector); it != (int *)vector_rend(intVector); --it) 
        printf("%d\n", *it);

    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}


```

## Example 14: 'insert' New element to vector 

```c 
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{   

    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Insert a new element
    int newElement = 25;
    vector_insert(intVector, 0, &newElement); // Inserts 25 at position 0
    // Print elements after insertion
    for (size_t i = 0; i < vector_size(intVector); ++i) 
    {
        int* item = (int*) vector_at(intVector, i);
        printf("%d\n", *item);
    }
    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}


```

## Example 15: 'erase' element from vector

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{   
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements to the vector
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Erase elements from position 1, removing 2 elements
    vector_erase(intVector, 1, 2); // Should remove 20 and 30

    // Print elements after erasure
    for (size_t i = 0; i < vector_size(intVector); ++i) 
    {
        int* item = (int*) vector_at(intVector, i);
        printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return EXIT_SUCCESS;
}


```

## Example 16: 'reserve'

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
   Vector *intVector = vector_create(sizeof(int));

    // Reserve capacity for at least 10 elements
    vector_reserve(intVector, 10);

    // Add elements and observe no need for reallocation until the 11th element
    for (int i = 0; i < 11; ++i) 
    {
        int value = i * 10;
        vector_push_back(intVector, &value);
    }

    printf("Vector size: %zu, Vector capacity: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}


```

## Example 17: 'resize' 

```c
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() 
{
    Vector *intVector = vector_create(sizeof(int));

    // Adding elements to the vector
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    // Resize to a larger size (5)
    vector_resize(intVector, 5);

    printf("After resizing to larger size:\n");
    for (size_t i = 0; i < vector_size(intVector); ++i) 
    {
        int* item = (int*) vector_at(intVector, i);
        printf("%d\n", *item); // The last two elements will be zero-initialized
    }

    // Resize to a smaller size (2)
    vector_resize(intVector, 2);

    printf("After resizing to smaller size:\n");
    for (size_t i = 0; i < vector_size(intVector); ++i) 
    {
        int* item = (int*) vector_at(intVector, i);
        printf("%d\n", *item); // Only the first two elements remain
    }

    vector_clear(intVector);
    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}

```

## Example 18 : 'shrink_to_fit' fixed capacity

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() 
{
    Vector *intVector = vector_create(sizeof(int));

    // Reserve larger capacity
    vector_reserve(intVector, 10);

    // Adding a few elements
    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) 
        vector_push_back(intVector, &values[i]);
        
    printf("Size before shrink_to_fit: %zu, Capacity before shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    // Shrink to fit the actual number of elements
    vector_shrink_to_fit(intVector);
        
    printf("Size after shrink_to_fit: %zu, Capacity after shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);

    return EXIT_SUCCESS;
}


```

## Example 19 : use 'emplace' and 'emplace_back' insert new element at positin and at the end of 

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() 
{
    Vector *stringVector = vector_create(sizeof(char*));
    char* values[] = {"Hello", "World", "Vector", "Example"};

    for (int i = 0; i < 4; ++i) 
        vector_push_back(stringVector, &values[i]);
        
    char* newValue = "NewString";
    vector_assign(stringVector, 1, &newValue);

    char* emplaceValue = "EmplacedString";
    vector_emplace(stringVector, 2, &emplaceValue, sizeof(char*));

    char* emplaceBackValue = "EmplacedBackString";
    vector_emplace_back(stringVector, &emplaceBackValue, sizeof(char*));

    for (size_t i = 0; i < vector_size(stringVector); ++i) 
    {
        char** item = (char **)vector_at(stringVector, i);
        printf("%s\n", *item);
    }

    vector_deallocate(stringVector);

    return EXIT_SUCCESS;
}

```

## Example 20 : how to use 'String' object in Vector

```c

#include "string/string.h"
#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Vector* vec = vector_create(sizeof(String*));

    String* myString1 = string_create("Hello");
    String* myString2 = string_create("World");
    String* myString3 = string_create("Example");
    String** strPtr = NULL;

    vector_push_back(vec, &myString1);
    vector_push_back(vec, &myString2);
    vector_push_back(vec, &myString3);

    for (size_t i = 0; i < vector_size(vec); ++i) 
    {
        strPtr = (String**) vector_at(vec, i);
        if (strPtr && *strPtr) 
            printf("%s\n", (*strPtr)->dataStr); 
    }

    string_deallocate(myString1);
    string_deallocate(myString2);
    string_deallocate(myString3);

    vector_deallocate(vec);

    return 0;
}

```

## Example 21 : how to use relationals operators in Vector 

```c

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ELEMENTS 100000000

int main() 
{
    Vector *vector1 = vector_create(sizeof(int));
    Vector *vector2 = vector_create(sizeof(int));

    int value1 = 10, value2 = 20;
    vector_push_back(vector1, &value1);
    vector_push_back(vector1, &value2);

    int value3 = 30, value4 = 40;
    vector_push_back(vector2, &value3);
    vector_push_back(vector2, &value4);
    
    if (vector_is_equal(vector1, vector2))
        printf("Vector1 is equal with Vector2\n");

    if (vector_is_less(vector1, vector2))
        printf("Vector1 is less than Vector2\n");

    if (vector_is_greater(vector1, vector2))
        printf("Vector1 is greater than Vector2\n");

    vector_deallocate(vector1);
    vector_deallocate(vector2);

    return EXIT_SUCCESS;
}


```

## Example 22 : bench 

gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./vector/vector.c
Average Custom Vector Time: 0.008564 seconds

```c 

#include "vector/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_ELEMENTS 100000000

int main() 
{
    struct timespec start, end;
    double time_sum = 0;
    Vector* vec = vector_create(sizeof(int));

    clock_gettime(CLOCK_MONOTONIC, &start);
        
    for (int i = 0; i < NUM_ELEMENTS; i++) 
        vector_push_back(vec, &i);
        
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sum += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    double average_time = time_sum / NUM_ITERATIONS;
    printf("Average Custom Vector Time: %f seconds\n", average_time);

    vector_deallocate(vec);

    return EXIT_SUCCESS;
}


```

and in c++ 
g++ -std=c++14 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.cpp
std::vector Time: 0.344828 seconds

```c
#include <iostream>
#include <vector>
#include <chrono>

#define NUM_ELEMENTS 100000000

int main() 
{
    std::vector<int> vec;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ELEMENTS; i++) 
        vec.push_back(i);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_spent = end - start;

    std::cout << "std::vector Time: " << time_spent.count() << " seconds\n";

    return 0;
}


```
## Contribution

Your contributions to this project are welcome! If you have suggestions or improvements, feel free to fork the repository, make your changes, and submit a pull request.


### Example 23 : how to use vector_begin and vector_end with String Object 

just you should now we can do this in lots of way by String ..
also don't use printf in loop because it is not optimal.

Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 0.000010 seconds

```c
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

```

also result in Cpp 

Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 1.1523e-05 seconds

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>

int main() 
{
    // Initialize the vector with some fruits
    std::vector<std::string> fruits = {"Apple", "Banana", "Cherry", "Lemon", "Watermelon"};
    std::stringstream buffer;

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Concatenate processed strings
    for (const auto& fruit : fruits) 
        buffer << fruit << "\n";
    
    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    // Output the concatenated string and the elapsed time
    std::cout << buffer.str(); // Single print call
    std::cout << "Time taken: " << elapsed.count() << " seconds\n";

    return 0;
}

```