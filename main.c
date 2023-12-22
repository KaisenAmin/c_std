#include "priority_queue/priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Task Task;

struct Task
{
    int taskID;
    int priority;
};

static int compare_tasks(const void* a, const void* b) 
{
    const Task* taskA = a;
    const Task* taskB = b;

    return (taskA->priority > taskB->priority) - (taskA->priority < taskB->priority);
}

int main() 
{
    PriorityQueue* taskQueue = priority_queue_create(sizeof(Task), compare_tasks);

    if (!taskQueue) 
    {
        fprintf(stderr, "Failed to create task queue.\n");
        return EXIT_FAILURE;
    }

    // Define some tasks with different priorities
    Task tasks[] = {
        {101, 3}, {102, 2}, {103, 1}, {104, 3}, {105, 2}
    };

    for (size_t i = 0; i < sizeof(tasks) / sizeof(tasks[0]); ++i) 
        priority_queue_push(taskQueue, &tasks[i]);

    printf("Executing tasks in priority order:\n");
    while (!priority_queue_empty(taskQueue)) 
    {
        Task* topTask = priority_queue_top(taskQueue);
        printf("Executing Task ID: %d, Priority: %d\n", topTask->taskID, topTask->priority);
        priority_queue_pop(taskQueue);
    }

    priority_queue_deallocate(taskQueue);

    return EXIT_SUCCESS;
}
