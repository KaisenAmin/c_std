/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Map
*/

#include <stdlib.h>
#include <string.h>
#include "map.h"


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
        MAP_LOG("[map_iterator_increment] : Error: Null iterator provided in map_iterator_increment.");
        return;
    }
    if (it->node == NULL) {
        MAP_LOG("[map_iterator_increment] : Error: Iterator's node is null in map_iterator_increment.");
        return;
    }
    MAP_LOG("[map_iterator_increment] : Incrementing iterator from node with key: %p", (void*)(it->node->key));

    // If the right subtree is not empty, the successor is the leftmost node in the right subtree
    if (it->node->right != NULL) {
        it->node = it->node->right;
        MAP_LOG("[map_iterator_increment] : Moved to right subtree, new node key: %p", (void*)(it->node->key));

        while (it->node->left != NULL) {
            it->node = it->node->left;
            MAP_LOG("[map_iterator_increment] : Moved to leftmost child, new node key: %p", (void*)(it->node->key));
        }
    } 
    else {
        // Go up the tree to find the first ancestor that is a left child of its parent
        while (it->node->parent != NULL && it->node == it->node->parent->right) { 
            it->node = it->node->parent;
            MAP_LOG("[map_iterator_increment] : Moving up the tree, current node key: %p", (void*)(it->node->key));
        }
        it->node = it->node->parent;
        if (it->node != NULL) {
            MAP_LOG("[map_iterator_increment] : Moved to parent, new node key: %p", (void*)(it->node->key));
        } 
        else {
            MAP_LOG("[map_iterator_increment] : Reached the end of the map.");
        }
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
        MAP_LOG("[map_iterator_decrement] : Error: Null iterator provided in map_iterator_decrement.");
        return;
    }
    if (it->node == NULL) {
        MAP_LOG("[map_iterator_decrement] : Error: Iterator's node is null in map_iterator_decrement.");
        return;
    }
    MAP_LOG("[map_iterator_decrement] : Decrementing iterator from node with key: %p", (void*)(it->node->key));

    // If the left subtree is not empty, the predecessor is the rightmost node in the left subtree
    if (it->node->left != NULL) {
        it->node = it->node->left;
        MAP_LOG("[map_iterator_decrement] : Moved to left subtree, new node key: %p", (void*)(it->node->key));

        while (it->node->right != NULL) { 
            it->node = it->node->right;
            MAP_LOG("[map_iterator_decrement] : Moved to rightmost child, new node key: %p", (void*)(it->node->key));
        }
    } 
    else {
        // Go up the tree to find the first ancestor that is a right child of its parent
        while (it->node->parent != NULL && it->node == it->node->parent->left) { 
            it->node = it->node->parent;
            MAP_LOG("[map_iterator_decrement] : Moving up the tree, current node key: %p", (void*)(it->node->key));
        }
        it->node = it->node->parent;
        if (it->node != NULL) {
            MAP_LOG("[map_iterator_decrement] : Moved to parent, new node key: %p", (void*)(it->node->key));
        } 
        else {
            MAP_LOG("[map_iterator_decrement] : Reached the beginning of the map.");
        }
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
    MAP_LOG("[create_node] : Creating a new node with key: %p and value: %p.", (void*)key, (void*)value);
    
    MapNode* node = (MapNode*)malloc(sizeof(MapNode));
    if (!node) {
        MAP_LOG("[create_node] : Error: Cannot allocate memory for node in create_node.");
        return NULL;
    }

    node->key = key;
    node->value = value;
    node->left = node->right = node->parent = NULL;
    node->color = RED; 

    MAP_LOG("[create_node] : Node created successfully with key: %p, value: %p, color: RED.", (void*)key, (void*)value);
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
        MAP_LOG("[map_left_rotate] : Error: Null pointer provided to map_left_rotate.");
        return;
    }

    MapNode* y = x->right;
    if (y == NULL) {
        MAP_LOG("[map_left_rotate] : Error: Right child is null in map_left_rotate for node: %p.", (void*)x);
        return;
    }

    MAP_LOG("[map_left_rotate] : Performing left rotation on node: %p with right child: %p.", (void*)x, (void*)y);

    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
        MAP_LOG("[map_left_rotate] : Set the parent of left child %p to %p.", (void*)(y->left), (void*)x);
    }

    y->parent = x->parent;
    if (x->parent == NULL) {
        map->root = y;
        MAP_LOG("[map_left_rotate] : Node %p is now the root after left rotation.", (void*)y);
    } 
    else if (x == x->parent->left) {
        x->parent->left = y;
        MAP_LOG("[map_left_rotate] : Node %p is now the left child of its parent %p.", (void*)y, (void*)(x->parent));
    } 
    else {
        x->parent->right = y;
        MAP_LOG("[map_left_rotate] : Node %p is now the right child of its parent %p.", (void*)y, (void*)(x->parent));
    }

    y->left = x;
    x->parent = y;
    MAP_LOG("[map_left_rotate] : Left rotation completed. Node %p is now left child of node %p.", (void*)x, (void*)y);
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
        MAP_LOG("[map_right_rotate] : Error: Null pointer provided to map_right_rotate.");
        return;
    }

    MapNode* x = y->left;
    if (x == NULL) {
        MAP_LOG("[map_right_rotate] : Error: Left child is null in map_right_rotate for node: %p.", (void*)y);
        return;
    }

    MAP_LOG("[map_right_rotate] : Performing right rotation on node: %p with left child: %p.", (void*)y, (void*)x);

    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
        MAP_LOG("[map_right_rotate] : Set the parent of right child %p to %p.", (void*)(x->right), (void*)y);
    }

    x->parent = y->parent;
    if (y->parent == NULL) {
        map->root = x;
        MAP_LOG("[map_right_rotate] : Node %p is now the root after right rotation.",(void*)x);
    } 
    else if (y == y->parent->right) {
        y->parent->right = x;
        MAP_LOG("[map_right_rotate] : Node %p is now the right child of its parent %p.", (void*)x, (void*)(y->parent));
    } 
    else {
        y->parent->left = x;
        MAP_LOG("[map_right_rotate] : Node %p is now the left child of its parent %p.", (void*)x, (void*)(y->parent));
    }

    x->right = y;
    y->parent = x;
    MAP_LOG("[map_right_rotate] : Right rotation completed. Node %p is now right child of node %p.", (void*)y, (void*)x);
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
        MAP_LOG("[map_transplant] : Error: Null pointer provided to map_transplant.");
        return;
    }
    MAP_LOG("[map_transplant]: Replacing node %p with node %p.", (void*)u, (void*)v);

    if (u->parent == NULL) {
        map->root = v;
        MAP_LOG("[map_transplant]: Node %p is the root. Setting new root to %p.", (void*)u, (void*)v);
    } 
    else if (u == u->parent->left) {
        u->parent->left = v;
        MAP_LOG("[map_transplant] : Node %p was the left child of its parent. Replacing with node %p.", (void*)u, (void*)v);
    } 
    else {
        u->parent->right = v;
        MAP_LOG("[map_transplant] : Node %p was the right child of its parent. Replacing with node %p.", (void*)u, (void*)v);
    }

    if (v != NULL) {
        v->parent = u->parent;
        MAP_LOG("[map_transplant] : Setting node %p's parent to %p.", (void*)v, (void*)(u->parent));
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
        MAP_LOG("[map_minimum] : Error: Null pointer provided to map_minimum.");
        return NULL;
    }
    MAP_LOG("[map_minimum] : Finding minimum node starting from %p.", (void*)node);

    while (node->left != NULL) {
        node = node->left;
        MAP_LOG("[map_minimum] : Moving left to node %p.", (void*)node);
    }

    MAP_LOG("[map_minimum] : Minimum node found: %p.", (void*)node);
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
    if (map == NULL) {
        MAP_LOG("[map_erase_fixup] : Error: Null pointer provided to map_erase_fixup.");
        return;
    }

    MAP_LOG("[map_erase_fixup] : Fixing up tree after deletion starting with node %p.", (void*)x);

    while (x != map->root && (x == NULL || x->color == BLACK)) {
        if (x == x->parent->left) {
            MapNode* w = x->parent->right;
            MAP_LOG("[map_erase_fixup] : Sibling node (right of parent) is %p.", (void*)w);

            if (w->color == RED) {
                MAP_LOG("[map_erase_fixup] : Case 1 - Sibling is red, performing left rotation on parent %p.", (void*)(x->parent));
                w->color = BLACK;
                x->parent->color = RED;
                map_left_rotate(map, x->parent);
                w = x->parent->right;
            }

            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                MAP_LOG("[map_erase_fixup] : Case 2 - Both children of sibling are black. Recoloring sibling %p.", (void*)w);
                w->color = RED;
                x = x->parent;
            } 
            else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left != NULL) {
                        w->left->color = BLACK;
                    }
                    MAP_LOG("[map_erase_fixup] : Case 3 - Left child of sibling is red. Performing right rotation.");
                    w->color = RED;
                    map_right_rotate(map, w);
                    w = x->parent->right;
                }

                MAP_LOG("[map_erase_fixup] : Case 4 - Right child of sibling is red. Performing left rotation on parent %p.", (void*)(x->parent));
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
            MAP_LOG("[map_erase_fixup] : Sibling node (left of parent) is %p.", (void*)w);

            if (w->color == RED) {
                MAP_LOG("[map_erase_fixup] : Case 1 - Sibling is red, performing right rotation on parent %p.", (void*)(x->parent));
                w->color = BLACK;
                x->parent->color = RED;
                map_right_rotate(map, x->parent);
                w = x->parent->left;
            }

            if ((w->right == NULL || w->right->color == BLACK) &&
                (w->left == NULL || w->left->color == BLACK)) {
                MAP_LOG("[map_erase_fixup] : Case 2 - Both children of sibling are black. Recoloring sibling %p.", (void*)w);
                w->color = RED;
                x = x->parent;
            } 
            else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right != NULL) {
                        w->right->color = BLACK;
                    }
                    MAP_LOG("[map_erase_fixup] : Case 3 - Right child of sibling is red. Performing left rotation.");
                    w->color = RED;
                    map_left_rotate(map, w);
                    w = x->parent->left;
                }

                MAP_LOG("[map_erase_fixup] : Case 4 - Left child of sibling is red. Performing right rotation on parent %p.", (void*)(x->parent));
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
        MAP_LOG("[map_erase_fixup] : Recoloring node %p to black.", (void*)x);
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
        MAP_LOG("[map_free_nodes] : Error: node param is null and invalid in map_free_nodes.");
        return;
    }
    MAP_LOG("[map_free_nodes] : Freeing node with key: %p.", (void*)(node->key));

    map_free_nodes(node->left, deallocKey, deallocValue);
    map_free_nodes(node->right, deallocKey, deallocValue);

    // Deallocate the key and value if deallocation functions are provided
    if (deallocKey) {
        MAP_LOG("[map_free_nodes] : Deallocating key: %p.", (void*)(node->key));
        deallocKey(node->key);
    }
    if (deallocValue) {
        MAP_LOG("[map_free_nodes] : Deallocating value: %p.", (void*)(node->value));
        deallocValue(node->value);
    }

    
    free(node); 
    MAP_LOG("[map_free_nodes] : Node with keysuccessfully freed.");
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
        MAP_LOG("[map_insert_fixup] : Error: Null pointer provided to map_insert_fixup.");
        return;
    }
    MAP_LOG("[map_insert_fixup] : Fixing up after insertion of node with key %p.", (void*)(newNode->key));

    while (newNode != map->root && newNode->parent->color == RED) {
        if (newNode->parent == newNode->parent->parent->left) {
            MapNode* uncle = newNode->parent->parent->right;
            if (uncle && uncle->color == RED) {
                MAP_LOG("[map_insert_fixup] : Case 1 (left) - Parent and uncle are red.");
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent;
            } 
            else {
                if (newNode == newNode->parent->right) {
                    MAP_LOG("[map_insert_fixup] : Case 2 (left) - Performing left rotation.");
                    newNode = newNode->parent;
                    map_left_rotate(map, newNode);
                }
                MAP_LOG("[map_insert_fixup] : Case 3 (left) - Performing right rotation.");

                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                map_right_rotate(map, newNode->parent->parent);
                break; // Exit loop
            }
        } 
        else {
            MapNode* uncle = newNode->parent->parent->left;
            if (uncle && uncle->color == RED) {
                MAP_LOG("[map_insert_fixup] : Case 1 (right) - Parent and uncle are red.");
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent;
            } 
            else {
                if (newNode == newNode->parent->left) {
                    MAP_LOG("[map_insert_fixup] : Case 2 (right) - Performing right rotation.");
                    newNode = newNode->parent;
                    map_right_rotate(map, newNode);
                }

                MAP_LOG("[map_insert_fixup] : Case 3 (right) - Performing left rotation.");
                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                map_left_rotate(map, newNode->parent->parent);
                break; 
            }
        }
    }

    map->root->color = BLACK;
    MAP_LOG("[map_insert_fixup] : Fix-up completed, root color set to BLACK.");
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
        MAP_LOG("[map_create] : Error: Compare function is null, cannot create map.");
        exit(-1);
    }
    MAP_LOG("[map_create]: Creating a new map.");

    Map* map = (Map*)malloc(sizeof(Map));
    if (!map) {
        MAP_LOG("[map_create] : Error: Cannot allocate memory for map.");
        exit(-1);
    }

    map->root = NULL;
    map->compFunc = comp;
    map->deallocKey = deallocKey;
    map->deallocValue = deallocValue;
    map->size = 0;

    MAP_LOG("[map_create] : Map created successfully.");

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
    if (!map) {
        MAP_LOG("[map_deallocate] : Warning: Map is null or empty, nothing to deallocate in map_deallocate.");
        return;
    }
    MAP_LOG("[map_deallocate] : Deallocating map and freeing all nodes.");

    map_free_nodes(map->root, map->deallocKey, map->deallocValue);
    free(map);

    MAP_LOG("[map_deallocate] : Map deallocated successfully.");
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
        MAP_LOG("[map_empty] : Error: Null pointer provided for map in map_empty.");
        return true; 
    }
    MAP_LOG("[map_empty]: Checking if the map is empty. Map size is %zu.", map->size);

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
        MAP_LOG("[map_size]: Error: Null pointer provided for map in map_size.");
        return 0; 
    }

    MAP_LOG("[map_size]: Returning the map size: %zu.", map->size);
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
        MAP_LOG("[map_max_size] : Error: Null pointer provided for map in map_max_size.");
        return 0;
    }

    size_t max_size = (size_t)(~((size_t)0)) / sizeof(MapNode);
    MAP_LOG("[map_max_size] : Maximum possible size of the map: %zu.", max_size);

    return max_size;
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
        MAP_LOG("[map_insert] : Error: Null pointer provided for map or key in map_insert.");
        return false;
    }
    MAP_LOG("[map_insert] : Attempting to insert key-value pair into the map.");

    MapNode** curr = &map->root;
    MapNode* parent = NULL;

    while (*curr) {
        parent = *curr;
        int cmp = map->compFunc(key, (*curr)->key);

        if (cmp == 0) {
            MAP_LOG("[map_insert] : Key found, replacing the existing value.");
            if (map->deallocValue) {
                map->deallocValue((*curr)->value);
            }

            (*curr)->value = value;
            MAP_LOG("[map_insert] : Value replaced successfully.");
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
        MAP_LOG("[map_insert] : Error: Node creation failed in map_insert.");
        return false;
    }

    *curr = newNode;
    newNode->parent = parent;
    map->size++;

    MAP_LOG("[map_insert] : Node inserted successfully. Performing tree balancing.");
    map_insert_fixup(map, newNode);
    MAP_LOG("[map_insert] : Insertion completed. Map size is now %zu.", map->size);

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
        MAP_LOG("[map_at] : Error: Null pointer provided for map or key in map_at.");
        return NULL; 
    }

    MAP_LOG("[map_at] : Searching for key in the map.");
    MapNode* curr = map->root;
    
    while (curr) {
        int cmp = map->compFunc(key, curr->key);

        if (cmp == 0) {
            MAP_LOG("[map_at] : Key found, returning the associated value.");
            return curr->value;
        }
        if (cmp < 0) {
            curr = curr->left;
        } 
        else {
            curr = curr->right;
        }
    }

    MAP_LOG("[map_at] : Key not found in the map.");
    return NULL; 
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
        MAP_LOG("[map_clear] : Error: Null pointer provided for map in map_clear.");
        return;
    }

    MAP_LOG("[map_clear] : Clearing all nodes in the map.");
    map_free_nodes(map->root, map->deallocKey, map->deallocValue);

    map->root = NULL;
    map->size = 0;

    MAP_LOG("[map_clear] : Map cleared successfully.");
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
        MAP_LOG("[map_swap]: Error: Null pointer provided for one or both maps in map_swap.");
        return;
    }

    // Log swapping details
    MAP_LOG("[map_swap]: Swapping the root nodes and sizes of the two maps.");

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

    MAP_LOG("[map_swap] : Swap operation completed successfully.");
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
        MAP_LOG("[map_count] : Error: Null pointer provided for map in map_count.");
        return 0;
    }

    MAP_LOG("[map_count] : Searching for the key in the map.");

    MapNode* current = map->root;
    while (current != NULL) {
        int cmp = map->compFunc(key, current->key);
        if (cmp == 0) {
            MAP_LOG("[map_count] : Key found in the map.");
            return 1; 
        }
        if (cmp < 0) { 
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    MAP_LOG("[map_count] : Key not found in the map.");
    return 0; 
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
        MAP_LOG("[map_emplace] : Error: Null pointer provided for map or key in map_emplace.");
        return false;
    }

    MAP_LOG("[map_emplace] : Attempting to insert key-value pair into the map.");

    MapNode** curr = &map->root;
    MapNode* parent = NULL;

    while (*curr) {
        parent = *curr;
        int cmp = map->compFunc(key, (*curr)->key);
        
        if (cmp == 0) {
            MAP_LOG("[map_emplace] : Key already exists in the map. No insertion performed.");
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
        MAP_LOG("[map_emplace] : Failed to create a new node for the key-value pair.");
        return false;
    }

    *curr = newNode;
    newNode->parent = parent;
    map->size++;

    MAP_LOG("[map_emplace] : Key-value pair inserted successfully. Rebalancing the tree.");
    map_insert_fixup(map, newNode); 
    MAP_LOG("[map_emplace] : Insertion completed. Map size is now %zu.", map->size);

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
        MAP_LOG("[map_key_comp] : Error: Null pointer provided for map in map_key_comp.");
        return NULL;
    }

    MAP_LOG("[map_key_comp] : Returning comparison function for the map.");
    return map->compFunc; 
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
    if (map == NULL || key == NULL) {
        MAP_LOG("[map_emplace_hint] : Error: map or key is null, cannot emplace in map_emplace_hint.");
        exit(-1);  
    }

    MapNode* newNode = create_node(key, value);
    if (newNode == NULL) {
        MAP_LOG("[map_emplace_hint] : Error: Unable to create new node in map_emplace_hint.");
        return false;
    }
    MAP_LOG("[map_emplace_hint] : New node created for key-value pair.");

    if (map->root == NULL) {
        map->root = newNode;
        map->root->color = BLACK;  
        map->size++;

        MAP_LOG("[map_emplace_hint] : Map was empty, inserted node as root. Map size: %zu", map->size);
        return true;
    }

    // If hint is provided and valid, attempt to use it.
    if (hint.node != NULL) {
        MapNode* curr = hint.node;
        int cmp = map->compFunc(key, curr->key);

        if (cmp < 0) {
            if (curr->left == NULL) {
                curr->left = newNode;
                newNode->parent = curr;
                map->size++;
                map_insert_fixup(map, newNode);

                MAP_LOG("[map_emplace_hint] : Inserted node to the left of hint node. Map size: %zu", map->size);
                return true;
            }
        } 
        else if (cmp > 0) {
            if (curr->right == NULL) {
                curr->right = newNode;
                newNode->parent = curr;
                map->size++;
                map_insert_fixup(map, newNode);

                MAP_LOG("[map_emplace_hint] : Inserted node to the right of hint node. Map size: %zu", map->size);
                return true;
            }
        } 
        else {
            // Key already exists, deallocate newNode and return false.
            free(newNode);
            MAP_LOG("[map_emplace_hint] : Key already exists, no insertion. Map size remains: %zu", map->size);
            return false;
        }
    }

    MapNode** curr = &map->root;
    MapNode* parent = NULL;

    while (*curr) {
        parent = *curr;
        int cmp = map->compFunc(key, (*curr)->key);

        if (cmp == 0) {
            free(newNode);
            MAP_LOG("[map_emplace_hint] : Key already exists during fallback insertion. Map size remains: %zu", map->size);
            return false;
        }

        curr = (cmp < 0) ? &(*curr)->left : &(*curr)->right;
    }

    *curr = newNode;
    newNode->parent = parent;
    map->size++;
    map_insert_fixup(map, newNode);
    MAP_LOG("[map_emplace_hint] : Inserted node without hint. Map size: %zu", map->size);

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
        MAP_LOG("[map_erase] : Error: map or map root is null in map_erase.");
        return false;
    }
    
    MapNode* z = map->root;
    while (z != NULL) {
        int cmp = map->compFunc(key, z->key);
        if (cmp == 0) {
            MAP_LOG("[map_erase] : Key found, proceeding with node deletion.");
            break; 
        }
        z = (cmp < 0) ? z->left : z->right;
    }

    if (z == NULL) {
        MAP_LOG("[map_erase] : Error: Key not found in map_erase.");
        return false;
    }
    
    MapNode* y = z;
    int y_original_color = y->color;
    MapNode* x;

    if (z->left == NULL) {
        x = z->right;
        map_transplant(map, z, z->right);
        MAP_LOG("[map_erase] : Node has no left child, transplanting right child.");
    } 
    else if (z->right == NULL) {
        x = z->left;
        map_transplant(map, z, z->left);
        MAP_LOG("[map_erase] : Node has no right child, transplanting left child.");
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
        MAP_LOG("[map_erase] : Node has two children, finding successor and transplanting.");
    }

    if (map->deallocKey) {
        map->deallocKey(z->key);
        MAP_LOG("[map_erase] : Deallocating key of the node.");
    }
    if (map->deallocValue) {
        map->deallocValue(z->value);
        MAP_LOG("[map_erase] : Deallocating value of the node.");
    }
    free(z);
    MAP_LOG("[map_erase] : Node deleted and memory freed.");

    if (y_original_color == BLACK) { 
        map_erase_fixup(map, x);
        MAP_LOG("[map_erase] : Fixing up the red-black tree after deletion.");
    }
    map->size--;
    
    MAP_LOG("[map_erase] : Node deleted successfully. New map size: %zu", map->size);
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
    MapIterator iterator = {0}; 

    if (map == NULL || key == NULL) {
        MAP_LOG("[map_find] : Error: Null pointer provided for map or key in map_find.");
        return iterator; 
    }

    MapNode* current = map->root;
    while (current != NULL) {
        int cmp = map->compFunc(key, current->key); 
        if (cmp == 0) {
            iterator.node = current;
            MAP_LOG("[map_find] : Key found in map.");
            return iterator;
        }
        current = (cmp < 0) ? current->left : current->right;
    }

    MAP_LOG("[map_find] : Key not found in map.");
    return iterator; 
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
    MapIterator iterator = {0};  

    if (map == NULL) {
        MAP_LOG("[map_begin] : Error: Null pointer provided for map in map_begin.");
        return iterator; 
    }

    // The first element is the leftmost node in the tree.
    MapNode* current = map->root;
    if (current == NULL) {
        MAP_LOG("[map_begin] : Map is empty, returning default iterator.");
        return iterator; 
    }

    while (current->left != NULL) {
        current = current->left;
    }

    iterator.node = current;  
    MAP_LOG("[map_begin] : Leftmost node found, iterator set to the first element.");

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
        MAP_LOG("[map_end] : Error: Null pointer provided for map in map_end.");
        return (MapIterator){0}; 
    }

    MAP_LOG("[map_end] : Returning default 'end' iterator.");
    return (MapIterator){0}; 
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
        MAP_LOG("[map_rbegin] : Error: Null pointer provided for map in map_rbegin.");
        return iterator; 
    }

    MapNode* current = map->root;
    if (current == NULL) {
        MAP_LOG("[map_rbegin] : Map is empty, returning default iterator.");
        return iterator; 
    }

    // The rightmost node is the largest element in the map
    while (current->right != NULL) {
        current = current->right;
    }
    iterator.node = current;  
    MAP_LOG("[map_rbegin] : Rightmost node found, iterator set to the last element.");

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
        MAP_LOG("[map_rend] : Error: Null pointer provided for map in map_rend.");
        return (MapIterator){0}; 
    }

    MAP_LOG("[map_rend] : Returning default 'rend' iterator.");
    return (MapIterator){0}; 
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
        MAP_LOG("[map_cbegin] : Error: Null pointer provided for map in map_cbegin.");
        return iterator; 
    }
    if (map->root != NULL) {
        MapNode* current = map->root;
        MAP_LOG("[map_cbegin] : Navigating to the leftmost node.");
        
        while (current->left != NULL) {
            current = current->left;
        }
        iterator.node = current;
        MAP_LOG("[map_cbegin] : Leftmost node found. Returning iterator to the first element.");
    } 
    else {
        MAP_LOG("[map_cbegin] : Map is empty, returning default iterator.");
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
        MAP_LOG("[map_cend] : Error: Null pointer provided for map in map_cend.");
        return (MapIterator){0}; 
    }

    MAP_LOG("[map_cend]: Returning constant iterator to the end of the map.");
    return (MapIterator){0}; 
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
        MAP_LOG("[map_crbegin] : Error: Null pointer provided for map in map_crbegin.");
        return iterator; 
    }
    if (map->root != NULL) {
        MapNode* current = map->root;
        MAP_LOG("[map_crbegin] : Navigating to the rightmost node.");
        
        while (current->right != NULL) {
            current = current->right;
        }
        iterator.node = current;
        MAP_LOG("[map_crbegin] : Rightmost node found. Returning iterator to the last element.");
    } 
    else {
        MAP_LOG("[map_crbegin] : Map is empty, returning default iterator.");
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
        MAP_LOG("[map_crend] : Error: Null pointer provided for map in map_crend.");
        return (MapIterator){0}; // Return default iterator on error
    }

    MAP_LOG("[map_crend] : Returning constant reverse iterator to the end of the reverse sequence.");
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
        MAP_LOG("[map_lower_bound] : Error: Null pointer provided for map or key in map_lower_bound.");
        return iterator; 
    }

    MapNode* current = map->root;
    MapNode* last = NULL;
    MAP_LOG("[map_lower_bound] : Starting map_lower_bound search for the provided key.");

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
        MAP_LOG("[map_lower_bound] : map_lower_bound found a matching node.");
    } 
    else {
        MAP_LOG("[map_lower_bound] : map_lower_bound did not find a matching node. Returning end iterator.");
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
    MapIterator iterator = {0}; 

    if (map == NULL || key == NULL) {
        MAP_LOG("[map_upper_bound] : Error: Null pointer provided for map or key in map_upper_bound.");
        return iterator; 
    }

    MapNode* current = map->root;
    MapNode* last = NULL;
    MAP_LOG("[map_upper_bound] : Starting map_upper_bound search for the provided key.");

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
        MAP_LOG("[map_upper_bound] : map_upper_bound found a matching node.");
    } 
    else {
        MAP_LOG("[map_upper_bound] : map_upper_bound did not find a matching node. Returning end iterator.");
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
        MAP_LOG("[map_equal_range] : Error: Map object is NULL in map_equal_range.");
        return iteratorPair; 
    }
    if (key == NULL) {
        MAP_LOG("[map_equal_range] : Error: KeyType value is NULL in map_equal_range.");
        return iteratorPair; 
    }

    MAP_LOG("[map_equal_range] : Starting map_equal_range search for the provided key.");
    iteratorPair.first = map_lower_bound(map, key);
    iteratorPair.second = map_upper_bound(map, key);

    MAP_LOG("[map_equal_range] : map_equal_range search completed.");

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
        MAP_LOG("[map_node_get_key] : Error: MapNode object is NULL and invalid in map_node_get_key.");
        return NULL;
    }

    MAP_LOG("[map_node_get_key] : Successfully retrieved key from MapNode.");
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
        MAP_LOG("[map_node_get_value] : Error: MapNode object is NULL and invalid in map_node_get_value.");
        return NULL;
    }

    MAP_LOG("[map_node_get_value] : Successfully retrieved value from MapNode.");
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
        MAP_LOG("[map_print] : Error: Map object or map->root is NULL and invalid in map_print.");
        return;
    }
    if (!printKey || !printValue) {
        MAP_LOG("[map_print] : Error: printKey or printValue is NULL in map_print.");
        return;
    }
    MAP_LOG("[map_print] : Starting map_print operation...");

    for (MapIterator it = map_begin(map); it.node != map_end(map).node; map_iterator_increment(&it)) {
        MAP_LOG("[map_print] : Printing key-value pair.");

        printKey(map_node_get_key(it.node));
        printf(": ");
        printValue(map_node_get_value(it.node));
        printf("\n");
    }
    MAP_LOG("[map_print] : Map printing completed.");
}

/**
 * @brief Creates a copy of the given map.
 *
 * @param src A pointer to the source Map to copy.
 * @return A pointer to the newly created copy of the map, or NULL if the source map is NULL or memory allocation fails.
 */
Map* map_copy(const Map* src) {
    if (src == NULL) {
        MAP_LOG("[map_copy] : Error: Map object is NULL and invalid in map_copy.");
        return NULL;
    }
    MAP_LOG("[map_copy] : Starting map_copy operation...");

    Map* newMap = map_create(src->compFunc, src->deallocKey, src->deallocValue);
    if (!newMap) {
        MAP_LOG("[map_copy] : Error: Cannot allocate memory for newMap in map_copy.");
        return NULL;
    }
    MAP_LOG("[map_copy] : New map created, starting element copy...");

    for (MapIterator it = map_begin(src); it.node != map_end(src).node; map_iterator_increment(&it)) {
        KeyType key = map_node_get_key(it.node);
        ValueType value = map_node_get_value(it.node);
        map_insert(newMap, key, value);
        MAP_LOG("[map_copy] : Inserted key-value pair into new map during copy.");
    }

    MAP_LOG("[map_copy] : Map copy operation completed successfully.");
    return newMap;
}
