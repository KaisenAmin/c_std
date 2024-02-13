#include "concurrent/thread_pool.h"
#include "fmt/fmt.h"

int example_task_function(void* arg) {
    int* num = (int*)arg;
    fmt_printf("Executing task number: %d\n", *num);

    free(num); 

    return 0;
}

int main() {
    int num_threads = 4;
    ThreadPool* pool = thread_pool_create(num_threads);

    // Add tasks to the pool
    for (int i = 0; i < 10; i++) {
        int* arg = malloc(sizeof(int));
        *arg = i;
        thread_pool_add_task(pool, example_task_function, arg);
    }

    // // Wait for all tasks to complete
    thread_pool_wait(pool);
    thread_pool_destroy(pool);

    return 0;
}