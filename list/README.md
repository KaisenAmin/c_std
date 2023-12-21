
# List Library

The List library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size List, offering similar functionality to `std::list` in C++.

## Compilation

To compile the List library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c list/list.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the List library in your project, include the `list.h` header file in your source code.

```c
#include "list/list.h"
```

### Example 1: Create List and Add Elements Using `push_back`

```c
List *myList = list_create(sizeof(int), compare_ints);
int values[] = {10, 20, 30, 40, 50};

for (int i = 0; i < 5; ++i) 
    myList->push_back(myList, &values[i]);

myList->deallocate(myList);
```

### Example 2: Add Elements Using `push_front`

```c
List *myList = list_create(sizeof(int), compare_ints);
int values[] = {10, 20, 30, 40, 50};

for (int i = 0; i < 5; ++i) 
    myList->push_front(myList, &values[i]);

myList->deallocate(myList);
```

### Example 3: Pop Elements from Front

```c
List *myList = list_create(sizeof(int), compare_ints);
int value = 100;

myList->push_front(myList, &value);
myList->pop_front(myList);

myList->deallocate(myList);
```

### Example 4: Pop Elements from Back

```c
List *myList = list_create(sizeof(int), compare_ints);
int value = 100;

myList->push_back(myList, &value);
myList->pop_back(myList);

myList->deallocate(myList);
```

### Example 5: Insert Element at Specific Position

```c
List *myList = list_create(sizeof(int), compare_ints);
int value = 100;

myList->insert(myList, 0, &value); // Insert at the beginning

myList->deallocate(myList);
```

### Example 6: Erase Element at Specific Position

```c
List *myList = list_create(sizeof(int), compare_ints);
int value = 100;

myList->push_back(myList, &value);
myList->erase(myList, 0); // Erase the first element

myList->deallocate(myList);
```

### Example 7: Clearing the List

```c
List *myList = list_create(sizeof(int), compare_ints);
int value = 100;

myList->push_back(myList, &value);
myList->clear(myList); // Clear the list

myList->deallocate(myList);
```

### Example 8: Resize the List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *list1 = list_create(sizeof(int), compare_ints);

    int values[] = {50, 40, 30, 20, 10};
    for (int i = 0; i < 5; ++i) 
        list_push_back(list1, &values[i]);

    list_resize(list1, 10, 0);

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next)
    {
        int* value = (int*)node->value;
        printf("%d\n", *value);
    }


    list_deallocate(list1);

    return EXIT_SUCCESS;
}
```

### Example 9: Swap Two Lists

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{

    List *list1 = list_create(sizeof(int), compare_ints);
    List *list2 = list_create(sizeof(int), compare_ints);

    int values[] = {50, 40, 30, 20, 10};
    for (int i = 0; i < 5; ++i) 
        list_push_back(list1, &values[i]);

    int values2[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; ++i) 
        list_push_back(list2, &values2[i]);

    list_swap(list1, list2); // Swap list1 and list2

    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next)
    {
        int* value = (int*)node->value;
        printf("%d\n", *value);
    }


    list_deallocate(list1);
    list_deallocate(list2);

    return EXIT_SUCCESS;
}
```

### Example 10: Reverse a List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {50, 40, 30, 20, 10};

    for (int i = 0; i < 5; ++i) 
        list_push_back(myList, &values[i]);
        
    list_reverse(myList); // Reverse the list_
    
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next)
    {
        int* value = (int*)node->value;
        printf("%d\n", *value);
    }

    list_deallocate(myList);

    return EXIT_SUCCESS;
}
```

### Example 11: Sort a List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {50, 40, 30, 20, 10};

    for (int i = 0; i < 5; ++i) 
        list_push_back(myList, &values[i]);

    list_sort(myList); // Sort the list
    
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next)
    {
        int* value = (int*)node->value;
        printf("%d\n", *value);
    }

    list_deallocate(myList);
    return EXIT_SUCCESS;
}
```

### Example 12: Check if List is Empty

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *myList = list_create(sizeof(int), compare_ints);
    bool isEmpty = list_empty(myList); // Check if list is empty

    list_deallocate(myList);

    return EXIT_SUCCESS;
}
```

### Example 13: Get the Length of the List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *myList = list_create(sizeof(int), compare_ints);
    int value = 100;

    list_push_back(myList, &value);
    size_t length = list_length(myList); // Get the length of the list

    printf("Size of List %zu\n", length);
    
    list_deallocate(myList);

    return EXIT_SUCCESS;
}
```

### Example 14: Assign Values to List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *myList = list_create(sizeof(int), compare_ints);
    int values[] = {1, 2, 3, 4, 5};

    list_assign(myList, values, 5); // Assign values to list

    // Print values
    for (Node* node = list_begin(myList); node != list_end(myList); node = node->next)
    {
        int* value = (int*)node->value;
        printf("%d\n", *value);
    }

    list_deallocate(myList);

    return EXIT_SUCCESS;
}
```

### Example 15: Merge Two Lists

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"


static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    List *list1 = list_create(sizeof(int), compare_ints);
    List *list2 = list_create(sizeof(int), compare_ints);
    int value1 = 10, value2 = 20;

    list_push_back(list1, &value1);
    list_push_back(list2, &value2);
    list_merge(list1, list2); // Merge list2 into list1

    list_deallocate(list1);
    list_deallocate(list2);

    return EXIT_SUCCESS;
}
```

### Example 16 : Storing Strings in a List and Sorting

```c
// Function to compare two strings in the list
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"


static int compare_strings(const void* a, const void* b) 
{
    String* strA = *(String**)a;
    String* strB = *(String**)b;

    return string_is_less(strA, strB) ? -1 : string_is_greater(strA, strB);
}

int main() 
{
    List* stringList = list_create(sizeof(String*), compare_strings);

    // Add strings to the list
    String* str1 = string_create("Apple");
    String* str2 = string_create("Banana");
    String* str3 = string_create("Cherry");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Sort the list of strings
    list_sort(stringList);

    // Iterate and print strings
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) 
    {
        String* str = *(String**)node->value;
        printf("%s\n", string_c_str(str));
    }

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);

    list_deallocate(stringList);

    return EXIT_SUCCESS;
}
```

### Example 17: Merging Two Lists of Strings

#include "string/string.h"

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"


int main() 
{
    List* list1 = list_create(sizeof(String*), NULL);
    List* list2 = list_create(sizeof(String*), NULL);

    String* str1 = string_create("Hello");
    String* str2 = string_create("World");
    String* str3 = string_create("Example");
    String* str4 = string_create("Text");

    list_push_back(list1, &str1);
    list_push_back(list1, &str2);
    list_push_back(list2, &str3);
    list_push_back(list2, &str4);

    // Merge list2 into list1
    list_merge(list1, list2);

    // Iterate and print strings from merged list
    for (Node* node = list_begin(list1); node != list_end(list1); node = node->next) 
    {
        String* str = *(String**)node->value;
        printf("%s\n", string_c_str(str));
    }

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);
    string_deallocate(str4);

    list_deallocate(list1);
    list_deallocate(list2);

    return EXIT_SUCCESS;
}
```

### Example 18: Filtering Strings from a List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"

// Condition function to filter out short strings
static bool filter_short_strings(void* value) 
{
    String* str = *(String**)value;
    return string_length(str) < 5;
}

int main() 
{
    List* stringList = list_create(sizeof(String*), NULL);

    // Add strings to the list
    String* str1 = string_create("Apple");
    String* str2 = string_create("Banana");
    String* str3 = string_create("Kiwi");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Remove short strings
    list_remove_if(stringList, filter_short_strings);

    // Iterate and print remaining strings
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next) 
    {
        String* str = *(String**)node->value;
        printf("%s\n", string_c_str(str));
    }

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);

    list_deallocate(stringList);

    return EXIT_SUCCESS;
}
```

### Example 19: Concatenating All Strings in a List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"

int main()
{
    List* stringList = list_create(sizeof(String*), NULL);
    String* concatenated = string_create("");

    // Add strings to the list
    String* str1 = string_create("Hello, ");
    String* str2 = string_create("world");
    String* str3 = string_create("!");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Concatenate all strings
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next)
    {
        String* str = *(String**)node->value;
        string_append(concatenated, string_c_str(str));
    }

    printf("Concatenated String: %s\n", string_c_str(concatenated));

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3); 

    string_deallocate(concatenated);
    list_deallocate(stringList);

    return EXIT_SUCCESS;
}
```

### Example 20: Reversing Each String in a List

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"
#include "string/string.h"

int main() 
{
    List* stringList = list_create(sizeof(String*), NULL);

    // Add strings to the list
    String* str1 = string_create("Hello");
    String* str2 = string_create("World");
    String* str3 = string_create("Example");

    list_push_back(stringList, &str1);
    list_push_back(stringList, &str2);
    list_push_back(stringList, &str3);

    // Reverse each string
    for (Node* node = list_begin(stringList); node != list_end(stringList); node = node->next)
    {
        String* str = *(String**)node->value;
        String* reversed = string_create(""); 

        for (int i = string_length(str) - 1; i >= 0; --i) 
            string_push_back(reversed, string_c_str(str)[i]);
        
        printf("Reversed String: %s\n", string_c_str(reversed));

        string_deallocate(reversed);
    }

    // Deallocate and clean up
    string_deallocate(str1);
    string_deallocate(str2);
    string_deallocate(str3);

    list_deallocate(stringList);

    return EXIT_SUCCESS;
}

```

## Example 21 : Relational operators in List 

```c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list/list.h"

// Function to compare integers for the list
static int int_compare(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    return (int_a > int_b) - (int_a < int_b);
}

// Function to add elements to a list
void add_elements_to_list(List* list, int* elements, size_t numElements) 
{
    for (size_t i = 0; i < numElements; ++i) 
        list_push_back(list, &elements[i]);
    
}

int main() 
{
    List* list1 = list_create(sizeof(int), int_compare);
    List* list2 = list_create(sizeof(int), int_compare);

    // Add elements to the lists
    int elements1[] = {1, 2, 3, 4, 5};
    int elements2[] = {1, 2, 3, 4, 6};

    add_elements_to_list(list1, elements1, 5);
    add_elements_to_list(list2, elements2, 5);

    // Perform relational comparisons
    printf("List 1 is less than List 2: %s\n", list_is_less(list1, list2) ? "true" : "false");
    printf("List 1 is greater than List 2: %s\n", list_is_greater(list1, list2) ? "true" : "false");
    printf("List 1 is equal to List 2: %s\n", list_is_equal(list1, list2) ? "true" : "false");
    printf("List 1 is less than or equal to List 2: %s\n", list_is_less_or_equal(list1, list2) ? "true" : "false");
    printf("List 1 is greater than or equal to List 2: %s\n", list_is_greater_or_equal(list1, list2) ? "true" :"false");
    printf("List 1 is not equal to List 2: %s\n", list_is_not_equal(list1, list2) ? "true" : "false");

    // Deallocate and clean up
    list_deallocate(list1);
    list_deallocate(list2);

    return EXIT_SUCCESS;
}


```