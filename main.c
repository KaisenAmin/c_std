#include "priority_queue/priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int main() 
{
    PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

    if (!pq) 
    {
        fprintf(stderr, "Failed to create priority queue.\n");
        return EXIT_FAILURE;
    }

    // Push some integers onto the priority queue
    int values[] = {5, 10, 3, 7, 4};
    for (int i = 0; i < 5; ++i) 
        priority_queue_push(pq, &values[i]);

    printf("Priority Queue size: %zu\n", priority_queue_size(pq));
    printf("Is the priority queue empty? %s\n", priority_queue_empty(pq) ? "Yes" : "No");
    
    priority_queue_deallocate(pq);

    return EXIT_SUCCESS;
}