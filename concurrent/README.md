
# Concurrent Lib in C

## Example 1 : Producer-Consumer Synchronization 

`This example demonstrate a classic synchronization problem known as the producer-consumer problem using threads, mutexes, and condition variables. The problem illustrates how two processes (or threads, in this example) share a common, fixed-size buffer. One thread (the producer) creates items and puts them into the buffer, while the other thread (the consumer) takes items out of the buffer. The key challenge is to ensure that the producer does not try to add items to the buffer if it is full, and the consumer does not try to remove items from the buffer if it is empty, to prevent race conditions and ensure data integrity.`

```c 
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define BUFFER_EMPTY (-1)

int sharedBuffer = BUFFER_EMPTY;
Mutex bufferMutex;
ThreadCondition bufferNotEmpty;
ThreadCondition bufferNotFull;

int producer(void* arg) {
    (void)arg; 

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

## Example 3: Fixed-Size Thread Pool

`This example creates a thread pool that manages a fixed number of worker threads. Tasks can be submitted to the pool and are executed by the next available worker thread. This pattern is useful for efficiently processing tasks asynchronously and concurrently, without the overhead of creating and destroying threads for each task.`

```c
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
```

## Example 4: Barrier Synchronization 

`This example demonstrates how to implement a barrier synchronization mechanism. The barrier will ensure that all participating threads reach a certain point in their execution before any of them can proceed. We'll simulate a scenario where threads perform an initial phase of work, wait at the barrier for all to finish, and then proceed to a final phase of work.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <stdlib.h>

#define NUM_THREADS 5

typedef struct barrier {
    Mutex mutex;
    ThreadCondition condition;
    int count;
    int waiting;
    int phase; 
} Barrier;

void barrier_init(Barrier* barrier, int count) {
    mutex_init(&barrier->mutex, MUTEX_PLAIN);
    condition_init(&barrier->condition);
    barrier->count = count;
    barrier->waiting = 0;
    barrier->phase = 0; 
}

void barrier_wait(Barrier* barrier) {
    mutex_lock(&barrier->mutex);
    int my_phase = barrier->phase; // Capture the current phase
    
    if (++barrier->waiting == barrier->count) {
        barrier->waiting = 0; // Reset for the next use
        barrier->phase++; 
        condition_broadcast(&barrier->condition); // Wake up all threads
    } 
    else {
        while (my_phase == barrier->phase) {
            condition_wait(&barrier->condition, &barrier->mutex);
        }
    }
    mutex_unlock(&barrier->mutex);
}

void barrier_destroy(Barrier* barrier) {
    mutex_destroy(&barrier->mutex);
    condition_destroy(&barrier->condition);
}

int worker(void* arg) {
    Barrier* barrier = (Barrier*)arg;
    
    // Phase 1: Initial work before the barrier
    fmt_printf("Thread %lu doing initial work\n", thread_current());
    thread_sleep(&(struct timespec){.tv_sec = 1, .tv_nsec = 0}, NULL); 

    // Wait at the barrier for all threads
    fmt_printf("Thread %lu waiting at the barrier\n", thread_current());
    barrier_wait(barrier);

    // Phase 2: Final work after the barrier
    fmt_printf("Thread %lu doing final work\n", thread_current());
    return 0;
}

int main(void) {
    Barrier barrier;
    barrier_init(&barrier, NUM_THREADS);

    Thread threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_create(&threads[i], worker, &barrier);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_join(threads[i], NULL);
    }

    barrier_destroy(&barrier);
    return 0;
}
```

## Example 5 : Thread-Specific Logging with One-Time Initialization

`This is a example that demonstrates the use of thread-specific storage (TSS), also known as thread-local storage (TLS), along with a one-time initialization pattern using call_once. This example will simulate a logging mechanism where each thread can have its own log buffer, and the initialization of the logging infrastructure is done only once in a thread-safe manner.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

#define LOG_BUFFER_SIZE 256
#define NUM_THREADS 4

// Thread-specific key for accessing the log buffer
ThreadSpecific logBufferKey;

// OnceFlag for one-time initialization
OnceFlag logInitOnce = ONCE_FLAG_INIT;

// Function to initialize logging resources
void initLogging(void) {
    // Create the thread-specific storage key for log buffers
    thread_specific_create(&logBufferKey, free);
}

// Function to get or create a log buffer for the current thread
char* getLogBuffer(void) {
    char* buffer = (char*)thread_specific_get(logBufferKey);

    if (buffer == NULL) {
        buffer = (char*)malloc(LOG_BUFFER_SIZE);
        if (buffer) {
            memset(buffer, 0, LOG_BUFFER_SIZE);
            thread_specific_set(logBufferKey, buffer);
        }
    }
    return buffer;
}

void logMessage(const char* message) {
    // Ensure the logging system is initialized exactly once
    call_once(&logInitOnce, initLogging);

    // Get the current thread's log buffer
    char* buffer = getLogBuffer();
    if (buffer) {
        strcat(buffer, message);
        strcat(buffer, "\n");
        
        fmt_printf("Thread %lu: %s", thread_current(), buffer);
    }
}

int worker(void* arg) {
    (void)arg; 

    logMessage("Starting work");
    // Simulate some work
    thread_sleep(&(struct timespec){.tv_sec = 1, .tv_nsec = 0}, NULL);
    logMessage("Work completed");

    return 0;
}

int main(void) {
    Thread threads[NUM_THREADS];

    // Create and start threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_create(&threads[i], worker, NULL);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_join(threads[i], NULL);
    }

    thread_specific_delete(logBufferKey);
    return 0;
}
```

## Example 6 : Thread Args as Python 

`This example demonstrates a simple multithreaded application using a custom concurrent library. It's structured to create two threads that execute concurrently, each printing the current time at specified intervals, synchronized using a mutex to ensure that the time printing does not occur simultaneously from both threads.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include "time/time.h"

// Define a structure for thread arguments
typedef struct {
    int threadID;
    const char* name;
    int counter;
    int delay;

} ThreadArgs;

// Mutex for synchronizing thread access
Mutex threadLock;

// Thread function prototype
int run(void* arguments);

// Function for printing the current time
void print_time(const char* threadName, int delay, int counter) {
    for (int i = 0; i < counter; ++i) {
        time_sleep(delay);
        Time* time = time_current_time();

        fmt_printf("%s: %s\n", threadName, time_to_string(time));
        time_deallocate(time);
    }
}

// Thread function
int run(void* arguments) {
    ThreadArgs* args = (ThreadArgs*)arguments;
    fmt_printf("Starting %s\n", args->name);

    // Acquire the lock
    mutex_lock(&threadLock);

    print_time(args->name, args->delay, args->counter);

    // Release the lock
    mutex_unlock(&threadLock);

    return 0;
}

int main(void) {
    Thread thread1, thread2;

    // Initialize the mutex
    mutex_init(&threadLock, MUTEX_PLAIN);

    // Define thread arguments
    ThreadArgs args1 = {1, "Thread-1", 3, 1};
    ThreadArgs args2 = {2, "Thread-2", 3, 2};

    // Create new threads
    thread_create(&thread1, run, &args1);
    thread_create(&thread2, run, &args2);

    // Wait for all threads to complete
    thread_join(thread1, NULL);
    thread_join(thread2, NULL);

    // Destroy the mutex
    mutex_destroy(&threadLock);
    fmt_printf("Exiting Main Thread\n");

    return 0;
}
```

*Same C Code in Python*

```py 
import threading
import time

class myThread (threading.Thread):
   def __init__(self, threadID, name, counter):
      threading.Thread.__init__(self)
      self.threadID = threadID
      self.name = name
      self.counter = counter
   def run(self):
      print ("Starting " + self.name)
      # Get lock to synchronize threads
      threadLock.acquire()
      print_time(self.name, self.counter, 3)
      # Free lock to release next thread
      threadLock.release()
      
def print_time(threadName, delay, counter):
   while counter:
      time.sleep(delay)
      print ("%s: %s" % (threadName, time.ctime(time.time())))
      counter -= 1
      
threadLock = threading.Lock()
threads = []

# Create new threads
thread1 = myThread(1, "Thread-1", 1)
thread2 = myThread(2, "Thread-2", 2)

# Start new Threads
thread1.start()
thread2.start()

# Add threads to thread list
threads.append(thread1)
threads.append(thread2)

# Wait for all threads to complete
for t in threads:
   t.join()
print ("Exiting Main Thread")
```

## Example 7 : Using Vector 

`This example demonstrates that how we can use vector container for saving each Thread`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include "vector/vector.h"
#include "time/time.h"

typedef struct {
    int threadID;
    const char* name;
    int delay;
} ThreadArgs;

int doTask(void* arguments) {
    ThreadArgs* args = (ThreadArgs*)arguments;

    fmt_printf("Starting %s\n", args->name);
    time_sleep(args->delay);
    fmt_printf("%s completed\n", args->name);

    return 0;
}

int main() {
    Vector* threads = vector_create(sizeof(Thread));

    for (int i = 0; i < 10; ++i) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->threadID = i;
        args->name = "Thread";
        args->delay = 1;

        Thread newThread;
        if (thread_create(&newThread, doTask, args) == THREAD_SUCCESS) {
            vector_push_back(threads, &newThread);
        } 
        else {
            fmt_printf("Failed to create thread\n");
        }
    }

    // Join all threads
    for (size_t i = 0; i < vector_size(threads); ++i) {
        Thread* t = (Thread*)vector_at(threads, i);
        if (t != NULL) {
            thread_join(*t, NULL);
        }
    }
    vector_deallocate(threads);
    fmt_printf("Exiting Main Thread\n");

    return 0;
}
```

## Example 8 : Parallel accumulate 

`This example demonstrates a parallel implementation of the accumulate algorithm, typically used to sum or otherwise combine all elements in a collection. This particular implementation is designed to work with an array of integers. The program is structured to leverage multi-threading for improved performance, especially beneficial for large datasets and on systems with multiple cores or processors.`

`The program efficiently utilizes multiple threads to parallelize the work, potentially reducing the execution time on multi-core systems`

`The number of threads is determined at runtime based on the hardware's concurrency level, ensuring the program can adapt to different environments`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include "algorithm/algorithm.h"
#include "vector/vector.h"

#define MIN_PER_THREAD 50
#define SIZE 100

// Function to perform accumulation in a segment
void accumulate_range(int* start, int* end, int* result) {
    *result = 0;
    for (int* ptr = start; ptr != end; ++ptr) {
        *result += *ptr;
    }
}

// Thread start function
int accumulate_segment(void* args) {
    Pair* range = (Pair*)args; // Using Pair to pass start and end pointers
    int* start = (int*)range->first;
    int* end = (int*)range->second;
    int result;
    accumulate_range(start, end, &result);
    return result;
}

// Parallel accumulate function
int parallel_accumulate(int* array, size_t num_elements, int init) {
    if (num_elements == 0) return init;

    unsigned long num_threads = thread_hardware_concurrency();
    num_threads = num_threads ? num_threads : 2; // Use 2 if hardware_concurrency() returns 0
    unsigned long block_size = num_elements / num_threads;

    if (block_size < MIN_PER_THREAD) {
        block_size = MIN_PER_THREAD;
        num_threads = (num_elements + MIN_PER_THREAD - 1) / MIN_PER_THREAD;
    }

    Vector* threads = vector_create(sizeof(Thread));
    Vector* results = vector_create(sizeof(int));

    for (size_t i = 0; i < (num_threads - 1); ++i) {
        Pair range = {(void*)(array + i * block_size), (void*)(array + (i + 1) * block_size)};
        Thread new_thread;
        thread_create(&new_thread, accumulate_segment, &range);
        vector_push_back(threads, &new_thread);
    }
    // Handle the last segment
    Pair last_range = {(void*)(array + (num_threads - 1) * block_size), (void*)(array + num_elements)};
    int last_result = accumulate_segment(&last_range);
    vector_push_back(results, &last_result);

    // Join threads and accumulate results
    int final_result = init;
    for (size_t i = 0; i < vector_size(threads); ++i) {
        Thread* thread = (Thread*)vector_at(threads, i);
        int res = 0;
        thread_join(*thread, &res);
        final_result += res;
    }

    final_result += last_result;

    vector_deallocate(threads);
    vector_deallocate(results);

    return final_result;
}

void int_generator(void* output) {
    static int counter = 1;
    *(int *)output = counter++;
}

int main() {
    int array[SIZE];
    algorithm_generate(array, array + SIZE, sizeof(int), int_generator);

    int sum = parallel_accumulate(array, SIZE, 0);
    fmt_printf("Sum: %d\n", sum);

    return 0;
}
```

## Example 9 : Parallel factorial calculation 

`This example divides the task of calculating a factorial into smaller tasks, each handled by a separate thread. The results are then combined to get the final output. It showcases thread creation, mutex locking for shared data protection, condition variables for synchronization, and the use of a dynamic array to store thread handles`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define NUM_FACT_THREADS 4

typedef struct {
    int start;
    int end;
    unsigned long long result;
} FactorialSegment;

Mutex resultMutex;
ThreadCondition calculationDone;
int calculationsCompleted = 0;

unsigned long long sequentialFactorial(int start, int end) {
    unsigned long long result = 1;
    for (int i = start; i <= end; ++i) {
        result *= i;
    }
    return result;
}

int factorialWorker(void* arg) {
    FactorialSegment* segment = (FactorialSegment*)arg;
    segment->result = sequentialFactorial(segment->start, segment->end);
    
    mutex_lock(&resultMutex);
    calculationsCompleted++;
    if (calculationsCompleted == NUM_FACT_THREADS) {
        condition_signal(&calculationDone);
    }
    mutex_unlock(&resultMutex);

    return 0;
}

unsigned long long parallelFactorial(int n) {
    int segmentSize = n / NUM_FACT_THREADS;
    Thread threads[NUM_FACT_THREADS];
    FactorialSegment segments[NUM_FACT_THREADS];

    mutex_init(&resultMutex, MUTEX_PLAIN);
    condition_init(&calculationDone);

    for (int i = 0; i < NUM_FACT_THREADS; ++i) {
        segments[i].start = i * segmentSize + 1;
        segments[i].end = (i == NUM_FACT_THREADS - 1) ? n : (i + 1) * segmentSize;
        thread_create(&threads[i], factorialWorker, &segments[i]);
    }

    mutex_lock(&resultMutex);
    while (calculationsCompleted < NUM_FACT_THREADS) {
        condition_wait(&calculationDone, &resultMutex);
    }
    mutex_unlock(&resultMutex);

    unsigned long long finalResult = 1;
    for (int i = 0; i < NUM_FACT_THREADS; ++i) {
        finalResult *= segments[i].result;
    }

    mutex_destroy(&resultMutex);
    condition_destroy(&calculationDone);

    return finalResult;
}

int main() {
    int n = 20; // Example: Calculate 20!
    
    unsigned long long result = parallelFactorial(n);
    fmt_printf("Factorial of %d is %llu\n", n, result);

    return 0;
}
```

## Example 10 : Simple Multithreading 

`This example demonstrates how to create and join multiple threads. Each thread will print its thread ID and a message, showcasing basic thread managemen`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

// Thread function
int printMessage(void* arg) {
    const char* message = (const char*)arg;
    unsigned long threadID = thread_current();

    fmt_printf("Thread %lu: %s\n", threadID, message);
    
    return 0;
}

int main(void) {
    Thread thread1, thread2;

    // Create threads
    thread_create(&thread1, printMessage, "Hello from Thread 1");
    thread_create(&thread2, printMessage, "Hello from Thread 2");

    // Wait for threads to finish
    thread_join(thread1, NULL);
    thread_join(thread2, NULL);

    fmt_printf("Main thread: All threads have finished.\n");

    return 0;
}
```

## Example 11 : Using Mutex for Mutual Exclusion

`This example shows how to use a mutex to ensure mutual exclusion when accessing a shared resource. Two threads will increment a shared counter, but access to the counter is synchronized using a mutex to prevent race conditions.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

int sharedCounter = 0;
Mutex counterMutex;

// Thread function to increment the shared counter
int incrementCounter(void* arg) {
    (void)arg; 

    for (int i = 0; i < 100000; ++i) {
        mutex_lock(&counterMutex);
        sharedCounter++;
        mutex_unlock(&counterMutex);
    }

    return 0;
}

int main(void) {
    Thread thread1, thread2;

    // Initialize the mutex
    mutex_init(&counterMutex, MUTEX_PLAIN);

    // Create and start threads
    thread_create(&thread1, incrementCounter, NULL);
    thread_create(&thread2, incrementCounter, NULL);

    // Wait for threads to finish
    thread_join(thread1, NULL);
    thread_join(thread2, NULL);

    // Destroy the mutex
    mutex_destroy(&counterMutex);

    fmt_printf("Main thread: Final counter value is %d.\n", sharedCounter);

    return 0;
}
```

## Example 12 : Synchronized Logging System 

`This example demonstrates how to implement a synchronized logging system using threads. Each thread generates log messages, and a dedicated logging thread writes these messages to a log file. The communication between threads is synchronized using mutexes and condition variables to ensure thread safety.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include "log/log.h"

#define LOG_QUEUE_SIZE 10
#define NUM_WORKER_THREADS 3

typedef struct {
    char message[256];
} LogMessage;

typedef struct {
    LogMessage logQueue[LOG_QUEUE_SIZE];
    int logCount;
    Mutex logMutex;
    ThreadCondition logNotEmpty;
    ThreadCondition logNotFull;
    int stopLogging;
    Log* logger; 
} Logger;

// Logging thread function using the custom Log library
int loggerThreadFunc(void* arg) {
    Logger* loggerStruct = (Logger*)arg;
    loggerStruct->logger = log_init(); 

    if (!loggerStruct->logger) {
        fmt_fprintf(stderr, "Failed to initialize logging system.\n");
        return -1;
    }

    while (!loggerStruct->stopLogging || loggerStruct->logCount > 0) {
        mutex_lock(&loggerStruct->logMutex);

        while (loggerStruct->logCount == 0 && !loggerStruct->stopLogging) {
            condition_wait(&loggerStruct->logNotEmpty, &loggerStruct->logMutex);
        }
        if (loggerStruct->logCount > 0) {
            log_message(loggerStruct->logger, LOG_LEVEL_INFO, loggerStruct->logQueue[0].message);

            for (int i = 1; i < loggerStruct->logCount; ++i) {
                loggerStruct->logQueue[i - 1] = loggerStruct->logQueue[i];
            }
            --loggerStruct->logCount;
            condition_signal(&loggerStruct->logNotFull);
        }
        mutex_unlock(&loggerStruct->logMutex);
    }

    log_deallocate(loggerStruct->logger); 
    return 0;
}

// Adapted to use the custom logging system
void addLogMessage(Logger* loggerStruct, const char* message) {
    mutex_lock(&loggerStruct->logMutex);
    while (loggerStruct->logCount == LOG_QUEUE_SIZE) {
        condition_wait(&loggerStruct->logNotFull, &loggerStruct->logMutex);
    }

    strncpy(loggerStruct->logQueue[loggerStruct->logCount++].message, message, sizeof(loggerStruct->logQueue[0].message) - 1);
    condition_signal(&loggerStruct->logNotEmpty);
    mutex_unlock(&loggerStruct->logMutex);
}

int main(void) {
    Logger loggerStruct;
    Thread loggerThread;
    int creationResult;

    mutex_init(&loggerStruct.logMutex, MUTEX_PLAIN);
    condition_init(&loggerStruct.logNotEmpty);
    condition_init(&loggerStruct.logNotFull);
    loggerStruct.logCount = 0;
    loggerStruct.stopLogging = 0;

    creationResult = thread_create(&loggerThread, loggerThreadFunc, &loggerStruct);
    if (creationResult != THREAD_SUCCESS) {
        fmt_fprintf(stderr, "Failed to create logger thread.\n");
    }

    for (int i = 0; i < 5; ++i) {
        char message[256];
        sprintf(message, "Log message %d", i + 1);
        addLogMessage(&loggerStruct, message);
        thread_sleep(&(struct timespec){.tv_sec = 1, .tv_nsec = 0}, NULL);
    }

    thread_sleep(&(struct timespec){.tv_sec = 2, .tv_nsec = 0}, NULL);

    if (creationResult == THREAD_SUCCESS) {
        mutex_lock(&loggerStruct.logMutex);
        loggerStruct.stopLogging = 1;
        condition_signal(&loggerStruct.logNotEmpty);
        mutex_unlock(&loggerStruct.logMutex);

        thread_join(loggerThread, NULL);
    }

    mutex_destroy(&loggerStruct.logMutex);
    condition_destroy(&loggerStruct.logNotEmpty);
    condition_destroy(&loggerStruct.logNotFull);

    return 0;
}
```

## Example 13 : Multithreaded File Copy Program using FileReader and FileWriter 

`This example demonstrate a File Copying Program using Multithreading. This example will showcase how to divide the task of copying a large file into smaller chunks, which are then processed in parallel by multiple threads. This approach can potentially speed up the file copying process on systems with multiple cores.`

```c
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include "file_io/file_reader.h"
#include "file_io/file_writer.h"

#define MAX_THREADS 4
#define BUFFER_SIZE 1024*1024 // 1MB buffer

typedef struct {
    const char* sourceFile;
    const char* destFile;
    size_t startOffset;
    size_t endOffset;
} FileCopyTask;

Mutex fileMutex;

// Thread function to copy a file segment
int copyFileSegment(void* arg) {
    FileCopyTask* task = (FileCopyTask*)arg;
    FileReader* source = NULL;
    FileWriter* dest = NULL;

    char buffer[BUFFER_SIZE];
    size_t bytesRead, totalBytesRead = 0;

    // Synchronize the opening of files to prevent concurrent access issues
    mutex_lock(&fileMutex);
    source = file_reader_open(task->sourceFile, READ_BINARY);
    dest = file_writer_open(task->destFile, WRITE_BINARY);
    mutex_unlock(&fileMutex);

    if (!source || !dest) {
        fmt_printf("Error opening files.\n");
        return -1;
    }

    file_reader_seek(source, task->startOffset, POS_BEGIN);
    file_writer_seek(dest, task->startOffset, POS_BEGIN);

    while (totalBytesRead < (task->endOffset - task->startOffset)) {
        size_t bytesToRead = BUFFER_SIZE;
        if ((task->startOffset + totalBytesRead + bytesToRead) > task->endOffset) {
            bytesToRead = task->endOffset - task->startOffset - totalBytesRead;
        }

        bytesRead = file_reader_read(buffer, 1, bytesToRead, source);
        file_writer_write(buffer, 1, bytesRead, dest);
        totalBytesRead += bytesRead;
    }

    file_reader_close(source);
    file_writer_close(dest);
    return 0;
}

void multiThreadedFileCopy(const char* sourceFile, const char* destFile) {
    FileReader* source = file_reader_open(sourceFile, READ_BINARY);
    if (!source) {
        fmt_printf("Error opening source file.\n");
        return;
    }

    long fileSize = file_reader_get_size(source);
    file_reader_close(source);

    FileWriter* dest = file_writer_open(destFile, WRITE_BINARY);
    if (!dest) {
        fmt_printf("Error preparing destination file.\n");
        return;
    }
    file_writer_close(dest); // Close immediately after open just to create or clear the file

    long partSize = fileSize / MAX_THREADS;
    Thread threads[MAX_THREADS];
    FileCopyTask tasks[MAX_THREADS];

    mutex_init(&fileMutex, MUTEX_PLAIN);

    for (int i = 0; i < MAX_THREADS; ++i) {
        tasks[i].sourceFile = sourceFile;
        tasks[i].destFile = destFile;
        tasks[i].startOffset = i * partSize;
        tasks[i].endOffset = (i == MAX_THREADS - 1) ? fileSize : (i + 1) * partSize;

        thread_create(&threads[i], copyFileSegment, &tasks[i]);
    }

    for (int i = 0; i < MAX_THREADS; ++i) {
        thread_join(threads[i], NULL);
    }

    mutex_destroy(&fileMutex);

    fmt_printf("File copy completed.\n");
}

int main(int argc, char* argv[]) {
    // send source and dest over cli 
    if (argc != 3) {
        fmt_printf("Usage: %s <source file> <destination file>\n", argv[0]);
        return 1;
    }

    multiThreadedFileCopy(argv[1], argv[2]);
    return 0;
}
```