#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void *first;
    void *second;
} Pair;

// Function pointer types for comparison and operations
typedef int (*CompareFunc)(const void *, const void *);
typedef bool (*CompareFuncBool)(const void *, const void *);
typedef void (*ForEachOpFunc)(void *);
typedef void (*AccumulateOpFunc)(void *, const void *);
typedef bool (*BoolPredicateFunc)(const void *);
typedef uint32_t (*UniformRandomNumberGenerator)(void);
typedef void (*TransformFunc)(void *, const void *);
typedef void (*ReduceFunc)(void*, const void*);
typedef bool (*PredicateFunc)(const void *);
typedef void (*GeneratorFunc)(void *);
typedef bool (*UnaryPredicateFunc)(const void *);

void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp);
void algorithm_stable_sort(void *base, size_t num, size_t size, CompareFunc comp);
void algorithm_copy(const void *source, size_t num, size_t size, void *dest);
void algorithm_for_each(void *base, size_t num, size_t size, ForEachOpFunc op);
void algorithm_transform(const void *base, size_t num, size_t size, void *result, TransformFunc op);
void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp);
void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp);
void algorithm_fill(void *first, void *last, size_t size, const void *val);
void algorithm_fill_n(void *first, size_t n, size_t size, const void *val);
void algorithm_shuffle(void *base, size_t num, size_t size, UniformRandomNumberGenerator rng);
void algorithm_generate(void *first, void *last, size_t size, GeneratorFunc gen);
void algorithm_generate_n(void *first, size_t n, size_t size, GeneratorFunc gen);
void algorithm_copy_backward(const void *first, const void *last, size_t size, void *result);
void algorithm_copy_if(const void *first, const void *last, size_t size, void *result, UnaryPredicateFunc pred);
void algorithm_copy_n(const void *first, size_t n, size_t size, void *result);
void algorithm_swap(void *a, void *b, size_t size);
void algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size);
void algorithm_rotate(void *first, void *middle, void *last, size_t size);
void algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result);
void algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, CompareFunc comp);
void algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, CompareFunc comp);
void algorithm_replace_if(void *base, size_t num, size_t size, const void *val, BoolPredicateFunc pred);

void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
void *algorithm_find_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
void *algorithm_find_if_not(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
void *algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp);
void *algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp);
void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, AccumulateOpFunc op);
void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp);
void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp);
void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, ReduceFunc op);
void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred);
void *algorithm_adjacent_find(const void *base, size_t num, size_t size, CompareFunc comp);
void *algorithm_is_sorted_until(const void *base, size_t num, size_t size, CompareFunc comp);
void *algorithm_remove(void *base, size_t num, size_t size, const void *val, CompareFunc comp);

bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred);

size_t algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp);
size_t algorithm_count(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
size_t algorithm_count_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred);
size_t algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, CompareFunc comp);
size_t algorithm_remove_copy_if(const void *source, size_t num, size_t size, void *result, BoolPredicateFunc pred);

Pair algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, CompareFunc comp);
Pair algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFuncBool comp);

bool algorithm_next_permutation(void *first, void *last, size_t size, CompareFuncBool comp);
bool algorithm_prev_permutation(void *first, void *last, size_t size, CompareFuncBool comp);
bool algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp);
bool algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp);
bool algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, CompareFunc comp);
bool algorithm_is_sorted(const void *base, size_t num, size_t size, CompareFunc comp);

const void* algorithm_search(const void* first1, const void* last1, size_t size1,const void* first2, const void* last2, size_t size2, CompareFuncBool comp);
const void *algorithm_search_n(const void *first, const void* last1, size_t size, size_t count, const void *val, CompareFuncBool comp);

#endif // ALGORITHM_H_
