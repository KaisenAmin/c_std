# Date Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview

The Date library in C offers a robust solution for handling and manipulating dates in both Gregorian and Persian calendars. This library is particularly useful for applications requiring operations on dates, such as calculations, conversions, and comparisons. Its comprehensive set of functions simplifies complex date-related tasks in C programming.

## Key Features

- **Dual Calendar Support**: Seamlessly work with both Gregorian and Persian calendars.
- **Date Creation and Manipulation**: Functions to create, add to, or subtract from date objects.
- **Date Comparison**: Compare dates for equality, less than, and greater than conditions.
- **Leap Year Calculation**: Functions to calculate leap years in both calendars.
- **Date Conversion**: Convert dates between Gregorian and Persian calendars.
- **String Conversion**: Convert dates to and from string representations.
- **Utility Functions**: Additional functions for day of the week, day of the year, and more.

## Installation and Compilation

To use the library, include `date.h` in your C project. Compile the source files with a C compiler such as GCC:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c date.c
```

## Function Descriptions

### `Date* date_create(CalendarType type)`
**Purpose**: Allocates a Date object with all fields set to -1 (representing an uninitialized "null" date) and the given calendar type.
**Parameters**:
- `type`: The calendar type (Gregorian or Persian) for the new Date.
**Return Value**: Pointer to the newly allocated Date, or NULL on allocation failure.
**Usage Case**: Use when you need a blank date to fill in later with `date_set_date`.

---

### `Date* date_create_ymd(int y, int m, int d, CalendarType type)`
**Purpose**: Allocates and initializes a Date from explicit year/month/day components, validating them against the calendar rules.
**Parameters**:
- `y`: The year component.
- `m`: The month component.
- `d`: The day component.
- `type`: The calendar type (Gregorian or Persian).
**Return Value**: Pointer to the newly allocated Date, or NULL if the date is invalid or allocation fails.
**Usage Case**: Use for the most common case of creating a known date.

---

### `Date* date_clone(const Date* date)`
**Purpose**: Creates an independent copy of `date` with all fields duplicated.
**Parameters**:
- `date`: Pointer to the Date object to copy.
**Return Value**: Pointer to the new copy, or NULL if `date` is NULL or allocation fails.
**Usage Case**: Use when you need to preserve the original while modifying a copy.

---

### `Date* date_current_date(CalendarType type)`
**Purpose**: Returns a newly allocated Date representing today in either the Gregorian or Persian calendar.
**Parameters**:
- `type`: The calendar type (Gregorian or Persian).
**Return Value**: Pointer to a new Date for today, or NULL on allocation failure or unsupported type.
**Usage Case**: Use to timestamp records with today’s date.

---

### `Date* date_from_string(const char* string, const char* format, CalendarType type)`
**Purpose**: Parses a date string according to `format` for the Gregorian calendar (via `strptime`) or in `"YYYY-MM-DD"` format for the Persian calendar.
**Parameters**:
- `string`: The date string to parse (e.g. `"2024-04-15"`).
- `format`: The format string (e.g. `"%Y-%m-%d"`); ignored for Persian dates.
- `type`: The calendar type (Gregorian or Persian).
**Return Value**: Pointer to a new Date, or NULL on parse failure.
**Usage Case**: Use to deserialize dates from text files or user input.

---

### `Date* date_from_julian_day(long jd)`
**Purpose**: Converts a Julian Day Number to a Gregorian Date.
**Parameters**:
- `jd`: The Julian Day Number to convert.
**Return Value**: Pointer to a new Gregorian Date, or NULL on allocation failure.
**Usage Case**: Use to convert Julian Day values back to calendar dates.

---

### `void date_deallocate(Date* date)`
**Purpose**: Frees the memory allocated for a Date object.
**Parameters**:
- `date`: Pointer to the Date object to free. Safe to call with NULL.
**Return Value**: None.
**Usage Case**: Use at the end of a Date’s lifetime to avoid memory leaks.

---

### `bool date_set_date(Date* date, int year, int month, int day, CalendarType type)`
**Purpose**: Updates an existing Date object in-place after validating the year/month/day against the calendar rules.
**Parameters**:
- `date`: Pointer to the Date object to update.
- `year`: The new year value.
- `month`: The new month value (1–12).
- `day`: The new day value.
- `type`: The calendar type (Gregorian or Persian).
**Return Value**: `true` on success, `false` if any argument is invalid.
**Usage Case**: Use to reuse an existing Date object with new values.

---

### `void date_get_date(const Date* date, int* year, int* month, int* day)`
**Purpose**: Writes the year, month, and day of `date` into the three output pointers.
**Parameters**:
- `date`: Pointer to the Date object.
- `year`: Output pointer for the year.
- `month`: Output pointer for the month.
- `day`: Output pointer for the day.
**Return Value**: None. Does nothing if `date` or any output pointer is NULL.
**Usage Case**: Use to extract all three components in a single call.

---

### `int date_year(const Date* date)`
**Purpose**: Returns the year field of `date`.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: The year, or -1 if `date` is NULL or invalid.
**Usage Case**: Use for quick read-only access to the year component.

---

### `int date_month(const Date* date)`
**Purpose**: Returns the month field (1–12) of `date`.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: The month, or -1 if `date` is NULL or invalid.
**Usage Case**: Use for quick read-only access to the month component.

---

### `int date_day(const Date* date)`
**Purpose**: Returns the day-of-month field of `date`.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: The day, or -1 if `date` is NULL or invalid.
**Usage Case**: Use for quick read-only access to the day component.

---

### `bool date_is_null(const Date* date)`
**Purpose**: Returns true if `date` is a NULL pointer, false otherwise.
**Parameters**:
- `date`: Pointer to the Date object to check.
**Return Value**: `true` if `date` is NULL, `false` otherwise.
**Usage Case**: Use as a null-safety guard before any other date operation.

---

### `bool date_is_valid(const Date* date)`
**Purpose**: Returns true if the year, month, and day fields represent a real date in the declared calendar.
**Parameters**:
- `date`: Pointer to the Date object to validate.
**Return Value**: `true` if valid, `false` for NULL or out-of-range fields.
**Usage Case**: Use before arithmetic or comparison operations to avoid undefined behaviour.

---

### `bool date_is_leap_year(const Date* date)`
**Purpose**: Returns true if the year of `date` is a leap year in its calendar (Gregorian or Persian).
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: `true` if the year is a leap year, `false` if `date` is NULL.
**Usage Case**: Use to determine February/Esfand day counts.

---

### `bool date_is_leap_year_y(int year, CalendarType type)`
**Purpose**: Returns true if `year` is a leap year in the specified calendar, without needing a Date object.
**Parameters**:
- `year`: The year to check.
- `type`: The calendar type (Gregorian or Persian).
**Return Value**: `true` if the year is a leap year, `false` otherwise.
**Usage Case**: Use when you only have a year number, not a full Date.

---

### `bool date_is_weekend(const Date* date)`
**Purpose**: Returns true if `date` falls on a weekend: Saturday or Sunday for Gregorian, Friday for Persian.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: `true` if weekend, `false` for NULL or invalid dates.
**Usage Case**: Use to skip non-working days in scheduling logic.

---

### `int date_compare(const Date* lhs, const Date* rhs)`
**Purpose**: Three-way comparison returning a negative value if `lhs < rhs`, 0 if equal, positive if `lhs > rhs`.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: Negative, zero, or positive integer; `INT_MIN` on error or calendar mismatch.
**Usage Case**: Use for sorting or binary search over date arrays.

---

### `bool date_is_equal(const Date* lhs, const Date* rhs)`
**Purpose**: Returns true if both dates have identical year, month, day, and calendar type.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: `true` if both dates are identical, `false` otherwise.
**Usage Case**: Use to test exact date equality.

---

### `bool date_is_not_equals(const Date* lhs, const Date* rhs)`
**Purpose**: Returns true if the two dates differ in any field.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: `true` if the dates differ, `false` if they are equal.
**Usage Case**: Use when inequality is the primary condition.

---

### `bool date_is_less_than(const Date* lhs, const Date* rhs)`
**Purpose**: Returns true if `lhs` is chronologically earlier than `rhs`.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: `true` if `lhs` is earlier, `false` otherwise.
**Usage Case**: Use to sort dates or enforce ordering constraints.

---

### `bool date_is_less_than_or_equal(const Date* lhs, const Date* rhs)`
**Purpose**: Returns true if `lhs` is earlier than or the same as `rhs`.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: `true` if `lhs <= rhs`, `false` otherwise.
**Usage Case**: Use in range checks where the boundary date is inclusive.

---

### `bool date_is_greater_than(const Date* lhs, const Date* rhs)`
**Purpose**: Returns true if `lhs` is chronologically later than `rhs`.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: `true` if `lhs` is later, `false` otherwise.
**Usage Case**: Use to check whether a deadline has passed.

---

### `bool date_is_greater_than_or_equal(const Date* lhs, const Date* rhs)`
**Purpose**: Returns true if `lhs` is later than or the same as `rhs`.
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: `true` if `lhs >= rhs`, `false` otherwise.
**Usage Case**: Use in range checks where the upper boundary is inclusive.

---

### `Date* date_add_days(const Date* date, int ndays)`
**Purpose**: Returns a new Date that is `ndays` days after (or before, if negative) `date`.
**Parameters**:
- `date`: Pointer to the original Date object.
- `ndays`: Number of days to add (negative to subtract).
**Return Value**: Pointer to a new Date, or NULL if `date` is NULL or invalid. Caller must `date_deallocate` the result.
**Usage Case**: Use for day-offset calculations.

---

### `Date* date_add_months(const Date* date, int nmonths)`
**Purpose**: Returns a new Date that is `nmonths` months later, clamping the day to the last day of the target month if necessary.
**Parameters**:
- `date`: Pointer to the original Date object.
- `nmonths`: Number of months to add.
**Return Value**: Pointer to a new Date, or NULL on error. Caller must `date_deallocate` the result.
**Usage Case**: Use for monthly recurrence logic.

---

### `Date* date_add_years(const Date* date, int nyears)`
**Purpose**: Returns a new Date that is `nyears` years later (or earlier if negative), adjusting for leap-year edge cases such as Feb 29.
**Parameters**:
- `date`: Pointer to the original Date object.
- `nyears`: Number of years to add (negative to subtract).
**Return Value**: Pointer to a new Date, or NULL on error. Caller must `date_deallocate` the result.
**Usage Case**: Use for annual anniversaries.

---

### `Date* date_min(const Date* lhs, const Date* rhs)`
**Purpose**: Returns a pointer to the earlier of the two dates (not a copy — returns one of the input pointers).
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: Pointer to the earlier Date, or NULL if either input is NULL or they are from different calendars.
**Usage Case**: Use to find the earliest of two dates.

---

### `Date* date_max(const Date* lhs, const Date* rhs)`
**Purpose**: Returns a pointer to the later of the two dates (not a copy).
**Parameters**:
- `lhs`: Pointer to the first Date object.
- `rhs`: Pointer to the second Date object.
**Return Value**: Pointer to the later Date, or NULL on error.
**Usage Case**: Use to find the latest of two dates.

---

### `Date* date_first_day_of_month(const Date* date)`
**Purpose**: Returns a new Date for the first day of `date`’s month and year.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Pointer to a new Date for day 1 of the same month/year. Caller must `date_deallocate` the result.
**Usage Case**: Use to anchor a month-start boundary.

---

### `Date* date_last_day_of_month(const Date* date)`
**Purpose**: Returns a new Date for the last day of `date`’s month and year.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Pointer to a new Date for the last day of the same month/year. Caller must `date_deallocate` the result.
**Usage Case**: Use to anchor a month-end boundary.

---

### `int date_days_to(const Date* from, const Date* to)`
**Purpose**: Returns the signed number of days from `from` to `to` (positive if `to` is later).
**Parameters**:
- `from`: Pointer to the start Date object.
- `to`: Pointer to the end Date object.
**Return Value**: Signed day count, or -1 on error. Both dates must share a calendar.
**Usage Case**: Use to compute durations.

---

### `int date_day_of_week(const Date* date)`
**Purpose**: Returns 1 for Monday through 7 for Sunday.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Weekday number (1–7), or -1 on error or unsupported calendar.
**Usage Case**: Use to determine which weekday a date falls on.

---

### `int date_day_of_year(const Date* date)`
**Purpose**: Returns the day number within the year (1–365 or 1–366).
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Day-of-year (1–366), or -1 on error.
**Usage Case**: Use to compute how far into a year a date is.

---

### `int date_days_in_month(const Date* date)`
**Purpose**: Returns the number of days in `date`’s month (e.g. 28–31 for Gregorian, 29–31 for Persian).
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Days in the month, or -1 on error.
**Usage Case**: Use to validate day fields or build calendar grids.

---

### `int date_days_in_year(const Date* date)`
**Purpose**: Returns 365 or 366 depending on whether `date`’s year is a leap year.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: 365 or 366, or -1 on error.
**Usage Case**: Use to compute day-of-year offsets.

---

### `int date_week_number(const Date* date, int* yearNumber)`
**Purpose**: Returns the ISO 8601 week number (1–53). If `yearNumber` is non-NULL, it is updated only when the week belongs to an adjacent year.
**Parameters**:
- `date`: Pointer to the Date object.
- `yearNumber`: Output pointer updated for edge-case weeks that belong to an adjacent year; may be NULL.
**Return Value**: ISO week number (1–53), or -1 on error.
**Usage Case**: Use for week-based scheduling or reporting.

---

### `int date_quarter(const Date* date)`
**Purpose**: Returns the quarter of the year (1–4) based on the month.
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Quarter number (1–4), or -1 if `date` is NULL or invalid.
**Usage Case**: Use to group dates by financial or reporting quarter.

---

### `const char* date_weekday_name(const Date* date)`
**Purpose**: Returns a static English string for the weekday (e.g. `"Monday"`, `"Tuesday"`, ...).
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Static weekday name string, or NULL on error. Do not free the returned string.
**Usage Case**: Use for display purposes.

---

### `const char* date_month_name(const Date* date)`
**Purpose**: Returns a static string for the month name in the date’s calendar (Gregorian: `"January"` ... `"December"`; Persian: `"Farvardin"` ... `"Esfand"`).
**Parameters**:
- `date`: Pointer to the Date object.
**Return Value**: Static month name string, or NULL on error. Do not free the returned string.
**Usage Case**: Use for display purposes.

---

### `Date* date_gregorian_to_solar(const Date* gregorian_date)`
**Purpose**: Converts a Gregorian Date to the equivalent Persian (Jalali) Date.
**Parameters**:
- `gregorian_date`: Pointer to the Gregorian Date object to convert.
**Return Value**: Pointer to a new Persian Date. Caller must `date_deallocate` the result. Returns NULL on error.
**Usage Case**: Use to translate between calendar systems.

---

### `Date* date_solar_to_gregorian(const Date* solar_date)`
**Purpose**: Converts a Persian Date to the equivalent Gregorian Date.
**Parameters**:
- `solar_date`: Pointer to the Persian Date object to convert.
**Return Value**: Pointer to a new Gregorian Date. Caller must `date_deallocate` the result. Returns NULL on error.
**Usage Case**: Use to translate a Persian date back to the Gregorian calendar.

---

### `long date_to_julian_day(const Date* date)`
**Purpose**: Returns the Julian Day Number for a Gregorian Date.
**Parameters**:
- `date`: Pointer to the Gregorian Date object.
**Return Value**: The Julian Day Number as a `long`, or -1 on error.
**Usage Case**: Use to convert dates to a common continuous count for arithmetic across month/year boundaries.

---

### `int64_t date_to_unix(const Date* date)`
**Purpose**: Converts a Date to **Unix time** — seconds since 1970-01-01T00:00:00 UTC. A Date has no time-of-day, so the result is the epoch second at UTC midnight of that calendar day. Works for both Gregorian and Persian dates (Persian routes through the shared Julian-Day axis) and is negative for days before 1970. The returned value is always a multiple of 86400.
**Parameters**:
- `date`: Pointer to the Date object (Gregorian or Persian).
**Return Value**: Seconds since the Unix epoch at UTC midnight, or `INT64_MIN` if `date` is `NULL` or invalid.
**Usage Case**: Store/transmit dates as epoch integers (databases, logs, JSON APIs), or compare a Date against a Unix timestamp without string parsing.

---

### `Date* date_from_unix(int64_t unix_seconds, CalendarType type)`
**Purpose**: Builds a Date from **Unix time**, keeping the calendar day at UTC. The inverse of `date_to_unix`: the time-of-day part of `unix_seconds` is floored to the start of the UTC day, and the resulting day is returned in the requested calendar. Negative inputs (pre-1970) use floor division so the day boundary is correct.
**Parameters**:
- `unix_seconds`: Seconds since the Unix epoch.
- `type`: Desired calendar of the result (`Gregorian` or `Persian`).
**Return Value**: A heap-allocated `Date` the caller must free with `date_deallocate`, or `NULL` on an out-of-range/unsupported input or allocation failure.
**Usage Case**: Turn a stored epoch timestamp back into a calendar date for display, day-based grouping, or per-day bucketing.

---

### `char* date_to_string(const Date* date, const char* format)`
**Purpose**: Returns a heap-allocated string with the date formatted according to `format`. For Gregorian dates, `format` follows `strftime` conventions; for Persian dates, the output is always `"YYYY-MM-DD"`.
**Parameters**:
- `date`: Pointer to the Date object to format.
- `format`: Format string (follows `strftime` for Gregorian; ignored for Persian).
**Return Value**: Heap-allocated formatted string. Caller must `free` the result. Returns NULL on error.
**Usage Case**: Use to serialize a date to a display or storage string.

---

## Example 1: Current Date in Gregorian Calendar
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* current_gregorian = date_current_date(Gregorian);
    if (current_gregorian) {
        fmt_printf("Current Gregorian Date: %d-%d-%d\n", current_gregorian->year, current_gregorian->month, current_gregorian->day);
        date_deallocate(current_gregorian);
    }
    else {
      fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }
    
    return 0;
}
```

**Result**
```
Current Gregorian Date: 2026-5-28
```
> Note: Output reflects today's date on the current machine.

---

## Example 2: Current Date in Persian Calendar
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* current_persian = date_current_date(Persian);
    if (current_persian) {
        fmt_printf("Current Persian Date: %d-%d-%d\n", current_persian->year, current_persian->month, current_persian->day);
        date_deallocate(current_persian);
    }
    else {
      fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }

    return 0;
}
```

**Result**
```
Current Persian Date: 1405-3-7
```
> Note: Output reflects today's date converted to the Persian calendar.

---

## Example 3: Add 10 Days to a Specific Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* specific_date = date_create_ymd(2024, 1, 15, Gregorian);
    Date* new_date = date_add_days(specific_date, 10);
    
    if (new_date && specific_date) {
        fmt_printf("New Gregorian Date: %d-%d-%d\n", new_date->year, new_date->month, new_date->day);
        date_deallocate(specific_date);   
        date_deallocate(new_date);
    }
    else {
        fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }

    return 0;
}
```

**Result**
```
New Gregorian Date: 2024-1-25
```

---

## Example 4: Add 5 Months to a Specific Persian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* persian_date = date_create_ymd(1402, 7, 1, Persian);
    Date* new_persian_date = date_add_months(persian_date, 5);

    if (new_persian_date && persian_date) {
        fmt_printf("New Persian Date: %d-%d-%d\n", new_persian_date->year, new_persian_date->month, new_persian_date->day);
        date_deallocate(persian_date);    
        date_deallocate(new_persian_date);
    }
    else {
        fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }

    return 0;
}
```

**Result**
```
New Persian Date: 1402-12-1
```

---

## Example 5: Subtract 2 Years from a Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* date_to_subtract = date_create_ymd(2024, 5, 20, Gregorian);
    Date* subtracted_date = date_add_years(date_to_subtract, -2);
    
    if (subtracted_date && date_to_subtract) {
        fmt_printf("Subtracted Gregorian Date: %d-%d-%d\n", subtracted_date->year, subtracted_date->month, subtracted_date->day);
        date_deallocate(date_to_subtract);    
        date_deallocate(subtracted_date);
    }
    else {
        fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }

    
    return 0;
}
```

**Result**
```
Subtracted Gregorian Date: 2022-5-20
```

---

## Example 6: Convert Gregorian to Persian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* gregorian_for_conversion = date_create_ymd(2024, 3, 21, Gregorian);
    Date* converted_persian = date_gregorian_to_solar(gregorian_for_conversion);

    if (converted_persian && gregorian_for_conversion) {
        fmt_printf("Converted to Persian: %d-%d-%d\n", converted_persian->year, converted_persian->month, converted_persian->day);
        date_deallocate(gregorian_for_conversion);
        date_deallocate(converted_persian);
    }
    else {
        fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }
    
    return 0;
}
```

**Result**
```
Converted to Persian: 1403-1-2
```

---

## Example 7: Convert Persian to Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    /* 1402 is NOT a leap year, so month 12 has only 29 days.
       date_create_ymd returns NULL for invalid dates.
       (1403 IS a leap year and would accept 1403-12-30.) */
    Date* persian_for_conversion = date_create_ymd(1402, 12, 30, Persian);

    if (!persian_for_conversion) {
        fmt_fprintf(stderr, "Invalid date: 1402-12-30 does not exist (1402 is not a leap year).\n");
        return 1;
    }

    Date* converted_gregorian = date_solar_to_gregorian(persian_for_conversion);
    if (converted_gregorian) {
        fmt_printf("Converted to Gregorian: %d-%d-%d\n",
                   converted_gregorian->year, converted_gregorian->month, converted_gregorian->day);
        date_deallocate(converted_gregorian);
    }
    date_deallocate(persian_for_conversion);
    return 0;
}
```

**Result**
```
Invalid date: 1402-12-30 does not exist (1402 is not a leap year).
```

---

## Example 8: Create a Persian Date String
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* persian_date_string = date_create_ymd(1402, 5, 14, Persian);
    const char* format = "%Y-%m-%d";
    char* date_str = date_to_string(persian_date_string, format);

    if (date_str) {
        fmt_printf("Persian Date String: %s\n", date_str);
        free(date_str);
    }

    date_deallocate(persian_date_string);
    return 0;
}
```

**Result**
```
Persian Date String: 1402-05-14
```

---

## Example 9: Check Leap Year in Gregorian Calendar
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* leap_year_gregorian = date_create_ymd(2024, 1, 1, Gregorian);

    if (date_is_leap_year(leap_year_gregorian)) {
        fmt_printf("2024 is a leap year in Gregorian calendar.\n");
    } 
    else {
        fmt_printf("2024 is not a leap year in Gregorian calendar.\n");
    }

    date_deallocate(leap_year_gregorian);
    return 0;
}
```

**Result**
```
2024 is a leap year in Gregorian calendar.
```

---

## Example 10: Check Leap Year in Persian Calendar
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* leap_year_persian = date_create_ymd(1403, 1, 1, Persian);

    if (date_is_leap_year(leap_year_persian)) {
        fmt_printf("1403 is a leap year in Persian calendar.\n");
    } 
    else {
        fmt_printf("1403 is not a leap year in Persian calendar.\n");
    }

    date_deallocate(leap_year_persian);
    return 0;
}
```

**Result**
```
1403 is a leap year in Persian calendar.
```

---

## Example 11: Parse Gregorian Date from String
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    const char* gregorian_string = "2024-04-15";
    Date* parsed_gregorian_date = date_from_string(gregorian_string, "%Y-%m-%d", Gregorian);

    if (parsed_gregorian_date) {
        fmt_printf("Parsed Gregorian Date: %d-%d-%d\n", parsed_gregorian_date->year, parsed_gregorian_date->month, parsed_gregorian_date->day);
        date_deallocate(parsed_gregorian_date);
    }
    return 0;
}
```

**Result**
```
Parsed Gregorian Date: 2024-4-15
```

---

### Example 12: Parse Persian Date from String
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    const char* persian_string = "1402-07-22";
    Date* parsed_persian_date = date_from_string(persian_string, "%Y-%m-%d", Persian);

    if (parsed_persian_date) {
        fmt_printf("Parsed Persian Date: %d-%d-%d\n", parsed_persian_date->year, parsed_persian_date->month, parsed_persian_date->day);
        date_deallocate(parsed_persian_date);
    }

    return 0;
}
```

**Result**
```
Parsed Persian Date: 1402-7-22
```

---

## Example 13: Check Date Equality
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* date1 = date_create_ymd(2024, 5, 10, Gregorian);
    Date* date2 = date_create_ymd(2024, 5, 10, Gregorian);

    if (date_is_equal(date1, date2)) {
        fmt_printf("The dates are equal.\n");
    } 
    else {
        fmt_printf("The dates are not equal.\n");
    }

    date_deallocate(date1);
    date_deallocate(date2);
    return 0;
}
```

**Result**
```
The dates are equal.
```

---

### Example 14: Check Date Less Than
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* early_date = date_create_ymd(2024, 1, 15, Gregorian);
    Date* later_date = date_create_ymd(2024, 5, 20, Gregorian);

    if (date_is_less_than(early_date, later_date)) {
        fmt_printf("Early date is less than later date.\n");
    } 
    else {
        fmt_printf("Early date is not less than later date.\n");
    }

    date_deallocate(early_date);
    date_deallocate(later_date);
    return 0;
}
```

**Result**
```
Early date is less than later date.
```

---

## Example 15: Check Date Greater Than
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* date3 = date_create_ymd(2024, 6, 15, Gregorian);
    Date* date4 = date_create_ymd(2024, 3, 20, Gregorian);

    if (date_is_greater_than(date3, date4)) {
        fmt_printf("Date3 is greater than date4.\n");
    } 
    else {
        fmt_printf("Date3 is not greater than date4.\n");
    }

    date_deallocate(date3);
    date_deallocate(date4);
    
    return 0;
}
```

**Result**
```
Date3 is greater than date4.
```

---

## Example 16: Get Day of the Week for a Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* weekday_date = date_create_ymd(2024, 2, 29, Gregorian);
    int day_of_week = date_day_of_week(weekday_date);

    fmt_printf("Day of the week for 2024-02-29: %d (1=Monday, 7=Sunday)\n", day_of_week);

    date_deallocate(weekday_date);
    return 0;
}
```

**Result**
```
Day of the week for 2024-02-29: 4 (1=Monday, 7=Sunday)
```

---

### Example 17: Get Day of the Year for a Persian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* day_of_year_date = date_create_ymd(1402, 12, 1, Persian);
    int day_of_year = date_day_of_year(day_of_year_date);

    fmt_printf("Day of the year for 1402-12-01: %d\n", day_of_year);
    
    date_deallocate(day_of_year_date);
    return 0;
}
```

**Result**
```
Day of the year for 1402-12-01: 337
```

---

## Example 18: Get Week Number for a Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    int week_number_year = 0;
    Date* week_number_date = date_create_ymd(2024, 4, 15, Gregorian);
    int week_number = date_week_number(week_number_date, &week_number_year);

    fmt_printf("Week number for 2024-04-15: %d, Year: %d\n", week_number, week_number_year);
    
    date_deallocate(week_number_date);
    return 0;
}
```

**Result**
```
Week number for 2024-04-15: 16, Year: 0
```
> Note: `Year` is 0 because `date_week_number` only updates `yearNumber` for edge-case weeks that belong to an adjacent year; for normal mid-year weeks the caller's initial value is preserved.

---

## Example 19: Check if a Date is Valid
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    // Date is invalid because February has at most 29 days (in a leap year).
    Date* invalid_date = date_create_ymd(2024, 2, 30, Gregorian);
    
    if (date_is_valid(invalid_date)) {
        fmt_printf("The date is valid.\n");
    } 
    else {
        fmt_printf("The date is invalid.\n");
    }

    date_deallocate(invalid_date);
    return 0;
}
```

**Result**
```
The date is invalid.
```

---

## Example 20: Convert Julian Day Number to Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    long julianDayNumber = 2459580; // Example Julian Day Number
    Date* gregorianDate = date_from_julian_day(julianDayNumber);

    if (gregorianDate != NULL) {
        fmt_printf("Gregorian Date: %d-%02d-%02d\n", 
               gregorianDate->year, 
               gregorianDate->month, 
               gregorianDate->day);

        date_deallocate(gregorianDate);
    } 
    else {
        fmt_printf("Conversion failed.\n");
    }
    return 0;
}
```

**Result**
```
Gregorian Date: 2021-12-31
```

---

## Example 21: Unix-time conversion with `date_to_unix` and `date_from_unix`
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* Calendar date -> Unix timestamp (UTC midnight). */
    Date* d = date_create_ymd(2024, 6, 15, Gregorian);
    int64_t unix_seconds = date_to_unix(d);
    fmt_printf("2024-06-15 -> unix %lld\n", (long long)unix_seconds);

    /* Round-trip: Unix timestamp -> calendar date. */
    Date* back = date_from_unix(unix_seconds, Gregorian);
    fmt_printf("round-trip: %d-%02d-%02d\n", back->year, back->month, back->day);

    /* Time-of-day is floored to the start of the UTC day. */
    Date* day = date_from_unix(unix_seconds + 86399, Gregorian);   /* 23:59:59 same day */
    fmt_printf("end-of-day floors to: %d-%02d-%02d\n", day->year, day->month, day->day);

    date_deallocate(d);
    date_deallocate(back);
    date_deallocate(day);
    return 0;
}
```

**Result**
```
2024-06-15 -> unix 1718409600
round-trip: 2024-06-15
end-of-day floors to: 2024-06-15
```
> **Why it matters:** databases, logs, and JSON APIs speak Unix time, not calendar structs. `date_to_unix` / `date_from_unix` bridge the two exactly (built on the library's Julian-Day axis, so Persian dates work too), and the result is always pinned to UTC midnight so day-based comparisons stay stable across time zones.

---

## License

This project is open-source and available under [ISC License].