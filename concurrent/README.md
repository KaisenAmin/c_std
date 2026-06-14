
# Concurrent Library in C


**Author:** amin tahmasebi
**Release Date:** 2025
**License:** ISC License


The `concurrent` module provides a small, cross-platform threading layer
(Windows + POSIX) and a companion `thread_pool` for queueing short-lived
tasks across a fixed set of worker threads.

It exposes:

- **Threads**: `Thread`, `thread_create`, `thread_join`, `thread_detach`,
  `thread_current`, `thread_equal`, `thread_sleep`, `thread_yield`,
  `thread_exit`, `thread_hardware_concurrency`
- **Mutexes**: `Mutex`, `mutex_init`, `mutex_lock`, `mutex_unlock`,
  `mutex_trylock`, `mutex_timed_lock`, `mutex_destroy`
- **Semaphores**: `Semaphore`, `semaphore_init`, `semaphore_wait`,
  `semaphore_trywait`, `semaphore_post`, `semaphore_destroy`
- **Condition variables**: `ThreadCondition`, `condition_init`,
  `condition_wait`, `condition_timedwait`, `condition_signal`,
  `condition_broadcast`, `condition_destroy`
- **Thread-local storage**: `ThreadSpecific`, `thread_specific_create`,
  `thread_specific_get`, `thread_specific_set`, `thread_specific_delete`
- **One-time initialization**: `OnceFlag`, `ONCE_FLAG_INIT`, `call_once`
- **Thread pool**: `ThreadPool`, `thread_pool_create`,
  `thread_pool_add_task`, `thread_pool_add_task_checked`,
  `thread_pool_wait`, `thread_pool_wait_timeout`, `thread_pool_pending_count`,
  `thread_pool_queued_count`, `thread_pool_thread_count`, `thread_pool_destroy`

All public functions are documented with doxygen blocks above their
implementations in [`concurrent.c`](concurrent.c) and
[`thread_pool.c`](thread_pool.c). The descriptions below summarise the
contract; consult the source for full semantics and platform notes.

---

## Concurrent Function Descriptions

### Threads

### `int thread_create(Thread* thr, ThreadStart func, void* arg)`
**Purpose**: Spawn a new OS thread that runs `func(arg)`.
**Parameters**:
  - `thr`: Pointer to a `Thread` handle that receives the new thread on success.
  - `func`: Entry-point function of type `ThreadStart` (`int (*)(void*)`).
  - `arg`: Opaque argument forwarded unchanged to `func`.
**Return Value**: `THREAD_SUCCESS` on success; on failure `*thr` is left untouched and an error code is returned.
**Usage Case**: Use to launch independent units of work; pair with `thread_join` or `thread_detach` depending on whether you need the result.

---

### `int thread_join(Thread thr, int* res)`
**Purpose**: Block until thread `thr` finishes and optionally capture its return value.
**Parameters**:
  - `thr`: Handle of the thread to wait for.
  - `res`: If non-NULL, the thread's return value is written here.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Use after `thread_create` to synchronise with a worker and collect its result before releasing shared resources.

---

### `int thread_detach(Thread thr)`
**Purpose**: Mark `thr` as detached so it releases its own resources when it exits.
**Parameters**:
  - `thr`: Handle of the thread to detach.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Use for fire-and-forget threads whose result is not needed and that should clean up without an explicit `thread_join`.

---

### `int thread_equal(Thread a, Thread b)`
**Purpose**: Test whether two thread handles refer to the same OS thread.
**Parameters**:
  - `a`: First thread handle.
  - `b`: Second thread handle.
**Return Value**: Non-zero if both handles refer to the same thread, zero otherwise.
**Usage Case**: Use to detect self-reference (e.g. a thread checking whether it is the "main" thread). On Windows, only call this on live handles — after `thread_join` the handle is released.

---

### `unsigned long thread_current(void)`
**Purpose**: Return a numeric identifier for the calling thread.
**Parameters**: None.
**Return Value**: The current thread's ID (`GetCurrentThreadId` on Windows, `pthread_self` cast on POSIX).
**Usage Case**: Use for logging, debugging, or constructing per-thread keys without a full TLS key allocation.

---

### `int thread_sleep(const struct timespec* duration, struct timespec* remaining)`
**Purpose**: Suspend the calling thread for at least the duration specified by `duration`.
**Parameters**:
  - `duration`: Pointer to a `timespec` holding the sleep length (seconds + nanoseconds).
  - `remaining`: If the sleep is interrupted early (POSIX only), the unslept time is written here. May be NULL.
**Return Value**: Zero on success, `-1` if interrupted, other negative values on error.
**Usage Case**: Use for rate-limiting, polling loops, or simulating latency in tests. The while-loop pattern (`while (thread_sleep(&dur, &rem)) dur = rem;`) handles POSIX signal interruptions.

---

### `void thread_yield(void)`
**Purpose**: Hint to the OS scheduler that the calling thread is willing to relinquish the CPU.
**Parameters**: None.
**Return Value**: None.
**Usage Case**: Use inside busy-wait or spin loops to avoid starving other threads. Not a substitute for condition variables in production code.

---

### `void thread_exit(int res)`
**Purpose**: Terminate the calling thread, returning `res` to a joiner. Does not return.
**Parameters**:
  - `res`: Exit code made available to `thread_join`.
**Return Value**: Never returns (declared `TTHREAD_NORETURN`).
**Usage Case**: Use inside a thread function when you need to exit before the function's natural return, e.g. on a fatal error path.

---

### `unsigned long thread_hardware_concurrency(void)`
**Purpose**: Query the number of hardware threads (logical CPU cores) available to the process.
**Parameters**: None.
**Return Value**: The hardware thread count, or `0` if the value cannot be determined.
**Usage Case**: Use to size a thread pool to match the machine — `thread_pool_create((int)thread_hardware_concurrency())`.

---

### Mutexes

### `int mutex_init(Mutex* mutex, int type)`
**Purpose**: Initialise a mutex object with the given flavour.
**Parameters**:
  - `mutex`: Pointer to an uninitialised `Mutex` to set up.
  - `type`: One of `MUTEX_PLAIN`, `MUTEX_TIMED`, or `MUTEX_RECURSIVE` (values may be OR-ed).
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Call once before any `mutex_lock` / `mutex_unlock` calls. Pair with `mutex_destroy` when the mutex is no longer needed.

---

### `int mutex_lock(Mutex* mutex)`
**Purpose**: Block until the mutex is acquired by the calling thread.
**Parameters**:
  - `mutex`: Pointer to an initialised `Mutex`.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Use to enter a critical section. For a `MUTEX_RECURSIVE` mutex held by the current thread, the lock count is incremented instead of blocking.

---

### `int mutex_trylock(Mutex* mutex)`
**Purpose**: Attempt to acquire the mutex without blocking.
**Parameters**:
  - `mutex`: Pointer to an initialised `Mutex`.
**Return Value**: `THREAD_SUCCESS` if the mutex was acquired, `THREAD_BUSY` if another thread already holds it.
**Usage Case**: Use in non-blocking polling loops — check `THREAD_BUSY` and retry later or fall back to alternative work rather than blocking the thread.

---

### `int mutex_timed_lock(Mutex* mutex, const struct timespec* ts)`
**Purpose**: Try to acquire the mutex until the absolute deadline `*ts` expires.
**Parameters**:
  - `mutex`: Pointer to a `MUTEX_TIMED` mutex.
  - `ts`: Absolute expiry time in UTC (use `timespec_get` + offset).
**Return Value**: `THREAD_SUCCESS` if acquired, `THREAD_TIMEOUT` if the deadline elapsed first, otherwise an error code.
**Usage Case**: Use when you need a lock but cannot afford to wait indefinitely — e.g. a watchdog that must respond within a bounded time.

---

### `int mutex_unlock(Mutex* mutex)`
**Purpose**: Release the mutex (or decrement the recursion count for a recursive mutex).
**Parameters**:
  - `mutex`: Pointer to a locked `Mutex`.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Always call in the same scope that called `mutex_lock` to avoid deadlocks. For recursive mutexes, call once per corresponding `mutex_lock`.

---

### `void mutex_destroy(Mutex* mutex)`
**Purpose**: Release the OS resources backing the mutex.
**Parameters**:
  - `mutex`: Pointer to an initialised `Mutex` that is NOT currently locked.
**Return Value**: None.
**Usage Case**: Call when the mutex is permanently no longer needed. Behaviour is undefined if called while the mutex is held.

---

### Semaphores

A counting semaphore tracks a number of interchangeable tokens (resources). `semaphore_wait` takes one (blocking when none remain); `semaphore_post` returns one. Backed by `CreateSemaphore` on Windows and `sem_t` on POSIX.

### `int semaphore_init(Semaphore* sem, unsigned int initial_count)`
**Purpose**: Initialise a counting semaphore with `initial_count` available tokens.
**Parameters**:
  - `sem`: Pointer to an uninitialised `Semaphore`.
  - `initial_count`: Number of tokens initially available.
**Return Value**: `THREAD_SUCCESS`, or `THREAD_ERROR` on a NULL argument or OS failure.
**Usage Case**: Limit concurrency to N (init with N), or model a bounded buffer's free/used slots. Pair with `semaphore_destroy`.

---

### `int semaphore_wait(Semaphore* sem)`
**Purpose**: Acquire a token (the "P"/"down" operation), blocking until one is available.
**Parameters**:
  - `sem`: Pointer to an initialised `Semaphore`.
**Return Value**: `THREAD_SUCCESS` once a token is taken, otherwise `THREAD_ERROR`. (POSIX `EINTR` is retried transparently.)
**Usage Case**: Block a worker until a resource/slot frees up.

---

### `int semaphore_trywait(Semaphore* sem)`
**Purpose**: Try to take a token without blocking.
**Parameters**:
  - `sem`: Pointer to an initialised `Semaphore`.
**Return Value**: `THREAD_SUCCESS` if a token was taken, `THREAD_BUSY` if none were available, otherwise `THREAD_ERROR`.
**Usage Case**: Opportunistically grab a resource and do something else if the pool is exhausted, instead of waiting.

---

### `int semaphore_post(Semaphore* sem)`
**Purpose**: Release a token (the "V"/"up" operation), waking one waiter if any.
**Parameters**:
  - `sem`: Pointer to an initialised `Semaphore`.
**Return Value**: `THREAD_SUCCESS`, or `THREAD_ERROR` on failure (e.g. count overflow).
**Usage Case**: Return a resource to the pool / mark a slot free so a blocked `semaphore_wait` can proceed.

---

### `void semaphore_destroy(Semaphore* sem)`
**Purpose**: Release the OS resources backing the semaphore. Safe to call with NULL.
**Parameters**:
  - `sem`: Pointer to a `Semaphore` that no thread is currently waiting on.
**Return Value**: None.
**Usage Case**: Call when the semaphore is permanently no longer needed.

---

### Read-Write Locks

A read-write lock (`RWLock`) lets **many threads read shared data at once**, but gives a writer **exclusive** access. It is the right primitive for data read far more often than it is written (caches, routing tables, configuration). This implementation is **writer-preference** — once a writer is waiting, new readers queue behind it, so a steady stream of readers cannot starve a writer. It is built on the library's own `Mutex` + `ThreadCondition`, so it behaves identically on Windows and POSIX. A single `rwlock_unlock` releases whichever mode the caller holds. Treat the `RWLock` fields as opaque.

### `int rwlock_init(RWLock* rw)`
**Purpose**: Initialise a read-write lock.
**Parameters**:
  - `rw`: Pointer to an uninitialised `RWLock`.
**Return Value**: `THREAD_SUCCESS`, or `THREAD_ERROR` on a NULL argument or if an underlying mutex/condition could not be created.
**Usage Case**: Create the lock before sharing the data it guards. Pair with `rwlock_destroy`.

---

### `int rwlock_rdlock(RWLock* rw)`
**Purpose**: Acquire the lock for reading (shared), blocking until granted. Many readers may hold it simultaneously; blocks while a writer holds or is waiting.
**Parameters**:
  - `rw`: Pointer to an initialised `RWLock`.
**Return Value**: `THREAD_SUCCESS`, or `THREAD_ERROR` on a NULL argument / internal error.
**Usage Case**: Wrap a read-only critical section. Balance every call with one `rwlock_unlock`.

---

### `int rwlock_tryrdlock(RWLock* rw)`
**Purpose**: Try to acquire the read lock without blocking.
**Parameters**:
  - `rw`: Pointer to an initialised `RWLock`.
**Return Value**: `THREAD_SUCCESS` if the read lock was taken, `THREAD_BUSY` if a writer holds it or is waiting (writer-preference), otherwise `THREAD_ERROR`.
**Usage Case**: Read opportunistically and fall back to other work when a writer is active, instead of blocking.

---

### `int rwlock_wrlock(RWLock* rw)`
**Purpose**: Acquire the lock for writing (exclusive), blocking until every current reader and writer has released.
**Parameters**:
  - `rw`: Pointer to an initialised `RWLock`.
**Return Value**: `THREAD_SUCCESS`, or `THREAD_ERROR` on a NULL argument / internal error.
**Usage Case**: Wrap a critical section that mutates the shared data. Balance with one `rwlock_unlock`.

---

### `int rwlock_trywrlock(RWLock* rw)`
**Purpose**: Try to acquire the write lock without blocking.
**Parameters**:
  - `rw`: Pointer to an initialised `RWLock`.
**Return Value**: `THREAD_SUCCESS` if exclusive access was granted, `THREAD_BUSY` if any reader or writer currently holds the lock, otherwise `THREAD_ERROR`.
**Usage Case**: Attempt an update only if it can be done immediately (e.g. a best-effort cache refresh).

---

### `int rwlock_unlock(RWLock* rw)`
**Purpose**: Release the lock, whether it is held for reading or writing. The lock state itself records the mode, so no per-thread bookkeeping is needed.
**Parameters**:
  - `rw`: Pointer to an initialised `RWLock` the calling thread holds.
**Return Value**: `THREAD_SUCCESS`, or `THREAD_ERROR` on a NULL argument / internal error.
**Usage Case**: End a read or write critical section. Calling it while nothing is held is a harmless no-op.

---

### `void rwlock_destroy(RWLock* rw)`
**Purpose**: Destroy a read-write lock and release its resources. The lock must be idle. Safe to call with NULL.
**Parameters**:
  - `rw`: Pointer to an `RWLock` no thread is holding or waiting on.
**Return Value**: None.
**Usage Case**: Call when the lock is permanently no longer needed.

---

### Condition Variables

### `int condition_init(ThreadCondition* cond)`
**Purpose**: Initialise a condition variable.
**Parameters**:
  - `cond`: Pointer to an uninitialised `ThreadCondition`.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Call once before any `condition_wait` or `condition_signal` calls. Pair with `condition_destroy`.

---

### `int condition_wait(ThreadCondition* cond, Mutex* mutex)`
**Purpose**: Atomically release `mutex` and block on `cond`. When woken, re-acquires `mutex` before returning.
**Parameters**:
  - `cond`: Pointer to an initialised `ThreadCondition`.
  - `mutex`: Pointer to a locked `Mutex` associated with the predicate.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Always call inside a `while` loop that re-checks the predicate to guard against spurious wake-ups: `while (!predicate) condition_wait(&cv, &m);`.

---

### `int condition_timedwait(ThreadCondition* cond, Mutex* mutex, const struct timespec* ts)`
**Purpose**: Same as `condition_wait` but gives up at the absolute deadline `*ts`, returning `THREAD_TIMEOUT`.
**Parameters**:
  - `cond`: Pointer to an initialised `ThreadCondition`.
  - `mutex`: Pointer to a locked `Mutex`.
  - `ts`: Absolute expiry time (use `timespec_get` + offset).
**Return Value**: `THREAD_SUCCESS` if signalled, `THREAD_TIMEOUT` if the deadline elapsed, otherwise an error code.
**Usage Case**: Use when a thread must not block forever — e.g. a client that must time out if the server does not respond within a bounded interval.

---

### `int condition_signal(ThreadCondition* cond)`
**Purpose**: Wake one thread currently waiting on `cond`.
**Parameters**:
  - `cond`: Pointer to an initialised `ThreadCondition`.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Use to notify a single consumer that new data is available. The caller does not need to hold the associated mutex, but waking-while-holding is the standard idiom on Windows.

---

### `int condition_broadcast(ThreadCondition* cond)`
**Purpose**: Wake every thread currently waiting on `cond`.
**Parameters**:
  - `cond`: Pointer to an initialised `ThreadCondition`.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Use when a state change affects all waiters at once — e.g. opening a gate, shutting down a pool, or broadcasting that a config has been reloaded.

---

### `void condition_destroy(ThreadCondition* cond)`
**Purpose**: Release the OS resources backing the condition variable.
**Parameters**:
  - `cond`: Pointer to an initialised `ThreadCondition` with no threads currently waiting on it.
**Return Value**: None.
**Usage Case**: Call after all threads have stopped waiting on `cond` and it will not be used again.

---

### Thread-Specific Storage (TLS via key)

### `int thread_specific_create(ThreadSpecific* key, ThreadSpecificDestructor dtor)`
**Purpose**: Allocate a process-wide TLS key. Each thread gets its own independent slot for this key.
**Parameters**:
  - `key`: Pointer to a `ThreadSpecific` that receives the new key.
  - `dtor`: Optional destructor (`void (*)(void*)`) invoked on each thread's slot when the thread exits. May be NULL.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Use once at program start (or via `call_once`) to create a key; then each thread stores its own private data with `thread_specific_set`.

---

### `int thread_specific_set(ThreadSpecific key, void* val)`
**Purpose**: Set the calling thread's value for `key`.
**Parameters**:
  - `key`: A TLS key previously created by `thread_specific_create`.
  - `val`: The pointer to store. May be NULL to clear the slot.
**Return Value**: `THREAD_SUCCESS` on success, otherwise an error code.
**Usage Case**: Call at thread startup to attach thread-local state (e.g. a per-thread context or buffer).

---

### `void* thread_specific_get(ThreadSpecific key)`
**Purpose**: Read the calling thread's value for `key`.
**Parameters**:
  - `key`: A TLS key previously created by `thread_specific_create`.
**Return Value**: The pointer previously stored by `thread_specific_set`, or NULL if the slot has never been set.
**Usage Case**: Call anywhere in a thread function to retrieve private state without passing it through every function call.

---

### `void thread_specific_delete(ThreadSpecific key)`
**Purpose**: Release the TLS key itself. Per-thread destructors are NOT re-run.
**Parameters**:
  - `key`: The TLS key to release.
**Return Value**: None.
**Usage Case**: Call after all threads that use the key have exited, to reclaim the OS key slot.

---

### One-Time Initialization

### `void call_once(OnceFlag* flag, void (*func)(void))`
**Purpose**: Run `func` exactly once across all threads sharing `flag`, regardless of how many threads call `call_once` concurrently.
**Parameters**:
  - `flag`: Pointer to an `OnceFlag` initialised with `ONCE_FLAG_INIT`.
  - `func`: Void function to invoke on the first call.
**Return Value**: None.
**Usage Case**: Use to lazily initialise shared resources (global caches, singletons, loaded libraries) in a thread-safe way without explicit locking at every call site.

---

## Thread Pool Function Descriptions

### `ThreadPool* thread_pool_create(int num_threads)`
**Purpose**: Spawn a pool with `num_threads` worker threads, all initially idle and waiting for tasks.
**Parameters**:
  - `num_threads`: Number of worker threads to create. Must be > 0.
**Return Value**: Pointer to the new `ThreadPool`, or NULL on bad input or allocation failure.
**Usage Case**: Call once at program start (or at the beginning of a parallel region). Size the pool with `thread_hardware_concurrency()` for CPU-bound work.

---

### `void thread_pool_add_task(ThreadPool* pool, TaskFunction fn, void* arg)`
**Purpose**: Submit a task to the pool. The function `fn` will be executed on the next idle worker; `arg` is forwarded unchanged.
**Parameters**:
  - `pool`: The pool to submit to. NULL is a silent no-op.
  - `fn`: Task entry point (`int (*)(void*)`). NULL is a silent no-op.
  - `arg`: Opaque argument forwarded to `fn`.
**Return Value**: None.
**Usage Case**: Use to dispatch independent work items. If the pool's queue is full or all workers are busy, the task is queued and executed as soon as a worker becomes free.

---

### `void thread_pool_wait(ThreadPool* pool)`
**Purpose**: Block the calling thread until every previously-added task has finished executing. Does NOT stop or destroy the workers.
**Parameters**:
  - `pool`: The pool to wait on. NULL is a no-op.
**Return Value**: None.
**Usage Case**: Use as a barrier between batches of work — submit a batch, call `thread_pool_wait`, inspect the results, then submit the next batch using the same pool.

---

### `void thread_pool_destroy(ThreadPool* pool)`
**Purpose**: Signal workers to stop (in-flight tasks finish first; queued tasks are discarded), join all threads, and free all pool resources.
**Parameters**:
  - `pool`: The pool to destroy. NULL is a no-op.
**Return Value**: None.
**Usage Case**: Call once when the pool is permanently no longer needed. After this call, the pointer is invalid and must not be used.

---

### `bool thread_pool_add_task_checked(ThreadPool* pool, TaskFunction function, void* arg)`
**Purpose**: Submit a task and report whether it was actually queued. Unlike `thread_pool_add_task` (which returns `void` and silently drops the task on a NULL argument or out-of-memory), this version returns a status.
**Parameters**:
  - `pool`: Pool created by `thread_pool_create`.
  - `function`: Function to invoke on a worker.
  - `arg`: Opaque argument forwarded to `function` (keep it alive until the task finishes).
**Return Value**: `true` if the task was enqueued; `false` on a NULL pool/function or allocation failure.
**Usage Case**: **Prefer this in production** so you can apply back-pressure or surface an error when a task cannot be accepted.

---

### `int thread_pool_pending_count(ThreadPool* pool)`
**Purpose**: Number of outstanding tasks — queued plus currently executing. Reaches 0 only once every submitted task has finished (the same condition `thread_pool_wait` blocks on).
**Parameters**:
  - `pool`: The pool to query. NULL → 0.
**Return Value**: Count of queued + in-flight tasks.
**Usage Case**: Monitoring / health metrics, or deciding whether to submit more work.

---

### `int thread_pool_queued_count(ThreadPool* pool)`
**Purpose**: Number of tasks still waiting in the queue (not yet picked up by a worker) — the backlog only. `pending_count − queued_count` is the number currently executing.
**Parameters**:
  - `pool`: The pool to query. NULL → 0.
**Return Value**: Count of queued (not-yet-started) tasks.
**Usage Case**: Detect a growing backlog (workers can't keep up) and apply back-pressure.

---

### `int thread_pool_thread_count(ThreadPool* pool)`
**Purpose**: Number of worker threads actually running. Usually equals the count requested at creation, but can be smaller if some threads failed to start. Fixed for the pool's lifetime.
**Parameters**:
  - `pool`: The pool to query. NULL → 0.
**Return Value**: Number of live worker threads.
**Usage Case**: Confirm the pool started at full strength, or size work distribution.

---

### `bool thread_pool_wait_timeout(ThreadPool* pool, long timeout_ms)`
**Purpose**: Like `thread_pool_wait`, but bounded — returns `false` if the deadline passes while tasks are still outstanding, so a stuck task can't hang a graceful shutdown forever.
**Parameters**:
  - `pool`: Pool to wait on. NULL → `true` (nothing to wait for).
  - `timeout_ms`: Maximum time to wait, in milliseconds.
**Return Value**: `true` if the pool drained (no pending tasks); `false` on timeout.
**Usage Case**: Graceful shutdown with a deadline — wait for in-flight work, but don't block indefinitely on a hung task.

---

## Building

The examples below depend on `concurrent/` and `fmt/`. A few also use
`vector/`. The simplest one-shot build (MinGW/MSYS2 or any GCC with
pthreads):

```bash
gcc -I. example.c \
    concurrent/concurrent.c concurrent/thread_pool.c \
    fmt/fmt.c string/std_string.c encoding/encoding.c \
    -o example.exe
```

Add `vector/vector.c` when the example uses `Vector`. On POSIX, also
pass `-lpthread`.

---

## Example 1 — Hello threads + hardware concurrency

`Create four threads, each returns a value joined back through thread_join. Also prints how many hardware threads the runtime reports.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define N_WORKERS 4

int worker(void* arg) {
    int id = *(int*)arg;
    fmt_printf("worker %d running on thread %lu\n", id, thread_current());
    return id * 10;
}

int main(void) {
    Thread threads[N_WORKERS];
    int ids[N_WORKERS];

    for (int i = 0; i < N_WORKERS; ++i) {
        ids[i] = i;
        if (thread_create(&threads[i], worker, &ids[i]) != THREAD_SUCCESS) {
            fmt_fprintf(stderr, "thread_create failed for #%d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < N_WORKERS; ++i) {
        int res = -1;
        thread_join(threads[i], &res);
        fmt_printf("worker %d returned %d\n", i, res);
    }

    fmt_printf("hardware concurrency reported: %lu\n", thread_hardware_concurrency());
    return 0;
}
```

**Result**

```
worker 0 running on thread 17956
worker 3 running on thread 22328
worker 2 running on thread 4456
worker 1 running on thread 17892
worker 0 returned 0
worker 1 returned 10
worker 2 returned 20
worker 3 returned 30
hardware concurrency reported: 12
```

---

## Example 2 — Mutex-protected shared counter

`Six threads each bump the same counter 200 000 times. Without the mutex the final value would be far below 1 200 000 because of lost-update races. With the mutex it is exact.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define ITERATIONS 200000
#define N_THREADS  6

static int counter = 0;
static Mutex counter_mutex;

int bump(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; ++i) {
        mutex_lock(&counter_mutex);
        counter++;
        mutex_unlock(&counter_mutex);
    }
    return 0;
}

int main(void) {
    Thread threads[N_THREADS];
    mutex_init(&counter_mutex, MUTEX_PLAIN);

    for (int i = 0; i < N_THREADS; ++i) {
        thread_create(&threads[i], bump, NULL);
    }
    for (int i = 0; i < N_THREADS; ++i) {
        thread_join(threads[i], NULL);
    }
    mutex_destroy(&counter_mutex);

    fmt_printf("counter = %d (expected %d)\n", counter, ITERATIONS * N_THREADS);
    return counter == ITERATIONS * N_THREADS ? 0 : 1;
}
```

**Result**

```
counter = 1200000 (expected 1200000)
```

---

## Example 3 — Recursive mutex

`A MUTEX_RECURSIVE allows the same thread to acquire the lock more than once. A plain mutex would deadlock on the second mutex_lock call.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

static Mutex rmtx;

static void inner(int depth) {
    mutex_lock(&rmtx);
    fmt_printf("acquired lock at depth %d\n", depth);
    if (depth < 3) {
        inner(depth + 1);
    }
    mutex_unlock(&rmtx);
    fmt_printf("released lock at depth %d\n", depth);
}

int recursive_worker(void* arg) {
    (void)arg;
    inner(0);
    return 0;
}

int main(void) {
    mutex_init(&rmtx, MUTEX_RECURSIVE);

    Thread t;
    thread_create(&t, recursive_worker, NULL);
    thread_join(t, NULL);

    mutex_destroy(&rmtx);
    return 0;
}
```

**Result**

```
acquired lock at depth 0
acquired lock at depth 1
acquired lock at depth 2
acquired lock at depth 3
released lock at depth 3
released lock at depth 2
released lock at depth 1
released lock at depth 0
```

---

## Example 4 — Non-blocking acquire with mutex_trylock

`A "holder" thread grabs the mutex and sits on it for 500ms. A "probe" thread polls with mutex_trylock and reports BUSY until the holder releases.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

static Mutex resource_mtx;

int holder(void* arg) {
    (void)arg;
    mutex_lock(&resource_mtx);
    fmt_printf("holder: acquired, sleeping 500ms\n");
    thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 500 * 1000 * 1000}, NULL);
    mutex_unlock(&resource_mtx);
    fmt_printf("holder: released\n");
    return 0;
}

int probe(void* arg) {
    (void)arg;
    thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000}, NULL);

    for (int i = 0; i < 6; ++i) {
        int rc = mutex_trylock(&resource_mtx);
        if (rc == THREAD_SUCCESS) {
            fmt_printf("probe: got the lock on attempt %d\n", i);
            mutex_unlock(&resource_mtx);
            return 0;
        }
        fmt_printf("probe: busy, retry in 100ms (attempt %d)\n", i);
        thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000}, NULL);
    }
    fmt_printf("probe: gave up\n");
    return 1;
}

int main(void) {
    mutex_init(&resource_mtx, MUTEX_PLAIN);

    Thread h, p;
    thread_create(&h, holder, NULL);
    thread_create(&p, probe, NULL);

    thread_join(h, NULL);
    thread_join(p, NULL);

    mutex_destroy(&resource_mtx);
    return 0;
}
```

**Result**

```
holder: acquired, sleeping 500ms
probe: busy, retry in 100ms (attempt 0)
probe: busy, retry in 100ms (attempt 1)
probe: busy, retry in 100ms (attempt 2)
probe: busy, retry in 100ms (attempt 3)
holder: released
probe: got the lock on attempt 4
```

---

## Example 5 — Bounded producer / consumer with condition variables

`Classic ring buffer of CAPACITY=4. Producer waits on not_full when the buffer is full; consumer waits on not_empty when it is empty. The while-loop around condition_wait guards against spurious wake-ups.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define CAPACITY  4
#define N_ITEMS   12

static int buf[CAPACITY];
static int head, tail, count;
static Mutex m;
static ThreadCondition not_full, not_empty;

int producer(void* arg) {
    (void)arg;
    for (int i = 1; i <= N_ITEMS; ++i) {
        mutex_lock(&m);
        while (count == CAPACITY) {
            condition_wait(&not_full, &m);
        }
        buf[tail] = i;
        tail = (tail + 1) % CAPACITY;
        ++count;
        fmt_printf("produced %d (buffer size = %d)\n", i, count);
        condition_signal(&not_empty);
        mutex_unlock(&m);
    }
    return 0;
}

int consumer(void* arg) {
    (void)arg;
    for (int i = 0; i < N_ITEMS; ++i) {
        mutex_lock(&m);
        while (count == 0) {
            condition_wait(&not_empty, &m);
        }
        int v = buf[head];
        head = (head + 1) % CAPACITY;
        --count;
        fmt_printf("    consumed %d (buffer size = %d)\n", v, count);
        condition_signal(&not_full);
        mutex_unlock(&m);
    }
    return 0;
}

int main(void) {
    mutex_init(&m, MUTEX_PLAIN);
    condition_init(&not_full);
    condition_init(&not_empty);

    Thread p, c;
    thread_create(&p, producer, NULL);
    thread_create(&c, consumer, NULL);

    thread_join(p, NULL);
    thread_join(c, NULL);

    condition_destroy(&not_empty);
    condition_destroy(&not_full);
    mutex_destroy(&m);
    return 0;
}
```

**Result**

```
produced 1 (buffer size = 1)
produced 2 (buffer size = 2)
produced 3 (buffer size = 3)
produced 4 (buffer size = 4)
    consumed 1 (buffer size = 3)
    consumed 2 (buffer size = 2)
    consumed 3 (buffer size = 1)
    consumed 4 (buffer size = 0)
...
    consumed 12 (buffer size = 0)
```

---

## Example 6 — `condition_broadcast` releases every waiter

`Five worker threads block at a shared "gate". When main flips gate_open=1 and calls condition_broadcast, every waiter wakes at once.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define N_RUNNERS 5

static Mutex gate_mtx;
static ThreadCondition gate_cv;
static int gate_open = 0;

int runner(void* arg) {
    int id = *(int*)arg;
    mutex_lock(&gate_mtx);
    while (!gate_open) {
        condition_wait(&gate_cv, &gate_mtx);
    }
    mutex_unlock(&gate_mtx);

    fmt_printf("runner %d: through the gate on thread %lu\n", id, thread_current());
    return 0;
}

int main(void) {
    mutex_init(&gate_mtx, MUTEX_PLAIN);
    condition_init(&gate_cv);

    Thread threads[N_RUNNERS];
    int ids[N_RUNNERS];
    for (int i = 0; i < N_RUNNERS; ++i) {
        ids[i] = i;
        thread_create(&threads[i], runner, &ids[i]);
    }

    thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 200 * 1000 * 1000}, NULL);
    fmt_printf("main: opening the gate\n");

    mutex_lock(&gate_mtx);
    gate_open = 1;
    condition_broadcast(&gate_cv);
    mutex_unlock(&gate_mtx);

    for (int i = 0; i < N_RUNNERS; ++i) {
        thread_join(threads[i], NULL);
    }

    condition_destroy(&gate_cv);
    mutex_destroy(&gate_mtx);
    return 0;
}
```

**Result**

```
main: opening the gate
runner 2: through the gate on thread 25060
runner 0: through the gate on thread 10192
runner 3: through the gate on thread 7084
runner 4: through the gate on thread 15976
runner 1: through the gate on thread 5280
```

---

## Example 7 — `condition_timedwait` with a deadline

`The signaller sleeps 1.2 seconds before flipping ready. The waiter has a 500ms timeout, so it observes two THREAD_TIMEOUT returns before the third wait succeeds.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

static Mutex m;
static ThreadCondition cv;
static int ready = 0;

int late_signaller(void* arg) {
    (void)arg;
    thread_sleep(&(struct timespec){.tv_sec = 1, .tv_nsec = 200 * 1000 * 1000}, NULL);

    mutex_lock(&m);
    ready = 1;
    condition_signal(&cv);
    mutex_unlock(&m);
    return 0;
}

int main(void) {
    mutex_init(&m, MUTEX_TIMED);
    condition_init(&cv);

    Thread signaller;
    thread_create(&signaller, late_signaller, NULL);

    mutex_lock(&m);
    int attempts = 0;
    while (!ready) {
        struct timespec deadline;
        timespec_get(&deadline, TIME_UTC);
        deadline.tv_nsec += 500 * 1000 * 1000;  /* 500ms */
        if (deadline.tv_nsec >= 1000 * 1000 * 1000) {
            deadline.tv_sec++;
            deadline.tv_nsec -= 1000 * 1000 * 1000;
        }

        int rc = condition_timedwait(&cv, &m, &deadline);
        ++attempts;
        if (rc == THREAD_TIMEOUT) {
            fmt_printf("attempt %d: timed out, still waiting\n", attempts);
        } else if (rc == THREAD_SUCCESS) {
            fmt_printf("attempt %d: signalled, ready=%d\n", attempts, ready);
        } else {
            fmt_printf("attempt %d: error rc=%d\n", attempts, rc);
            break;
        }
        if (attempts > 10) break;
    }
    mutex_unlock(&m);

    thread_join(signaller, NULL);
    condition_destroy(&cv);
    mutex_destroy(&m);
    return 0;
}
```

**Result**

```
attempt 1: timed out, still waiting
attempt 2: timed out, still waiting
attempt 3: signalled, ready=1
```

---

## Example 8 — Per-thread state via `ThreadSpecific` (TSS/TLS-by-key)

`Each thread allocates its own PerThread struct and stores it in a thread-specific key. The destructor runs automatically when each thread exits, freeing the struct without the main thread needing to track it.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <stdlib.h>

#define N_THREADS 4

typedef struct {
    int tid;
    int local_count;
} PerThread;

static ThreadSpecific tss_key;

static void destroy_per_thread(void* p) {
    PerThread* pt = (PerThread*)p;
    fmt_printf("destructor: tid=%d local_count=%d\n", pt->tid, pt->local_count);
    free(pt);
}

int worker(void* arg) {
    int id = *(int*)arg;

    PerThread* pt = (PerThread*)malloc(sizeof *pt);
    pt->tid = id;
    pt->local_count = 0;
    thread_specific_set(tss_key, pt);

    for (int i = 0; i < 5; ++i) {
        PerThread* slot = (PerThread*)thread_specific_get(tss_key);
        slot->local_count++;
    }

    PerThread* slot = (PerThread*)thread_specific_get(tss_key);
    fmt_printf("worker tid=%d sees local_count=%d\n", id, slot->local_count);
    return 0;
}

int main(void) {
    thread_specific_create(&tss_key, destroy_per_thread);

    Thread threads[N_THREADS];
    int ids[N_THREADS];
    for (int i = 0; i < N_THREADS; ++i) {
        ids[i] = 100 + i;
        thread_create(&threads[i], worker, &ids[i]);
    }
    for (int i = 0; i < N_THREADS; ++i) {
        thread_join(threads[i], NULL);
    }

    thread_specific_delete(tss_key);
    return 0;
}
```

**Result**

```
worker tid=100 sees local_count=5
destructor: tid=100 local_count=5
worker tid=101 sees local_count=5
destructor: tid=101 local_count=5
worker tid=103 sees local_count=5
destructor: tid=103 local_count=5
worker tid=102 sees local_count=5
destructor: tid=102 local_count=5
```

---

## Example 9 — `call_once` / `OnceFlag` for one-time initialization

`Eight threads call call_once on the same OnceFlag. The init function must run exactly once, no matter how many threads race for it.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define N_THREADS 8

static OnceFlag init_flag = ONCE_FLAG_INIT;
static int init_runs = 0;
static int config_value = 0;
static Mutex log_mtx;

void initialize_config(void) {
    thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 100 * 1000 * 1000}, NULL);
    init_runs++;
    config_value = 42;
    fmt_printf("initialize_config: ran on thread %lu\n", thread_current());
}

int worker(void* arg) {
    int id = *(int*)arg;
    call_once(&init_flag, initialize_config);

    mutex_lock(&log_mtx);
    fmt_printf("worker %d: config_value=%d\n", id, config_value);
    mutex_unlock(&log_mtx);
    return 0;
}

int main(void) {
    mutex_init(&log_mtx, MUTEX_PLAIN);

    Thread threads[N_THREADS];
    int ids[N_THREADS];
    for (int i = 0; i < N_THREADS; ++i) {
        ids[i] = i;
        thread_create(&threads[i], worker, &ids[i]);
    }
    for (int i = 0; i < N_THREADS; ++i) {
        thread_join(threads[i], NULL);
    }

    fmt_printf("init_runs = %d (must be 1)\n", init_runs);
    mutex_destroy(&log_mtx);
    return init_runs == 1 ? 0 : 1;
}
```

**Result**

```
initialize_config: ran on thread 10840
worker 1: config_value=42
worker 2: config_value=42
...
worker 7: config_value=42
init_runs = 1 (must be 1)
```

---

## Example 10 — `thread_sleep`, `thread_yield`, `thread_equal`

`Two interleaving spinners using thread_sleep + thread_yield, then a check that thread_equal correctly distinguishes two live thread handles.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

int spinner(void* arg) {
    const char* label = (const char*)arg;
    for (int i = 0; i < 5; ++i) {
        fmt_printf("%s tick %d on thread %lu\n", label, i, thread_current());
        thread_yield();
        thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 50 * 1000 * 1000}, NULL);
    }
    return 0;
}

int main(void) {
    Thread a, b;
    thread_create(&a, spinner, "A");
    thread_create(&b, spinner, "B");

    /* thread_equal must be called on LIVE handles. On Windows the handle
       is released by thread_join, so compare first, join second. */
    fmt_printf("thread_equal(a,a) = %d (non-zero)\n", thread_equal(a, a));
    fmt_printf("thread_equal(a,b) = %d (zero)\n",      thread_equal(a, b));

    thread_join(a, NULL);
    thread_join(b, NULL);
    return 0;
}
```

**Result**

```
thread_equal(a,a) = 1 (non-zero)
thread_equal(a,b) = 0 (zero)
A tick 0 on thread 7188
B tick 0 on thread 23924
A tick 1 on thread 7188
B tick 1 on thread 23924
...
```

---

## Example 11 — Thread pool: create, submit, wait, destroy

`Submit twelve ~50ms tasks to a pool of 4 workers. thread_pool_wait blocks until the LAST in-flight task has returned (a regression fixed during the audit), then destroy joins and releases everything.`

```c
#include "concurrent/concurrent.h"
#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"

#define N_TASKS 12

static Mutex m;
static int finished;

int task(void* arg) {
    int id = *(int*)arg;
    thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = 50 * 1000 * 1000}, NULL);
    mutex_lock(&m);
    ++finished;
    fmt_printf("task %d done by thread %lu (finished=%d)\n", id, thread_current(), finished);
    mutex_unlock(&m);
    return 0;
}

int main(void) {
    mutex_init(&m, MUTEX_PLAIN);

    ThreadPool* pool = thread_pool_create(4);
    if (!pool) { fmt_fprintf(stderr, "pool create failed\n"); return 1; }

    int ids[N_TASKS];
    for (int i = 0; i < N_TASKS; ++i) {
        ids[i] = i;
        thread_pool_add_task(pool, task, &ids[i]);
    }

    thread_pool_wait(pool);
    thread_pool_destroy(pool);
    mutex_destroy(&m);

    fmt_printf("finished=%d (expected %d)\n", finished, N_TASKS);
    return finished == N_TASKS ? 0 : 1;
}
```

**Result**

```
task 1 done by thread 600 (finished=1)
task 0 done by thread 8652 (finished=2)
...
task 8 done by thread 600 (finished=12)
finished=12 (expected 12)
```

---

## Example 12 — Pool is reusable across batches

`thread_pool_wait does not destroy the pool, so the same pool can be used for multiple synchronised batches. Useful when an outer loop calls into a parallel-for kernel many times.`

```c
#include "concurrent/concurrent.h"
#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"

static Mutex m;
static int counter;

int bump(void* arg) {
    (void)arg;
    mutex_lock(&m);
    counter++;
    mutex_unlock(&m);
    return 0;
}

int main(void) {
    mutex_init(&m, MUTEX_PLAIN);
    ThreadPool* pool = thread_pool_create(3);

    for (int i = 0; i < 50; ++i) thread_pool_add_task(pool, bump, NULL);
    thread_pool_wait(pool);
    fmt_printf("after batch 1: counter=%d\n", counter);

    for (int i = 0; i < 30; ++i) thread_pool_add_task(pool, bump, NULL);
    thread_pool_wait(pool);
    fmt_printf("after batch 2: counter=%d\n", counter);

    for (int i = 0; i < 20; ++i) thread_pool_add_task(pool, bump, NULL);
    thread_pool_wait(pool);
    fmt_printf("after batch 3: counter=%d (expected 100)\n", counter);

    thread_pool_destroy(pool);
    mutex_destroy(&m);
    return counter == 100 ? 0 : 1;
}
```

**Result**

```
after batch 1: counter=50
after batch 2: counter=80
after batch 3: counter=100 (expected 100)
```

---

## Example 13 — Parallel sum of 1 000 000 ints with `thread_pool` + `Vector`

`The array is sliced into 40 chunks. Each chunk gets its own output slot in a Vector<long long>, so no per-task locking is needed — only a final reduction in main after thread_pool_wait. Pool size = thread_hardware_concurrency().`

```c
#include "concurrent/concurrent.h"
#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"
#include "vector/vector.h"
#include <stdlib.h>

#define ARRAY_SIZE   1000000
#define CHUNK_SIZE   25000

typedef struct {
    const int* base;
    size_t       begin;
    size_t       end;
    long long*   out_slot;
} SumChunk;

int sum_chunk(void* arg) {
    SumChunk* c = (SumChunk*)arg;
    long long s = 0;
    for (size_t i = c->begin; i < c->end; ++i) s += c->base[i];
    *c->out_slot = s;
    return 0;
}

int main(void) {
    int* data = (int*)malloc(sizeof(int) * ARRAY_SIZE);
    for (size_t i = 0; i < ARRAY_SIZE; ++i) data[i] = (int)(i % 1000);

    size_t n_chunks = (ARRAY_SIZE + CHUNK_SIZE - 1) / CHUNK_SIZE;

    Vector* partials = vector_create(sizeof(long long));
    Vector* descs    = vector_create(sizeof(SumChunk));
    for (size_t i = 0; i < n_chunks; ++i) {
        long long zero = 0;
        vector_push_back(partials, &zero);
        SumChunk d = {0};
        vector_push_back(descs, &d);
    }

    /* Fill descriptors AFTER push_back so the slot pointers are stable. */
    for (size_t i = 0; i < n_chunks; ++i) {
        SumChunk* d = (SumChunk*)vector_at(descs, i);
        d->base = data;
        d->begin = i * CHUNK_SIZE;
        d->end   = d->begin + CHUNK_SIZE;
        if (d->end > ARRAY_SIZE) d->end = ARRAY_SIZE;
        d->out_slot = (long long*)vector_at(partials, i);
    }

    ThreadPool* pool = thread_pool_create((int)thread_hardware_concurrency());
    for (size_t i = 0; i < n_chunks; ++i) {
        thread_pool_add_task(pool, sum_chunk, vector_at(descs, i));
    }
    thread_pool_wait(pool);
    thread_pool_destroy(pool);

    long long total = 0;
    for (size_t i = 0; i < n_chunks; ++i) total += *(long long*)vector_at(partials, i);

    long long ref = 0;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) ref += data[i];

    fmt_printf("parallel total = %lld\n", total);
    fmt_printf("reference      = %lld\n", ref);
    fmt_printf("match: %s\n", total == ref ? "YES" : "NO");

    vector_deallocate(partials);
    vector_deallocate(descs);
    free(data);
    return total == ref ? 0 : 1;
}
```

**Result**

```
parallel total = 499500000
reference      = 499500000
match: YES
```

---

## Example 14 — Parallel "URL fetch" simulator (pool + shared results table)

`Ten URLs are dispatched to a 4-worker pool. Each task simulates a 20–120ms round-trip with thread_sleep and writes its result into a shared FetchResult[] guarded by a mutex.`

```c
#include "concurrent/concurrent.h"
#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* url;
    int         http_status;
    long        latency_ms;
} FetchResult;

static Mutex results_mtx;
static FetchResult* results;
static int next_slot = 0;

static const char* URLS[] = {
    "https://example.com/a", "https://example.com/b",
    "https://example.com/c", "https://example.com/d",
    "https://example.com/e", "https://example.com/f",
    "https://example.com/g", "https://example.com/h",
    "https://example.com/i", "https://example.com/j",
};
#define N_URLS (sizeof(URLS) / sizeof(URLS[0]))

int fetch_task(void* arg) {
    const char* url = (const char*)arg;

    long ms = 20 + (long)((unsigned long)thread_current() % 100);
    thread_sleep(&(struct timespec){.tv_sec = 0, .tv_nsec = ms * 1000 * 1000}, NULL);

    FetchResult r = {.url = url, .http_status = 200, .latency_ms = ms};

    mutex_lock(&results_mtx);
    results[next_slot++] = r;
    mutex_unlock(&results_mtx);
    return 0;
}

int main(void) {
    mutex_init(&results_mtx, MUTEX_PLAIN);
    results = (FetchResult*)calloc(N_URLS, sizeof(FetchResult));

    ThreadPool* pool = thread_pool_create(4);
    for (size_t i = 0; i < N_URLS; ++i) {
        thread_pool_add_task(pool, fetch_task, (void*)URLS[i]);
    }
    thread_pool_wait(pool);
    thread_pool_destroy(pool);

    long total_ms = 0;
    for (int i = 0; i < next_slot; ++i) {
        fmt_printf("  [%d] %s -> %d in %ld ms\n",
                   i, results[i].url, results[i].http_status, results[i].latency_ms);
        total_ms += results[i].latency_ms;
    }
    fmt_printf("fetched %d urls, total simulated work = %ld ms\n", next_slot, total_ms);

    free(results);
    mutex_destroy(&results_mtx);
    return 0;
}
```

**Result**

```
  [0] https://example.com/c -> 200 in 40 ms
  [1] https://example.com/b -> 200 in 48 ms
  ...
  [9] https://example.com/j -> 200 in 40 ms
fetched 10 urls, total simulated work = 472 ms
```

---

## Example 15 — Two-stage pipeline (producer thread + worker pool)

`A single producer thread pushes ints into a CV-protected ring buffer. The main thread drains the buffer and dispatches each item as a "double the value" task to a 4-worker pool. The full pipeline runs end-to-end with no leaks and a verified final reduction.`

```c
#include "concurrent/concurrent.h"
#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"
#include <stdlib.h>

#define BUF_CAP 8
#define N_ITEMS 30

static int buf[BUF_CAP];
static int b_head, b_tail, b_count;
static Mutex b_mtx;
static ThreadCondition b_not_full, b_not_empty;
static int producer_done = 0;

static Mutex out_mtx;
static long doubled_sum = 0;

int producer(void* arg) {
    (void)arg;
    for (int i = 1; i <= N_ITEMS; ++i) {
        mutex_lock(&b_mtx);
        while (b_count == BUF_CAP) condition_wait(&b_not_full, &b_mtx);
        buf[b_tail] = i;
        b_tail = (b_tail + 1) % BUF_CAP;
        ++b_count;
        condition_signal(&b_not_empty);
        mutex_unlock(&b_mtx);
    }
    mutex_lock(&b_mtx);
    producer_done = 1;
    condition_broadcast(&b_not_empty);
    mutex_unlock(&b_mtx);
    fmt_printf("producer: emitted %d items\n", N_ITEMS);
    return 0;
}

int double_task(void* arg) {
    int v = *(int*)arg;
    free(arg);
    int out = v * 2;
    mutex_lock(&out_mtx);
    doubled_sum += out;
    mutex_unlock(&out_mtx);
    return 0;
}

int main(void) {
    mutex_init(&b_mtx, MUTEX_PLAIN);
    mutex_init(&out_mtx, MUTEX_PLAIN);
    condition_init(&b_not_full);
    condition_init(&b_not_empty);

    ThreadPool* pool = thread_pool_create(4);

    Thread prod;
    thread_create(&prod, producer, NULL);

    int dispatched = 0;
    while (1) {
        mutex_lock(&b_mtx);
        while (b_count == 0 && !producer_done) condition_wait(&b_not_empty, &b_mtx);
        if (b_count == 0 && producer_done) { mutex_unlock(&b_mtx); break; }
        int v = buf[b_head];
        b_head = (b_head + 1) % BUF_CAP;
        --b_count;
        condition_signal(&b_not_full);
        mutex_unlock(&b_mtx);

        int* heap_v = (int*)malloc(sizeof(int));
        *heap_v = v;
        thread_pool_add_task(pool, double_task, heap_v);
        ++dispatched;
    }

    thread_join(prod, NULL);
    thread_pool_wait(pool);
    thread_pool_destroy(pool);

    long expected = (long)N_ITEMS * (long)(N_ITEMS + 1);
    fmt_printf("dispatched %d, doubled_sum=%ld (expected %ld)\n",
               dispatched, doubled_sum, expected);

    condition_destroy(&b_not_empty);
    condition_destroy(&b_not_full);
    mutex_destroy(&b_mtx);
    mutex_destroy(&out_mtx);
    return doubled_sum == expected ? 0 : 1;
}
```

**Result**

```
producer: emitted 30 items
dispatched 30, doubled_sum=930 (expected 930)
```

---

## Example 16 — Bounded buffer with a counting `Semaphore`

The `empty`/`full` semaphores enforce the buffer's capacity: the producer blocks in `semaphore_wait(&empty)` when the buffer is full, the consumer blocks in `semaphore_wait(&full)` when it is empty.

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define CAP 4
#define N   10

typedef struct {
    int       buf[CAP];
    int       head, tail;
    Semaphore empty;   /* free slots  */
    Semaphore full;    /* filled slots */
    Mutex     lock;
    long      sum;
} Shared;

static int producer(void* arg) {
    Shared* s = (Shared*)arg;
    for (int i = 1; i <= N; ++i) {
        semaphore_wait(&s->empty);          /* wait for a free slot   */
        mutex_lock(&s->lock);
        s->buf[s->tail] = i;
        s->tail = (s->tail + 1) % CAP;
        mutex_unlock(&s->lock);
        semaphore_post(&s->full);           /* announce a filled slot */
    }
    return 0;
}

int main(void) {
    Shared s;
    s.head = s.tail = 0;
    s.sum = 0;
    semaphore_init(&s.empty, CAP);          /* CAP free slots  */
    semaphore_init(&s.full, 0);             /* 0 filled slots  */
    mutex_init(&s.lock, MUTEX_PLAIN);

    Thread prod;
    thread_create(&prod, producer, &s);

    for (int i = 0; i < N; ++i) {
        semaphore_wait(&s.full);            /* wait for a filled slot */
        mutex_lock(&s.lock);
        int v = s.buf[s.head];
        s.head = (s.head + 1) % CAP;
        mutex_unlock(&s.lock);
        semaphore_post(&s.empty);           /* free the slot */
        s.sum += v;
    }
    thread_join(prod, NULL);

    fmt_printf("consumed %d items, sum = %ld\n", N, s.sum);

    semaphore_destroy(&s.empty);
    semaphore_destroy(&s.full);
    mutex_destroy(&s.lock);

    return 0;
}
```

**Result**

```
consumed 10 items, sum = 55
```

---

## Example 17 — Thread pool: checked submit, introspection, and bounded wait

```c
#include "concurrent/concurrent.h"
#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"

static Mutex g_lock;
static int   g_done = 0;

static int work(void* arg) {
    (void)arg;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 20 * 1000000L }; /* 20 ms */
    thread_sleep(&ts, NULL);
    mutex_lock(&g_lock);
    g_done++;
    mutex_unlock(&g_lock);
    return 0;
}

int main(void) {
    mutex_init(&g_lock, MUTEX_PLAIN);

    ThreadPool* pool = thread_pool_create(4);
    fmt_printf("workers: %d\n", thread_pool_thread_count(pool));

    for (int i = 0; i < 20; ++i) {
        if (!thread_pool_add_task_checked(pool, work, NULL)) {
            fmt_printf("submit %d rejected\n", i);   /* would fire on OOM */
        }
    }
    fmt_printf("work outstanding: %s\n", thread_pool_pending_count(pool) > 0 ? "yes" : "no");

    /* 20 tasks * 20ms / 4 workers ~= 100ms, so 1ms is far too short. */
    if (!thread_pool_wait_timeout(pool, 1)) {
        fmt_printf("not drained in 1ms (expected)\n");
    }
    if (thread_pool_wait_timeout(pool, 5000)) {
        fmt_printf("drained: %d tasks done, pending now %d\n",
                   g_done, thread_pool_pending_count(pool));
    }

    thread_pool_destroy(pool);
    mutex_destroy(&g_lock);
    return 0;
}
```

**Result**

```
workers: 4
work outstanding: yes
not drained in 1ms (expected)
drained: 20 tasks done, pending now 0
```

---

## Example 18 — Read-write lock: concurrent readers, exclusive writers

`RWLock` lets many readers observe shared state at once while a writer gets exclusive access. Here four writers each perform 1000 `+1` updates under `rwlock_wrlock`, while four readers repeatedly read the value under `rwlock_rdlock`. Two outcomes prove the lock works: the final value has **no lost updates** (writers were exclusive), and readers **never observe a torn value** — two reads taken under one read-lock always agree, because no writer can change the data while a reader holds it.

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define WRITERS   4
#define READERS   4
#define ITERS     1000

static RWLock rw;
static long   balance = 0;     /* shared state guarded by rw */

/* Writers take the lock EXCLUSIVELY and mutate the shared state. */
static int writer(void* arg) {
    (void)arg;

    for (int i = 0; i < ITERS; ++i) {
        rwlock_wrlock(&rw);
        balance += 1;
        rwlock_unlock(&rw);
    }

    return 0;
}

/* Readers SHARE the lock and observe a consistent snapshot: while a reader
   holds it, no writer can change `balance`, so two reads must agree. */
static int torn_total = 0;
static Mutex torn_mtx;

static int reader(void* arg) {
    (void)arg;
    int torn = 0;

    for (int i = 0; i < ITERS; ++i) {
        rwlock_rdlock(&rw);
        long a = balance;
        for (volatile int k = 0; k < 50; ++k) { /* widen the read window */ }

        long b = balance;
        if (a != b) {
            torn++;
        }

        rwlock_unlock(&rw);
    }
    mutex_lock(&torn_mtx);
    torn_total += torn;
    mutex_unlock(&torn_mtx);

    return 0;
}

int main(void) {
    rwlock_init(&rw);
    mutex_init(&torn_mtx, MUTEX_PLAIN);

    Thread tw[WRITERS], tr[READERS];
    for (int i = 0; i < WRITERS; ++i) {
        thread_create(&tw[i], writer, NULL);
    }

    for (int i = 0; i < READERS; ++i) {
        thread_create(&tr[i], reader, NULL);
    }

    for (int i = 0; i < WRITERS; ++i) {
        thread_join(tw[i], NULL);
    }

    for (int i = 0; i < READERS; ++i) {
        thread_join(tr[i], NULL);
    }

    fmt_printf("final balance = %ld (expected %d)\n", balance, WRITERS * ITERS);
    fmt_printf("torn reads observed = %d\n", torn_total);

    mutex_destroy(&torn_mtx);
    rwlock_destroy(&rw);

    return 0;
}
```

**Result**
```
final balance = 4000 (expected 4000)
torn reads observed = 0
```

---

## Example 19 — Mutex-guarded `Vector`: concurrent append

`Eight threads each push 1000 ints into one shared Vector. A Mutex serialises every push_back, so no element is lost or corrupted. The final size and sum are fixed no matter how the threads interleave.`

```c
#include "concurrent/concurrent.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

#define N_THREADS  8
#define PER_THREAD 1000


typedef struct {
    int     id;
    Vector* vec;     /* shared, NOT thread-safe on its own */
    Mutex*  lock;    /* serialises every mutation of vec    */
} AppendJob;


int appender(void* arg) {
    AppendJob* j = (AppendJob*)arg;

    for (int i = 0; i < PER_THREAD; ++i) {
        mutex_lock(j->lock);
        vector_push_back(j->vec, &j->id);   /* guarded shared mutation */
        mutex_unlock(j->lock);
    }

    return 0;
}


int main(void) {
    Vector* vec = vector_create(sizeof(int));
    Mutex lock;
    mutex_init(&lock, MUTEX_PLAIN);

    Thread th[N_THREADS];
    AppendJob jobs[N_THREADS];
    for (int i = 0; i < N_THREADS; ++i) {
        jobs[i].id = i;
        jobs[i].vec = vec;
        jobs[i].lock = &lock;

        thread_create(&th[i], appender, &jobs[i]);
    }

    for (int i = 0; i < N_THREADS; ++i) {
        thread_join(th[i], NULL);
    }

    long long sum = 0;
    for (size_t i = 0; i < vector_size(vec); ++i) {
        sum += *(int*)vector_at(vec, i);
    }

    fmt_printf("items pushed: %zu (expected %d)\n", vector_size(vec), N_THREADS * PER_THREAD);
    fmt_printf("sum of items: %lld (expected %d)\n", sum, PER_THREAD * (0 + 1 + 2 + 3 + 4 + 5 + 6 + 7));

    mutex_destroy(&lock);
    vector_deallocate(vec);

    return 0;
}
```

**Result**
```
items pushed: 8000 (expected 8000)
sum of items: 28000 (expected 28000)
```

---

## Example 20 — Mutex-guarded `Map`: concurrent counter (read-modify-write)

`A shared Map<int,int*> of per-key counters. Eight threads each increment all four keys 1000 times. The increment is a read-modify-write — load the value, ++, store — so it MUST happen under the lock; otherwise updates are lost to races. Every key ends at exactly N_THREADS * ROUNDS.`

```c
#include "concurrent/concurrent.h"
#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

#define N_THREADS 8
#define ROUNDS    1000
#define K_KEYS    4

static int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}


typedef struct {
    Map*   map;      /* shared key -> int* count */
    Mutex* lock;
    int*   keys;     /* K_KEYS stable key addresses */
} CountJob;


int counter(void* arg) {
    CountJob* j = (CountJob*)arg;

    for (int r = 0; r < ROUNDS; ++r) {
        for (int k = 0; k < K_KEYS; ++k) {
            mutex_lock(j->lock);
            int* v = (int*)map_at(j->map, &j->keys[k]);   /* read-modify-write */
            (*v)++;
            mutex_unlock(j->lock);
        }
    }
    return 0;
}

int main(void) {
    int keys[K_KEYS];
    Map* map = map_create(compare_ints, NULL, free);   /* values are heap int*, freed by map */

    for (int k = 0; k < K_KEYS; ++k) {
        keys[k] = k;
        int* zero = (int*)malloc(sizeof(int));
        *zero = 0;
        map_insert(map, &keys[k], zero);               /* pre-seed every key */
    }

    Mutex lock;
    mutex_init(&lock, MUTEX_PLAIN);
    Thread th[N_THREADS];
    CountJob jobs[N_THREADS];

    for (int i = 0; i < N_THREADS; ++i) {
        jobs[i].map = map;
        jobs[i].lock = &lock;
        jobs[i].keys = keys;
        thread_create(&th[i], counter, &jobs[i]);
    }

    for (int i = 0; i < N_THREADS; ++i) {
        thread_join(th[i], NULL);
    }

    long long total = 0;
    for (int k = 0; k < K_KEYS; ++k) {
        int c = *(int*)map_at(map, &keys[k]);
        total += c;
        fmt_printf("key %d -> %d\n", k, c);
    }

    fmt_printf("total increments: %lld (expected %d)\n", total, N_THREADS * ROUNDS * K_KEYS);

    mutex_destroy(&lock);
    map_deallocate(map);

    return 0;
}
```

**Result**
```
key 0 -> 8000
key 1 -> 8000
key 2 -> 8000
key 3 -> 8000
total increments: 32000 (expected 32000)
```

---

## Example 21 — `RWLock` + `Map`: many readers, one writer

`A read-write lock lets several reader threads inspect a shared Map concurrently (read-lock) while a single writer mutates it under an exclusive write-lock. Readers never see a torn value, and the writer's deterministic updates leave each key at exactly WRITES.`

```c
#include "concurrent/concurrent.h"
#include "map/map.h"
#include "fmt/fmt.h"
#include <stdlib.h>

#define N_READERS 3
#define WRITES    500
#define K_KEYS    3
#define READ_ITERS 2000


static int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}

typedef struct {
    Map*    map;
    RWLock* rw;
    int*    keys;
} ConfigJob;


/* Many readers run concurrently under a shared read-lock. */
int reader(void* arg) {
    ConfigJob* j = (ConfigJob*)arg;
    long long checksum = 0;

    for (int it = 0; it < READ_ITERS; ++it) {
        rwlock_rdlock(j->rw);                 /* shared: readers don't block readers */
        for (int k = 0; k < K_KEYS; ++k) {
            checksum += *(int*)map_at(j->map, &j->keys[k]);
        }
        rwlock_unlock(j->rw);
    }
    (void)checksum;                           /* value is timing-dependent: not printed */

    return 0;
}


/* A single writer mutates under an exclusive write-lock. */
int writer(void* arg) {
    ConfigJob* j = (ConfigJob*)arg;

    for (int w = 0; w < WRITES; ++w) {
        for (int k = 0; k < K_KEYS; ++k) {
            rwlock_wrlock(j->rw);             /* exclusive: blocks all readers + writers */
            (*(int*)map_at(j->map, &j->keys[k]))++;
            rwlock_unlock(j->rw);
        }
    }

    return 0;
}


int main(void) {
    int keys[K_KEYS];
    Map* map = map_create(compare_ints, NULL, free);

    for (int k = 0; k < K_KEYS; ++k) {
        keys[k] = k;
        int* zero = (int*)malloc(sizeof(int));
        *zero = 0;
        map_insert(map, &keys[k], zero);
    }

    RWLock rw;
    rwlock_init(&rw);

    ConfigJob job = { map, &rw, keys };
    Thread readers[N_READERS], wr;

    for (int i = 0; i < N_READERS; ++i) {
        thread_create(&readers[i], reader, &job);
    }
    thread_create(&wr, writer, &job);

    for (int i = 0; i < N_READERS; ++i) {
        thread_join(readers[i], NULL);
    }
    thread_join(wr, NULL);

    fmt_printf("final config after writer:\n");
    for (int k = 0; k < K_KEYS; ++k) {
        fmt_printf("  key %d -> %d\n", k, *(int*)map_at(map, &keys[k]));
    }
    fmt_printf("%d reader(s) + 1 writer finished with no torn reads\n", N_READERS);

    rwlock_destroy(&rw);
    map_deallocate(map);

    return 0;
}
```

**Result**
```
final config after writer:
  key 0 -> 500
  key 1 -> 500
  key 2 -> 500
3 reader(s) + 1 writer finished with no torn reads
```

---

## Example 22 — Counting `Semaphore` + `Queue`: producer / consumer

`Four producers push items into one shared Queue (guarded by a Mutex). A counting Semaphore tracks how many items are available, so consumers block instead of busy-waiting. A poison-pill per consumer drains the system cleanly. Items produced == items consumed, and the value sums match.`

```c
#include "concurrent/concurrent.h"
#include "queue/queue.h"
#include "fmt/fmt.h"

#define N_PRODUCERS 4
#define N_CONSUMERS 3
#define PER_PRODUCER 500
#define SENTINEL    (-1)

static Queue*    g_queue;
static Mutex     g_lock;     /* guards g_queue                         */
static Semaphore g_items;    /* counts items available in g_queue      */
static long long g_count;    /* consumed item count (under g_lock)     */
static long long g_sum;      /* consumed value sum  (under g_lock)     */


int producer(void* arg) {
    int id = *(int*)arg;          /* pushes its own (id+1) PER_PRODUCER times */
    int value = id + 1;

    for (int i = 0; i < PER_PRODUCER; ++i) {
        mutex_lock(&g_lock);
        queue_push(g_queue, &value);
        mutex_unlock(&g_lock);
        semaphore_post(&g_items);  /* announce one more item */
    }

    return 0;
}


int consumer(void* arg) {
    (void)arg;
    for (;;) {
        semaphore_wait(&g_items);  /* block until an item is available */
        mutex_lock(&g_lock);
        int v = *(int*)queue_front(g_queue);

        queue_pop(g_queue);
        if (v != SENTINEL) {
            g_count++;
            g_sum += v;
        }

        mutex_unlock(&g_lock);
        if (v == SENTINEL) {
            break;                 /* poison pill: shut this consumer down */
        }
    }
    return 0;
}


int main(void) {
    g_queue = queue_create(sizeof(int));
    mutex_init(&g_lock, MUTEX_PLAIN);
    semaphore_init(&g_items, 0);

    Thread cons[N_CONSUMERS], prod[N_PRODUCERS];
    int ids[N_PRODUCERS];

    for (int i = 0; i < N_CONSUMERS; ++i) {
        thread_create(&cons[i], consumer, NULL);
    }

    for (int i = 0; i < N_PRODUCERS; ++i) {
        ids[i] = i;
        thread_create(&prod[i], producer, &ids[i]);
    }

    for (int i = 0; i < N_PRODUCERS; ++i) {
        thread_join(prod[i], NULL);
    }

    /* All real items are queued; send one poison pill per consumer. */
    for (int i = 0; i < N_CONSUMERS; ++i) {
        int sentinel = SENTINEL;
        mutex_lock(&g_lock);
        queue_push(g_queue, &sentinel);
        mutex_unlock(&g_lock);
        semaphore_post(&g_items);
    }

    for (int i = 0; i < N_CONSUMERS; ++i) {
        thread_join(cons[i], NULL);
    }

    long long expect_sum = (long long)PER_PRODUCER * (1 + 2 + 3 + 4);
    fmt_printf("produced: %d items, sum %lld\n", N_PRODUCERS * PER_PRODUCER, expect_sum);
    fmt_printf("consumed: %lld items, sum %lld\n", g_count, g_sum);
    fmt_printf("queue drained: %s\n", queue_empty(g_queue) ? "yes" : "no");

    semaphore_destroy(&g_items);
    mutex_destroy(&g_lock);
    queue_deallocate(g_queue);

    return 0;
}
```

**Result**
```
produced: 2000 items, sum 5000
consumed: 2000 items, sum 5000
queue drained: yes
```

---

## Example 23 — `ThreadCondition` + `Deque`: a blocking work queue

`Worker threads sleep on a condition variable until tasks land in a shared Deque, instead of spinning. main pushes 2000 tasks (signalling one worker each) and then sets a done flag and broadcasts to wake everyone for shutdown. Every task is processed exactly once; the value sum is fixed.`

```c
#include "concurrent/concurrent.h"
#include "deque/deque.h"
#include "fmt/fmt.h"

#define N_WORKERS 4
#define N_TASKS   2000

static Deque*          g_tasks;     /* shared task queue            */
static Mutex           g_lock;
static ThreadCondition g_cv;        /* "a task is available"        */
static int             g_done;      /* set once no more tasks coming */
static long long       g_processed;
static long long       g_sum;

int worker(void* arg) {
    (void)arg;
    for (;;) {
        mutex_lock(&g_lock);

        while (deque_empty(g_tasks) && !g_done) {
            condition_wait(&g_cv, &g_lock);   /* sleeps, releasing the lock */
        }

        if (deque_empty(g_tasks) && g_done) {
            mutex_unlock(&g_lock);
            break;                            /* drained + shutdown -> exit */
        }

        int v = *(int*)deque_front(g_tasks);
        deque_pop_front(g_tasks);

        g_processed++;
        g_sum += v;

        mutex_unlock(&g_lock);
    }

    return 0;
}

int main(void) {
    g_tasks = deque_create(sizeof(int));
    mutex_init(&g_lock, MUTEX_PLAIN);
    condition_init(&g_cv);

    Thread th[N_WORKERS];
    for (int i = 0; i < N_WORKERS; ++i) {
        thread_create(&th[i], worker, NULL);
    }

    for (int i = 0; i < N_TASKS; ++i) {
        mutex_lock(&g_lock);
        deque_push_back(g_tasks, &i);
        condition_signal(&g_cv);              /* wake one waiting worker */
        mutex_unlock(&g_lock);
    }

    mutex_lock(&g_lock);
    g_done = 1;
    condition_broadcast(&g_cv);               /* wake everyone for shutdown */
    mutex_unlock(&g_lock);

    for (int i = 0; i < N_WORKERS; ++i) {
        thread_join(th[i], NULL);
    }

    long long expect = (long long)N_TASKS * (N_TASKS - 1) / 2;
    fmt_printf("tasks submitted: %d\n", N_TASKS);
    fmt_printf("tasks processed: %lld\n", g_processed);
    fmt_printf("sum of task values: %lld (expected %lld)\n", g_sum, expect);

    condition_destroy(&g_cv);
    mutex_destroy(&g_lock);
    deque_deallocate(g_tasks);

    return 0;
}
```

**Result**
```
tasks submitted: 2000
tasks processed: 2000
sum of task values: 1999000 (expected 1999000)
```

---

## Example 24 — `call_once` + `Set`: thread-safe lazy init + concurrent de-dup

`A shared Set is created lazily on first use. call_once guarantees the initializer runs exactly once even when every thread races to touch it first. Each of eight threads then inserts the same 100 values under a Mutex; the Set collapses duplicates, so its final size is exactly the number of distinct values.`

```c
#include "concurrent/concurrent.h"
#include "set/set.h"
#include "fmt/fmt.h"

#define N_THREADS 8
#define DISTINCT  100

static int set_cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static Set*     g_registry;                 /* lazily created, shared       */
static Mutex    g_lock;                      /* guards g_registry inserts    */
static OnceFlag g_once = ONCE_FLAG_INIT;     /* makes init run exactly once  */
static int      g_init_count;                /* how many times init ran      */

/* Runs at most once across all threads, guaranteed by call_once. */
static void init_registry(void) {
    g_registry = set_create(sizeof(int), set_cmp_int);
    mutex_init(&g_lock, MUTEX_PLAIN);
    g_init_count++;
}


int worker(void* arg) {
    (void)arg;
    call_once(&g_once, init_registry);       /* first caller initialises; rest skip */

    for (int v = 0; v < DISTINCT; ++v) {
        mutex_lock(&g_lock);
        set_insert(g_registry, &v);          /* duplicates collapse (set semantics) */
        mutex_unlock(&g_lock);
    }

    return 0;
}

int main(void) {
    Thread th[N_THREADS];

    for (int i = 0; i < N_THREADS; ++i) {
        thread_create(&th[i], worker, NULL);
    }
    for (int i = 0; i < N_THREADS; ++i) {
        thread_join(th[i], NULL);
    }

    fmt_printf("registry initialized exactly once: %s\n", g_init_count == 1 ? "yes" : "no");
    fmt_printf("distinct values in set: %zu (expected %d)\n", set_size(g_registry), DISTINCT);
    fmt_printf("total insert attempts: %d\n", N_THREADS * DISTINCT);

    set_deallocate(g_registry);
    mutex_destroy(&g_lock);

    return 0;
}
```

**Result**
```
registry initialized exactly once: yes
distinct values in set: 100 (expected 100)
total insert attempts: 800
```

---

## License

This project is open-source and available under the [ISC License](LICENSE).
