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

1. **String Conversion**:
    - **`char* time_to_string(const Time* t)`**: 
      - Converts a `Time` object to a human-readable string in the format `(HH:MM:SS:MS)` (e.g., `(12:34:56:789)`).
      - Returns: A dynamically allocated string representing the time.
      - **Use case**: Useful for displaying the time object in a readable format.
    
2. **Current Time Functions**:
    - **`double time_current_time_in_seconds()`**: 
      - Retrieves the current system time in seconds since the Unix epoch (January 1, 1970).
      - Returns: The current time in seconds as a floating-point number.
    
    - **`double time_current_time_in_microseconds()`**: 
      - Retrieves the current system time in microseconds since the Unix epoch.
      - Returns: The current time in microseconds as a floating-point number.
    
    - **`Time* time_current_time(void)`**: 
      - Returns the current system time as a `Time` object.
      - **Use case**: Can be used to fetch the precise current time, including milliseconds.

3. **Time Creation**:
    - **`Time* time_create(int h, int m, int s, int ms)`**: 
      - Creates a new `Time` object with the specified hours (`h`), minutes (`m`), seconds (`s`), and milliseconds (`ms`).
      - Returns: A pointer to a dynamically allocated `Time` object.

    - **`Time* time_from_msecs_since_start_of_day(int msecs)`**: 
      - Creates a `Time` object from the given milliseconds since the start of the day (00:00:00.000).
      - **Use case**: Useful for generating a time object relative to the start of a day.

4. **Time Manipulation**:
    - **`void time_add_msecs(Time *t, int ms)`**: 
      - Adds the specified number of milliseconds to the `Time` object.
      - **Use case**: Can be used to increment the time object, handling overflow for seconds, minutes, and hours.

    - **`void time_add_secs(Time* t, int s)`**: 
      - Adds the specified number of seconds to the `Time` object.
    
    - **`void time_sleep(unsigned int second)`**: 
      - Puts the program to sleep (halts execution) for the specified number of seconds.

5. **Time Information Extraction**:
    - **`int time_hour(const Time* t)`**: 
      - Returns the hour component of the `Time` object.
    
    - **`int time_minute(const Time* t)`**: 
      - Returns the minute component of the `Time` object.
    
    - **`int time_second(const Time* t)`**: 
      - Returns the second component of the `Time` object.
    
    - **`int time_msec(const Time* t)`**: 
      - Returns the millisecond component of the `Time` object.

6. **Time Calculation**:
    - **`double time_diff_in_seconds(const Time* from, const Time* to)`**: 
      - Calculates the difference in seconds between two `Time` objects (`from` and `to`).
      - Returns: The time difference as a floating-point number in seconds.
    
    - **`int time_msecs_since_start_of_day()`**: 
      - Returns the total number of milliseconds that have passed since the start of the current day (00:00:00.000).
    
    - **`int time_msecs_to(Time* from, Time* to)`**: 
      - Returns the number of milliseconds between two `Time` objects (`from` and `to`).
    
    - **`int time_secs_to(Time* from, Time* to)`**: 
      - Returns the number of seconds between two `Time` objects.

7. **Time Validation**:
    - **`bool time_is_valid_time(int h, int m, int s, int ms)`**: 
      - Checks whether the provided hours, minutes, seconds, and milliseconds constitute a valid time.
      - **Returns**: `true` if the time is valid, `false` otherwise.
    
    - **`bool time_is_null(const Time* t)`**: 
      - Checks if the `Time` object is `null`.
    
    - **`bool time_is_valid(const Time* t)`**: 
      - Validates whether a `Time` object represents a valid time.

8. **Relational Functions**:
    - **`bool time_is_equal(const Time* lhs, const Time* rhs)`**: 
      - Compares two `Time` objects for equality.
      - **Returns**: `true` if both times are equal.
    
    - **`bool time_is_less_than(const Time* lhs, const Time* rhs)`**: 
      - Checks if the `lhs` time is less than the `rhs` time.
    
    - **`bool time_is_less_than_or_equal(const Time* lhs, const Time* rhs)`**: 
      - Checks if the `lhs` time is less than or equal to the `rhs` time.
    
    - **`bool time_is_greater_than(const Time* lhs, const Time* rhs)`**: 
      - Checks if the `lhs` time is greater than the `rhs` time.
    
    - **`bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs)`**: 
      - Checks if the `lhs` time is greater than or equal to the `rhs` time.
    
    - **`bool time_is_not_equal(const Time* lhs, const Time* rhs)`**: 
      - Checks if two `Time` objects are not equal.

9. **Time Modification**:
    - **`bool time_set_hms(Time *t, int h, int m, int s, int ms)`**: 
      - Sets the hour, minute, second, and millisecond values of a `Time` object.

10. **Memory Management**:
    - **`void time_deallocate(Time* t)`**: 
      - Frees the memory allocated for the `Time` object.
  
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