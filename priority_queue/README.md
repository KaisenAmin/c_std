# Priority Queue Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

The Priority Queue library is part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic-sized priority queues, offering similar functionality to `std::priority_queue` in C++.

## Compilation

To compile the Priority Queue library along with your main program, use the following GCC command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./priority_queue/priority_queue.c 
```

If you need other libraries, you can add them by including their `.c` files.

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Priority Queue library in your project, include the `priority_queue.h` header file in your source code:

```c
#include "priority_queue/priority_queue.h"
```

## Functions Descriptions 

### Priority Queue Creation and Deallocation

### `PriorityQueue* priority_queue_create(size_t itemSize, int (*compare)(const void*, const void*));`
- **Purpose**: Creates a new Priority Queue object. The size of each item in the queue and a comparison function to determine the priority of elements must be specified.
- **Parameters:**
    - `itemSize`: The size of the items that will be stored in the priority queue.
    - `compare`: A pointer to the comparison function used to determine the order of elements.
- **Returns:** A pointer to the newly created Priority Queue object.

---

### `void priority_queue_deallocate(PriorityQueue* pq);` 
- **Purpose**: Deallocates a Priority Queue object, freeing all associated memory.
- **Parameters:**
    - `pq`: Pointer to the Priority Queue object to be deallocated.

---

### `void priority_queue_push(PriorityQueue* pq, void* item);` 
- **Purpose**: Adds an item to the priority queue, maintaining the heap property.
- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.
    - `item`: Pointer to the item to be added to the queue.

---

### `void priority_queue_pop(PriorityQueue* pq);` 
- **Purpose**: Removes the top (highest priority) item from the priority queue.
- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.

---

### `void* priority_queue_top(const PriorityQueue* pq);` 
- **Purpose**: Returns a pointer to the top item of the priority queue without removing it.
- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.
- **Returns:** A pointer to the top item or `NULL` if the queue is empty.

---

### `bool priority_queue_empty(const PriorityQueue* pq);` 
- **Purpose**: Checks if the priority queue is empty.
- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.
- **Returns:** `true` if the priority queue is empty, otherwise `false`.

---

### `size_t priority_queue_size(const PriorityQueue* pq);` 
- **Purpose**: Returns the number of items currently in the priority queue.
- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.
- **Returns:** The number of items in the priority queue.

---

### `void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2);` 
- **Purpose**: Swaps the contents of two priority queues.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.

---

### `bool priority_queue_is_equal(const PriorityQueue* pq1, const PriorityQueue* pq2);` 
- **Purpose**: Compares two priority queues to determine if they are equal.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.
- **Returns:** `true` if the priority queues are equal (contain the same elements in the same order), otherwise `false`.

---

### `bool priority_queue_is_less(const PriorityQueue* pq1, const PriorityQueue* pq2);` 
- **Purpose**: Compares two priority queues to determine if the first queue is less than the second.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.
- **Returns:** `true` if the first priority queue is less than the second (lexicographical comparison), otherwise `false`.

---

### `bool priority_queue_is_greater(const PriorityQueue* pq1, const PriorityQueue* pq2);` 
- **Purpose**: Compares two priority queues to determine if the first queue is greater than the second.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.
- **Returns:** `true` if the first priority queue is greater than the second (lexicographical comparison), otherwise `false`.

---

### `bool priority_queue_is_not_equal(const PriorityQueue* pq1, const PriorityQueue* pq2);` 
- **Purpose**: Compares two priority queues to determine if they are not equal.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.
- **Returns:** `true` if the priority queues are not equal, otherwise `false`.

---

### `bool priority_queue_is_less_or_equal(const PriorityQueue* pq1, const PriorityQueue* pq2);` 
- **Purpose**: Compares two priority queues to determine if the first queue is less than or equal to the second.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.
- **Returns:** `true` if the first priority queue is less than or equal to the second, otherwise `false`.

---

### `bool priority_queue_is_greater_or_equal(const PriorityQueue* pq1, const PriorityQueue* pq2);` 
- **Purpose**: Compares two priority queues to determine if the first queue is greater than or equal to the second.
- **Parameters:**
    - `pq1`: Pointer to the first Priority Queue object.
    - `pq2`: Pointer to the second Priority Queue object.
- **Returns:** `true` if the first priority queue is greater than or equal to the second, otherwise `false`.

---

### `void* priority_queue_front(const PriorityQueue* pq);` 
- **Purpose**: Returns a pointer to the front item of the priority queue without removing it. In the context of a priority queue, the "front" typically refers to the element with the highest priority.

- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.
- **Returns:** A pointer to the front (highest priority) item, or `NULL` if the queue is empty.

---

### `void* priority_queue_back(const PriorityQueue* pq);` 
- **Purpose**: Returns a pointer to the back item of the priority queue without removing it. While a priority queue is typically accessed through its "front" (highest priority element), this function can be used to access the "least priority" item.

- **Parameters:**
    - `pq`: Pointer to the Priority Queue object.
- **Returns:** A pointer to the back (lowest priority) item, or `NULL` if the queue is empty.

---

### Example 1: How to Create a Priority Queue and Push Elements

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
    int values[] = {5, 10, 3, 7, 4};
    
    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    fmt_printf("Priority Queue size: %zu\n", priority_queue_size(pq));
    
    priority_queue_deallocate(pq);
    return 0;
}
```

**Result:**
```
Priority Queue size: 5
```

---

### Example 2: Get Size with `priority_queue_size` and Check if Queue is Empty

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

**Result:**
```
Priority Queue size: 5
Is the priority queue empty? No
```

---

### Example 3: Get Top Element with `priority_queue_top`

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
    int values[] = {5, 10, 3, 7, 4};

    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    int* top = (int*)priority_queue_top(pq);
    if (top) {
        fmt_printf("Top element: %d\n", *top);
    }

    priority_queue_deallocate(pq);
    return 0;
}
```

**Result:**
```
Top element: 10
```

---

### Example 4: Get Top Element and Pop

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
    int values[] = {5, 10, 3, 7, 4};

    for (int i = 0; i < 5; ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    int* top = (int*)priority_queue_top(pq);
    if (top) {
        fmt_printf("Top element: %d\n", *top);
    }
    // Pop the top element and access the new top
    priority_queue_pop(pq);
    top = (int*)priority_queue_top(pq);

    if (top) {
        fmt_printf("New top element after pop: %d\n", *top);
    }

    priority_queue_deallocate(pq);
    return 0;
}
```

**Result:**
```
Top element: 10
New top element after pop: 7
```

---

### Example 5: Sorting Integers in Descending Order

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
    int values[] = {5, 10, 3, 7, 4, 15, 8};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) { 
        priority_queue_push(pq, &values[i]);
    }

    fmt_printf("Sorted elements in descending order:\n");
    while (!priority_queue_empty(pq)) {
        int* top = (int*)priority_queue_top(pq);
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

**Result:**
```
Sorted elements in descending order:
3 4 5 7 8 10 15
```

---

### Example 6: Merging Two Priority Queues

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
        int* top = (int*)priority_queue_top(source);

        priority_queue_push(destination, top);
        priority_queue_pop(source);
    }
}

int main() {
    PriorityQueue* pq1 = priority_queue_create(sizeof(int), compare_ints);
    PriorityQueue* pq2 = priority_queue_create(sizeof(int), compare_ints);
    PriorityQueue* mergedPQ = priority_queue_create(sizeof(int), compare_ints);
    int values1[] = {1, 3, 5, 7, 9};
    int values2[] = {2, 4, 6, 8, 10};

    for (size_t i = 0; i < sizeof(values1) / sizeof(values1[0]); ++i) {
        priority_queue_push(pq1, &values1[i]);
    }
    for (size_t i = 0; i < sizeof(values2) / sizeof(values2[0]); ++i) {
        priority_queue_push(pq2, &values2[i]);
    }

    // Merge both queues into the third one
    merge_priority_queues(mergedPQ, pq1);
    merge_priority_queues(mergedPQ, pq2);

    fmt_printf("Merged Priority Queue:\n");
    while (!priority_queue_empty(mergedPQ)) {
        int* top = (int*)priority_queue_top(mergedPQ);
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

**Result:**
```
Merged Priority Queue:
10 9 8 7 6 5 4 3 2 1 
```

---

### Example 7: Using Priority Queue for Task Scheduling

```c
#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

typedef struct Task Task;

struct Task {
    int taskID;
    int priority;
};

static int compare_tasks(const void* a, const void* b) {
    const Task* taskA = (const Task*)a;
    const Task* taskB = (const Task*)b;

    return (taskA->priority > taskB->priority) - (taskA->priority < taskB->priority);
}


int main() {
    PriorityQueue* taskQueue = priority_queue_create(sizeof(Task), compare_tasks);

    // Define some tasks with different priorities
    Task tasks[] = {
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2}
    };

    for (size_t i = 0; i < sizeof(tasks) / sizeof(tasks[0]); ++i) { 
        priority_queue_push(taskQueue, &tasks[i]);
    }

    fmt_printf("Executing tasks in priority order:\n");
    while (!priority_queue_empty(taskQueue)) {
        Task* topTask = (Task*)priority_queue_top(taskQueue);
        fmt_printf("Executing Task ID: %d, Priority: %d\n", topTask->taskID, topTask->priority);
        priority_queue_pop(taskQueue);
    }

    priority_queue_deallocate(taskQueue);
    return 0;
}
```

**Result:**
```
Executing tasks in priority order:
Executing Task ID: 101, Priority: 3
Executing Task ID: 104, Priority: 3
Executing Task ID: 102, Priority: 2
Executing Task ID: 105, Priority: 2
Executing Task ID: 103, Priority: 1
```

---

## License

This project is open-source and available under [ISC License].