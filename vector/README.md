# Vector Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2023
**License:** ISC License

## Overview

This Vector library, written in C, is an implementation that mimics the functionality of the C++ `std::vector`. It aims to provide dynamic array capabilities to C programs. This library is part of a project to reimplement features of the C++ standard library in C, making it useful for developers familiar with C++ but working in C environments.

The Vector library allows for creating dynamic arrays that can resize automatically when elements are added or removed. It supports various operations such as inserting, deleting, and accessing elements, as well as capacity management (reserve / shrink-to-fit) for efficient memory use.

## Key Features

- **Dynamic Resizing:** Automatically adjust size when elements are added or removed.
- **Memory Efficiency:** Elements live in a single contiguous buffer grown with `realloc` (doubling), so peak memory tracks the live element count — the same storage model as C++ `std::vector`. Scales cleanly to 10–100 million elements.
- **Generic Container:** Can store elements of any data type, including structures.
- **Various Utility Functions:** Functions for comparison, swapping, and checking the state of the vector.
- **std::vector speed in hot loops:** `push_back`, `at`, `size`, `data`, `front` / `back`, `pop_back` and `begin` / `end` are `static inline` in the header, so a loop compiles into the caller exactly like `std::vector`. Typed macros (`VECTOR_PUSH_BACK`, `VECTOR_AT`, `VECTOR_FOREACH`, ...) remove the remaining type-erasure cost where the element type is known. See [Performance compared with `std::vector`](#performance-compared-with-stdvector).
- **Everything `std::vector` has, plus more:** range insert / assign / append, fill, C++20 `erase_if` / `erase`, find, sort, binary search, unique, reverse, slices and copies — and things `std::vector` cannot do: adopt a `malloc`'d buffer (`vector_from_buffer`), hand its buffer over (`vector_release`), append uninitialised elements to fill in place (`vector_append_uninit`), O(1) unordered removal (`vector_swap_remove`).
- **A sort faster than `std::sort`:** `VECTOR_DEFINE_SORT` generates a pattern-defeating quicksort for your element type with the comparison inlined — about 2x faster than `std::sort` on random data.
- **Alias-safe:** every function that takes an element or a range may be given a pointer into the same vector (`v.push_back(v[0])`, appending a vector to itself, ...), even when the call reallocates.

## Installation and Compilation

To use this library, include `vector.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O2 -Wall -Wextra -o test.exe test.c vector/vector.c string/std_string.c -I.
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like creating a vector, adding and removing elements, as well as more complex functionalities such as capacity management and vector comparison.

### Function Descriptions:

### `Vector* vector_create(size_t itemSize)`

**Purpose**: Initializes and returns a new vector capable of storing elements of the specified size.

**Parameters**:
- `itemSize`: The size in bytes of each element that will be stored in the vector.

**Return Value**: A pointer to the newly created vector. Returns `NULL` if memory allocation fails.

**Usage Case**: Use at the start to create a typed dynamic array before pushing elements.

---

### `void vector_set_compare(Vector* vec, VectorCompareFunc compare)`

**Purpose**: Sets the element comparator (`int (*)(const void*, const void*)`, qsort convention) used by `vector_is_equal`, `vector_is_less` and the other comparisons. Without a comparator each element's bytes are compared with `memcmp`: fine for equality of plain data and for ordering byte-like elements (`char`, `unsigned char`), but not the numeric order of `int`, negative numbers or `double` (on little-endian `{256}` would sort before `{1}`, and `0.0` would not equal `-0.0`). When only one of the two vectors has a comparator, that one is used. Vectors with different element sizes are never equal and never ordered.

**Parameters**:
- `vec`: The vector (NULL is ignored).
- `compare`: The comparator, or `NULL` for byte comparison.

**Return Value**: None.

**Usage Case**: Call once after `vector_create` when the vector will be compared with the relational functions.

---

### `bool vector_is_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Compares two vectors and checks if they are equal in terms of both size and content.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if both vectors have equal elements in the same order (per the comparator set with `vector_set_compare`, or byte for byte without one), otherwise `false`.

**Usage Case**: Use when you need to verify that two vectors hold the same sequence of elements.

---

### `bool vector_is_less(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically less than the second vector, comparing elements with the comparator set by `vector_set_compare` (or their bytes without one, which is not the numeric order of `int`/`double`).

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is lexicographically smaller than `vec2`, otherwise `false`.

**Usage Case**: Use for sorting or ordering comparisons between two vectors.

---

### `bool vector_is_greater(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically greater than the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is lexicographically greater than `vec2`, otherwise `false`.

**Usage Case**: Use for sorting or ordering comparisons between two vectors.

---

### `bool vector_is_not_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Compares two vectors and checks if they are not equal.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if the vectors are not equal, otherwise `false`.

**Usage Case**: Use when you need to detect differences between two vectors.

---

### `bool vector_is_greater_or_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically greater than or equal to the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is greater than or equal to `vec2`, otherwise `false`.

**Usage Case**: Use in conditional checks where equality or greater order is acceptable.

---

### `bool vector_is_less_or_equal(const Vector* vec1, const Vector* vec2)`

**Purpose**: Checks if the first vector is lexicographically less than or equal to the second vector.

**Parameters**:
- `vec1`: The first vector to compare.
- `vec2`: The second vector to compare.

**Return Value**: `true` if `vec1` is less than or equal to `vec2`, otherwise `false`.

**Usage Case**: Use in conditional checks where equality or lesser order is acceptable.

---

### `bool vector_is_empty(const Vector* vec)`

**Purpose**: Checks if the vector is empty, meaning it contains no elements.

**Parameters**:
- `vec`: The vector to check.

**Return Value**: `true` if the vector is empty, otherwise `false`.

**Usage Case**: Use before accessing elements to guard against operating on an empty vector.

---

### `bool vector_reserve(Vector* vec, size_t size)`

**Purpose**: Allocates enough memory for the vector to hold at least the specified number of elements without resizing.

**Parameters**:
- `vec`: The vector to reserve space for.
- `size`: The minimum number of elements to reserve capacity for.

**Return Value**: `true` if memory allocation is successful, otherwise `false`.

**Usage Case**: Use when you know ahead of time how many elements will be added, to avoid repeated reallocations.

---

### `bool vector_push_back(Vector* vec, const void* item)`

**Purpose**: Adds an item to the end of the vector, resizing the vector if necessary.

**Parameters**:
- `vec`: The vector to which the item is added.
- `item`: A pointer to the item being added to the vector.

**Return Value**: `true` if the operation succeeds, otherwise `false` (e.g., memory allocation failure). A `NULL` `item` is rejected (`false`, nothing added) here and in `vector_emplace_back`; `vector_insert`, `vector_assign` and `vector_emplace` ignore a `NULL` item (they used to copy from it and crash).

**Usage Case**: Use to append elements one at a time to the end of the vector.

---

### `bool vector_emplace_back(Vector* vec, void* item, size_t itemSize)`

**Purpose**: Constructs an item in place at the end of the vector, potentially avoiding copying and allowing more efficient insertion.

**Parameters**:
- `vec`: The vector to add the item to.
- `item`: A pointer to the item to be added.
- `itemSize`: The size of the item being added.

**Return Value**: `true` if successful, otherwise `false`.

**Usage Case**: Use when inserting items at the end with explicit size control for efficiency.

---

### `void vector_erase(Vector* vec, size_t pos, size_t len)`

**Purpose**: Erases a range of elements from the vector, starting at the specified position and removing the specified number of elements.

**Parameters**:
- `vec`: The vector from which elements will be removed.
- `pos`: The starting position of the elements to be removed.
- `len`: The number of elements to erase.

**Return Value**: None.

**Usage Case**: Use to remove a contiguous block of elements from any position in the vector.

---

### `void vector_insert(Vector* vec, size_t pos, void* item)`

**Purpose**: Inserts an item into the vector at the specified position, shifting subsequent elements to the right.

**Parameters**:
- `vec`: The vector where the item will be inserted.
- `pos`: The index at which the item will be inserted.
- `item`: A pointer to the item to insert.

**Return Value**: None.

**Usage Case**: Use to add an element at a specific index rather than at the end.

---

### `bool vector_resize(Vector* vec, size_t size)`

**Purpose**: Resizes the vector to contain the specified number of elements. If the new size is larger, additional elements will be zero-initialized. Growing uses the same doubling policy as `vector_push_back`, so calling `resize(size + 1)` in a loop does not reallocate every time (it used to grow to exactly `size`, one realloc per call).

**Parameters**:
- `vec`: The vector to resize.
- `size`: The new size of the vector.

**Return Value**: `true` on success; `false` if `vec` is NULL or memory could not be allocated (the vector is then unchanged). It used to return nothing and fail silently.

**Usage Case**: Use to explicitly set the number of elements, growing or shrinking the vector.

---

### `void vector_shrink_to_fit(Vector* vec)`

**Purpose**: Shrinks the capacity of the vector to match its current size, freeing any excess allocated memory.

**Parameters**:
- `vec`: The vector to shrink.

**Return Value**: None.

**Usage Case**: Use after many removals to reclaim unused memory.

---

### `void vector_clear(Vector* vec)`

**Purpose**: Clears all elements from the vector, setting its size to 0 but maintaining its current capacity.

**Parameters**:
- `vec`: The vector to clear.

**Return Value**: None.

**Usage Case**: Use to empty the vector for reuse without releasing its allocated memory.

---

### `void vector_swap(Vector* vec1, Vector* vec2)`

**Purpose**: Swaps the contents of two vectors, exchanging their sizes, capacities, and elements.

**Parameters**:
- `vec1`: The first vector.
- `vec2`: The second vector.

**Return Value**: None.

**Usage Case**: Use to exchange the contents of two vectors efficiently without copying elements.

---

### `void vector_assign(Vector* vec, size_t pos, void* item)`

**Purpose**: Assigns a new value to the element at the specified position.

**Parameters**:
- `vec`: The vector to modify.
- `pos`: The position of the element to assign a new value to.
- `item`: A pointer to the new item.

**Return Value**: None.

**Usage Case**: Use to update an existing element at a known index.

---

### `void vector_emplace(Vector* vec, size_t pos, void* item, size_t itemSize)`

**Purpose**: Constructs an item in place at the specified position within the vector.

**Parameters**:
- `vec`: The vector where the item will be constructed.
- `pos`: The index at which to construct the item.
- `item`: A pointer to the item to be placed.
- `itemSize`: The size of the item.

**Return Value**: None.

**Usage Case**: Use to insert an element at an arbitrary position with explicit size control.

---

### `void vector_deallocate(Vector* vec)`

**Purpose**: Frees all memory associated with the vector, including its elements and the vector structure itself.

**Parameters**:
- `vec`: The vector to deallocate.

**Return Value**: None.

**Usage Case**: Call when done using a vector to release all its memory.

---

### `const void* vector_cbegin(Vector* vec)`

**Purpose**: Returns a constant pointer to the first element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to the first element of the vector.

**Usage Case**: Use for read-only forward iteration from the beginning.

---

### `const void* vector_cend(Vector* vec)`

**Purpose**: Returns a constant pointer to the element following the last element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to one past the last element (end sentinel).

**Usage Case**: Use as the end bound for read-only forward iteration.

---

### `const void* vector_crbegin(Vector* vec)`

**Purpose**: Returns a constant pointer to the first element of the reversed vector (i.e., the last element).

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to the last element of the vector.

**Usage Case**: Use for read-only reverse iteration starting from the last element.

---

### `const void* vector_crend(Vector* vec)`

**Purpose**: Returns a constant pointer to the element preceding the first element in the reversed vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A constant pointer to one before the first element (reverse-end sentinel), or `NULL` for an empty vector — the same value `vector_crbegin` returns then, so a `for (it = crbegin; it != crend; --it)` loop runs zero times. It is only a sentinel: never dereference it.

**Usage Case**: Use as the end bound for read-only reverse iteration.

---

### `void* vector_begin(const Vector* vec)`

**Purpose**: Returns a pointer to the first element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the first element.

**Usage Case**: Use to start forward iteration over the vector's elements.

---

### `void* vector_end(const Vector* vec)`

**Purpose**: Returns a pointer to the element following the last element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to one past the last element (end sentinel).

**Usage Case**: Use as the end bound for forward iteration.

---

### `void* vector_pop_back(Vector* vec)`

**Purpose**: Removes the last element from the vector and returns a pointer to it.

**Parameters**:
- `vec`: The vector to modify.

**Return Value**: A pointer to the removed last element.

**Usage Case**: Use to retrieve and remove the last element from the vector.

---

### `void* vector_front(const Vector* vec)`

**Purpose**: Returns a pointer to the first element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the first element.

**Usage Case**: Use when you need to read or modify the first element without removing it.

---

### `void* vector_back(const Vector* vec)`

**Purpose**: Returns a pointer to the last element in the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the last element.

**Usage Case**: Use when you need to read or modify the last element without removing it.

---

### `void* vector_data(const Vector* vec)`

**Purpose**: Returns a pointer to the underlying array used by the vector.

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the underlying array of elements.

**Usage Case**: Use when you need direct access to the raw array, such as for passing to C APIs.

---

### `void* vector_at(const Vector* vec, size_t pos)`

**Purpose**: Returns a pointer to the element at the specified position.

**Parameters**:
- `vec`: The vector to access.
- `pos`: The position of the element to access.

**Return Value**: A pointer to the element at the specified position.

**Usage Case**: Use for random access to any element by index.

---

### `void* vector_rbegin(Vector* vec)`

**Purpose**: Returns a pointer to the last element in the vector (start of reverse iteration).

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to the last element of the vector.

**Usage Case**: Use to start reverse iteration over the vector's elements.

---

### `void* vector_rend(Vector* vec)`

**Purpose**: Returns a pointer to the element preceding the first element of the vector (end of reverse iteration).

**Parameters**:
- `vec`: The vector to access.

**Return Value**: A pointer to one before the first element (reverse-end sentinel), or `NULL` for an empty vector — the same value `vector_rbegin` returns then, so a `for (it = rbegin; it != rend; --it)` loop runs zero times (it used to run once and dereference NULL). Never dereference it.

**Usage Case**: Use as the end bound for reverse iteration.

---

### `size_t vector_size(const Vector* vec)`

**Purpose**: Returns the number of elements currently stored in the vector.

**Parameters**:
- `vec`: The vector whose size is to be returned.

**Return Value**: The number of elements in the vector.

**Usage Case**: Use to check how many elements are currently stored before iterating or accessing.

---

### `size_t vector_capacity(const Vector* vec)`

**Purpose**: Returns the total capacity of the vector (the number of elements it can store without resizing).

**Parameters**:
- `vec`: The vector whose capacity is to be returned.

**Return Value**: The capacity of the vector.

**Usage Case**: Use to understand how much space is pre-allocated before a reallocation occurs.

---

### `size_t vector_max_size(Vector* vec)`

**Purpose**: Returns the maximum number of elements that the vector can hold, based on the system's limitations.

**Parameters**:
- `vec`: The vector whose maximum size is to be returned.

**Return Value**: The maximum number of elements the vector can hold.

**Usage Case**: Use to guard against requesting sizes larger than what the system can support.

---

### `size_t vector_item_size(const Vector* vec)`

**Purpose**: Returns the size in bytes of one element (the `itemSize` the vector was created with).

**Parameters**:
- `vec`: The vector (may be `NULL`, which gives 0).

**Return Value**: The element size in bytes.

**Usage Case**: Generic code that receives a `Vector*` and needs to step through `vector_data` by hand.

---

### `Vector* vector_create_with_capacity(size_t itemSize, size_t capacity)`

**Purpose**: Creates an empty vector with room for `capacity` elements. `capacity == 0` allocates no element buffer at all (it is created by the first insertion), which is what an empty `std::vector` costs; `vector_create` reserves 32 elements.

**Parameters**:
- `itemSize`: Size of one element in bytes (> 0).
- `capacity`: Number of elements to reserve.

**Return Value**: The new vector, or `NULL` for `itemSize == 0` / out of memory.

**Usage Case**: When the final size is known in advance (one allocation, no regrowth), or for many small or often-empty vectors.

---

### `Vector* vector_from_array(size_t itemSize, const void* data, size_t count)`

**Purpose**: Creates a vector holding a copy of `count` elements from `data` (the range constructor of `std::vector`), with one allocation and one copy.

**Parameters**:
- `itemSize`: Size of one element in bytes.
- `data`: The elements to copy (may be `NULL` only when `count` is 0).
- `count`: Number of elements.

**Return Value**: The new vector (capacity == `count`), or `NULL` on invalid arguments / out of memory.

**Usage Case**: Turning a C array or a buffer received from another API into a vector.

---

### `Vector* vector_from_buffer(void* buffer, size_t itemSize, size_t count, size_t capacity)`

**Purpose**: Wraps an existing `malloc`'d buffer in a vector **without copying** it. The vector takes ownership: it reallocs and frees the buffer from then on. `std::vector` cannot adopt a buffer.

**Parameters**:
- `buffer`: Memory from `malloc` / `calloc` / `realloc` holding `count` elements with room for `capacity` (`NULL` only when `capacity` is 0).
- `itemSize`: Size of one element (> 0).
- `count`: Number of valid elements (`<= capacity`).
- `capacity`: Number of elements the buffer can hold.

**Return Value**: The vector, or `NULL` for invalid arguments / out of memory — the buffer then still belongs to the caller.

**Usage Case**: Taking over an array produced by a parser, a file reader or `vector_release`.

---

### `Vector* vector_copy(const Vector* src)`

**Purpose**: Deep copy: a new vector with the same elements, element size and comparator (the copy constructor of `std::vector`; capacity == size).

**Parameters**:
- `src`: The vector to copy.

**Return Value**: The copy, or `NULL` for a `NULL` source / out of memory.

**Usage Case**: Keeping a snapshot before modifying a vector.

---

### `Vector* vector_slice(const Vector* vec, size_t pos, size_t count)`

**Purpose**: Creates a new vector with a copy of the elements `[pos, pos + count)`.

**Parameters**:
- `vec`: The source vector.
- `pos`: First element of the range.
- `count`: Number of elements.

**Return Value**: The new vector (with the source's comparator), or `NULL` when the range does not lie inside the vector or memory runs out.

**Usage Case**: Splitting work into chunks, or taking a sub-range of results.

---

### `void* vector_release(Vector* vec, size_t* count)`

**Purpose**: Hands the element buffer to the caller, who must `free()` it. The vector stays valid and becomes empty, with no buffer. A zero-copy way to pass the elements to an API that takes ownership of an array; `std::vector` cannot release its storage.

**Parameters**:
- `vec`: The vector.
- `count`: Receives the number of elements in the returned buffer (may be `NULL`).

**Return Value**: The buffer (`NULL` when the vector had none or `vec` is `NULL`).

**Usage Case**: Returning the result of a computation as a plain array, or moving it into another vector with `vector_from_buffer`.

---

### `bool vector_resize_with(Vector* vec, size_t size, const void* fill)`

**Purpose**: Like `vector_resize`, but new elements are copies of `*fill` (`std::vector::resize(n, value)`). `fill` may point into the vector; `NULL` zero-fills like `vector_resize`.

**Parameters**:
- `vec`: The vector.
- `size`: The new number of elements.
- `fill`: The value for new elements.

**Return Value**: `true` on success; `false` (vector unchanged) for `NULL` / out of memory.

**Usage Case**: Growing a vector to a fixed size with a default value such as -1.

---

### `void* vector_emplace_back_slot(Vector* vec)`

**Purpose**: Appends a new zero-filled element and returns a pointer to it, so a large element can be built in place instead of being built elsewhere and copied in (the C form of `emplace_back`).

**Parameters**:
- `vec`: The vector.

**Return Value**: The new last element (valid until the vector grows again), or `NULL` for a `NULL` vector / out of memory.

**Usage Case**: Appending big structs field by field.

---

### `bool vector_insert_range(Vector* vec, size_t pos, const void* data, size_t count)`

**Purpose**: Inserts copies of `count` elements from `data` before position `pos` with a single shift (`std::vector::insert(pos, first, last)`). `data` may point into the vector.

**Parameters**:
- `vec`: The vector.
- `pos`: Insert position (`0 .. size`).
- `data`: The elements to insert.
- `count`: Number of elements.

**Return Value**: `true` on success (also for `count == 0`); `false` (vector unchanged) for `NULL` arguments, `pos > size` or out of memory.

**Usage Case**: Inserting a block of elements: inserting them one at a time with `vector_insert` shifts the tail once per element.

---

### `bool vector_insert_fill(Vector* vec, size_t pos, size_t count, const void* item)`

**Purpose**: Inserts `count` copies of `*item` before position `pos` (`std::vector::insert(pos, n, value)`). `item` may be one of the vector's own elements.

**Parameters**:
- `vec`: The vector.
- `pos`: Insert position (`0 .. size`).
- `count`: Number of copies.
- `item`: The value to insert.

**Return Value**: `true` on success (also for `count == 0`); `false` (vector unchanged) otherwise.

**Usage Case**: Padding a sequence in the middle.

---

### `bool vector_assign_range(Vector* vec, const void* data, size_t count)`

**Purpose**: Replaces the contents with copies of `count` elements from `data` (`std::vector::assign(first, last)`). `data` may point into the vector (keeping a sub-range).

**Parameters**:
- `vec`: The vector.
- `data`: The new elements.
- `count`: Number of elements.

**Return Value**: `true` on success; `false` (vector unchanged) for `NULL` arguments / out of memory.

**Usage Case**: Reusing one vector (and its buffer) for a series of inputs.

---

### `bool vector_assign_fill(Vector* vec, size_t count, const void* item)`

**Purpose**: Replaces the contents with `count` copies of `*item` (`std::vector::assign(n, value)`). `item` may be one of the vector's elements. An all-zero value into a new buffer uses `calloc` (no fill pass); a value whose bytes are all equal becomes one `memset`.

**Parameters**:
- `vec`: The vector.
- `count`: The new number of elements.
- `item`: The value.

**Return Value**: `true` on success; `false` (vector unchanged) for `NULL` arguments / out of memory.

**Usage Case**: Initialising a table of counters or flags.

---

### `void vector_fill(Vector* vec, const void* item)`

**Purpose**: Overwrites every element with a copy of `*item` (`std::fill` over the vector); `item` may be one of the vector's elements.

**Parameters**:
- `vec`: The vector.
- `item`: The value.

**Return Value**: None.

**Usage Case**: Resetting all elements without changing the size.

---

### `bool vector_append(Vector* vec, const void* data, size_t count)`

**Purpose**: Appends copies of `count` elements from `data` with one growth step and one copy (`insert(end(), first, last)` / C++23 `append_range`). `data` may point into the vector: `vector_append(v, vector_data(v), vector_size(v))` doubles it.

**Parameters**:
- `vec`: The vector.
- `data`: The elements to append.
- `count`: Number of elements.

**Return Value**: `true` on success (also for `count == 0`); `false` (vector unchanged) for `NULL` arguments / out of memory.

**Usage Case**: Concatenating arrays; much faster than a loop of `vector_push_back`.

---

### `void* vector_append_uninit(Vector* vec, size_t count)`

**Purpose**: Adds `count` elements at the end **without initialising them** and returns a pointer to the first, to be filled in place. Filling a vector this way runs at the speed of a plain array loop; `std::vector` has no equivalent (`resize` first writes every element, C++23 `resize_and_overwrite` exists only for `std::string`). The typed form is `VECTOR_APPEND_UNINIT(v, T, n)`.

**Parameters**:
- `vec`: The vector.
- `count`: Number of new elements.

**Return Value**: The first new element (valid until the vector grows again), or `NULL` for a `NULL` vector / out of memory (vector unchanged). For `count == 0` the end of the elements is returned (`NULL` when the vector has no buffer yet).

**Usage Case**: Generating or decoding many elements: write them straight into the vector.

---

### `bool vector_append_vector(Vector* dst, const Vector* src)`

**Purpose**: Appends all elements of `src` to `dst`; `src` may be `dst` itself.

**Parameters**:
- `dst`: The vector to extend.
- `src`: The vector to copy from (same element size).

**Return Value**: `true` on success; `false` for `NULL` vectors, different element sizes or out of memory (`dst` unchanged).

**Usage Case**: Merging partial results.

---

### `bool vector_swap_remove(Vector* vec, size_t pos)`

**Purpose**: Removes the element at `pos` in O(1) by moving the last element into its place. The order of the elements is not kept; `std::vector::erase` is O(n).

**Parameters**:
- `vec`: The vector.
- `pos`: Index of the element to remove.

**Return Value**: `true` when an element was removed; `false` for `NULL` / `pos >= size`.

**Usage Case**: Unordered collections such as particle or entity lists, where removing from the middle must be cheap.

---

### `bool vector_pop_back_value(Vector* vec, void* out)`

**Purpose**: Removes the last element, copying it to `out` first. Unlike `vector_pop_back` (which returns a pointer into the buffer that the next insertion overwrites), the value is safe to keep.

**Parameters**:
- `vec`: The vector.
- `out`: Receives the element (may be `NULL` to just drop it).

**Return Value**: `true` when an element was removed; `false` for a `NULL` or empty vector.

**Usage Case**: Using the vector as a stack: `while (vector_pop_back_value(v, &x)) { ... }`.

---

### `size_t vector_erase_if(Vector* vec, VectorPredicateFunc pred, void* ctx)`

**Purpose**: Removes every element for which `pred(element, ctx)` returns `true`, keeping the order of the rest, in one pass (C++20 `std::erase_if`). With a known element type, `VECTOR_ERASE_IF` does the same with the condition inlined.

**Parameters**:
- `vec`: The vector.
- `pred`: `bool pred(const void* item, void* ctx)`.
- `ctx`: Passed to `pred` unchanged (e.g. a threshold).

**Return Value**: The number of elements removed.

**Usage Case**: Filtering: calling `vector_erase` for each element instead is O(n²).

---

### `size_t vector_remove_value(Vector* vec, const void* value)`

**Purpose**: Removes every element equal to `*value` (C++20 `std::erase`), using the comparator from `vector_set_compare` or, without one, the bytes (a fast specialised scan for 1/2/4/8-byte elements). `value` may be one of the vector's elements.

**Parameters**:
- `vec`: The vector.
- `value`: The value to remove.

**Return Value**: The number of elements removed.

**Usage Case**: Removing all occurrences of an ID or a sentinel.

---

### `size_t vector_find(const Vector* vec, const void* value)`

**Purpose**: Linear search for the first element equal to `*value` (comparator from `vector_set_compare`, else the bytes; byte comparison of 1/2/4/8-byte elements is unrolled like `std::find` and about 6x faster than a `memcmp` per element).

**Parameters**:
- `vec`: The vector.
- `value`: The value to look for.

**Return Value**: Its index, or `VECTOR_NPOS`.

**Usage Case**: Membership tests and lookups in small or unsorted vectors.

---

### `bool vector_contains(const Vector* vec, const void* value)`

**Purpose**: `true` when an element equal to `*value` is present (see `vector_find`).

**Parameters**:
- `vec`: The vector.
- `value`: The value to look for.

**Return Value**: `true` if found.

**Usage Case**: Avoiding duplicates before a push.

---

### `bool vector_sort(Vector* vec, VectorCompareFunc compare)`

**Purpose**: Sorts the elements in ascending order of `compare` (a qsort-style comparator; `NULL` uses the one from `vector_set_compare`). Not stable. For speed, `VECTOR_DEFINE_SORT` generates a sort for the element type that is about 4x faster.

**Parameters**:
- `vec`: The vector.
- `compare`: `int compare(const void* a, const void* b)`.

**Return Value**: `true` when sorted; `false` for a `NULL` vector or when no comparator is available.

**Usage Case**: Sorting with a runtime-chosen comparator.

---

### `size_t vector_binary_search(const Vector* vec, const void* value, VectorCompareFunc compare)`

**Purpose**: Binary search in a vector sorted ascending by `compare` (`NULL`: the comparator from `vector_set_compare`); O(log n).

**Parameters**:
- `vec`: The sorted vector.
- `value`: The value to look for.
- `compare`: The comparator the vector is sorted by.

**Return Value**: The index of the **first** element equal to `*value` (like `std::lower_bound` followed by a check), or `VECTOR_NPOS` (also when no comparator is available).

**Usage Case**: Fast lookups in a sorted vector.

---

### `void vector_reverse(Vector* vec)`

**Purpose**: Reverses the order of the elements in place (`std::reverse`).

**Parameters**:
- `vec`: The vector.

**Return Value**: None.

**Usage Case**: Turning an ascending sort into a descending one.

---

### `size_t vector_unique(Vector* vec)`

**Purpose**: Removes consecutive duplicate elements, keeping the first of each run (`std::unique` followed by `erase`); sort first to remove all duplicates. Equality: the comparator from `vector_set_compare`, else the bytes.

**Parameters**:
- `vec`: The vector.

**Return Value**: The number of elements removed.

**Usage Case**: Deduplicating a sorted list.

---

### `void vector_for_each(Vector* vec, VectorVisitFunc fn, void* ctx)`

**Purpose**: Calls `fn(element, ctx)` for every element, front to back.

**Parameters**:
- `vec`: The vector.
- `fn`: `void fn(void* item, void* ctx)`; it may modify the element.
- `ctx`: Passed to `fn` unchanged.

**Return Value**: None.

**Usage Case**: Applying an operation to every element through a callback. With a known element type, `VECTOR_FOREACH` is the inlined form.

---

## Typed fast paths (macros)

The functions above work with `void*` and a runtime element size. That costs a little in tight loops: the element copy goes through `memcpy`, which may alias anything, so the compiler must reload the vector's fields after every store. These macros take the element **type** and use typed loads and stores, so the compiler can keep everything in registers, exactly like `std::vector<T>`. Use them where the element type is known.

| Macro | Meaning |
| --- | --- |
| `VECTOR_PUSH_BACK(v, T, value)` | append `value`; `true`, or `false` when memory runs out |
| `VECTOR_AT(v, T, i)` | element `i` as an lvalue, unchecked (`operator[]`) |
| `VECTOR_DATA(v, T)` | `T*` to the buffer |
| `VECTOR_FRONT(v, T)` / `VECTOR_BACK(v, T)` | first / last element (lvalue, unchecked) |
| `VECTOR_POP_BACK(v, T)` | remove and return the last element (unchecked) |
| `VECTOR_APPEND_UNINIT(v, T, n)` | `T*` to `n` new uninitialised elements at the end (`NULL` when memory runs out) |
| `VECTOR_FOREACH(v, T, p)` | `for (T* p ...)` over every element |
| `VECTOR_ERASE_IF(v, T, x, cond)` | remove every element `x` for which `cond` is true, keeping the order |
| `VECTOR_DEFINE_SORT(name, T, LESS)` | defines `name(Vector*)` and `name_array(T*, size_t)`: a typed sort (below) |

Rules: `v` is evaluated more than once (pass a plain variable), `T` must be the element type the vector was created with (`sizeof(T) == itemSize`), and `value` / `cond` are evaluated once per call / per element.

```c
Vector* v = vector_create(sizeof(int));
for (int i = 0; i < 1000; i++) {
    VECTOR_PUSH_BACK(v, int, i * i);
}
long long sum = 0;
VECTOR_FOREACH(v, int, p) {
    sum += *p;
}
VECTOR_ERASE_IF(v, int, x, x % 2 != 0);     /* keep the even squares */
```

### Sorting with `VECTOR_DEFINE_SORT`

`vector_sort` goes through `qsort` and calls the comparator through a function pointer for every comparison. `VECTOR_DEFINE_SORT(name, T, LESS)` instead generates a sort for the element type, at file scope, with the comparison inlined — the way `std::sort` inlines a lambda:

```c
typedef struct { const char* name; int score; } Player;
#define BY_SCORE(a, b) ((a).score < (b).score)
VECTOR_DEFINE_SORT(sort_players, Player, BY_SCORE)   /* sort_players(Vector*), sort_players_array(Player*, size_t) */

#define INT_LESS(a, b) ((a) < (b))
VECTOR_DEFINE_SORT(sort_ints, int, INT_LESS)
```

`LESS(a, b)` receives two elements (lvalues of type `T`) and is true when `a` goes before `b`; it must be a strict weak order, as for `std::sort`. The algorithm is **pattern-defeating quicksort** (pdqsort, by Orson Peters — the unstable sort of Rust and Go): introsort with branchless block partitioning, a ninther pivot, pattern breaking, special handling of runs of equal elements and an early exit for sorted input. It is O(n log n) in the worst case (heapsort fallback), O(n) for sorted, reversed or all-equal input, not stable, and allocates nothing.

---

## Performance compared with `std::vector`

Same workloads in C and C++ (one non-inlined function per workload, identical data and checksums), MinGW-w64 GCC / G++ 14.2 `-O2`, Windows; best of 15 runs, minimum over 3 interleaved rounds. `C ÷ C++` below 1.0 means the C vector is faster. "generic" is the `void*` API, "typed" the macros above.

| Workload (`int` unless noted) | C | C++ `std::vector` | C ÷ C++ |
| --- | --: | --: | :--: |
| `push_back` 10M, generic / typed | 0.037 / 0.031 s | 0.029 s | 1.29 / 1.08 |
| `reserve` + `push_back` 10M, generic / typed | 0.021 / 0.015 s | 0.013 s | 1.72 / 1.20 |
| fill 10M computed values (`VECTOR_APPEND_UNINIT` vs `resize` + `[]`) | **0.0099 s** | 0.0122 s | **0.82** |
| indexed read 10M, generic / `VECTOR_FOREACH` | 0.0046 s | 0.0046 s | 1.00 |
| `push_back` 2M 64-byte structs | 0.065 s | 0.064 s | 1.00 |
| insert at front 30k / erase at front 30k | 0.038 / 0.037 s | 0.038 / 0.038 s | 0.99 / 0.98 |
| copy 10M / append a 10M range | 0.0089 / 0.0091 s | 0.0088 / 0.0091 s | 1.02 / 1.00 |
| `pop_back` 10M, generic / typed | 0.0140 / 0.0137 s | 0.0140 s | 1.00 / 0.98 |
| `resize` 10M (zero-filled) | **0.0060 s** | 0.0077 s | **0.78** |
| `assign` 10M copies | **0.0086 s** | 0.0089 s | **0.97** |
| `erase_if` half of 10M, callback / `VECTOR_ERASE_IF` | 0.030 / 0.019 s | 0.019 s | 1.58 / 1.00 |
| 100k vectors of 16 elements | **0.010 s** | 0.024 s | **0.43** |
| linear find x20 in 10M | **0.063 s** | 0.071 s | **0.89** |
| sort 5M random ints (`VECTOR_DEFINE_SORT` vs `std::sort`) | **0.157 s** | 0.317 s | **0.50** |
| sort 5M ints with 10 distinct values | **0.025 s** | 0.102 s | **0.24** |
| sort 5M sorted / reversed ints | **0.0044 / 0.010 s** | 0.051 / 0.038 s | **0.09 / 0.27** |
| sort 500k 64-byte structs by key | **0.032 s** | 0.040 s | **0.80** |

- **Faster than `std::vector`:** zero-initialised growth (`resize`, `assign_fill` with zero) comes from `calloc` — the operating system hands out pages that are already zero, so there is no fill pass; `VECTOR_APPEND_UNINIT` lets you write elements in place without the zero-fill that `std::vector::resize` must do; many small vectors are cheaper because the first allocation already holds 32 elements (`std::vector` reallocates at 1, 2, 4, 8 and 16); `vector_find` / `vector_remove_value` use unrolled integer scans; and the generated sort is pdqsort, which beats libstdc++'s introsort on every input above. The one measured sort input where `std::sort` is faster is an interleaved ascending/descending ("organ pipe") sequence: 0.070 s against 0.037 s.
- **At parity:** reads, copies, appends, insert / erase, pop, and pushes of larger elements.
- **Still slower — per-element `push_back` into a growing vector.** `std::vector` keeps its begin / end pointers in registers across the whole loop. The C `Vector` is a heap object with public fields whose growth function is out of line, so every push must store the new size back to memory and reload the buffer pointer: about 8–20% with the typed macro, more with the generic API (a runtime element size and a `memcpy` that may alias the vector's own fields). For bulk work use `vector_append`, `vector_append_uninit` / `VECTOR_APPEND_UNINIT` or `vector_insert_range`, which are as fast as or faster than `std::vector`. A callback-based `vector_erase_if` pays one indirect call per element; `VECTOR_ERASE_IF` inlines the condition and is at parity.

---

## Examples

Several examples are provided to demonstrate the usage of the Vector library in various scenarios, including handling primitive data types, strings, and custom structures. These examples also show how to perform operations like iteration, sorting, and comparison on vectors.


```c
#include "vector/vector.h"
```

## Example 1: Integers

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector* intVector = vector_create(sizeof(int));
    int value = 10;
    vector_push_back(intVector, &value);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*)vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
10
```

---

## Example 2: Strings

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector* stringVector = vector_create(sizeof(char*));
    char *str = "Hello, World!";

    vector_push_back(stringVector, &str);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(stringVector); ++i) {
        char** item = (char**)vector_at(stringVector, i);
        fmt_printf("%s\n", *item);
    }

    // Cleanup
    vector_deallocate(stringVector);
    return 0;
}
```
**Result**
```
Hello, World!
```

---

## Example 3: User-Defined Structures

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

typedef struct {
    int id;
    double value;

} MyStruct;

int main() {   
    Vector* structVector = vector_create(sizeof(MyStruct));
    MyStruct s = {1, 10.5};

    vector_push_back(structVector, &s);

    // Iterate over the vector
    for (size_t i = 0; i < vector_size(structVector); ++i) {
        MyStruct* item = (MyStruct*)vector_at(structVector, i);
        fmt_printf("ID: %d, Value: %.2f\n", item->id, item->value);
    }

    vector_deallocate(structVector);   
    return 0;
}
```
**Result**
```
ID: 1, Value: 10.50
```

---

## Example 4: Use relational operators

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector* vec1 = vector_create(sizeof(int));
    Vector* vec2 = vector_create(sizeof(int));
    int items1[] = {1, 2, 3};
    int items2[] = {1, 2, 4};

    for (int i = 0; i < 3; i++) {
        vector_push_back(vec1, &items1[i]);
    }
    for (int i = 0; i < 3; i++) {
        vector_push_back(vec2, &items2[i]);
    }

    if (vector_is_equal(vec1, vec2)) { 
        fmt_printf("vec1 is equal to vec2\n");
    }
    else {
        fmt_printf("vec1 is not equal to vec2\n");
    }

    if (vector_is_less(vec1, vec2)) { 
        fmt_printf("vec1 is less than vec2\n");
    }
    if (vector_is_greater(vec2, vec1)) {
        fmt_printf("vec2 is greater than vec1\n");
    }
    // Cleanup
    vector_deallocate(vec1);
    vector_deallocate(vec2);
        
    return 0;
}
```
**Result**
```
vec1 is not equal to vec2
vec1 is less than vec2
vec2 is greater than vec1
```

---

## Example 5: Use at and char*

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int value1 = 10, value2 = 20, value3 = 30;

    vector_push_back(intVector, &value1);
    vector_push_back(intVector, &value2);
    vector_push_back(intVector, &value3);

    for (size_t i = 0; i < (vector_size(intVector)); ++i) {
        int* item = (int*)vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    Vector *stringVector = vector_create(sizeof(char*));
    const char *str1 = "Hello";
    const char *str2 = "World";
    const char *str3 = "Example";

    vector_push_back(stringVector, &str1);
    vector_push_back(stringVector, &str2);
    vector_push_back(stringVector, &str3);

    for (size_t i = 0; i < vector_size(stringVector); ++i) {
        char **item = (char**) vector_at(stringVector, i);
        fmt_printf("%s\n", *item);
    }

    vector_clear(intVector);
    vector_deallocate(intVector);
    vector_clear(stringVector);
    vector_deallocate(stringVector);
    
    return 0;
}
```
**Result**
```
10
20
30
Hello
World
Example
```

---

## Example 6: how to use 'swap'

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *vector1 = vector_create(sizeof(int));
    Vector *vector2 = vector_create(sizeof(int));
    int value1 = 10, value2 = 20;

    vector_push_back(vector1, &value1);
    vector_push_back(vector1, &value2);

    int value3 = 30, value4 = 40;
    vector_push_back(vector2, &value3);
    vector_push_back(vector2, &value4);

    vector_swap(vector1, vector2);

    fmt_printf("Contents of vector1 after swap:\n");
    for (size_t i = 0; i < vector_size(vector1); ++i) {
        int* item = (int*) vector_at(vector1, i);
        fmt_printf("%d\n", *item);
    }

    fmt_printf("Contents of vector2 after swap:\n");
    for (size_t i = 0; i < vector_size(vector2); ++i) {
        int* item = (int*)vector_at(vector2, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);   

    return 0;
}
```
**Result**
```
Contents of vector1 after swap:
30
40
Contents of vector2 after swap:
10
20
```

---

## Example 7: how to use `vector_pop_back`

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40};

    for (int i = 0; i < 4; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Pop the last element and access it
    int *poppedItem = (int *)vector_pop_back(intVector);
    if (poppedItem) {
        fmt_printf("Popped item: %d\n", *poppedItem);
    }

    vector_deallocate(intVector);       
    return 0;
}
```
**Result**
```
Popped item: 40
```

---

## Example 8: how to use `vector_front`

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    int *frontItem = (int *)vector_front(intVector);
    if (frontItem) {
        fmt_printf("First item: %d\n", *frontItem);
    }

    vector_deallocate(intVector);        
    return 0;
}
```
**Result**
```
First item: 10
```

---

## Example 9: how to use `vector_back`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) {
        vector_push_back(intVector, &values[i]);
    }

    int *backItem = (int *)vector_back(intVector);
    if (backItem) { 
        fmt_printf("Last item: %d\n", *backItem);
    }

    vector_deallocate(intVector);        
    return 0;
}
```
**Result**
```
Last item: 30
```

---

## Example 10: use `vector_data` Returns pointer to the underlying array serving as element storage
```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Accessing the underlying array
    int *data = (int *)vector_data(intVector);
    if (data){
        for (size_t i = 0; i < vector_size(intVector); ++i) { 
            fmt_printf("Item %zu: %d\n", i, data[i]);
        }
    }
        
    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
Item 0: 10
Item 1: 20
Item 2: 30
```

---

## Example 11: `vector_begin`, `vector_end` Return pointer to the first and last element

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using begin and end to iterate over the vector
    for (int *it = (int *)vector_begin(intVector); it != (int *)vector_end(intVector); ++it) { 
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
10
20
30
```

---

## Example 12: `vector_cbegin`, `vector_cend` Return const pointer to first and last element

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using cbegin and cend for read-only iteration over the vector
    for (const int *it = (const int *)vector_cbegin(intVector); it != (const int *)vector_cend(intVector); ++it) {
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
10
20
30
```

---

## Example 13 : `vector_crbegin`, `vector_crend` Return reverse read-only iteration over the vector
```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using crbegin and crend for reverse read-only iteration over the vector
    for (const int *it = (const int *)vector_crbegin(intVector); it != (const int *)vector_crend(intVector); --it) {
        fmt_printf("%d\n", *it);
    }
        
    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
30
20
10
```

---

## Example 14: Using `vector_rbegin` and `vector_rend` for reverse iteration over the vector

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Using rbegin and rend for reverse iteration over the vector
    for (int *it = (int *)vector_rbegin(intVector); it != (int *)vector_rend(intVector); --it) { 
        fmt_printf("%d\n", *it);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
30
20
10
```

---

## Example 15: `vector_insert` New element to vector 

```c 
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Insert a new element
    int newElement = 25;
    vector_insert(intVector, 0, &newElement); // Inserts 25 at position 0
    
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
25
10
20
30
```

---

## Example 16: `vector_erase` element from vector

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {   
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; ++i) { 
        vector_push_back(intVector, &values[i]);
    }
    // Erase elements from position 1, removing 2 elements
    vector_erase(intVector, 1, 2); // Should remove 20 and 30

    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item);
    }

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
10
40
50
```

---

## Example 17: `vector_reserve`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *intVector = vector_create(sizeof(int));

    // Reserve capacity for at least 10 elements
    vector_reserve(intVector, 10);
    for (int i = 0; i < 11; ++i) {
        int value = i * 10;
        vector_push_back(intVector, &value);
    }
    fmt_printf("Vector size: %zu, Vector capacity: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
Vector size: 11, Vector capacity: 32
```

---

## Example 18: resize size of the vector with `vector_resize` 

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *intVector = vector_create(sizeof(int));
    int values[] = {10, 20, 30};

    for (int i = 0; i < 3; ++i) { 
        vector_push_back(intVector, &values[i]);
    }

    // Resize to a larger size (5)
    vector_resize(intVector, 5);
    fmt_printf("After resizing to larger size:\n");

    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item); 
    }

    // Resize to a smaller size (2)
    vector_resize(intVector, 2);

    fmt_printf("After resizing to smaller size:\n");
    for (size_t i = 0; i < vector_size(intVector); ++i) {
        int* item = (int*) vector_at(intVector, i);
        fmt_printf("%d\n", *item); // Only the first two elements remain
    }

    vector_clear(intVector);
    vector_deallocate(intVector);

    return 0;
}
```
**Result**
```
After resizing to larger size:
10
20
30
0
0
After resizing to smaller size:
10
20
```

---

## Example 19 : `vector_shrink_to_fit` fixed capacity

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *intVector = vector_create(sizeof(int));

    // Reserve larger capacity
    vector_reserve(intVector, 10);

    int values[] = {10, 20, 30};
    for (int i = 0; i < 3; ++i) {
        vector_push_back(intVector, &values[i]);
    }

    fmt_printf("Size before shrink_to_fit: %zu, Capacity before shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    // Shrink to fit the actual number of elements
    vector_shrink_to_fit(intVector);    
    fmt_printf("Size after shrink_to_fit: %zu, Capacity after shrink_to_fit: %zu\n", vector_size(intVector), vector_capacity(intVector));

    vector_deallocate(intVector);
    return 0;
}
```
**Result**
```
Size before shrink_to_fit: 3, Capacity before shrink_to_fit: 32
Size after shrink_to_fit: 3, Capacity after shrink_to_fit: 3
```

---

## Example 20 : use `vector_emplace` and `vector_emplace_back` insert new element at positin and at the end of 

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *stringVector = vector_create(sizeof(char*));
    char* values[] = {"Hello", "World", "Vector", "Example"};

    for (int i = 0; i < 4; ++i) { 
        vector_push_back(stringVector, &values[i]);
    }

    char* newValue = "NewString";
    vector_assign(stringVector, 1, &newValue);

    char* emplaceValue = "EmplacedString";
    vector_emplace(stringVector, 2, &emplaceValue, sizeof(char*));

    char* emplaceBackValue = "EmplacedBackString";
    vector_emplace_back(stringVector, &emplaceBackValue, sizeof(char*));

    for (size_t i = 0; i < vector_size(stringVector); ++i) {
        char** item = (char **)vector_at(stringVector, i);
        fmt_printf("%s\n", *item);
    }

    vector_deallocate(stringVector);
    return 0;
}
```
**Result**
```
Hello
NewString
EmplacedString
Vector
Example
EmplacedBackString
```

---

## Example 21 : how to use `String` object in Vector

```c
#include "string/std_string.h"
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector* vec = vector_create(sizeof(String*));
    String* myString1 = string_create("Hello");
    String* myString2 = string_create("World");
    String* myString3 = string_create("Example");
    String** strPtr = NULL;

    vector_push_back(vec, &myString1);
    vector_push_back(vec, &myString2);
    vector_push_back(vec, &myString3);

    for (size_t i = 0; i < vector_size(vec); ++i) {
        strPtr = (String**) vector_at(vec, i);
        if (strPtr && *strPtr) {
            fmt_printf("%s\n", (*strPtr)->dataStr); 
        }
    }

    string_deallocate(myString1);
    string_deallocate(myString2);
    string_deallocate(myString3);

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Hello
World
Example
```

---

## Example 22 : how to use relationals operators in `Vector` 

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    Vector *vector1 = vector_create(sizeof(int));
    Vector *vector2 = vector_create(sizeof(int));

    int value1 = 10, value2 = 20;
    vector_push_back(vector1, &value1);
    vector_push_back(vector1, &value2);

    int value3 = 30, value4 = 40;
    vector_push_back(vector2, &value3);
    vector_push_back(vector2, &value4);
    
    if (vector_is_equal(vector1, vector2)) {
        fmt_printf("Vector1 is equal with Vector2\n");
    }
    if (vector_is_less(vector1, vector2)) {
        fmt_printf("Vector1 is less than Vector2\n");
    }
    if (vector_is_greater(vector1, vector2)) {
        fmt_printf("Vector1 is greater than Vector2\n");
    }

    vector_deallocate(vector1);
    vector_deallocate(vector2);

    return 0;
}
```
**Result**
```
Vector1 is less than Vector2
```

---

## Example 23 : how to use `vector_begin` and `vector_end` with String Object

```c
#include "string/std_string.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {

    Vector *vec = vector_create(sizeof(String*));
    String *fruits[5] = {
        string_create("Apple"),
        string_create("Banana"),
        string_create("Cherry"),
        string_create("Lemon"),
        string_create("Watermelon")
    };
    String *concat = string_create("");

    for (size_t index = 0; index < 5; index++) {
        vector_push_back(vec, &fruits[index]);
    }

    Time* start_time = time_current_time();

    for (size_t index = 0; index < vector_size(vec); ++index) {
        String **strPtr = (String**)vector_at(vec, index);
        string_push_back(*strPtr, '\n');
        string_concatenate(concat, *strPtr);
    }
    
    for (size_t index = 0; index < 5; index++) {
        string_deallocate(fruits[index]);
    }

    vector_deallocate(vec);

    Time* end_time = time_current_time();
    double time_elapsed = time_diff_in_seconds(start_time, end_time);
    
    fmt_printf("%s", string_c_str(concat));
    fmt_printf("Time taken: %lf seconds\n", time_elapsed);

    time_deallocate(start_time);
    time_deallocate(end_time);
    string_deallocate(concat);
    return 0;
}
```
**Result in C:**
```
Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 0.000009 seconds
```

`also result in Cpp` 

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>

int main() {
    std::vector<std::string> fruits = {"Apple", "Banana", "Cherry", "Lemon", "Watermelon"};
    std::stringstream buffer;

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& fruit : fruits) { 
        buffer << fruit << "\n";
    }
    // Stop timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << buffer.str(); // Single print call
    std::cout << "Time taken: " << elapsed.count() << " seconds\n";

    return 0;
}
```
**Result in C++:**
```
Apple
Banana
Cherry
Lemon
Watermelon
Time taken: 1.2074e-05 seconds
```

---

## Example 24: Vector of Vectors (2D Vector)

This example creates a two-dimensional vector, where each element of the main vector is another vector. This can be useful for matrix-like structures or grid representations.
```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main() {
    // Create a vector of vectors (2D vector)
    Vector* vec2D = vector_create(sizeof(Vector*));

    for (int i = 0; i < 3; i++) {
        Vector* subVec = vector_create(sizeof(int));
        for (int j = 0; j < 3; j++) {
            int value = i * 3 + j;
            vector_push_back(subVec, &value);
        }
        vector_push_back(vec2D, &subVec);
    }

    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** subVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* subVec = *subVecPtr;

        for (size_t j = 0; j < vector_size(subVec); j++) {
            int* valPtr = (int*)vector_at(subVec, j);
            fmt_printf("%d ", *valPtr);
        }
        fmt_printf("\n");
    }

    // Cleanup
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** subVecPtr = (Vector**)vector_at(vec2D, i);
        vector_deallocate(*subVecPtr);
    }
    
    vector_deallocate(vec2D);
    return 0;
}
```
**Result**
```
0 1 2 
3 4 5 
6 7 8 
```

---

## Example 25: Vector of Structs with Dynamic String Fields

This example demonstrates how to use the Vector library to store a collection of structs, where each struct contains dynamically allocated string fields (using the String library).
```c
#include "vector/vector.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

typedef struct {
    String* name;
    String* description;
} Item;

int main() {
    Vector* items = vector_create(sizeof(Item));
    const char* names[] = {"Item1", "Item2", "Item3"};
    const char* descriptions[] = {"Description1", "Description2", "Description3"};

    for (int i = 0; i < 3; i++) {
        Item item;
        item.name = string_create(names[i]);
        item.description = string_create(descriptions[i]);

        vector_push_back(items, &item);
    }

    for (size_t i = 0; i < vector_size(items); i++) {
        Item* itemPtr = (Item*)vector_at(items, i);
        fmt_printf("Name: %s, Description: %s\n", 
               string_c_str(itemPtr->name), 
               string_c_str(itemPtr->description));
    }

    for (size_t i = 0; i < vector_size(items); i++) {
        Item* itemPtr = (Item*)vector_at(items, i);
        string_deallocate(itemPtr->name);
        string_deallocate(itemPtr->description);
    }

    vector_deallocate(items);
    return 0;
}
```
**Result**
```
Name: Item1, Description: Description1
Name: Item2, Description: Description2
Name: Item3, Description: Description3
```

---

## Example 26: Vector of Vectors (2D Vector) of Custom Structs

```c
#include <stdlib.h>
#include "vector/vector.h"
#include "string/std_string.h"
#include "fmt/fmt.h"


typedef struct {
    String* name;
    int age;
} Person;

Person* create_person(int group, int index) {
    Person* p = (Person*)malloc(sizeof(Person));
    p->name = string_create("");
    string_format(p->name, "Person_%d_%d", group, index);
    p->age = group * 10 + index;

    return p;
}

void deallocate_person(Person* p) {
    if (p) {
        string_deallocate(p->name);
        free(p);
    }
}

int main() {
    Vector* vec2D = vector_create(sizeof(Vector*));

    // Populate the 2D vector with vectors of Persons
    for (int i = 0; i < 2; i++) {
        Vector* peopleVec = vector_create(sizeof(Person*));

        for (int j = 0; j < 3; j++) {
            Person* person = create_person(i, j);
            vector_push_back(peopleVec, &person);
        }
        vector_push_back(vec2D, &peopleVec);
    }

    // Iterate and print each person's details
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** peopleVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* peopleVec = *peopleVecPtr;

        for (size_t j = 0; j < vector_size(peopleVec); j++) {
            Person** personPtr = (Person**)vector_at(peopleVec, j);
            Person* person = *personPtr;

            fmt_printf("Name: %s, Age: %d\n", string_c_str(person->name), person->age);
        }
    }

    // Cleanup
    for (size_t i = 0; i < vector_size(vec2D); i++) {
        Vector** peopleVecPtr = (Vector**)vector_at(vec2D, i);
        Vector* peopleVec = *peopleVecPtr;

        for (size_t j = 0; j < vector_size(peopleVec); j++) {
            Person** personPtr = (Person**)vector_at(peopleVec, j);
            deallocate_person(*personPtr);
        }
        vector_deallocate(peopleVec);
    }

    vector_deallocate(vec2D);
    return 0;
}
```
**Result**
```
Name: Person_0_0, Age: 0
Name: Person_0_1, Age: 1
Name: Person_0_2, Age: 2
Name: Person_1_0, Age: 10
Name: Person_1_1, Age: 11
Name: Person_1_2, Age: 12
```

---

## Example 27 : sort and search a Vector with the `Algorithm` library

This example stores integers in a vector, then sorts the vector's underlying storage in place with `algorithm_sort` and locates a value with `algorithm_find`. `vector_data` exposes the contiguous buffer the algorithm functions operate on.

```c
#include "vector/vector.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

static int compare_ints(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Vector* vec = vector_create(sizeof(int));
    int values[] = {42, 7, 19, 3, 25, 11};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Sort the vector's underlying storage in place with algorithm_sort
    algorithm_sort(vector_data(vec), vector_size(vec), sizeof(int), compare_ints);

    fmt_printf("Sorted: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%d ", *(int*)vector_at(vec, i));
    }
    fmt_printf("\n");

    // Search the sorted data with algorithm_find
    int key = 19;
    int* found = (int*)algorithm_find(vector_data(vec), vector_size(vec), sizeof(int), &key, compare_ints);
    if (found) {
        fmt_printf("Found %d at index %ld\n", *found, (long)(found - (int*)vector_data(vec)));
    }

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Sorted: 3 7 11 19 25 42 
Found 19 at index 3
```

---

## Example 28 : build a delimited `String` from a Vector of integers

This example converts each integer in a vector to a `String` with `string_from_int` and appends it to a running `String` to produce a comma-separated list. Each temporary piece is deallocated as soon as it is appended.

```c
#include "vector/vector.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main(void) {
    Vector* vec = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Build a single comma-separated String from the vector's integers
    String* csv = string_create("");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int* item = (int*)vector_at(vec, i);
        String* piece = string_from_int(*item);

        if (i > 0) {
            string_append(csv, ", ");
        }
        string_append(csv, string_c_str(piece));
        string_deallocate(piece);
    }

    fmt_printf("Joined: %s\n", string_c_str(csv));
    fmt_printf("Length: %zu\n", string_length(csv));

    string_deallocate(csv);
    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Joined: 10, 20, 30, 40
Length: 14
```

---

## Example 29 : copy a fixed `Array` into a Vector and reduce it

This example uses the compile-time fixed-size `Array` library as a data source: every element is copied into a dynamic `Vector`, and `array_reduce` collapses the fixed array down to a single sum.

```c
#include "vector/vector.h"
#include "array/array.h"
#include "fmt/fmt.h"

array_create(int, 5, ArrayInt5);

static void sum_reduce(const void* acc, const void* elem, void* out) {
    *(int*)out = *(const int*)acc + *(const int*)elem;
}

int main(void) {
    // A compile-time fixed array as the source of data
    ArrayInt5 fixed = { .data = {5, 10, 15, 20, 25} };

    // Copy every element of the fixed array into a dynamic vector
    Vector* vec = vector_create(sizeof(int));
    for (size_t i = 0; i < array_size(fixed); ++i) {
        int v = array_at(fixed, i);
        vector_push_back(vec, &v);
    }

    fmt_printf("Vector contents: ");
    for (size_t i = 0; i < vector_size(vec); ++i) {
        fmt_printf("%d ", *(int*)vector_at(vec, i));
    }
    fmt_printf("\n");

    // Reduce the fixed array to a single sum using the array API
    int total = 0;
    array_reduce(fixed, &total, sum_reduce);

    fmt_printf("Sum from array_reduce: %d\n", total);
    fmt_printf("Vector size: %zu\n", vector_size(vec));

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Vector contents: 5 10 15 20 25 
Sum from array_reduce: 75
Vector size: 5
```

---

## Example 30 : serialize a Vector of numbers as a `Json` array

This example walks a vector of `double` values, wraps each in a `JSON_NUMBER` element, appends them to a `JSON_ARRAY`, and serializes the result. The serialized buffer returned by `json_serialize` is `free`d and the whole element tree is released with `json_deallocate`.

```c
#include "vector/vector.h"
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    Vector* vec = vector_create(sizeof(double));
    double values[] = {1.5, 2.0, 3.25, 4.0};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        vector_push_back(vec, &values[i]);
    }

    // Build a JSON array element from the vector's numbers
    JsonElement* arr = json_create(JSON_ARRAY);
    for (size_t i = 0; i < vector_size(vec); ++i) {
        double* item = (double*)vector_at(vec, i);
        JsonElement* num = json_create(JSON_NUMBER);
        num->value.number_val = *item;
        json_add_to_array(arr, num);
    }

    fmt_printf("Array size: %zu\n", json_array_size(arr));

    char* serialized = json_serialize(arr);
    fmt_printf("JSON: %s\n", serialized);
    free(serialized);

    json_deallocate(arr);
    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Array size: 4
JSON: [1.5, 2, 3.25, 4]
```

---

## Example 31 : compute descriptive statistics over a Vector

Because `vector_data` returns a contiguous block, a vector of `double` can be handed straight to the `Statistics` library. This example computes the mean, median, and population standard deviation of the stored samples.

```c
#include "vector/vector.h"
#include "statistics/statistics.h"
#include "fmt/fmt.h"

int main(void) {
    Vector* vec = vector_create(sizeof(double));
    double samples[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};

    for (size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        vector_push_back(vec, &samples[i]);
    }

    // vector_data gives a contiguous double[] the statistics API can read directly
    const double* data = (const double*)vector_data(vec);
    size_t n = vector_size(vec);

    fmt_printf("Count:  %zu\n", n);
    fmt_printf("Mean:   %.4f\n", statistics_mean(data, n));
    fmt_printf("Median: %.4f\n", statistics_median(data, n));
    fmt_printf("Pstdev: %.4f\n", statistics_pstdev(data, n, false, 0.0));

    vector_deallocate(vec);
    return 0;
}
```
**Result**
```
Count:  8
Mean:   5.0000
Median: 4.5000
Pstdev: 2.0000
```

---

## Example 32 : bulk operations — `append`, `insert_range`, `insert_fill`, `erase_if`, `remove_value`, `find`

Each of these does its work with one growth step and one pass instead of a loop of single-element calls.

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

static bool greater_than(const void* item, void* ctx) {
    return *(const int*)item > *(const int*)ctx;
}

static void print_ints(const char* label, const Vector* v) {
    fmt_printf("%s:", label);
    for (size_t i = 0; i < vector_size(v); ++i) {
        fmt_printf(" %d", *(const int*)vector_at(v, i));
    }
    fmt_printf("\n");
}

int main(void) {
    int first[] = {1, 2, 3, 4};
    Vector* v = vector_from_array(sizeof(int), first, 4);
    print_ints("from_array", v);

    int more[] = {10, 20, 30};
    vector_append(v, more, 3);
    print_ints("append", v);

    int mid[] = {7, 8};
    vector_insert_range(v, 2, mid, 2);
    print_ints("insert_range at 2", v);

    int zero = 0;
    vector_insert_fill(v, 0, 3, &zero);
    print_ints("insert_fill 3 zeros", v);

    vector_append(v, vector_data(v), vector_size(v));   /* the source may be the vector itself */
    fmt_printf("appended to itself: %zu elements\n", vector_size(v));

    int limit = 8;
    size_t removed = vector_erase_if(v, greater_than, &limit);
    fmt_printf("erase_if > 8 removed %zu\n", removed);
    print_ints("now", v);

    removed = vector_remove_value(v, &zero);
    fmt_printf("remove_value 0 removed %zu\n", removed);
    print_ints("now", v);

    int key = 7;
    fmt_printf("find 7 -> index %zu, contains 99: %s\n", vector_find(v, &key),
               vector_contains(v, &(int){99}) ? "yes" : "no");

    vector_deallocate(v);
    return 0;
}
```
**Result**
```
from_array: 1 2 3 4
append: 1 2 3 4 10 20 30
insert_range at 2: 1 2 7 8 3 4 10 20 30
insert_fill 3 zeros: 0 0 0 1 2 7 8 3 4 10 20 30
appended to itself: 24 elements
erase_if > 8 removed 6
now: 0 0 0 1 2 7 8 3 4 0 0 0 1 2 7 8 3 4
remove_value 0 removed 6
now: 1 2 7 8 3 4 1 2 7 8 3 4
find 7 -> index 2, contains 99: no
```

---

## Example 33 : typed macros — `VECTOR_PUSH_BACK`, `VECTOR_FOREACH`, `VECTOR_ERASE_IF`, `VECTOR_APPEND_UNINIT`

With the element type known, the macros compile to the same code as `std::vector<int>`.

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

int main(void) {
    Vector* v = vector_create(sizeof(int));
    for (int i = 1; i <= 10; i++) {
        VECTOR_PUSH_BACK(v, int, i * i);
    }

    long sum = 0;
    VECTOR_FOREACH(v, int, p) {
        sum += *p;
    }
    fmt_printf("10 squares, sum %ld, last %d\n", sum, VECTOR_BACK(v, int));

    VECTOR_ERASE_IF(v, int, x, x % 2 != 0);       /* keep the even squares */
    fmt_printf("even squares:");
    VECTOR_FOREACH(v, int, p) {
        fmt_printf(" %d", *p);
    }
    fmt_printf("\n");

    /* five new elements, written in place */
    int* tail = VECTOR_APPEND_UNINIT(v, int, 5);
    if (tail) {
        for (int i = 0; i < 5; i++) {
            tail[i] = -i;
        }
    }
    VECTOR_AT(v, int, 0) = 1000;                  /* an lvalue, like operator[] */
    int popped = VECTOR_POP_BACK(v, int);
    fmt_printf("popped %d, size %zu, first %d\n", popped, vector_size(v), VECTOR_FRONT(v, int));

    vector_deallocate(v);
    return 0;
}
```
**Result**
```
10 squares, sum 385, last 100
even squares: 4 16 36 64 100
popped -4, size 9, first 1000
```

---

## Example 34 : sort structs with `VECTOR_DEFINE_SORT`, then `vector_binary_search` and `vector_unique`

`VECTOR_DEFINE_SORT` generates a pdqsort for the element type; `vector_sort` takes a runtime comparator instead.

```c
#include "vector/vector.h"
#include "fmt/fmt.h"

typedef struct {
    const char* name;
    int score;
} Player;

#define BY_SCORE(a, b) ((a).score < (b).score)
VECTOR_DEFINE_SORT(sort_players, Player, BY_SCORE)

#define INT_LESS(a, b) ((a) < (b))
VECTOR_DEFINE_SORT(sort_ints, int, INT_LESS)

static int compare_ints(const void* a, const void* b) {
    int x = *(const int*)a, y = *(const int*)b;
    return (x > y) - (x < y);
}

int main(void) {
    Player players[] = {{"Sara", 72}, {"Omid", 95}, {"Leila", 64}, {"Reza", 88}, {"Nika", 79}};
    Vector* team = vector_from_array(sizeof(Player), players, 5);
    sort_players(team);
    vector_reverse(team);                         /* highest score first */
    VECTOR_FOREACH(team, Player, p) {
        fmt_printf("%-6s %d\n", p->name, p->score);
    }

    int raw[] = {5, 3, 9, 3, 1, 5, 9, 9, 2};
    Vector* nums = vector_from_array(sizeof(int), raw, 9);
    sort_ints(nums);
    int key = 9;
    fmt_printf("first 9 at index %zu\n", vector_binary_search(nums, &key, compare_ints));
    size_t dups = vector_unique(nums);
    fmt_printf("removed %zu duplicates:", dups);
    VECTOR_FOREACH(nums, int, p) {
        fmt_printf(" %d", *p);
    }
    fmt_printf("\n");

    vector_deallocate(team);
    vector_deallocate(nums);
    return 0;
}
```
**Result**
```
Omid   95
Reza   88
Nika   79
Sara   72
Leila  64
first 9 at index 6
removed 4 duplicates: 1 2 3 5 9
```

---

## Example 35 : ownership — `vector_release`, `vector_from_buffer`, `vector_copy`, `vector_slice`

A vector can hand its buffer to the caller and adopt a `malloc`'d one, without copying the elements.

```c
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    double* samples = malloc(4 * sizeof(double));
    if (!samples) {
        return 1;
    }
    for (int i = 0; i < 4; i++) {
        samples[i] = i * 1.5;
    }

    /* the vector takes the buffer over: no copy, and it frees it later */
    Vector* v = vector_from_buffer(samples, sizeof(double), 4, 4);
    double extra = 99.0;
    vector_push_back(v, &extra);                  /* grows the adopted buffer */

    Vector* copy = vector_copy(v);
    Vector* middle = vector_slice(v, 1, 3);
    fmt_printf("copy has %zu, slice [1, 4) = %.1f %.1f %.1f\n", vector_size(copy),
               *(double*)vector_at(middle, 0), *(double*)vector_at(middle, 1), *(double*)vector_at(middle, 2));

    /* hand the buffer back as a plain array */
    size_t count = 0;
    double* arr = vector_release(v, &count);
    fmt_printf("released %zu doubles, last %.1f, vector now has %zu\n", count, arr[count - 1], vector_size(v));
    free(arr);

    vector_deallocate(v);
    vector_deallocate(copy);
    vector_deallocate(middle);
    return 0;
}
```
**Result**
```
copy has 5, slice [1, 4) = 1.5 3.0 4.5
released 5 doubles, last 99.0, vector now has 0
```

---

## Example 36 : `vector_swap_remove`, `vector_pop_back_value`, `vector_emplace_back_slot`, `vector_assign_fill`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <string.h>

typedef struct {
    int id;
    char tag[16];
} Entity;

int main(void) {
    Vector* world = vector_create(sizeof(Entity));
    for (int i = 0; i < 5; i++) {
        Entity* e = vector_emplace_back_slot(world);   /* zero-filled, built in place */
        e->id = i;
        strcpy(e->tag, i % 2 ? "odd" : "even");
    }

    vector_swap_remove(world, 1);                 /* O(1): the last entity takes its place */
    VECTOR_FOREACH(world, Entity, e) {
        fmt_printf("[%d %s] ", e->id, e->tag);
    }
    fmt_printf("\n");

    Entity last;
    while (vector_pop_back_value(world, &last)) {
        fmt_printf("popped %d\n", last.id);
    }

    Vector* counters = vector_create(sizeof(int));
    int start = 3;
    vector_assign_fill(counters, 6, &start);
    int marker = -1;
    vector_resize_with(counters, 8, &marker);
    VECTOR_FOREACH(counters, int, c) {
        fmt_printf("%d ", *c);
    }
    fmt_printf("\n");

    vector_deallocate(world);
    vector_deallocate(counters);
    return 0;
}
```
**Result**
```
[0 even] [4 even] [2 even] [3 odd]
popped 3
popped 2
popped 4
popped 0
3 3 3 3 3 3 -1 -1
```

---

## License

This project is open-source and available under [ISC License].