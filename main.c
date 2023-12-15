#include "vector/vector.h"
#include "string/string.h"
#include "stack/stack.h"
#include "array/array.h"
#include "queue/queue.h"
#include "priority_queue/priority_queue.h"
// #include "map/map.h"
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

    String *myString = string_create("Hello World");

    // Assign a new string
    myString->assign(myString, "New String");
    printf("String after assign: %s\n", myString->dataStr);

    // Insert a string
    myString->insert(myString, 4, "Test ");
    printf("String after insert: %s\n", myString->dataStr);

    // Erase a portion of the string
    myString->erase(myString, 0, 5);  // Erase "New T"
    printf("String after erase: %s\n", myString->dataStr);

    // Deallocate and clean up
    myString->clear(myString);
    myString->deallocate(myString);



    // String *str1 = string_create("Hello World");
    // str1->replace(str1, "World", "There");
    // printf("After replace: %s\n", str1->dataStr);

    // // Swap example
    // String *str2 = string_create("Sample Text");
    // str2->swap(str1, str2);
    // printf("After swap, str1: %s, str2: %s\n", str1->dataStr, str2->dataStr);

    // // Pop back example
    // str2->pop_back(str2);
    // printf("After pop back: %s\n", str2->dataStr);


    // str1->clear(str1);
    // str2->clear(str2);
    // // Deallocate and clean up
    // str1->deallocate(str1);
    // str2->deallocate(str2);

    // String *myString = string_create("Hello World");

    // // Get the last character
    // char lastChar = myString->back(myString);
    // printf("Last character: %c\n", lastChar);

    // // Get the first character
    // char firstChar = myString->front(myString);
    // printf("First character: %c\n", firstChar);
    // printf("Max size is %zu\n", myString->max_size(myString));
    // // Deallocate and clean up
    // myString->clear(myString);
    // myString->deallocate(myString);


    // String *myString = string_create("Hello, World!");

    // // Copy example
    // char buffer[50];
    // size_t copied = myString->copy(myString, buffer, 7, 5);
    // printf("Copied '%s' (%zu characters)\n", buffer, copied);

    // // Find example
    // int findPos = myString->find(myString, "World", 0);
    // printf("Found 'World' at position: %d\n", findPos);

    // // Deallocate and clean up
    // myString->clear(myString);
    // myString->deallocate(myString);
    

    // String *myString = string_create("Hello, World!");

    // // rfind example
    // int rfindPos = myString->rfind(myString, "o,", myString->length(myString) - 1);
    // printf("Last 'o' found at position: %d\n", rfindPos);

    // int findFirstOfPos = myString->find_first_of(myString, "World", 0);
    // printf("First occurrence of 'World' found at position: %d\n", findFirstOfPos);

    // // find_last_of example
    // int findLastOfPos = myString->find_last_of(myString, "World", myString->length(myString) - 1);
    // printf("Last occurrence of 'World' found at position: %d\n", findLastOfPos);

    // // Deallocate and clean up
    // myString->clear(myString);
    // myString->deallocate(myString);

    // String *myString = string_create("Hello, World!");

    // // find_first_not_of example
    // int firstNotOfPos = myString->find_first_not_of(myString, "Hello", 0);
    // printf("First position not matching 'Hello': %d\n", firstNotOfPos);

    // // find_last_not_of example
    // int lastNotOfPos = myString->find_last_not_of(myString, "World", myString->length(myString) - 1);
    // printf("Last position not matching 'World': %d\n", lastNotOfPos);

    // // Data example
    // const char *data = myString->data(myString);
    // printf("Data: %s\n", data);

    // // Deallocate and clean up
    // myString->clear(myString);
    // myString->deallocate(myString);

    // String *myString = string_create("Hello amin");

    // for (char* it = myString->begin(myString); it != myString->end(myString); it++)
    // {
    //     printf("%c", *it);
    // }

    // myString->clear(myString);
    // myString->deallocate(myString);

    // String *myString = string_create("Hello amin");

    // // Reverse iteration using rbegin and rend
    // printf("String in reverse: ");

    // for (char* it = myString->rbegin(myString); it != myString->rend(myString); --it) 
    // {
    //     printf("%c", *it);
    // }
    // printf("\n");

    // // Clean up
    // myString->clear(myString);
    // myString->deallocate(myString);
    // free(myString);

    // String *myString = string_create("Hello, World!");

    // // Iterate using cbegin and cend
    // printf("String using cbegin and cend: ");
    // for (const char* it = myString->cbegin(myString); it != myString->cend(myString); ++it) 
    // {
    //     printf("%c", *it);
    // }
    // printf("\n");

    // // Reverse iterate using crbegin and crend
    // printf("String in reverse using crbegin and crend: ");
    // for (const char* it = myString->crbegin(myString); it != myString->crend(myString); --it) 
    // {   
        
    //     printf("%c", *it);
    // }
    // printf("\n");

    // // Clean up
    // myString->deallocate(myString);
    // free(myString);

    // Stack* stack = stack_create(sizeof(int));

    // int arr[] = {10, 20, 30, 40, 50};

    // for (int i = 0; i < 5; i++)
    // {
    //     stack->push(stack, &arr[i]);
    // }

    // printf("Size of stack is %d\n", stack->size(stack));
    
    // if (!stack->empty(stack))
    // {
    //     int topValue = *(int*)stack->top(stack);
    //     printf("Top Element is %d\n", topValue);

    //     int pop = *(int*)stack->pop(stack);
    //     printf("Pop value is %d\n", pop);
    //     printf("After Pop size is %d\n", stack->size(stack));
    // }


    // stack->deallocate(stack);
    // free(stack);


    // String* myString = string_create("");
    // Stack* stack = stack_create(sizeof(char*));

    // char* value1 = "Amin";
    // myString->append(myString, value1);
    // stack->push(stack, &myString);

    // char* value2 = "Tahmasebi";
    // myString->append(myString, value2);
    // stack->push(stack, &myString);

    // char* value3 = "C Programming";
    // myString->append(myString, value3);
    // stack->push(stack, &myString);


    // printf("Size of Stack is %d\n", stack->size(stack));

    // String** str1 = (String**)stack->pop(stack);
    // printf("%s", (*str1)->c_str(*str1));

    // (*str1)->deallocate(*str1);
    // stack->deallocate(stack);
    // free(*str1);
    // free(stack);


    // Stack* stk1 = stack_create(sizeof(int));
    // Stack* stk2 = stack_create(sizeof(int));


    // if (stk1->is_equal(stk1, stk2)) {
    //     printf("stk1 is equal to stk2\n");
    // }

    // if (stk1->is_less(stk1, stk2)) {
    //     printf("stk1 is less than stk2\n");
    // }

    // if (stk1->is_greater(stk1, stk2)) {
    //     printf("stk1 is greater than stk2\n");
    // }

    // if (stk1->is_less_or_equal(stk1, stk2)) {
    //     printf("stk1 is less than or equal to stk2\n");
    // }

    // if (stk1->is_greater_or_equal(stk1, stk2)) {
    //     printf("stk1 is greater than or equal to stk2\n");
    // }

    // if (stk1->is_not_equal(stk1, stk2)) {
    //     printf("stk1 is not equal to stk2\n");
    // }

    // // Clean up the stacks...
    // stk1->deallocate(stk1);
    // stk2->deallocate(stk2);
    // free(stk1);
    // free(stk2);


    // String* str1 = string_create("Hello");
    // String* str2 = string_create("World");

    // if (str1->is_equal(str1, str2)) 
    // {
    //     printf("Strings are equal\n");
    // }

    // if (str1->is_less(str1, str2)) 
    // {
    //     printf("String 1 is less than String 2\n");
    // }


    // str1->deallocate(str1);
    // str2->deallocate(str2);

    // free(str1);
    // free(str2);
     
    /// Array

    // Array* intArray = array_create(sizeof(int), 5);

    // if (intArray == NULL) {
    //     printf("Failed to create array.\n");
    //     return 1;
    // }

    // int valueToFill = 42;
    // intArray->fill(intArray, &valueToFill);

    // printf("Array elements:\n");
    // for (size_t i = 0; i < intArray->size(intArray); ++i) 
    // {
    //     int* element = (int*)intArray->at(intArray, i);

    //     if (element != NULL) 
    //         printf("Element %zu: %d\n", i, *element);
    // }

    // // Check if the array is empty
    // if (intArray->empty(intArray)) {
    //     printf("Array is empty.\n");
    // } else {
    //     printf("Array is not empty.\n");
    // }

    // // Access the front and back elements
    // int* frontElement = (int*)intArray->front(intArray);
    // int* backElement = (int*)intArray->back(intArray);

    // if (frontElement != NULL && backElement != NULL) 
    //     printf("Front element: %d, Back element: %d\n", *frontElement, *backElement);
    
    // for (size_t i = 0; i < intArray->size(intArray); ++i) 
    // {
    //     int value = i * 2; 
    //     intArray->set(intArray, i, &value);
    //     int* element = (int*)intArray->at(intArray, i);

    //     if (element != NULL) 
    //         printf("Element %zu: %d\n", i, *element);
        
    // }
    // // Deallocate the array
    // intArray->deallocate(intArray);
    // free(intArray);

    // Queue* myQueue = queue_create(sizeof(int));

    // if (!myQueue) 
    // {
    //     fprintf(stderr, "Failed to create queue.\n");
    //     return EXIT_FAILURE;
    // }

    // // Push some integers onto the queue
    // int values[] = {10, 20, 30, 40, 50};
    // for (int i = 0; i < 5; ++i) 
    // {
    //     myQueue->push(myQueue, &values[i]);
    // }

    // // Print the size of the queue
    // printf("Queue size: %zu\n", myQueue->size(myQueue));

    // // Check if the queue is empty
    // printf("Is the queue empty? %s\n", myQueue->empty(myQueue) ? "Yes" : "No");

    // // Access the front and back elements
    // int* front = myQueue->front(myQueue);
    // int* back = myQueue->back(myQueue);
    // if (front && back) 
    // {
    //     printf("Front element: %d\n", *front);
    //     printf("Back element: %d\n", *back);
    // }

    // // Pop an element from the queue
    // myQueue->pop(myQueue);
    // front = myQueue->front(myQueue);

    // if (front) 
    //     printf("New front element after pop: %d\n", *front);

    // myQueue->deallocate(myQueue);
    

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

    ForwardList *myList = forward_list_create(sizeof(char*));
    if (myList == NULL) {
        printf("Failed to create list.\n");
        return 1;
    }

    char *str1 = "Hello";
    char *str2 = "World";
    char *str3 = "Example";
    char *str4 = "OpenAI";
    char *str5 = "ChatGPT";

    myList->push_front(myList, str1);
    myList->push_front(myList, str2);
    myList->emplace_front(myList, str3);
    myList->push_front(myList, str4); // Pushing another string
    myList->emplace_front(myList, str5); 

    if (myList->empty(myList)) {
    printf("List is empty.\n");
    } else {
        printf("List is not empty.\n");

        ForwardListNode *node = myList->begin(myList);
        while (node != myList->end(myList)) {
            if (node->value != NULL) {
                printf("%s ", (char *)node->value);
            } else {
                printf("(null) ");
            }
            node = node->next;
        }
        printf("\n");
    }

    char *frontElement = myList->front(myList);
    if (frontElement != NULL) {
        printf("Front element: %s\n", frontElement);
    }

    ForwardList *anotherList = forward_list_create(sizeof(char*));
    char *extra1 = "Extra1";
    char *extra2 = "Extra2";
    anotherList->push_front(anotherList, extra1);
    anotherList->push_front(anotherList, extra2);

    myList->merge(myList, anotherList); // Merge anotherList into myList

        // Remove a specific element
    myList->remove_if(myList, conditionToRemove);

    // Remove the front element
    myList->pop_front(myList);

    myList->clear(myList);
    myList->deallocate(myList);


    getchar();
    return 0;
}