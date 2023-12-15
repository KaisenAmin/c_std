#include "vector/vector.h"
#include "string/string.h"
#include "stack/stack.h"
#include "array/array.h"
#include "queue/queue.h"
#include "priority_queue/priority_queue.h"
#include "list/list.h"
#include "forward_list/forward_list.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    int id;
    double value;

} MyStruct;

// Comparison function for integers (for a max heap)
static int compare_ints(const void* a, const void* b) 
{
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;

    return (int_a > int_b) - (int_a < int_b);
}

int int_compare(const void *a, const void *b) 
{
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return (*ia > *ib) - (*ia < *ib);
}

int string_compare(const void *a, const void *b) 
{
    return strcmp((const char *)a, (const char *)b);
}

void print_found(const char *func, int key, void *value) 
{
    if (value) {
        printf("%s: Key %d found with value %d\n", func, key, *(int*)value);
    } else {
        printf("%s: Key %d not found\n", func, key);
    }
}

bool conditionToRemove(void *value) {
    char *str = (char *)value;
    return strcmp(str, "World") == 0;
}

int main(int argc, char** argv)
{


    // Pop an element from the queue
    myQueue->pop(myQueue);
    front = myQueue->front(myQueue);

    if (front) 
        printf("New front element after pop: %d\n", *front);

    myQueue->deallocate(myQueue);
    

    // Queue* myQueue1 = queue_create(sizeof(int));
    // Queue* myQueue2 = queue_create(sizeof(int));

    // if (!myQueue1 || !myQueue2) 
    // {
    //     fprintf(stderr, "Failed to create queues.\n");
    //     return EXIT_FAILURE;
    // }

    // // Push some integers onto the first queue
    // int values1[] = {10, 20, 30, 40, 50};
    // for (int i = 0; i < 5; ++i) 
    // {
    //     myQueue1->push(myQueue1, &values1[i]);
    // }

    // int values2[] = {15, 25, 35, 45, 55};
    // for (int i = 0; i < 5; ++i) 
    // {
    //     myQueue2->emplace(myQueue2, &values2[i], sizeof(int));
    // }

    // // Compare the two queues
    // printf("Are the queues equal? %s\n", myQueue1->is_equal(myQueue1, myQueue2) ? "Yes" : "No");
    // printf("Is myQueue1 less than myQueue2? %s\n", myQueue1->is_less(myQueue1, myQueue2) ? "Yes" : "No");

    // // Swap the two queues
    // myQueue1->swap(myQueue1, myQueue2);

    // // Check the front element of the swapped queues
    // int* front1 = myQueue1->front(myQueue1);
    // int* front2 = myQueue2->front(myQueue2);
    // if (front1 && front2) 
    // {
    //     printf("Front element of myQueue1 after swap: %d\n", *front1);
    //     printf("Front element of myQueue2 after swap: %d\n", *front2);
    // }

    // // Pop an element from myQueue1
    // myQueue1->pop(myQueue1);
    // front1 = myQueue1->front(myQueue1);
    // if (front1) 
    //     printf("New front element of myQueue1 after pop: %d\n", *front1);
    

    // // Clean up
    // myQueue1->deallocate(myQueue1);
    // myQueue2->deallocate(myQueue2);
    // free(myQueue1);
    // free(myQueue2);

    // Clean up

    // PriorityQueue* pq = priority_queue_create(sizeof(int), compare_ints);

    // if (!pq) 
    // {
    //     fprintf(stderr, "Failed to create priority queue.\n");
    //     return EXIT_FAILURE;
    // }

    // // Push some integers onto the priority queue
    // int values[] = {5, 10, 3, 7, 4};
    // for (int i = 0; i < 5; ++i) {
    //     pq->push(pq, &values[i]);
    // }

    // // Print the size of the priority queue
    // printf("Priority Queue size: %zu\n", pq->size(pq));

    // // Check if the priority queue is empty
    // printf("Is the priority queue empty? %s\n", pq->empty(pq) ? "Yes" : "No");

    // // Access the top element
    // int* top = pq->top(pq);
    // if (top) {
    //     printf("Top element: %d\n", *top);
    // }

    // // Pop the top element and access the new top
    // pq->pop(pq);
    // top = pq->top(pq);
    // if (top) {
    //     printf("New top element after pop: %d\n", *top);
    // }

    // // Deallocate the priority queue
    // pq->deallocate(pq);

    // size_t keySize = sizeof(int);
    // size_t valueSize = sizeof(int);
    // Map *intMap = map_create(keySize, valueSize, int_compare);

    // // Check if the map was successfully created
    // if (!intMap) 
    // {
    //     printf("Failed to create map.\n");
    //     return 1;
    // }

    // int keys[] = {10, 20, 30};
    // int values[] = {100, 200, 300};

    // for (int i = 0; i < 3; i++) 
    // {
    //     intMap->insert(intMap, &keys[i], &values[i]);
    // }

    // size_t keySize = sizeof(int);
    // size_t valueSize = sizeof(int);
    // Map *intMap = map_create(keySize, valueSize, int_compare);

    // // Insert some key-value pairs
    // int keys[] = {1, 2, 3, 4, 5};
    // int values[] = {10, 20, 30, 40, 50};
    // for (int i = 0; i < 5; i++) {
    //     intMap->insert(intMap, &keys[i], &values[i]);
    // }

    // // Access a value using 'at'
    // int keyToFind = 3;
    // int *value = intMap->at(intMap, &keyToFind);
    // if (value) {
    //     printf("Value at key %d is %d\n", keyToFind, *value);
    // }

    // // Count the occurrences of a key
    // size_t count = intMap->count(intMap, &keyToFind);
    // printf("Count for key %d is %zu\n", keyToFind, count);

    // // Get the length of the map
    // size_t length = intMap->length(intMap);
    // printf("Map length is %zu\n", length);

    // // Get the maximum size of the map
    // size_t maxSize = intMap->max_size(intMap);
    // printf("Map maximum size is %zu\n", maxSize);

    // // Erase a key-value pair
    // int keyToRemove = 2;
    // intMap->erase(intMap, &keyToRemove);
    // printf("Key %d removed.\n", keyToRemove);

    // // Try to find the removed key
    // int *removedValue = intMap->at(intMap, &keyToRemove);
    // if (removedValue) {
    //     printf("Key %d is not in the map anymore.\n", keyToRemove);
    // }

    // // Swap with another map
    // Map *anotherMap = map_create(keySize, valueSize, int_compare);
    // intMap->swap(intMap, anotherMap);
    // printf("Maps swapped.\n");

    // // Check the size of the maps after swapping
    // size_t sizeAfterSwapIntMap = intMap->length(intMap);
    // size_t sizeAfterSwapAnotherMap = anotherMap->length(anotherMap);
    // printf("Size of intMap after swap: %zu\n", sizeAfterSwapIntMap);
    // printf("Size of anotherMap after swap: %zu\n", sizeAfterSwapAnotherMap);

    // // Deallocate the maps
    // intMap->deallocate(intMap);
    // anotherMap->deallocate(anotherMap);

    // size_t keySize = sizeof(char *);
    // size_t valueSize = sizeof(double);
    // Map *stringDoubleMap = map_create(keySize, valueSize, string_compare);


    // char *keys[] = {"key1", "key2", "key3", "key4", "key5"};
    // double values[] = {1.1, 2.2, 3.3, 4.4, 5.5};

    // for (int i = 0; i < 5; i++) 
    //     stringDoubleMap->insert(stringDoubleMap, keys[i], &values[i]);

    //     // Access a value
    // char *keyToFind = "key3";
    // double *value = stringDoubleMap->at(stringDoubleMap, keyToFind);

    // if (value) 
    //     printf("Value at key '%s' is %f\n", keyToFind, *value);

    // // Erase a key-value pair
    // char *keyToRemove = "key2";
    // stringDoubleMap->erase(stringDoubleMap, keyToRemove);

    // printf("Key '%s' removed.\n", keyToRemove);

    // // Deallocate the map
    // stringDoubleMap->deallocate(stringDoubleMap);

    // Map *myMap = map_create(sizeof(int), sizeof(int), int_compare);
    // clock_t start, end;
    // double cpu_time_used;
    // start = clock();
    // // Inserting key-value pairs
    // for (int i = 0; i < 100; ++i) 
    //     myMap->insert(myMap, &i, &i);
    
    // printf("after insert\n");
    // end = clock();
    

    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("Time taken for benchmark: %f seconds\n", cpu_time_used);

    // for (int *it = myMap->begin(myMap); it != myMap->end(myMap); it++)
    //     printf("%d, ", );
    // // Benchmark
    // printf("Time taken for benchmark: %f seconds\n", cpu_time_used);
    
    // // Testing find, lower_bound, upper_bound, and equal_range
    // int testKey = 50000;
    // void *foundValue = myMap->find(myMap, &testKey);
    // print_found("Find", testKey, foundValue);

    // foundValue = myMap->lower_bound(myMap, &testKey);
    // print_found("Lower Bound", testKey, foundValue);

    // foundValue = myMap->upper_bound(myMap, &testKey);
    // print_found("Upper Bound", testKey, foundValue);

    // void* equal_range_result = myMap->equal_range(myMap, &testKey);
    // if (equal_range_result) {
    //     int equal_range_value = *(int*)equal_range_result;  // Casting and dereferencing
    //     printf("Equal Range: Key %d found with value %d\n", testKey, equal_range_value);
    // } else {
    //     printf("Equal Range: Key %d not found\n", testKey);
    // }

    // myMap->deallocate(myMap);


    // List *myList = list_create(sizeof(int));
    // myList->compare = compare_ints;

    // // Add some integers to the list
    // // int values[] = {10, 20, 30, 40, 50};
    // clock_t start, end;
    // double calc;
    // start = clock();
    // for (int i = 0; i < 100000000; ++i) 
    //     myList->push_back(myList, &i);
    
    // end = clock();
    // calc = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // printf("%f\n", calc);
    // // Demonstrate some methods
    // // Display the first and last elements
    // int *front = myList->front(myList);
    // int *back = myList->back(myList);
    // printf("First element: %d\n", *front);
    // printf("Last element: %d\n", *back);

    // // Remove the first element
    // myList->pop_front(myList);

    // // Display the new first element
    // front = myList->front(myList);
    // printf("New first element after pop: %d\n", *front);

    // // Clear the list
    // myList->clear(myList);
    // printf("List cleared. Is it empty? %s\n", myList->empty(myList) ? "Yes" : "No");

    // // Deallocate the list
    // myList->deallocate(myList);

    // ForwardList *myList = forward_list_create(sizeof(char*));
    // if (myList == NULL) {
    //     printf("Failed to create list.\n");
    //     return 1;
    // }

    // char *str1 = "Hello";
    // char *str2 = "World";
    // char *str3 = "Example";
    // char *str4 = "OpenAI";
    // char *str5 = "ChatGPT";

    // myList->push_front(myList, str1);
    // myList->push_front(myList, str2);
    // myList->emplace_front(myList, str3);
    // myList->push_front(myList, str4); // Pushing another string
    // myList->emplace_front(myList, str5); 

    // if (myList->empty(myList)) {
    // printf("List is empty.\n");
    // } else {
    //     printf("List is not empty.\n");

    //     ForwardListNode *node = myList->begin(myList);
    //     while (node != myList->end(myList)) {
    //         if (node->value != NULL) {
    //             printf("%s ", (char *)node->value);
    //         } else {
    //             printf("(null) ");
    //         }
    //         node = node->next;
    //     }
    //     printf("\n");
    // }

    // char *frontElement = myList->front(myList);
    // if (frontElement != NULL) {
    //     printf("Front element: %s\n", frontElement);
    // }

    // ForwardList *anotherList = forward_list_create(sizeof(char*));
    // char *extra1 = "Extra1";
    // char *extra2 = "Extra2";
    // anotherList->push_front(anotherList, extra1);
    // anotherList->push_front(anotherList, extra2);

    // myList->merge(myList, anotherList); // Merge anotherList into myList

    //     // Remove a specific element
    // myList->remove_if(myList, conditionToRemove);

    // // Remove the front element
    // myList->pop_front(myList);

    // myList->clear(myList);
    // myList->deallocate(myList);


    getchar();
    return 0;
}