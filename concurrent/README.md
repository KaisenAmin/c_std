

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