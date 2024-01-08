/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Map
*/

#ifndef MAP_H_
#define MAP_H_

#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct Map Map;
typedef struct MapNode MapNode;

// Generic key and value types
typedef void* KeyType;
typedef void* ValueType;

// Function pointer types for comparison, deallocation, and iteration
typedef int (*CompareFunc)(const KeyType, const KeyType);
typedef void (*ValueDeallocFunc)(void*);

struct MapNode 
{
    void* key;
    void* value;
    struct MapNode* left;
    struct MapNode* right;
    struct MapNode* parent;
    int color;
};

// Map entry structure
typedef struct MapEntry 
{
    KeyType key;
    ValueType value;

} MapEntry;

// Example definition of MapIterator
typedef struct MapIterator 
{
    MapNode* node;
    // Implementation details, e.g., pointers to map nodes
} MapIterator;

typedef struct MapIteratorPair 
{
    MapIterator first;
    MapIterator second;
    
} MapIteratorPair;

// Function prototypes for the map
Map* map_create(CompareFunc comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue);
void map_deallocate(Map* map);

// Iterators
MapIterator map_begin(const Map* map);
MapIterator map_end(const Map* map);
MapIterator map_rbegin(const Map* map);
MapIterator map_rend(const Map* map);
MapIterator map_cbegin(const Map* map);
MapIterator map_cend(const Map* map);
MapIterator map_crbegin(const Map* map);
MapIterator map_crend(const Map* map);

// Capacity
bool map_empty(const Map* map);
size_t map_size(const Map* map);
size_t map_max_size(const Map* map);

// Element access
ValueType map_at(const Map* map, KeyType key);

// Modifiers
bool map_insert(Map* map, KeyType key, ValueType value);
bool map_erase(Map* map, KeyType key);
void map_swap(Map* map1, Map* map2);
void map_clear(Map* map);
bool map_emplace(Map* map, KeyType key, ValueType value);
bool map_emplace_hint(Map* map, MapIterator hint, KeyType key, ValueType value);

// Observers
CompareFunc map_key_comp(const Map* map);

// Operations
MapIterator map_find(const Map* map, KeyType key);
size_t map_count(const Map* map, KeyType key);
MapIterator map_lower_bound(const Map* map, KeyType key);
MapIterator map_upper_bound(const Map* map, KeyType key);
MapIteratorPair map_equal_range(const Map* map, KeyType key);
KeyType map_node_get_key(MapNode* node);
ValueType map_node_get_value(MapNode* node);
void map_iterator_increment(MapIterator* it);
void map_iterator_decrement(MapIterator* it);

void map_print(const Map* map, void (*printKey)(const KeyType), void (*printValue)(const ValueType));
Map* map_copy(const Map* src);

#endif // MAP_H_
