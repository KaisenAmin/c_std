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
        int* arg = malloc(sizeof(int));
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
