# Queue Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

The Queue library is part of a project aimed at reimplementing C++ standard library features in C. It provides a generic container that encapsulates dynamic-sized Queues, offering functionality similar to `std::queue` in C++.

## Compilation

The Queue library depends on the `vector` module (for storage) and the
`algorithm` module (used by `queue_sort`). When you compile, all three
translation units must be included:

```bash
gcc -std=c17 -O2 -Wall -Wextra -pedantic -o main \
    main.c \
    queue/queue.c vector/vector.c algorithm/algorithm.c
```

If you also use `fmt`, `string`, etc., add their `.c` files to the
command line in the same way.

Ensure you have the GCC compiler installed on your system and that all
source files are in the correct directory structure as shown in the
project.

## Usage

To use the Queue library in your project, include the `queue.h` header file in your source code:

```c
#include "queue/queue.h"
```

## Functions Descriptions

---

### `Queue* queue_create(size_t itemSize)`

**Purpose**:  
Allocates and initializes a new Queue object backed by a dynamic Vector. The queue stores elements of exactly `itemSize` bytes each.

**Parameters**:  
- `itemSize`: The size in bytes of each element the queue will store.

**Return Value**:  
A pointer to the newly created `Queue`, or `NULL` if `itemSize` is 0 or memory allocation fails.

**Usage Case**:  
Call once at the start of any FIFO usage to obtain a typed queue. Pass `sizeof(MyType)` to create a queue of `MyType` elements.

---

### `void queue_deallocate(Queue* q)`

**Purpose**:  
Frees all memory owned by the queue, including the internal Vector and the Queue wrapper struct. Passing `NULL` is a safe no-op.

**Parameters**:  
- `q`: Pointer to the Queue to destroy. May be `NULL`.

**Return Value**:  
None.

**Usage Case**:  
Always call at the end of a queue's lifetime to prevent memory leaks. After this call, the pointer `q` must not be used again.

---

### `bool queue_push(Queue* q, const void* item)`

**Purpose**:  
Copies `itemSize` bytes from `*item` and appends them to the back of the queue. Resizes internal storage if needed.

**Parameters**:  
- `q`: Pointer to the Queue. Must not be `NULL`.
- `item`: Pointer to the value to enqueue. Must not be `NULL`.

**Return Value**:  
`true` on success; `false` if either pointer is `NULL` or internal allocation fails.

**Usage Case**:  
The primary way to enqueue a value. For structs, pass the address of a local or heap-allocated instance.

---

### `void queue_pop(Queue* q)`

**Purpose**:  
Removes the element at the front of the queue. All remaining elements shift one position toward the front (**O(n)**).

**Parameters**:  
- `q`: Pointer to the Queue. Passing `NULL` or an empty queue is a safe no-op.

**Return Value**:  
None.

**Usage Case**:  
The standard dequeue operation. Prefer calling `queue_front` to inspect the element before calling `queue_pop` to remove it.

---

### `void* queue_front(const Queue* q)`

**Purpose**:  
Returns a borrowed pointer to the element at the front of the queue without removing it.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.

**Return Value**:  
Pointer to the front element, or `NULL` if the queue is `NULL` or empty. The pointer is valid until the next mutating operation.

**Usage Case**:  
Inspect or process the next element before deciding whether to pop it. Cast the returned `void*` to your element type.

---

### `void* queue_back(const Queue* q)`

**Purpose**:  
Returns a borrowed pointer to the element at the back of the queue (the most recently pushed element) without removing it.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.

**Return Value**:  
Pointer to the back element, or `NULL` if the queue is `NULL` or empty.

**Usage Case**:  
Peek at the most recently enqueued value — for example, to check whether a duplicate was just pushed.

---

### `void* queue_emplace(Queue* q, const void* item, size_t itemSize)`

**Purpose**:  
Copies `itemSize` bytes from `*item` and appends them at the back of the queue (mirrors C++17 `std::queue::emplace`). `itemSize` must match the queue's element size; a mismatch is rejected.

**Parameters**:  
- `q`: Pointer to the Queue. Must not be `NULL`.
- `item`: Pointer to the value to emplace. Must not be `NULL`.
- `itemSize`: Size of the item in bytes. Must equal `queue_item_size(q)`.

**Return Value**:  
Pointer to the newly inserted element inside the queue's storage, or `NULL` on failure (size mismatch, `NULL` pointers, or OOM). The pointer is valid until the next mutating operation.

**Usage Case**:  
Use instead of `queue_push` when you need a pointer to the element immediately after insertion — for example, to initialize fields of a large struct in place.

---

### `size_t queue_size(const Queue* q)`

**Purpose**:  
Returns the number of elements currently stored in the queue.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.

**Return Value**:  
Number of elements, or `0` if `q` is `NULL`.

**Usage Case**:  
Check how many items are waiting before entering a processing loop, or to enforce a maximum queue depth.

---

### `bool queue_empty(const Queue* q)`

**Purpose**:  
Reports whether the queue contains no elements.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.

**Return Value**:  
`true` if the queue is empty or `NULL`; `false` otherwise.

**Usage Case**:  
Use as the loop condition for draining a queue: `while (!queue_empty(q)) { ... queue_pop(q); }`.

---

### `void queue_swap(Queue* q1, Queue* q2)`

**Purpose**:  
Exchanges the entire contents of two queues in O(1) by swapping their internal Vector pointers. If the element sizes differ, the swap is a safe no-op.

**Parameters**:  
- `q1`: Pointer to the first Queue. Must not be `NULL`.
- `q2`: Pointer to the second Queue. Must not be `NULL`.

**Return Value**:  
None.

**Usage Case**:  
Efficiently hand off a batch of work from one queue to another — for example, in a double-buffering producer/consumer pattern.

---

### `bool queue_is_equal(const Queue* q1, const Queue* q2)`

**Purpose**:  
Compares two queues element-by-element. Two `NULL` pointers are considered equal.

**Parameters**:  
- `q1`: First Queue. May be `NULL`.
- `q2`: Second Queue. May be `NULL`.

**Return Value**:  
`true` if both queues have the same size and identical byte content; `false` otherwise. `queue_is_equal(NULL, NULL)` returns `true`; `queue_is_equal(NULL, q)` returns `false`.

**Usage Case**:  
Validate that a queue copy or snapshot matches the original after a sequence of operations.

---

### `bool queue_is_not_equal(const Queue* q1, const Queue* q2)`

**Purpose**:  
Returns the logical negation of `queue_is_equal(q1, q2)`.

**Parameters**:  
- `q1`: First Queue. May be `NULL`.
- `q2`: Second Queue. May be `NULL`.

**Return Value**:  
`true` if the queues differ in size or content; `false` if they are equal.

**Usage Case**:  
Use as a guard to detect when a queue has changed since the last checkpoint.

---

### `bool queue_is_less(const Queue* q1, const Queue* q2)`

**Purpose**:  
Lexicographic less-than comparison over the raw byte content of both queues.

**Parameters**:  
- `q1`: First Queue. May be `NULL`.
- `q2`: Second Queue. May be `NULL`.

**Return Value**:  
`true` if `q1` is lexicographically less than `q2`; `false` otherwise. Returns `false` if either operand is `NULL`.

**Usage Case**:  
Sort or rank queues by content — for example, comparing serialized message buffers.

---

### `bool queue_is_greater(const Queue* q1, const Queue* q2)`

**Purpose**:  
Lexicographic greater-than comparison over the raw byte content of both queues.

**Parameters**:  
- `q1`: First Queue. May be `NULL`.
- `q2`: Second Queue. May be `NULL`.

**Return Value**:  
`true` if `q1` is lexicographically greater than `q2`; `false` otherwise. Returns `false` if either operand is `NULL`.

**Usage Case**:  
Use in sorted containers or priority logic based on queue content.

---

### `bool queue_is_less_or_equal(const Queue* q1, const Queue* q2)`

**Purpose**:  
Returns `true` when `q1` is lexicographically less than or equal to `q2`.

**Parameters**:  
- `q1`: First Queue. May be `NULL`.
- `q2`: Second Queue. May be `NULL`.

**Return Value**:  
`true` if `q1 <= q2` lexicographically; `false` otherwise.

**Usage Case**:  
Relational comparison in ordered data structures or assertion checks.

---

### `bool queue_is_greater_or_equal(const Queue* q1, const Queue* q2)`

**Purpose**:  
Returns `true` when `q1` is lexicographically greater than or equal to `q2`.

**Parameters**:  
- `q1`: First Queue. May be `NULL`.
- `q2`: Second Queue. May be `NULL`.

**Return Value**:  
`true` if `q1 >= q2` lexicographically; `false` otherwise.

**Usage Case**:  
Relational comparison in ordered data structures or assertion checks.

---

### `void queue_sort(Queue* q, QueueCompareFunc comp)`

**Purpose**:  
Sorts the elements of the queue in place using the caller-supplied comparison function. The sort is performed directly on the underlying contiguous storage (quicksort).

**Parameters**:  
- `q`: Pointer to the Queue to sort. Passing `NULL` is a safe no-op.
- `comp`: Comparison function with signature `int (*)(const void*, const void*)`. Must return a negative value when the first argument should come before the second, zero for equal elements, and a positive value when the first should come after.

**Return Value**:  
None.

**Usage Case**:  
Reorder a queue by priority, timestamp, or any other key — for example, converting a FIFO into a sorted work list before batch processing.

---

### `Queue* queue_copy(const Queue* src)`

**Purpose**:  
Creates an independent deep copy of `src`. The new queue owns its own storage; subsequent mutations to either queue do not affect the other.

**Parameters**:  
- `src`: Queue to copy. May be `NULL` (returns `NULL`).

**Return Value**:  
Pointer to the new Queue, or `NULL` on `NULL` input or OOM.

**Usage Case**:  
Snapshot a queue before a destructive operation — for example, save a checkpoint before draining a job queue.

---

### `bool queue_assign(Queue* dest, const Queue* src)`

**Purpose**:  
Replaces `dest`'s contents with a deep copy of `src`. The element sizes of both queues must match; a mismatch is rejected.

**Parameters**:  
- `dest`: Destination Queue. Must not be `NULL`.
- `src`: Source Queue. Must not be `NULL`.

**Return Value**:  
`true` on success; `false` if either pointer is `NULL`, if element sizes differ, or if OOM.

**Usage Case**:  
Re-initialize a queue from a known-good snapshot without allocating a new Queue wrapper struct.

---

### `size_t queue_capacity(const Queue* q)`

**Purpose**:  
Returns the number of elements the queue can currently hold without triggering a reallocation.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.

**Return Value**:  
Allocated capacity in element units, or `0` if `q` is `NULL`.

**Usage Case**:  
Inspect the capacity before and after `queue_reserve` to confirm pre-allocation took effect.

---

### `size_t queue_item_size(const Queue* q)`

**Purpose**:  
Returns the size in bytes of each element stored in the queue — the value originally passed to `queue_create`.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.

**Return Value**:  
Bytes per element, or `0` if `q` is `NULL`.

**Usage Case**:  
Use in generic code that manipulates queues through `void*` pointers without knowing the element type at compile time.

---

### `bool queue_reserve(Queue* q, size_t n)`

**Purpose**:  
Pre-allocates storage so that the queue can hold at least `n` elements without any further reallocation. If the current capacity already meets or exceeds `n`, this is a no-op.

**Parameters**:  
- `q`: Pointer to the Queue. Must not be `NULL`.
- `n`: Minimum number of elements to accommodate.

**Return Value**:  
`true` on success (or if no action was needed); `false` if `q` is `NULL` or OOM.

**Usage Case**:  
Call before a burst of `queue_push` operations when the final count is known in advance, to eliminate repeated reallocations.

---

### `void* queue_at(const Queue* q, size_t index)`

**Purpose**:  
Returns a borrowed pointer to the element at zero-based position `index` from the front of the queue, without removing it. This is a random-access extension not present in `std::queue`.

**Parameters**:  
- `q`: Pointer to the Queue. May be `NULL`.
- `index`: Zero-based position from the front (0 = front element).

**Return Value**:  
Pointer to the element at `index`, or `NULL` if `q` is `NULL` or `index` is out of range.

**Usage Case**:  
Iterate over queue contents for inspection or statistics without popping elements — for example, printing all pending jobs or computing a running average.

---

**Behavioral notes**

- `queue_pop()` is **O(n)** because the underlying contiguous Vector shifts all remaining elements one slot forward. Use a deque- or list-backed FIFO if O(1) pop is required.
- `queue_swap` is a safe no-op when `q1->itemSize != q2->itemSize`.
- `queue_is_equal(NULL, NULL)` returns `true`; every other comparison involving at least one `NULL` returns `false`.

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

    int* front = (int*)queue_front(myQueue);
    int* back = (int*)queue_back(myQueue);

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
    int* front = (int*)queue_front(myQueue);

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

    int* front1 = (int*)queue_front(myQueue1);
    int* front2 = (int*)queue_front(myQueue2);

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

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-truncation"
        strncpy(task.description, desc, sizeof(task.description));
    #pragma GCC diagnostic pop

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
#include "string/std_string.h"
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

### Example 9 : Simulate Print 

```c
#include "fmt/fmt.h"
#include "queue/queue.h"
#include <stdlib.h>
#include <string.h>


typedef struct PrintJob {
    int id;
    char description[100];
} PrintJob;


void addPrintJob(Queue* queue, int id, const char* desc) {
    PrintJob job;
    job.id = id;
    strncpy(job.description, desc, sizeof(job.description) - 1);
    job.description[sizeof(job.description) - 1] = '\0';
    queue_emplace(queue, &job, sizeof(PrintJob));
}


void processPrintQueue(Queue* queue) {
    while (!queue_empty(queue)) {
        PrintJob* job = (PrintJob*)queue_front(queue);
        fmt_printf("Printing Job ID %d: %s\n", job->id, job->description);
        queue_pop(queue);
    }
}


int main() {
    Queue* printQueue = queue_create(sizeof(PrintJob));

    addPrintJob(printQueue, 1, "Document 1: Report.pdf");
    addPrintJob(printQueue, 2, "Document 2: Presentation.pptx");
    addPrintJob(printQueue, 3, "Document 3: Invoice.xlsx");

    fmt_printf("Starting print queue processing...\n");
    processPrintQueue(printQueue);

    queue_deallocate(printQueue);
    return 0;
}
```
**Result**
```
Starting print queue processing...
Printing Job ID 1: Document 1: Report.pdf
Printing Job ID 2: Document 2: Presentation.pptx
Printing Job ID 3: Document 3: Invoice.xlsx
```

---

### Example 10 : Task Scheduling with Priorities
```c
#include "fmt/fmt.h"
#include "queue/queue.h"
#include <stdlib.h>



typedef struct Task {
    int priority;
    char description[100];
} Task;


int compare_tasks(const void* a, const void* b) {
    const Task* t1 = (const Task*)a;
    const Task* t2 = (const Task*)b;
    return (t1->priority > t2->priority) - (t1->priority < t2->priority);
}


void print_tasks(const Queue* q) {
    for (size_t i = 0; i < queue_size(q); ++i) {
        Task* task = (Task*)queue_at(q, i);
        fmt_printf("Task: %s (Priority: %d)\n", task->description, task->priority);
    }
}


int main() {
    Queue* taskQueue = queue_create(sizeof(Task));

    Task t1 = {2, "Fix Bug #123"};
    Task t2 = {3, "Write Documentation"};
    Task t3 = {1, "Deploy Feature A"};

    queue_push(taskQueue, &t1);
    queue_push(taskQueue, &t2);
    queue_push(taskQueue, &t3);

    fmt_printf("Before sorting:\n");
    print_tasks(taskQueue);
    
    fmt_printf("Sorting tasks by priority...\n");
    queue_sort(taskQueue, compare_tasks);

    print_tasks(taskQueue);

    queue_deallocate(taskQueue);
    return 0;
}
```
**Result**
```
Before sorting:
Task: Fix Bug #123 (Priority: 2)
Task: Write Documentation (Priority: 3)
Task: Deploy Feature A (Priority: 1)
Sorting tasks by priority...
Task: Deploy Feature A (Priority: 1)
Task: Fix Bug #123 (Priority: 2)
Task: Write Documentation (Priority: 3)
```

---

### Example 11 : Deep-Copy Independence with `queue_copy`

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* original = queue_create(sizeof(int));
    int vals[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) {
        queue_push(original, &vals[i]);
    }

    Queue* copy = queue_copy(original);

    /* Modify original — copy must be unaffected */
    int bonus = 99;
    queue_push(original, &bonus);
    queue_pop(original);           /* remove 10 */

    fmt_printf("Original (%zu elements):\n", queue_size(original));
    for (size_t i = 0; i < queue_size(original); ++i) {
        fmt_printf("  [%zu] = %d\n", i, *(int*)queue_at(original, i));
    }

    fmt_printf("Copy (%zu elements):\n", queue_size(copy));
    for (size_t i = 0; i < queue_size(copy); ++i) {
        fmt_printf("  [%zu] = %d\n", i, *(int*)queue_at(copy, i));
    }

    queue_deallocate(original);
    queue_deallocate(copy);

    return 0;
}
```
**Result**
```
Original (5 elements):
  [0] = 20
  [1] = 30
  [2] = 40
  [3] = 50
  [4] = 99
Copy (5 elements):
  [0] = 10
  [1] = 20
  [2] = 30
  [3] = 40
  [4] = 50
```

---

### Example 12 : Replacing Contents with `queue_assign`

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* src = queue_create(sizeof(double));
    double measurements[] = {1.1, 2.2, 3.3, 4.4};

    for (int i = 0; i < 4; ++i) {
        queue_push(src, &measurements[i]);
    }

    Queue* dest = queue_create(sizeof(double));
    double init_val = 0.0;
    queue_push(dest, &init_val);

    fmt_printf("Before assign - dest size: %zu\n", queue_size(dest));

    queue_assign(dest, src);

    fmt_printf("After assign - dest size: %zu\n", queue_size(dest));
    fmt_printf("dest elements:\n");
    for (size_t i = 0; i < queue_size(dest); ++i) {
        fmt_printf("  [%zu] = %.1f\n", i, *(double*)queue_at(dest, i));
    }

    /* Modify src after assign — dest must not change */
    double extra = 9.9;
    queue_push(src, &extra);
    fmt_printf("After pushing to src, dest size still: %zu\n", queue_size(dest));

    queue_deallocate(src);
    queue_deallocate(dest);

    return 0;
}
```
**Result**
```
Before assign - dest size: 1
After assign - dest size: 4
dest elements:
  [0] = 1.1
  [1] = 2.2
  [2] = 3.3
  [3] = 4.4
After pushing to src, dest size still: 4
```

---

### Example 13 : Pre-allocation Introspection with `queue_reserve`, `queue_capacity`, `queue_item_size`

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* q = queue_create(sizeof(int));

    fmt_printf("Item size:        %zu bytes\n", queue_item_size(q));
    fmt_printf("Initial capacity: %zu\n", queue_capacity(q));
    fmt_printf("Initial size:     %zu\n", queue_size(q));

    queue_reserve(q, 100);
    fmt_printf("After reserve(100):\n");
    fmt_printf("  Capacity >= 100: %s\n", queue_capacity(q) >= 100 ? "yes" : "no");
    fmt_printf("  Size:     %zu\n", queue_size(q));

    for (int i = 0; i < 50; ++i) {
        queue_push(q, &i);
    }

    fmt_printf("After 50 pushes:\n");
    fmt_printf("  Capacity: %zu\n", queue_capacity(q));
    fmt_printf("  Size:     %zu\n", queue_size(q));

    queue_deallocate(q);
    return 0;
}
```
**Result**
```
Item size:        4 bytes
Initial capacity: 32
Initial size:     0
After reserve(100):
  Capacity >= 100: yes
  Size:     0
After 50 pushes:
  Capacity: 100
  Size:     50
```

---

### Example 14 : Random Access Without Popping via `queue_at`

```c
#include "fmt/fmt.h"
#include "queue/queue.h"

typedef struct {
    int x, y;
} Point;

int main() {
    Queue* path = queue_create(sizeof(Point));
    Point points[] = {{0, 0}, {1, 3}, {4, 2}, {7, 5}, {9, 1}};

    for (int i = 0; i < 5; ++i) {
        queue_push(path, &points[i]);
    }

    fmt_printf("Path waypoints (via queue_at, without popping):\n");
    for (size_t i = 0; i < queue_size(path); ++i) {
        Point* p = (Point*)queue_at(path, i);
        fmt_printf("  Waypoint %zu: (%d, %d)\n", i, p->x, p->y);
    }

    /* Calculate total Manhattan distance without consuming the queue */
    int total_dist = 0;
    for (size_t i = 1; i < queue_size(path); ++i) {
        Point* a = (Point*)queue_at(path, i - 1);
        Point* b = (Point*)queue_at(path, i);

        int dx = b->x - a->x;
        int dy = b->y - a->y;

        total_dist += (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
    }
    fmt_printf("Total Manhattan distance: %d\n", total_dist);
    fmt_printf("Queue still has %zu elements after inspection.\n", queue_size(path));

    queue_deallocate(path);
    return 0;
}
```
**Result**
```
Path waypoints (via queue_at, without popping):
  Waypoint 0: (0, 0)
  Waypoint 1: (1, 3)
  Waypoint 2: (4, 2)
  Waypoint 3: (7, 5)
  Waypoint 4: (9, 1)
Total Manhattan distance: 20
Queue still has 5 elements after inspection.
```

---

### Example 15 : Priority Processing with `queue_copy` + `queue_sort` + Drain

```c
#include "fmt/fmt.h"
#include "queue/queue.h"


typedef struct {
    int  id;
    int  priority;
    char item[32];
} Order;


int cmp_order_priority(const void* a, const void* b) {
    const Order* oa = (const Order*)a;
    const Order* ob = (const Order*)b;

    return (oa->priority > ob->priority) - (oa->priority < ob->priority);
}


int main() {
    Queue* incoming = queue_create(sizeof(Order));
    Order orders[] = {
        {101, 3, "Widget A"},
        {102, 1, "Widget B"},
        {103, 2, "Widget C"},
        {104, 1, "Widget D"},
        {105, 3, "Widget E"},
    };

    for (int i = 0; i < 5; ++i) {
        queue_push(incoming, &orders[i]);
    }

    fmt_printf("Incoming FIFO order:\n");
    for (size_t i = 0; i < queue_size(incoming); ++i) {
        Order* o = (Order*)queue_at(incoming, i);
        fmt_printf("  Order #%d  priority=%d  item=%s\n", o->id, o->priority, o->item);
    }

    /* Make a copy for priority processing; leave original intact */
    Queue* priority_q = queue_copy(incoming);
    queue_sort(priority_q, cmp_order_priority);

    fmt_printf("\nProcessing by priority (lowest number first):\n");
    while (!queue_empty(priority_q)) {
        Order* o = (Order*)queue_front(priority_q);
        fmt_printf("  Processing Order #%d [priority %d]: %s\n", o->id, o->priority, o->item);
        queue_pop(priority_q);
    }

    fmt_printf("\nOriginal FIFO still intact (%zu orders).\n", queue_size(incoming));

    queue_deallocate(incoming);
    queue_deallocate(priority_q);

    return 0;
}
```
**Result**
```
Incoming FIFO order:
  Order #101  priority=3  item=Widget A
  Order #102  priority=1  item=Widget B
  Order #103  priority=2  item=Widget C
  Order #104  priority=1  item=Widget D
  Order #105  priority=3  item=Widget E

Processing by priority (lowest number first):
  Processing Order #104 [priority 1]: Widget D
  Processing Order #102 [priority 1]: Widget B
  Processing Order #103 [priority 2]: Widget C
  Processing Order #105 [priority 3]: Widget E
  Processing Order #101 [priority 3]: Widget A

Original FIFO still intact (5 orders).
```

---

## License

This library is licensed under the ISC License.