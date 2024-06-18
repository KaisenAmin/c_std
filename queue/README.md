
# Queue Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License

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

## Example 1 : how to create queue and `queue_push_back` then get `queue_size`

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));

    if (!myQueue) {
        fmt_fprintf(stderr, "Failed to create queue.\n");
        return -1;
    }

    // Push some integers onto the queue
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }

    fmt_printf("Queue size: %zu\n", queue_size(myQueue));
    
    queue_deallocate(myQueue);
    return 0;
}
```
**Result:**
```
Queue size: 5
```

## Example 2 : check queue is `queue_empty` or not

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));

    if (!myQueue) {
        fmt_fprintf(stderr, "Failed to create queue.\n");
        return -1;
    }

    // Push some integers onto the queue
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }
    fmt_printf("Is the queue empty? %s\n", queue_empty(myQueue) ? "Yes" : "No");

    queue_deallocate(myQueue);
    return 0;
}
```
**Result:**
```
Is the queue empty? No
```

## Example 3 : `queue_front` and `queue_back` get ref to first and last element

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));

    if (!myQueue) {
        fmt_fprintf(stderr, "Failed to create queue.\n");
        return -1;
    }

    // Push some integers onto the queue
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }
    // Access the front and back elements
    int* front = queue_front(myQueue);
    int* back = queue_back(myQueue);

    if (front && back) {
        fmt_printf("Front element: %d\n", *front);
        fmt_printf("Back element: %d\n", *back);

        *(int*)queue_back(myQueue) = 15236;
        fmt_printf("Back element After change: %d\n", *(int*)queue_back(myQueue));
    }

    queue_deallocate(myQueue);
    return 0;
}
```
**Result:**
```
Front element: 10
Back element: 50
Back element After change: 15236
```

## Example 4 : how to pop element with `queue_pop`

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));

    if (!myQueue) {
        fmt_fprintf(stderr, "Failed to create queue.\n");
        return -1;
    }

    // Push some integers onto the queue
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }
    queue_pop(myQueue);
    int* front = queue_front(myQueue);

    if (front) {
        fmt_printf("New front element after pop: %d\n", *front);
    }

    queue_deallocate(myQueue);
    return 0;
}
```
**Result:**
```
New front element after pop: 20
```

## Example 5 : `queue_emplace` element also use some relationals operators

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue1 = queue_create(sizeof(int));
    Queue* myQueue2 = queue_create(sizeof(int));

    if (!myQueue1 || !myQueue2) {
        fmt_fprintf(stderr, "Failed to create queue\n");
        return -1;
    }

    // Push some integers onto the first queue
    int values1[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) {
        queue_push(myQueue1, &values1[i]);
    }

    int values2[] = {15, 25, 35, 45, 55};
    for (int i = 0; i < 5; ++i) { 
        queue_emplace(myQueue2, &values2[i], sizeof(int));
    }
 
    fmt_printf("Are the queues equal? %s\n", queue_is_equal(myQueue1, myQueue2) ? "Yes" : "No");
    fmt_printf("Is myQueue1 less than myQueue2? %s\n", queue_is_less(myQueue1, myQueue2) ? "Yes" : "No");

    queue_deallocate(myQueue1);
    queue_deallocate(myQueue2);

    return 0;
}
```
**Result:**
```
Are the queues equal? No
Is myQueue1 less than myQueue2? Yes
```

## Example 6 : how to use `queue_swap` in Queue

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue1 = queue_create(sizeof(int));
    Queue* myQueue2 = queue_create(sizeof(int));

    if (!myQueue1 || !myQueue2) {
        fmt_fprintf(stderr, "Failed to create queue\n");
        return -1;
    }

    int values1[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) {
        queue_push(myQueue1, &values1[i]);
    }

    int values2[] = {15, 25, 35, 45, 55};
    for (int i = 0; i < 5; ++i) { 
        queue_emplace(myQueue2, &values2[i], sizeof(int));
    }

    queue_swap(myQueue1, myQueue2);

    // Check the front element of the swapped queues
    int* front1 = queue_front(myQueue1);
    int* front2 = queue_front(myQueue2);

    if (front1 && front2) {
        fmt_printf("Front element of myQueue1 after swap: %d\n", *front1);
        fmt_printf("Front element of myQueue2 after swap: %d\n", *front2);
    }

    queue_deallocate(myQueue1);
    queue_deallocate(myQueue2);

    return 0;
}
```
**Result:**
```
Front element of myQueue1 after swap: 15
Front element of myQueue2 after swap: 10
```

## Example 7 : Task Management System Using Queue

```c
#include <string.h>
#include "fmt/fmt.h"
#include "queue/queue.h"

typedef struct Task {
    int id;
    char description[100];
    int priority;
} Task;

void addTask(Queue* queue, int id, const char* desc, int priority) {
    Task task;
    task.id = id;

    strncpy(task.description, desc, sizeof(task.description));
    task.priority = priority;
    queue_emplace(queue, &task, sizeof(Task));
}

void processTasks(Queue* queue) {
    while (!queue_empty(queue)) {
        Task* task = (Task*)queue_front(queue);
        fmt_printf("Processing Task ID %d: %s\n", task->id, task->description);
        // Simulate task processing
        queue_pop(queue);
    }
}

int main() {
    Queue* taskQueue = queue_create(sizeof(Task));

    addTask(taskQueue, 1, "Fix bug in code", 5);
    addTask(taskQueue, 2, "Update documentation", 3);
    addTask(taskQueue, 3, "Refactor module", 4);
    addTask(taskQueue, 4, "Fix bug in code", 5);
    addTask(taskQueue, 5, "Update documentation", 3);
    addTask(taskQueue, 6, "Refactor module", 4);

    fmt_printf("Task queue size before processing: %zu\n", queue_size(taskQueue));
    processTasks(taskQueue);
    fmt_printf("Task queue size after processing: %zu\n", queue_size(taskQueue));

    queue_deallocate(taskQueue);
    return 0;
}
```
**Result:**
```
Task queue size before processing: 6
Processing Task ID 1: Fix bug in code
Processing Task ID 2: Update documentation
Processing Task ID 3: Refactor module
Processing Task ID 4: Fix bug in code
Processing Task ID 5: Update documentation
Processing Task ID 6: Refactor module
Task queue size after processing: 0
```

## Example 8 : 2D Queue of String Objects

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include "queue/queue.h"
#include <stdlib.h>

int main() {
    // Create a Queue of String Queues (2D Queue)
    Queue* queue2D = queue_create(sizeof(Queue*));

    // Create and populate inner Queues
    for (int i = 0; i < 3; ++i) { 
        Queue* stringQueue = queue_create(sizeof(String*));

        // Add Strings to the inner Queue
        for (int j = 0; j < 5; ++j) { 
            // Each inner Queue has 5 Strings
            char *buffer = NULL;
            buffer = fmt_sprintf("String %d-%d", i, j);
            

            String* str = string_create(buffer);
            queue_emplace(stringQueue, &str, sizeof(String*));
            free(buffer);
        }

        // Add the inner Queue to the 2D Queue
        queue_emplace(queue2D, &stringQueue, sizeof(Queue*));
    }

    // Iterate over each inner Queue and process its Strings
    while (!queue_empty(queue2D)) {
        Queue** innerQueuePtr = (Queue**)queue_front(queue2D);
        Queue* innerQueue = *innerQueuePtr;

        while (!queue_empty(innerQueue)) {
            String** strPtr = (String**)queue_front(innerQueue);
            String* str = *strPtr;
            
            fmt_printf("Processing: %s\n", string_c_str(str));

            // Pop the processed String
            queue_pop(innerQueue);
            string_deallocate(str);
        }

        // Pop the processed inner Queue
        queue_pop(queue2D);
        queue_deallocate(innerQueue);
    }

    queue_deallocate(queue2D);
    return 0;
}
```
**Result:**
```
Processing: String 0-0
Processing: String 0-1
Processing: String 0-2
Processing: String 0-3
Processing: String 0-4
Processing: String 1-0
Processing: String 1-1
Processing: String 1-2
Processing: String 1-3
Processing: String 1-4
Processing: String 2-0
Processing: String 2-1
Processing: String 2-2
Processing: String 2-3
Processing: String 2-4
```
