
# ForwardList Library

The ForwardList library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size ForwardList, offering similar functionality to `std::forward_list` in C++.

## Compilation

To compile the ForwardList library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c forward_list/forward_list.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the ForwardList library in your project, include the `forward_list.h` header file in your source code.

```c
#include "forward_list/forward_list.h"
```


Creating 20 different examples to demonstrate the usage of all methods in your `ForwardList` structure will provide a comprehensive understanding of how each function operates. Here are the examples:

### Example 1: Create a ForwardList and Push Front
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) 
        printf("%d ", *(int*)(node->value));
    printf("\n");
    
    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 2: Pop Front Element
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);
    forward_list_pop_front(list);

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 3: Access Front Element
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_push_front(list, &value);
    int *frontValue = forward_list_front(list);

    printf("Front value: %d\n", *frontValue);

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 4: Clear List
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) 
        printf("%d ", *(int*)(node->value));
    printf("\n");

    forward_list_clear(list);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) 
        printf("%d ", *(int*)(node->value));
    printf("\n");
    
    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 5: Check if List is Empty
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));

    printf("Is list empty? %s\n", forward_list_empty(list) ? "Yes" : "No");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 6: Get List Length
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);
    printf("List length: %zu\n", forward_list_length(list));

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 7: Assign Values to List
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);

    for (ForwardListNode* node = forward_list_begin(list); node != NULL; node = node->next) 
        printf("%d ", *(int*)(node->value));
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 8: Get before_begin Iterator
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    ForwardListNode *node = forward_list_before_begin(list);

    printf("Node: %p\n", (void*)node);

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 9: Get begin and end Iterator
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);

    for (ForwardListNode *node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d\n", *(int *)node->value);
    
    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 10: Get max_size
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));

    printf("Max size: %zu\n", forward_list_max_size(list));

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 11: Emplace Front
```c
#include <stdio.h>

#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_emplace_front(list, &value);

    // Print list after emplace_front
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 12: Emplace After
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    forward_list_emplace_front(list, &value);
    forward_list_emplace_after(list, forward_list_before_begin(list), &value);

    // Print list after emplace_after
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 13: Insert After
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20};

    forward_list_insert_after(list, forward_list_before_begin(list), values, 2);

    // Print list after insert_after
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 14: Erase After
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list, values, 3);
    forward_list_erase_after(list, forward_list_begin(list));

    // Print list after erase
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

   forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 15: Swap Lists
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

void print_list(ForwardList *list) 
{
    ForwardListNode *current = forward_list_begin(list);
    while (current != NULL) 
    {
        int *value = (int *)current->value;
        printf("%d ", *value);
        current = current->next;
    }
    printf("\n");
}

int main() 
{
    ForwardList* list1 = forward_list_create(sizeof(int));
    ForwardList* list2 = forward_list_create(sizeof(int));

    // Initialize list1 with values
    int values1[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) 
        forward_list_push_front(list1, &values1[i]);

    // Initialize list2 with values
    int values2[] = {4, 5, 6};
    for (int i = 0; i < 3; i++) 
        forward_list_push_front(list2, &values2[i]);

    // Swap the lists
    forward_list_swap(list1, list2);

    // Print the lists after swap
    printf("List1 after swap: ");
    print_list(list1);

    printf("List2 after swap: ");
    print_list(list2);

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);

    return EXIT_SUCCESS;
}

```

### Example 16: Resize List
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    forward_list_resize(list, 3);

    printf("Size of the list after resizing: %zu\n", forward_list_length(list));
    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 17: Splice After
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list1 = forward_list_create(sizeof(int));
    ForwardList* list2 = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    forward_list_assign(list2, values, 3);
    forward_list_splice_after(list1, forward_list_before_begin(list1), list2);

    // Print list1 after splice
    for (ForwardListNode* node = forward_list_begin(list1); node != forward_list_end(list1); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);

    return EXIT_SUCCESS;
}

```

### Example 18: Remove Element
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>


int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};
    forward_list_assign(list, values, 3);

    int valueToRemove = 20;
    forward_list_remove(list, &valueToRemove);

    // Print list after remove
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 19: Remove Elements if Condition is Met
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

bool is_even(void* value) 
{
    int val = *(int*)value;

    return val % 2 == 0;
}

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 15, 20, 25, 30};

    forward_list_assign(list, values, 5);
    forward_list_remove_if(list, is_even);

    // Print list after remove_if
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 20: Unique Elements
```c
#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 10, 20, 30, 30, 30};
    forward_list_assign(list, values, 6);

    forward_list_unique(list);

    // Print list after unique
    for (ForwardListNode* node = forward_list_begin(list); node != forward_list_end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    forward_list_deallocate(list);

    return EXIT_SUCCESS;
}

```

## Example 21 : Relational operators 

```c

#include "forward_list/forward_list.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    ForwardList *list1 = forward_list_create(sizeof(int));
    ForwardList *list2 = forward_list_create(sizeof(int));

    int values1[] = {1, 2, 3, 4, 5};
    for (size_t i = 0; i < sizeof(values1) / sizeof(values1[0]); i++) 
        forward_list_push_front(list1, &values1[i]);
    
    int values2[] = {1, 2, 3, 4, 5, 6}; 
    for (size_t i = 0; i < sizeof(values2) / sizeof(values2[0]); i++) 
        forward_list_push_front(list2, &values2[i]);
    
    if (forward_list_is_less(list1, list2)) 
        printf("List1 is less than List2\n");
    else if (forward_list_is_greater(list1, list2)) 
        printf("List1 is greater than List2\n");
    else if (forward_list_is_equal(list1, list2)) 
        printf("List1 is equal to List2\n");
    

    if (forward_list_is_not_equal(list1, list2)) 
        printf("List1 is not equal to List2\n");
    

    forward_list_deallocate(list1);
    forward_list_deallocate(list2);

    return EXIT_SUCCESS;
}

```