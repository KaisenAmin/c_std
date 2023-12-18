
# Queue Library

The Queue library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size Queues, offering similar functionality to `std::queue` in C++.

## Compilation

To compile the Queue library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./queue/queue.c 
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

## Example 3 : 'front' and 'back' get ref to first and last element

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

## Example 4 : 'pop' element

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

myQueue->pop(myQueue);
front = myQueue->front(myQueue);

if (front) 
    printf("New front element after pop: %d\n", *front);

myQueue->deallocate(myQueue);

```

## Example 5 : 'emplace' element also use some relationals operators

```c
Queue* myQueue1 = queue_create(sizeof(int));
Queue* myQueue2 = queue_create(sizeof(int));

if (!myQueue1 || !myQueue2) 
{
    printf("Failed to create queue\n");
    return EXIT_FAILURE;
}

// Push some integers onto the first queue
int values1[] = {10, 20, 30, 40, 50};
for (int i = 0; i < 5; ++i) 
    myQueue1->push(myQueue1, &values1[i]);
    
int values2[] = {15, 25, 35, 45, 55};
for (int i = 0; i < 5; ++i) 
    myQueue2->emplace(myQueue2, &values2[i], sizeof(int));
    
    
// // Compare the two queues
printf("Are the queues equal? %s\n", myQueue1->is_equal(myQueue1, myQueue2) ? "Yes" : "No");
printf("Is myQueue1 less than myQueue2? %s\n", myQueue1->is_less(myQueue1, myQueue2) ? "Yes" : "No");

myQueue1->deallocate(myQueue1);
myQueue2->deallocate(myQueue2);
```

## Example 6 : how to 'swap' Queue

```c
Queue* myQueue1 = queue_create(sizeof(int));
Queue* myQueue2 = queue_create(sizeof(int));

if (!myQueue1 || !myQueue2) 
{
    printf("Failed to create queue\n");
    return EXIT_FAILURE;
}

// Push some integers onto the first queue
int values1[] = {10, 20, 30, 40, 50};
for (int i = 0; i < 5; ++i) 
    myQueue1->push(myQueue1, &values1[i]);
    
int values2[] = {15, 25, 35, 45, 55};
for (int i = 0; i < 5; ++i) 
    myQueue2->emplace(myQueue2, &values2[i], sizeof(int));
    
// Swap the two queues
myQueue1->swap(myQueue1, myQueue2);

// Check the front element of the swapped queues
int* front1 = myQueue1->front(myQueue1);
int* front2 = myQueue2->front(myQueue2);
if (front1 && front2) 
{
    printf("Front element of myQueue1 after swap: %d\n", *front1);
    printf("Front element of myQueue2 after swap: %d\n", *front2);
}

myQueue1->deallocate(myQueue1);
myQueue2->deallocate(myQueue2);

```

## Example 7 : Task Management System Using Queue

```c

#include <stdio.h>
#include <string.h>
#include "queue/queue.h"

typedef struct Task 
{
    int id;
    char description[100];
    int priority;

} Task;

void addTask(Queue* queue, int id, const char* desc, int priority) 
{
    Task task;
    task.id = id;

    strncpy(task.description, desc, sizeof(task.description));
    task.priority = priority;
    queue->emplace(queue, &task, sizeof(Task));
}

void processTasks(Queue* queue) 
{
    while (!queue->empty(queue)) 
    {
        Task* task = (Task*)queue->front(queue);
        printf("Processing Task ID %d: %s\n", task->id, task->description);

        // Simulate task processing
        queue->pop(queue);
    }
}

int main() 
{
    Queue* taskQueue = queue_create(sizeof(Task));

    if (!taskQueue) 
    {
        printf("Failed to create queue\n");
        return EXIT_FAILURE;
    }

    addTask(taskQueue, 1, "Fix bug in code", 5);
    addTask(taskQueue, 2, "Update documentation", 3);
    addTask(taskQueue, 3, "Refactor module", 4);

    printf("Task queue size before processing: %zu\n", taskQueue->size(taskQueue));
    processTasks(taskQueue);
    printf("Task queue size after processing: %zu\n", taskQueue->size(taskQueue));

    taskQueue->deallocate(taskQueue);

    return EXIT_SUCCESS;
}

```

## Example 8 : 2D Queue of String Objects

```c

#include <stdio.h>
#include "queue/queue.h"
#include "string/string.h"

int main() 
{
    // Create a Queue of String Queues (2D Queue)
    Queue* queue2D = queue_create(sizeof(Queue*));

    // Create and populate inner Queues
    for (int i = 0; i < 3; ++i) 
    { 
        Queue* stringQueue = queue_create(sizeof(String*));

        // Add Strings to the inner Queue
        for (int j = 0; j < 5; ++j) 
        { 
            // Each inner Queue has 5 Strings
            char buffer[50];
            sprintf(buffer, "String %d-%d", i, j);

            String* str = string_create(buffer);
            stringQueue->emplace(stringQueue, &str, sizeof(String*));
        }

        // Add the inner Queue to the 2D Queue
        queue2D->emplace(queue2D, &stringQueue, sizeof(Queue*));
    }

    // Iterate over each inner Queue and process its Strings
    while (!queue2D->empty(queue2D)) 
    {
        Queue** innerQueuePtr = (Queue**)queue2D->front(queue2D);
        Queue* innerQueue = *innerQueuePtr;

        while (!innerQueue->empty(innerQueue)) 
        {
            String** strPtr = (String**)innerQueue->front(innerQueue);
            String* str = *strPtr;
            
            printf("Processing: %s\n", str->c_str(str));

            // Pop the processed String
            innerQueue->pop(innerQueue);
            str->deallocate(str);
        }

        // Pop the processed inner Queue
        queue2D->pop(queue2D);
        innerQueue->deallocate(innerQueue);
    }

    // Deallocate the outer Queue
    queue2D->deallocate(queue2D);

    return 0;
}


```