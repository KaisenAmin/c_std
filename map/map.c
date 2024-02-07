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

bool map_empty(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_empty.\n");
        #endif 
        return true; // Consider an empty map for null pointer
    }
    return map->size == 0;
}

size_t map_size(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_size.\n");
        #endif 
        return 0; // Return 0 for null pointer
    }
    return map->size;
}

size_t map_max_size(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_max_size.\n");
        #endif 
        return 0; // Return 0 for null pointer
    }
    return (size_t)(~((size_t)0)) / sizeof(MapNode);
}

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

CompareFuncMap map_key_comp(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_key_comp.\n");
        #endif 
        return NULL;
    }
    return map->compFunc;  // Return the comparison function pointer
}

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

MapIterator map_end(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_end.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default 'end' iterator
}

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

MapIterator map_rend(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_rend.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default 'end' iterator
}

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

MapIterator map_cend(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_cend.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default end iterator
}

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

MapIterator map_crend(const Map* map) {
    if (map == NULL) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null pointer provided for map in map_crend.\n");
        #endif 
        return (MapIterator){0}; // Return default iterator on error
    }
    return (MapIterator){0}; // Default end iterator
}

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

KeyType map_node_get_key(MapNode* node) {
    if (!node) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: MapNode object is Null and Invalid in map_node_get_key.\n");
        #endif 
        return NULL;
    }
    return node->key;
}

ValueType map_node_get_value(MapNode* node) {
    if (!node) {
        #ifdef MAP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: MapNode object is Null and Invalid in map_node_get_value.\n");
        #endif 
        return NULL;
    }
    return node->value;
}

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
