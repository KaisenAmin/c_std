
# Queue Library in C

Author: amin tahmasebi
Release Date: 2023
License: ISC License

The Priority_Queue library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size Priority_Queues, offering similar functionality to `std::priority_queue` in C++.

## Compilation

To compile the Priority_Queue library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c priority_queue/priority_queue.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Priority_Queue library in your project, include the `priority_queue.h` header file in your source code.

```c
#include "priority_queue/priority_queue.h"
```


## Example 1 : How to create pq and `priority_queuepush`

```c

#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

    if (!pq) {
        fmt_fprintf(stderr, "Failed to create priority queue.\n");
        return -1;
    }

    // Push some integers onto the priority queue
    int values[] = {5, 10, 3, 7, 4};
    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }
    fmt_printf("Priority Queue size: %zu\n", priority_queue_size(pq));
    
    priority_queue_deallocate(pq);
    return 0;
}
```

## Example 2 : get size with `priority_queue_size` and check is pq is empty or not with `priority_queue_empty`  

```c

#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

    if (!pq) {
        fmt_fprintf(stderr, "Failed to create priority queue.\n");
        return -1;
    }

    // Push some integers onto the priority queue
    int values[] = {5, 10, 3, 7, 4};
    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    fmt_printf("Priority Queue size: %zu\n", priority_queue_size(pq));
    fmt_printf("Is the priority queue empty? %s\n", priority_queue_empty(pq) ? "Yes" : "No");
    
    priority_queue_deallocate(pq);
    return 0;
}

```

## Example 3 : get top element with `priority_queue_top`

```c

#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

    if (!pq) {
        fmt_fprintf(stderr, "Failed to create priority queue.\n");
        return -1;
    }

    // Push some integers onto the priority queue
    int values[] = {5, 10, 3, 7, 4};
    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    int* top = priority_queue_top(pq);
    if (top) {
        fmt_printf("Top element: %d\n", *top);
    }

    priority_queue_deallocate(pq);
    return 0;
}

```

## Example 4 : get 'top' element

```c

#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main() {
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

    if (!pq) {
        fmt_fprintf(stderr, "Failed to create priority queue.\n");
        return -1;
    }

    // Push some integers onto the priority queue
    int values[] = {5, 10, 3, 7, 4};
    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    int* top = priority_queue_top(pq);
    if (top) {
        fmt_printf("Top element: %d\n", *top);
    }
    // Pop the top element and access the new top
    priority_queue_pop(pq);
    top = priority_queue_top(pq);

    if (top) {
        fmt_printf("New top element after pop: %d\n", *top);
    }

    priority_queue_deallocate(pq);
    return 0;
}
```

## Example 5 : Sorting Integers in Desc orders 

```c
#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints_desc(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    
    return (int_b > int_a) - (int_b < int_a);  // For descending order, reverse the comparison logic
}

int main() {
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints_desc);
 
    if (!pq) {
        fmt_fprintf(stderr, "Failed to create priority queue.\n");
        return -1;
    }

    // Push some integers onto the priority queue
    int values[] = {5, 10, 3, 7, 4, 15, 8};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    fmt_printf("Sorted elements in descending order:\n");
    while (!priority_queue_empty(pq)) {
        int* top = priority_queue_top(pq);
        if (top) {
            fmt_printf("%d ", *top);
        }
        priority_queue_pop(pq);
    }
    fmt_printf("\n");

    priority_queue_deallocate(pq);
    return 0;
}
```

## Example 6: Merging Two Priority Queues

```c
#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

void merge_priority_queues(PriorityQueue* destination, PriorityQueue* source) {
    while (!priority_queue_empty(source)) {
        int* top = priority_queue_top(source);

        priority_queue_push(destination, top);
        priority_queue_pop(source);
    }
}

int main() {
    PriorityQueue* pq1 = priority_queue_create(sizeof(int), compare_ints);
    PriorityQueue* pq2 = priority_queue_create(sizeof(int), compare_ints);
    PriorityQueue* mergedPQ = priority_queue_create(sizeof(int), compare_ints);

    if (!pq1 || !pq2 || !mergedPQ) {
        fmt_fprintf(stderr, "Failed to create priority queues.\n");
        return -1;
    }

    int values1[] = {1, 3, 5, 7, 9};
    for (size_t i = 0; i < sizeof(values1) / sizeof(values1[0]); ++i) {
        priority_queue_push(pq1, &values1[i]);
    }

    int values2[] = {2, 4, 6, 8, 10};
    for (size_t i = 0; i < sizeof(values2) / sizeof(values2[0]); ++i) {
        priority_queue_push(pq2, &values2[i]);
    }

    // Merge both queues into the third one
    merge_priority_queues(mergedPQ, pq1);
    merge_priority_queues(mergedPQ, pq2);

    fmt_printf("Merged Priority Queue:\n");
    while (!priority_queue_empty(mergedPQ)) {
        int* top = priority_queue_top(mergedPQ);
        fmt_printf("%d ", *top);

        priority_queue_pop(mergedPQ);
    }

    fmt_printf("\n");

    priority_queue_deallocate(pq1);
    priority_queue_deallocate(pq2);
    priority_queue_deallocate(mergedPQ);

    return 0;
}

```

## Example 7: Using Priority Queue for Task Scheduling

```c
#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

typedef struct Task Task;

struct Task {
    int taskID;
    int priority;
};

static int compare_tasks(const void* a, const void* b) {
    const Task* taskA = a;
    const Task* taskB = b;

    return (taskA->priority > taskB->priority) - (taskA->priority < taskB->priority);
}

int main() {
    PriorityQueue* taskQueue = priority_queue_create(sizeof(Task), compare_tasks);

    if (!taskQueue) {
        fmt_fprintf(stderr, "Failed to create task queue.\n");
        return -1;
    }

    // Define some tasks with different priorities
    Task tasks[] = {
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2},
    };

    for (size_t i = 0; i < sizeof(tasks) / sizeof(tasks[0]); ++i) { 
        priority_queue_push(taskQueue, &tasks[i]);
    }

    fmt_printf("Executing tasks in priority order:\n");
    while (!priority_queue_empty(taskQueue)) {
        Task* topTask = priority_queue_top(taskQueue);
        fmt_printf("Executing Task ID: %d, Priority: %d\n", topTask->taskID, topTask->priority);
        priority_queue_pop(taskQueue);
    }

    priority_queue_deallocate(taskQueue);
    return 0;
}

```