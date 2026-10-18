# UUID Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2026
**License:** ISC License

## Overview

The UUID library implements Universally Unique Identifiers per **RFC 9562**
(the specification that obsoletes RFC 4122), with an API modelled on Python's
[`uuid`](https://docs.python.org/3/library/uuid.html) module. A `UUID` is a
16-byte value type — generators return one by value and formatting writes into a
caller-supplied buffer, so the **entire module is allocation-free** (there is
nothing to leak).

It generates the modern, recommended versions — **v4** (random) and **v7**
(Unix-time-ordered) — plus the name-based **v3** (MD5) and **v5** (SHA-1), whose
hashes are embedded so the module needs no OpenSSL. Randomness for v4/v7 comes
from the cross-platform `secrets` CSPRNG.

## Key Features

- **RFC 9562 versions:** v4 (random), v7 (Unix-time-ordered, sortable — ideal for
  database keys), v3 (MD5 name-based), v5 (SHA-1 name-based).
- **Special UUIDs:** the Nil (all-zero) and Max (all-one) identifiers.
- **Predefined namespaces:** DNS, URL, OID, X.500 — for v3/v5.
- **Faithful to Python:** `uuid_v3`/`uuid_v5` reproduce the exact values from the
  Python `uuid` documentation, e.g. `uuid5(NAMESPACE_DNS, "python.org")` →
  `886313e1-3b8a-5372-9b90-0c9aee199e5d`.
- **Formatting & parsing:** canonical lowercase string, `urn:uuid:` form, and a
  lenient parser that accepts hyphenated, bare-32-hex, brace-wrapped, and
  `urn:uuid:` inputs case-insensitively.
- **Inspection:** version, variant, Nil/Max checks, equality, ordering
  (`uuid_compare`), and v7 timestamp extraction.
- **Cross-platform & allocation-free:** pure C; works on Windows and POSIX with
  no heap allocation anywhere.

## Installation and Compilation

Include `uuid.h` and compile `uuid.c` together with the `secrets` module (its
CSPRNG drives v4/v7). On Windows, `secrets` links `bcrypt`:

```bash
# Linux / macOS
gcc -std=c17 -O2 main.c uuid/uuid.c secrets/secrets.c -o main

# Windows (MinGW)
gcc -std=c17 -O2 main.c uuid/uuid.c secrets/secrets.c -lbcrypt -o main.exe
```

MD5 and SHA-1 are embedded, so **no OpenSSL is required**.

### The `UUID` name and `<windows.h>`

The type is `Uuid` (used throughout this README); `UUID` is an alias for it. Windows declares its own `UUID` (a `GUID`) in `<rpcdce.h>`, which
`<windows.h>` pulls in, and the two names used to collide in any translation
unit that had both (including one with c_std's `unittest.h`, `encoding.h`,
`serial_port.h`, `concurrent.h` or the network headers). Now both include
orders compile:

- `<windows.h>` (or one of those headers) first: `UUID` keeps meaning Windows'
  `GUID`; write `Uuid` for this library's values.
- `uuid.h` first: `UUID` is this library's type, and a later `<windows.h>`
  does not redeclare it (Windows RPC functions such as `UuidCreate` then take a
  `Uuid*`, which has the same 16-byte layout).

Portable code can simply use `Uuid` everywhere. Define
`CSTD_UUID_NO_ALIAS` to never declare `UUID`.

## Function Descriptions

### `Uuid uuid_v4(void)`

**Purpose**:  
Generates a version-4 (random) UUID — 122 bits come from the cross-platform `secrets` CSPRNG (BCryptGenRandom on Windows, `getrandom()` with a `/dev/urandom` fallback on Linux), with the version nibble set to 4 and the variant to RFC 4122. If the OS generator fails, the bits come from a clock/counter fallback that is not cryptographically random but still never repeats a UUID within the process (the same applies to `uuid_v7`).

**Parameters**:  
None.

**Return Value**:  
A freshly generated v4 UUID (returned by value).

**Usage Case**:  
Use when you need an unpredictable identifier and don't care about ordering — session tokens, object IDs, request IDs.

---

### `Uuid uuid_v7(void)`

**Purpose**:  
Generates a version-7 (Unix-time-ordered) UUID. The first 48 bits are the current Unix timestamp in milliseconds (big-endian), so v7 UUIDs sort chronologically. The next 42 free bits are a counter (RFC 9562 §6.2 method 1, as in Python 3.14): it starts at a random value in every new millisecond and is incremented for each UUID made in the same millisecond, so within one process every `uuid_v7()` compares greater than the previous one, also across threads (it used to be in random order within a millisecond, out of order about half the time). If the clock goes backwards the last timestamp is reused, and if the counter runs out the timestamp moves 1 ms ahead. The last 32 bits are random. Across processes the order holds at millisecond granularity.

**Parameters**:  
None.

**Return Value**:  
A freshly generated v7 UUID (returned by value).

**Usage Case**:  
The recommended choice for new database primary keys and log identifiers — RFC 9562 prefers v7 over v1/v6 because the time prefix keeps inserts local in B-tree indexes.

---

### `Uuid uuid_v3(Uuid namespace_id, const char *name)`

**Purpose**:  
Generates a deterministic version-3 (name-based, **MD5**) UUID by hashing the 16 bytes of `namespace_id` followed by `name`. The same namespace and name always produce the same UUID.

**Parameters**:  
- `namespace_id`: The namespace UUID (e.g. `uuid_namespace_dns()`).
- `name`: The name to hash within that namespace. A `NULL` name is treated as the empty string.

**Return Value**:  
The name-based v3 UUID (returned by value).

**Usage Case**:  
Derive a stable ID from a known string for interoperability with existing MD5-based UUIDs. For new code prefer `uuid_v5`.

---

### `Uuid uuid_v5(Uuid namespace_id, const char *name)`

**Purpose**:  
Like `uuid_v3` but hashes with **SHA-1** (the first 16 bytes of the 20-byte digest). Deterministic, and the recommended name-based version.

**Parameters**:  
- `namespace_id`: The namespace UUID.
- `name`: The name to hash. A `NULL` name is treated as the empty string.

**Return Value**:  
The name-based v5 UUID (returned by value).

**Usage Case**:  
Stable IDs for URLs, file paths, or account names — anywhere the same input must always map to the same UUID.

---

### `Uuid uuid_nil(void)`

**Purpose**:  
Returns the Nil UUID — all 128 bits zero (`00000000-0000-0000-0000-000000000000`).

**Parameters**:  
None.

**Return Value**:  
The Nil UUID (returned by value).

**Usage Case**:  
Use as a sentinel "no UUID" value, e.g. the default in a struct before a real UUID is assigned.

---

### `Uuid uuid_max(void)`

**Purpose**:  
Returns the Max UUID — all 128 bits one (`ffffffff-ffff-ffff-ffff-ffffffffffff`), defined by RFC 9562.

**Parameters**:  
None.

**Return Value**:  
The Max UUID (returned by value).

**Usage Case**:  
A convenient upper sentinel for range comparisons over UUIDs.

---

### `Uuid uuid_namespace_dns(void)`

**Purpose**:  
Returns the predefined DNS namespace UUID (`6ba7b810-9dad-11d1-80b4-00c04fd430c8`) from RFC 9562 Appendix C.

**Parameters**:  
None.

**Return Value**:  
The DNS namespace UUID (returned by value).

**Usage Case**:  
Pass as the `namespace_id` argument to `uuid_v3` / `uuid_v5` when the name is a domain name.

---

### `Uuid uuid_namespace_url(void)`

**Purpose**:  
Returns the predefined URL namespace UUID (`6ba7b811-9dad-11d1-80b4-00c04fd430c8`).

**Parameters**:  
None.

**Return Value**:  
The URL namespace UUID (returned by value).

**Usage Case**:  
Use with `uuid_v3` / `uuid_v5` when the name is a URL.

---

### `Uuid uuid_namespace_oid(void)`

**Purpose**:  
Returns the predefined ISO OID namespace UUID (`6ba7b812-9dad-11d1-80b4-00c04fd430c8`).

**Parameters**:  
None.

**Return Value**:  
The OID namespace UUID (returned by value).

**Usage Case**:  
Use with `uuid_v3` / `uuid_v5` when the name is an ISO object identifier.

---

### `Uuid uuid_namespace_x500(void)`

**Purpose**:  
Returns the predefined X.500 namespace UUID (`6ba7b814-9dad-11d1-80b4-00c04fd430c8`).

**Parameters**:  
None.

**Return Value**:  
The X.500 namespace UUID (returned by value).

**Usage Case**:  
Use with `uuid_v3` / `uuid_v5` when the name is an X.500 distinguished name.

---

### `void uuid_to_string(Uuid u, char out[UUID_STR_LEN])`

**Purpose**:  
Writes the canonical lowercase string form `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`, plus a NUL terminator, into `out`.

**Parameters**:  
- `u`: The UUID to format.
- `out`: Destination buffer of at least `UUID_STR_LEN` (37) bytes.

**Return Value**:  
None.

**Usage Case**:  
Render a UUID for logging, JSON, HTTP headers, or database text columns.

---

### `void uuid_to_urn(Uuid u, char out[UUID_URN_LEN])`

**Purpose**:  
Writes the URN form `urn:uuid:xxxxxxxx-…`, plus a NUL terminator, into `out`.

**Parameters**:  
- `u`: The UUID to format.
- `out`: Destination buffer of at least `UUID_URN_LEN` (46) bytes.

**Return Value**:  
None.

**Usage Case**:  
Emit the RFC 9562 URN form where a namespaced URI is expected.

---

### `bool uuid_from_string(const char *str, Uuid *out)`

**Purpose**:  
Parses a UUID string into `out`. Accepts the canonical hyphenated form, a bare 32-hex-digit form, a brace-wrapped `{…}` form, and the `urn:uuid:…` form, all case-insensitively; hyphens are optional and ignored.

**Parameters**:  
- `str`: The string to parse. Pass `NULL` and the call fails safely.
- `out`: Receives the parsed UUID on success; left untouched on failure.

**Return Value**:  
`true` on success; `false` if `str` / `out` is `NULL` or the input does not contain exactly 32 hexadecimal digits.

**Usage Case**:  
Read a UUID back from configuration, a request parameter, or a database column.

---

### `void uuid_to_bytes(Uuid u, unsigned char out[16])`

**Purpose**:  
Copies the 16 raw bytes (big-endian field order) of `u` into `out`.

**Parameters**:  
- `u`: The UUID.
- `out`: Destination buffer of at least 16 bytes.

**Return Value**:  
None.

**Usage Case**:  
Store a UUID as a compact 16-byte BLOB or transmit it on the wire.

---

### `Uuid uuid_from_bytes(const unsigned char in[16])`

**Purpose**:  
Builds a UUID from 16 raw bytes (big-endian field order). The bytes are stored verbatim — no version/variant fixing — so it round-trips exactly with `uuid_to_bytes`.

**Parameters**:  
- `in`: Source of 16 bytes.

**Return Value**:  
The UUID holding those bytes (returned by value).

**Usage Case**:  
Reconstruct a UUID from a 16-byte BLOB read from storage or the network.

---

### `int uuid_version(Uuid u)`

**Purpose**:  
Returns the version nibble of `u` (octet 6, high 4 bits).

**Parameters**:  
- `u`: The UUID.

**Return Value**:  
The version number, 0–15 (4, 7, 3, or 5 for UUIDs from this library).

**Usage Case**:  
Branch on how a UUID was generated, or validate that an external UUID is the version you expect.

---

### `UuidVariant uuid_variant(Uuid u)`

**Purpose**:  
Returns the variant of `u` (the top bits of octet 8).

**Parameters**:  
- `u`: The UUID.

**Return Value**:  
One of `UUID_VARIANT_NCS`, `UUID_VARIANT_RFC4122` (what this library produces), `UUID_VARIANT_MICROSOFT`, or `UUID_VARIANT_FUTURE`.

**Usage Case**:  
Verify that a UUID uses the standard RFC 4122 / 9562 variant before relying on its layout.

---

### `bool uuid_is_nil(Uuid u)`

**Purpose**:  
Tests whether `u` is the Nil UUID (all 16 bytes zero).

**Parameters**:  
- `u`: The UUID.

**Return Value**:  
`true` if `u` is the Nil UUID, otherwise `false`.

**Usage Case**:  
Detect an unset/sentinel UUID before using it.

---

### `bool uuid_is_max(Uuid u)`

**Purpose**:  
Tests whether `u` is the Max UUID (all 16 bytes `0xFF`).

**Parameters**:  
- `u`: The UUID.

**Return Value**:  
`true` if `u` is the Max UUID, otherwise `false`.

**Usage Case**:  
Detect the Max sentinel, e.g. as an open-ended range upper bound.

---

### `bool uuid_equal(Uuid a, Uuid b)`

**Purpose**:  
Compares two UUIDs for byte-for-byte equality.

**Parameters**:  
- `a`: The first UUID.
- `b`: The second UUID.

**Return Value**:  
`true` if `a` and `b` are equal, otherwise `false`.

**Usage Case**:  
Membership tests, deduplication, and asserting that two IDs refer to the same entity.

---

### `int uuid_compare(Uuid a, Uuid b)`

**Purpose**:  
Lexicographically orders two UUIDs by their 16 big-endian bytes. For v7 UUIDs this is also chronological order (creation order within one process).

`uuid_compare` and `uuid_variant` are macros for the linked functions `cstd_uuid_compare` / `cstd_uuid_variant`, because Linux libuuid (util-linux) exports functions with the same names and other signatures. Define `CSTD_UUID_NO_FUNC_ALIAS` to drop the two macros (for example in a file that also includes libuuid's header) and call the `cstd_` names. Note that with the c_std root on the include path, `#include <uuid/uuid.h>` finds this header, not libuuid's; give libuuid's directory first (or add the c_std root with `-idirafter`) in code that needs both.

**Parameters**:  
- `a`: The first UUID.
- `b`: The second UUID.

**Return Value**:  
A negative value if `a < b`, `0` if they are equal, a positive value if `a > b`.

**Usage Case**:  
Use as the comparator when sorting UUIDs or keying an ordered container — v7 UUIDs sort into time order.

---

### `unsigned long long uuid_v7_timestamp_ms(Uuid u)`

**Purpose**:  
Extracts the embedded Unix-millisecond timestamp from a version-7 UUID.

**Parameters**:  
- `u`: The UUID.

**Return Value**:  
The 48-bit timestamp in milliseconds since the Unix epoch, or `0` if `u` is not a v7 UUID.

**Usage Case**:  
Recover the creation time of a v7-keyed record without a separate timestamp column.

---

## Examples

### Example 1: Generating and inspecting UUIDs

```c
#include "uuid/uuid.h"
#include "fmt/fmt.h"

int main(void) {
    char s[UUID_STR_LEN];

    /* Random (v4) and time-ordered (v7) — values differ every run, so we
       report their version/variant rather than the (random) text. */
    Uuid a = uuid_v4();
    Uuid b = uuid_v7();
    
    fmt_printf("v4: version=%d  rfc4122=%d\n", uuid_version(a), uuid_variant(a) == UUID_VARIANT_RFC4122);
    fmt_printf("v7: version=%d  rfc4122=%d\n", uuid_version(b), uuid_variant(b) == UUID_VARIANT_RFC4122);

    /* Name-based (v5) is deterministic — same input, same UUID, matching
       Python's uuid5(NAMESPACE_DNS, "python.org"). */
    uuid_to_string(uuid_v5(uuid_namespace_dns(), "python.org"), s);
    fmt_printf("v5(dns, python.org): %s\n", s);

    /* Parse a canonical string back into a UUID and round-trip it. */
    Uuid parsed;
    if (uuid_from_string("6ba7b810-9dad-11d1-80b4-00c04fd430c8", &parsed)) {
        uuid_to_string(parsed, s);
        fmt_printf("parsed namespace DNS: %s\n", s);
    }

    return 0;
}
```

**Result**
```
v4: version=4  rfc4122=1
v7: version=7  rfc4122=1
v5(dns, python.org): 886313e1-3b8a-5372-9b90-0c9aee199e5d
parsed namespace DNS: 6ba7b810-9dad-11d1-80b4-00c04fd430c8
```

### Example 2: Special values, name-based determinism, and the URN form

```c
#include "uuid/uuid.h"
#include "fmt/fmt.h"

int main(void) {
    char s[UUID_STR_LEN], urn[UUID_URN_LEN];

    uuid_to_string(uuid_nil(), s); 
    fmt_printf("nil: %s\n", s);

    uuid_to_string(uuid_max(), s); 
    fmt_printf("max: %s\n", s);

    /* Deterministic name-based IDs (match Python's uuid3/uuid5). */
    uuid_to_string(uuid_v3(uuid_namespace_dns(), "python.org"), s); 
    fmt_printf("v3:  %s\n", s);

    uuid_to_string(uuid_v5(uuid_namespace_dns(), "python.org"), s); 
    fmt_printf("v5:  %s\n", s);

    /* Same namespace + name always yields the same UUID. */
    Uuid a = uuid_v5(uuid_namespace_url(), "https://example.com");
    Uuid b = uuid_v5(uuid_namespace_url(), "https://example.com");

    fmt_printf("v5 stable across calls: %d\n", uuid_equal(a, b));

    uuid_to_urn(uuid_namespace_dns(), urn);
    fmt_printf("urn: %s\n", urn);

    return 0;
}
```

**Result**
```
nil: 00000000-0000-0000-0000-000000000000
max: ffffffff-ffff-ffff-ffff-ffffffffffff
v3:  6fa459ea-ee8a-3ca4-894e-db77e160355e
v5:  886313e1-3b8a-5372-9b90-0c9aee199e5d
v5 stable across calls: 1
urn: urn:uuid:6ba7b810-9dad-11d1-80b4-00c04fd430c8
```

### Example 3: Parsing every accepted form and inspecting a UUID

```c
#include "uuid/uuid.h"
#include "fmt/fmt.h"

int main(void) {
    Uuid u;

    /* All five input forms parse to the same UUID. */
    const char* forms[] = {
        "6ba7b810-9dad-11d1-80b4-00c04fd430c8",
        "6BA7B810-9DAD-11D1-80B4-00C04FD430C8",
        "{6ba7b810-9dad-11d1-80b4-00c04fd430c8}",
        "urn:uuid:6ba7b810-9dad-11d1-80b4-00c04fd430c8",
        "6ba7b8109dad11d180b400c04fd430c8"
    };

    int all_match = 1;
    for (size_t i = 0; i < sizeof(forms) / sizeof(forms[0]); i++) {
        if (!uuid_from_string(forms[i], &u) || !uuid_equal(u, uuid_namespace_dns())) {
            all_match = 0;
        }
    }

    fmt_printf("all 5 forms parse to NS_DNS: %d\n", all_match);
    fmt_printf("reject \"not-a-uuid\": %d\n", uuid_from_string("not-a-uuid", &u) == false);

    Uuid v5 = uuid_v5(uuid_namespace_dns(), "python.org");
    fmt_printf("v5 version=%d rfc4122=%d\n", uuid_version(v5), uuid_variant(v5) == UUID_VARIANT_RFC4122);

    unsigned char raw[16];
    uuid_to_bytes(v5, raw);
    fmt_printf("bytes round-trip: %d\n", uuid_equal(v5, uuid_from_bytes(raw)));

    return 0;
}
```

**Result**
```
all 5 forms parse to NS_DNS: 1
reject "not-a-uuid": 1
v5 version=5 rfc4122=1
bytes round-trip: 1
```

### Example 4: v7 UUIDs sort chronologically with `uuid_compare`

Because a v7 UUID begins with its millisecond timestamp, sorting v7 UUIDs by
`uuid_compare` (e.g. via `qsort`) puts them in creation order — the property
that makes them excellent database primary keys.

```c
#include "uuid/uuid.h"
#include "fmt/fmt.h"
#include <stdlib.h>


/* qsort comparator over UUID using uuid_compare. */
static int cmp(const void* a, const void* b) {
    return uuid_compare(*(const Uuid*)a, *(const Uuid*)b);
}

/* Build a v7-shaped UUID with a chosen millisecond timestamp. */
static Uuid v7_with_ts(unsigned long long ms) {
    unsigned char b[16] = {0};

    for (int i = 0; i < 6; i++) {
        b[i] = (unsigned char)(ms >> (8 * (5 - i)));
    }
    b[6] = 0x70;   /* version 7 */
    b[8] = 0x80;   /* RFC4122 variant */

    return uuid_from_bytes(b);
}


int main(void) {
    /* Three v7 UUIDs created out of timestamp order. */
    Uuid arr[3] = { v7_with_ts(3000), v7_with_ts(1000), v7_with_ts(2000) };
    qsort(arr, 3, sizeof(Uuid), cmp);

    fmt_printf("sorted timestamps: %llu %llu %llu\n",
               uuid_v7_timestamp_ms(arr[0]),
               uuid_v7_timestamp_ms(arr[1]),
               uuid_v7_timestamp_ms(arr[2]));

    fmt_printf("ascending: %d\n", uuid_compare(arr[0], arr[1]) < 0 && uuid_compare(arr[1], arr[2]) < 0);

    Uuid now = uuid_v7();
    fmt_printf("live v7 is version 7: %d\n", uuid_version(now) == 7);
    
    return 0;
}
```

**Result**
```
sorted timestamps: 1000 2000 3000
ascending: 1
live v7 is version 7: 1
```

## License

This project is open-source and available under [ISC License].
