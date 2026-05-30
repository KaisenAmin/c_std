# UniquePtr Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2025
**License:** ISC License

## Overview

`UniquePtr` is a tiny, memory-safe, C++-shaped `std::unique_ptr` for
C. It owns exactly one pointer plus a deleter; when the UniquePtr
goes out of scope (with `UPTR_AUTO`) or is explicitly destroyed, the
deleter runs exactly once. There is no reference counting, no global
state, no errno, and no "unsafe" getters — the whole API fits on one
screen.

The library was rewritten from scratch during the audit. Design rules:

- **One owner per pointer.** Copying a `UniquePtr` struct does *not*
  duplicate ownership — use `uptr_move` / `uptr_take` to transfer it.
- **No "unsafe" anything.** There is only one getter; it returns the
  raw pointer for you to dereference.
- **No global state.** No `errno`-style last-error, no debug flag, no
  hidden registries.
- **Optional RAII** via `UPTR_AUTO` (uses the GCC/Clang `cleanup`
  attribute). At scope exit the compiler calls `uptr_destroy` for
  you — as close as C gets to a C++ destructor.
- **Tiny.** The header is ~100 lines, the implementation ~180.

All public functions have Doxygen comments above their implementations
in [`uniqueptr.c`](uniqueptr.c).

## Mental model — `std::unique_ptr` ↔ `UniquePtr`

| C++                                | C (this module)                          |
|-----------------------------------|------------------------------------------|
| `std::unique_ptr<T> p(new T(...))`| `UniquePtr p = uptr_new(ptr, fn);`       |
| `~unique_ptr` at scope exit       | `uptr_destroy(&p)` or `UPTR_AUTO`        |
| `p.get()`                         | `uptr_get(&p)`                           |
| `p.release()`                     | `uptr_release(&p)`                       |
| `p.reset(q)`                      | `uptr_reset(&p, q)`                      |
| `p.reset()`                       | `uptr_reset(&p, NULL)`                   |
| `p.swap(q)`                       | `uptr_swap(&p, &q)`                      |
| `std::move(p)` (move-assign)      | `uptr_move(&dst, &p)`                    |
| `auto q = std::move(p)`           | `UniquePtr q = uptr_take(&p);`           |
| `if (p)`                          | `!uptr_is_null(&p)`                      |
| `p.get_deleter()`                 | `uptr_get_deleter(&p)`                   |

## Features

- One owning pointer + one deleter — that's the whole struct
- RAII via `UPTR_AUTO` (GCC/Clang `cleanup` attribute)
- Move semantics (`uptr_move`, `uptr_take`) match `std::move`
- `uptr_reset` runs the deleter on the previous payload before
  adopting the new one (including the "self-reset" edge case)
- `uptr_release` hands ownership back to the caller without running
  the deleter
- Default deleter is `free`, so `uptr_new(malloc(N), NULL)` just works
- NULL-safe everywhere; double-destroy is a no-op
- Works with any deleter: `free`, `fclose`, custom resource releaser

---

## Function Definitions

### `UniquePtr uptr_new(void* ptr, UniquePtrDeleter deleter)`

**Purpose**: Build a UniquePtr owning `ptr`. If `deleter` is `NULL`, `free` is used as the default. Passing `ptr == NULL` is equivalent to `uptr_none()`.

**Parameters**:
- `ptr`: The raw pointer to own.
- `deleter`: The function to call on destruction, or `NULL` to use `free`.

**Return Value**: A new `UniquePtr` that owns `ptr`.

**Usage Case**: Primary constructor — wraps any heap pointer with a matching deleter.

---

### `UniquePtr uptr_none(void)`

**Purpose**: Return an empty UniquePtr with no pointer and no deleter.

**Parameters**: None.

**Return Value**: An empty `UniquePtr` (mirrors a default-constructed `std::unique_ptr`).

**Usage Case**: Initialize a UniquePtr variable before it has a value, or as a sentinel return from factory functions that fail.

---

### `void uptr_destroy(UniquePtr* up)`

**Purpose**: Run the deleter on the owned pointer (if any), then reset the UniquePtr to empty. Idempotent and NULL-safe — calling it twice is safe.

**Parameters**:
- `up`: Pointer to the UniquePtr to destroy.

**Return Value**: None.

**Usage Case**: Explicit cleanup when not using `UPTR_AUTO`, or to eagerly release a resource before scope exit.

---

### `void* uptr_get(const UniquePtr* up)`

**Purpose**: Return the raw owned pointer without transferring ownership.

**Parameters**:
- `up`: Pointer to the UniquePtr.

**Return Value**: The raw pointer, or `NULL` if `up` is empty or `NULL`.

**Usage Case**: Access the underlying resource (read or write) without giving up ownership.

---

### `UniquePtrDeleter uptr_get_deleter(const UniquePtr* up)`

**Purpose**: Return the deleter function currently registered with this UniquePtr.

**Parameters**:
- `up`: Pointer to the UniquePtr.

**Return Value**: The deleter function pointer, or `NULL` if `up` is `NULL`.

**Usage Case**: Inspect or copy the deleter for logging, testing, or constructing a sibling UniquePtr with the same cleanup strategy.

---

### `bool uptr_is_null(const UniquePtr* up)`

**Purpose**: Check whether the UniquePtr holds no pointer.

**Parameters**:
- `up`: Pointer to the UniquePtr.

**Return Value**: `true` if `up` is `NULL` or holds no pointer; `false` otherwise.

**Usage Case**: Guard before dereferencing with `uptr_get`, or to test the result of a factory that returns `uptr_none()` on failure.

---

### `void* uptr_release(UniquePtr* up)`

**Purpose**: Give up ownership without running the deleter. The caller becomes responsible for the returned raw pointer.

**Parameters**:
- `up`: Pointer to the UniquePtr.

**Return Value**: The raw pointer that was owned; the UniquePtr becomes empty after the call.

**Usage Case**: Hand ownership back to a C API that expects a raw pointer it will `free` itself.

---

### `void uptr_reset(UniquePtr* up, void* new_ptr)`

**Purpose**: Run the current deleter on the existing payload (if any), then take ownership of `new_ptr`. Passing `new_ptr == NULL` is equivalent to destroying the current payload and leaving the UniquePtr empty.

**Parameters**:
- `up`: Pointer to the UniquePtr to reset.
- `new_ptr`: The new raw pointer to own, or `NULL`.

**Return Value**: None.

**Usage Case**: Replace the managed resource in place — e.g., refresh a buffer or connection without changing the deleter.

---

### `void uptr_swap(UniquePtr* a, UniquePtr* b)`

**Purpose**: Exchange the pointer and deleter of two UniquePtrs in O(1). NULL-safe; self-swap is a no-op.

**Parameters**:
- `a`: First UniquePtr.
- `b`: Second UniquePtr.

**Return Value**: None.

**Usage Case**: Implement move semantics manually, or swap two resources that need to trade owners.

---

### `void uptr_move(UniquePtr* dst, UniquePtr* src)`

**Purpose**: Move-assign: destroy `dst`'s current payload, then transfer `src`'s payload into `dst`. After the call `src` is empty.

**Parameters**:
- `dst`: Destination UniquePtr (its current payload is destroyed first).
- `src`: Source UniquePtr (becomes empty after the call).

**Return Value**: None.

**Usage Case**: C analogue of `dst = std::move(src)` — transfer ownership between two existing UniquePtr variables.

---

### `UniquePtr uptr_take(UniquePtr* src)`

**Purpose**: Move-construct: return a new UniquePtr holding `src`'s payload; `src` becomes empty.

**Parameters**:
- `src`: Source UniquePtr (becomes empty after the call).

**Return Value**: A new `UniquePtr` that now owns the pointer that `src` previously held.

**Usage Case**: C analogue of `auto q = std::move(p)` — move into a freshly declared variable.

---

### `UPTR_AUTO`

**Purpose**: Storage-class attribute macro that hooks `uptr_destroy` to the enclosing scope's exit via the GCC/Clang `cleanup` attribute. Falls back to a no-op on other compilers.

**Parameters**: N/A — used as a qualifier before `UniquePtr` in a local declaration.

**Return Value**: N/A.

**Usage Case**: Zero-overhead RAII — declare `UPTR_AUTO UniquePtr p = uptr_new(...)` and the compiler calls `uptr_destroy(&p)` automatically when `p` goes out of scope.

---

## Building

Pure C; no third-party dependencies. The simplest one-shot build:

```bash
gcc -I. example.c uniqueptr/uniqueptr.c -o example
```

---

## Example 1 — Hello, owned memory

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    UniquePtr p = uptr_new(malloc(sizeof(int)), free);
    *(int*)uptr_get(&p) = 42;

    fmt_printf("value = %d\n", *(int*)uptr_get(&p));

    uptr_destroy(&p);   /* explicit cleanup */
    return 0;
}
```

**Result**

```
value = 42
```

---

## Example 2 — RAII with `UPTR_AUTO`

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>


int main(void) {
    /* When main returns, the compiler calls uptr_destroy(&p). */
    UPTR_AUTO UniquePtr p = uptr_new(malloc(sizeof(int)), free);
    *(int*)uptr_get(&p) = 7;
    fmt_printf("value = %d\n", *(int*)uptr_get(&p));
    /* no uptr_destroy call needed */
    return 0;
}
```

**Result**

```
value = 7
```

---

## Example 3 — Default deleter is `free`

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    /* Pass NULL for the deleter -> free is used. */
    UPTR_AUTO UniquePtr greeting = uptr_new(strdup("hello"), NULL);
    fmt_printf("%s\n", (const char*)uptr_get(&greeting));
    return 0;
}
```

**Result**

```
hello
```

---

## Example 4 — Custom deleter for a `FILE*`

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdio.h>


static void fclose_deleter(void* p) { 
  fclose((FILE*)p); 
}

int main(void) {
    UPTR_AUTO UniquePtr f = uptr_new(fopen("README.md", "r"), fclose_deleter);
    if (uptr_is_null(&f)) {
        fmt_printf("open failed\n");
        return 1;
    }

    char line[64];
    if (fgets(line, sizeof line, (FILE*)uptr_get(&f))) {
        fmt_printf("first line: %s", line);
    }
    /* fclose runs automatically when f goes out of scope. */
    return 0;
}
```

**Result**
```
first line: # C++ Standard Library and Qt, Python etc Implementation in C
```

---

## Example 5 — `uptr_release`: hand ownership back to the caller

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>


/* Builds an int* and returns it. */
static int* make_seven(void) {
    UPTR_AUTO UniquePtr p = uptr_new(malloc(sizeof(int)), free);
    *(int*)uptr_get(&p) = 7;

    return (int*)uptr_release(&p);   /* p becomes empty; caller owns */
}

int main(void) {
    int* n = make_seven();
    fmt_printf("returned %d\n", *n);
    free(n);
    return 0;
}
```

**Result**

```
returned 7
```

---

## Example 6 — `uptr_reset` swaps the payload safely

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    UPTR_AUTO UniquePtr p = uptr_new(malloc(sizeof(int)), free);
    *(int*)uptr_get(&p) = 1;

    /* Replace the payload; the old int is freed automatically. */
    uptr_reset(&p, malloc(sizeof(int)));
    *(int*)uptr_get(&p) = 2;

    fmt_printf("value = %d\n", *(int*)uptr_get(&p));
    return 0;
}
```

**Result**

```
value = 2
```

---

## Example 7 — `uptr_move`: transfer ownership in place

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    UPTR_AUTO UniquePtr a = uptr_new(malloc(sizeof(int)), free);
    UPTR_AUTO UniquePtr b = uptr_none();
    *(int*)uptr_get(&a) = 99;

    uptr_move(&b, &a);   /* a now empty, b owns the int */

    fmt_printf("a is null? %d\n", uptr_is_null(&a));
    fmt_printf("b holds   = %d\n", *(int*)uptr_get(&b));
    return 0;
}
```

**Result**

```
a is null? 1
b holds   = 99
```

---

## Example 8 — `uptr_take`: move-construct a new owner

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>


int main(void) {
    UniquePtr a = uptr_new(malloc(sizeof(int)), free);
    *(int*)uptr_get(&a) = 11;

    /* C analogue of: auto b = std::move(a); */
    UPTR_AUTO UniquePtr b = uptr_take(&a);

    fmt_printf("a is null? %d\n", uptr_is_null(&a));
    fmt_printf("b holds   = %d\n", *(int*)uptr_get(&b));
    /* No uptr_destroy(&a) needed — a is empty. */
    return 0;
}
```

**Result**

```
a is null? 1
b holds   = 11
```

---

## Example 9 — `uptr_swap`

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    int* a_raw = (int*)malloc(sizeof(int)); *a_raw = 100;
    int* b_raw = (int*)malloc(sizeof(int)); *b_raw = 200;

    UPTR_AUTO UniquePtr a = uptr_new(a_raw, free);
    UPTR_AUTO UniquePtr b = uptr_new(b_raw, free);

    uptr_swap(&a, &b);
    fmt_printf("a holds = %d\n", *(int*)uptr_get(&a));
    fmt_printf("b holds = %d\n", *(int*)uptr_get(&b));

    return 0;
}
```

**Result**

```
a holds = 200
b holds = 100
```

---

## Example 10 — Owning a struct with custom cleanup

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>


typedef struct {
    char* name;
    int   age;
} Person;

/* Custom deleter: free the inner string then the struct. */
static void person_delete(void* p) {
    Person* pp = (Person*)p;
    free(pp->name);
    free(pp);
}

int main(void) {
    Person* raw = (Person*)malloc(sizeof(Person));
    raw->name = strdup("Alice");
    raw->age  = 30;

    UPTR_AUTO UniquePtr u = uptr_new(raw, person_delete);
    Person* p = (Person*)uptr_get(&u);
    fmt_printf("name=%s age=%d\n", p->name, p->age);
    /* person_delete runs automatically; both name and struct freed. */
    return 0;
}
```

**Result**

```
name=Alice age=30
```

---

## Example 11 — Conditional ownership

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>


/* Returns a UniquePtr that may be empty if construction fails. */
static UniquePtr try_make_int(int v) {
    int* p = (int*)malloc(sizeof(int));
    if (!p) {
      return uptr_none();
    }

    *p = v;
    return uptr_new(p, free);
}

int main(void) {
    UPTR_AUTO UniquePtr p = try_make_int(123);

    if (uptr_is_null(&p)) {
        fmt_printf("allocation failed\n");
        return 1;
    }
    fmt_printf("got %d\n", *(int*)uptr_get(&p));

    return 0;
}
```

**Result**

```
got 123
```

---

## Example 12 — RAII saves you from `goto cleanup` chains

The classic C "many ways to fail, all need cleanup" pattern goes away.

```c
#include "uniqueptr/uniqueptr.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <stdio.h>


static void fclose_deleter(void* p) { 
  fclose((FILE*)p); 
}

int main(void) {
    UPTR_AUTO UniquePtr buf = uptr_new(malloc(1024), free);
    UPTR_AUTO UniquePtr in  = uptr_new(fopen("nonexistent_file_xyz", "r"), fclose_deleter);
    UPTR_AUTO UniquePtr out = uptr_new(fopen("out.txt", "w"), fclose_deleter);

    if (uptr_is_null(&buf) || uptr_is_null(&in) || uptr_is_null(&out)) {
        fmt_printf("setup failed (no goto-cleanup needed!)\n");
        return 1;   /* UPTR_AUTO frees buf / fcloses in / fcloses out */
    }

    /* …normal processing… */
    return 0;
}
```

**Result (when the input file is missing)**

```
setup failed (no goto-cleanup needed!)
```

---

## Example 13 — Owning a `String` with a matching deleter

When a UniquePtr owns another c_std object, give it a deleter that calls
that object's deallocator — not plain `free`.

```c
#include "uniqueptr/uniqueptr.h"
#include "string/std_string.h"
#include "fmt/fmt.h"


/* Deleter that frees a c_std String* (not plain free). */
static void string_deleter(void* p) {
    string_deallocate((String*)p);
}

int main(void) {
    /* The unique_ptr owns a heap String; string_deallocate runs at scope exit. */
    UPTR_AUTO UniquePtr s = uptr_new(string_create("unique"), string_deleter);

    String* str = (String*)uptr_get(&s);
    string_append(str, "_ptr");

    fmt_printf("text   = %s\n", string_c_str(str));
    fmt_printf("length = %zu\n", string_length(str));
    return 0;
}
```

**Result**

```
text   = unique_ptr
length = 10
```

---

## Example 14 — Owning a `Vector` (deleter calls `vector_deallocate`)

```c
#include "uniqueptr/uniqueptr.h"
#include "vector/vector.h"
#include "fmt/fmt.h"


/* Deleter that frees a c_std Vector* (not plain free). */
static void vector_deleter(void* p) {
    vector_deallocate((Vector*)p);
}

int main(void) {
    UPTR_AUTO UniquePtr v = uptr_new(vector_create(sizeof(int)), vector_deleter);
    Vector* vec = (Vector*)uptr_get(&v);

    for (int i = 1; i <= 4; ++i) {
        vector_push_back(vec, &i);
    }

    int sum = 0;
    for (size_t i = 0; i < vector_size(vec); ++i) {
        sum += *(int*)vector_at(vec, i);
    }

    fmt_printf("count = %zu\n", vector_size(vec));
    fmt_printf("sum   = %d\n", sum);
    return 0;
}
```

**Result**

```
count = 4
sum   = 10
```

---

## Example 15 — Owning a fixed-size `array` block

A type made with `array_create` is one flat block, so the default
deleter (`free`) is all it needs.

```c
#include "uniqueptr/uniqueptr.h"
#include "array/array.h"
#include "fmt/fmt.h"
#include <stdlib.h>


/* A fixed-size array-of-5-ints type, heap-allocated and owned by a UniquePtr. */
array_create(int, 5, Arr5);

int main(void) {
    Arr5* raw = (Arr5*)malloc(sizeof(Arr5));
    /* Default deleter (free) is fine: Arr5 is one flat block. */
    UPTR_AUTO UniquePtr a = uptr_new(raw, NULL);

    Arr5* arr = (Arr5*)uptr_get(&a);
    for (size_t i = 0; i < array_size(*arr); ++i) {
        array_at(*arr, i) = (int)(i * i);
    }

    fmt_printf("size  = %zu\n", array_size(*arr));
    fmt_printf("back  = %d\n", array_back(*arr));
    fmt_printf("at[3] = %d\n", array_at(*arr, 3));
    
    return 0;
}
```

**Result**

```
size  = 5
back  = 16
at[3] = 9
```

---

## Example 16 — `uptr_swap` trades two owned `String`s

`uptr_swap` exchanges the payloads in O(1); neither String is freed at
the swap. Each owner still frees exactly one String at scope exit.

```c
#include "uniqueptr/uniqueptr.h"
#include "string/std_string.h"
#include "fmt/fmt.h"


static void string_deleter(void* p) {
    string_deallocate((String*)p);
}

int main(void) {
    /* Two owners, each holding a heap String. */
    UPTR_AUTO UniquePtr a = uptr_new(string_create("alpha"), string_deleter);
    UPTR_AUTO UniquePtr b = uptr_new(string_create("beta"),  string_deleter);

    /* Trade the owned Strings in O(1); no String is freed here. */
    uptr_swap(&a, &b);

    fmt_printf("a now = %s\n", string_c_str((String*)uptr_get(&a)));
    fmt_printf("b now = %s\n", string_c_str((String*)uptr_get(&b)));
    /* Both string_deallocate calls run automatically at scope exit. */
    return 0;
}
```

**Result**

```
a now = beta
b now = alpha
```

---

## Example 17 — Custom deleter for a struct that owns a `Vector`

A resource that itself owns a c_std object needs a deleter that releases
the inner object first, then the struct. `uptr_take` moves the whole
thing to a new owner without double-freeing.

```c
#include "uniqueptr/uniqueptr.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <stdlib.h>


/* A small resource: a name plus an owned Vector of scores. */
typedef struct {
    const char* name;
    Vector*     scores;
} Report;

/* Custom deleter: free the inner Vector, then the Report struct. */
static void report_delete(void* p) {
    Report* r = (Report*)p;
    vector_deallocate(r->scores);
    free(r);
}

static UniquePtr make_report(const char* name) {
    Report* r = (Report*)malloc(sizeof(Report));
    r->name   = name;
    r->scores = vector_create(sizeof(int));
    return uptr_new(r, report_delete);
}

int main(void) {
    UniquePtr src = make_report("Q1");
    Report* r = (Report*)uptr_get(&src);
    for (int s = 10; s <= 30; s += 10) {
        vector_push_back(r->scores, &s);
    }

    /* Move-construct a new owner; src becomes empty (no double-free). */
    UPTR_AUTO UniquePtr dst = uptr_take(&src);
    Report* d = (Report*)uptr_get(&dst);

    fmt_printf("src empty? %d\n", uptr_is_null(&src));
    fmt_printf("name       = %s\n", d->name);
    fmt_printf("scores     = %zu\n", vector_size(d->scores));
    fmt_printf("first      = %d\n", *(int*)vector_at(d->scores, 0));
    /* report_delete runs once, for dst, at scope exit. */
    return 0;
}
```

**Result**

```
src empty? 1
name       = Q1
scores     = 3
first      = 10
```

---

## License

This project is open-source and available under [ISC License].
