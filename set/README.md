# Set Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2026
**License:** ISC License

## Overview

The Set library provides a generic, ordered **set** container modeled on C++ `std::set<T>`. It is implemented as a **Red-Black Tree** (a self-balancing binary search tree), so insertion, deletion, lookup, and bound queries are all **O(log n)**, and in-order iteration visits elements in sorted order.

Unlike the `map` module — which stores opaque, caller-owned pointers — `set` has **value semantics** like the sequence containers (`vector`, `list`): each element is **deep-copied** (`elemSize` bytes) into the tree and **owned by the set**. The copy is freed automatically on `set_erase`, `set_clear`, and `set_deallocate`. You only manage memory that the element type *itself* points to (exactly like `vector`/`list`).

## Features

- **Ordered & unique:** elements are kept sorted by your comparator; duplicates are rejected (like `std::set`).
- **Red-Black Tree:** guaranteed O(log n) insert / erase / find / `lower_bound` / `upper_bound`.
- **Value semantics:** the set owns deep copies of your elements — no dangling references.
- **Pooled — scales to 10M+ elements:** nodes are carved from large slabs (a node *pool* with a free-list), so building a big set makes **far fewer than one `malloc` per element**, tear-down is O(slabs) (not one `free` per node), and erased slots are recycled. The element is stored *inline* in its node, and the per-element footprint matches a typed container (element alignment is inferred from `elemSize`, so a generic `void*` set wastes no space). Building **10 million** `int`s and destroying them takes ~2 s and ~470 MB — comparable to `std::set`. Proven by `tests/test_set_memory.c`.
- **Bidirectional iterators:** forward (`begin`/`end`) and reverse (`rbegin`/`rend`), with `increment`/`decrement`.
- **Set algebra:** `union`, `intersection`, `difference`, `symmetric_difference`, plus `is_subset` / `is_superset` / `is_disjoint`.
- **Lexicographic comparison:** the full `is_equal` / `is_less` / … family, like `std::set`'s relational operators.
- **Cross-platform & leak-free:** pure C17, no third-party dependencies; verified under Valgrind on Linux and MinGW/MSVC on Windows.

## Usage

Include `set.h` and compile `set.c` with your program. The module has **no other c_std dependencies**:

```bash
gcc -std=c17 -O2 -Wall -Wextra -o my_app my_app.c set/set.c -I.
```

A comparator is required at creation. It must return a negative value, zero, or a positive value (like `strcmp`) and define a strict weak ordering:

```c
int compare_ints(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}
```

## Set Structures

### `Set`
Opaque handle to an ordered set. Created with `set_create` and released with `set_deallocate`.

### `SetNode`
A node in the underlying Red-Black Tree.
- **Fields:**
  - `void* key`: pointer to the owned, deep-copied element (`elemSize` bytes), stored **inline** in the same allocation as the node.
  - `struct SetNode* left` / `right` / `parent`: tree links.
  - `int color`: `RED`/`BLACK`, used for balancing.

### `SetIterator`
Traverses the set in sorted order.
- **Fields:**
  - `SetNode* node`: the current node, or `NULL` for the past-the-end (`end`/`rend`) sentinel.

### `SetIteratorPair`
Holds two iterators, returned by `set_equal_range`.
- **Fields:** `SetIterator first`, `SetIterator second`.

### `SetCompareFunc`
`typedef int (*SetCompareFunc)(const void* a, const void* b);` — orders two elements.

## Function Descriptions

### `Set* set_create(size_t elemSize, SetCompareFunc compare)`

**Purpose**: Initializes and returns a new, empty ordered set whose elements are each `elemSize` bytes and are kept sorted by the supplied comparator.

**Parameters**:
- `elemSize`: The size in bytes of every element the set will store (must be greater than 0).
- `compare`: A comparator returning a value `< 0`, `0`, or `> 0` (like `strcmp`) that defines a strict weak ordering over two elements.

**Return Value**: A pointer to the newly created set. Returns `NULL` if `elemSize` is 0, if `compare` is `NULL`, or if memory allocation fails.

**Usage Case**: Use first, to create a typed, ordered, duplicate-free container before inserting elements.

---

### `Set* set_copy(const Set* src)`

**Purpose**: Produces an independent, deep copy of an existing set — the same elements, comparator, and element size, in an identically balanced tree.

**Parameters**:
- `src`: The set to copy.

**Return Value**: A pointer to a new set equal to `src`. Returns `NULL` if `src` is `NULL` or memory allocation fails.

**Usage Case**: Use to snapshot a set before mutating it, or to hand an owned copy to another part of the program.

---

### `void set_deallocate(Set* set)`

**Purpose**: Frees every stored element and the set object itself.

**Parameters**:
- `set`: The set to destroy.

**Return Value**: None.

**Usage Case**: Use once you are finished with a set to release all of its memory. Safe to call with `NULL`.

---

### `void set_clear(Set* set)`

**Purpose**: Removes and frees all elements while leaving the set valid and reusable.

**Parameters**:
- `set`: The set to empty.

**Return Value**: None.

**Usage Case**: Use to reset a set for reuse without destroying and recreating it.

---

### `void set_swap(Set* set1, Set* set2)`

**Purpose**: Exchanges the entire contents of two sets in constant time.

**Parameters**:
- `set1`: The first set.
- `set2`: The second set.

**Return Value**: None.

**Usage Case**: Use for an O(1) hand-off of contents, such as the copy-and-swap idiom.

---

### `size_t set_size(const Set* set)`

**Purpose**: Reports how many elements the set currently holds.

**Parameters**:
- `set`: The set to query.

**Return Value**: The number of elements, or `0` if `set` is `NULL`.

**Usage Case**: Use to read the cardinality of the set.

---

### `bool set_empty(const Set* set)`

**Purpose**: Reports whether the set contains no elements.

**Parameters**:
- `set`: The set to query.

**Return Value**: `true` if the set is empty (or `NULL`), otherwise `false`.

**Usage Case**: Use as a guard before iterating or removing elements.

---

### `size_t set_elem_size(const Set* set)`

**Purpose**: Returns the per-element size the set was created with.

**Parameters**:
- `set`: The set to query.

**Return Value**: The element size in bytes, or `0` if `set` is `NULL`.

**Usage Case**: Use when generic code needs to know the stored element width.

---

### `size_t set_max_size(const Set* set)`

**Purpose**: Returns a theoretical upper bound on the number of elements the set could hold.

**Parameters**:
- `set`: The set to query.

**Return Value**: The theoretical maximum element count, or `0` if `set` is `NULL`.

**Usage Case**: Use for capacity diagnostics; the real limit is the available memory.

---

### `bool set_insert(Set* set, const void* element)`

**Purpose**: Inserts a deep copy of `element` if an equal element is not already present (a set holds no duplicates).

**Parameters**:
- `set`: The destination set.
- `element`: A pointer to the element to copy in.

**Return Value**: `true` if a new element was inserted; `false` if an equal element already existed, if an argument is `NULL`, or if allocation failed.

**Usage Case**: Use to add elements while automatically keeping them unique and sorted.

---

### `bool set_erase(Set* set, const void* element)`

**Purpose**: Removes (and frees) the element equal to `element`, if it is present.

**Parameters**:
- `set`: The set to modify.
- `element`: A pointer to the element to remove.

**Return Value**: `true` if an element was removed; `false` if it was not found or an argument is `NULL`.

**Usage Case**: Use to delete a specific value from the set.

---

### `bool set_contains(const Set* set, const void* element)`

**Purpose**: Tests whether an element equal to `element` exists in the set.

**Parameters**:
- `set`: The set to search.
- `element`: A pointer to the element to look for.

**Return Value**: `true` if the element is present, otherwise `false`.

**Usage Case**: Use for fast O(log n) membership checks.

---

### `size_t set_count(const Set* set, const void* element)`

**Purpose**: Counts how many times an element appears (always 0 or 1 for a set).

**Parameters**:
- `set`: The set to search.
- `element`: A pointer to the element to look for.

**Return Value**: `1` if the element is present, `0` otherwise.

**Usage Case**: Use as the STL-compatible membership count.

---

### `SetIterator set_find(const Set* set, const void* element)`

**Purpose**: Locates an element and returns an iterator positioned at it.

**Parameters**:
- `set`: The set to search.
- `element`: A pointer to the element to find.

**Return Value**: An iterator to the element, or the end iterator (`.node == NULL`) if it is absent.

**Usage Case**: Use when you need the element's position, e.g. to iterate onward from it.

---

### `SetIterator set_lower_bound(const Set* set, const void* element)`

**Purpose**: Finds the first element that is not less than `element`.

**Parameters**:
- `set`: The set to search.
- `element`: The bound to compare against.

**Return Value**: An iterator to the first element `>=` `element`, or the end iterator if no element qualifies.

**Usage Case**: Use as the inclusive lower edge of a range query.

---

### `SetIterator set_upper_bound(const Set* set, const void* element)`

**Purpose**: Finds the first element that is strictly greater than `element`.

**Parameters**:
- `set`: The set to search.
- `element`: The bound to compare against.

**Return Value**: An iterator to the first element `>` `element`, or the end iterator if no element qualifies.

**Usage Case**: Use as the exclusive upper edge of a range query.

---

### `SetIteratorPair set_equal_range(const Set* set, const void* element)`

**Purpose**: Returns the half-open range `[lower_bound, upper_bound)` that brackets `element`.

**Parameters**:
- `set`: The set to search.
- `element`: The element to bracket.

**Return Value**: A pair whose `first` is the lower bound and `second` the upper bound (the two are equal when the element is absent).

**Usage Case**: Use to obtain both bounds of an element in a single call.

---

### `SetIterator set_begin(const Set* set)`

**Purpose**: Returns an iterator to the first (smallest) element.

**Parameters**:
- `set`: The set to iterate.

**Return Value**: An iterator to the smallest element, or the end iterator if the set is empty.

**Usage Case**: Use to start a forward, ascending traversal.

---

### `SetIterator set_end(const Set* set)`

**Purpose**: Returns the past-the-end sentinel iterator.

**Parameters**:
- `set`: The set (accepted for symmetry).

**Return Value**: The end iterator (`.node == NULL`).

**Usage Case**: Use as the loop terminator when comparing iterators.

---

### `SetIterator set_rbegin(const Set* set)`

**Purpose**: Returns an iterator to the last (largest) element, for reverse traversal.

**Parameters**:
- `set`: The set to iterate.

**Return Value**: An iterator to the largest element, or the rend iterator if the set is empty.

**Usage Case**: Use with `set_iterator_decrement` to walk the set in descending order.

---

### `SetIterator set_rend(const Set* set)`

**Purpose**: Returns the before-the-beginning sentinel iterator for reverse traversal.

**Parameters**:
- `set`: The set (accepted for symmetry).

**Return Value**: The rend iterator (`.node == NULL`).

**Usage Case**: Use as the terminator of a reverse loop.

---

### `SetIterator set_cbegin / set_cend / set_crbegin / set_crend (const Set* set)`

**Purpose**: Const-named aliases of `set_begin`, `set_end`, `set_rbegin`, and `set_rend`.

**Parameters**:
- `set`: The set to iterate.

**Return Value**: The same iterator the corresponding non-`c` function returns.

**Usage Case**: Use for readability when the intent is read-only traversal.

---

### `void set_iterator_increment(SetIterator* it)`

**Purpose**: Advances an iterator to the next-larger element (the in-order successor).

**Parameters**:
- `it`: The iterator to advance; it becomes the end iterator after the last element.

**Return Value**: None.

**Usage Case**: Use to step forward during ascending iteration.

---

### `void set_iterator_decrement(SetIterator* it)`

**Purpose**: Moves an iterator to the next-smaller element (the in-order predecessor).

**Parameters**:
- `it`: The iterator to move back.

**Return Value**: None.

**Usage Case**: Use with `set_rbegin` to iterate in descending order.

---

### `bool set_iterator_equal(const SetIterator* it1, const SetIterator* it2)`

**Purpose**: Tests whether two iterators refer to the same position.

**Parameters**:
- `it1`: The first iterator.
- `it2`: The second iterator.

**Return Value**: `true` if both refer to the same node (or both are the end iterator), otherwise `false`.

**Usage Case**: Use to detect when a traversal has reached `set_end`.

---

### `const void* set_iterator_get(const SetIterator* it)`

**Purpose**: Returns a read-only pointer to the element the iterator currently refers to.

**Parameters**:
- `it`: The iterator to dereference.

**Return Value**: A `const void*` to the current element, or `NULL` at the end iterator.

**Usage Case**: Use to read a value during iteration. Never modify it in place — that would break the set's ordering.

---

### `Set* set_union(const Set* a, const Set* b)`

**Purpose**: Builds a new set containing every element present in `a` or `b`.

**Parameters**:
- `a`: The first operand set.
- `b`: The second operand set (must share `a`'s element size).

**Return Value**: A new, caller-owned set `a ∪ b`. Returns `NULL` if the operands are `NULL`/incompatible or on allocation failure.

**Usage Case**: Use to merge two sets; free the result with `set_deallocate`.

---

### `Set* set_intersection(const Set* a, const Set* b)`

**Purpose**: Builds a new set of the elements common to both `a` and `b`.

**Parameters**:
- `a`: The first operand set.
- `b`: The second operand set (must share `a`'s element size).

**Return Value**: A new, caller-owned set `a ∩ b`. Returns `NULL` if the operands are `NULL`/incompatible or on allocation failure.

**Usage Case**: Use to find the shared elements of two sets; free the result with `set_deallocate`.

---

### `Set* set_difference(const Set* a, const Set* b)`

**Purpose**: Builds a new set of the elements that are in `a` but not in `b`.

**Parameters**:
- `a`: The minuend set.
- `b`: The subtrahend set (must share `a`'s element size).

**Return Value**: A new, caller-owned set `a \ b`. Returns `NULL` if the operands are `NULL`/incompatible or on allocation failure.

**Usage Case**: Use to subtract one set from another; free the result with `set_deallocate`.

---

### `Set* set_symmetric_difference(const Set* a, const Set* b)`

**Purpose**: Builds a new set of the elements that are in exactly one of `a` or `b`.

**Parameters**:
- `a`: The first operand set.
- `b`: The second operand set (must share `a`'s element size).

**Return Value**: A new, caller-owned set `(a \ b) ∪ (b \ a)`. Returns `NULL` if the operands are `NULL`/incompatible or on allocation failure.

**Usage Case**: Use to find the elements that are not shared; free the result with `set_deallocate`.

---

### `bool set_is_subset(const Set* a, const Set* b)`

**Purpose**: Tests whether every element of `a` is also in `b`.

**Parameters**:
- `a`: The candidate subset.
- `b`: The candidate superset.

**Return Value**: `true` if `a ⊆ b`, otherwise `false` (or if an operand is `NULL`).

**Usage Case**: Use to check containment relationships between sets.

---

### `bool set_is_superset(const Set* a, const Set* b)`

**Purpose**: Tests whether `a` contains every element of `b`.

**Parameters**:
- `a`: The candidate superset.
- `b`: The candidate subset.

**Return Value**: `true` if `a ⊇ b`, otherwise `false`.

**Usage Case**: Use as the inverse of `set_is_subset`.

---

### `bool set_is_disjoint(const Set* a, const Set* b)`

**Purpose**: Tests whether `a` and `b` share no elements.

**Parameters**:
- `a`: The first set.
- `b`: The second set.

**Return Value**: `true` if the sets are disjoint, `false` if they intersect (or an operand is `NULL`).

**Usage Case**: Use to confirm two sets have nothing in common.

---

### `bool set_is_equal(const Set* set1, const Set* set2)`

**Purpose**: Tests whether two sets contain exactly the same elements.

**Parameters**:
- `set1`: The first set.
- `set2`: The second set.

**Return Value**: `true` if the sets are equal, otherwise `false` (or if an operand is `NULL`).

**Usage Case**: Use to compare two sets for content equality.

---

### `bool set_is_not_equal(const Set* set1, const Set* set2)`

**Purpose**: The logical negation of `set_is_equal` for two valid sets.

**Parameters**:
- `set1`: The first set.
- `set2`: The second set.

**Return Value**: `true` if the sets differ, otherwise `false`.

**Usage Case**: Use to detect any difference in contents.

---

### `bool set_is_less(const Set* set1, const Set* set2)`

**Purpose**: Lexicographically compares the sorted elements and reports whether `set1 < set2`.

**Parameters**:
- `set1`: The left operand.
- `set2`: The right operand.

**Return Value**: `true` if `set1` orders before `set2`, otherwise `false`.

**Usage Case**: Use to order sets (e.g. as keys), like `std::set`'s `operator<`.

---

### `bool set_is_less_or_equal(const Set* set1, const Set* set2)`

**Purpose**: Lexicographic `set1 <= set2` over the sorted elements.

**Parameters**:
- `set1`: The left operand.
- `set2`: The right operand.

**Return Value**: `true` if `set1` orders before or equal to `set2`, otherwise `false`.

**Usage Case**: Use for inclusive ordering comparisons.

---

### `bool set_is_greater(const Set* set1, const Set* set2)`

**Purpose**: Lexicographic `set1 > set2` over the sorted elements.

**Parameters**:
- `set1`: The left operand.
- `set2`: The right operand.

**Return Value**: `true` if `set1` orders after `set2`, otherwise `false`.

**Usage Case**: Use as the inverse of `set_is_less`.

---

### `bool set_is_greater_or_equal(const Set* set1, const Set* set2)`

**Purpose**: Lexicographic `set1 >= set2` over the sorted elements.

**Parameters**:
- `set1`: The left operand.
- `set2`: The right operand.

**Return Value**: `true` if `set1` orders after or equal to `set2`, otherwise `false`.

**Usage Case**: Use for inclusive ordering comparisons.

---

### `SetCompareFunc set_key_comp(const Set* set)`

**Purpose**: Returns the comparator the set was created with.

**Parameters**:
- `set`: The set to query.

**Return Value**: The comparator function pointer, or `NULL` if `set` is `NULL`.

**Usage Case**: Use when generic code needs the set's ordering function.

---

### `void set_print(const Set* set, void (*printElem)(const void*))`

**Purpose**: Calls `printElem` on each element in ascending order — a small debugging helper.

**Parameters**:
- `set`: The set to print.
- `printElem`: A callback invoked with a pointer to each element.

**Return Value**: None.

**Usage Case**: Use during debugging to dump a set's contents in order.

---


## Notes

- **Ownership:** the set copies element *bytes*. For a set of strings, store `char*` (the pointer is copied by value) and compare with `strcmp` inside your comparator; you remain the owner of the string buffers, just as with `vector`/`list`.
- **Immutability:** never modify an element in place via `set_iterator_get` — re-insert instead, or the ordering invariant breaks (this mirrors `std::set`, whose elements are `const`).


## Examples

Every example below is a complete program (`int main`) and prints with this project's own **`fmt`** library (`fmt_printf`). They behave exactly like the equivalent `std::set` usage and are leak-free — each was built `-Wall -Wextra -Werror` and run under `valgrind --leak-check=full` with zero leaks and zero errors.

Because the examples print with `fmt_printf`, compile them together with `set.c`, the `fmt` module and its `encoding`/`string` dependencies. 

```bash
gcc -std=c17 -I. my_example.c \
    set/set.c fmt/fmt.c encoding/encoding.c string/std_string.c \
    vector/vector.c random/random.c algorithm/algorithm.c map/map.c -lm
```

### Example 1: Insert keeps elements unique and sorted

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int data[] = {5, 3, 8, 3, 1, 8, 9, 2};

    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
        set_insert(s, &data[i]);
    }

    fmt_printf("size=%zu elements:", set_size(s));
    for (SetIterator it = set_begin(s); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %d", *(const int*)set_iterator_get(&it));
    }
    fmt_printf("\n");

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
size=6 elements: 1 2 3 5 8 9
```

### Example 2: Duplicates are rejected (`set_insert` returns a flag)

```c
#include "fmt/fmt.h"
#include "set/set.h"


static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int v = 42;

    fmt_printf("first insert : %s\n", set_insert(s, &v) ? "inserted" : "rejected");
    fmt_printf("second insert: %s\n", set_insert(s, &v) ? "inserted" : "rejected");
    fmt_printf("size=%zu\n", set_size(s));

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
first insert : inserted
second insert: rejected
size=1
```


### Example 3: Membership with `set_contains` / `set_count`

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {10, 20, 30};

    for (int i = 0; i < 3; i++) {
        set_insert(s, &vals[i]);
    }

    int k = 20;
    fmt_printf("contains 20: %s (count=%zu)\n", set_contains(s, &k) ? "yes" : "no", set_count(s, &k));
    k = 25;
    fmt_printf("contains 25: %s (count=%zu)\n", set_contains(s, &k) ? "yes" : "no", set_count(s, &k));

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
contains 20: yes (count=1)
contains 25: no (count=0)
```

### Example 4: `set_find` returns an iterator (or `end`)

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        set_insert(s, &vals[i]);
    }

    int k = 3;
    SetIterator it = set_find(s, &k);
    if (it.node) {
        fmt_printf("found %d\n", *(const int*)set_iterator_get(&it));
    }

    k = 9;
    it = set_find(s, &k);
    fmt_printf("find 9: %s\n", it.node ? "found" : "end (not found)");

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
found 3
find 9: end (not found)
```

### Example 5: `set_lower_bound` / `set_upper_bound`

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {10, 20, 30, 40};
    
    for (int i = 0; i < 4; i++) {
        set_insert(s, &vals[i]);
    }

    int k = 20;
    SetIterator lo = set_lower_bound(s, &k);   /* first >= 20 */
    SetIterator hi = set_upper_bound(s, &k);   /* first  > 20 */

    fmt_printf("lower_bound(20)=%d upper_bound(20)=%d\n", *(const int*)set_iterator_get(&lo), *(const int*)set_iterator_get(&hi));

    k = 25;
    lo = set_lower_bound(s, &k);
    fmt_printf("lower_bound(25)=%d\n", *(const int*)set_iterator_get(&lo));

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
lower_bound(20)=20 upper_bound(20)=30
lower_bound(25)=30
```

### Example 6: `set_equal_range`

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {1, 3, 5, 7};

    for (int i = 0; i < 4; i++) {
        set_insert(s, &vals[i]);
    }

    int k = 5;
    SetIteratorPair p = set_equal_range(s, &k);
    fmt_printf("[%d, %d)\n",
           *(const int*)set_iterator_get(&p.first),
           *(const int*)set_iterator_get(&p.second));

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
[5, 7)
```

### Example 7: Erase

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}


int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        set_insert(s, &vals[i]);
    }

    int k = 3;
    bool removed = set_erase(s, &k);   /* sequence the erase before reading the size */
    fmt_printf("erase 3: %s (size=%zu)\n", removed ? "ok" : "absent", set_size(s));

    k = 9;
    removed = set_erase(s, &k);
    fmt_printf("erase 9: %s (size=%zu)\n", removed ? "ok" : "absent", set_size(s));

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
erase 3: ok (size=4)
erase 9: absent (size=4)
```

### Example 8: Clear and reuse

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        set_insert(s, &vals[i]);
    }

    set_clear(s);
    fmt_printf("after clear: size=%zu empty=%s\n", set_size(s), set_empty(s) ? "yes" : "no");

    int v = 7;
    set_insert(s, &v);
    fmt_printf("after reuse: size=%zu\n", set_size(s));

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
after clear: size=0 empty=yes
after reuse: size=1
```

### Example 9: Reverse iteration

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* s = set_create(sizeof(int), cmp_int);
    int vals[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++) {
        set_insert(s, &vals[i]);
    }

    fmt_printf("reverse:");
    for (SetIterator it = set_rbegin(s); it.node; set_iterator_decrement(&it)) {
        fmt_printf(" %d", *(const int*)set_iterator_get(&it));
    }
    fmt_printf("\n");

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
reverse: 5 4 3 2 1
```

### Example 10: Deep copy is independent of the original

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Set* A = set_create(sizeof(int), cmp_int);
    int vals[] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        set_insert(A, &vals[i]);
    }

    Set* B = set_copy(A);
    fmt_printf("copy equal: %s\n", set_is_equal(A, B) ? "yes" : "no");

    int x = 99;
    set_insert(B, &x);                 /* mutate only the copy */

    fmt_printf("A size=%zu, B size=%zu\n", set_size(A), set_size(B));
    fmt_printf("A has 99: %s, B has 99: %s\n",
           set_contains(A, &x) ? "yes" : "no", set_contains(B, &x) ? "yes" : "no");

    set_deallocate(A);
    set_deallocate(B);

    return 0;
}
```

**Output:**
```
copy equal: yes
A size=3, B size=4
A has 99: no, B has 99: yes
```

### Example 11: Swap

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static void print_set(const char* label, Set* s) {
    fmt_printf("%s", label);

    for (SetIterator it = set_begin(s); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %d", *(const int*)set_iterator_get(&it));
    }

    fmt_printf(" (size %zu)\n", set_size(s));
}

int main(void) {
    Set* a = set_create(sizeof(int), cmp_int);
    Set* b = set_create(sizeof(int), cmp_int);
    int va[] = {1, 2};
    int vb[] = {7, 8, 9};

    for (int i = 0; i < 2; i++) {
        set_insert(a, &va[i]);
    }

    for (int i = 0; i < 3; i++) {
        set_insert(b, &vb[i]);
    }


    set_swap(a, b);
    print_set("a:", a);
    print_set("b:", b);

    set_deallocate(a);
    set_deallocate(b);
    return 0;
}
```

**Output:**
```
a: 7 8 9 (size 3)
b: 1 2 (size 2)
```

### Example 12: Set algebra — union / intersection / difference / symmetric difference

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static void print_set(const char* label, Set* s) {
    fmt_printf("%s", label);

    for (SetIterator it = set_begin(s); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %d", *(const int*)set_iterator_get(&it));
    }

    fmt_printf("\n");
}

int main(void) {
    Set* A = set_create(sizeof(int), cmp_int);
    Set* B = set_create(sizeof(int), cmp_int);
    int va[] = {1, 2, 3, 4};
    int vb[] = {3, 4, 5, 6};

    for (int i = 0; i < 4; i++) { 
        set_insert(A, &va[i]); 
        set_insert(B, &vb[i]); 
    }

    Set* U = set_union(A, B);
    Set* I = set_intersection(A, B);
    Set* D = set_difference(A, B);              /* A \ B */
    Set* S = set_symmetric_difference(A, B);

    print_set("A | B =", U);
    print_set("A & B =", I);
    print_set("A \\ B =", D);
    print_set("A ^ B =", S);

    set_deallocate(A); 
    set_deallocate(B);
    set_deallocate(U); 
    set_deallocate(I); 
    set_deallocate(D); 
    set_deallocate(S);

    return 0;
}
```

**Output:**
```
A | B = 1 2 3 4 5 6
A & B = 3 4
A \ B = 1 2
A ^ B = 1 2 5 6
```


### Example 13: Subset / superset / disjoint

```c
#include "fmt/fmt.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static Set* make(const int* v, int n) {
    Set* s = set_create(sizeof(int), cmp_int);

    for (int i = 0; i < n; i++) {
        set_insert(s, &v[i]);
    }

    return s;
}

int main(void) {
    int va[] = {1, 2, 3, 4}, vsub[] = {2, 3}, vother[] = {5, 6};
    Set* A     = make(va, 4);
    Set* sub   = make(vsub, 2);
    Set* other = make(vother, 2);

    fmt_printf("{2,3} subset of A   : %s\n", set_is_subset(sub, A)    ? "yes" : "no");
    fmt_printf("A superset of {2,3} : %s\n", set_is_superset(A, sub)  ? "yes" : "no");
    fmt_printf("A disjoint {5,6}    : %s\n", set_is_disjoint(A, other)? "yes" : "no");

    set_deallocate(A); set_deallocate(sub); set_deallocate(other);
    return 0;
}
```

**Output:**
```
{2,3} subset of A   : yes
A superset of {2,3} : yes
A disjoint {5,6}    : yes
```


### Example 14: Lexicographic comparison family

```c
#include "fmt/fmt.h"
#include "set/set.h"


static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static Set* make(const int* v, int n) {
    Set* s = set_create(sizeof(int), cmp_int);

    for (int i = 0; i < n; i++) {
        set_insert(s, &v[i]);
    }

    return s;
}

int main(void) {
    int vx[] = {1, 2, 3}, vy[] = {1, 2, 4}, vz[] = {1, 2, 3};
    Set* X = make(vx, 3);
    Set* Y = make(vy, 3);
    Set* Z = make(vz, 3);

    fmt_printf("X == Z : %s\n", set_is_equal(X, Z)   ? "yes" : "no");
    fmt_printf("X <  Y : %s\n", set_is_less(X, Y)    ? "yes" : "no");
    fmt_printf("Y >  X : %s\n", set_is_greater(Y, X) ? "yes" : "no");
    fmt_printf("X != Y : %s\n", set_is_not_equal(X, Y) ? "yes" : "no");

    set_deallocate(X); set_deallocate(Y); set_deallocate(Z);
    return 0;
}
```

**Output:**
```
X == Z : yes
X <  Y : yes
Y >  X : yes
X != Y : yes
```

### Example 15: Any element type — a set of structs and a set of strings

```c
#include <stdio.h>
#include <string.h>
#include "set/set.h"


typedef struct { 
    int x, y; 
} Point;

static int cmp_point(const void* a, const void* b) {
    const Point* p = a; const Point* q = b;

    if (p->x != q->x) {
        return (p->x > q->x) - (p->x < q->x);
    }

    return (p->y > q->y) - (p->y < q->y);
}

static int cmp_str(const void* a, const void* b) {
    return strcmp(*(const char* const*)a, *(const char* const*)b);
}

int main(void) {
    Set* pts = set_create(sizeof(Point), cmp_point);
    Point pv[] = {{2, 1}, {1, 5}, {2, 1}, {1, 2}};   /* {2,1} duplicated */

    for (int i = 0; i < 4; i++) {
        set_insert(pts, &pv[i]);
    }

    fmt_printf("points (size %zu):", set_size(pts));
    for (SetIterator it = set_begin(pts); it.node; set_iterator_increment(&it)) {
        const Point* p = set_iterator_get(&it);
        fmt_printf(" (%d,%d)", p->x, p->y);
    }
    fmt_printf("\n");

    /* set of strings */
    Set* words = set_create(sizeof(const char*), cmp_str);
    const char* wv[] = {"pear", "apple", "pear", "banana", "apple"};
    for (int i = 0; i < 5; i++) {
        set_insert(words, &wv[i]);
    }

    fmt_printf("words  (size %zu):", set_size(words));
    for (SetIterator it = set_begin(words); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %s", *(const char* const*)set_iterator_get(&it));
    }
    fmt_printf("\n");

    set_deallocate(pts);
    set_deallocate(words);

    return 0;
}
```

**Output:**
```
points (size 3): (1,2) (1,5) (2,1)
words  (size 3): apple banana pear
```


## Cross-module examples (with other c_std libraries)

These combine `set` with other modules from the library — `vector`, `string`, `random`, `algorithm`, and `map`.

### Example 16: `set` + `vector` — deduplicate and sort a vector

```c
#include "fmt/fmt.h"
#include "vector/vector.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    int raw[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    Vector* v = vector_create(sizeof(int));

    for (size_t i = 0; i < sizeof(raw) / sizeof(raw[0]); i++) {
        vector_push_back(v, &raw[i]);
    }

    /* Use a set to deduplicate + sort the vector's values. */
    Set* s = set_create(sizeof(int), cmp_int);
    for (size_t i = 0; i < vector_size(v); i++) {
        set_insert(s, vector_at(v, i));
    }

    /* Read the sorted, unique values back into a fresh vector. */
    Vector* uniq = vector_create(sizeof(int));
    for (SetIterator it = set_begin(s); it.node; set_iterator_increment(&it)) {
        vector_push_back(uniq, set_iterator_get(&it));
    }

    fmt_printf("%zu values -> %zu unique:", vector_size(v), vector_size(uniq));
    for (size_t i = 0; i < vector_size(uniq); i++) {
        fmt_printf(" %d", *(const int*)vector_at(uniq, i));
    }
    fmt_printf("\n");

    vector_deallocate(v);
    vector_deallocate(uniq);
    set_deallocate(s);

    return 0;
}
```

**Output:**
```
11 values -> 7 unique: 1 2 3 4 5 6 9
```


### Example 17: `set` + `string` — unique, sorted vocabulary from a sentence

```c
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include "string/std_string.h"
#include "set/set.h"

static int cmp_str(const void* a, const void* b) {
    return strcmp(*(const char* const*)a, *(const char* const*)b);
}

int main(void) {
    String* sentence = string_create("the quick brown fox the lazy dog the fox");
    int n = 0;
    String** words = string_split(sentence, " ", &n);

    /* The set stores each word's char* (by value); the String objects own
     * the text and stay alive until they are deallocated below. */
    Set* vocab = set_create(sizeof(const char*), cmp_str);
    for (int i = 0; i < n; i++) {
        const char* w = string_c_str(words[i]);
        set_insert(vocab, &w);
    }

    fmt_printf("%d words, %zu unique:", n, set_size(vocab));
    for (SetIterator it = set_begin(vocab); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %s", *(const char* const*)set_iterator_get(&it));
    }
    fmt_printf("\n");
    set_deallocate(vocab);

    for (int i = 0; i < n; i++) {
        string_deallocate(words[i]);
    }
    free(words);
    string_deallocate(sentence);

    return 0;
}
```

**Output:**
```
9 words, 6 unique: brown dog fox lazy quick the
```

### Example 18: `set` + `random` — distinct values from a stream of rolls

```c
#include "fmt/fmt.h"
#include "random/random.h"
#include "set/set.h"

static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    random_seed(2026);                  /* reproducible across runs/platforms */
    Set* distinct = set_create(sizeof(int), cmp_int);

    for (int i = 0; i < 40; i++) {
        int r = random_randint(1, 10);  /* inclusive [1, 10] */
        set_insert(distinct, &r);
    }

    fmt_printf("distinct values in 40 rolls (%zu):", set_size(distinct));
    for (SetIterator it = set_begin(distinct); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %d", *(const int*)set_iterator_get(&it));
    }
    fmt_printf("\n");

    set_deallocate(distinct);
    return 0;
}
```

**Output:**
```
distinct values in 40 rolls (10): 1 2 3 4 5 6 7 8 9 10
```

### Example 19: `set` + `algorithm` — run `<algorithm>` over the unique elements

```c
#include "fmt/fmt.h"
#include "algorithm/algorithm.h"
#include "set/set.h"


static int cmp_int(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

static void print_int(void* el) { 
    fmt_printf(" %d", *(const int*)el); 
}

int main(void) {
    int vals[] = {5, 2, 9, 2, 7, 5, 1, 9};
    Set* s = set_create(sizeof(int), cmp_int);

    for (size_t i = 0; i < sizeof(vals) / sizeof(vals[0]); i++) { 
        set_insert(s, &vals[i]);
    }

    /* Copy the sorted, unique elements into a flat array, then use algorithm_*. */
    int uniq[16];
    size_t n = 0;
    for (SetIterator it = set_begin(s); it.node; set_iterator_increment(&it)) {
        uniq[n++] = *(const int*)set_iterator_get(&it);
    }

    fmt_printf("unique sorted:");

    algorithm_for_each(uniq, n, sizeof(int), print_int);
    int* mx = (int*)algorithm_max_element(uniq, n, sizeof(int), cmp_int);

    fmt_printf("  | count=%zu max=%d\n", n, *mx);

    set_deallocate(s);
    return 0;
}
```

**Output:**
```
unique sorted: 1 2 5 7 9  | count=5 max=9
```

### Example 20: `set` + `map` — find words that repeat

```c
#include "fmt/fmt.h"
#include <string.h>
#include <stdint.h>
#include "map/map.h"
#include "set/set.h"


static int cmp_key(const KeyType a, const KeyType b) {     /* map keys are char* */
    return strcmp((const char*)a, (const char*)b);
}
static int cmp_str(const void* a, const void* b) {         /* set elements are char* */
    return strcmp(*(const char* const*)a, *(const char* const*)b);
}

int main(void) {
    const char* words[] = {"red", "blue", "red", "green", "blue", "red"};

    /* Count occurrences in a map so no per-value allocation is needed). */
    Map* counts = map_create(cmp_key, NULL, NULL);
    for (size_t i = 0; i < sizeof(words) / sizeof(words[0]); i++) {
        ValueType v = map_at(counts, (KeyType)words[i]);
        intptr_t c = v ? (intptr_t)v : 0;

        map_insert(counts, (KeyType)words[i], (ValueType)(c + 1));
    }

    /* Collect the words seen more than once into a sorted set. */
    Set* repeated = set_create(sizeof(const char*), cmp_str);
    for (MapIterator it = map_begin(counts); it.node; map_iterator_increment(&it)) {
        const char* w = (const char*)map_node_get_key(it.node);

        if ((intptr_t)map_node_get_value(it.node) >= 2) {
            set_insert(repeated, &w);
        }
    }

    fmt_printf("repeated words (%zu):", set_size(repeated));
    for (SetIterator it = set_begin(repeated); it.node; set_iterator_increment(&it)) {
        fmt_printf(" %s", *(const char* const*)set_iterator_get(&it));
    }
    fmt_printf("\n");

    set_deallocate(repeated);
    map_deallocate(counts);
    
    return 0;
}
```

**Output:**
```
repeated words (2): blue red
```

---

## License

This project is open-source and available under [ISC License].
