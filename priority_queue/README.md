
# Queue Library

The Priority_Queue library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size Priority_Queues, offering similar functionality to `std::priority_queue` in C++.

## Compilation

To compile the Priority_Queue library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -o main ./main.c priority_queue/priority_queue.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Priority_Queue library in your project, include the `priority_queue.h` header file in your source code.

```c
#include "priority_queue/priority_queue.h"
```


## Example 1 : How to create pq and 'push'

```c

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

if (!pq) 
{
    fprintf(stderr, "Failed to create priority queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the priority queue
int values[] = {5, 10, 3, 7, 4};
for (int i = 0; i < 5; ++i) 
    pq->push(pq, &values[i]);

printf("Priority Queue size: %zu\n", pq->size(pq));

pq->deallocate(pq);

```


## Example 2 : get 'size' and check is 'empty' or not 

```c

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

if (!pq) 
{
    fprintf(stderr, "Failed to create priority queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the priority queue
int values[] = {5, 10, 3, 7, 4};
for (int i = 0; i < 5; ++i) 
    pq->push(pq, &values[i]);

printf("Priority Queue size: %zu\n", pq->size(pq));
printf("Is the priority queue empty? %s\n", pq->empty(pq) ? "Yes" : "No");
   
pq->deallocate(pq);

```


## Example 3 : get 'top' element

```c

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

if (!pq) 
{
    fprintf(stderr, "Failed to create priority queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the priority queue
int values[] = {5, 10, 3, 7, 4};
for (int i = 0; i < 5; ++i) 
    pq->push(pq, &values[i]);

int* top = pq->top(pq);
if (top) 
    printf("Top element: %d\n", *top);
   
pq->deallocate(pq);

```

## Example 4 : get 'top' element

```c

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

if (!pq) 
{
    fprintf(stderr, "Failed to create priority queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the priority queue
int values[] = {5, 10, 3, 7, 4};
for (int i = 0; i < 5; ++i) 
    pq->push(pq, &values[i]);

int* top = pq->top(pq);
if (top) 
    printf("Top element: %d\n", *top);

// Pop the top element and access the new top
pq->pop(pq);
top = pq->top(pq);

if (top) 
    printf("New top element after pop: %d\n", *top);

pq->deallocate(pq);

```