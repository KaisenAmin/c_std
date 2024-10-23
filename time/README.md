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


### `Time* time_create(int h, int m, int s, int ms)`  
- **Purpose**: Initializes a `Time` object with the provided hour, minute, second, and millisecond values.  
- **Parameters**:  
  - `h`: The hour (0-23).  
  - `m`: The minute (0-59).  
  - `s`: The second (0-59).  
  - `ms`: The millisecond (0-999).  
- **Returns**:  
  - A pointer to the newly created `Time` object.  
- **Use case**: Used to create and initialize a `Time` object with specific values.

---

### `Time* time_current_time(void)`  
- **Purpose**: Returns the current system time as a `Time` object.  
- **Parameters**:  
  - None.  
- **Returns**:  
  - A pointer to a `Time` object representing the current system time.  
- **Use case**: Used to get the system’s current local time in a `Time` object.

---

### `bool time_is_valid(const Time* t)`  
- **Purpose**: Checks if the provided `Time` object is valid, ensuring that the hour, minute, second, and millisecond values fall within acceptable ranges.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object to validate.  
- **Returns**:  
  - `true` if the `Time` object is valid, `false` otherwise.  
- **Use case**: Used to ensure the time values are within valid ranges before performing operations.

---

### `void time_add_msecs(Time *t, int ms)`  
- **Purpose**: Adds the specified number of milliseconds to the provided `Time` object, handling the overflow of milliseconds into seconds, seconds into minutes, and minutes into hours.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
  - `ms`: The number of milliseconds to add.  
- **Returns**:  
  - Nothing (void).  
- **Use case**: Used to increment the time by a specified number of milliseconds, adjusting for overflow.

---

### `void time_add_secs(Time* t, int s)`  
- **Purpose**: Adds the specified number of seconds to the provided `Time` object, handling the overflow of seconds into minutes and minutes into hours.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
  - `s`: The number of seconds to add.  
- **Returns**:  
  - Nothing (void).  
- **Use case**: Used to increment the time by a specified number of seconds, adjusting for overflow.

---

### `bool time_is_null(const Time* t)`  
- **Purpose**: Checks whether the provided `Time` object is `NULL`.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
- **Returns**:  
  - `true` if the `Time` object is `NULL`, `false` otherwise.  
- **Use case**: Used to verify if a `Time` object is uninitialized or `NULL`.

---

### `int time_hour(const Time* t)`  
- **Purpose**: Returns the hour component of the provided `Time` object.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
- **Returns**:  
  - The hour component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.  
- **Use case**: Used to retrieve the hour from a `Time` object.

---

### `int time_minute(const Time* t)`  
- **Purpose**: Returns the minute component of the provided `Time` object.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
- **Returns**:  
  - The minute component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.  
- **Use case**: Used to retrieve the minute from a `Time` object.

---

### `int time_second(const Time* t)`  
- **Purpose**: Returns the second component of the provided `Time` object.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
- **Returns**:  
  - The second component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.  
- **Use case**: Used to retrieve the second from a `Time` object.

---

### `int time_msec(const Time* t)`  
- **Purpose**: Returns the millisecond component of the provided `Time` object.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object.  
- **Returns**:  
  - The millisecond component of the `Time` object, or `-1` if the `Time` object is `NULL` or invalid.  
- **Use case**: Used to retrieve the millisecond value from a `Time` object.

---

### `int time_msecs_since_start_of_day()`  
- **Purpose**: Returns the number of milliseconds that have elapsed since the start of the day (00:00:00.000).  
- **Parameters**:  
  - None.  
- **Returns**:  
  - The number of milliseconds since the start of the day, or `-1` if the current time could not be retrieved.  
- **Use case**: Used to calculate how many milliseconds have passed since midnight.

---

### `int time_msecs_to(Time* from, Time* to)`  
- **Purpose**: Computes the difference in milliseconds between two `Time` objects (`from` and `to`). The result is adjusted to ensure it is within the range of -86400000 to 86400000 milliseconds.  
- **Parameters**:  
  - `from`: A pointer to the starting `Time` object.  
  - `to`: A pointer to the ending `Time` object.  
- **Returns**:  
  - The difference in milliseconds between the two `Time` objects, or `0` if either object is `NULL` or invalid.  
- **Use case**: Used to calculate the time difference between two `Time` objects in milliseconds.

---

### `int time_secs_to(Time* from, Time* to)`  
- **Purpose**: Computes the difference in seconds between two `Time` objects (`from` and `to`). The result is adjusted to ensure it is within the range of -86400 and 86400 seconds (the number of seconds in a day).  
- **Parameters**:  
  - `from`: A pointer to the starting `Time` object.  
  - `to`: A pointer to the ending `Time` object.  
- **Returns**:  
  - The difference in seconds between the two `Time` objects, or `0` if either `Time` object is `NULL` or invalid.  
- **Use case**: Used to calculate the time difference in seconds between two `Time` objects.

---

### `bool time_set_hms(Time *t, int h, int m, int s, int ms)`  
- **Purpose**: Sets the hour, minute, second, and millisecond components of a `Time` object.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object to be modified.  
  - `h`: The hour (0-23).  
  - `m`: The minute (0-59).  
  - `s`: The second (0-59).  
  - `ms`: The millisecond (0-999).  
- **Returns**:  
  - `true` if the time was successfully set, `false` otherwise.  
- **Use case**: Used to modify the components of a `Time` object.

---

### `char* time_to_string(const Time* t)`  
- **Purpose**: Converts a `Time` object to a human-readable string in the format "(hh:mm:ss:ms)".  
- **Parameters**:  
  - `t`: A pointer to the `Time` object to be converted.  
- **Returns**:  
  - A string representing the `Time` object, or `NULL` if the `Time` object is `NULL` or invalid.  
- **Use case**: Used to create a string representation of a `Time` object. The caller is responsible for freeing the allocated memory.

---

### `bool time_is_valid_time(int h, int m, int s, int ms)`  
- **Purpose**: Checks if the provided hour, minute, second, and millisecond values represent a valid time.  
- **Parameters**:  
  - `h`: The hour (0-23).  
  - `m`: The minute (0-59).  
  - `s`: The second (0-59).  
  - `ms`: The millisecond (0-999).  
- **Returns**:  
  - `true` if the parameters represent a valid time, `false` otherwise.  
- **Use case**: Used to validate individual time components.

---

### `Time* time_from_msecs_since_start_of_day(int msecs)`  
- **Purpose**: Converts a time given in milliseconds since the start of the day into a `Time` object.  
- **Parameters**:  
  - `msecs`: The number of milliseconds since the start of the day (must be between 0 and 86399999).  
- **Returns**:  
  - A pointer to the newly created `Time` object, or `NULL` if the input is invalid or memory allocation fails.  
- **Use case**: Used to create a `Time` object from a given number of milliseconds since the start of the day.

---

### `bool time_is_equal(const Time* lhs, const Time* rhs)`  
- **Purpose**: Checks if two `Time` objects represent the same time.  
- **Parameters**:  
  - `lhs`: A pointer to the first `Time` object.  
  - `rhs`: A pointer to the second `Time` object.  
- **Returns**:  
  - `true` if the two `Time` objects are equal, `false` otherwise.  
- **Use case**: Used to compare two `Time` objects for equality.

---

### `bool time_is_less_than(const Time* lhs, const Time* rhs)`  
- **Purpose**: Checks if the first `Time` object (`lhs`) represents an earlier time than the second `Time` object (`rhs`).  
- **Parameters**:  
  - `lhs`: A pointer to the first `Time` object.  
  - `rhs`: A pointer to the second `Time` object.  
- **Returns**:  
  - `true` if `lhs` is less than `rhs`, `false` otherwise. Returns `false` if either pointer is `NULL` or the `Time` objects are invalid.  
- **Use case**: Used to compare two `Time` objects to see if one represents an earlier time than the other.

---

### `bool time_is_less_than_or_equal(const Time* lhs, const Time* rhs)`  
- **Purpose**: Checks if the first `Time` object (`lhs`) represents an earlier or the same time as the second `Time` object (`rhs`).  
- **Parameters**:  
  - `lhs`: A pointer to the first `Time` object.  
  - `rhs`: A pointer to the second `Time` object.  
- **Returns**:  
  - `true` if `lhs` is less than or equal to `rhs`, `false` otherwise.  
- **Use case**: Used to determine if one `Time` object is earlier than or equal to another.

---

### `bool time_is_greater_than(const Time* lhs, const Time* rhs)`  
- **Purpose**: Checks if the first `Time` object (`lhs`) represents a later time than the second `Time` object (`rhs`).  
- **Parameters**:  
  - `lhs`: A pointer to the first `Time` object.  
  - `rhs`: A pointer to the second `Time` object.  
- **Returns**:  
  - `true` if `lhs` is greater than `rhs`, `false` otherwise. Returns `false` if either pointer is `NULL`.  
- **Use case**: Used to compare two `Time` objects to see if one represents a later time than the other.

---

### `bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs)`  
- **Purpose**: Checks if the first `Time` object (`lhs`) represents the same or a later time than the second `Time` object (`rhs`).  
- **Parameters**:  
  - `lhs`: A pointer to the first `Time` object.  
  - `rhs`: A pointer to the second `Time` object.  
- **Returns**:  
  - `true` if `lhs` is greater than or equal to `rhs`, `false` otherwise.  
- **Use case**: Used to determine if one `Time` object is the same or later than another.

---

### `bool time_is_not_equal(const Time* lhs, const Time* rhs)`  
- **Purpose**: Checks if two `Time` objects do not represent the same time.  
- **Parameters**:  
  - `lhs`: A pointer to the first `Time` object.  
  - `rhs`: A pointer to the second `Time` object.  
- **Returns**:  
  - `true` if `lhs` is not equal to `rhs`, `false` otherwise. Returns `true` if either pointer is `NULL`.  
- **Use case**: Used to determine if two `Time` objects represent different times.

---

### `void time_deallocate(Time* t)`  
- **Purpose**: Frees the memory allocated for a `Time` object.  
- **Parameters**:  
  - `t`: A pointer to the `Time` object to be deallocated.  
- **Returns**:  
  - Nothing (void).  
- **Use case**: Used to free up memory when a `Time` object is no longer needed.

---

### `double time_current_time_in_seconds()`  
- **Purpose**: Returns the current time in seconds, with fractional seconds represented as a double.  
- **Parameters**:  
  - None.  
- **Returns**:  
  - The current time in seconds since the Unix epoch as a double.  
- **Use case**: Used to retrieve the current time in seconds with high precision.

---

### `double time_current_time_in_microseconds()`  
- **Purpose**: Returns the current time in microseconds, with fractional microseconds represented as a double.  
- **Parameters**:  
  - None.  
- **Returns**:  
  - The current time in microseconds since the Unix epoch as a double.  
- **Use case**: Used to retrieve the current time in microseconds with high precision.

---

### `void time_sleep(unsigned int second)`  
- **Purpose**: Pauses the program for a specified number of seconds.  
- **Parameters**:  
  - `second`: The number of seconds to sleep.  
- **Returns**:  
  - Nothing (void).  
- **Use case**: Used to delay execution for a given number of seconds.

---

### `double time_diff_in_seconds(const Time* from, const Time* to)`  
- **Purpose**: Computes the difference in time between two `Time` objects, returning the result in seconds. The result is adjusted to ensure it is within the range of -86400 to 86400 seconds (the number of seconds in a day).  
- **Parameters**:  
  - `from`: A pointer to the starting `Time` object.  
  - `to`: A pointer to the ending `Time` object.  
- **Returns**:  
  - The difference in time between the two `Time` objects in seconds as a double, or `0.0` if either `Time` object is `NULL` or invalid.  
- **Use case**: Used to calculate the time difference between two `Time` objects in seconds.

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
**Result:**
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
**Result:**
```
Add milliseconds: (0:00:01:000)
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
**Result:**
```
Add seconds: (2:02:03:000)
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
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
**Result:**
```
Time from msecs since start of day: (12:30:00:000)
```

---

## Example 11 : get current time in seconds and microseconds with `time_current_time_in_seconds` and `time_current_time_in_microsecds`

```c
#include "fmt/fmt.h"
#include "time/std_time.h"

int main() {
    fmt_printf("Current Time in seconds %f\n", time_current_time_in_seconds());
    fmt_printf("Current Time in msec %f\n", time_current_time_in_microseconds());

    return 0;
}
```
**Result:**
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
**Result:**
```
Difference in Seconds : 10.007000
```

---

## License

This project is open-source and available under [ISC License].