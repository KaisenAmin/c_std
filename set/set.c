/**
 * @author Amin Tahmasebi
 * @date 2026
 * @class Set
 *
 * Ordered set backed by a red-black tree (the same proven algorithm as the
 * `map` module), but with VALUE SEMANTICS: each element is deep-copied
 * (`elemSize` bytes) into its node and owned by the set. O(log n) insert,
 * erase, find, and bound queries; O(n) in-order iteration.
 *
 * Per the project-wide convention the public Doxygen contracts live above
 * the DEFINITIONS in this file (the header only declares the prototypes).
 *
 * Naming: the public API is prefixed `set_*`; every internal (static) helper
 * uses the `rbset_*` prefix ("red-black set" internals), so a private function
 * never looks like a public one.
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "set.h"


#define RED   1
#define BLACK 0


/* Nodes are carved out of a per-set POOL: each malloc'd slab holds many node
 * slots, so inserting avoids a malloc per element and destroying frees whole
 * slabs (O(slabs)) instead of one node at a time. Erased nodes go on a
 * free-list for reuse. The element is stored INLINE in each slot (like
 * std::set's node-with-value), at an offset matching the element's alignment. */
typedef struct SetSlab {
    struct SetSlab* next;
    size_t          used;       /* node slots handed out from this slab */
} SetSlab;

struct Set {
    SetNode*        root;
    SetCompareFunc  compare;
    size_t          elemSize;
    size_t          size;
    size_t          dataOffset; /* byte offset of the inline element in a slot */
    size_t          slotSize;   /* bytes per node slot (header + element)      */
    size_t          slabCap;    /* node slots per slab                         */
    SetSlab*        slabs;      /* slab list (newest first)                    */
    SetNode*        freeList;   
};


/* Largest power of two dividing elemSize, capped at max_align_t. A type's
 * alignment always divides its size, so this is always >= the real element
 * alignment (it never under-aligns), yet it is tighter than always using
 * max_align_t — letting a generic void* set pack as compactly as a typed
 * container instead of wasting up to 8 bytes per node. */
static size_t rbset_elem_align(size_t elemSize) {
    size_t maxa = _Alignof(max_align_t), a = 1;

    while (a < maxa && (elemSize % (a * 2)) == 0) {
        a *= 2;
    }

    return a;
}

/* Carve one node slot from the pool: reuse a freed slot, else the current
 * slab, else a fresh slab. Returns NULL only if a slab allocation fails.
 * node->key is pointed at the slot's inline element area. */
static SetNode* rbset_pool_alloc(Set* set) {
    SetNode* node;
    if (set->freeList) {
        node = set->freeList;
        set->freeList = node->left;
    }
    else {
        SetSlab* slab = set->slabs;

        if (!slab || slab->used == set->slabCap) {
            slab = (SetSlab*)malloc(sizeof(SetSlab) + set->slabCap * set->slotSize);

            if (!slab) {
                SET_LOG("[rbset_pool_alloc] Error: slab malloc failed.");
                return NULL;
            }

            slab->used = 0;
            slab->next = set->slabs;
            set->slabs = slab;
        }
        node = (SetNode*)((unsigned char*)slab + sizeof(SetSlab) + slab->used * set->slotSize);
        slab->used++;
    }
    node->key = (unsigned char*)node + set->dataOffset;
    return node;
}


/* Return a node's slot to the free-list for reuse (used on erase). */
static void rbset_pool_free(Set* set, SetNode* node) {
    node->left = set->freeList;
    set->freeList = node;
}


/* Release every slab (used on clear / deallocate). O(number of slabs). */
static void rbset_pool_destroy(Set* set) {
    SetSlab* s = set->slabs;

    while (s) {
        SetSlab* next = s->next;
        free(s);
        s = next;
    }

    set->slabs = NULL;
    set->freeList = NULL;
}


/**
 * @brief Allocates a new RED leaf node holding a deep copy of `element`.
 *
 * The node comes from the set's pool and the `elemSize`-byte element is stored
 * INLINE in its slot (like std::set's node-with-value). `node->key` already
 * points at the inline area. The node is coloured RED, with all links NULL.
 *
 * @param set     The owning set (provides the pool and element size).
 * @param element Pointer to the element bytes to copy in.
 * @return The new node, or NULL if a slab allocation fails.
 */
static SetNode* rbset_create_node(Set* set, const void* element) {
    SetNode* node = rbset_pool_alloc(set);
    if (!node) {
        return NULL;
    }

    memcpy(node->key, element, set->elemSize);   /* deep copy — set owns it */
    node->left = node->right = node->parent = NULL;
    node->color = RED;
    return node;
}


/* (set_free_nodes was removed: the pool frees whole slabs in rbset_pool_destroy,
 *  which is O(slabs) rather than one free() per node.) */


/**
 * @brief Deep structural clone of a subtree into `dst`'s pool.
 *
 * Used by set_copy: the copy is an exact, already-balanced replica (colours and
 * shape preserved, so no rebalancing). On any allocation failure `*ok` is set
 * to false; set_copy then releases the partial tree via the pool.
 *
 * @param dst    The destination set (provides the node pool and element size).
 * @param node   Source subtree root.
 * @param parent Parent to assign to the cloned root.
 * @param ok     Out-flag set to false on allocation failure.
 * @return The cloned subtree root, or NULL.
 */
static SetNode* rbset_clone_subtree(Set* dst, const SetNode* node, SetNode* parent, bool* ok) {
    if (!node || !*ok) {
        return NULL;
    }

    SetNode* n = rbset_pool_alloc(dst);
    if (!n) {
        *ok = false;
        return NULL;
    }
    memcpy(n->key, node->key, dst->elemSize);   /* n->key set by the pool */
    n->color  = node->color;
    n->parent = parent;
    n->left   = rbset_clone_subtree(dst, node->left,  n, ok);
    n->right  = rbset_clone_subtree(dst, node->right, n, ok);

    return n;
}


/** @brief Leftmost (minimum) node of a subtree, or NULL. */
static SetNode* rbset_minimum(SetNode* node) {
    if (!node) {
        return NULL;
    }
    while (node->left) {
        node = node->left;
    }
    return node;
}


/** @brief Rightmost (maximum) node of a subtree, or NULL. */
static SetNode* rbset_maximum(SetNode* node) {
    if (!node) {
        return NULL;
    }
    while (node->right) {
        node = node->right;
    }
    return node;
}


/** @brief BST lookup of the node equal to `element`, or NULL if absent. */
static SetNode* rbset_find_node(const Set* set, const void* element) {
    SetNode* cur = set->root;
    while (cur) {
        int cmp = set->compare(element, cur->key);
        if (cmp == 0) {
            return cur;
        }
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return NULL;
}


/** @brief Left rotation about `x`, keeping parent links consistent. */
static void rbset_left_rotate(Set* set, SetNode* x) {
    if (!set || !x) {
        return;
    }

    SetNode* y = x->right;
    if (!y) {
        return;
    }

    x->right = y->left;
    if (y->left) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        set->root = y;
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


/** @brief Right rotation about `y`, keeping parent links consistent. */
static void rbset_right_rotate(Set* set, SetNode* y) {
    if (!set || !y) {
        return;
    }

    SetNode* x = y->left;
    if (!x) {
        return;
    }

    y->left = x->right;
    if (x->right) {
        x->right->parent = y;
    }

    x->parent = y->parent;
    if (y->parent == NULL) {
        set->root = x;
    }
    else if (y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}


/** @brief Replaces the subtree rooted at `u` with the subtree rooted at `v`. */
static void rbset_transplant(Set* set, SetNode* u, SetNode* v) {
    if (!set || !u) {
        return;
    }
    if (u->parent == NULL) {
        set->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v) {
        v->parent = u->parent;
    }
}


/**
 * @brief Restores red-black invariants after a BST insertion (CLRS RB-INSERT-FIXUP).
 * @param set     Owning set.
 * @param newNode The freshly inserted RED node.
 */
static void rbset_insert_fixup(Set* set, SetNode* newNode) {
    if (!set || !newNode) {
        return;
    }
    while (newNode != set->root && newNode->parent->color == RED) {
        if (newNode->parent == newNode->parent->parent->left) {
            SetNode* uncle = newNode->parent->parent->right;
            if (uncle && uncle->color == RED) {
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent;
            }
            else {
                if (newNode == newNode->parent->right) {
                    newNode = newNode->parent;
                    rbset_left_rotate(set, newNode);
                }
                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                rbset_right_rotate(set, newNode->parent->parent);
                break;
            }
        }
        else {
            SetNode* uncle = newNode->parent->parent->left;
            if (uncle && uncle->color == RED) {
                newNode->parent->color = BLACK;
                uncle->color = BLACK;
                newNode->parent->parent->color = RED;
                newNode = newNode->parent->parent;
            }
            else {
                if (newNode == newNode->parent->left) {
                    newNode = newNode->parent;
                    rbset_right_rotate(set, newNode);
                }
                newNode->parent->color = BLACK;
                newNode->parent->parent->color = RED;
                rbset_left_rotate(set, newNode->parent->parent);
                break;
            }
        }
    }
    set->root->color = BLACK;
}


/**
 * @brief Restores red-black invariants after a deletion (CLRS RB-DELETE-FIXUP).
 *
 * `x` may be NULL when the deleted leaf had no replacement child; in that case
 * `x_parent` identifies which slot of which parent `x` conceptually occupies,
 * so the sibling can still be found without dereferencing a NULL `x`.
 *
 * @param set      Owning set.
 * @param x        The node that replaced the spliced-out node (may be NULL).
 * @param x_parent Parent slot of `x` (used when `x` is NULL).
 */
static void rbset_erase_fixup(Set* set, SetNode* x, SetNode* x_parent) {
    if (!set) {
        return;
    }
    while (x != set->root && (x == NULL || x->color == BLACK)) {
        SetNode* parent = x ? x->parent : x_parent;
        if (parent == NULL) {
            break;
        }
        if (x == parent->left) {
            SetNode* w = parent->right;
            if (w == NULL) {
                break;
            }
            if (w->color == RED) {
                w->color = BLACK;
                parent->color = RED;
                rbset_left_rotate(set, parent);
                w = parent->right;

                if (w == NULL) {
                    break;
                }
            }
            if ((w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = parent;
                x_parent = parent->parent;
            }
            else {
                if (w->right == NULL || w->right->color == BLACK) {
                    if (w->left) w->left->color = BLACK;
                    w->color = RED;
                    rbset_right_rotate(set, w);
                    w = parent->right;

                    if (w == NULL) {
                        break;
                    }
                }
                w->color = parent->color;
                parent->color = BLACK;
                if (w->right) {
                    w->right->color = BLACK;
                }
                rbset_left_rotate(set, parent);
                x = set->root;
                x_parent = NULL;
            }
        }
        else {
            SetNode* w = parent->left;
            if (w == NULL) {
                break;
            }
            if (w->color == RED) {
                w->color = BLACK;
                parent->color = RED;
                rbset_right_rotate(set, parent);
                w = parent->left;
                
                if (w == NULL) {
                    break;
                }
            }
            if ((w->right == NULL || w->right->color == BLACK) && (w->left == NULL || w->left->color == BLACK)) {
                w->color = RED;
                x = parent;
                x_parent = parent->parent;
            }
            else {
                if (w->left == NULL || w->left->color == BLACK) {
                    if (w->right) {
                        w->right->color = BLACK;
                    }
                    w->color = RED;
                    rbset_left_rotate(set, w);
                    w = parent->left;
                    
                    if (w == NULL) {
                        break;
                    }
                }

                w->color = parent->color;
                parent->color = BLACK;
                if (w->left) {
                    w->left->color = BLACK;
                }
                rbset_right_rotate(set, parent);
                x = set->root;
                x_parent = NULL;
            }
        }
    }
    if (x) {
        x->color = BLACK;
    }
}


/**
 * @brief Creates a new, empty ordered set.
 *
 * Every element later inserted is deep-copied into the set (the set owns
 * `elemSize` bytes per element and frees them automatically), and elements
 * are kept in the order defined by `compare`. The returned set must be
 * released with set_deallocate.
 *
 * @param elemSize Size in bytes of each stored element. Must be greater than 0.
 * @param compare  Comparator establishing a strict weak ordering. It receives
 *                 pointers to two elements and must return a value < 0, 0, or
 *                 > 0 (like `strcmp`). Must not be NULL.
 *
 * @return A pointer to the new, empty Set, or NULL if `elemSize` is 0, if
 *         `compare` is NULL, or if memory allocation fails.
 *
 * @note The library never calls exit(); allocation failure is reported by a
 *       NULL return.
 */
Set* set_create(size_t elemSize, SetCompareFunc compare) {
    SET_LOG("[set_create] Creating set with elemSize=%zu.", elemSize);
    if (elemSize == 0 || !compare) {
        SET_LOG("[set_create] Error: invalid elemSize (0) or NULL comparator.");
        return NULL;
    }

    Set* set = (Set*)malloc(sizeof(Set));
    if (!set) {
        SET_LOG("[set_create] Error: malloc failed for Set.");
        return NULL;
    }

    set->root     = NULL;
    set->compare  = compare;
    set->elemSize = elemSize;
    set->size     = 0;

    /* Configure the node pool. The element alignment is inferred from elemSize
     * (always safe), the slot holds the node header + the inline element, and a
     * slab targets ~64 KB (at least 16 slots). */
    {
        size_t ea = rbset_elem_align(elemSize);
        size_t slotAlign = ea < 8 ? 8 : ea;   /* node has pointers -> needs >= 8 */
        set->dataOffset = (sizeof(SetNode) + ea - 1) / ea * ea;
        set->slotSize   = (set->dataOffset + elemSize + slotAlign - 1) / slotAlign * slotAlign;
        set->slabCap    = 65536 / set->slotSize;
        if (set->slabCap < 16) {
            set->slabCap = 16;
        }
        set->slabs    = NULL;
        set->freeList = NULL;
    }

    SET_LOG("[set_create] Set created successfully at %p.", (void*)set);
    return set;
}


/**
 * @brief Creates an independent deep copy of a set.
 *
 * The copy has the same element size and comparator as `src` and contains
 * its own copies of every element, arranged in an identically balanced tree
 * (structurally cloned, so no rebalancing is performed). Modifying either set
 * afterwards never affects the other.
 *
 * @param src The set to copy. May be NULL.
 * @return A new Set equal to `src`, or NULL if `src` is NULL or allocation fails.
 */
Set* set_copy(const Set* src) {
    if (!src) {
        SET_LOG("[set_copy] Error: source set is NULL.");
        return NULL;
    }
    SET_LOG("[set_copy] Cloning set of size %zu.", src->size);

    Set* dst = set_create(src->elemSize, src->compare);
    if (!dst) {
        return NULL;
    }

    bool ok = true;
    dst->root = rbset_clone_subtree(dst, src->root, NULL, &ok);
    if (!ok) {
        SET_LOG("[set_copy] Error: allocation failed during clone; rolling back.");
        set_deallocate(dst);
        return NULL;
    }
    dst->size = src->size;

    SET_LOG("[set_copy] Copy complete (size %zu).", dst->size);
    return dst;
}


/**
 * @brief Frees every element and the set object itself.
 *
 * After this call the `set` pointer is invalid and must not be reused.
 *
 * @param set The set to destroy. Passing NULL is a safe no-op.
 */
void set_deallocate(Set* set) {
    if (!set) {
        SET_LOG("[set_deallocate] NULL set; nothing to do.");
        return;
    }
    SET_LOG("[set_deallocate] Freeing set of size %zu.", set->size);
    rbset_pool_destroy(set);   /* frees all node slabs in O(slabs) */
    free(set);
}


/**
 * @brief Removes all elements, leaving an empty but still-usable set.
 *
 * Frees every stored element; the set keeps its element size and comparator
 * so new elements can be inserted afterwards.
 *
 * @param set The set to clear. Passing NULL is a safe no-op.
 */
void set_clear(Set* set) {
    if (!set) {
        SET_LOG("[set_clear] NULL set; nothing to do.");
        return;
    }
    SET_LOG("[set_clear] Clearing %zu elements.", set->size);

    rbset_pool_destroy(set);   /* frees all node slabs in O(slabs) */
    set->root = NULL;
    set->size = 0;
}


/**
 * @brief Exchanges the contents of two sets in constant time.
 *
 * Only the internal bookkeeping (root, size, element size, comparator) is
 * swapped; no elements are copied. All existing iterators remain valid but
 * now refer to the other set.
 *
 * @param set1 First set. Must not be NULL.
 * @param set2 Second set. Must not be NULL.
 */
void set_swap(Set* set1, Set* set2) {
    if (!set1 || !set2) {
        SET_LOG("[set_swap] Error: one or both sets are NULL.");
        return;
    }
    SET_LOG("[set_swap] Swapping sets of size %zu and %zu.", set1->size, set2->size);
    Set tmp = *set1;
    *set1 = *set2;
    *set2 = tmp;
}


/**
 * @brief Returns the number of elements stored in the set.
 * @param set The set to query (NULL yields 0).
 * @return The element count, or 0 if `set` is NULL.
 */
size_t set_size(const Set* set) {
    return set ? set->size : 0;
}


/**
 * @brief Returns the size in bytes of each element the set stores.
 * @param set The set to query (NULL yields 0).
 * @return The per-element size, or 0 if `set` is NULL.
 */
size_t set_elem_size(const Set* set) {
    return set ? set->elemSize : 0;
}


/**
 * @brief Reports whether the set contains no elements.
 * @param set The set to query.
 * @return true if the set is empty or NULL; false otherwise.
 */
bool set_empty(const Set* set) {
    return set ? (set->size == 0) : true;
}


/**
 * @brief Returns a theoretical upper bound on the number of elements.
 *
 * This is `SIZE_MAX / (sizeof(node) + elemSize)` — the count the address space
 * could hold, not a hard cap. Actual capacity is limited by available memory.
 *
 * @param set The set to query (NULL yields 0).
 * @return The theoretical maximum element count.
 */
size_t set_max_size(const Set* set) {
    if (!set) {
        return 0;
    }
    return ((size_t)SIZE_MAX) / (sizeof(SetNode) + (set->elemSize ? set->elemSize : 1));
}


/**
 * @brief Inserts a copy of `element` if an equal element is not already present.
 *
 * Mirrors `std::set::insert`: a set holds at most one element per equivalence
 * class, so attempting to insert a duplicate leaves the set unchanged. On
 * success a deep copy of `element` is stored and the tree is rebalanced in
 * O(log n).
 *
 * @param set     The set to insert into. Must not be NULL.
 * @param element Pointer to the element to copy in. Must not be NULL.
 *
 * @return true if a new element was inserted; false if an equal element
 *         already existed, if an argument is NULL, or if allocation failed.
 */
bool set_insert(Set* set, const void* element) {
    if (!set || !element) {
        SET_LOG("[set_insert] Error: NULL set or element.");
        return false;
    }

    SetNode** curr = &set->root;
    SetNode*  parent = NULL;
    while (*curr) {
        parent = *curr;
        int cmp = set->compare(element, (*curr)->key);
        if (cmp == 0) {
            SET_LOG("[set_insert] Element already present; not inserted.");
            return false;        /* std::set: no duplicates, no replacement */
        }
        curr = (cmp < 0) ? &(*curr)->left : &(*curr)->right;
    }

    SetNode* newNode = rbset_create_node(set, element);
    if (!newNode) {
        SET_LOG("[set_insert] Error: node creation failed.");
        return false;
    }

    *curr = newNode;
    newNode->parent = parent;
    set->size++;
    rbset_insert_fixup(set, newNode);

    SET_LOG("[set_insert] Inserted; new size is %zu.", set->size);
    return true;
}


/**
 * @brief Removes the element equal to `element`, if present.
 *
 * Mirrors `std::set::erase(key)`: the matching element (and its owned copy) is
 * freed and the tree is rebalanced in O(log n). Absent elements are a no-op.
 *
 * @param set     The set to erase from. Must not be NULL.
 * @param element Pointer to the element to remove. Must not be NULL.
 *
 * @return true if an element was removed; false if it was not found or an
 *         argument is NULL.
 */
bool set_erase(Set* set, const void* element) {
    if (!set || !element || !set->root) {
        SET_LOG("[set_erase] NULL/empty set or NULL element; nothing to erase.");
        return false;
    }

    SetNode* z = rbset_find_node(set, element);
    if (!z) {
        SET_LOG("[set_erase] Element not found.");
        return false;
    }

    SetNode* y = z;
    int      y_original_color = y->color;
    SetNode* x;
    SetNode* x_parent;

    if (z->left == NULL) {
        x = z->right;
        x_parent = z->parent;
        rbset_transplant(set, z, z->right);
    }
    else if (z->right == NULL) {
        x = z->left;
        x_parent = z->parent;
        rbset_transplant(set, z, z->left);
    }
    else {
        y = rbset_minimum(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            x_parent = y;
            if (x) {
                x->parent = y;
            }
        }
        else {
            x_parent = y->parent;
            rbset_transplant(set, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbset_transplant(set, z, y);

        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    rbset_pool_free(set, z);   /* return the slot to the pool's free-list */

    if (y_original_color == BLACK) {
        rbset_erase_fixup(set, x, x_parent);
    }
    set->size--;

    SET_LOG("[set_erase] Erased; new size is %zu.", set->size);
    return true;
}


/**
 * @brief Tests membership of an element.
 * @param set     The set to search. NULL yields false.
 * @param element The element to look for. NULL yields false.
 * @return true if an equal element is present; false otherwise.
 */
bool set_contains(const Set* set, const void* element) {
    if (!set || !element) {
        return false;
    }

    return rbset_find_node(set, element) != NULL;
}


/**
 * @brief Counts occurrences of an element (0 or 1, since sets are unique).
 * @param set     The set to search.
 * @param element The element to look for.
 * @return 1 if present, 0 otherwise.
 */
size_t set_count(const Set* set, const void* element) {
    return set_contains(set, element) ? 1 : 0;
}


/**
 * @brief Finds an element and returns an iterator to it.
 * @param set     The set to search.
 * @param element The element to look for.
 * @return An iterator to the matching element, or the end iterator (node == NULL)
 *         if it is absent or an argument is NULL.
 */
SetIterator set_find(const Set* set, const void* element) {
    SetIterator it = {0};
    if (!set || !element) {
        return it;
    }

    it.node = rbset_find_node(set, element);
    SET_LOG("[set_find] %s.", it.node ? "found" : "not found");
    return it;
}


/**
 * @brief Returns an iterator to the first element not less than `element`.
 *
 * Equivalent to `std::set::lower_bound`: the first element `>=` `element` in
 * the ordering, or the end iterator if every element is smaller.
 *
 * @param set     The set to search.
 * @param element The bound to compare against.
 * @return An iterator to the lower bound, or the end iterator.
 */
SetIterator set_lower_bound(const Set* set, const void* element) {
    SetIterator it = {0};
    if (!set || !element) {
        return it;
    }

    SetNode* current = set->root;
    SetNode* last = NULL;
    while (current) {
        if (set->compare(current->key, element) >= 0) {
            last = current;
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    it.node = last;

    return it;
}


/**
 * @brief Returns an iterator to the first element greater than `element`.
 *
 * Equivalent to `std::set::upper_bound`: the first element `>` `element`, or
 * the end iterator if no element is greater.
 *
 * @param set     The set to search.
 * @param element The bound to compare against.
 * @return An iterator to the upper bound, or the end iterator.
 */
SetIterator set_upper_bound(const Set* set, const void* element) {
    SetIterator it = {0};
    if (!set || !element) {
        return it;
    }

    SetNode* current = set->root;
    SetNode* last = NULL;
    while (current) {
        if (set->compare(current->key, element) > 0) {
            last = current;
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    it.node = last;

    return it;
}


/**
 * @brief Returns the range `[lower_bound, upper_bound)` for an element.
 *
 * Because sets are unique the range spans at most one element: `first` equals
 * `second` when the element is absent, and they straddle the single matching
 * element when it is present.
 *
 * @param set     The set to search.
 * @param element The element to bracket.
 * @return A pair of iterators {lower_bound, upper_bound}.
 */
SetIteratorPair set_equal_range(const Set* set, const void* element) {
    SetIteratorPair pair = {{0}, {0}};
    pair.first  = set_lower_bound(set, element);
    pair.second = set_upper_bound(set, element);

    return pair;
}


/**
 * @brief Advances an iterator to the in-order successor.
 *
 * After the largest element the iterator becomes the end iterator
 * (`node == NULL`). NULL or end iterators are left unchanged.
 *
 * @param it The iterator to advance.
 */
void set_iterator_increment(SetIterator* it) {
    if (!it || !it->node) {
        return;
    }
    if (it->node->right) {
        it->node = rbset_minimum(it->node->right);
    }
    else {
        while (it->node->parent && it->node == it->node->parent->right) {
            it->node = it->node->parent;
        }
        it->node = it->node->parent;
    }
}


/**
 * @brief Steps an iterator back to the in-order predecessor.
 *
 * Pairs with set_rbegin for reverse traversal. Before the smallest element the
 * iterator becomes the end iterator (`node == NULL`).
 *
 * @param it The iterator to move back.
 */
void set_iterator_decrement(SetIterator* it) {
    if (!it || !it->node) {
        return;
    }
    if (it->node->left) {
        it->node = rbset_maximum(it->node->left);
    }
    else {
        while (it->node->parent && it->node == it->node->parent->left) {
            it->node = it->node->parent;
        }
        it->node = it->node->parent;
    }
}

/**
 * @brief Tests whether two iterators refer to the same position.
 * @param it1 First iterator (NULL yields false).
 * @param it2 Second iterator (NULL yields false).
 * @return true if both refer to the same node (or both are the end iterator).
 */
bool set_iterator_equal(const SetIterator* it1, const SetIterator* it2) {
    if (!it1 || !it2) {
        return false;
    }

    return it1->node == it2->node;
}

/**
 * @brief Returns a read-only pointer to the element at the iterator.
 *
 * @param it The iterator to dereference.
 * @return A `const void*` to the current element, or NULL at the end iterator.
 *
 * @warning Treat the element as immutable. Mutating it in place can violate the
 *          ordering invariant; to "change" an element, erase it and insert the
 *          new value (this matches std::set, whose elements are const).
 */
const void* set_iterator_get(const SetIterator* it) {
    if (!it || !it->node) {
        return NULL;
    }
    return it->node->key;
}


/**
 * @brief Returns an iterator to the first (smallest) element.
 * @param set The set (NULL yields the end iterator).
 * @return An iterator to the smallest element, or the end iterator if empty.
 */
SetIterator set_begin(const Set* set) {
    SetIterator it = {0};
    if (set) {
        it.node = rbset_minimum(set->root);
    }
    return it;
}


/**
 * @brief Returns the past-the-end iterator (`node == NULL`).
 * @param set The set (unused; accepted for symmetry).
 * @return The end iterator.
 */
SetIterator set_end(const Set* set) {
    (void)set;
    return (SetIterator){0};
}


/**
 * @brief Returns an iterator to the last (largest) element, for reverse walks.
 *
 * Walk backwards with set_iterator_decrement until the iterator equals set_rend.
 *
 * @param set The set (NULL yields the rend iterator).
 * @return An iterator to the largest element, or the rend iterator if empty.
 */
SetIterator set_rbegin(const Set* set) {
    SetIterator it = {0};
    if (set) {
        it.node = rbset_maximum(set->root);
    }
    return it;
}


/**
 * @brief Returns the before-the-beginning iterator for reverse walks (`node == NULL`).
 * @param set The set (unused; accepted for symmetry).
 * @return The rend iterator.
 */
SetIterator set_rend(const Set* set) {
    (void)set;
    return (SetIterator){0};
}

/** @brief Const alias of set_begin. */
SetIterator set_cbegin(const Set* set)  {
    return set_begin(set);
}


/** @brief Const alias of set_end. */
SetIterator set_cend(const Set* set)    {
    return set_end(set);
}


/** @brief Const alias of set_rbegin. */
SetIterator set_crbegin(const Set* set) {
    return set_rbegin(set);
}


/** @brief Const alias of set_rend. */
SetIterator set_crend(const Set* set)   {
    return set_rend(set);
}


/** @brief Internal: true when both sets are non-NULL and share an element size. */
static bool rbset_compatible(const Set* a, const Set* b) {
    return a && b && a->elemSize == b->elemSize;
}

/**
 * @brief Returns a new set containing every element of `a` or `b` (the union).
 *
 * The result uses `a`'s comparator and contains deep copies of the elements.
 * The caller owns the returned set and must set_deallocate it.
 *
 * @param a First operand. @param b Second operand.
 * @return A new set `a ∪ b`, or NULL if the operands are NULL or have
 *         different element sizes, or on allocation failure.
 */
Set* set_union(const Set* a, const Set* b) {
    if (!rbset_compatible(a, b)) {
        SET_LOG("[set_union] Error: incompatible or NULL operands.");
        return NULL;
    }

    Set* result = set_copy(a);
    if (!result) {
        return NULL;
    }

    for (SetIterator it = set_begin(b); it.node; set_iterator_increment(&it)) {
        set_insert(result, it.node->key);
    }
    return result;
}

/**
 * @brief Returns a new set of the elements present in both `a` and `b`.
 *
 * Iterates the smaller operand and probes the larger, so the cost is
 * O(min(|a|,|b|) · log max(|a|,|b|)). The caller owns the result.
 *
 * @param a First operand. @param b Second operand.
 * @return A new set `a ∩ b`, or NULL on incompatible/NULL operands or
 *         allocation failure.
 */
Set* set_intersection(const Set* a, const Set* b) {
    if (!rbset_compatible(a, b)) {
        SET_LOG("[set_intersection] Error: incompatible or NULL operands.");
        return NULL;
    }

    Set* result = set_create(a->elemSize, a->compare);
    if (!result) {
        return NULL;
    }

    const Set* small = (a->size <= b->size) ? a : b;
    const Set* other = (small == a) ? b : a;
    for (SetIterator it = set_begin(small); it.node; set_iterator_increment(&it)) {
        if (set_contains(other, it.node->key)) {
            set_insert(result, it.node->key);
        }
    }
    return result;
}


/**
 * @brief Returns a new set of the elements in `a` but not in `b` (`a \ b`).
 * @param a First operand. @param b Second operand.
 * @return A new set, or NULL on incompatible/NULL operands or allocation failure.
 *         The caller owns the result.
 */
Set* set_difference(const Set* a, const Set* b) {
    if (!rbset_compatible(a, b)) {
        SET_LOG("[set_difference] Error: incompatible or NULL operands.");
        return NULL;
    }

    Set* result = set_create(a->elemSize, a->compare);
    if (!result) {
        return NULL;
    }
    for (SetIterator it = set_begin(a); it.node; set_iterator_increment(&it)) {
        if (!set_contains(b, it.node->key)) {
            set_insert(result, it.node->key);
        }
    }

    return result;
}


/**
 * @brief Returns a new set of the elements in exactly one of `a` or `b`.
 *
 * The symmetric difference `(a \ b) ∪ (b \ a)`. The caller owns the result.
 *
 * @param a First operand. @param b Second operand.
 * @return A new set, or NULL on incompatible/NULL operands or allocation failure.
 */
Set* set_symmetric_difference(const Set* a, const Set* b) {
    if (!rbset_compatible(a, b)) {
        SET_LOG("[set_symmetric_difference] Error: incompatible or NULL operands.");
        return NULL;
    }
    Set* result = set_create(a->elemSize, a->compare);
    if (!result) {
        return NULL;
    }
    for (SetIterator it = set_begin(a); it.node; set_iterator_increment(&it)) {
        if (!set_contains(b, it.node->key)) {
            set_insert(result, it.node->key);
        }
    }
    for (SetIterator it = set_begin(b); it.node; set_iterator_increment(&it)) {
        if (!set_contains(a, it.node->key)) {
            set_insert(result, it.node->key);
        }
    }
    return result;
}


/**
 * @brief Reports whether every element of `a` is also in `b` (`a ⊆ b`).
 * @param a Candidate subset. @param b Candidate superset.
 * @return true if `a` is a subset of `b`; false otherwise or if an operand is NULL.
 */
bool set_is_subset(const Set* a, const Set* b) {
    if (!a || !b) {
        return false;
    }
    if (a->size > b->size) {
        return false;
    }
    for (SetIterator it = set_begin(a); it.node; set_iterator_increment(&it)) {
        if (!set_contains(b, it.node->key)) {
            return false;
        }
    }
    return true;
}


/**
 * @brief Reports whether every element of `b` is also in `a` (`a ⊇ b`).
 * @param a Candidate superset. @param b Candidate subset.
 * @return true if `a` is a superset of `b`; false otherwise.
 */
bool set_is_superset(const Set* a, const Set* b) {
    return set_is_subset(b, a);
}


/**
 * @brief Reports whether `a` and `b` share no elements.
 * @param a First operand. @param b Second operand.
 * @return true if the sets are disjoint; false if they intersect or an operand is NULL.
 */
bool set_is_disjoint(const Set* a, const Set* b) {
    if (!a || !b) {
        return false;
    }

    const Set* small = (a->size <= b->size) ? a : b;
    const Set* other = (small == a) ? b : a;
    for (SetIterator it = set_begin(small); it.node; set_iterator_increment(&it)) {
        if (set_contains(other, it.node->key)) {
            return false;
        }
    }

    return true;
}


/**
 * @brief Internal: three-way lexicographic comparison of the sorted sequences.
 * @return -1 if `a` < `b`, 1 if `a` > `b`, 0 if equal. Uses `a`'s comparator.
 */
static int rbset_lex_compare(const Set* a, const Set* b) {
    SetIterator ia = set_begin(a);
    SetIterator ib = set_begin(b);

    while (ia.node && ib.node) {
        int c = a->compare(ia.node->key, ib.node->key);
        if (c != 0) {
            return c < 0 ? -1 : 1;
        }
        set_iterator_increment(&ia);
        set_iterator_increment(&ib);
    }

    if (ia.node) {
        return 1;          /* a has extra trailing elements -> a > b */
    }
    if (ib.node) {
        return -1;
    }

    return 0;
}


/**
 * @brief Reports whether two sets contain exactly the same elements.
 * @param set1 First set. @param set2 Second set.
 * @return true if equal; false otherwise or if an operand is NULL.
 */
bool set_is_equal(const Set* set1, const Set* set2) {
    if (!set1 || !set2) {
        return false;
    }
    if (set1->size != set2->size) {
        return false;
    }

    return rbset_lex_compare(set1, set2) == 0;
}


/**
 * @brief Logical negation of set_is_equal (for two non-NULL sets).
 * @param set1 First set. @param set2 Second set.
 * @return true if the sets differ; false if equal or an operand is NULL.
 */
bool set_is_not_equal(const Set* set1, const Set* set2) {
    if (!set1 || !set2) {
        return false;
    }
    return !set_is_equal(set1, set2);
}


/**
 * @brief Lexicographic less-than over the sorted elements (like std::set's `<`).
 * @param set1 Left operand. @param set2 Right operand.
 * @return true if `set1` < `set2`; false otherwise or if an operand is NULL.
 */
bool set_is_less(const Set* set1, const Set* set2) {
    if (!set1 || !set2) {
        return false;
    }
    return rbset_lex_compare(set1, set2) < 0;
}


/**
 * @brief Lexicographic less-than-or-equal over the sorted elements.
 * @param set1 Left operand. @param set2 Right operand.
 * @return true if `set1` <= `set2`; false otherwise or if an operand is NULL.
 */
bool set_is_less_or_equal(const Set* set1, const Set* set2) {
    if (!set1 || !set2) {
        return false;
    }
    return rbset_lex_compare(set1, set2) <= 0;
}


/**
 * @brief Lexicographic greater-than over the sorted elements.
 * @param set1 Left operand. @param set2 Right operand.
 * @return true if `set1` > `set2`; false otherwise or if an operand is NULL.
 */
bool set_is_greater(const Set* set1, const Set* set2) {
    if (!set1 || !set2) {
        return false;
    }
    return rbset_lex_compare(set1, set2) > 0;
}


/**
 * @brief Lexicographic greater-than-or-equal over the sorted elements.
 * @param set1 Left operand. @param set2 Right operand.
 * @return true if `set1` >= `set2`; false otherwise or if an operand is NULL.
 */
bool set_is_greater_or_equal(const Set* set1, const Set* set2) {
    if (!set1 || !set2) {
        return false;
    }
    return rbset_lex_compare(set1, set2) >= 0;
}


/**
 * @brief Returns the comparator the set was created with.
 * @param set The set to query (NULL yields NULL).
 * @return The comparator function pointer, or NULL if `set` is NULL.
 */
SetCompareFunc set_key_comp(const Set* set) {
    return set ? set->compare : NULL;
}


/** @brief Internal: in-order traversal invoking `printElem` on each element. */
static void rbset_print_subtree(const SetNode* node, void (*printElem)(const void*)) {
    if (!node) {
        return;
    }

    rbset_print_subtree(node->left, printElem);
    printElem(node->key);
    rbset_print_subtree(node->right, printElem);
}


/**
 * @brief Calls `printElem` on each element in ascending order (debug helper).
 * @param set       The set to print.
 * @param printElem Callback invoked with a pointer to each element.
 */
void set_print(const Set* set, void (*printElem)(const void*)) {
    if (!set || !printElem) {
        return;
    }
    rbset_print_subtree(set->root, printElem);
}
