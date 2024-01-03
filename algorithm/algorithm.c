#include "algorithm.h"
#include <string.h>
#include <stdlib.h>

static void quickSortInternal(void *base, size_t low, size_t high, size_t size, CompareFunc comp, void *temp) {
    if (low < high) {
        char* pivot = (char*)base + high * size;
        size_t i = low;

        for (size_t j = low; j < high; j++) {
            if (comp((char*)base + j * size, pivot) < 0) {
                memcpy(temp, (char*)base + j * size, size);
                memcpy((char*)base + j * size, (char*)base + i * size, size);
                memcpy((char*)base + i * size, temp, size);
                i++;
            }
        }

        memcpy(temp, (char*)base + i * size, size);
        memcpy((char*)base + i * size, pivot, size);
        memcpy(pivot, temp, size);

        if (i > 0) 
            quickSortInternal(base, low, i - 1, size, comp, temp);
        quickSortInternal(base, i + 1, high, size, comp, temp);
    }
}

void algorithm_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num > 1) {
        void* temp = malloc(size);
        if (temp) {
            quickSortInternal(base, 0, num - 1, size, comp, temp);
            free(temp);
        } 
        else {
            perror("Failed to allocate memory for sorting");
            exit(EXIT_FAILURE);
        }
    }
}

void *algorithm_find(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    const char *ptr = (const char*)base;

    for (size_t i = 0; i < num; i++) {
        if (comp(ptr + i * size, val) == 0) {
            return (void *)(ptr + i * size);
        }
    }

    return NULL;
}

void *algorithm_find_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char*)base;

    for (size_t i = 0; i < num; i++) {
        if (pred(ptr + i * size)) {
            return (void *)(ptr + i * size);
        }
    }

    return NULL;
}

void *algorithm_find_if_not(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char*)base;

    for (size_t i = 0; i < num; i++) {
        if (!pred(ptr + i * size)) {
            return (void *)(ptr + i * size);
        }
    }

    return NULL;
}

void *algorithm_find_end(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    if (num2 == 0) {
        return (void*)((char*)base1 + num1 * size1);
    }

    void *ret = NULL;
    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    for (size_t i = 0; i <= num1 - num2; ++i) {
        bool found = true;
        for (size_t j = 0; j < num2; ++j) {
            if (!comp(ptr1 + (i + j) * size1, ptr2 + j * size2)) {
                found = false;
                break;
            }
        }
        if (found) {
            ret = (void *)(ptr1 + i * size1);
        }
    }
    return ret;
}

void *algorithm_find_first_of(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    const char *ptr1 = (const char*)base1;
    const char *ptr2 = (const char*)base2;

    for (size_t i = 0; i < num1; ++i) {
        for (size_t j = 0; j < num2; ++j) {
            if (comp(ptr1 + i * size1, ptr2 + j * size2) == 0) {
                return (void *)(ptr1 + i * size1);
            }
        }
    }

    return NULL;
}

bool algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        int result = comp(mid_elem, val);
        if (result < 0) {
            low = mid + 1;
        } 
        else if (result > 0) {
            high = mid;
        } 
        else {
            return true; // Element found
        }
    }
    return false; // Element not found
}

void *algorithm_max_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        return NULL;
    }

    const char *max_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(max_element, element) < 0) {
            max_element = element;
        }
    }
    return (void *)max_element;
}

void *algorithm_min_element(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) { 
        return NULL;
    }

    const char *min_element = (const char *)base;
    for (size_t i = 1; i < num; ++i) {
        const char *element = (const char *)base + i * size;
        if (comp(min_element, element) > 0) {
            min_element = element;
        }
    }
    return (void *)min_element;
}

void algorithm_for_each(void *base, size_t num, size_t size, ForEachOpFunc op) {
    char *ptr = (char *)base;
    for (size_t i = 0; i < num; ++i) {
        op(ptr + i * size);
    }
}

void algorithm_copy(const void *source, size_t num, size_t size, void *dest) {
    const char *src_ptr = (const char *)source;
    char *dest_ptr = (char *)dest;

    for (size_t i = 0; i < num; ++i) {
        memcpy(dest_ptr + i * size, src_ptr + i * size, size);
    }
}

void *algorithm_accumulate(const void *base, size_t num, size_t size, void *init, AccumulateOpFunc op) {
    char *result = (char *)init;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        op(result, ptr + i * size);
    }
    return result;
}

bool algorithm_all_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (!pred(ptr + i * size)) {
            return false;
        }
    }

    return true;
}

bool algorithm_any_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            return true;
        }
    }

    return false;
}

bool algorithm_none_of(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            return false;
        }
    }

    return true;
}

void algorithm_fill(void *first, void *last, size_t size, const void *val) {
    for (char *ptr = first; ptr != last; ptr += size) {
        memcpy(ptr, val, size);
    }
}

void algorithm_fill_n(void *first, size_t n, size_t size, const void *val) {
    for (char *ptr = first; n > 0; ptr += size, n--) {
        memcpy(ptr, val, size);
    }
}

size_t algorithm_count(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    size_t count = 0;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) == 0) {
            ++count;
        }
    }

    return count;
}

size_t algorithm_count_if(const void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    size_t count = 0;
    const char *ptr = (const char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            ++count;
        }
    }

    return count;
}

void algorithm_shuffle(void *base, size_t num, size_t size, UniformRandomNumberGenerator rng) {
    if (num > 1) {
        char *arr = (char *)base;
        char *temp = malloc(size);
        if (!temp) {
            perror("Failed to allocate memory for shuffle");
            exit(EXIT_FAILURE);
        }

        for (size_t i = num - 1; i > 0; i--) {
            size_t j = rng() % (i + 1);

            // Swap arr[i] and arr[j]
            memcpy(temp, arr + i * size, size);
            memcpy(arr + i * size, arr + j * size, size);
            memcpy(arr + j * size, temp, size);
        }

        free(temp);
    }
}
