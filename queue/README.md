
# Queue Library

The Queue library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size Queues, offering similar functionality to `std::queue` in C++.

## Compilation

To compile the Queue library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -o main ./main.c ./queue/queue.c 
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Queue library in your project, include the `queue.h` header file in your source code.

```c
#include "queue/queue.h"
```

## Example 1 : how to create queue and 'push_back' then get 'size'

```c
Queue* myQueue = queue_create(sizeof(int));

if (!myQueue) 
{
    fprintf(stderr, "Failed to create queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the queue
int values[] = {10, 20, 30, 40, 50};
for (int i = 0; i < 5; ++i) 
    myQueue->push(myQueue, &values[i]);
    
// Print the size of the queue
printf("Queue size: %zu\n", myQueue->size(myQueue));

myQueue->deallocate(myQueue);

```

## Example 2 : check queue is 'empty' or not

```c
Queue* myQueue = queue_create(sizeof(int));

if (!myQueue) 
{
    fprintf(stderr, "Failed to create queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the queue
int values[] = {10, 20, 30, 40, 50};
for (int i = 0; i < 5; ++i) 
    myQueue->push(myQueue, &values[i]);
    
printf("Is the queue empty? %s\n", myQueue->empty(myQueue) ? "Yes" : "No");

myQueue->deallocate(myQueue);

```

## Example 32 : 'front' and 'back' get ref to first and last element

```c
Queue* myQueue = queue_create(sizeof(int));

if (!myQueue) 
{
    fprintf(stderr, "Failed to create queue.\n");
    return EXIT_FAILURE;
}

// Push some integers onto the queue
int values[] = {10, 20, 30, 40, 50};
for (int i = 0; i < 5; ++i) 
    myQueue->push(myQueue, &values[i]);
    
    // Access the front and back elements
int* front = myQueue->front(myQueue);
int* back = myQueue->back(myQueue);

if (front && back) 
{
    printf("Front element: %d\n", *front);
    printf("Back element: %d\n", *back);

    *(int*)myQueue->back(myQueue) = 15236;
    printf("Back element After change: %d\n", *(int*)myQueue->back(myQueue));
}

myQueue->deallocate(myQueue);

```