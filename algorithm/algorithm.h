#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <stdbool.h>
#include <stddef.h>

// Function pointer types for comparison and operations
typedef int (*CompareFunc)(const void *, const void *);
typedef void (*OperationFunc)(void *);
typedef bool (*BoolPredicateFunc)(const void *);


void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp);
void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
void algorithm_copy(const void *source, size_t num, size_t size, void *dest);
void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, OperationFunc op);

// Other possible functions:
void algorithm_for_each(void *base, size_t num, size_t size, OperationFunc op);
void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp);
void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp);
bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
bool algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
void algorithm_transform(const void *base, size_t num, size_t size, void *result, OperationFunc op);
void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, OperationFunc op);
void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred);

//Rearrange elements such that the element at the nth position is the one that would be in that position in a sorted sequence.
void algorithm_nth_element(void *base, size_t num, size_t size, size_t n, CompareFunc comp);

// Remove consecutive duplicate elements in a range.
size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp);

// Merge two sorted ranges.
void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp);

// Merge two consecutive sorted ranges in-place.
void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp);
 

#endif // ALGORITHM_H_
