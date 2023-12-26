#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define the Red-Black Tree color constants
#define RED 1
#define BLACK 0

// MapNode structure definition
struct MapNode 
{
    void* key;
    void* value;
    struct MapNode* left;
    struct MapNode* right;
    struct MapNode* parent;
    int color;
};

// Map structure definition
struct Map 
{
    MapNode* root;
    CompareFunc compFunc;
    ValueDeallocFunc deallocKey;
    ValueDeallocFunc deallocValue;
    size_t size;
};

static void map_free_nodes(MapNode* node, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue) 
{
    if (node == NULL) 
        return;

    // Recursively free left and right subtrees
    map_free_nodes(node->left, deallocKey, deallocValue);
    map_free_nodes(node->right, deallocKey, deallocValue);

    // Deallocate the key and value if deallocation functions are provided
    if (deallocKey) 
        deallocKey(node->key);

    if (deallocValue) 
        deallocValue(node->value);
    
    free(node); // Finally, free the node itself
}

Map* map_create(CompareFunc comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue) 
{
    Map* map = (Map*)malloc(sizeof(Map));

    if (!map) 
        return NULL;

    map->root = NULL;
    map->compFunc = comp;
    map->deallocKey = deallocKey;
    map->deallocValue = deallocValue;
    map->size = 0;

    return map;
}

void map_deallocate(Map* map) 
{
    if (!map) 
        return;

    map_free_nodes(map->root, map->deallocKey, map->deallocValue); // Use map_free_nodes to recursively free all nodes

    free(map);
}
