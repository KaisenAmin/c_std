# Map Library in C

## Overview
This C library provides a generic implementation of a map, also known as an associative array. The map is implemented as a Red-Black Tree, which is a self-balancing binary search tree. This ensures efficient operations such as insertion, deletion, and lookup.

## Features
- **Generic Key and Value Types:** Can store any type of key-value pairs.
- **Red-Black Tree Implementation:** Guarantees O(log n) time complexity for basic operations.
- **Iterators:** Support for forward and backward traversal.
- **Flexible Memory Management:** Customizable deallocation functions for keys and values.
- **Operations:** Insertion, deletion, lookup, size, swap, and more.

## Usage
Include the `map.h` header file in your C program to use the map functionality. The `map.c` file contains the implementation and should be compiled with your program.


## Example 1 : insert and get value by Key 
```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) 
{
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;

    return (*ia - *ib);
}

void int_deallocator(void* data) {
    free(data);
}

int main() 
{
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);
    int key = 5, value = 10;
    int lookupKey = 5;

    map_insert(myMap, &key, &value);
    
    int *foundValue = map_at(myMap, &lookupKey);
    // int foundValue = *(int*)map_at(myMap, &lookupKey);

    if (foundValue) 
        printf("Found value: %d\n", *foundValue);
    else 
        printf("Key not found.\n");
    
    map_deallocate(myMap);

    return 0;
}
```

### Example 2 : Map is Genreic accept different kind of key and value type 

```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* my_strdup(const char* s) 
{
    if (s == NULL) 
        return NULL;
        
    char* new_str = malloc(strlen(s) + 1);
    if (new_str) 
        strcpy(new_str, s);

    return new_str;
}

int compare_ints(const KeyType a, const KeyType b) 
{
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;
    return (*ia - *ib);
}

void string_deallocator(void* data) {
    free(data);
}

int main() 
{
    Map* myMap = map_create(compare_ints, free, string_deallocator);

    int* key1 = malloc(sizeof(int));
    *key1 = 1;
    char* value1 = my_strdup("Alice");

    int* key2 = malloc(sizeof(int));
    *key2 = 2;
    char* value2 = my_strdup("Bob");

    map_insert(myMap, key1, value1);
    map_insert(myMap, key2, value2);

    int lookupKey = 1;
    char* foundValue = (char*)map_at(myMap, &lookupKey);

    if (foundValue)
        printf("Found value for %d: %s\n", lookupKey, foundValue);
    else
        printf("Key not found.\n");

    map_deallocate(myMap);

    return 0;
}
```

### Example 3 : how to get size of Map 
```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* my_strdup(const char* s) 
{
    if (s == NULL) 
        return NULL;
        
    char* new_str = malloc(strlen(s) + 1);
    if (new_str) 
        strcpy(new_str, s);

    return new_str;
}

int compare_ints(const KeyType a, const KeyType b) 
{
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;

    return (*ia - *ib);
}


int main() 
{
    Map* map1 = map_create(compare_ints, NULL, free);
    int key1 = 1;
    char* value1 = my_strdup("Hello");

    map_insert(map1, &key1, value1);

    // Use and demonstration of map functions
    printf("Size: %zu\n", map_size(map1));
    printf("Value for key 1: %s\n", (char*)map_at(map1, &key1));

    map_deallocate(map1);

    return 0;
}

```

### Example 4 : swap two Map object 
```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_ints(const KeyType a, const KeyType b) 
{
    const int* ia = (const int*)a;
    const int* ib = (const int*)b;

    return (*ia - *ib);
}

int main()
{
    Map* map3a = map_create(compare_ints, NULL, NULL);
    int key3a = 1, value3a = 100;

    map_insert(map3a, &key3a, &value3a);

    Map* map3b = map_create(compare_ints, NULL, NULL);
    int key3b = 2, value3b = 200;
    
    map_insert(map3b, &key3b, &value3b);

    map_swap(map3a, map3b);

    // After swap, map3a should have the contents of map3b and vice versa
    printf("Value in map3a for key 2: %d\n", *(int*)map_at(map3a, &key3b));
    printf("Value in map3b for key 1: %d\n", *(int*)map_at(map3b, &key3a));

    map_deallocate(map3a);
    map_deallocate(map3b);

}

```

### Example 5 : erase Key and value in Map 
```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>

int compare_ints(const KeyType a, const KeyType b) {
    return *(const int*)a - *(const int*)b;
}

void int_deallocator(void* data) {
    free(data);
}

int main() 
{
    Map* myMap = map_create(compare_ints, int_deallocator, int_deallocator);

    // Insert elements
    for (int i = 0; i < 5; ++i) 
    {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 100;
        map_insert(myMap, key, value);
    }

    // Search for a key
    int searchKey = 3;
    int* foundValue = (int*)map_at(myMap, &searchKey);
    if (foundValue) 
        printf("Found value: %d\n", *foundValue);
    else 
        printf("Key not found.\n");

    // Delete a key
    int deleteKey = 2;
    if (map_erase(myMap, &deleteKey)) 
        printf("Key %d erased.\n", deleteKey);
    else 
        printf("Key not found for deletion.\n");

    map_deallocate(myMap);
    return 0;
}

```

### Example 6 : decrement and increment iterator

```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_strings(const KeyType a, const KeyType b) {
    return strcmp((const char*)a, (const char*)b);
}

void string_deallocator(void* data){
    free(data);
}

char* my_strdup(const char* s) 
{
    char* new_str = malloc(strlen(s) + 1);
    if (new_str) 
        strcpy(new_str, s);

    return new_str;
}

int main() 
{
    Map* map1 = map_create(compare_strings, string_deallocator, string_deallocator);
    map_insert(map1, my_strdup("apple"), my_strdup("red"));
    map_insert(map1, my_strdup("banana"), my_strdup("yellow"));

    Map* map2 = map_create(compare_strings, string_deallocator, string_deallocator);
    map_insert(map2, my_strdup("cherry"), my_strdup("red"));
    map_insert(map2, my_strdup("grape"), my_strdup("green"));

    printf("Size of Map1 before swap: %zu\n", map_size(map1));


    printf("Map1 before swap:\n");
    for (MapIterator it = map_begin(map1); it.node != map_end(map1).node; map_iterator_increment(&it)) 
    {
        MapNode* node = it.node;
        printf("%s: %s\n", (char*)node->key, (char*)node->value);
    }

    map_swap(map1, map2);

    printf("Map1 after swap:\n");
    for (MapIterator it = map_begin(map1); it.node != map_end(map1).node; map_iterator_increment(&it)) 
    {
        MapNode* node = it.node;
        printf("%s: %s\n", (char*)node->key, (char*)node->value);
    }

    printf("Before do-while decrement iterator\n");
    MapIterator it = map_rbegin(map2);

    // Iterate backwards through the map
    do {
        MapNode* node = it.node;
        printf("%s: %s\n", (char*)node->key, (char*)node->value);        
        map_iterator_decrement(&it);// Move to the previous element

    } while (it.node != NULL);

    map_deallocate(map1);
    map_deallocate(map2);

    return 0;
}

```

### Example 7 : lower and upper bound 

```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>

int compare_doubles(const KeyType a, const KeyType b) 
{
    const double* da = (const double*)a;
    const double* db = (const double*)b;

    if (*da < *db) 
        return -1;
    else if (*da > *db) 
        return 1;
    else 
        return 0;
}

void double_deallocator(void* data) {
    free(data);
}

int main() 
{
    Map* myMap = map_create(compare_doubles, double_deallocator, double_deallocator);

    // Insert elements
    double keys[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    for (int i = 0; i < 5; ++i) 
    {
        double* key = malloc(sizeof(double));
        double* value = malloc(sizeof(double));
        *key = keys[i];
        *value = keys[i] * 10;
        
        map_insert(myMap, key, value);
    }

    // Find lower and upper bounds
    double queryKey = 3.0;
    MapIterator lower = map_lower_bound(myMap, &queryKey);
    MapIterator upper = map_upper_bound(myMap, &queryKey);

    printf("Lower bound for %f: %f\n", queryKey, *((double*)(lower.node)->key));
    printf("Upper bound for %f: %f\n", queryKey, *((double*)(upper.node)->key));

    map_deallocate(myMap);
    return 0;
}
```

### Example 8 : map_node_get_key , and map_node_get_value 

```c
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>

int compare_doubles(const KeyType a, const KeyType b) 
{
    const double* da = (const double*)a;
    const double* db = (const double*)b;

    if (*da < *db) 
        return -1;
    else if (*da > *db) 
        return 1;
    else 
        return 0;
}

void double_deallocator(void* data) {
    free(data);
}

int main() 
{
    Map* myMap = map_create(compare_doubles, double_deallocator, double_deallocator);

    // Insert elements
    double keys[] = {1.1, 2.2, 3.3, 4.4, 5.5};
    for (int i = 0; i < 5; ++i)
    {
        double* key = malloc(sizeof(double));
        double* value = malloc(sizeof(double));
        *key = keys[i];
        *value = keys[i] * 10;
        map_insert(myMap, key, value);
    }

    // Iterate over the map and print keys and values
    for (MapIterator it = map_begin(myMap); it.node != map_end(myMap).node; map_iterator_increment(&it)) 
    {
        double* key = (double*)map_node_get_key(it.node);
        double* value = (double*)map_node_get_value(it.node);
        
        if (key && value) 
            printf("%f: %f\n", *key, *value);
    }

    map_deallocate(myMap);
    return 0;
}

```

### Example 9 : Use String Obj in Map 
```c
#include "map/map.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Compare function for String keys
int compare_strings(const KeyType a, const KeyType b) 
{
    String* strA = *(String**)a;
    String* strB = *(String**)b;

    return strcmp(string_c_str(strA), string_c_str(strB));
}

void string_deallocator(void* data) 
{
    if (data != NULL) 
    {
        String** strPtr = (String**)data;
        if (*strPtr != NULL) 
        {
            string_deallocate(*strPtr);
            *strPtr = NULL;  // Set the pointer to NULL after deallocating
        }
    }
}

int main() 
{
    // Create a map with String keys and String values
    Map* myMap = map_create(compare_strings, string_deallocator, string_deallocator);

    // Insert elements into the map
    String* key1 = string_create("key1");
    String* value1 = string_create("Hello");
    String* key2 = string_create("key2");
    String* value2 = string_create("World");

    map_insert(myMap, &key1, &value1);
    map_insert(myMap, &key2, &value2);

    printf("Map contents:\n");
    for (MapIterator it = map_begin(myMap); it.node != map_end(myMap).node; map_iterator_increment(&it)) 
    {
        String* key = *(String**)map_node_get_key(it.node);
        String* value = *(String**)map_node_get_value(it.node);
        printf("%s: %s\n", string_c_str(key), string_c_str(value));
    }

    if (map_erase(myMap, &key2)) 
        printf("Removed 'key2' from the map.\n");
    else 
        printf("Key 'key2' not found for removal.\n");
    
    if (map_erase(myMap, &key2)) 
        printf("Removed 'key2' from the map.\n");
    else 
        printf("Key 'key2' not found for removal.\n");
    
    // Search for a key
    String* searchKey = string_create("key1");
    String** foundValue = map_at(myMap, &searchKey);

    if (foundValue) 
        printf("Found value for 'key1': %s\n", string_c_str(*foundValue));
    else 
        printf("Key 'key1' not found.\n");
    
    // Cleanup
    string_deallocate(searchKey);
    map_deallocate(myMap);

    return 0;
}

```