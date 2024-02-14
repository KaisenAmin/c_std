#include "thread_pool.h"
#include "../fmt/fmt.h"

static int thread_pool_worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    fmt_printf("Worker thread started\n");
    while (pool->keepAlive) {
        mutex_lock(&pool->queue.lock);
        while (pool->queue.count == 0 && pool->keepAlive) {
            fmt_printf("Worker thread waiting for tasks\n");
            condition_wait(&pool->queue.hasTasks, &pool->queue.lock);
        }
        if (!pool->keepAlive) {
            fmt_printf("Worker thread stopping\n");
            mutex_unlock(&pool->queue.lock);
            break;
        }
        Task* task = pool->queue.front;
        if(task != NULL) { // Ensure task is not NULL
            pool->queue.front = task->next;
            pool->queue.count--;
            fmt_printf("Worker thread executing task\n");
            mutex_unlock(&pool->queue.lock); // Move mutex unlock here to ensure it's unlocked before task execution

            if (task->function) {
                task->function(task->arg);
            }
            free(task);
        } else {
            mutex_unlock(&pool->queue.lock);
        }
    }
    mutex_lock(&pool->workMutex);
    pool->numWorking--;
    fmt_printf("Worker thread decreasing working count\n");
    if (pool->numWorking == 0) {
        condition_signal(&pool->allIdle);
    }
    mutex_unlock(&pool->workMutex);
    fmt_printf("Worker thread exited\n");
    return 0;
}


// Initialize TaskQueue
static void task_queue_init(TaskQueue* queue) {
    queue->front = queue->rear = NULL;
    queue->count = 0;
    mutex_init(&queue->lock, MUTEX_PLAIN);
    condition_init(&queue->hasTasks);
}

// Add task to TaskQueue
static void task_queue_push(TaskQueue* queue, Task* task) {
    mutex_lock(&queue->lock);
    if (queue->rear == NULL) {
        queue->front = queue->rear = task;
    } 
    else {
        queue->rear->next = task;
        queue->rear = task;
    }

    task->next = NULL;
    queue->count++;

    condition_signal(&queue->hasTasks);
    mutex_unlock(&queue->lock);
}

// Create ThreadPool
ThreadPool* thread_pool_create(int num_threads) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (!pool) {
        fmt_fprintf(stderr, "Could not allocate memory for ThreadPool\n");
        return NULL;
    }

    pool->numThreads = num_threads;
    pool->keepAlive = true;
    pool->numWorking = 0;
    mutex_init(&pool->workMutex, MUTEX_PLAIN);
    condition_init(&pool->allIdle);

    task_queue_init(&pool->queue);

    pool->threads = (Thread*)malloc(num_threads * sizeof(Thread));
    if (!pool->threads) {
        fmt_fprintf(stderr, "Could not allocate memory for worker threads\n");
        free(pool);
        return NULL;
    }

    for (int i = 0; i < num_threads; ++i) {
        if (thread_create(&pool->threads[i], thread_pool_worker, pool) != THREAD_SUCCESS) {
            fmt_fprintf(stderr, "Failed to create worker thread\n");
            thread_pool_destroy(pool);

            return NULL;
        }
    }

    return pool;
}

void thread_pool_add_task(ThreadPool* pool, TaskFunction function, void* arg) {
    Task* newTask = (Task*)malloc(sizeof(Task));
    if (newTask == NULL) {
        fmt_fprintf(stderr, "Error allocating memory for new task.\n");
        return;
    }

    newTask->function = function;
    newTask->arg = arg;
    newTask->next = NULL;

    task_queue_push(&pool->queue, newTask);
}

void thread_pool_wait(ThreadPool* pool) {
    fmt_printf("Entering thread_pool_wait\n");
    mutex_lock(&pool->workMutex);
    fmt_printf("Waiting for all tasks to complete. Tasks count: %d, Workers working: %d\n", pool->queue.count, pool->numWorking);

    while (pool->queue.count > 0 || pool->numWorking) {
        fmt_printf("Waiting on allIdle condition. Pending tasks: %d, Active workers: %d\n", pool->queue.count, pool->numWorking);
        condition_wait(&pool->allIdle, &pool->workMutex);
        fmt_printf("Woke up from allIdle condition. Checking conditions again...\n");
        // Log the condition after waking up to see if it matches expectations
        fmt_printf("After wakeup - Tasks count: %d, Workers working: %d\n", pool->queue.count, pool->numWorking);
    }

    fmt_printf("All tasks completed and all worker threads are idle.\n");
    mutex_unlock(&pool->workMutex);
    fmt_printf("Exiting thread_pool_wait\n");
}


void thread_pool_destroy(ThreadPool* pool) {
    if (pool == NULL) {
        return;
    }
    fmt_printf("Destroying ThreadPool\n");

    // Signal all worker threads to stop
    mutex_lock(&pool->queue.lock);
    pool->keepAlive = false;
    fmt_printf("Signaling all worker threads to stop\n");
    condition_broadcast(&pool->queue.hasTasks);

    mutex_unlock(&pool->queue.lock);
    
    
    // Wait for all worker threads to finish their current task and exit
    for (int i = 0; i < pool->numThreads; ++i) {
        fmt_printf("Joining thread %d\n", i);
        if (thread_join(pool->threads[i], NULL) != THREAD_SUCCESS) {
            fmt_fprintf(stderr, "Error joining thread %d\n", i);
        } 
        else {
            fmt_printf("Successfully joined thread %d\n", i);
        }
        
    }

    
    fmt_printf("All worker threads have been joined\n");

    // Free thread handles
    free(pool->threads);

    // Destroy synchronization primitives
    mutex_destroy(&pool->workMutex);
    condition_destroy(&pool->allIdle);
    mutex_destroy(&pool->queue.lock);
    condition_destroy(&pool->queue.hasTasks);

    // Clear task queue and free remaining tasks
    while (pool->queue.front != NULL) {
        Task* task = pool->queue.front;
        pool->queue.front = task->next;
        free(task);
    }

    // Finally, free the ThreadPool structure itself
    free(pool);
    fmt_printf("ThreadPool destroyed\n");
}