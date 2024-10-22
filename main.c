#include <string.h>
#include "fmt/fmt.h"
#include "queue/queue.h"

typedef struct Task {
    int id;
    char description[100];
    int priority;
} Task;

void addTask(Queue* queue, int id, const char* desc, int priority) {
    Task task;
    task.id = id;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-truncation"
        strncpy(task.description, desc, sizeof(task.description));
    #pragma GCC diagnostic pop

    task.priority = priority;
    queue_emplace(queue, &task, sizeof(Task));
}

void processTasks(Queue* queue) {
    while (!queue_empty(queue)) {
        Task* task = (Task*)queue_front(queue);
        fmt_printf("Processing Task ID %d: %s\n", task->id, task->description);
        queue_pop(queue);
    }
}

int main() {
    Queue* taskQueue = queue_create(sizeof(Task));

    addTask(taskQueue, 1, "Fix bug in code", 5);
    addTask(taskQueue, 2, "Update documentation", 3);
    addTask(taskQueue, 3, "Refactor module", 4);

    fmt_printf("Task queue size before processing: %zu\n", queue_size(taskQueue));
    processTasks(taskQueue);
    fmt_printf("Task queue size after processing: %zu\n", queue_size(taskQueue));

    queue_deallocate(taskQueue);
    return 0;
}