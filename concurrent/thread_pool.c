#include "thread_pool.h"
#include <stdlib.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <time.h>
#endif

/* Monotonic milliseconds, for timeouts that must not follow wall-clock
   steps (NTP / manual changes). */
static long long tp_now_ms(void) {
#if defined(_WIN32) || defined(_WIN64)
    return (long long)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000L;
#endif
}

/* Enqueues `task` unless the pool is shutting down (G4-11: tasks were still
   accepted, and run, while thread_pool_destroy was in progress). */
static bool tp_enqueue(ThreadPool* pool, Task* newTask) {
    mutex_lock(&pool->queue.lock);
    if (!pool->keepAlive) {
        mutex_unlock(&pool->queue.lock);
        return false;
    }
    /* `pending` goes up before the task becomes visible, so thread_pool_wait
       never sees an empty queue and pending == 0 in between. */
    mutex_lock(&pool->workMutex);
    pool->pending++;
    mutex_unlock(&pool->workMutex);

    if (pool->queue.rear == NULL) {
        pool->queue.front = pool->queue.rear = newTask;
    } else {
        pool->queue.rear->next = newTask;
        pool->queue.rear = newTask;
    }
    pool->queue.count++;
    condition_signal(&pool->queue.hasTasks);
    mutex_unlock(&pool->queue.lock);
    return true;
}

/* Worker loop:
 *   1. Wait for a task to appear (or for keepAlive to drop).
 *   2. Pop the task off the queue under queue.lock.
 *   3. Run the task with NO locks held.
 *   4. Lock workMutex, decrement `pending`, signal `allIdle` if it hit 0.
 *
 * `pending` is incremented in thread_pool_add_task and decremented here.
 * The previous implementation never incremented `numWorking`, so
 * thread_pool_wait would return while tasks were still running. */
static int thread_pool_worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    THREAD_POOL_LOG("worker started");

    for (;;) {
        mutex_lock(&pool->queue.lock);

        /* Wait until there's a task OR we've been told to shut down. */
        while (pool->queue.count == 0 && pool->keepAlive) {
            condition_wait(&pool->queue.hasTasks, &pool->queue.lock);
        }

        if (!pool->keepAlive && pool->queue.count == 0) {
            mutex_unlock(&pool->queue.lock);
            break;
        }

        Task* task = pool->queue.front;
        if (!task) {
            /* Spurious wake-up or another worker beat us to it. */
            mutex_unlock(&pool->queue.lock);
            continue;
        }
        pool->queue.front = task->next;
        if (pool->queue.front == NULL) pool->queue.rear = NULL;
        pool->queue.count--;
        mutex_unlock(&pool->queue.lock);

        /* Run the task with no locks held so it can recurse / block freely. */
        if (task->function) {
            task->function(task->arg);
        }
        free(task);

        /* Mark task complete. allIdle is signaled when no work remains. */
        mutex_lock(&pool->workMutex);
        pool->pending--;
        if (pool->pending == 0) {
            condition_broadcast(&pool->allIdle);
        }
        mutex_unlock(&pool->workMutex);
    }

    THREAD_POOL_LOG("worker exiting");
    return 0;
}

/* Initialize TaskQueue */
static void task_queue_init(TaskQueue* queue) {
    queue->front = queue->rear = NULL;
    queue->count = 0;
    mutex_init(&queue->lock, MUTEX_PLAIN);
    condition_init(&queue->hasTasks);
}

/**
 * @brief Create and start a thread pool with the requested number of worker threads.
 *
 * On success, returns a pool whose workers are blocked waiting for the first
 * task. The caller should add tasks with `thread_pool_add_task`, optionally
 * synchronize with `thread_pool_wait`, and finally release the pool with
 * `thread_pool_destroy`.
 *
 * @param num_threads Number of worker threads to spawn. Must be > 0.
 * @return Pointer to the new pool, or NULL on allocation/thread-create failure.
 */
ThreadPool* thread_pool_create(int num_threads) {
    if (num_threads <= 0) {
        THREAD_POOL_LOG("invalid num_threads %d", num_threads);
        return NULL;
    }

    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (!pool) {
        THREAD_POOL_LOG("OOM allocating ThreadPool");
        return NULL;
    }

    pool->numThreads = num_threads;
    pool->startedThreads = 0;
    pool->keepAlive = true;
    pool->pending = 0;
    mutex_init(&pool->workMutex, MUTEX_PLAIN);
    condition_init(&pool->allIdle);

    task_queue_init(&pool->queue);

    pool->threads = (Thread*)malloc((size_t)num_threads * sizeof(Thread));
    if (!pool->threads) {
        THREAD_POOL_LOG("OOM allocating threads array");
        condition_destroy(&pool->allIdle);
        mutex_destroy(&pool->workMutex);
        condition_destroy(&pool->queue.hasTasks);
        mutex_destroy(&pool->queue.lock);
        free(pool);
        return NULL;
    }

    /* Track exactly how many threads actually got created — needed so
       cleanup-on-failure (and thread_pool_destroy in general) only joins
       handles that are valid. Previously a partial-create would corrupt
       memory by trying to join uninitialized Thread slots. */
    for (int i = 0; i < num_threads; ++i) {
        if (thread_create(&pool->threads[i], thread_pool_worker, pool) != THREAD_SUCCESS) {
            THREAD_POOL_LOG("failed to create worker thread %d", i);
            /* Stop the ones we already started cleanly. */
            thread_pool_destroy(pool);
            return NULL;
        }
        pool->startedThreads++;
    }

    THREAD_POOL_LOG("pool created with %d threads", pool->startedThreads);
    return pool;
}

/**
 * @brief Submit a task to the pool. The task runs on the next available worker.
 *
 * The task struct itself is allocated internally and freed after the task
 * runs. `arg` is passed straight through and is the caller's responsibility
 * to keep alive until the task has finished executing.
 *
 * @param pool     Pool created by `thread_pool_create`. NULL is a no-op.
 * @param function Function to invoke on a worker. NULL is a no-op.
 * @param arg      Opaque argument forwarded to `function`.
 */
void thread_pool_add_task(ThreadPool* pool, TaskFunction function, void* arg) {
    if (!pool || !function) {
        THREAD_POOL_LOG("add_task: NULL pool or function");
        return;
    }

    Task* newTask = (Task*)malloc(sizeof(Task));
    if (!newTask) {
        THREAD_POOL_LOG("OOM allocating Task");
        return;
    }
    newTask->function = function;
    newTask->arg = arg;
    newTask->next = NULL;

    if (!tp_enqueue(pool, newTask)) {
        THREAD_POOL_LOG("add_task: pool is shutting down; task dropped");
        free(newTask);
    }
}


/**
 * @brief Block until every previously-added task has finished executing.
 *
 * Returns immediately if the queue is empty and no worker is busy. NULL pool
 * is a no-op. This does NOT stop the workers — the pool remains usable for
 * further `thread_pool_add_task` calls.
 *
 * @param pool Pool to wait on.
 */
void thread_pool_wait(ThreadPool* pool) {
    if (!pool) return;

    mutex_lock(&pool->workMutex);
    while (pool->pending > 0) {
        condition_wait(&pool->allIdle, &pool->workMutex);
    }
    mutex_unlock(&pool->workMutex);
}


/**
 * @brief Stop all workers, join them, and free all pool resources.
 *
 * Any tasks still queued (not yet picked up by a worker) are discarded.
 * In-flight tasks are allowed to finish first. The pool pointer is invalid
 * after this call returns. NULL is a no-op.
 *
 * @param pool Pool to destroy.
 */
void thread_pool_destroy(ThreadPool* pool) {
    if (!pool) return;
    THREAD_POOL_LOG("destroying pool");


    mutex_lock(&pool->queue.lock);
    int discarded = 0;
    while (pool->queue.front != NULL) {
        Task* task = pool->queue.front;
        pool->queue.front = task->next;
        free(task);
        discarded++;
    }
    pool->queue.rear = NULL;
    pool->queue.count = 0;
    pool->keepAlive = false;
    condition_broadcast(&pool->queue.hasTasks);
    mutex_unlock(&pool->queue.lock);

    /* Discarded tasks no longer count as pending, so a concurrent
       thread_pool_wait can return. */
    if (discarded > 0) {
        mutex_lock(&pool->workMutex);
        pool->pending -= discarded;
        if (pool->pending <= 0) {
            pool->pending = 0;
            condition_broadcast(&pool->allIdle);
        }
        mutex_unlock(&pool->workMutex);
    }

    /* Only join threads that were actually started — guards against
       partial-create failures in thread_pool_create. */
    for (int i = 0; i < pool->startedThreads; ++i) {
        if (thread_join(pool->threads[i], NULL) != THREAD_SUCCESS) {
            THREAD_POOL_LOG("error joining thread %d", i);
        }
    }

    free(pool->threads);

    /* Drain any tasks the workers didn't pick up. */
    while (pool->queue.front != NULL) {
        Task* task = pool->queue.front;
        pool->queue.front = task->next;
        free(task);
    }

    mutex_destroy(&pool->workMutex);
    condition_destroy(&pool->allIdle);
    mutex_destroy(&pool->queue.lock);
    condition_destroy(&pool->queue.hasTasks);

    free(pool);
    THREAD_POOL_LOG("pool destroyed");
}


/**
 * @brief Submit a task, reporting whether it was actually queued.
 *
 * Identical to `thread_pool_add_task`, but returns a status instead of failing
 * silently — `thread_pool_add_task` discards the task on a NULL argument or an
 * out-of-memory condition without telling the caller. In production, prefer
 * this so you can apply back-pressure or surface the error when a task cannot
 * be accepted.
 *
 * @param pool     Pool created by `thread_pool_create`.
 * @param function Function to invoke on a worker.
 * @param arg      Opaque argument forwarded to `function` (caller keeps it
 *                 alive until the task finishes).
 * @return `true` if the task was enqueued, `false` on a NULL pool/function,
 *         allocation failure, or while thread_pool_destroy is shutting the
 *         pool down (in which case nothing was queued).
 */
bool thread_pool_add_task_checked(ThreadPool* pool, TaskFunction function, void* arg) {
    if (!pool || !function) {
        THREAD_POOL_LOG("add_task_checked: NULL pool or function");
        return false;
    }

    Task* newTask = (Task*)malloc(sizeof(Task));
    if (!newTask) {
        THREAD_POOL_LOG("add_task_checked: OOM allocating Task");
        return false;
    }
    newTask->function = function;
    newTask->arg = arg;
    newTask->next = NULL;

    if (!tp_enqueue(pool, newTask)) {
        THREAD_POOL_LOG("add_task_checked: pool is shutting down; refused");
        free(newTask);
        return false;
    }
    return true;
}


/**
 * @brief Number of outstanding tasks: queued plus currently executing.
 *
 * Reaches 0 only once every submitted task has finished (the same condition
 * `thread_pool_wait` blocks on). Safe to call from any thread.
 *
 * @param pool Pool to query (NULL → 0).
 * @return Count of queued + in-flight tasks.
 */
int thread_pool_pending_count(ThreadPool* pool) {
    if (!pool) {
        return 0;
    }
    mutex_lock(&pool->workMutex);
    int n = pool->pending;
    mutex_unlock(&pool->workMutex);
    return n;
}


/**
 * @brief Number of tasks still waiting in the queue (not yet picked up).
 *
 * This is the backlog only; `thread_pool_pending_count() - thread_pool_queued_count()`
 * gives the number currently executing. Safe to call from any thread.
 *
 * @param pool Pool to query (NULL → 0).
 * @return Count of queued (not-yet-started) tasks.
 */
int thread_pool_queued_count(ThreadPool* pool) {
    if (!pool) {
        return 0;
    }
    mutex_lock(&pool->queue.lock);
    int n = pool->queue.count;
    mutex_unlock(&pool->queue.lock);
    return n;
}


/**
 * @brief Number of worker threads actually running in the pool.
 *
 * Always equals the count requested in `thread_pool_create` (which returns
 * NULL when any worker fails to start). Fixed for the pool's lifetime.
 *
 * @param pool Pool to query (NULL → 0).
 * @return Number of live worker threads.
 */
int thread_pool_thread_count(ThreadPool* pool) {
    return pool ? pool->startedThreads : 0;
}


/**
 * @brief Block until all tasks finish, or until @p timeout_ms elapses.
 *
 * Like `thread_pool_wait`, but bounded — it returns `false` if the deadline
 * passes while work is still outstanding, so a stuck task can't hang a
 * graceful shutdown forever. The pool remains usable on either outcome.
 *
 * The timeout is measured on a monotonic clock (wall-clock steps do not
 * stretch or cut it short); a negative timeout is treated as 0.
 *
 * @param pool       Pool to wait on (NULL → `true`, nothing to wait for).
 * @param timeout_ms Maximum time to wait, in milliseconds.
 * @return `true` if the pool drained (no pending tasks), `false` on timeout.
 */
bool thread_pool_wait_timeout(ThreadPool* pool, long timeout_ms) {
    if (!pool) {
        return true;
    }
    if (timeout_ms < 0) {
        timeout_ms = 0;   /* a negative value gave an unnormalized tv_nsec */
    }
    long long deadline_ms = tp_now_ms() + timeout_ms;

    mutex_lock(&pool->workMutex);
    while (pool->pending > 0) {
        long long left = deadline_ms - tp_now_ms();
        if (left <= 0) {
            break;
        }
        /* condition_timedwait wants an absolute wall-clock time: wait in
           slices of at most 50 ms, re-checking the monotonic deadline, so a
           clock step can only shift one slice. */
        long slice = (left > 50) ? 50 : (long)left;
        struct timespec until;
        timespec_get(&until, TIME_UTC);
        until.tv_nsec += slice * 1000000L;
        if (until.tv_nsec >= 1000000000L) {
            until.tv_sec  += 1;
            until.tv_nsec -= 1000000000L;
        }
        int rc = condition_timedwait(&pool->allIdle, &pool->workMutex, &until);
        if (rc != THREAD_SUCCESS && rc != THREAD_TIMEOUT) {
            break;   /* an error */
        }
    }
    bool drained = (pool->pending == 0);
    mutex_unlock(&pool->workMutex);
    return drained;
}
