/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Map
*/

#include "map.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

// Define the Red-Black Tree color constants
#define RED 1
#define BLACK 0

// Map structure definition
struct Map {
    MapNode* root;
    CompareFuncMap compFunc;
    ValueDeallocFunc deallocKey;
    ValueDeallocFunc deallocValue;
    size_t size;
};


/**
 * @brief Advances the iterator to the next element in the map.
 *
 * This function increments the iterator to point to the next element in the map
 * according to the in-order traversal of the underlying Red-Black Tree. If the current
 * node has a right child, the iterator moves to the leftmost node in the right subtree.
 * Otherwise, it moves up the tree to find the first ancestor that is a left child of its parent.
 *
 * @param it A pointer to the MapIterator to be incremented. If the iterator is NULL or
 * the node it points to is NULL, the function will log an error and return.
 */
void map_iterator_increment(MapIterator* it) {
    if (it == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null iterator provided in map_iterator_increment.\n");
        #endif 
        return;
    }
    if (it->node == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Iterator's node is null in map_iterator_increment.\n");
        #endif 
        return;
    }
    // If the right subtree is not empty, the successor is the leftmost node in the right subtree
    if (it->node->right != NULL) {
        it->node = it->node->right;
        
        while (it->node->left != NULL) {
            it->node = it->node->left;
        }
    } 
    else {
        // Go up the tree to find the first ancestor that is a left child of its parent
        while (it->node->parent != NULL && it->node == it->node->parent->right) { 
            it->node = it->node->parent;
        }
        it->node = it->node->parent;
    }
}

/**
 * @brief Moves the iterator to the previous element in the map.
 *
 * This function decrements the iterator to point to the previous element in the map
 * according to the in-order traversal of the underlying Red-Black Tree. If the current
 * node has a left child, the iterator moves to the rightmost node in the left subtree.
 * Otherwise, it moves up the tree to find the first ancestor that is a right child of its parent.
 *
 * @param it A pointer to the MapIterator to be decremented. If the iterator is NULL or
 * the node it points to is NULL, the function will log an error and return.
 */
void map_iterator_decrement(MapIterator* it) {
    if (it == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null iterator provided in map_iterator_decrement.\n");
        #endif 
        return;
    }
    if (it->node == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Iterator's node is null in map_iterator_decrement.\n");
        #endif 
        return;
    }
    // If the left subtree is not empty, the predecessor is the rightmost node in the left subtree
    if (it->node->left != NULL) {
        it->node = it->node->left;
        while (it->node->right != NULL) { 
            it->node = it->node->right;
        }
    } 
    else {
        // Go up the tree to find the first ancestor that is a right child of its parent
        while (it->node->parent != NULL && it->node == it->node->parent->left) { 
            it->node = it->node->parent;
        }
        it->node = it->node->parent;
    }
}

/**
 * @brief Creates a new MapNode with the given key and value.
 *
 * This function allocates memory for a new MapNode, initializes its key and value,
 * and sets its color to RED, as required by Red-Black Tree properties.
 *
 * @param key The key to be stored in the new node.
 * @param value The value to be associated with the key in the new node.
 * @return A pointer to the newly created MapNode, or NULL if memory allocation fails.
 */
static MapNode* create_node(KeyType key, ValueType value) {
    MapNode* node = (MapNode*)malloc(sizeof(MapNode));
    
    if (!node) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Cannot allocate memory for node in create_node.\n");
        #endif 
        return NULL;
    }
    node->key = key;
    node->value = value;
    node->left = node->right = node->parent = NULL;
    node->color = RED;  // All new nodes are red in a Red-Black Tree
    
    return node;
}

/**
 * @brief Performs a left rotation on the given node in the map.
 *
 * This function rotates the subtree rooted at node `x` to the left, ensuring
 * the Red-Black Tree properties are maintained. The left rotation involves
 * making the right child of `x` the new root of the subtree, and `x` becomes
 * the left child of its former right child.
 *
 * @param map A pointer to the map structure.
 * @param x A pointer to the node on which to perform the left rotation.
 */
static void map_left_rotate(Map* map, MapNode* x) {
    if (map == NULL || x == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided to map_left_rotate.\n");
        #endif 
        return;
    }
    MapNode* y = x->right;
    if (y == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Right child is null in map_left_rotate.\n");
        #endif 
        return;
    }

    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    
    if (x->parent == NULL) { 
        map->root = y;
    }
    else if (x == x->parent->left) { 
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

/**
 * @brief Performs a right rotation on the given node in the map.
 *
 * This function rotates the subtree rooted at node `y` to the right, ensuring
 * the Red-Black Tree properties are maintained. The right rotation involves
 * making the left child of `y` the new root of the subtree, and `y` becomes
 * the right child of its former left child.
 *
 * @param map A pointer to the map structure.
 * @param y A pointer to the node on which to perform the right rotation.
 */
static void map_right_rotate(Map* map, MapNode* y) {
    if (map == NULL || y == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided to map_right_rotate.\n");
        #endif 
        return;
    }
    MapNode* x = y->left;
    if (x == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Left child is null in map_right_rotate.\n");
        #endif 
        return;
    }

    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == NULL) { 
        map->root = x;
    }
    else if (y == y->parent->right) { 
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

/**
 * @brief Replaces one subtree as a child of its parent with another subtree.
 *
 * This function is used during the deletion of a node in a Red-Black Tree.
 * It replaces the subtree rooted at `u` with the subtree rooted at `v`.
 * If `u` is the root of the tree, `v` becomes the new root.
 *
 * @param map A pointer to the map structure.
 * @param u A pointer to the node to be replaced.
 * @param v A pointer to the node to replace `u`.
 */
static void map_transplant(Map* map, MapNode* u, MapNode* v) {
    if (map == NULL || u == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided to map_transplant.\n");
        #endif 
        return;
    }
    
    if (u->parent == NULL) {
        map->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != NULL) { 
        v->parent = u->parent;
    }
}

/**
 * @brief Finds the node with the minimum key in the subtree.
 *
 * This function traverses the left children of the given node
 * until it reaches the leftmost (smallest) node in the subtree.
 *
 * @param node A pointer to the root of the subtree.
 * @return A pointer to the node with the minimum key in the subtree.
 */
static MapNode* map_minimum(MapNode* node) {
    if (node == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided to map_minimum.\n");
        #endif 
        return NULL;
    }
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

/**
 * @brief Restores Red-Black Tree properties after a node deletion.
 *
 * This function is called after a node has been removed from the Red-Black Tree.
 * It ensures that the tree remains balanced and that all Red-Black properties are preserved.
 * The function performs rotations and recoloring as necessary to fix any violations.
 *
 * @param map A pointer to the map structure.
 * @param x A pointer to the node that replaces the deleted node, which may cause a violation of Red-Black Tree properties.
 */
static void map_erase_fixup(Map* map, MapNode* x) {
    while (x != map->root && (x == NULL || x->color == BLACK)) {
        if (x == x->parent->left) {
      
            MapNode* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                map_left_rotate(map, x->parent);
                w = x->parent->right;
            }
            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } 
            else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left != NULL) {
                        w->left->color = BLACK;
                    }
                    w->color = RED;
                    map_right_rotate(map, w);
                    w = x->parent->right;
                }
                
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right != NULL) { 
                    w->right->color = BLACK;
                }
                map_left_rotate(map, x->parent);
                x = map->root;
            }
        } 
        else {
            MapNode* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                map_right_rotate(map, x->parent);
                w = x->parent->left;
            }
            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } 
            else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right != NULL) {
                        w->right->color = BLACK;
                    }
                    w->color = RED;
                    map_left_rotate(map, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->left != NULL) { 
                    w->left->color = BLACK;
                }
                map_right_rotate(map, x->parent);
                x = map->root;
            }
        }
    }
    if (x != NULL) { 
        x->color = BLACK;
    }
}

/**
 * @brief Recursively frees all nodes in the subtree rooted at the given node.
 *
 * This function performs a post-order traversal of the tree,
 * freeing each node and its associated key and value, using the provided deallocation functions.
 *
 * @param node A pointer to the root of the subtree to be freed.
 * @param deallocKey A function pointer for deallocating the key associated with each node.
 * @param deallocValue A function pointer for deallocating the value associated with each node.
 */
static void map_free_nodes(MapNode* node, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue) {
    if (node == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: node param is null and invalid in map_free_nodes\n");
        #endif 
        return;
    }
    // Recursively free left and right subtrees
    map_free_nodes(node->left, deallocKey, deallocValue);
    map_free_nodes(node->right, deallocKey, deallocValue);

    // Deallocate the key and value if deallocation functions are provided
    if (deallocKey) { 
        deallocKey(node->key);
    }
    if (deallocValue) {
        deallocValue(node->value);
    }
    free(node); // Finally, free the node itself
}

/**
 * @brief Restores Red-Black Tree properties after a node insertion.
 *
 * This function ensures that the Red-Black Tree properties are maintained after inserting a new node.
 * It performs rotations and recoloring as necessary to restore balance to the tree.
 *
 * @param map A pointer to the map structure.
 * @param newNode A pointer to the newly inserted node that may cause a violation of the Red-Black Tree properties.
 */
static void map_insert_fixup(Map* map, MapNode* newNode) {
    if (map == NULL || newNode == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided to map_insert_fixup.\n");
        #endif 
        return;
    }
    // Example fix-up logic, doesn't cover all cases.
    while (newNode != map->root && newNode->parent->color == RED) {
        // Determine if parent is the left or right child of the grandparent
        if (newNode->parent == newNode->parent->parent->left) {
            MapNode* uncle = newNode->parent->parent->right;
            if (uncle && uncle->color == RED) {
                // Case 1: Parent and Uncle are red
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent;
            } 
            else {
                // Cases 2 and 3: Uncle is black
                if (newNode == newNode->parent->right) { 
                    newNode = newNode->parent;  // Convert to linear configuration
                    // Perform left rotation
                }
                // Perform right rotation
                // Update colors
                break; // Exit loop
            }
        } 
        else {
            // Symmetric to the above case
            MapNode* uncle = newNode->parent->parent->left;

            if (uncle && uncle->color == RED) {
                // Case 1: Parent and Uncle are red
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent;
            } 
            else {
                // Cases 2 and 3: Uncle is black
                if (newNode == newNode->parent->left) {
                    // Convert to linear configuration
                    newNode = newNode->parent;
                    map_right_rotate(map, newNode);
                }
                // Perform left rotation
                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                map_left_rotate(map, newNode->parent->parent);

                break; // Exit loop
            }
        }
    }
    map->root->color = BLACK;
}

/**
 * @brief Creates a new map with the specified comparison and deallocation functions.
 *
 * This function initializes a new map with the provided comparison function for keys
 * and optional deallocation functions for keys and values. The map is implemented as
 * a Red-Black Tree, ensuring balanced insertion, deletion, and lookup operations.
 *
 * @param comp A comparison function used to order the keys in the map. This function must not be null.
 * @param deallocKey A function to deallocate the keys when a node is deleted, or NULL if no deallocation is needed.
 * @param deallocValue A function to deallocate the values when a node is deleted, or NULL if no deallocation is needed.
 * 
 * @return A pointer to the newly created map. If memory allocation fails or the comparison function is null, 
 * the function will print an error message and terminate the program.
 */
Map* map_create(CompareFuncMap comp, ValueDeallocFunc deallocKey, ValueDeallocFunc deallocValue) {
    if (!comp) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Compare function is null, cannot create map.\n");
        #endif 
        exit(-1);
    }

    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Cannot allocate memory for map.\n");
        #endif 
        exit(-1);
    }

    map->root = NULL;
    map->compFunc = comp;
    map->deallocKey = deallocKey;
    map->deallocValue = deallocValue;
    map->size = 0;

    return map;
}

/**
 * @brief Deallocates the memory used by the map and its nodes.
 *
 * This function frees all the memory associated with the map, including all of its nodes, keys, and values.
 * After calling this function, the map pointer should no longer be used.
 *
 * @param map A pointer to the Map to be deallocated.
 */
void map_deallocate(Map* map) {
    if (!map){
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Warning: Map is null or empty there is nothing to deallocate in map_deallocate.\n");
        #endif 
        return;
    }
    map_free_nodes(map->root, map->deallocKey, map->deallocValue); // Use map_free_nodes to recursively free all nodes
    free(map);
}

/**
 * @brief Checks if the map is empty.
 *
 * This function returns true if the map contains no elements, or if the map pointer is null.
 *
 * @param map A pointer to the Map to be checked.
 * @return true if the map is empty or null, false otherwise.
 */
bool map_empty(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_empty.\n");
        #endif 
        return true; // Consider an empty map for null pointer
    }
    return map->size == 0;
}

/**
 * @brief Returns the number of elements in the map.
 *
 * This function returns the number of key-value pairs currently stored in the map.
 * If the map pointer is null, the function returns 0.
 *
 * @param map A pointer to the Map whose size is to be determined.
 * @return The number of elements in the map, or 0 if the map is null.
 */
size_t map_size(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_size.\n");
        #endif 
        return 0; // Return 0 for null pointer
    }
    return map->size;
}

/**
 * @brief Returns the maximum possible number of elements in the map.
 *
 * This function returns the maximum number of elements that can be held in the map.
 * The maximum size is determined based on the size of the MapNode structure.
 *
 * @param map A pointer to the Map whose maximum size is to be determined.
 * @return The maximum number of elements that the map can hold, or 0 if the map pointer is null.
 */
size_t map_max_size(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_max_size.\n");
        #endif 
        return 0; // Return 0 for null pointer
    }
    return (size_t)(~((size_t)0)) / sizeof(MapNode);
}

/**
 * @brief Inserts a key-value pair into the map.
 *
 * This function inserts a new key-value pair into the map. If the key already exists, its associated value is replaced 
 * with the new value. If the key does not exist, a new node is created and inserted into the map. 
 * The function returns true on successful insertion or replacement.
 *
 * @param map A pointer to the Map where the key-value pair will be inserted.
 * @param key The key to be inserted into the map.
 * @param value The value to be associated with the key.
 * 
 * @return true if the key-value pair was successfully inserted or replaced, 
 * false if the map or key is null or if node creation fails.
 */
bool map_insert(Map* map, KeyType key, ValueType value) {
    if (map == NULL || key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map or key in map_insert.\n");
        #endif 
        return false;
    }
    MapNode** curr = &map->root;
    MapNode* parent = NULL;

    while (*curr) {
        parent = *curr;
        int cmp = map->compFunc(key, (*curr)->key);
        
        if (cmp == 0) {
            // Key already exists, replace its value
            if (map->deallocValue) {
                map->deallocValue((*curr)->value);
            }
            (*curr)->value = value;
            return true;
        }
        if (cmp < 0) {
            curr = &(*curr)->left;
        } 
        else {
            curr = &(*curr)->right;
        }
    }

    MapNode* newNode = create_node(key, value);
    if (!newNode) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Node creation failed in map_insert.\n");
        #endif 
        return false;
    }
    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode);
    return true;
}

/**
 * @brief Retrieves the value associated with the specified key in the map.
 *
 * This function searches for a key in the map and returns the associated value if the key is found.
 * If the key is not found, the function returns NULL.
 *
 * @param map A pointer to the Map to be searched.
 * @param key The key to search for in the map.
 * @return The value associated with the key, or NULL if the key is not found or if a null pointer is provided.
 */
ValueType map_at(const Map* map, KeyType key) {
    if (map == NULL || key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map or key in map_at.\n");
        #endif 
        return NULL; // Return NULL for null pointer
    }
    MapNode* curr = map->root;
    
    while (curr) {
        int cmp = map->compFunc(key, curr->key);

        if (cmp == 0) { 
            return curr->value;
        }
        if (cmp < 0) { 
            curr = curr->left;
        }
        else { 
            curr = curr->right;
        }
    }
    return NULL;  // Key not found
}

/**
 * @brief Clears all elements from the map.
 *
 * This function removes all elements from the map, freeing any associated memory. After calling this
 * function, the map will be empty.
 *
 * @param map A pointer to the Map to be cleared.
 */
void map_clear(Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_clear.\n");
        #endif 
        return;
    }
    map_free_nodes(map->root, map->deallocKey, map->deallocValue);
    map->root = NULL;
    map->size = 0;
}

/**
 * @brief Swaps the contents of two maps.
 *
 * This function exchanges the contents of two maps, including their root nodes, sizes, comparison functions,
 * and deallocation functions. After calling this function, the two maps will effectively have swapped their
 * entire state.
 *
 * @param map1 A pointer to the first Map to be swapped.
 * @param map2 A pointer to the second Map to be swapped.
 */
void map_swap(Map* map1, Map* map2) {
    if (map1 == NULL || map2 == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for one or both maps in map_swap.\n");
        #endif 
        return;
    }
    // Swap the roots of the two maps
    MapNode* tempRoot = map1->root;
    map1->root = map2->root;
    map2->root = tempRoot;

    // Swap sizes
    size_t tempSize = map1->size;
    map1->size = map2->size;
    map2->size = tempSize;

    // Optionally, swap comparison and deallocation functions
    CompareFuncMap tempCompFunc = map1->compFunc;
    map1->compFunc = map2->compFunc;
    map2->compFunc = tempCompFunc;

    ValueDeallocFunc tempDeallocKey = map1->deallocKey;
    map1->deallocKey = map2->deallocKey;
    map2->deallocKey = tempDeallocKey;

    ValueDeallocFunc tempDeallocValue = map1->deallocValue;
    map1->deallocValue = map2->deallocValue;
    map2->deallocValue = tempDeallocValue;
}

/**
 * @brief Counts the number of elements matching a specific key in the map.
 *
 * This function searches for the specified key in the map and returns the number of elements matching the key.
 * Since a map does not allow duplicate keys, the function will return either 0 (if the key is not found) or 1 (if the key is found).
 *
 * @param map A pointer to the Map to be searched.
 * @param key The key to search for in the map.
 * 
 * @return The number of elements matching the key (0 or 1), or 0 if a null pointer is provided.
 */
size_t map_count(const Map* map, KeyType key) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_count.\n");
        #endif 
        return 0;
    }
    MapNode* current = map->root;
    
    while (current != NULL) {
        int cmp = map->compFunc(key, current->key);
        if (cmp == 0) {
            return 1; // Key found
        }
        if (cmp < 0) { 
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return 0; // Key not found
}

/**
 * @brief Inserts a new key-value pair into the map if the key does not already exist.
 *
 * This function attempts to insert a new key-value pair into the map. If the key already exists,
 * the function returns false without modifying the map. If the insertion is successful, the function
 * returns true, and the Red-Black Tree is rebalanced if necessary.
 *
 * @param map A pointer to the Map where the key-value pair should be inserted.
 * @param key The key to be inserted.
 * @param value The value to be associated with the key.
 * 
 * @return true if the key-value pair was successfully inserted, false if the key already exists or 
 * if a null pointer is provided.
 */
bool map_emplace(Map* map, KeyType key, ValueType value) {
    if (map == NULL || key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map or key in map_emplace.\n");
        #endif 
        return false;
    }
    MapNode** curr = &map->root;
    MapNode* parent = NULL;

    while (*curr) {
        parent = *curr;
        int cmp = map->compFunc(key, (*curr)->key);
        
        if (cmp == 0) { 
            return false; // Key already exists
        }
        if (cmp < 0) { 
            curr = &(*curr)->left;
        }
        else { 
            curr = &(*curr)->right;
        }
    }
    MapNode* newNode = create_node(key, value);

    if (!newNode) {
        return false;
    }
    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode); // Perform necessary tree balancing
    
    return true;
}

/**
 * @brief Returns the comparison function used by the map.
 *
 * This function returns a pointer to the comparison function used by the map to order its elements.
 *
 * @param map A pointer to the Map whose comparison function is requested.
 * @return A pointer to the comparison function, or NULL if the map is NULL.
 */
CompareFuncMap map_key_comp(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_key_comp.\n");
        #endif 
        return NULL;
    }
    return map->compFunc;  // Return the comparison function pointer
}

/**
 * @brief Inserts a new key-value pair into the map with a hint for the insertion position.
 *
 * This function attempts to insert a new key-value pair into the map, using a hint to suggest the
 * insertion position. If the key already exists, the function returns false without modifying the map.
 * If the insertion is successful, the function returns true, and the Red-Black Tree is rebalanced if necessary.
 *
 * @param map A pointer to the Map where the key-value pair should be inserted.
 * @param hint A MapIterator that hints at the insertion position.
 * @param key The key to be inserted.
 * @param value The value to be associated with the key.
 * 
 * @return true if the key-value pair was successfully inserted, 
 * false if the key already exists or if a null pointer is provided.
 */
bool map_emplace_hint(Map* map, MapIterator hint, KeyType key, ValueType value) {
    if (map == NULL || !key) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: map or key is null cannot emplace in map_emplace_hint.\n");
        #endif 
        exit(-1);
    }
    MapNode* newNode = create_node(key, value);
    
    if (newNode == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Unable to crate new Node in map_emplace_hint.\n");
        #endif 
        return false;
    }

    // If the map is empty, just insert the node at the root.
    if (map->root == NULL) {
        map->root = newNode;
        map->root->color = BLACK; // Root must be black.
        map->size++;

        return true;
    }

    // If hint is provided and valid, attempt to use it.
    if (hint.node != NULL) {
        MapNode* curr = hint.node; // Replace dummy with actual hint implementation
        int cmp = map->compFunc(key, curr->key);

        if (cmp < 0) {
            if (curr->left == NULL) {
                curr->left = newNode;
                newNode->parent = curr;
                map->size++;
                map_insert_fixup(map, newNode);

                return true;
            }
        } 
        else if (cmp > 0) {
            if (curr->right == NULL) {
                curr->right = newNode;
                newNode->parent = curr;
                map->size++;
                map_insert_fixup(map, newNode);

                return true;
            }
        } 
        else {
            // Key already exists, deallocate newNode and return false.
            free(newNode);
            return false;
        }
    }

    // If hint is not useful, fall back to regular insert.
    MapNode** curr = &map->root;
    MapNode* parent = NULL;

    while (*curr) {
        parent = *curr;
        int cmp = map->compFunc(key, (*curr)->key);

        if (cmp == 0) {
            // Key already exists, deallocate newNode and return false.
            free(newNode);
            return false;
        }

        if (cmp < 0) {
            curr = &(*curr)->left;
        }
        else { 
            curr = &(*curr)->right;
        }
    }

    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode);

    return true;
}

/**
 * @brief Erases an element from the map by its key.
 *
 * This function removes the element with the specified key from the map. If the key is not found,
 * the function returns false. It also handles the necessary re-balancing of the Red-Black Tree
 * after the removal to maintain its properties.
 *
 * @param map A pointer to the Map from which the element should be erased.
 * @param key The key of the element to be removed.
 * @return true if the element was successfully erased, false if the key was not found or the map was NULL.
 */
bool map_erase(Map* map, KeyType key) {
    if (map == NULL || map->root == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: map or map root is null in map_erase.\n");
        #endif 
        return false;
    }
    
    MapNode* z = map->root;
    while (z != NULL) {
        int cmp = map->compFunc(key, z->key);
        if (cmp == 0) {
            break; // Key found
        }
        if (cmp < 0) { 
            z = z->left;
        }
        else {
            z = z->right;
        }
    }

    if (z == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Key not found in map_erase.\n");
        #endif 
        return false;
    }
    
    MapNode* y = z;
    int y_original_color = y->color;
    MapNode* x;

    if (z->left == NULL) {
        x = z->right;
        map_transplant(map, z, z->right);
    } 
    else if (z->right == NULL) {
        x = z->left;
        map_transplant(map, z, z->left);
    } 
    else {
        y = map_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        
        if (y->parent == z) {
            if (x != NULL) {
                x->parent = y;
            }
        } 
        else {
            map_transplant(map, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        map_transplant(map, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (map->deallocKey) {
        map->deallocKey(z->key);
    }
    if (map->deallocValue) {
        map->deallocValue(z->value);
    }
    free(z);

    if (y_original_color == BLACK) { 
        map_erase_fixup(map, x);
    }
    map->size--;
    
    return true;
}

/**
 * @brief Finds an element in the map by its key.
 *
 * This function searches for an element with the specified key in the map.
 * If the key is found, it returns an iterator pointing to the corresponding element.
 * If the key is not found, it returns an iterator representing the end of the map.
 *
 * @param map A pointer to the Map to be searched.
 * @param key The key of the element to find.
 * @return A MapIterator pointing to the element if found, or to the end of the map if not found.
 */
MapIterator map_find(const Map* map, KeyType key) {
    MapIterator iterator = {0}; // Initialize iterator to represent the end or null
    if (map == NULL || key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map or key in map_find.\n");
        #endif 
        return iterator; // Return default iterator on error
    }

    MapNode* current = map->root;
    while (current != NULL) {
        int cmp = map->compFunc(key, current->key); // Compare the current node's key with the search key
        if (cmp == 0) {
            iterator.node = current; // Found the key, set iterator to point to the current node
            return iterator;
        }
        // Decide to move left or right in the tree based on comparison result
        current = cmp < 0 ? current->left : current->right;
    }
    return iterator; // Key not found, return end iterator
}

/**
 * @brief Returns an iterator to the first element in the map.
 *
 * This function returns an iterator pointing to the first element in the map,
 * which is the leftmost node in the Red-Black Tree. If the map is empty, it returns an end iterator.
 *
 * @param map A pointer to the Map whose beginning is requested.
 * @return A MapIterator pointing to the first element in the map, or a default iterator if the map is NULL.
 */
MapIterator map_begin(const Map* map) {
    MapIterator iterator = {0};  // Initialize to default, adjust as per your MapIterator structure.
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_begin.\n");
        #endif 
        return iterator; // Return default iterator on error
    }
    // The first element is the leftmost node in the tree.
    MapNode* current = map->root;
    while (current->left != NULL) {
        current = current->left;
    }

    iterator.node =current;  // Set iterator to point to the leftmost node.
    return iterator;
}

/**
 * @brief Returns an iterator to the position after the last element in the map.
 *
 * This function returns an iterator representing the end of the map. The end iterator
 * does not point to any element; it is used to denote the position after the last element in the map.
 *
 * @param map A pointer to the Map whose end iterator is requested.
 * @return A MapIterator representing the end of the map, or a default iterator if the map is NULL.
 */
MapIterator map_end(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_end.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default 'end' iterator
}

/**
 * @brief Returns a reverse iterator to the last element of the map.
 *
 * This function is used to obtain an iterator representing the last element in the map,
 * allowing traversal of the map in reverse order.
 *
 * @param map A pointer to the Map whose reverse begin iterator is requested.
 * @return A MapIterator pointing to the last element in the map, or a default iterator if the map is NULL.
 */
MapIterator map_rbegin(const Map* map) {
    MapIterator iterator = {0};
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_rbegin.\n");
        #endif 
        return iterator; // Return default iterator on error
    }

    MapNode* current = map->root; // The rightmost node is the largest element in the map
    
    while (current->right != NULL) { 
        current = current->right;
    }
    iterator.node = current;  // Replace with actual assignment to iterator
    
    return iterator;
}

/**
 * @brief Returns a reverse iterator to the position before the first element of the map.
 *
 * This function is used to obtain an iterator representing the end of the reverse sequence of elements
 * in the map. It effectively points to the position before the first element when iterating in reverse.
 *
 * @param map A pointer to the Map whose reverse end iterator is requested.
 * @return A MapIterator pointing to the position before the first element in reverse, 
 * or a default iterator if the map is NULL.
 */
MapIterator map_rend(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_rend.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default 'end' iterator
}

/**
 * @brief Returns a constant iterator to the first element of the map.
 *
 * This function is used to obtain a constant iterator representing the first element in the map,
 * allowing read-only access to the elements in forward order.
 *
 * @param map A pointer to the Map whose constant begin iterator is requested.
 * @return A MapIterator pointing to the first element in the map, or a default iterator if the map is NULL.
 */
MapIterator map_cbegin(const Map* map) {
    MapIterator iterator = {0};
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_cbegin.\n");
        #endif 
        return iterator; // Return default iterator on error
    }

    if (map != NULL && map->root != NULL) {
        MapNode* current = map->root;
        
        while (current->left != NULL) {
            current = current->left;
        }
        iterator.node = current;
    }
    return iterator;
}

/**
 * @brief Returns a constant iterator to the position after the last element of the map.
 *
 * This function is used to obtain a constant iterator representing the end of the map,
 * allowing read-only access to the elements. The iterator points to the position just after the last element.
 *
 * @param map A pointer to the Map whose constant end iterator is requested.
 * @return A MapIterator pointing to the position after the last element, or a default iterator if the map is NULL.
 */
MapIterator map_cend(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_cend.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default end iterator
}

/**
 * @brief Returns a constant reverse iterator to the last element of the map.
 *
 * This function is used to obtain a constant reverse iterator representing the last element in the map,
 * allowing read-only access to the elements in reverse order.
 *
 * @param map A pointer to the Map whose constant reverse begin iterator is requested.
 * @return A MapIterator pointing to the last element in the map, or a default iterator if the map is NULL.
 */
MapIterator map_crbegin(const Map* map) {
    MapIterator iterator = {0};
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_crbegin.\n");
        #endif 
        return iterator; // Return default iterator on error
    }

    if (map != NULL && map->root != NULL) {
        MapNode* current = map->root;
        
        while (current->right != NULL) {
            current = current->right;
        }
        iterator.node = current;
    }
    return iterator;
}

/**
 * @brief Returns a constant reverse iterator to the position before the first element of the map.
 *
 * This function is used to obtain an iterator representing the end of the reverse sequence of elements
 * in the map. It effectively points to the position before the first element when iterating in reverse.
 *
 * @param map A pointer to the Map whose constant reverse end iterator is requested.
 * @return A MapIterator pointing to the end of the reverse sequence, or a default iterator if the map is NULL.
 */
MapIterator map_crend(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_crend.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default end iterator
}

/**
 * @brief Finds the first element that is not less than the given key.
 *
 * This function searches the map for the first element whose key is not less than the specified key
 * and returns an iterator to it. If all elements are less than the specified key, the end iterator is returned.
 *
 * @param map A pointer to the Map to search.
 * @param key The key to compare the elements against.
 * 
 * @return A MapIterator pointing to the first element not less than the given key, 
 * or the end iterator if no such element is found.
 */
MapIterator map_lower_bound(const Map* map, KeyType key) {
    MapIterator iterator = {0}; // Initialize to default, representing the end.
    if (map == NULL || key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map or key in map_lower_bound.\n");
        #endif 
        return iterator; // Return default iterator on error
    }

    MapNode* current = map->root;
    MapNode* last = NULL;

    while (current != NULL) {
        if (map->compFunc(current->key, key) >= 0) {
            last = current;
            current = current->left;
        } 
        else {
            current = current->right;
        }
    }

    if (last != NULL) { 
        iterator.node = last;
    }
    return iterator;
}

/**
 * @brief Finds the first element that is greater than the given key.
 *
 * This function searches the map for the first element whose key is greater than the specified key
 * and returns an iterator to it. If all elements are less than or equal to the specified key, 
 * the end iterator is returned.
 *
 * @param map A pointer to the Map to search.
 * @param key The key to compare the elements against.
 * @return A MapIterator pointing to the first element greater than the given key, 
 * or the end iterator if no such element is found.
 */
MapIterator map_upper_bound(const Map* map, KeyType key) {
    MapIterator iterator = {0}; // Initialize to default, representing the end.
    if (map == NULL || key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map or key in map_upper_bound.\n");
        #endif 
        return iterator; // Return default iterator on error
    }

    MapNode* current = map->root;
    MapNode* last = NULL;

    while (current != NULL) {
        if (map->compFunc(current->key, key) > 0) {
            last = current;
            current = current->left;
        } 
        else {
            current = current->right;
        }
    }

    if (last != NULL) { 
        iterator.node = last;
    }
    return iterator;
}

/**
 * @brief Returns the range of elements that match the specified key.
 *
 * This function returns a pair of iterators representing the range of elements in the map that match the given key.
 * The first iterator points to the first element not less than the key, and the second 
 * iterator points to the first element greater than the key.
 *
 * @param map A pointer to the Map to search.
 * @param key The key to compare the elements against.
 * @return A MapIteratorPair containing two iterators: the first pointing to the lower bound and 
 * the second pointing to the upper bound of the key.
 */
MapIteratorPair map_equal_range(const Map* map, KeyType key) {
    MapIteratorPair iteratorPair = {{0}, {0}};

    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Map object is Null in map_equal_range.\n");
        #endif 
        return iteratorPair; // Return default iterator pair on error
    }
    if (key == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: KeyType value is Null in map_equal_range.\n");
        #endif 
        return iteratorPair; // Return default iterator pair on error
    }

    iteratorPair.first = map_lower_bound(map, key);
    iteratorPair.second = map_upper_bound(map, key);

    return iteratorPair;
}

/**
 * @brief Retrieves the key from the given map node.
 *
 * @param node A pointer to the MapNode from which to retrieve the key.
 * @return The key stored in the node, or NULL if the node is NULL.
 */
KeyType map_node_get_key(MapNode* node) {
    if (!node) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: MapNode object is Null and Invalid in map_node_get_key.\n");
        #endif 
        return NULL;
    }
    return node->key;
}

/**
 * @brief Retrieves the value from the given map node.
 *
 * @param node A pointer to the MapNode from which to retrieve the value.
 * @return The value stored in the node, or NULL if the node is NULL.
 */
ValueType map_node_get_value(MapNode* node) {
    if (!node) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: MapNode object is Null and Invalid in map_node_get_value.\n");
        #endif 
        return NULL;
    }
    return node->value;
}

/**
 * @brief Prints all key-value pairs in the map.
 *
 * @param map A pointer to the Map to be printed.
 * @param printKey A function pointer for printing the key.
 * @param printValue A function pointer for printing the value.
 */
void map_print(const Map* map, void (*printKey)(const KeyType), void (*printValue)(const ValueType)) {
    if (map == NULL || map->root == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Map object or map->root is Null and Invalid in map_print.\n");
        #endif 
        return;
    }

    if (!printKey || !printValue) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: printKey or PrintValue of Both of them are NULL in map_print.\n");
        #endif 
        return;
    }

    for (MapIterator it = map_begin(map); it.node != map_end(map).node; map_iterator_increment(&it)) {
        printKey(map_node_get_key(it.node));
        fmt_printf(": ");
        printValue(map_node_get_value(it.node));
        fmt_printf("\n");
    }
}

/**
 * @brief Creates a copy of the given map.
 *
 * @param src A pointer to the source Map to copy.
 * @return A pointer to the newly created copy of the map, or NULL if the source map is NULL or memory allocation fails.
 */
Map* map_copy(const Map* src) {
    if (src == NULL){
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Map Object is NULL and Invalid in map_copy.\n");
        #endif 
        return NULL;
    }

    Map* newMap = map_create(src->compFunc, src->deallocKey, src->deallocValue);
    if (!newMap) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Can not Allocate memory for newMap in map_copy.\n");    
        #endif 
        return NULL;
    }

    for (MapIterator it = map_begin(src); it.node != map_end(src).node; map_iterator_increment(&it)) { 
        map_insert(newMap, map_node_get_key(it.node), map_node_get_value(it.node));
    }
    return newMap;
}
