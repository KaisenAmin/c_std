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

    // Display all elements in the map
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
