
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
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    list->push_front(list, &value);

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 2: Pop Front Element
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    list->push_front(list, &value);
    list->pop_front(list);

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 3: Access Front Element
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    list->push_front(list, &value);
    int *frontValue = list->front(list);

    printf("Front value: %d\n", *frontValue);

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 4: Clear List
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    list->assign(list, values, 3);
    list->clear(list);

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 5: Check if List is Empty
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));

    printf("Is list empty? %s\n", list->empty(list) ? "Yes" : "No");

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 6: Get List Length
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    list->assign(list, values, 3);
    printf("List length: %zu\n", list->length(list));

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 7: Assign Values to List
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    list->assign(list, values, 3);

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 8: Get before_begin Iterator
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    ForwardListNode *node = list->before_begin(list);

    printf("Node: %p\n", node);

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 9: Get begin and end Iterator
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    list->assign(list, values, 3);

    for (ForwardListNode *node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d\n", *(int *)node->value);
    
    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 10: Get max_size
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));

    printf("Max size: %zu\n", list->max_size(list));

    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 11: Emplace Front
```c
#include <stdio.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    list->emplace_front(list, &value);

    // Print list after emplace_front
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 12: Emplace After
```c
#include <stdio.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int value = 10;

    list->emplace_front(list, &value);
    list->emplace_after(list, list->before_begin(list), &value);

    // Print list after emplace_after
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 13: Insert After
```c
#include <stdio.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20};

    list->insert_after(list, list->before_begin(list), values, 2);

    // Print list after insert_after
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 14: Erase After
```c
#include <stdio.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    list->assign(list, values, 3);
    list->erase_after(list, list->begin(list));

    // Print list after erase
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 15: Swap Lists
```c
int main() 
{
    ForwardList* list1 = forward_list_create(sizeof(int));
    ForwardList* list2 = forward_list_create(sizeof(int));

    list1->swap(list1, list2);

    list1->deallocate(list1);
    list2->deallocate(list2);

    return EXIT_SUCCESS;
}
```

### Example 16: Resize List
```c
int main() {
    ForwardList* list = forward_list_create(sizeof(int));
    list->resize(list, 3);
    list->deallocate(list);

    return EXIT_SUCCESS;
}
```

### Example 17: Splice After
```c
#include <stdio.h>

int main() 
{
    ForwardList* list1 = forward_list_create(sizeof(int));
    ForwardList* list2 = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};

    list2->assign(list2, values, 3);
    list1->splice_after(list1, list1->before_begin(list), list2);

    // Print list1 after splice
    for (ForwardListNode* node = list1->begin(list1); node != list1->end(list1); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list1->deallocate(list1);
    list2->deallocate(list2);

    return EXIT_SUCCESS;
}

```

### Example 18: Remove Element
```c
#include <stdio.h>

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 20, 30};
    list->assign(list, values, 3);

    int valueToRemove = 20;
    list->remove(list, &valueToRemove);

    // Print list after remove
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 19: Remove Elements if Condition is Met
```c
#include <stdio.h>

bool is_even(void* value) 
{
    int val = *(int*)value;

    return val % 2 == 0;
}

int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 15, 20, 25, 30};

    list->assign(list, values, 5);
    list->remove_if(list, is_even);

    // Print list after remove_if
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

    return EXIT_SUCCESS;
}

```

### Example 20: Unique Elements
```c
int main() 
{
    ForwardList* list = forward_list_create(sizeof(int));
    int values[] = {10, 10, 20, 30, 30, 30};
    list->assign(list, values, 6);

    list->unique(list);

    // Print list after unique
    for (ForwardListNode* node = list->begin(list); node != list->end(list); node = node->next) 
        printf("%d ", *(int*)(node->value));
    
    printf("\n");

    list->deallocate(list);

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