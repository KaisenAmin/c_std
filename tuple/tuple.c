/**
 * @author Amin Tahmasebi
 * @date 2024 
 * @class Tuple
*/

#include "tuple.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>


Tuple* tuple_create(size_t size) {
    Tuple* tuple = (Tuple*)malloc(sizeof(Tuple));
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for tuple in tuple_create\n");
        #endif 
        exit(-1);
    }

    tuple->size = size;
    tuple->elements = (TupleElement*)calloc(size, sizeof(TupleElement));
    if (!tuple->elements) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed for tuple elements in tuple_create\n");
        #endif 
        free(tuple);
        exit(-1);
    }
    return tuple;
}

void tuple_deallocate(Tuple* tuple) {
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Info: tuple is NULL can't deallocate\n");
        #endif 
        return;
    }
    for (size_t i = 0; i < tuple->size; ++i) {
        free(tuple->elements[i].data);
    }
    free(tuple->elements);
    free(tuple);
}

bool tuple_set(Tuple* tuple, size_t index, void* data, size_t size) {
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: data is null in tuple_set\n");
        #endif 
        return false;
    }
    if (index >= tuple->size) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: index is bigger or equal than tuple->size in tuple_set");
        #endif 
        return false;
    }
    if (!data) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: date is null in tuple_set\n");
        #endif 
        return false;
    }

    void* newData = malloc(size);
    if (!newData) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocattion failed for newData in tuple_set\n");
        #endif 
        return false;
    }

    memcpy(newData, data, size);
    free(tuple->elements[index].data);

    tuple->elements[index].data = newData;
    tuple->elements[index].size = size;
    return true;
}

void* tuple_get(const Tuple* tuple, size_t index, size_t* outSize) {
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: tuple is null in tuple_get\n");
        #endif 
        return NULL;
    }
    if (index >= tuple->size) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: index is bigger or equal than tuple->size in tuple_get\n");
        #endif 
        return NULL;
    }
    if(!outSize) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: outSize is null in tuple_get please give valid address\n");
        #endif 
        return NULL;
    }

    *outSize = tuple->elements[index].size;
    return tuple->elements[index].data;
}

Tuple* tuple_make_tuple(size_t num, ...) {
    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Creation of tuple failed in tuple_make_tuple\n");
        #endif 
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void* data = va_arg(args, void*);
        size_t size = va_arg(args, size_t);
        if (!tuple_set(tuple, i, data, size)) {
            #ifdef TUPLE_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: failed to set data in tuple_make_tuple\n");
            #endif 
            tuple_deallocate(tuple);
            va_end(args);
            return NULL;
        }
    }
    va_end(args);
    return tuple;
}

// Function to create a tuple of pointers to the provided arguments
Tuple* tuple_tie(size_t num, ...) {
    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Creation of tuple failed in tuple_tie\n");
        #endif 
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void** pointer = va_arg(args, void**);
        if (!tuple_set(tuple, i, &pointer, sizeof(void*))) {
            #ifdef TUPLE_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: failed to set data in tuple_tie\n");
            #endif 
            tuple_deallocate(tuple);
            va_end(args);
            return NULL;
        }
    }
    va_end(args);
    return tuple;
}

void tuple_swap(Tuple* a, Tuple* b) {
    if (a == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 'a' is null in tuple_swap\n");
        #endif 
        return;
    }
    if (b == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 'b' is null in tuple_swap\n");
        #endif 
        return;
    }
    // Swap the sizes
    size_t tempSize = a->size;
    a->size = b->size;
    b->size = tempSize;

    // Swap the elements
    TupleElement* tempElements = a->elements;
    a->elements = b->elements;
    b->elements = tempElements;
}

Tuple* tuple_forward_as_tuple(size_t num, ...) {
    Tuple* tuple = tuple_create(num);
    if (!tuple) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Creation of tuple failed in tuple_forward_as_tuple\n");
        #endif 
        return NULL;
    }

    va_list args;
    va_start(args, num);

    for (size_t i = 0; i < num; ++i) {
        void* pointer = va_arg(args, void*);
        // Store the pointer directly, without allocating new memory
        tuple->elements[i].data = pointer;
        tuple->elements[i].size = sizeof(pointer);
    }
    va_end(args);
    return tuple;
}

size_t tuple_size(const Tuple* tuple) {
    if (tuple == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: tuple is null in tuple_size\n");
        #endif 
        return 0;
    }
    return tuple->size;
}

bool tuple_is_equal(const Tuple* t1, const Tuple* t2) {
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_equal\n");
        #endif 
        return false;
    }
    if (tuple_size(t1) != tuple_size(t2)) {
        return false;
    }
    for (size_t i = 0; i < tuple_size(t1); ++i) {
        if (t1->elements[i].size != t2->elements[i].size ||
            memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size) != 0) {
            return false;
        }
    }
    return true;
}

bool tuple_is_less(const Tuple* t1, const Tuple* t2) {
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_less\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_less\n");
        #endif 
        return false;
    }
    size_t minSize = (tuple_size(t1) < tuple_size(t2)) ? tuple_size(t1) : tuple_size(t2);

    for (size_t i = 0; i < minSize; ++i) {
        if (t1->elements[i].size < t2->elements[i].size) {
            return true;
        }
        if (t1->elements[i].size > t2->elements[i].size) { 
            return false;
        }

        int cmp = memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size);
        if (cmp < 0) {
            return true;
        }
        if (cmp > 0) { 
            return false;
        }
    }

    return tuple_size(t1) < tuple_size(t2);
}

bool tuple_is_greater(const Tuple* t1, const Tuple* t2) {
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_greater\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_greater\n");
        #endif 
        return false;
    }

    size_t minSize = (tuple_size(t1) < tuple_size(t2)) ? tuple_size(t1) : tuple_size(t2);

    for (size_t i = 0; i < minSize; ++i) {
        if (t1->elements[i].size > t2->elements[i].size) {
            return true;
        }
        if (t1->elements[i].size < t2->elements[i].size) { 
            return false;
        }

        int cmp = memcmp(t1->elements[i].data, t2->elements[i].data, t1->elements[i].size);
        if (cmp > 0) { 
            return true;
        }
        if (cmp < 0) { 
            return false;
        }
    }
    return tuple_size(t1) > tuple_size(t2);
}

bool tuple_is_not_equal(const Tuple* t1, const Tuple* t2) {
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_not_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_not_equal\n");
        #endif 
        return false;
    }
    return !tuple_is_equal(t1, t2);
}

bool tuple_is_greater_or_equal(const Tuple* t1, const Tuple* t2) {
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_greater_or_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_greater_or_equal\n");
        #endif 
        return false;
    }
    return tuple_is_greater(t1, t2) || tuple_is_equal(t1, t2);
}

bool tuple_is_less_or_equal(const Tuple* t1, const Tuple* t2) {
    if (t1 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't1' is null in tuple_is_less_or_equal\n");
        #endif 
        return false;
    }
    if (t2 == NULL) {
        #ifdef TUPLE_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Param 't2' is null in tuple_is_less_or_equal\n");
        #endif 
        return false;
    }
    return tuple_is_less(t1, t2) || tuple_is_equal(t1, t2);
}

bool tuple_is_empty(Tuple* t) {
    if (!t) { 
        return true;
    }
    return t->size == 0;
}
