# Time Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview

The Time library in C is a comprehensive tool designed for handling time-related operations. Inspired by the functionality of more advanced languages, this library provides a robust set of functions for manipulating and comparing time values in C programs. It is ideal for developers who need detailed time manipulation capabilities in a C environment.

The library includes functions for creating time objects, adding or subtracting time, comparing different time objects, and converting time to string representations. It's designed to be both efficient and easy to use, making it a valuable addition to any C project that requires time handling.

## Key Features

- **Time Creation and Manipulation**: Functions to create and modify time objects.
- **Time Comparison**: Comprehensive set of functions to compare time objects.
- **String Conversion**: Convert time objects to human-readable string format.
- **Validation and Utility Functions**: Ensure time objects are valid and provide utility functions for common operations.

## Installation and Compilation

To use this library, include `time/std_time.h` in your project. Compile the source files with your C compiler. For example, using GCC:

```bash
gcc -std=c17 -O2 -Wall -Wextra -pedantic -o main main.c time/std_time.c
```

If your program also uses `fmt` or `string`, add their `.c` files to the command line:

```bash
gcc -std=c17 -O2 -Wall -Wextra -pedantic -o main \
    main.c time/std_time.c fmt/fmt.c string/std_string.c encoding/encoding.c -lm
```

## Function Descriptions


### `Time* time_create(int h, int m, int s, int ms)`

**Purpose**: Initializes a `Time` object with the provided hour, minute, second, and millisecond values.

**Parameters**:
- `h`: The hour (0-23).
- `m`: The minute (0-59).
- `s`: The second (0-59).
- `ms`: The millisecond (0-999).

**Return Value**: A pointer to the newly created `Time` object, or `NULL` if any parameter is out of range or memory allocation fails.

**Usage Case**: Used to create and initialize a `Time` object with specific values.

---

### `Time* time_current_time(void)`

**Purpose**: Returns the current system time as a `Time` object.

**Parameters**: None.

**Return Value**: A pointer to a `Time` object representing the current system time.

**Usage Case**: Used to get the system’s current local time in a `Time` object.

---

### `bool time_is_valid(const Time* t)`

**Purpose**: Checks if the provided `Time` object is valid, ensuring that the hour, minute, second, and millisecond values fall within acceptable ranges.

**Parameters**:
- `t`: A pointer to the `Time` object to validate.

**Return Value**: `true` if the `Time` object is valid, `false` otherwise.

**Usage Case**: Used to ensure the time values are within valid ranges before performing operations.

---

### `void time_add_msecs(Time *t, int ms)`

**Purpose**: Adds the specified number of milliseconds to the provided `Time` object, handling the overflow of milliseconds into seconds, seconds into minutes, and minutes into hours.

**Parameters**:
- `t`: A pointer to the `Time` object.
- `ms`: The number of milliseconds to add.

**Return Value**: None.

**Usage Case**: Used to increment the time by a specified number of milliseconds, adjusting for overflow.

---

### `void time_add_secs(Time* t, int s)`

**Purpose**: Adds the specified number of seconds to the provided `Time` object, handling the overflow of seconds into minutes and minutes into hours.

**Parameters**:
- `t`: A pointer to the `Time` object.
- `s`: The number of seconds to add.

**Return Value**: None.

**Usage Case**: Used to increment the time by a specified number of seconds, adjusting for overflow.

---

### `bool time_is_null(const Time* t)`

**Purpose**: Checks whether the provided `Time` object is `NULL`.

**Parameters**:
- `t`: A pointer to the `Time` object.

**Return Value**: `true` if the `Time` object is `NULL`, `false` otherwise.

**Usage Case**: Used to verify if a `Time` object is uninitialized or `NULL`.

---

### `int time_hour(const Time* t)`

**Purpose**: Returns the hour component of the provided `Time` object.

**Parameters**:
- `t`: A pointer to the `Time` object.

**Return Value**: The hour component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.

**Usage Case**: Used to retrieve the hour from a `Time` object.

---

### `int time_minute(const Time* t)`

**Purpose**: Returns the minute component of the provided `Time` object.

**Parameters**:
- `t`: A pointer to the `Time` object.

**Return Value**: The minute component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.

**Usage Case**: Used to retrieve the minute from a `Time` object.

---

### `int time_second(const Time* t)`

**Purpose**: Returns the second component of the provided `Time` object.

**Parameters**:
- `t`: A pointer to the `Time` object.

**Return Value**: The second component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.

**Usage Case**: Used to retrieve the second from a `Time` object.

---

### `int time_msec(const Time* t)`

**Purpose**: Returns the millisecond component of the provided `Time` object.

**Parameters**:
- `t`: A pointer to the `Time` object.

**Return Value**: The millisecond component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.

**Usage Case**: Used to retrieve the millisecond value from a `Time` object.

---

### `int time_msecs_since_start_of_day()`

**Purpose**: Returns the number of milliseconds that have elapsed since the start of the day (00:00:00.000).

**Parameters**: None.

**Return Value**: The number of milliseconds since the start of the day, or `-1` if the current time could not be retrieved.

**Usage Case**: Used to calculate how many milliseconds have passed since midnight.

---

### `int time_msecs_to(Time* from, Time* to)`

**Purpose**: Computes the difference in milliseconds between two `Time` objects (`from` and `to`). The result is adjusted to ensure it is within the range of -86400000 to 86400000 milliseconds.

**Parameters**:
- `from`: A pointer to the starting `Time` object.
- `to`: A pointer to the ending `Time` object.

**Return Value**: The difference in milliseconds between the two `Time` objects, or `0` if either object is `NULL` or invalid.

**Usage Case**: Used to calculate the time difference between two `Time` objects in milliseconds.

---

### `int time_secs_to(Time* from, Time* to)`

**Purpose**: Computes the difference in seconds between two `Time` objects (`from` and `to`). The result is adjusted to ensure it is within the range of -86400 and 86400 seconds (the number of seconds in a day).

**Parameters**:
- `from`: A pointer to the starting `Time` object.
- `to`: A pointer to the ending `Time` object.

**Return Value**: The difference in seconds between the two `Time` objects, or `0` if either `Time` object is `NULL` or invalid.

**Usage Case**: Used to calculate the time difference in seconds between two `Time` objects.

---

### `bool time_set_hms(Time *t, int h, int m, int s, int ms)`

**Purpose**: Sets the hour, minute, second, and millisecond components of a `Time` object.

**Parameters**:
- `t`: A pointer to the `Time` object to be modified.
- `h`: The hour (0-23).
- `m`: The minute (0-59).
- `s`: The second (0-59).
- `ms`: The millisecond (0-999).

**Return Value**: `true` if the time was successfully set, `false` otherwise.

**Usage Case**: Used to modify the components of a `Time` object.

---

### `char* time_to_string(const Time* t)`

**Purpose**: Converts a `Time` object to a human-readable string in the format "(hh:mm:ss:ms)".

**Parameters**:
- `t`: A pointer to the `Time` object to be converted.

**Return Value**: A string representing the `Time` object, or `NULL` if the `Time` object is `NULL` or invalid.

**Usage Case**: Used to create a string representation of a `Time` object. The caller is responsible for freeing the allocated memory.

---

### `bool time_is_valid_time(int h, int m, int s, int ms)`

**Purpose**: Checks if the provided hour, minute, second, and millisecond values represent a valid time.

**Parameters**:
- `h`: The hour (0-23).
- `m`: The minute (0-59).
- `s`: The second (0-59).
- `ms`: The millisecond (0-999).

**Return Value**: `true` if the parameters represent a valid time, `false` otherwise.

**Usage Case**: Used to validate individual time components.

---

### `Time* time_from_msecs_since_start_of_day(int msecs)`

**Purpose**: Converts a time given in milliseconds since the start of the day into a `Time` object.

**Parameters**:
- `msecs`: The number of milliseconds since the start of the day (must be between 0 and 86399999).

**Return Value**: A pointer to the newly created `Time` object, or `NULL` if the input is invalid or memory allocation fails.

**Usage Case**: Used to create a `Time` object from a given number of milliseconds since the start of the day.

---

### `bool time_is_equal(const Time* lhs, const Time* rhs)`

**Purpose**: Checks if two `Time` objects represent the same time.

**Parameters**:
- `lhs`: A pointer to the first `Time` object.
- `rhs`: A pointer to the second `Time` object.

**Return Value**: `true` if the two `Time` objects are equal, `false` otherwise.

**Usage Case**: Used to compare two `Time` objects for equality.

---

### `bool time_is_less_than(const Time* lhs, const Time* rhs)`

**Purpose**: Checks if the first `Time` object (`lhs`) represents an earlier time than the second `Time` object (`rhs`).

**Parameters**:
- `lhs`: A pointer to the first `Time` object.
- `rhs`: A pointer to the second `Time` object.

**Return Value**: `true` if `lhs` is less than `rhs`, `false` otherwise. Returns `false` if either pointer is `NULL` or the `Time` objects are invalid.

**Usage Case**: Used to compare two `Time` objects to see if one represents an earlier time than the other.

---

### `bool time_is_less_than_or_equal(const Time* lhs, const Time* rhs)`

**Purpose**: Checks if the first `Time` object (`lhs`) represents an earlier or the same time as the second `Time` object (`rhs`).

**Parameters**:
- `lhs`: A pointer to the first `Time` object.
- `rhs`: A pointer to the second `Time` object.

**Return Value**: `true` if `lhs` is less than or equal to `rhs`, `false` otherwise.

**Usage Case**: Used to determine if one `Time` object is earlier than or equal to another.

---

### `bool time_is_greater_than(const Time* lhs, const Time* rhs)`

**Purpose**: Checks if the first `Time` object (`lhs`) represents a later time than the second `Time` object (`rhs`).

**Parameters**:
- `lhs`: A pointer to the first `Time` object.
- `rhs`: A pointer to the second `Time` object.

**Return Value**: `true` if `lhs` is greater than `rhs`, `false` otherwise. Returns `false` if either pointer is `NULL`.

**Usage Case**: Used to compare two `Time` objects to see if one represents a later time than the other.

---

### `bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs)`

**Purpose**: Checks if the first `Time` object (`lhs`) represents the same or a later time than the second `Time` object (`rhs`).

**Parameters**:
- `lhs`: A pointer to the first `Time` object.
- `rhs`: A pointer to the second `Time` object.

**Return Value**: `true` if `lhs` is greater than or equal to `rhs`, `false` otherwise.

**Usage Case**: Used to determine if one `Time` object is the same or later than another.

---

### `bool time_is_not_equal(const Time* lhs, const Time* rhs)`

**Purpose**: Checks if two `Time` objects do not represent the same time.

**Parameters**:
- `lhs`: A pointer to the first `Time` object.
- `rhs`: A pointer to the second `Time` object.

**Return Value**: `true` if `lhs` is not equal to `rhs`, `false` otherwise. Returns `true` if either pointer is `NULL`.

**Usage Case**: Used to determine if two `Time` objects represent different times.

---

### `void time_deallocate(Time* t)`

**Purpose**: Frees the memory allocated for a `Time` object.

**Parameters**:
- `t`: A pointer to the `Time` object to be deallocated.

**Return Value**: None.

**Usage Case**: Used to free up memory when a `Time` object is no longer needed.

---

### `double time_current_time_in_seconds()`

**Purpose**: Returns the current time in seconds, with fractional seconds represented as a double.

**Parameters**: None.

**Return Value**: The current time in seconds since the Unix epoch as a double.

**Usage Case**: Used to retrieve the current time in seconds with high precision.

---

### `double time_current_time_in_microseconds()`

**Purpose**: Returns the current time in microseconds, with fractional microseconds represented as a double.

**Parameters**: None.

**Return Value**: The current time in microseconds since the Unix epoch as a double.

**Usage Case**: Used to retrieve the current time in microseconds with high precision.

---

### `void time_sleep(unsigned int second)`

**Purpose**: Pauses the program for a specified number of seconds.

**Parameters**:
- `second`: The number of seconds to sleep.

**Return Value**: None.

**Usage Case**: Used to delay execution for a given number of seconds.

---

### `double time_diff_in_seconds(const Time* from, const Time* to)`

**Purpose**: Computes the difference in time between two `Time` objects, returning the result in seconds. The result is adjusted to ensure it is within the range of -86400 to 86400 seconds (the number of seconds in a day).

**Parameters**:
- `from`: A pointer to the starting `Time` object.
- `to`: A pointer to the ending `Time` object.

**Return Value**: The difference in time between the two `Time` objects in seconds as a double, or `0.0` if either `Time` object is `NULL` or invalid.

**Usage Case**: Used to calculate the time difference between two `Time` objects in seconds.

---

### `Time* time_copy(const Time* src)`

**Purpose**: Returns a deep, independent copy of a `Time` object.

**Parameters**:
- `src`: Pointer to the source `Time`. May be `NULL`.

**Return Value**: Newly allocated `Time*` the caller must free with `time_deallocate()`, or `NULL` on `NULL`/invalid input or allocation failure.

**Usage Case**: Snapshot a `Time` before further mutation, hand a copy to another subsystem without ownership concerns, or build a "previous"/"current" pair.

---

### `char* time_to_iso_string(const Time* t)`

**Purpose**: Formats a `Time` as a 12-character ISO 8601 extended local time string `"HH:MM:SS.mmm"`.

**Parameters**:
- `t`: Pointer to the `Time` to serialise.

**Return Value**: Newly malloc'd null-terminated string the caller must `free()`, or `NULL` on `NULL`/invalid input.

**Usage Case**: Logging, structured telemetry, and interop with parsers expecting RFC 3339 partial-time.

---

### `Time* time_from_string(const char* str)`

**Purpose**: Parses a textual time representation back into a `Time` object. Accepts `"HH:MM:SS"`, `"HH:MM:SS.mmm"`, `"HH:MM:SS:mmm"`, and the parenthesised form `"(HH:MM:SS:mmm)"`. Leading/trailing whitespace is tolerated; out-of-range components are rejected.

**Parameters**:
- `str`: Input string. May be `NULL`.

**Return Value**: Newly allocated `Time*` (caller frees with `time_deallocate()`), or `NULL` on `NULL`/invalid input.

**Usage Case**: Round-trip support against `time_to_string` and `time_to_iso_string`; parsing time fields from config files, CSV columns, or user input.

---

### `double time_to_seconds_of_day(const Time* t)`

**Purpose**: Total seconds since 00:00:00.000 as a fractional double, in `[0.0, 86400.0)`.

**Parameters**:
- `t`: Pointer to the `Time`.

**Return Value**: Fractional seconds since midnight, or `-1.0` if `t` is `NULL`/invalid.

**Usage Case**: Numerical computation, interpolation, or feeding time into a floating-point statistics pipeline.

---

### `Time* time_from_seconds_of_day(double secs)`

**Purpose**: Inverse of `time_to_seconds_of_day`. Builds a `Time` from a fractional seconds-of-day value. Returns `NULL` on out-of-range or non-finite input.

**Parameters**:
- `secs`: Seconds since midnight, fractional permitted, in `[0.0, 86400.0)`.

**Return Value**: Newly allocated `Time*` (caller frees with `time_deallocate()`), or `NULL`.

**Usage Case**: Convert back from a numerical seconds-of-day representation produced by interpolation or analytics.

---

### `int64_t time_now_unix_msecs(void)`

**Purpose**: Current Unix timestamp in milliseconds (since 1970-01-01T00:00:00 UTC) as a 64-bit signed integer.

**Parameters**: None.

**Return Value**: Milliseconds since the Unix epoch — safe past the 2038 boundary because it uses 64 bits.

**Usage Case**: Elapsed-time measurements, request timing, distributed-system event ordering.

---

### `bool time_is_midnight(const Time* t)`

**Purpose**: Returns `true` exactly when the `Time` is `00:00:00.000`.

**Parameters**:
- `t`: Pointer to the `Time`.

**Return Value**: `true` for midnight, `false` for any other valid time or for `NULL`/invalid input.

**Usage Case**: Day-rollover detection, scheduler edge cases, sentinel checks.

---

### `int time_compare(const Time* lhs, const Time* rhs)`

**Purpose**: Three-way comparison of two `Time` objects.

**Parameters**:
- `lhs`, `rhs`: Pointers to the two `Time` objects.

**Return Value**: `-1` if `lhs < rhs`, `0` if equal, `+1` if `lhs > rhs`. Returns `0` on `NULL`/invalid input.

**Usage Case**: Drop-in comparator for sorting `Time` arrays or building 3-state branches without nested `if`s.

---

### `double time_diff_in_msecs(const Time* from, const Time* to)`

**Purpose**: Exact difference between two `Time` objects in milliseconds (`to - from`).

**Parameters**:
- `from`, `to`: Pointers to the two `Time` objects.

**Return Value**: Difference in ms as a `double` (range `(-86_400_000, 86_400_000)`), or `0.0` if either input is `NULL`/invalid.

**Usage Case**: Millisecond-resolution telemetry, replacing `time_diff_in_seconds` when the integer part is too coarse.

---

### `char* time_format(const Time* t, const char* fmt)`

**Purpose**: `strftime`-like formatter for `Time` objects. Recognised specifiers: `%H` (00-23), `%I` (01-12), `%M`, `%S`, `%f` (000-999 ms), `%p` (AM/PM), `%%` literal `%`. Unknown specifiers pass through unchanged.

**Parameters**:
- `t`: Source `Time`.
- `fmt`: Format string.

**Return Value**: Newly malloc'd string the caller must `free()`, or `NULL` on `NULL`/invalid input.

**Usage Case**: Producing custom display formats (24-hour, 12-hour with AM/PM, log prefixes) without writing manual `snprintf` calls each time.

---



### Examples 

## Example 1: `time_create` and `time_to_string`
```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "time/std_time.h"


int main () {
    Time* t1 = time_create(14, 30, 45, 500);
    char* str1 = time_to_string(t1);
    
    fmt_printf("Time create and to string: %s\n", str1);

    free(str1);
    time_deallocate(t1);
    
    return 0;
}
```
**Result**
```
Time create and to string: (14:30:45:500)
```

---

## Example 2 : Testing `time_add_msecs`
```c
#include <stdlib.h>
#include "time/std_time.h"
#include "fmt/fmt.h"

int main() {
    Time *t2 = time_create(23, 59, 59, 500);
    time_add_msecs(t2, 1500); // Adds 1.5 seconds

    char *str2 = time_to_string(t2);
    fmt_printf("Add milliseconds: %s\n", str2);

    free(str2);
    time_deallocate(t2);

    return 0;
}
```
**Result**
```
Add milliseconds: (00:00:01:000)
```

---

## Example 3 :Testing `time_add_secs`
```c
#include <stdlib.h>
#include "time/std_time.h"
#include "fmt/fmt.h"

int main() {
    Time *t3 = time_create(1, 2, 3, 0);
    time_add_secs(t3, 3600); // Adds 1 hour

    char *str3 = time_to_string(t3);
    fmt_printf("Add seconds: %s\n", str3);
    
    free(str3);
    time_deallocate(t3);

    return 0;
}
```
**Result**
```
Add seconds: (02:02:03:000)
```

---

## Example 4: Testing `time_current_time`
```c
#include <stdlib.h>
#include "time/std_time.h"
#include "fmt/fmt.h"

int main() {
    Time *t4 = time_current_time();
    char *str4 = time_to_string(t4);

    fmt_printf("Current time: %s\n", str4);
    
    free(str4);
    time_deallocate(t4);

    return 0;
}
```
**Result**
```
Current time: (21:21:48:492)
```

---

## Example 5: Testing `time_msecs_since_start_of_day`
```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    int msecs = time_msecs_since_start_of_day();
    fmt_printf("Milliseconds since start of day: %d\n", msecs);
    
    return 0;
}   
```
**Result**
```
Milliseconds since start of day: 76943088
```

---

## Example 6: Testing `time_msecs_to`
```c
#include "time/std_time.h"
#include "fmt/fmt.h"

int main() {
    Time t5 = {10, 0, 0, 0};
    Time t6 = {11, 0, 0, 0};
    int diffMsecs = time_msecs_to(&t5, &t6);

    fmt_printf("Milliseconds difference: %d\n", diffMsecs);

    return 0;
}
```
**Result**
```
Milliseconds difference: 3600000
```

---

## Example 7: Testing `time_secs_to`
```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    Time t7 = {10, 0, 0, 0};
    Time t8 = {11, 0, 0, 0};
    int diffSecs = time_secs_to(&t7, &t8);
    
    fmt_printf("Seconds difference: %d\n", diffSecs);

    return 0;
}   
```
**Result**
```
Seconds difference: 3600
```

---

## Example 8: Testing `time_is_equal`
```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    Time *t9 = time_create(12, 0, 0, 0);
    Time *t10 = time_create(12, 0, 0, 0);
    
    fmt_printf("Times are equal: %s\n", time_is_equal(t9, t10) ? "true" : "false");

    time_deallocate(t9);
    time_deallocate(t10);

    return 0;
}
```
**Result**
```
Times are equal: true
```

---

## Example 9: Testing `time_is_greater_than` and `time_is_less_than`
```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    Time *t11 = time_create(14, 0, 0, 0);
    Time *t12 = time_create(15, 0, 0, 0);

    fmt_printf("Time t11 is less than t12: %s\n", time_is_less_than(t11, t12) ? "true" : "false");
    fmt_printf("Time t12 is greater than t11: %s\n", time_is_greater_than(t12, t11) ? "true" : "false");
    
    time_deallocate(t11);
    time_deallocate(t12);

    return 0;
}
```
**Result**
```
Time t11 is less than t12: true
Time t12 is greater than t11: true
```

---

## Example 10: Testing `time_from_msecs_since_start_of_day`
```c
#include <stdlib.h>
#include "time/std_time.h"
#include "fmt/fmt.h"

int main() {
    Time *t13 = time_from_msecs_since_start_of_day(45000000); // Arbitrary milliseconds
    char *str13 = time_to_string(t13);
    fmt_printf("Time from msecs since start of day: %s\n", str13);
    
    free(str13);
    time_deallocate(t13);
    
    return 0;
}    
```
**Result**
```
Time from msecs since start of day: (12:30:00:000)
```

---

## Example 11 : get current time in seconds and microseconds with `time_current_time_in_seconds` and `time_current_time_in_microseconds`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    fmt_printf("Current Time in seconds %f\n", time_current_time_in_seconds());
    fmt_printf("Current Time in msec %f\n", time_current_time_in_microseconds());

    return 0;
}
```
**Result**
```
Current Time in seconds 1718733335.131222
Current Time in msec 1718733335132198.000000
```

---

## Example 12: calculate diff Time with `time_diff_in_seconds`

```c 
#include "fmt/fmt.h"
#include "time/std_time.h"


int main() {
    Time* start_time = time_current_time();

    time_sleep(10);

    Time* end_time = time_current_time();
    double diff_time = time_diff_in_seconds(start_time, end_time);

    fmt_printf("Difference in Seconds : %lf", diff_time);

    time_deallocate(start_time);
    time_deallocate(end_time);

    return 0;
}
```
**Result**
```
Difference in Seconds : 10.007000
```

---

## Example 13: Deep copy with `time_copy`

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* original = time_create(13, 45, 30, 250);
    Time* snapshot = time_copy(original);

    /* Mutate the original — snapshot is unaffected. */
    time_add_secs(original, 60);

    char* o = time_to_string(original);
    char* s = time_to_string(snapshot);
    fmt_printf("original: %s\n", o);
    fmt_printf("snapshot: %s\n", s);

    free(o);
    free(s);
    time_deallocate(original);
    time_deallocate(snapshot);

    return 0;
}
```
**Result**
```
original: (13:46:30:250)
snapshot: (13:45:30:250)
```

---

## Example 14: ISO 8601 output with `time_to_iso_string`

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* t = time_create(9, 5, 7, 42);
    char* iso = time_to_iso_string(t);

    fmt_printf("iso: %s\n", iso);

    free(iso);
    time_deallocate(t);

    return 0;
}
```
**Result**
```
iso: 09:05:07.042
```

---

## Example 15: Parsing strings with `time_from_string`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* a = time_from_string("(14:30:45:678)");
    Time* b = time_from_string("09:00:00.500");
    Time* c = time_from_string("23:59:59");

    fmt_printf("a hour=%d msec=%d\n", time_hour(a), time_msec(a));
    fmt_printf("b hour=%d msec=%d\n", time_hour(b), time_msec(b));
    fmt_printf("c hour=%d msec=%d\n", time_hour(c), time_msec(c));

    time_deallocate(a);
    time_deallocate(b);
    time_deallocate(c);

    return 0;
}
```
**Result**
```
a hour=14 msec=678
b hour=9 msec=500
c hour=23 msec=0
```

---

## Example 16: Round-trip seconds-of-day

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* t = time_create(1, 30, 0, 500);

    double seconds = time_to_seconds_of_day(t);
    Time* back = time_from_seconds_of_day(seconds);

    fmt_printf("seconds: %.3f\n", seconds);
    fmt_printf("equal:   %s\n", time_is_equal(t, back) ? "true" : "false");

    time_deallocate(t);
    time_deallocate(back);

    return 0;
}
```
**Result**
```
seconds: 5400.500
equal:   true
```

---

## Example 17: Unix milliseconds with `time_now_unix_msecs`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    int64_t t0 = time_now_unix_msecs();
    time_sleep(1);
    int64_t t1 = time_now_unix_msecs();

    fmt_printf("elapsed: %lld ms\n", (long long)(t1 - t0));
    return 0;
}
```
**Result**
```
elapsed: 1001 ms
```

---

## Example 18: Midnight detection with `time_is_midnight`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* a = time_create(0, 0, 0, 0);
    Time* b = time_create(0, 0, 0, 1);

    fmt_printf("a is midnight: %s\n", time_is_midnight(a) ? "true" : "false");
    fmt_printf("b is midnight: %s\n", time_is_midnight(b) ? "true" : "false");

    time_deallocate(a);
    time_deallocate(b);

    return 0;
}
```
**Result**
```
a is midnight: true
b is midnight: false
```

---

## Example 19: Three-way comparison with `time_compare`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* a = time_create(10, 0, 0, 0);
    Time* b = time_create(11, 0, 0, 0);

    fmt_printf("compare(a, b) = %d\n", time_compare(a, b));
    fmt_printf("compare(b, a) = %d\n", time_compare(b, a));
    fmt_printf("compare(a, a) = %d\n", time_compare(a, a));

    time_deallocate(a);
    time_deallocate(b);

    return 0;
}
```
**Result**
```
compare(a, b) = -1
compare(b, a) = 1
compare(a, a) = 0
```

---

## Example 20: Millisecond difference with `time_diff_in_msecs`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* start = time_create(10, 0, 0, 0);
    Time* end   = time_create(10, 0, 1, 250);

    double diff = time_diff_in_msecs(start, end);
    fmt_printf("diff: %.3f ms\n", diff);

    time_deallocate(start);
    time_deallocate(end);

    return 0;
}
```
**Result**
```
diff: 1250.000 ms
```

---

## Example 21: Custom formatting with `time_format`

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "time/std_time.h"

int main(void) {
    Time* t = time_create(14, 5, 30, 7);

    char* a = time_format(t, "%H:%M:%S.%f");
    char* b = time_format(t, "%I:%M %p");
    char* c = time_format(t, "100%% sure at %H:%M");

    fmt_printf("24h: %s\n", a);
    fmt_printf("12h: %s\n", b);
    fmt_printf("lit: %s\n", c);

    free(a);
    free(b);
    free(c);
    time_deallocate(t);

    return 0;
}
```
**Result**
```
24h: 14:05:30.007
12h: 02:05 PM
lit: 100% sure at 14:05
```

---

## Example 22: Sorting `Time` arrays via `time_compare`

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "time/std_time.h"

static int time_qcmp(const void* a, const void* b) {
    /* qsort hands us pointers to elements; elements are Time* themselves. */
    const Time* const* pa = (const Time* const*)a;
    const Time* const* pb = (const Time* const*)b;

    return time_compare(*pa, *pb);
}

int main(void) {
    Time* arr[4] = {
        time_create(14, 0, 0, 0),
        time_create(8, 30, 0, 0),
        time_create(22, 15, 0, 0),
        time_create(8, 30, 0, 1),
    };

    qsort(arr, 4, sizeof(Time*), time_qcmp);

    for (int i = 0; i < 4; i++) {
        char* s = time_to_string(arr[i]);
        fmt_printf("%d: %s\n", i, s);
        free(s);
        time_deallocate(arr[i]);
    }
    return 0;
}
```
**Result**
```
0: (08:30:00:000)
1: (08:30:00:001)
2: (14:00:00:000)
3: (22:15:00:000)
```

---

## License

This project is open-source and available under [ISC License].