/**
 * @class ThreadPool
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in thread_pool.c (file-level convention).
 *
 * Simple fixed-size worker pool layered on the project's concurrent
 * primitives. Submit work with `thread_pool_add_task`, block until
 * everything drains with `thread_pool_wait`.
 */

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <stddef.h>
#include <stdbool.h>
#include "concurrent.h"


/* #define THREAD_POOL_LOGGING_ENABLE */

#ifdef THREAD_POOL_LOGGING_ENABLE
    #include <stdio.h>
    #define THREAD_POOL_LOG(fmt, ...) \
        do { fprintf(stderr, "[THREAD_POOL] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define THREAD_POOL_LOG(...) do { } while (0)
#endif



/* Task entry-point. The integer return value is currently unused but
 * kept for compatibility with `ThreadStart`. */
typedef int (*TaskFunction)(void*);

typedef struct Task {
    TaskFunction  function;
    void*         arg;
    struct Task*  next;
} Task;


typedef struct TaskQueue {
    Task*           front;
    Task*           rear;
    int             count;
    Mutex           lock;
    ThreadCondition hasTasks;
} TaskQueue;


typedef struct ThreadPool {
    int             numThreads;     /* requested thread count                       */
    int             startedThreads; /* actually created (may be < requested)        */
    Thread*         threads;
    TaskQueue       queue;
    volatile bool   keepAlive;
    int             pending;        /* queued + in-progress tasks                   */
    Mutex           workMutex;      /* protects `pending`; pairs with `allIdle`     */
    ThreadCondition allIdle;
} ThreadPool;


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

ThreadPool*  thread_pool_create               (int num_threads);
void         thread_pool_destroy              (ThreadPool* pool);


/* ------------------------------------------------------------------ */
/* Task submission / synchronization                                  */
/* ------------------------------------------------------------------ */

void         thread_pool_add_task             (ThreadPool* pool, TaskFunction function, void* arg);
void         thread_pool_wait                 (ThreadPool* pool);


/* ------------------------------------------------------------------ */
/* Checked submission / introspection / bounded wait                  */
/* ------------------------------------------------------------------ */

bool         thread_pool_add_task_checked     (ThreadPool* pool, TaskFunction function, void* arg);
int          thread_pool_pending_count        (ThreadPool* pool);
int          thread_pool_queued_count         (ThreadPool* pool);
int          thread_pool_thread_count         (ThreadPool* pool);
bool         thread_pool_wait_timeout         (ThreadPool* pool, long timeout_ms);


#endif /* _THREADPOOL_H_ */
