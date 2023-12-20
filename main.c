#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue/queue.h"

int main() 
{
    Queue* myQueue = queue_create(sizeof(int));

    if (!myQueue) 
    {
        fprintf(stderr, "Failed to create queue.\n");
        return EXIT_FAILURE;
    }

    // Push some integers onto the queue
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; ++i) 
        queue_push(myQueue, &values[i]);
        
    // Print the size of the queue
    printf("Queue size: %zu\n", queue_size(myQueue));
    queue_deallocate(myQueue);

    return EXIT_SUCCESS;
}
