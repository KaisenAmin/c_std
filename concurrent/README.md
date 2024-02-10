

## Example 1 : Producer-Consumer Synchronization 

```c 
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define BUFFER_EMPTY (-1)

int sharedBuffer = BUFFER_EMPTY;
Mutex bufferMutex;
ThreadCondition bufferNotEmpty;
ThreadCondition bufferNotFull;

int producer(void* arg) {
    (void)arg; // To avoid unused parameter warning
    for (int i = 0; i < 10; ++i) {
        mutex_lock(&bufferMutex);
        while (sharedBuffer != BUFFER_EMPTY) {
            condition_wait(&bufferNotEmpty, &bufferMutex);
        }
        sharedBuffer = i;
        fmt_printf("Produced: %d\n", i);
        condition_signal(&bufferNotFull);
        mutex_unlock(&bufferMutex);
    }
    return 0; 
}

int consumer(void* arg) {
    (void)arg; // To avoid unused parameter warning
    for (int i = 0; i < 10; ++i) {
        mutex_lock(&bufferMutex);
        while (sharedBuffer == BUFFER_EMPTY) {
            condition_wait(&bufferNotFull, &bufferMutex);
        }
        fmt_printf("Consumed: %d\n", sharedBuffer);
        sharedBuffer = BUFFER_EMPTY;
        condition_signal(&bufferNotEmpty);
        mutex_unlock(&bufferMutex);
    }
    return 0; 
}

int main(void) {
    Thread producerThread, consumerThread;

    mutex_init(&bufferMutex, MUTEX_PLAIN);
    condition_init(&bufferNotEmpty);
    condition_init(&bufferNotFull);

    thread_create(&producerThread, (ThreadStart)producer, NULL);
    thread_create(&consumerThread, (ThreadStart)consumer, NULL);

    thread_join(producerThread, NULL);
    thread_join(consumerThread, NULL);

    mutex_destroy(&bufferMutex);
    condition_destroy(&bufferNotEmpty);
    condition_destroy(&bufferNotFull);

    return 0;
}
```

## Example 2: Dynamic Task Queue with Worker Threads

`This example demonstrates a multi-threaded task processing system using a dynamic queue. It uses mutexes and condition variables to safely enqueue and dequeue tasks in a shared task queue. Each task consists of a function pointer and an argument, allowing for varied task execution. Worker threads continuously dequeue and execute tasks. The program showcases thread synchronization, dynamic memory allocation for task arguments, and safe multi-threaded access to a shared resource (the task queue).`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <stdlib.h>

#define MAX_TASKS 100
#define NUM_THREADS 5

typedef struct task {
    void (*function)(void*);
    void* arg;
} Task;

Task taskQueue[MAX_TASKS];
int taskCount = 0;
Mutex queueMutex;
ThreadCondition hasTasks;

void enqueueTask(void (*function)(void*), void* arg) {
    mutex_lock(&queueMutex);
    if (taskCount < MAX_TASKS) {
        Task newTask;

        newTask.function = function;
        newTask.arg = arg;
        taskQueue[taskCount++] = newTask;
        
        condition_signal(&hasTasks);
    }
    mutex_unlock(&queueMutex);
}

Task* dequeueTask() {
    Task* task = NULL;
    if (taskCount > 0) {
        task = &taskQueue[--taskCount];
    }
    return task;
}

int worker(void* arg) {
    (void)arg; // Unused parameter
    while (1) {
        mutex_lock(&queueMutex);
        while (taskCount == 0) {
            condition_wait(&hasTasks, &queueMutex);
        }
        Task* task = dequeueTask();
        mutex_unlock(&queueMutex);
        if (task) {
            task->function(task->arg);
        }
    }
    return -1;
}

void exampleTask(void* arg) {
    int* num = (int*)arg;
    fmt_printf("Executing task: %d\n", *num);

    free(arg); 
}

int main(void) {
    Thread threads[NUM_THREADS];
    struct timespec remaining;

    mutex_init(&queueMutex, MUTEX_PLAIN);
    condition_init(&hasTasks);

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_create(&threads[i], worker, NULL);
    }

    for (int i = 0; i < 20; ++i) {
        int* arg = (int*) malloc(sizeof(int));
        *arg = i;
        enqueueTask(exampleTask, arg);
    }

    thread_sleep(&(struct timespec){.tv_sec = 2, .tv_nsec = 0}, &remaining); // Wait for tasks to complete

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_detach(threads[i]); // Detach threads for cleanup
    }

    mutex_destroy(&queueMutex);
    condition_destroy(&hasTasks);

    return 0;
}
```
