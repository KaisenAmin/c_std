# Variant Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2025
**License:** ISC License

## Overview

`Variant` is a small, memory-safe, C++-flavoured `std::variant` for C.
A single `Variant` can hold an `int`, `long`, `float`, `double`,
`char`, `bool`, raw pointer, owning C string, or any user-defined type
registered via a `VariantTypeInfo` descriptor. The active alternative
is tracked with a tag, and every accessor is type-checked at runtime.

The library was rewritten from scratch during the audit. Design rules:

- **Memory-safe**: the `Variant` owns its payload (string buffer or
  custom-type allocation). `variant_destroy` releases everything;
  calling it twice is safe, and so is calling it on `NULL`.
- **No global state**: there is no errno-style last-error global.
  Every function returns enough information through its return value
  to know whether the operation succeeded.
- **No "unsafe" getters**: every accessor checks the type and returns
  `false` on mismatch instead of crashing.
- **Complete**: every declared function has an implementation. The
  previous version declared `variant_serialize`, `variant_assign_*`,
  and others that were never implemented — those have been removed in
  favour of the simpler `variant_to_string`, `variant_clone`, and
  `variant_set_*` families that exist for real.
- **Fast & small**: ~32 bytes for a primitive variant, no allocation.
  Strings and custom types are heap-allocated only because they have
  to be.

All public functions have Doxygen comments above their implementations
in [`variant.c`](variant.c).

## Features

- Tagged union over int / long / float / double / char / bool / pointer / string / custom
- Deep copy (`variant_clone`), in-place re-assignment (`variant_set_*`),
  move-assign (`variant_move`), swap (`variant_swap`), detach
  (`variant_take`)
- Total ordering: `variant_compare`, `variant_equal`, FNV-1a `variant_hash`
- Single-callback visitor pattern (`variant_visit`,
  `variant_visit_all`)
- Predictable `variant_to_string` for every type
- `VariantArray` — a growable, owning vector of variants
- Built-in `VariantTypeInfo` for each primitive so they plug into the
  custom-type machinery
- NULL-safe everywhere, double-destroy safe, no leaks on transition

---

## Function Definitions

### `Variant variant_none(void)`
**Purpose**: Creates an empty, NONE-typed variant that holds no value.
**Parameters**: None.
**Return Value**: A `Variant` with type `VARIANT_TYPE_NONE`.
**Usage Case**: Use as a default-constructed Variant before assigning a value, or to reset a Variant without knowing its current type.

---

### `Variant variant_from_int(int v)`
**Purpose**: Creates a Variant holding the given `int` value.
**Parameters**:
  - `v`: The integer value to store.
**Return Value**: A `Variant` of type `VARIANT_TYPE_INT`.
**Usage Case**: Use to wrap a plain `int` in a Variant for storage in a heterogeneous container or for passing to a type-agnostic interface.

---

### `Variant variant_from_long(long v)`
**Purpose**: Creates a Variant holding the given `long` value.
**Parameters**:
  - `v`: The long integer value to store.
**Return Value**: A `Variant` of type `VARIANT_TYPE_LONG`.
**Usage Case**: Use when the value may exceed `INT_MAX` but does not require heap allocation.

---

### `Variant variant_from_float(float v)`
**Purpose**: Creates a Variant holding the given `float` value.
**Parameters**:
  - `v`: The float value to store.
**Return Value**: A `Variant` of type `VARIANT_TYPE_FLOAT`.
**Usage Case**: Use when single-precision floating-point is sufficient and no heap allocation is desired.

---

### `Variant variant_from_double(double v)`
**Purpose**: Creates a Variant holding the given `double` value.
**Parameters**:
  - `v`: The double value to store.
**Return Value**: A `Variant` of type `VARIANT_TYPE_DOUBLE`.
**Usage Case**: Standard choice for floating-point variants in scientific or financial computations.

---

### `Variant variant_from_char(char v)`
**Purpose**: Creates a Variant holding the given `char` value.
**Parameters**:
  - `v`: The character value to store.
**Return Value**: A `Variant` of type `VARIANT_TYPE_CHAR`.
**Usage Case**: Use to represent a single character in a mixed-type collection.

---

### `Variant variant_from_bool(bool v)`
**Purpose**: Creates a Variant holding the given `bool` value.
**Parameters**:
  - `v`: The boolean value to store.
**Return Value**: A `Variant` of type `VARIANT_TYPE_BOOL`.
**Usage Case**: Use to store flag or switch values alongside other types in a Variant container.

---

### `Variant variant_from_pointer(void* v)`
**Purpose**: Creates a non-owning Variant holding a raw pointer. The Variant does NOT free the pointed-to memory when destroyed.
**Parameters**:
  - `v`: The pointer to store (non-owning).
**Return Value**: A `Variant` of type `VARIANT_TYPE_POINTER`.
**Usage Case**: Use to pass an opaque handle through a Variant pipeline without transferring ownership.

---

### `Variant variant_from_string(const char* v)`
**Purpose**: Creates an owning Variant that deep-copies the given C string.
**Parameters**:
  - `v`: The NUL-terminated string to copy. NULL is treated as an empty string.
**Return Value**: A `Variant` of type `VARIANT_TYPE_STRING` that owns its buffer.
**Usage Case**: Use when a string must outlive the original buffer, or when the Variant is stored in a container that may destroy the original.

---

### `Variant variant_from_custom(const void* src, const VariantTypeInfo* info)`
**Purpose**: Creates an owning Variant by deep-copying `info->size` bytes from `src` using the copy callback in `info`.
**Parameters**:
  - `src`: Pointer to the source data to copy.
  - `info`: Descriptor providing size, copy, destroy, compare, and print callbacks.
**Return Value**: A `Variant` of type `VARIANT_TYPE_CUSTOM`.
**Usage Case**: Use to store arbitrary user-defined types (structs, objects) in a Variant with proper lifetime management.

---

### `Variant variant_clone(const Variant* src)`
**Purpose**: Returns a deep copy of the given Variant, including its payload.
**Parameters**:
  - `src`: The Variant to clone. NULL returns a NONE Variant.
**Return Value**: An independent `Variant` whose lifetime is separate from `src`.
**Usage Case**: Use when you need two independent copies of the same Variant, e.g. before modifying one while keeping the original.

---

### `void variant_destroy(Variant* v)`
**Purpose**: Releases the Variant's owned payload (string buffer or custom data) and resets it to NONE.
**Parameters**:
  - `v`: Pointer to the Variant to destroy. NULL is a safe no-op.
**Return Value**: None.
**Usage Case**: Call when a Variant goes out of scope or is no longer needed to prevent memory leaks. Safe to call multiple times.

---

### `void variant_move(Variant* dst, Variant* src)`
**Purpose**: Move-assigns `src` into `dst`, destroying `dst`'s current value and leaving `src` as NONE.
**Parameters**:
  - `dst`: Destination Variant (its current value is destroyed first).
  - `src`: Source Variant (becomes NONE after the move).
**Return Value**: None.
**Usage Case**: Use for efficient in-place reassignment without allocation — the C analogue of C++ move-assignment.

---

### `void variant_swap(Variant* a, Variant* b)`
**Purpose**: Swaps the contents of two Variants in constant time without allocation.
**Parameters**:
  - `a`: First Variant.
  - `b`: Second Variant.
**Return Value**: None.
**Usage Case**: Use to exchange two Variants in-place, e.g. during sorting or when swapping elements in a container.

---

### `void* variant_take(Variant* v)`
**Purpose**: Detaches the owned payload (string buffer or custom data pointer) from the Variant without freeing it, leaving the Variant as NONE.
**Parameters**:
  - `v`: The Variant whose payload is to be detached.
**Return Value**: The detached pointer (caller takes ownership and must `free` it), or NULL for non-owning Variants.
**Usage Case**: Use when you need to move ownership of the internal buffer out of a Variant, e.g. to pass a string to a legacy API that expects `malloc`-allocated memory.

---

### `bool variant_set_none(Variant* v)`
**Purpose**: Destroys the Variant's current value and resets it to NONE in place.
**Parameters**:
  - `v`: Pointer to the Variant to reset. NULL → returns false.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use to clear a Variant slot without destroying the Variant struct itself.

---

### `bool variant_set_int(Variant* v, int x)`
**Purpose**: Destroys the Variant's current value and assigns a new `int` in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New int value.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use to re-use an existing Variant slot without creating a new one.

---

### `bool variant_set_long(Variant* v, long x)`
**Purpose**: Destroys the Variant's current value and assigns a new `long` in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New long value.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Same as `variant_set_int` but for long values.

---

### `bool variant_set_float(Variant* v, float x)`
**Purpose**: Destroys the Variant's current value and assigns a new `float` in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New float value.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use for in-place float reassignment.

---

### `bool variant_set_double(Variant* v, double x)`
**Purpose**: Destroys the Variant's current value and assigns a new `double` in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New double value.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use for in-place double reassignment.

---

### `bool variant_set_char(Variant* v, char x)`
**Purpose**: Destroys the Variant's current value and assigns a new `char` in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New char value.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use for in-place char reassignment.

---

### `bool variant_set_bool(Variant* v, bool x)`
**Purpose**: Destroys the Variant's current value and assigns a new `bool` in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New bool value.
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use for in-place bool reassignment.

---

### `bool variant_set_pointer(Variant* v, void* x)`
**Purpose**: Destroys the Variant's current value and stores a new non-owning pointer in place.
**Parameters**:
  - `v`: Target Variant.
  - `x`: New pointer value (non-owning).
**Return Value**: `true` on success, `false` if `v` is NULL.
**Usage Case**: Use to update the pointer in an existing Variant slot.

---

### `bool variant_set_string(Variant* v, const char* x)`
**Purpose**: Destroys the Variant's current value and deep-copies the given string in place. NULL acts like `variant_set_none`.
**Parameters**:
  - `v`: Target Variant.
  - `x`: NUL-terminated string to copy (may be NULL).
**Return Value**: `true` on success, `false` if `v` is NULL or allocation fails.
**Usage Case**: Use to replace a Variant's payload with a new string without creating a new Variant.

---

### `bool variant_set_custom(Variant* v, const void* src, const VariantTypeInfo* info)`
**Purpose**: Destroys the Variant's current value and deep-copies a custom type in place.
**Parameters**:
  - `v`: Target Variant.
  - `src`: Source data to copy.
  - `info`: Type descriptor.
**Return Value**: `true` on success, `false` on NULL input or allocation failure.
**Usage Case**: Use to replace a Variant's payload with a custom object.

---

### `bool variant_get_int(const Variant* v, int* out)`
**Purpose**: Type-checked getter for `int`. Returns false on type mismatch.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the int value. Left untouched on mismatch.
**Return Value**: `true` if `v` holds an `int` and the value was written to `*out`, `false` otherwise.
**Usage Case**: Use to safely extract an int value, then branch on the return value to handle the mismatch.

---

### `bool variant_get_long(const Variant* v, long* out)`
**Purpose**: Type-checked getter for `long`.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the long value.
**Return Value**: `true` if the type matches and the value was written, `false` otherwise.
**Usage Case**: Use to safely extract a long value with type checking.

---

### `bool variant_get_float(const Variant* v, float* out)`
**Purpose**: Type-checked getter for `float`.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the float value.
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to safely extract a float value.

---

### `bool variant_get_double(const Variant* v, double* out)`
**Purpose**: Type-checked getter for `double`.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the double value.
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to safely extract a double value with type verification.

---

### `bool variant_get_char(const Variant* v, char* out)`
**Purpose**: Type-checked getter for `char`.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the char value.
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to safely extract a char value.

---

### `bool variant_get_bool(const Variant* v, bool* out)`
**Purpose**: Type-checked getter for `bool`.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the bool value.
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to safely extract a bool value.

---

### `bool variant_get_pointer(const Variant* v, void** out)`
**Purpose**: Type-checked getter for a raw pointer.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the pointer value.
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to safely retrieve a non-owning pointer stored in a Variant.

---

### `bool variant_get_string(const Variant* v, const char** out)`
**Purpose**: Type-checked getter for a string. Returns a borrowed pointer valid until the Variant is destroyed or reassigned.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Destination for the string pointer (borrowed — do not free).
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to read a string from a Variant without copying it.

---

### `bool variant_get_custom(const Variant* v, void** out, const VariantTypeInfo** out_info)`
**Purpose**: Type-checked getter for a custom type. Returns borrowed pointers to the data and type descriptor.
**Parameters**:
  - `v`: Source Variant.
  - `out`: Receives a pointer to the custom data (borrowed).
  - `out_info`: Receives a pointer to the type descriptor (borrowed).
**Return Value**: `true` on match, `false` on mismatch.
**Usage Case**: Use to inspect or use a custom-type payload without copying it.

---

### `VariantType variant_type(const Variant* v)`
**Purpose**: Returns the active type tag of the Variant.
**Parameters**:
  - `v`: The Variant to inspect. NULL → `VARIANT_TYPE_NONE`.
**Return Value**: The `VariantType` enum value for the current payload.
**Usage Case**: Use in a switch statement to dispatch based on the active type.

---

### `const char* variant_type_name(const Variant* v)`
**Purpose**: Returns a human-readable string name for the active type (e.g. `"int"`, `"string"`, `"none"`).
**Parameters**:
  - `v`: The Variant to inspect.
**Return Value**: A static string (do not free). For custom types, returns `info->type_name` or `"custom"`.
**Usage Case**: Use for logging, error messages, and debug output.

---

### `size_t variant_type_size(const Variant* v)`
**Purpose**: Returns the storage size of the active payload type.
**Parameters**:
  - `v`: The Variant to inspect.
**Return Value**: `sizeof` of the active payload (e.g. 4 for int, pointer size for string/custom). 0 for NONE.
**Usage Case**: Use when you need to know the memory footprint of a Variant's payload.

---

### `size_t variant_hash(const Variant* v)`
**Purpose**: Computes an FNV-1a hash over the live bytes of the Variant, mixed with the type tag.
**Parameters**:
  - `v`: The Variant to hash. NULL → 0.
**Return Value**: A `size_t` hash value. Two equal Variants produce the same hash.
**Usage Case**: Use as a hash function for maps or sets keyed by Variant values.

---

### `bool variant_is(const Variant* v, VariantType t)`
**Purpose**: Checks if the Variant's active type equals `t`.
**Parameters**:
  - `v`: The Variant to check.
  - `t`: The expected type tag.
**Return Value**: `true` if `variant_type(v) == t`, `false` otherwise.
**Usage Case**: Short-cut for checking the active alternative before a getter call.

---

### `bool variant_is_none(const Variant* v)`
**Purpose**: Checks if the Variant is in the NONE (empty) state.
**Parameters**:
  - `v`: The Variant to check.
**Return Value**: `true` if the type is `VARIANT_TYPE_NONE`.
**Usage Case**: Use to detect unset/null Variants before accessing their payload.

---

### `bool variant_equal(const Variant* a, const Variant* b)`
**Purpose**: Checks if two Variants are equal by type and by value.
**Parameters**:
  - `a`: First Variant.
  - `b`: Second Variant.
**Return Value**: `true` if both have the same type and the same payload value.
**Usage Case**: Use for equality checks in generic containers or assertions.

---

### `int variant_compare(const Variant* a, const Variant* b)`
**Purpose**: Total ordering comparison. Same-tag variants compare by value; different-tag variants compare by tag (enum order).
**Parameters**:
  - `a`: First Variant.
  - `b`: Second Variant.
**Return Value**: Negative if `a < b`, 0 if equal, positive if `a > b`.
**Usage Case**: Use as a comparator for `qsort`, binary search, or ordered containers.

---

### `char* variant_to_string(const Variant* v)`
**Purpose**: Serialises the Variant to a heap-allocated string.
**Parameters**:
  - `v`: The Variant to serialise.
**Return Value**: A newly allocated NUL-terminated string (caller must `free`), or NULL on allocation failure.
**Usage Case**: Use for printing, logging, or serialisation. Call `free()` on the result when done.

---

### `int variant_print(const Variant* v)`
**Purpose**: Prints the Variant's string representation to stdout (no trailing newline).
**Parameters**:
  - `v`: The Variant to print.
**Return Value**: The number of characters written, or negative on error.
**Usage Case**: Use for quick debugging output without managing the string buffer manually.

---

### `int variant_println(const Variant* v)`
**Purpose**: Prints the Variant's string representation to stdout followed by a newline.
**Parameters**:
  - `v`: The Variant to print.
**Return Value**: The number of characters written, or negative on error.
**Usage Case**: Use when you want each Variant on its own line (e.g. printing elements of a collection).

---

### `int variant_visit(const Variant* v, VariantVisitFn fn, void* user_data)`
**Purpose**: Calls `fn(v, user_data)` once with the given Variant.
**Parameters**:
  - `v`: The Variant to visit.
  - `fn`: Visitor callback (`int (*)(const Variant*, void*)`).
  - `user_data`: Opaque pointer forwarded to `fn`.
**Return Value**: The return value of `fn`, or 0 if `v` or `fn` is NULL.
**Usage Case**: Use to apply a single generic operation to a Variant without knowing its type upfront.

---

### `int variant_visit_all(const Variant* arr, size_t count, VariantVisitFn fn, void* user_data)`
**Purpose**: Calls `fn` for each Variant in the array `arr[0..count-1]`, stopping early if `fn` returns non-zero.
**Parameters**:
  - `arr`: Array of Variants.
  - `count`: Number of Variants in the array.
  - `fn`: Visitor callback.
  - `user_data`: Opaque pointer forwarded to every `fn` call.
**Return Value**: The first non-zero return value from `fn`, or 0 if all calls returned 0.
**Usage Case**: Use to apply a generic operation (print, validate, serialize) to every element of a mixed-type array.

---

### `VariantArray* variant_array_new(size_t initial_capacity)`
**Purpose**: Creates a new growable owning array of Variants.
**Parameters**:
  - `initial_capacity`: Initial reserved capacity (0 is valid — the array grows on first push).
**Return Value**: A pointer to the new `VariantArray`, or NULL on allocation failure.
**Usage Case**: Use as a type-safe heterogeneous list for mixed-type values.

---

### `void variant_array_free(VariantArray* a)`
**Purpose**: Destroys every element and frees the array itself.
**Parameters**:
  - `a`: The array to free. NULL is a no-op.
**Return Value**: None.
**Usage Case**: Call when the array is no longer needed to release all resources.

---

### `void variant_array_clear(VariantArray* a)`
**Purpose**: Destroys every element but keeps the backing storage alive.
**Parameters**:
  - `a`: The array to clear. NULL is a no-op.
**Return Value**: None.
**Usage Case**: Use to empty the array without freeing its buffer (ready for reuse).

---

### `bool variant_array_push(VariantArray* a, const Variant* v)`
**Purpose**: Appends a deep copy of `v` to the end of the array.
**Parameters**:
  - `a`: Target array.
  - `v`: Variant to copy and append.
**Return Value**: `true` on success, `false` on NULL input or allocation failure.
**Usage Case**: Use to build a dynamic mixed-type list one element at a time.

---

### `bool variant_array_pop(VariantArray* a)`
**Purpose**: Destroys and removes the last element of the array.
**Parameters**:
  - `a`: Target array.
**Return Value**: `true` on success, `false` if the array is NULL or empty.
**Usage Case**: Use to implement a stack by pairing with `variant_array_push`.

---

### `bool variant_array_set(VariantArray* a, size_t index, const Variant* v)`
**Purpose**: Replaces the element at `index` with a deep copy of `v`.
**Parameters**:
  - `a`: Target array.
  - `index`: Zero-based index. Must be < `variant_array_size(a)`.
  - `v`: Replacement Variant.
**Return Value**: `true` on success, `false` on out-of-range or NULL.
**Usage Case**: Use to update an element in the array without removing and re-inserting it.

---

### `bool variant_array_remove_at(VariantArray* a, size_t index)`
**Purpose**: Destroys the element at `index` and shifts subsequent elements left.
**Parameters**:
  - `a`: Target array.
  - `index`: Zero-based index of the element to remove.
**Return Value**: `true` on success, `false` on out-of-range or NULL.
**Usage Case**: Use to delete a specific element by position.

---

### `bool variant_array_reserve(VariantArray* a, size_t min_capacity)`
**Purpose**: Ensures the array can hold at least `min_capacity` elements without reallocation.
**Parameters**:
  - `a`: Target array.
  - `min_capacity`: Minimum required capacity.
**Return Value**: `true` on success, `false` on NULL or allocation failure.
**Usage Case**: Use before a bulk push to avoid repeated reallocations.

---

### `Variant* variant_array_at(VariantArray* a, size_t index)`
**Purpose**: Returns a pointer to the element at `index`.
**Parameters**:
  - `a`: Source array.
  - `index`: Zero-based index. Must be < `variant_array_size(a)`.
**Return Value**: A borrowed pointer to the Variant (valid until the array is modified). NULL on out-of-range or NULL input.
**Usage Case**: Use to inspect or modify an element in place without copying.

---

### `size_t variant_array_size(const VariantArray* a)`
**Purpose**: Returns the number of elements currently in the array.
**Parameters**:
  - `a`: Source array. NULL → 0.
**Return Value**: The element count.
**Usage Case**: Use to iterate over all elements or check emptiness.

---

### `size_t variant_array_capacity(const VariantArray* a)`
**Purpose**: Returns the number of elements the backing buffer can hold before reallocation.
**Parameters**:
  - `a`: Source array. NULL → 0.
**Return Value**: The current capacity.
**Usage Case**: Use to diagnose or optimise memory usage.

---

### `bool variant_array_empty(const VariantArray* a)`
**Purpose**: Checks whether the array contains no elements.
**Parameters**:
  - `a`: Source array. NULL → true.
**Return Value**: `true` if the array is empty or NULL.
**Usage Case**: Guard before calling `variant_array_pop` or iterating.

---

## Building

The library is plain C17 with no third-party dependencies. The
simplest one-shot build:

```bash
gcc -I. example.c variant/variant.c fmt/fmt.c string/std_string.c encoding/encoding.c -o example.exe
```

(`fmt`, `string`, and `encoding` are only used by examples that call
`fmt_printf`. The variant library itself only needs the standard C
library.)

---

## Example 1 — Construct, read, destroy

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    Variant v = variant_from_int(42);

    int n = 0;
    if (variant_get_int(&v, &n)) {
        fmt_printf("int = %d, type = %s\n", n, variant_type_name(&v));
    }

    variant_destroy(&v);
    return 0;
}
```

**Result**

```
int = 42, type = int
```

---

## Example 2 — Strings are deep-copied and owned by the Variant

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    char buf[] = "this is the caller's buffer";
    Variant v = variant_from_string(buf);

    /* Mutate the caller's buffer — the variant kept its own copy. */
    buf[0] = '!';

    const char* held = NULL;
    variant_get_string(&v, &held);
    
    fmt_printf("caller: %s\n", buf);
    fmt_printf("variant: %s\n", held);

    variant_destroy(&v);
    return 0;
}
```

**Result**

```
caller: !his is the caller's buffer
variant: this is the caller's buffer
```

---

## Example 3 — Safe accessors reject the wrong type

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    Variant v = variant_from_double(3.14);

    int    i = 0;
    double d = 0.0;
    int ok_int = variant_get_int(&v, &i);
    int ok_dbl = variant_get_double(&v, &d);
    fmt_printf("get_int    -> %d\n", ok_int);
    fmt_printf("get_double -> %d (d=%g)\n", ok_dbl, d);

    variant_destroy(&v);
    return 0;
}
```

**Result**

```
get_int    -> 0
get_double -> 1 (d=3.14)
```

---

## Example 4 — `variant_clone` for an independent deep copy

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    Variant a = variant_from_string("shared text");
    Variant b = variant_clone(&a);

    variant_destroy(&a);   /* b survives — it owns its own copy */

    const char* s = NULL;
    variant_get_string(&b, &s);
    fmt_printf("b = %s\n", s);

    variant_destroy(&b);
    return 0;
}
```

**Result**

```
b = shared text
```

---

## Example 5 — `variant_set_*` reuses one slot for different types

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    Variant v = variant_none();

    variant_set_int(&v, 7);
    variant_println(&v);              /* 7 */

    variant_set_string(&v, "hello");  /* free()s nothing — was an int */
    variant_println(&v);              /* "hello" */

    variant_set_double(&v, 2.5);      /* free()s the "hello" string */
    variant_println(&v);              /* 2.5 */

    variant_destroy(&v);
    return 0;
}
```

**Result**

```
7
"hello"
2.5
```

---

## Example 6 — Move and swap (no allocation)

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    Variant a = variant_from_string("payload");
    Variant b = variant_from_int(0);

    variant_move(&b, &a);
    fmt_printf("a is now %s\n", variant_type_name(&a));   /* none */
    fmt_printf("b is now %s\n", variant_type_name(&b));   /* string */

    Variant x = variant_from_int(1);
    Variant y = variant_from_int(2);
    variant_swap(&x, &y);
    int xv = 0, yv = 0;

    variant_get_int(&x, &xv);
    variant_get_int(&y, &yv);
    fmt_printf("after swap: x=%d y=%d\n", xv, yv);

    variant_destroy(&b); 
    variant_destroy(&x); 
    variant_destroy(&y);
    return 0;
}
```

**Result**

```
a is now none
b is now string
after swap: x=2 y=1
```

---

## Example 7 — `variant_take`: hand the buffer over to the caller

```c
#include "variant/variant.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    Variant v = variant_from_string("detach me");

    char* s = (char*)variant_take(&v);

    /* v is now NONE; we own s. */
    fmt_printf("caller now owns: %s\n", s);
    fmt_printf("variant type:    %s\n", variant_type_name(&v));

    free(s);
    return 0;
}
```

**Result**

```
caller now owns: detach me
variant type:    none
```

---

## Example 8 — Sorting variants with `variant_compare`

```c
#include "variant/variant.h"
#include "fmt/fmt.h"
#include <stdlib.h>


static int by_variant(const void* a, const void* b) {
    return variant_compare((const Variant*)a, (const Variant*)b);
}

int main(void) {
    Variant items[] = {
        variant_from_int(30),
        variant_from_int(10),
        variant_from_int(20),
    };
    qsort(items, 3, sizeof(Variant), by_variant);

    for (int i = 0; i < 3; ++i) {
      variant_println(&items[i]);
    }
    for (int i = 0; i < 3; ++i) {
      variant_destroy(&items[i]);
    }

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

## Example 9 — Visitor: one callback, every type

```c
#include "variant/variant.h"
#include "fmt/fmt.h"
#include <stdlib.h>

static int dump(const Variant* v, void* user) {
    (void)user;
    char* s = variant_to_string(v);
    fmt_printf("%-7s : %s\n", variant_type_name(v), s);

    free(s);
    return 0;
}

int main(void) {
    Variant items[] = {
        variant_from_int(42),
        variant_from_string("hello"),
        variant_from_double(3.14),
        variant_from_bool(false),
    };
    variant_visit_all(items, 4, dump, NULL);

    for (int i = 0; i < 4; ++i) {
      variant_destroy(&items[i]);
    }

    return 0;
}
```

**Result**

```
int     : 42
string  : "hello"
double  : 3.14
bool    : false
```

---

## Example 10 — Custom user type (`Point`) with copy/destroy/compare/print

```c
#include "variant/variant.h"
#include "fmt/fmt.h"
#include <stdio.h>


typedef struct { 
  int x, y; 
} Point;

static void Point_copy(void* d, const void* s) { 
  *(Point*)d = *(const Point*)s; 
}

static int  Point_compare(const void* a, const void* b) {
    const Point* pa = a; const Point* pb = b;
    if (pa->x != pb->x) {
      return pa->x - pb->x;
    }

    return pa->y - pb->y;
}

static int Point_print(char* buf, size_t n, const void* p) {
    const Point* pt = p;
    return snprintf(buf, n, "(%d,%d)", pt->x, pt->y);
}

static const VariantTypeInfo POINT_INFO = {
    sizeof(Point), Point_copy, NULL, Point_compare, Point_print, "Point"
};

int main(void) {
    Point  origin = {0, 0};
    Point  far    = {3, 4};

    Variant a = variant_from_custom(&origin, &POINT_INFO);
    Variant b = variant_from_custom(&far,    &POINT_INFO);

    variant_print(&a); 
    fmt_printf(" < ");

    variant_print(&b); 
    fmt_printf(" ? %s\n", variant_compare(&a, &b) < 0 ? "yes" : "no");

    variant_destroy(&a);
    variant_destroy(&b);
    return 0;
}
```

**Result**

```
(0,0) < (3,4) ? yes
```

---

## Example 11 — `VariantArray`: vector of mixed types

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    VariantArray* a = variant_array_new(0);

    Variant tmp;
    tmp = variant_from_int(1);        
    variant_array_push(a, &tmp); 
    variant_destroy(&tmp);

    tmp = variant_from_string("two"); 
    variant_array_push(a, &tmp); 
    variant_destroy(&tmp);

    tmp = variant_from_double(3.0);   
    variant_array_push(a, &tmp); 
    variant_destroy(&tmp);

    for (size_t i = 0; i < variant_array_size(a); ++i) {
        Variant* slot = variant_array_at(a, i);
        fmt_printf("[%zu] ", i);
        variant_println(slot);
    }

    variant_array_free(a);
    return 0;
}
```

**Result**

```
[0] 1
[1] "two"
[2] 3
```

---

## Example 12 — Hashing variants (use as a key)

```c
#include "variant/variant.h"
#include "fmt/fmt.h"

int main(void) {
    Variant a = variant_from_string("abc");
    Variant b = variant_from_string("abc");
    Variant c = variant_from_string("abd");

    fmt_printf("hash(\"abc\") == hash(\"abc\")  -> %s\n", variant_hash(&a) == variant_hash(&b) ? "yes" : "no");
    fmt_printf("hash(\"abc\") == hash(\"abd\")  -> %s\n", variant_hash(&a) == variant_hash(&c) ? "yes" : "no");

    variant_destroy(&a); variant_destroy(&b); variant_destroy(&c);
    return 0;
}
```

**Result**

```
hash("abc") == hash("abc")  -> yes
hash("abc") == hash("abd")  -> no
```

---

## License

This project is open-source and available under [ISC License].
