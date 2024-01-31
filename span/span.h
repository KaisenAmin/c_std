/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Span
*/


#ifndef SPAN_H_
#define SPAN_H_

#define SPAN_LOGGING_ENABLE 

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    void *data;
    size_t size;
    size_t elemSize; // Size of each element.
} Span;

Span* span_create(void* data, size_t elemCount, size_t elemSize);
void span_destroy(Span* span);

Span span_first(Span* span, size_t count);
Span span_last(Span* span, size_t count);
Span span_subspan(Span* span, size_t offset, size_t count);

const void* span_cbegin(const Span* span);
const void* span_cend(const Span* span);
const void* span_cdata(const Span* span);
const void* span_crbegin(const Span* span);
const void* span_crend(const Span* span);

void* span_increment(Span* span, void* ptr);
void* span_decrement(Span* span, void* ptr);
void* span_front(Span* span);
void* span_back(Span* span);
void* span_at(Span* span, size_t index);
void* span_rend(Span* span);
void* span_rbegin(Span* span);
void* span_end(Span* span);
void* span_begin(Span* span);
void* span_data(Span* span);

bool span_is_equal(const Span* span1, const Span* span2);
bool span_is_less(const Span* span1, const Span* span2);
bool span_is_greater(const Span* span1, const Span* span2);
bool span_is_not_equal(const Span* span1, const Span* span2);
bool span_is_greater_or_equal(const Span* span1, const Span* span2);
bool span_is_less_or_equal(const Span* span1, const Span* span2);
bool span_empty(const Span* span);

size_t span_size_bits(const Span* span);
size_t span_size(const Span* span);

#endif // SPAN_H_
