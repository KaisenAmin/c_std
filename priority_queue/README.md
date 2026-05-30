# Priority Queue Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2023  
**License:** ISC License

The Priority Queue library is part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic-sized priority queues, offering similar functionality to `std::priority_queue` in C++.

## Compilation

The priority-queue module is built on top of `vector`, so both source
files must be supplied to the compiler:

```bash
gcc -std=c17 -O2 -Wall -Wextra -pedantic -o main \
    main.c \
    priority_queue/priority_queue.c vector/vector.c
```

If you also use `fmt`, `string`, etc., add their `.c` files to the
command line in the same way.

## Usage

To use the Priority Queue library in your project, include the `priority_queue.h` header file in your source code:

```c
#include "priority_queue/priority_queue.h"
```

## Function Descriptions

---

### `PriorityQueue* priority_queue_create(size_t itemSize, PQCompareFunc compare)`

**Purpose**:  
Allocates and initialises a new, empty priority queue backed by a dynamic vector. The queue maintains the binary max-heap property using the supplied comparator: `compare(a, b) > 0` means `a` has higher priority than `b`.

**Parameters**:  
- `itemSize`: Byte size of one element. Must be > 0.  
- `compare`: User-supplied comparator (qsort convention). Must not be `NULL`.

**Return Value**:  
Pointer to the newly created `PriorityQueue`, or `NULL` if `itemSize == 0`, `compare == NULL`, or memory allocation fails.

**Usage Case**:  
Call once to obtain a typed heap. Pass `sizeof(MyType)` and an appropriate comparator — negate the comparison to get a min-heap.

---

### `void priority_queue_deallocate(PriorityQueue* pq)`

**Purpose**:  
Frees all memory owned by the priority queue, including the internal vector and the wrapper struct. Passing `NULL` is a safe no-op.

**Parameters**:  
- `pq`: Priority queue to destroy. May be `NULL`.

**Return Value**:  
None.

**Usage Case**:  
Always call when the queue is no longer needed to prevent memory leaks. After this call the pointer must not be used again.

---

### `bool priority_queue_push(PriorityQueue* pq, const void* item)`

**Purpose**:  
Copies `itemSize` bytes from `*item` onto the heap and sifts up to restore the heap property. O(log n).

**Parameters**:  
- `pq`: Destination priority queue. Must not be `NULL`.  
- `item`: Pointer to the value to insert. Must not be `NULL`.

**Return Value**:  
`true` on success; `false` on `NULL` arguments or allocation failure.

**Usage Case**:  
The standard enqueue operation. The caller's buffer can be reused or freed after the call — the queue owns its own copy.

---

### `void priority_queue_pop(PriorityQueue* pq)`

**Purpose**:  
Removes the top (highest-priority) element from the heap and sifts down the replacement to restore the heap property. O(log n). Safe no-op on an empty queue or `NULL`.

**Parameters**:  
- `pq`: Priority queue to modify. May be `NULL` or empty.

**Return Value**:  
None.

**Usage Case**:  
Call after inspecting `priority_queue_top` to consume the element with the highest priority.

---

### `void* priority_queue_top(const PriorityQueue* pq)`

**Purpose**:  
Returns a borrowed pointer to the element at the root of the heap (the highest-priority element) without removing it. O(1).

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL`.

**Return Value**:  
Borrowed pointer to the top element, or `NULL` if the queue is `NULL` or empty. The pointer is valid only until the next mutating operation.

**Usage Case**:  
Peek at the next element to be processed before deciding whether to pop it.

---

### `void* priority_queue_front(const PriorityQueue* pq)`

**Purpose**:  
Alias for `priority_queue_top`. Returns a borrowed pointer to the highest-priority element (the root of the heap) without removing it.

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL`.

**Return Value**:  
Borrowed pointer to the top element, or `NULL` if the queue is `NULL` or empty.

**Usage Case**:  
Use when you want the `std::queue`-style `front()` name for the highest-priority slot.

---

### `void* priority_queue_back(const PriorityQueue* pq)`

**Purpose**:  
Returns a borrowed pointer to the **last element of the underlying heap array** — not necessarily the lowest-priority element. A binary heap only constrains parent/child relationships; leaves can appear in any relative order.

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL`.

**Return Value**:  
Pointer to the last array slot, or `NULL` if the queue is `NULL` or empty.

**Usage Case**:  
Diagnostic or layout-inspection only. Do not rely on this being the minimum-priority element.

---

### `bool priority_queue_empty(const PriorityQueue* pq)`

**Purpose**:  
Returns `true` when the queue contains no elements (or is `NULL`).

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL`.

**Return Value**:  
`true` if empty or `NULL`, `false` otherwise.

**Usage Case**:  
Loop guard for draining a queue: `while (!priority_queue_empty(pq)) { ... priority_queue_pop(pq); }`.

---

### `size_t priority_queue_size(const PriorityQueue* pq)`

**Purpose**:  
Returns the number of elements currently in the priority queue.

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL` (returns `0`).

**Return Value**:  
Element count, or `0` on `NULL` input.

**Usage Case**:  
Check the queue depth before a processing loop, or enforce a maximum queue length policy.

---

### `size_t priority_queue_capacity(const PriorityQueue* pq)`

**Purpose**:  
Returns the number of elements the internal vector can hold before it must reallocate. Capacity is always ≥ size.

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL` (returns `0`).

**Return Value**:  
Allocated capacity in elements, or `0` on `NULL` input.

**Usage Case**:  
Check available headroom before a bulk-push loop; combine with `priority_queue_reserve` to pre-allocate and avoid incremental reallocations.

---

### `size_t priority_queue_item_size(const PriorityQueue* pq)`

**Purpose**:  
Returns the byte size of one element, exactly as supplied to `priority_queue_create`.

**Parameters**:  
- `pq`: Priority queue to inspect. May be `NULL` (returns `0`).

**Return Value**:  
Per-element size in bytes, or `0` on `NULL` input.

**Usage Case**:  
Validate that a second queue or a caller-supplied buffer has the correct element size before calling `priority_queue_assign` or `priority_queue_emplace`.

---

### `bool priority_queue_reserve(PriorityQueue* pq, size_t n)`

**Purpose**:  
Pre-allocates backing storage for at least `n` elements. If the current capacity is already ≥ `n`, this is a no-op success. No elements are added or removed.

**Parameters**:  
- `pq`: Priority queue to grow. Must not be `NULL`.  
- `n`: Minimum desired capacity in elements.

**Return Value**:  
`true` on success; `false` on `NULL` input or underlying allocation failure.

**Usage Case**:  
Call once before pushing a known batch of elements to avoid O(log n) reallocations on every push.

---

### `void priority_queue_clear(PriorityQueue* pq)`

**Purpose**:  
Removes all elements without deallocating backing storage or the queue struct. After the call `priority_queue_size(pq) == 0` and the queue is ready for reuse with its current capacity.

**Parameters**:  
- `pq`: Priority queue to clear. May be `NULL` (safe no-op).

**Return Value**:  
None.

**Usage Case**:  
Reset a queue between processing rounds while retaining pre-reserved capacity, avoiding repeated malloc/free cycles.

---

### `PriorityQueue* priority_queue_copy(const PriorityQueue* src)`

**Purpose**:  
Creates a deep, independent copy of `src`. The entire backing heap storage is duplicated so mutations to the copy do not affect the original and vice versa. O(n) — the source is already a valid heap so the layout is copied directly without re-heapifying.

**Parameters**:  
- `src`: Source priority queue to clone. Must not be `NULL`.

**Return Value**:  
Newly allocated `PriorityQueue*` the caller must free with `priority_queue_deallocate`, or `NULL` on `NULL` input or allocation failure.

**Usage Case**:  
Snapshot a queue before a destructive batch-pop, or hand a copy to another subsystem without transferring ownership.

---

### `bool priority_queue_assign(PriorityQueue* dest, const PriorityQueue* src)`

**Purpose**:  
Replaces all elements in `dest` with a copy of the elements from `src`. `dest` keeps its own comparator; the copied elements are re-heapified with Floyd's O(n) bottom-up algorithm so the result is a valid heap under `dest`'s ordering.

**Parameters**:  
- `dest`: Destination queue (must not be `NULL`). Its previous contents are discarded.  
- `src`: Source queue (must not be `NULL`). Element sizes of `dest` and `src` must match.

**Return Value**:  
`true` on success; `false` on `NULL` input, element-size mismatch, or allocation failure.

**Usage Case**:  
Re-fill a queue from a reference dataset without reallocating the wrapper struct. Useful when `dest` already has pre-reserved capacity you want to keep.

---

### `void* priority_queue_emplace(PriorityQueue* pq, const void* item, size_t itemSize)`

**Purpose**:  
Copies `itemSize` bytes from `*item` into the queue, sifts the new element to its correct heap position, and returns a borrowed pointer to it at its **final** position. Equivalent to `priority_queue_push` but gives back a pointer to the inserted slot.

**Parameters**:  
- `pq`: Destination priority queue. Must not be `NULL`.  
- `item`: Pointer to the source value to insert. Must not be `NULL`.  
- `itemSize`: Must equal `priority_queue_item_size(pq)`; a mismatch is rejected.

**Return Value**:  
Borrowed pointer to the newly inserted element at its heap position, or `NULL` on `NULL` inputs, size mismatch, or OOM. The pointer is invalidated by the next push, pop, or reserve.

**Usage Case**:  
Use instead of `priority_queue_push` when you need the element's address immediately after insertion — for example, to record it in a lookup table.

---

### `void priority_queue_swap(PriorityQueue* pq1, PriorityQueue* pq2)`

**Purpose**:  
Exchanges the entire contents (backing vector pointer) of two priority queues in O(1). If the queues have different element sizes the swap is a safe no-op, preventing UB from pairing the wrong comparator with the wrong storage layout.

**Parameters**:  
- `pq1`: First priority queue. May be `NULL` (safe no-op).  
- `pq2`: Second priority queue. May be `NULL` (safe no-op).

**Return Value**:  
None.

**Usage Case**:  
Efficiently hand off a processed batch from one queue to another — for example, in a double-buffering producer/consumer pattern — without copying elements.

---

### `bool priority_queue_is_equal(const PriorityQueue* a, const PriorityQueue* b)`

**Purpose**:  
Returns `true` if both queues have identical heap storage layout (same size, same item size, same bytes). Two `NULL` pointers compare equal; a single `NULL` operand returns `false`.

**Parameters**:  
- `a`, `b`: Priority queues to compare. Either may be `NULL`.

**Return Value**:  
`true` if the underlying heap arrays are byte-identical; `false` otherwise.

**Usage Case**:  
Verify that a copy or snapshot still matches its origin after a sequence of operations.

---

### `bool priority_queue_is_not_equal(const PriorityQueue* a, const PriorityQueue* b)`

**Purpose**:  
Logical negation of `priority_queue_is_equal`.

**Parameters**:  
- `a`, `b`: Priority queues to compare. Either may be `NULL`.

**Return Value**:  
`true` if the queues differ in size, item size, or content; `false` if they are identical.

**Usage Case**:  
Guard condition to detect when a queue has changed since a last checkpoint.

---

### `bool priority_queue_is_less(const PriorityQueue* a, const PriorityQueue* b)`

**Purpose**:  
Lexicographic less-than comparison of the underlying heap byte arrays.

**Parameters**:  
- `a`, `b`: Priority queues to compare. Either may be `NULL` (returns `false`).

**Return Value**:  
`true` if `a` is lexicographically less than `b`; `false` otherwise or on `NULL` input.

**Usage Case**:  
Ordered operations on collections of queues.

---

### `bool priority_queue_is_less_or_equal(const PriorityQueue* a, const PriorityQueue* b)`

**Purpose**:  
Returns `priority_queue_is_less(a, b) || priority_queue_is_equal(a, b)`.

**Parameters**:  
- `a`, `b`: Priority queues to compare. Either may be `NULL` (returns `false`).

**Return Value**:  
`true` if `a ≤ b` lexicographically; `false` otherwise or on `NULL` input.

**Usage Case**:  
Range checks and ordering assertions on priority queues.

---

### `bool priority_queue_is_greater(const PriorityQueue* a, const PriorityQueue* b)`

**Purpose**:  
Lexicographic greater-than comparison of the underlying heap byte arrays.

**Parameters**:  
- `a`, `b`: Priority queues to compare. Either may be `NULL` (returns `false`).

**Return Value**:  
`true` if `a` is lexicographically greater than `b`; `false` otherwise or on `NULL` input.

**Usage Case**:  
Sorting or ranking a collection of queues by content.

---

### `bool priority_queue_is_greater_or_equal(const PriorityQueue* a, const PriorityQueue* b)`

**Purpose**:  
Returns `priority_queue_is_greater(a, b) || priority_queue_is_equal(a, b)`.

**Parameters**:  
- `a`, `b`: Priority queues to compare. Either may be `NULL` (returns `false`).

**Return Value**:  
`true` if `a ≥ b` lexicographically; `false` otherwise or on `NULL` input.

**Usage Case**:  
Ordered comparisons where equality must also succeed.

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

### Example 5: Sorting Integers in Ascending Order (min-heap via reversed comparator)

`compare_ints_desc` returns positive when `a < b`, so the heap treats
*smaller* values as higher priority — effectively a **min-heap**.
Repeatedly popping the top therefore yields the elements in ascending
order.

```c
#include "priority_queue/priority_queue.h"
#include "fmt/fmt.h"

static int compare_ints_desc(const void* a, const void* b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_b > int_a) - (int_b < int_a);  // reversed -> min-heap
}

int main() {
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints_desc);
    int values[] = {5, 10, 3, 7, 4, 15, 8};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        priority_queue_push(pq, &values[i]);
    }

    fmt_printf("Sorted elements in ascending order:\n");
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
Sorted elements in ascending order:
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