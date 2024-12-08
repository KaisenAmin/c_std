/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Tuple
*/

#ifndef TUPLE_H_
#define TUPLE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif 

// #define TUPLE_LOGGING_ENABLE

#ifdef TUPLE_LOGGING_ENABLE
    #define TUPLE_LOG(fmt, ...) \
        do { fprintf(stderr, "[TUPLE LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define TUPLE_LOG(fmt, ...) do { } while (0)
#endif

typedef struct {
    void* data;
    size_t size;
} TupleElement;

typedef struct {
    TupleElement* elements;
    size_t size;
} Tuple;

Tuple* tuple_create(size_t size);
Tuple* tuple_make_tuple(size_t num, ...);
Tuple* tuple_tie(size_t num, ...);
Tuple* tuple_forward_as_tuple(size_t num, ...);

void tuple_deallocate(Tuple* tuple);
void tuple_swap(Tuple* a, Tuple* b);

bool tuple_set(Tuple* tuple, size_t index, void* data, size_t size);
bool tuple_is_equal(const Tuple* t1, const Tuple* t2);
bool tuple_is_less(const Tuple* t1, const Tuple* t2);
bool tuple_is_greater(const Tuple* t1, const Tuple* t2);
bool tuple_is_not_equal(const Tuple* t1, const Tuple* t2);
bool tuple_is_greater_or_equal(const Tuple* t1, const Tuple* t2);
bool tuple_is_less_or_equal(const Tuple* t1, const Tuple* t2);
bool tuple_is_empty(Tuple* t);

void* tuple_get(const Tuple* tuple, size_t index, size_t* outSize);
size_t tuple_size(const Tuple* tuple);

#ifdef __cplusplus 
}
#endif 

#endif 
