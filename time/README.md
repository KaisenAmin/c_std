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

To use this library, include `time.h` in your project. Compile the source files with your C compiler. For example, using GCC:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c time.c
```

## Function Descriptions

- `Time* time_create(int h, int m, int s, int ms)`: Initializes a new Time object with specified hours, minutes, seconds, and milliseconds.
- `void time_add_msecs(Time *t, int ms)`: Adds milliseconds to a Time object.
- `void time_add_secs(Time* t, int s)`: Adds seconds to a Time object.
- `int time_hour(const Time* t)`: Returns the hour part of a Time object.
- `bool time_is_null(const Time* t)`: Checks if the given Time object is null.
- `bool time_is_valid(const Time* t)`: Validates whether a Time object represents a valid time.
- `int time_minute(const Time* t)`: Returns the minute part of a Time object.
- `int time_msec(const Time* t)`: Returns the milliseconds part of a Time object.
- `int time_msecs_since_start_of_day()`: Returns the number of milliseconds since the start of the day.
- `int time_msecs_to(Time* from, Time* to)`: Calculates the difference in milliseconds between two Time objects.
- `int time_second(const Time* t)`: Returns the second part of a Time object.
- `int time_secs_to(Time* from, Time* to)`: Calculates the difference in seconds between two Time objects.
- `bool time_set_hms(Time *t, int h, int m, int s, int ms)`: Sets the hour, minute, second, and millisecond values of a Time object.

- `double time_current_time_in_seconds` : get current time in seconds;
- `double time_current_time_in_microseconds` : get current time in microseconds.
- `char* time_to_string(const Time* t)`: Converts a Time object to a string format.
- `Time* time_current_time(void)`: Gets the current system time.
- `Time* time_from_msecs_since_start_of_day(int msecs)`: Creates a Time object from milliseconds since the start of the day.
- `bool time_is_equal(const Time* lhs, const Time* rhs)`: Checks if two Time objects are equal.
- `bool time_is_less_than(const Time* lhs, const Time* rhs)`: Determines if one Time object is less than another.
- `bool time_is_less_than_or_equal(const Time* lhs, const Time* rhs)`: Determines if one Time object is less than or equal to another.
- `bool time_is_greater_than(const Time* lhs, const Time* rhs)`: Determines if one Time object is greater than another.
- `bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs)`: Determines if one Time object is greater than or equal to another.
- `bool time_is_not_equal(const Time* lhs, const Time* rhs)`: Checks if two Time objects are not equal.

To test the functionality of your `Time` library, you can create a series of examples that cover each function. Here are 10 examples, one for each major functionality:

## Example 1: `time_create` and `time_to_string`
```c
#include "fmt/fmt.h"
#include "time/time.h"
#include <stdlib.h>

int main () {
    Time* t1 = time_create(14, 30, 45, 500);
    char* str1 = time_to_string(t1);
    
    fmt_printf("Time create and to string: %s\n", str1);

    free(str1);
    time_deallocate(t1);
    
    return 0;
}
```

## Example 2 : Testing `time_add_msecs`
```c
#include <stdlib.h>
#include "time/time.h"
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

## Example 3 :Testing `time_add_secs`
```c
#include <stdlib.h>
#include "time/time.h"
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

## Example 4: Testing `time_current_time`
```c
#include <stdlib.h>
#include "time/time.h"
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

## Example 5: Testing `time_msecs_since_start_of_day`
```c
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    int msecs = time_msecs_since_start_of_day();
    fmt_printf("Milliseconds since start of day: %d\n", msecs);
    
    return 0;
}   
```

## Example 6: Testing `time_msecs_to`
```c
#include "time/time.h"
#include "fmt/fmt.h"

int main() {
    Time t5 = {10, 0, 0, 0};
    Time t6 = {11, 0, 0, 0};
    int diffMsecs = time_msecs_to(&t5, &t6);

    fmt_printf("Milliseconds difference: %d\n", diffMsecs);

    return 0;
}
```

## Example 7: Testing `time_secs_to`
```c
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    Time t7 = {10, 0, 0, 0};
    Time t8 = {11, 0, 0, 0};
    int diffSecs = time_secs_to(&t7, &t8);
    
    fmt_printf("Seconds difference: %d\n", diffSecs);

    return 0;
}   
```

## Example 8: Testing `time_is_equal`
```c
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    Time *t9 = time_create(12, 0, 0, 0);
    Time *t10 = time_create(12, 0, 0, 0);
    
    fmt_printf("Times are equal: %s\n", time_is_equal(t9, t10) ? "true" : "false");

    time_deallocate(t9);
    time_deallocate(t10);
    return 0;
}
```

## Example 9: Testing `time_is_greater_than` and `time_is_less_than`
```c
#include "fmt/fmt.h"
#include "time/time.h"

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

## Example 10: Testing `time_from_msecs_since_start_of_day`
```c
#include <stdlib.h>
#include "time/time.h"
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

## Example 11 : get current time in seconds and microseconds with `time_current_time_in_seconds` and `time_current_time_in_microsecds`

```c
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    fmt_printf("Current Time in seconds %f\n", time_current_time_in_seconds());
    fmt_printf("Current Time in msec %f\n", time_current_time_in_microseconds());

    return 0;
}
```