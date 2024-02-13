#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "concurrent.h" 
#include <stddef.h>
#include <stdbool.h>

typedef int (*TaskFunction)(void*);

typedef struct Task {
    TaskFunction function;
    void* arg;
    struct Task* next;
} Task;

typedef struct TaskQueue {
    Task* front;
    Task* rear;
    int count;
    Mutex lock;
    ThreadCondition hasTasks;
} TaskQueue;

typedef struct ThreadPool {
    int numThreads;
    Thread* threads;
    TaskQueue queue;
    volatile bool keepAlive;
    volatile int numWorking;
    Mutex workMutex;
    ThreadCondition allIdle;
} ThreadPool;

ThreadPool* thread_pool_create(int num_threads);
void thread_pool_add_task(ThreadPool* pool, TaskFunction function, void* arg);
void thread_pool_wait(ThreadPool* pool);
void thread_pool_destroy(ThreadPool* pool);

#endif
