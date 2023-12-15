#include "vector/vector.h"
#include "string/string.h"
#include "stack/stack.h"
#include "array/array.h"
#include "queue/queue.h"
#include "priority_queue/priority_queue.h"
#include "map/map.h"
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

    Vector* vec1 = vector_create(sizeof(int));
    // Vector* vec2 = vector_create(sizeof(int));

    // int items1[] = {1, 2, 3};
    // for (int i = 0; i < 3; i++) 
    // {
    //     vec1->push_back(vec1, &items1[i]);
    // }

    // int items2[] = {1, 2, 4};
    // for (int i = 0; i < 3; i++) 
    // {
    //     vec2->push_back(vec2, &items2[i]);
    // }

    // if (vec1->is_equal(vec1, vec2)) 
    // {
    //     printf("vec1 is equal to vec2\n");
    // } 
    // else 
    // {
    //     printf("vec1 is not equal to vec2\n");
    // }

    // if (vec1->is_less(vec1, vec2)) 
    // {
    //     printf("vec1 is less than vec2\n");
    // }

    // if (vec1->is_greater(vec2, vec1)) 
    // {
    //     printf("vec2 is greater than vec2\n");
    // }

    // // Cleanup
    // vec1->deallocate(vec1);
    // vec2->deallocate(vec2);
    // free(vec1);
    // free(vec2);

    // Vector *intVector = vector_create(sizeof(int));

    // int value1 = 10, value2 = 20, value3 = 30;
    // intVector->push_back(intVector, &value1);
    // intVector->push_back(intVector, &value2);
    // intVector->push_back(intVector, &value3);

    // for (size_t i = 0; i < (intVector->length(intVector)); ++i) 
    // {
    //     int* item = (int*) intVector->at(intVector, i);
    //     printf("%d\n", *item);
    // }

    // Vector *stringVector = vector_create(sizeof(char*));

    // char *str1 = "Hello";
    // char *str2 = "World";
    // char *str3 = "Example";

    // stringVector->push_back(stringVector, &str1);
    // stringVector->push_back(stringVector, &str2);
    // stringVector->push_back(stringVector, &str3);

    // for (size_t i = 0; i < stringVector->length(stringVector); ++i) 
    // {
    //     char **item = (char**) stringVector->at(stringVector, i);
    //     printf("%s\n", *item);
    // }


    // Vector *structVector = vector_create(sizeof(MyStruct));

    // MyStruct s1 = {1, 10.5};
    // MyStruct s2 = {2, 20.5};
    // MyStruct s3 = {3, 30.5};

    // structVector->push_back(structVector, &s1);
    // structVector->push_back(structVector, &s2);
    // structVector->push_back(structVector, &s3);

    // for (size_t i = 0; i < structVector->length(structVector); ++i) 
    // {
    //     MyStruct* item = (MyStruct*) structVector->at(structVector, i);
    //     printf("ID: %d, Value: %.2f\n", item->id, item->value);
    // }

    // Vector *doubleVector = vector_create(sizeof(double));

    // double valueDouble1 = 1.1, valueDouble2 = 2.2, valueDouble3 = 3.3;
    // doubleVector->push_back(doubleVector, &valueDouble1);
    // doubleVector->push_back(doubleVector, &valueDouble2);
    // doubleVector->push_back(doubleVector, &valueDouble3);

    // for (size_t i = 0; i < doubleVector->length(doubleVector); ++i) 
    // {
    //     double* item = (double*) doubleVector->at(doubleVector, i);
    //     printf("%.2f\n", *item);
    // }

    // Vector *myVector = vector_create(sizeof(int));

    // // Check if the vector is empty
    // if (myVector->is_empty(myVector)) 
    // {
    //     printf("The vector is empty.\n");
    // } 
    // else 
    // {
    //     printf("The vector is not empty.\n");
    // }

    // Vector *vector1 = vector_create(sizeof(int));
    // Vector *vector2 = vector_create(sizeof(int));

    // int value1 = 10, value2 = 20;
    // vector1->push_back(vector1, &value1);
    // vector1->push_back(vector1, &value2);


    // int value3 = 30, value4 = 40;
    // vector2->push_back(vector2, &value3);
    // vector2->push_back(vector2, &value4);

    // vector1->swap(vector1, vector2);

    // printf("Contents of vector1 after swap:\n");
    // for (size_t i = 0; i < vector1->length(vector1); ++i) 
    // {
    //     int* item = (int*) vector1->at(vector1, i);
    //     printf("%d\n", *item);
    // }

    // printf("Contents of vector2 after swap:\n");
    // for (size_t i = 0; i < vector2->length(vector2); ++i) 
    // {
    //     int* item = (int*) vector2->at(vector2, i);
    //     printf("%d\n", *item);
    // }


    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements to the vector
    // int values[] = {10, 20, 30, 40};
    // for (int i = 0; i < 4; ++i) 
    // {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Pop the last element and access it
    // int *poppedItem = (int *)intVector->pop_back(intVector);
    // if (poppedItem) 
    // {
    //     printf("Popped item: %d\n", *poppedItem);
    // }


    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements to the vector
    // int values[] = {10, 20, 30, 40};
    // for (int i = 0; i < 4; ++i) 
    // {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Clearing the vector
    // intVector->clear(intVector);

    // // Check if the vector is empty
    // if (intVector->is_empty(intVector)) 
    // {
    //     printf("The vector is now empty.\n");
    // } 
    // else 
    // {
    //     printf("The vector is not empty.\n");
    // }


    // Vector *intVector = vector_create(sizeof(int));

    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) 
    // {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // int *frontItem = (int *)intVector->front(intVector);
    // if (frontItem) 
    // {
    //     printf("First item: %d\n", *frontItem);
    // }

    // Vector *intVector = vector_create(sizeof(int));

    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // int *backItem = (int *)intVector->back(intVector);
    // if (backItem) {
    //     printf("Last item: %d\n", *backItem);
    // }

    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements to the vector
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Accessing the underlying array
    // int *data = (int *)intVector->data(intVector);
    // if (data) {
    //     for (size_t i = 0; i < intVector->length(intVector); ++i) {
    //         printf("Item %zu: %d\n", i, data[i]);
    //     }
    // }

    // intVector->clear(intVector);


    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Using begin and end to iterate over the vector
    // for (int *it = (int *)intVector->begin(intVector); it != (int *)intVector->end(intVector); ++it) {
    //     printf("%d\n", *it);
    // }


    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Using cbegin and cend for read-only iteration over the vector
    // for (const int *it = (const int *)intVector->cbegin(intVector); it != (const int *)intVector->cend(intVector); ++it) {
    //     printf("%d\n", *it);
    // }


    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Using crbegin and crend for reverse read-only iteration over the vector
    // for (const int *it = (const int *)intVector->crbegin(intVector); it != (const int *)intVector->crend(intVector); --it) {
    //     printf("%d\n", *it);
    // }

    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Using rbegin and rend for reverse iteration over the vector
    // for (int *it = (int *)intVector->rbegin(intVector); it != (int *)intVector->rend(intVector); --it) 
    // {
    //     printf("%d\n", *it);
    // }

    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements to the vector
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Insert a new element
    // int newElement = 25;
    // intVector->insert(intVector, 0, &newElement); // Inserts 25 at position 1

    // // Print elements after insertion
    // for (size_t i = 0; i < intVector->length(intVector); ++i) {
    //     int* item = (int*) intVector->at(intVector, i);
    //     printf("%d\n", *item);
    // }


    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements to the vector
    // int values[] = {10, 20, 30, 40, 50};
    // for (int i = 0; i < 5; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Erase elements from position 1, removing 2 elements
    // intVector->erase(intVector, 1, 2); // Should remove 20 and 30

    // // Print elements after erasure
    // for (size_t i = 0; i < intVector->length(intVector); ++i) {
    //     int* item = (int*) intVector->at(intVector, i);
    //     printf("%d\n", *item);
    // }

    // Vector *intVector = vector_create(sizeof(int));

    // // Reserve capacity for at least 10 elements
    // intVector->reserve(intVector, 10);

    // // Add elements and observe no need for reallocation until the 11th element
    // for (int i = 0; i < 11; ++i) {
    //     int value = i * 10;
    //     intVector->push_back(intVector, &value);
    // }

    // printf("Vector size: %zu, Vector capacity: %zu\n", intVector->length(intVector), intVector->capacity(intVector));

    // Vector *intVector = vector_create(sizeof(int));

    // // Adding elements to the vector
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // // Resize to a larger size (5)
    // intVector->resize(intVector, 5);

    // printf("After resizing to larger size:\n");
    // for (size_t i = 0; i < intVector->length(intVector); ++i) {
    //     int* item = (int*) intVector->at(intVector, i);
    //     printf("%d\n", *item); // The last two elements will be zero-initialized
    // }

    // // Resize to a smaller size (2)
    // intVector->resize(intVector, 2);

    // printf("After resizing to smaller size:\n");
    // for (size_t i = 0; i < intVector->length(intVector); ++i) {
    //     int* item = (int*) intVector->at(intVector, i);
    //     printf("%d\n", *item); // Only the first two elements remain
    // }

    // intVector->clear(intVector);

    // Vector *intVector = vector_create(sizeof(int));

    // // Reserve larger capacity
    // intVector->reserve(intVector, 10);

    // // Adding a few elements
    // int values[] = {10, 20, 30};
    // for (int i = 0; i < 3; ++i) {
    //     intVector->push_back(intVector, &values[i]);
    // }

    // printf("Size before shrink_to_fit: %zu, Capacity before shrink_to_fit: %zu\n", intVector->length(intVector), intVector->capacity(intVector));

    // // Shrink to fit the actual number of elements
    // intVector->shrink_to_fit(intVector);

    // printf("Size after shrink_to_fit: %zu, Capacity after shrink_to_fit: %zu\n", intVector->length(intVector), intVector->capacity(intVector));

    // Vector *stringVector = vector_create(sizeof(char*));
    // char* values[] = {"Hello", "World", "Vector", "Example"};

    // for (int i = 0; i < 4; ++i) 
    // {
    //     stringVector->push_back(stringVector, &values[i]);
    // }

    // char* newValue = "NewString";
    // stringVector->assign(stringVector, 1, &newValue);

    // char* emplaceValue = "EmplacedString";
    // stringVector->emplace(stringVector, 2, &emplaceValue, sizeof(char*));

    // char* emplaceBackValue = "EmplacedBackString";
    // stringVector->emplace_back(stringVector, &emplaceBackValue, sizeof(char*));

    // for (size_t i = 0; i < stringVector->length(stringVector); ++i) 
    // {
    //     char** item = (char **)stringVector->at(stringVector, i);
    //     printf("%s\n", *item);
    // }

    // String 

    // String* myString = string_create("");
    // Vector* vec = vector_create(sizeof(String*));

    // String* myString1 = string_create("Hello");
    // String* myString2 = string_create("World");
    // String* myString3 = string_create("Example");

    // vec->push_back(vec, &myString1);
    // vec->push_back(vec, &myString2);
    // vec->push_back(vec, &myString3);

    // for (size_t i = 0; i < vec->length(vec); ++i) 
    // {
    //     String** strPtr = (String**) vec->at(vec, i);
    //     if (strPtr && *strPtr) 
    //     {
    //         printf("%s\n", (*strPtr)->dataStr);
    //     }
    // }

    // for (size_t i = 0; i < vec->length(vec); ++i) 
    // {
    //     String** strPtr = (String**) vec->at(vec, i);

    //     if (strPtr && *strPtr) 
    //     {
    //         (*strPtr)->deallocate(*strPtr);
    //         free(*strPtr);
    //     }
    // }


    // vec->clear(vec);

    // // Push back a new character
    // myString->push_back(myString, '!');
    // printf("String after push_back: %s\n", myString->dataStr);

    // // Access a character
    // char ch = myString->at(myString, 1);
    // printf("Character at index 1: %c\n", ch);

    // // Print length and capacity
    // printf("Length: %zu, Capacity: %zu\n", myString->length(myString), myString->capacity(myString));

    // // Clear the string
    // myString->clear(myString);
    // printf("String after clear: %s\n", myString->dataStr);

    // // Clean up
    // myString->clear(myString);


    
    // String *myString = string_create("Hello");

    // // Append a string
    // myString->append(myString, ", World!");
    // printf("String after append: %s\n", myString->dataStr);

    // // Resize the string
    // myString->resize(myString, 5);  // Resize to "Hello"
    // printf("String after resize: %s\n", myString->dataStr);

    // // Shrink to fit
    // myString->shrink_to_fit(myString);
    // printf("String capacity after shrink to fit: %zu\n", myString->capacity(myString));

    // // Deallocate and clean up
    // myString->clear(myString);
    // myString->deallocate(myString);
    // free(myString);



    // String *myString = string_create("Hello World");

    // // Assign a new string
    // myString->assign(myString, "New String");
    // printf("String after assign: %s\n", myString->dataStr);

    // // Insert a string
    // myString->insert(myString, 4, "Test ");
    // printf("String after insert: %s\n", myString->dataStr);

    // // Erase a portion of the string
    // myString->erase(myString, 0, 5);  // Erase "New T"
    // printf("String after erase: %s\n", myString->dataStr);

    // // Deallocate and clean up
    // myString->clear(myString);
    // myString->deallocate(myString);
    // free(myString);


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