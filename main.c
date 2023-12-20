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
            queue_emplace(stringQueue, &str, sizeof(String*));
        }

        // Add the inner Queue to the 2D Queue
        queue_emplace(queue2D, &stringQueue, sizeof(Queue*));
    }

    // Iterate over each inner Queue and process its Strings
    while (!queue_empty(queue2D)) 
    {
        Queue** innerQueuePtr = (Queue**)queue_front(queue2D);
        Queue* innerQueue = *innerQueuePtr;

        while (!queue_empty(innerQueue)) 
        {
            String** strPtr = (String**)queue_front(innerQueue);
            String* str = *strPtr;
            
            printf("Processing: %s\n", string_c_str(str));

            // Pop the processed String
            queue_pop(innerQueue);
            string_deallocate(str);
        }

        // Pop the processed inner Queue
        queue_pop(queue2D);
        queue_deallocate(innerQueue);
    }

    // Deallocate the outer Queue
    queue_deallocate(queue2D);

    return 0;
}