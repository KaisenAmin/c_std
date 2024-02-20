#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "algorithm.h"

static void swap(void *a, void *b, size_t size) {
    // Using a stack-allocated buffer for small elements
    
    unsigned char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

static void reverse(void *first, void *last, size_t size) {
    char *a = (char *)first;
    char *b = (char *)last - size;

    while (a < b) {
        swap(a, b, size);
        a += size;
        b -= size;
    }
}

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

static void merge(void *base, size_t low, size_t mid, size_t high, size_t size, CompareFunc comp, void *temp) {
    size_t i = low, j = mid, k = 0;

    while (i < mid && j < high) {
        if (comp((char*)base + i * size, (char*)base + j * size) <= 0) {
            memcpy((char*)temp + k * size, (char*)base + i * size, size);
            i++;
        } else {
            memcpy((char*)temp + k * size, (char*)base + j * size, size);
            j++;
        }
        k++;
    }

    while (i < mid) {
        memcpy((char*)temp + k * size, (char*)base + i * size, size);
        i++; k++;
    }

    while (j < high) {
        memcpy((char*)temp + k * size, (char*)base + j * size, size);
        j++; k++;
    }

    memcpy((char*)base + low * size, temp, k * size);
}

static void mergeSortInternal(void *base, size_t low, size_t high, size_t size, CompareFunc comp, void *temp) {
    if (high - low > 1) {
        size_t mid = low + (high - low) / 2;
        mergeSortInternal(base, low, mid, size, comp, temp);
        mergeSortInternal(base, mid, high, size, comp, temp);
        merge(base, low, mid, high, size, comp, temp);
    }
}

void algorithm_stable_sort(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num > 1) {
        void* temp = malloc(num * size);
        if (temp) {
            mergeSortInternal(base, 0, num, size, comp, temp);
            free(temp);
        } else {
            perror("Failed to allocate memory for stable sorting");
            exit(EXIT_FAILURE);
        }
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

size_t algorithm_binary_search(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
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
            return (size_t)mid; // Element found, return index
        }
    }
    return (size_t)-1; // Element not found
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

void *algorithm_lower_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        if (comp(mid_elem, val) < 0) {
            low = mid + 1;
        } 
        else {
            high = mid;
        }
    }
    return (void *)((const char *)base + low * size);
}

void *algorithm_upper_bound(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    size_t low = 0;
    size_t high = num;

    while (low < high) {
        size_t mid = low + (high - low) / 2;
        const void *mid_elem = (const char *)base + mid * size;

        if (comp(mid_elem, val) <= 0) {
            low = mid + 1;
        } 
        else {
            high = mid;
        }
    }
    return (void *)((const char *)base + low * size);
}

void algorithm_transform(const void *base, size_t num, size_t size, void *result, TransformFunc op) {
    const char *input_ptr = (const char *)base;
    char *output_ptr = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        op(output_ptr + i * size, input_ptr + i * size);
    }
}

void *algorithm_reduce(const void *base, size_t num, size_t size, void *init, ReduceFunc op) {
    const char *ptr = (const char *)base;
    char *result = (char *)init;

    for (size_t i = 0; i < num; ++i) {
        op(result, ptr + i * size);
    }
    return result;
}

size_t algorithm_unique(void *base, size_t num, size_t size, CompareFunc comp) {
    if (num == 0) {
        return 0;
    }

    size_t uniqueCount = 1; // First element is always unique
    char *array = (char *)base;

    for (size_t i = 1; i < num; ++i) {
        // Compare current element with the last unique element
        if (comp(array + (uniqueCount - 1) * size, array + i * size) != 0) {
            // If different, move it to the next unique position
            if (uniqueCount != i) {
                memcpy(array + uniqueCount * size, array + i * size, size);
            }
            uniqueCount++;
        }
    }
    return uniqueCount;
}

bool algorithm_equal(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, 
                     CompareFunc comp) {
    // If the number of elements or sizes differ, the ranges are not equal
    if (num1 != num2 || size1 != size2) {
        return false;
    }

    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;

    for (size_t i = 0; i < num1; ++i) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            return false;
        }
    }

    return true;
}

bool algorithm_next_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    if (first == last) {
        return false;
    }

    char *i = (char *)last - size;
    while (i != (char *)first) {
        char *j = i;
        i -= size;

        if (comp(i, j)) {
            char *k = (char *)last - size;
            while (!comp(i, k)) {
                k -= size;
            }
            swap(i, k, size);
            reverse(j, last, size);

            return true;
        }

        if (i == (char *)first) {
            reverse(first, last, size);
            return false;
        }
    }
    return false; // This should not be reached
}

bool algorithm_prev_permutation(void *first, void *last, size_t size, CompareFuncBool comp) {
    if (first == last) {
        return false;
    }

    char *i = (char *)last - size;
    while (i != (char *)first) {
        char *j = i;
        i -= size;

        if (comp(j, i)) {
            char *k = (char *)last - size;
            while (!comp(k, i)) {
                k -= size;
            }
            swap(i, k, size);
            reverse(j, last, size);
            return true;
        }

        if (i == (char *)first) {
            reverse(first, last, size);
            return false;
        }
    }
    return false;
}

void *algorithm_partition(void *base, size_t num, size_t size, BoolPredicateFunc pred) {
    char *first = (char *)base;
    char *last = first + num * size;

    while (first != last) {
        while (first != last && pred(first)) {
            first += size;
        }
        do {
            last -= size;
            if (first == last) 
                break;
        } while (!pred(last));

        if (first != last) {
            swap(first, last, size);
            first += size;
        }
    }
    return first;
}

void algorithm_generate(void *first, void *last, size_t size, GeneratorFunc gen) {
    char *current = (char *)first;
    while (current != last) {
        gen(current);
        current += size;
    }
}

void algorithm_generate_n(void *first, size_t n, size_t size, GeneratorFunc gen) {
    char *current = (char *)first;
    for (size_t i = 0; i < n; ++i) {
        gen(current);
        current += size;
    }
}

void algorithm_copy_backward(const void *first, const void *last, size_t size, void *result) {
    const char *src = (const char *)last;
    char *dest = (char *)result;

    while (src != (const char *)first) {
        src -= size;  // Move source pointer backwards
        dest -= size; // Move destination pointer backwards
        memcpy(dest, src, size); // Copy element
    }
}

void algorithm_copy_if(const void *first, const void *last, size_t size, void *result, UnaryPredicateFunc pred) {
    const char *src = (const char *)first;
    char *dest = (char *)result;

    while (src != (const char *)last) {
        if (pred(src)) {
            memcpy(dest, src, size);
            dest += size;
        }
        src += size;
    }
}

void algorithm_copy_n(const void *first, size_t n, size_t size, void *result) {
    const char *src = (const char *)first;
    char *dest = (char *)result;

    for (size_t i = 0; i < n; ++i) {
        memcpy(dest, src, size);
        src += size;
        dest += size;
    }
}

Pair algorithm_equal_range(const void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    Pair range;
    range.first = algorithm_lower_bound(base, num, size, val, comp);
    range.second = algorithm_upper_bound(range.first, num, size, val, comp);
    return range;
}

bool algorithm_includes(const void *first1, size_t num1, size_t size1, const void *first2, size_t num2, size_t size2, 
                        CompareFunc comp) {
    const char *ptr1 = (const char *)first1;
    const char *ptr2 = (const char *)first2;
    const char *last1 = ptr1 + num1 * size1;
    const char *last2 = ptr2 + num2 * size2;

    while (ptr2 != last2) {
        if ((ptr1 == last1) || comp(ptr2, ptr1) < 0) {
            return false;
        }
        if (!comp(ptr1, ptr2)) {
            ptr2 += size2;
        }
        ptr1 += size1;
    }
    return true;
}

size_t algorithm_unique_copy(const void *first, size_t num, size_t size, void *result, CompareFunc comp) {
    if (num == 0) {
        return 0;
    }

    const char *src = (const char *)first;
    char *dst = (char *)result;
    memcpy(dst, src, size); // Copy first element
    size_t count = 1;

    for (size_t i = 1; i < num; ++i) {
        if (comp(dst + (count - 1) * size, src + i * size) != 0) {
            memcpy(dst + count * size, src + i * size, size);
            ++count;
        }
    }
    return count;
}

void algorithm_swap(void *a, void *b, size_t size) {
    void *temp = malloc(size);
    if (!temp) {
        perror("Failed to allocate memory for swap");
        exit(EXIT_FAILURE);
    }

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);

    free(temp);
}

void algorithm_swap_ranges(void *first1, void *first2, size_t num, size_t size) {
    char *ptr1 = (char *)first1;
    char *ptr2 = (char *)first2;

    for (size_t i = 0; i < num; ++i) {
        algorithm_swap(ptr1 + i * size, ptr2 + i * size, size);
    }
}

bool algorithm_is_sorted(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num < 2) {
        return true; // A range with less than two elements is always sorted.
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            return false; // Found an element that is greater than the next element.
        }
    }
    return true; // All elements are in sorted order.
}

void *algorithm_is_sorted_until(const void *base, size_t num, size_t size, CompareFunc comp) {
    if (num < 2) {
        return (void *)((char *)base + num * size); // Entire range is sorted
    }

    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) > 0) {
            return (void *)(ptr + (i + 1) * size); // Found unsorted element
        }
    }
    return (void *)((char *)base + num * size); // Entire range is sorted
}

void algorithm_rotate(void *first, void *middle, void *last, size_t size) {
    char *next = (char *)middle;

    while ((char *)first != next) {
        swap(first, next, size);
        first = (char *)first + size;
        next = (char *)next + size;

        if (next == (char *)last) {
            next = (char *)middle;
        } 
        else if (first == (char *)middle) {
            middle = next;
        }
    }
}

void algorithm_rotate_copy(const void *first, const void *middle, const void *last, size_t size, void *result) {
    const char *first_ptr = (const char *)first;
    const char *middle_ptr = (const char *)middle;
    const char *last_ptr = (const char *)last;
    char *result_ptr = (char *)result;

    // Copy from middle to last into result
    while (middle_ptr != last_ptr) {
        memcpy(result_ptr, middle_ptr, size);
        result_ptr += size;
        middle_ptr += size;
    }

    // Copy from first to middle into result
    while (first_ptr != (const char *)middle) {
        memcpy(result_ptr, first_ptr, size);
        result_ptr += size;
        first_ptr += size;
    }
}

void algorithm_merge(const void *base1, size_t num1, const void *base2, size_t num2, size_t size, void *result, CompareFunc comp) {
    size_t i = 0, j = 0, k = 0;
    const char *a = (const char *)base1;
    const char *b = (const char *)base2;
    char *res = (char *)result;

    while (i < num1 && j < num2) {
        if (comp(a + i * size, b + j * size) <= 0) {
            memcpy(res + k * size, a + i * size, size);
            i++;
        } 
        else {
            memcpy(res + k * size, b + j * size, size);
            j++;
        }
        k++;
    }

    while (i < num1) {
        memcpy(res + k * size, a + i * size, size);
        i++;
        k++;
    }

    while (j < num2) {
        memcpy(res + k * size, b + j * size, size);
        j++;
        k++;
    }
}

void algorithm_inplace_merge(void *base, size_t middle, size_t num, size_t size, CompareFunc comp) {
    size_t i = 0, j = middle, k;
    char *arr = (char *)base;
    char temp[size];

    while (i < middle && j < num) {
        if (comp(arr + i * size, arr + j * size) <= 0) {
            i++;
        } else {
            memcpy(temp, arr + j * size, size);
            for (k = j; k > i; k--) {
                memcpy(arr + k * size, arr + (k - 1) * size, size);
            }
            memcpy(arr + i * size, temp, size);

            i++;
            middle++;
            j++;
        }
    }
}

void *algorithm_adjacent_find(const void *base, size_t num, size_t size, CompareFunc comp) {
    const char *ptr = (const char *)base;
    for (size_t i = 0; i < num - 1; ++i) {
        if (comp(ptr + i * size, ptr + (i + 1) * size) == 0) {
            return (void *)(ptr + i * size);
        }
    }
    return NULL;
}

Pair algorithm_mismatch(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFuncBool comp) {
    const char *ptr1 = (const char *)base1;
    const char *ptr2 = (const char *)base2;

    size_t min_num = num1 < num2 ? num1 : num2;

    for (size_t i = 0; i < min_num; i++) {
        if (comp(ptr1 + i * size1, ptr2 + i * size2) != 0) {
            Pair mismatch;
            mismatch.first = (void *)(ptr1 + i * size1);
            mismatch.second = (void *)(ptr2 + i * size2);
            return mismatch;
        }
    }

    Pair mismatch = {NULL, NULL};
    return mismatch;
}

bool algorithm_is_permutation(const void *base1, size_t num1, size_t size1, const void *base2, size_t num2, size_t size2, CompareFunc comp) {
    // Check if both arrays have the same number of elements
    if (num1 != num2 || size1 != size2) {
        return false;
    }

    // Temporary arrays to mark elements as found
    bool found1[num1], found2[num2];
    memset(found1, 0, sizeof(found1));
    memset(found2, 0, sizeof(found2));

    for (size_t i = 0; i < num1; ++i) {
        for (size_t j = 0; j < num2; ++j) {
            // Check if elements match and neither has been paired before
            if (!found1[i] && !found2[j] && comp((char *)base1 + i * size1, (char *)base2 + j * size2)) {
                found1[i] = true;
                found2[j] = true;
                break;
            }
        }
        // If an element in base1 does not have a match in base2
        if (!found1[i]) {
            return false;
        }
    }

    return true;
}

const void* algorithm_search(const void* first1, const void* last1, size_t size1,const void* first2, const void* last2, size_t size2, CompareFuncBool comp) {
    const char* ptr1 = (const char*)first1;
    const char* ptr2 = (const char*)first2;
    const char* end1 = (const char*)last1;

    if (ptr2 == last2) {
        return first1; // If the second sequence is empty, return the start of the first
    }

    while (ptr1 != end1) {
        const char* it1 = ptr1;
        const char* it2 = ptr2;

        while (comp(it1, it2)) { // Compare elements
            it1 += size1;
            it2 += size2;
            if (it2 == last2) {
                return ptr1; // Found a match
            }
            if (it1 == end1) {
                return last1; // Reached the end of the first sequence
            }
        }
        ptr1 += size1;
    }
    return last1; // No match found
}

const void *algorithm_search_n(const void *first, const void* last, size_t size, size_t count, const void *val, CompareFuncBool comp) {
    const char* ptr = (const char*)first;
    const char* end = (const char*)last;

    while (ptr + size * count <= end) { // Ensure there's enough room for 'count' elements
        size_t matched = 0;
        for (size_t i = 0; i < count; ++i) {
            if (comp(ptr + i * size, val)) {
                matched++;
            } 
            else {
                break;
            }
        }

        if (matched == count) {
            return ptr; // Found the subsequence
        }
        ptr += size; // Move to the next element
    }
    return last; // Subsequence not found
}

void *algorithm_remove(void *base, size_t num, size_t size, const void *val, CompareFunc comp) {
    char *ptr = (char *)base;
    size_t new_num = 0;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, val) != 0) {
            if (i != new_num) {
                memcpy(ptr + new_num * size, ptr + i * size, size);
            }
            ++new_num;
        }
    }
    return ptr + new_num * size; // Pointer to the new end of the array
}

void algorithm_remove_copy(const void *source, size_t num, size_t size, void *result, const void *val, CompareFunc comp) {
    const char *src = (const char *)source;
    char *dst = (char *)result;

    for (size_t i = 0; i < num; ++i) {
        if (comp(src + i * size, val) != 0) {
            memcpy(dst, src + i * size, size);
            dst += size;
        }
    }
}


/**
 * Removes elements from the source array based on a given predicate function and copies the remaining elements to the result array.
 *
 * @param source The source array.
 * @param num The number of elements in the source array.
 * @param size The size of each element in bytes.
 * @param result The result array where the remaining elements will be copied.
 * @param pred The predicate function that determines whether an element should be removed or not.
 * @return The number of elements copied to the result array.
 */
size_t algorithm_remove_copy_if(const void *source, size_t num, size_t size, void *result, BoolPredicateFunc pred) {
    if (!source || !result || !pred || size == 0 || num == 0) {
        return 0;
    }

    const char *src = (const char *)source;
    char *dst = (char *)result;
    size_t count = 0;

    for (size_t i = 0; i < num; ++i) {
        if (!pred(src + i * size)) {
            memcpy(dst, src + i * size, size);
            dst += size;
            count++;
        }
    }

    return count;
}

/**
 * Replaces occurrences of a value in an array.
 *
 * This function replaces occurrences of the value `old_val` with the value `new_val` in the array `base`.
 * The number of elements in the array is specified by `num`, and the size of each element is specified by `size`.
 * The comparison function `comp` is used to compare elements in the array.
 *
 * @param base     Pointer to the start of the array.
 * @param num      Number of elements in the array.
 * @param size     Size of each element in bytes.
 * @param old_val  Pointer to the value to be replaced.
 * @param new_val  Pointer to the new value.
 * @param comp     Comparison function used to compare elements.
 */
void algorithm_replace(void *base, size_t num, size_t size, const void *old_val, const void *new_val, CompareFunc comp){
    if (!base || !old_val || !new_val || !comp || size == 0 || num == 0) {
        return;
    }

    char *ptr = (char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (comp(ptr + i * size, old_val) == 0) {
            memcpy(ptr + i * size, new_val, size);
        }
    }
}

/**
 * Replaces elements in an array based on a given predicate function.
 *
 * This function iterates over the elements in the array pointed to by `base` and replaces
 * elements that satisfy the predicate function `pred` with the value pointed to by `new_val`.
 *
 * @param base      Pointer to the start of the array.
 * @param num       Number of elements in the array.
 * @param size      Size of each element in bytes.
 * @param new_val   Pointer to the value to replace the elements with.
 * @param pred      Predicate function that determines if an element should be replaced.
 *
 * @return None.
 */
void algorithm_replace_if(void *base, size_t num, size_t size, const void *new_val, BoolPredicateFunc pred) {
    if (!base || !new_val || !pred || size == 0 || num == 0) {
        return;
    }

    char *ptr = (char *)base;

    for (size_t i = 0; i < num; ++i) {
        if (pred(ptr + i * size)) {
            memcpy(ptr + i * size, new_val, size);
        }
    }
}