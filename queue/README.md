# Queue Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

The Queue library is part of a project aimed at reimplementing C++ standard library features in C. It provides a generic container that encapsulates dynamic-sized Queues, offering functionality similar to `std::queue` in C++.

## Compilation

To compile the Queue library along with your main program, use the following GCC command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./queue/queue.c 
```

If you need other libraries, you can add them by including their `.c` files.

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Queue library in your project, include the `queue.h` header file in your source code:

```c
#include "queue/queue.h"
```

## Functions Descriptions 

### Queue Creation and Deallocation

### `Queue* queue_create(size_t itemSize);`

- **Purpose**: Creates a new Queue object. The size of each item in the queue must be specified.
- **Parameters:**
    - `itemSize`: The size of the items that will be stored in the queue.
- **Returns:** A pointer to the newly created Queue object.

---

### `void queue_deallocate(Queue* q);`

- **Purpose**: Deallocates a Queue object, freeing all associated memory.
- **Parameters:**
    - `q`: Pointer to the Queue object to be deallocated.

---

### `void queue_push(Queue* q, void* item);`

- **Purpose**: Adds an item to the back of the queue.
- **Parameters:**
    - `q`: Pointer to the Queue object.
    - `item`: Pointer to the item to be added to the queue.

---

### `void queue_pop(Queue* q);` 

- **Purpose**: Removes the item at the front of the queue.
- **Parameters:**
    - `q`: Pointer to the Queue object.

---

### `void* queue_front(const Queue* q);` 
- **Purpose**: Returns a pointer to the front item of the queue without removing it.
- **Parameters:**
    - `q`: Pointer to the Queue object.
- **Returns:** A pointer to the front item or `NULL` if the queue is empty.

---

### `void* queue_back(const Queue* q);` 

- **Purpose**: Returns a pointer to the back item of the queue without removing it.
- **Parameters:**
    - `q`: Pointer to the Queue object.
- **Returns:** A pointer to the back item or `NULL` if the queue is empty.

---

### `void queue_emplace(Queue* q, void* item, size_t itemSize);` 

- **Purpose**: Constructs and adds an item at the back of the queue, without copying.
- **Parameters:**
    - `q`: Pointer to the Queue object.
    - `item`: Pointer to the item to be emplaced.
    - `itemSize`: Size of the item to be emplaced.

---

### `size_t queue_size(const Queue* q);` 

- **Purpose**: Returns the number of items currently in the queue.
- **Parameters:**
    - `q`: Pointer to the Queue object.
- **Returns:** The number of items in the queue.

---

### `bool queue_empty(const Queue* q);` 

- **Purpose**: Checks if the queue is empty.
- **Parameters:**
    - `q`: Pointer to the Queue object.
- **Returns:** `true` if the queue is empty, otherwise `false`.

---

### `void queue_swap(Queue* q1, Queue* q2);` 

- **Purpose**: Swaps the contents of two queues.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.

---

### `bool queue_is_equal(const Queue* q1, const Queue* q2);` 

- **Purpose**: Checks if two queues are equal.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.
- **Returns:** `true` if the queues are equal, otherwise `false`.

---

### `bool queue_is_less(const Queue* q1, const Queue* q2);` 

- **Purpose**: Checks if the first queue is less than the second.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.
- **Returns:** `true` if the first queue is less than the second, otherwise `false`.

--- 

### `bool queue_is_greater(const Queue* q1, const Queue* q2);` 

- **Purpose**: Checks if the first queue is greater than the second.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.
- **Returns:** `true` if the first queue is greater than the second, otherwise `false`.

--- 

### `bool queue_is_not_equal(const Queue* q1, const Queue* q2);` 

- **Purpose**: Checks if two queues are not equal.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.
- **Returns:** `true` if the queues are not equal, otherwise `false`.

--- 

### `bool queue_is_less_or_equal(const Queue* q1, const Queue* q2);` 

- **Purpose**: Checks if the first queue is less than or equal to the second.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.
- **Returns:** `true` if the first queue is less than or equal to the second, otherwise `false`.

--- 

### `bool queue_is_greater_or_equal(const Queue* q1, const Queue* q2);` 

- **Purpose**: Checks if the first queue is greater than or equal to the second.
- **Parameters:**
    - `q1`: Pointer to the first Queue object.
    - `q2`: Pointer to the second Queue object.
- **Returns:** `true` if the first queue is greater than or equal to the second, otherwise `false`.

--- 

## Examples

### Example 1: Creating a Queue and Adding Items

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }

    fmt_printf("Queue size: %zu\n", queue_size(myQueue));
    
    queue_deallocate(myQueue);
    return 0;
}
```
**Expected Output:**
```
Queue size: 5
```

---

### Example 2: Checking If the Queue Is Empty

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }
    fmt_printf("Is the queue empty? %s\n", queue_empty(myQueue) ? "Yes" : "No");

    queue_deallocate(myQueue);
    return 0;
}
```
**Expected Output:**
```
Is the queue empty? No
```

---

### Example 3: Accessing Front and Back Elements

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) { 
        queue_push(myQueue, &values[i]);
    }

    int* front = queue_front(myQueue);
    int* back = queue_back(myQueue);

    if (front && back) {
        fmt_printf("Front element: %d\n", *front);
        fmt_printf("Back element: %d\n", *back);
    }

    queue_deallocate(myQueue);
    return 0;
}
```
**Expected Output:**
```
Front element: 10
Back element: 50
```

---

### Example 4: Removing Items from the Queue

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue = queue_create(sizeof(int));
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
**Expected Output:**
```
New front element after pop: 20
```

---

### Example 5: Emplacing Items and Using Relational Operators

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue1 = queue_create(sizeof(int));
    Queue* myQueue2 = queue_create(sizeof(int));
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
**Expected Output:**
```
Are the queues equal? No
Is myQueue1 less than myQueue2? Yes
```

---

### Example 6: Swapping Queues

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue1 = queue_create(sizeof(int));
    Queue* myQueue2 = queue_create(sizeof(int));
    int values1[] = {10, 20, 30, 40, 50};
    
    for (int i = 0; i < 5; ++i) {
        queue_push(myQueue1, &values1[i]);
    }

    int values2[] = {15, 25, 35, 45, 55};
    for (int i = 0; i < 5; ++i) { 
        queue_emplace(myQueue2, &values2[i], sizeof(int));
    }

    queue_swap(myQueue1, myQueue2);

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
**Expected Output:**
```
Front element of myQueue1 after swap: 15
Front element of myQueue2 after swap: 10
```

---

### Example 7: Task Management System Using Queue

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
        queue_pop(queue);
    }
}

int main() {
    Queue* taskQueue = queue_create(sizeof(Task));

    addTask(taskQueue, 1, "Fix bug in code", 5);
    addTask(taskQueue, 2, "Update documentation", 3);
    addTask(taskQueue, 3, "Refactor module", 4);

    fmt_printf("Task queue size before processing: %zu\n", queue_size(taskQueue));
    processTasks(taskQueue);
    fmt_printf("Task queue size after processing: %zu\n", queue_size(taskQueue));

    queue_deallocate(taskQueue);
    return 0;
}
```
**Expected Output:**
```
Task queue size before processing: 3
Processing Task ID 1: Fix bug in code
Processing Task ID 2: Update documentation
Processing Task ID 3: Refactor module
Task queue size after processing: 0
```

---

### Example 8: 2D Queue of String Objects

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include "queue/queue.h"
#include <stdlib.h>

int main() {
    Queue* queue2D = queue_create(sizeof(Queue*));

    for (int i = 0; i < 3; ++i) { 
        Queue* stringQueue = queue_create(sizeof(String*));

        for (int j = 0; j < 5; ++j) { 
            char *buffer = fmt_sprintf("String %d-%d", i, j);
            String* str = string_create(buffer);
            queue_emplace(stringQueue, &str, sizeof(String*));
            free(buffer);
        }

        queue_emplace(queue2D, &stringQueue, sizeof(Queue*));
    }

    while (!queue_empty(queue2D)) {
        Queue** innerQueuePtr = (Queue**)queue_front(queue2D);
        Queue* innerQueue = *innerQueuePtr;

        while (!queue_empty(innerQueue)) {
            String** strPtr = (String**)queue_front(innerQueue);
            String* str = *strPtr;
            fmt_printf("Processing: %s\n", string_c_str(str));

            queue_pop(innerQueue);
            string_deallocate(str);
        }

        queue_pop(queue2D);
        queue_deallocate(innerQueue);
    }

    queue_deallocate(queue2D);
    return 0;
}
```
**Expected Output:**
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

---

## Conclusion

The Queue library provides a flexible, easy-to-use API for creating and managing dynamic-sized queues in C. With support for standard queue operations and relational comparisons, it is a robust tool for a variety of applications, from simple data structures to complex task management systems.