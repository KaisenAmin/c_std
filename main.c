#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define MAX_TASKS 100
#define THREAD_POOL_SIZE 4

typedef struct task {
    void (*function)(void*);
    void* arg;
} Task;

Task taskQueue[MAX_TASKS];
int queueStart = 0;  // Points to the start of the queue
int queueEnd = 0;    // Points to the end of the queue
Mutex queueMutex;
ThreadCondition queueNotEmpty;
ThreadCondition queueNotFull;
int threadPoolShutdown = 0;

void enqueueTask(void (*function)(void*), void* arg) {
    mutex_lock(&queueMutex);
    while ((queueEnd + 1) % MAX_TASKS == queueStart) { // Wait if the queue is full
        condition_wait(&queueNotFull, &queueMutex);
    }
    taskQueue[queueEnd].function = function;
    taskQueue[queueEnd].arg = arg;
    queueEnd = (queueEnd + 1) % MAX_TASKS;

    condition_signal(&queueNotEmpty);
    mutex_unlock(&queueMutex);
}

Task dequeueTask() {
    Task task;
    mutex_lock(&queueMutex);
    while (queueStart == queueEnd && !threadPoolShutdown) { // Wait if the queue is empty
        condition_wait(&queueNotEmpty, &queueMutex);
    }
    if (threadPoolShutdown) {
        mutex_unlock(&queueMutex);
        thread_exit(0);
    }
    task = taskQueue[queueStart];
    queueStart = (queueStart + 1) % MAX_TASKS;
    condition_signal(&queueNotFull);
    mutex_unlock(&queueMutex);

    return task;
}

int worker(void* arg) {
    (void)arg; // Unused parameter
    while (1) {
        Task task = dequeueTask();
        if (task.function) {
            task.function(task.arg);
        }
    }
    return 0; 
}

void initializeThreadPool() {
    mutex_init(&queueMutex, MUTEX_PLAIN);
    condition_init(&queueNotEmpty);
    condition_init(&queueNotFull);

    Thread threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        thread_create(&threads[i], worker, NULL);
    }
}

void shutdownThreadPool() {
    mutex_lock(&queueMutex);
    threadPoolShutdown = 1;
    condition_broadcast(&queueNotEmpty); // Wake up all workers for shutdown
    mutex_unlock(&queueMutex);
}

void exampleTask(void* arg) {
    int* num = (int*)arg;
    fmt_printf("Processing task: %d\n", *num);
    free(arg);
}

int main(void) {
    initializeThreadPool();

    // Submit tasks to the pool
    for (int i = 0; i < 20; ++i) {
        int* arg = (int*) malloc(sizeof(int));
        *arg = i;
        enqueueTask(exampleTask, arg);
    }

    // Give some time for tasks to be processed
    thread_sleep(&(struct timespec){.tv_sec = 5, .tv_nsec = 0}, NULL);

    fmt_printf("After Proccess\n");
    shutdownThreadPool();

    return 0;
}
