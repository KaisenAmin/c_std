
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