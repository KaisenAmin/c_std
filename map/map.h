/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Map
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in map.c (file-level convention).
 *
 * Red-black tree based ordered map, modeled on C++ std::map<K, V>.
 */

#ifndef MAP_H_
#define MAP_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define MAP_LOGGING_ENABLE */

#ifdef MAP_LOGGING_ENABLE
    #define MAP_LOG(fmt, ...) \
        do { fprintf(stderr, "[MAP LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define MAP_LOG(...) do { } while (0)
#endif


typedef struct Map     Map;
typedef struct MapNode MapNode;

/* Generic key / value types — Maps store opaque pointers. */
typedef void* KeyType;
typedef void* ValueType;

/* Comparator + deallocator callbacks. */
typedef int  (*CompareFuncMap)  (const KeyType, const KeyType);
typedef void (*ValueDeallocFunc)(void*);

struct MapNode {
    void*           key;
    void*           value;
    struct MapNode* left;
    struct MapNode* right;
    struct MapNode* parent;
    int             color;
};


typedef struct MapEntry {
    KeyType   key;
    ValueType value;
} MapEntry;


typedef struct MapIterator {
    MapNode* node;
} MapIterator;


typedef struct MapIteratorPair {
    MapIterator first;
    MapIterator second;
} MapIteratorPair;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Map*            map_create                  (CompareFuncMap comp,
                                             ValueDeallocFunc deallocKey,
                                             ValueDeallocFunc deallocValue);
Map*            map_copy                    (const Map* src);


/* ------------------------------------------------------------------ */
/* Destruction                                                        */
/* ------------------------------------------------------------------ */

void            map_deallocate              (Map* map);
void            map_clear                   (Map* map);
void            map_swap                    (Map* map1, Map* map2);


/* ------------------------------------------------------------------ */
/* Capacity                                                           */
/* ------------------------------------------------------------------ */

size_t          map_size                    (const Map* map);
size_t          map_max_size                (const Map* map);
size_t          map_count                   (const Map* map, KeyType key);
bool            map_empty                   (const Map* map);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

ValueType       map_at                      (const Map* map, KeyType key);
KeyType         map_node_get_key            (MapNode* node);
ValueType       map_node_get_value          (MapNode* node);


/* ------------------------------------------------------------------ */
/* Iterators — mutating                                               */
/* ------------------------------------------------------------------ */

MapIterator     map_begin                   (const Map* map);
MapIterator     map_end                     (const Map* map);
MapIterator     map_rbegin                  (const Map* map);
MapIterator     map_rend                    (const Map* map);


/* ------------------------------------------------------------------ */
/* Iterators — const                                                  */
/* ------------------------------------------------------------------ */

MapIterator     map_cbegin                  (const Map* map);
MapIterator     map_cend                    (const Map* map);
MapIterator     map_crbegin                 (const Map* map);
MapIterator     map_crend                   (const Map* map);


/* ------------------------------------------------------------------ */
/* Iterator advance                                                   */
/* ------------------------------------------------------------------ */

void            map_iterator_increment      (MapIterator* it);
void            map_iterator_decrement      (MapIterator* it);


/* ------------------------------------------------------------------ */
/* Lookup                                                             */
/* ------------------------------------------------------------------ */

MapIterator     map_find                    (const Map* map, KeyType key);
MapIterator     map_lower_bound             (const Map* map, KeyType key);
MapIterator     map_upper_bound             (const Map* map, KeyType key);
MapIteratorPair map_equal_range             (const Map* map, KeyType key);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

bool            map_insert                  (Map* map, KeyType key, ValueType value);
bool            map_emplace                 (Map* map, KeyType key, ValueType value);
bool            map_emplace_hint            (Map* map, MapIterator hint, KeyType key, ValueType value);
bool            map_erase                   (Map* map, KeyType key);


/* ------------------------------------------------------------------ */
/* Observers                                                          */
/* ------------------------------------------------------------------ */

CompareFuncMap  map_key_comp                (const Map* map);


/* ------------------------------------------------------------------ */
/* Debug                                                              */
/* ------------------------------------------------------------------ */

void            map_print                   (const Map* map,
                                             void (*printKey)(const KeyType),
                                             void (*printValue)(const ValueType));


#ifdef __cplusplus
}
#endif

#endif 
