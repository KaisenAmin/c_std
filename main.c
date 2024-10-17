#include "fmt/fmt.h"
#include "queue/queue.h"

int main() {
    Queue* myQueue1 = queue_create(sizeof(int));
    Queue* myQueue2 = queue_create(sizeof(int));
    int values1[] = {10, 20, 30, 40, 50};
    
    for (int i = 0; i < 5; ++i) {
        queue_push(myQueue1, &values1[i]);
    }

    int values2[] = {15, 25, 35, 45, 55};
    for (int i = 0; i < 5; ++i) { 
        queue_emplace(myQueue2, &values2[i], sizeof(int));
    }

    queue_swap(myQueue1, myQueue2);

    int* front1 = queue_front(myQueue1);
    int* front2 = queue_front(myQueue2);

    if (front1 && front2) {
        fmt_printf("Front element of myQueue1 after swap: %d\n", *front1);
        fmt_printf("Front element of myQueue2 after swap: %d\n", *front2);
    }

    queue_deallocate(myQueue1);
    queue_deallocate(myQueue2);

    return 0;
}