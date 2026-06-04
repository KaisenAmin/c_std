/**
 * @author Amin Tahmasebi
 * @date 2026
 * @class Set
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in set.c (file-level convention).
 *
 * Red-black tree based ordered set, modeled on C++ std::set<T>.
 *
 * Unlike `map` (which stores opaque caller-owned pointers), `set` has
 * VALUE SEMANTICS like the sequence containers (vector / list): each
 * element is deep-copied (`elemSize` bytes) into the set on insert, the
 * set owns that copy, and it is freed on erase / clear / deallocate.
 * Elements are kept ordered by the user comparator; lookups, insertions
 * and deletions are O(log n).
 *
 * As in std::set, the stored elements are logically const — never modify
 * an element in place through an iterator, as that would break the
 * ordering invariant. (If the element type itself owns heap memory, the
 * set copies the bytes only; you remain responsible for that memory,
 * exactly like vector / list.)
 */

 
#ifndef SET_H_
#define SET_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define SET_LOGGING_ENABLE */

#ifdef SET_LOGGING_ENABLE
    #define SET_LOG(fmt, ...) \
        do { fprintf(stderr, "[SET LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SET_LOG(...) do { } while (0)
#endif


typedef struct Set     Set;
typedef struct SetNode SetNode;

typedef int (*SetCompareFunc)(const void* a, const void* b);

struct SetNode {
    void*           key;     /* owned deep copy of the element (elemSize bytes) */
    struct SetNode* left;
    struct SetNode* right;
    struct SetNode* parent;
    int             color;
};


typedef struct SetIterator {
    SetNode* node;           /* NULL == past-the-end (end / rend) sentinel */
} SetIterator;


typedef struct SetIteratorPair {
    SetIterator first;
    SetIterator second;
} SetIteratorPair;


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Set*            set_create                  (size_t elemSize, SetCompareFunc compare);
Set*            set_copy                    (const Set* src);
void            set_deallocate              (Set* set);
void            set_clear                   (Set* set);
void            set_swap                    (Set* set1, Set* set2);


/* ------------------------------------------------------------------ */
/* Capacity                                                           */
/* ------------------------------------------------------------------ */

size_t          set_size                    (const Set* set);
size_t          set_max_size                (const Set* set);
size_t          set_elem_size               (const Set* set);
bool            set_empty                   (const Set* set);


/* ------------------------------------------------------------------ */
/* Modifiers                                                          */
/* ------------------------------------------------------------------ */

bool            set_insert                  (Set* set, const void* element);
bool            set_erase                   (Set* set, const void* element);


/* ------------------------------------------------------------------ */
/* Lookup                                                             */
/* ------------------------------------------------------------------ */

bool            set_contains                (const Set* set, const void* element);
size_t          set_count                   (const Set* set, const void* element);
SetIterator     set_find                    (const Set* set, const void* element);
SetIterator     set_lower_bound             (const Set* set, const void* element);
SetIterator     set_upper_bound             (const Set* set, const void* element);
SetIteratorPair set_equal_range             (const Set* set, const void* element);


/* ------------------------------------------------------------------ */
/* Iterators — mutating                                               */
/* ------------------------------------------------------------------ */

SetIterator     set_begin                   (const Set* set);
SetIterator     set_end                     (const Set* set);
SetIterator     set_rbegin                  (const Set* set);
SetIterator     set_rend                    (const Set* set);


/* ------------------------------------------------------------------ */
/* Iterators — const                                                  */
/* ------------------------------------------------------------------ */

SetIterator     set_cbegin                  (const Set* set);
SetIterator     set_cend                    (const Set* set);
SetIterator     set_crbegin                 (const Set* set);
SetIterator     set_crend                   (const Set* set);


/* ------------------------------------------------------------------ */
/* Iterator operations                                                */
/* ------------------------------------------------------------------ */

void            set_iterator_increment      (SetIterator* it);
void            set_iterator_decrement      (SetIterator* it);
bool            set_iterator_equal          (const SetIterator* it1, const SetIterator* it2);
const void*     set_iterator_get            (const SetIterator* it);


/* ------------------------------------------------------------------ */
/* Set algebra                                                        */
/* ------------------------------------------------------------------ */

Set*            set_union                   (const Set* a, const Set* b);
Set*            set_intersection            (const Set* a, const Set* b);
Set*            set_difference              (const Set* a, const Set* b);
Set*            set_symmetric_difference    (const Set* a, const Set* b);
bool            set_is_subset               (const Set* a, const Set* b);
bool            set_is_superset             (const Set* a, const Set* b);
bool            set_is_disjoint             (const Set* a, const Set* b);


/* ------------------------------------------------------------------ */
/* Comparison (lexicographic over the sorted elements, like std::set)  */
/* ------------------------------------------------------------------ */

bool            set_is_equal                (const Set* set1, const Set* set2);
bool            set_is_not_equal            (const Set* set1, const Set* set2);
bool            set_is_less                 (const Set* set1, const Set* set2);
bool            set_is_less_or_equal        (const Set* set1, const Set* set2);
bool            set_is_greater              (const Set* set1, const Set* set2);
bool            set_is_greater_or_equal     (const Set* set1, const Set* set2);


/* ------------------------------------------------------------------ */
/* Observers / debug                                                  */
/* ------------------------------------------------------------------ */

SetCompareFunc  set_key_comp                (const Set* set);
void            set_print                   (const Set* set, void (*printElem)(const void*));


#ifdef __cplusplus
}
#endif

#endif
