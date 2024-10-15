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