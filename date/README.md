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

### `bool date_is_valid(const Date* date)`

- **Purpose**: Checks whether a `Date` object represents a valid date by validating the year, month, and day fields according to the specified calendar type (Gregorian or Persian).
- **Parameters**:
  - `date`: A pointer to the `Date` object that needs to be validated.
- **Return**: Returns `true` if the `Date` object is valid, otherwise `false`.
- **Details**:
  - If the `Date` pointer is `NULL`, the function logs an error message and returns `false`.
  - The function calls `date_is_valid_ymd` internally, passing the year, month, day, and calendar type to validate the date components.
  - This function ensures that no operations are performed on invalid or uninitialized `Date` objects.

---

### `Date* date_create(CalendarType type) `

- **Purpose**: Creates a new `Date` object and initializes it with invalid values to represent a "null" date (for later initialization).
- **Parameters**:
  - `type`: The `CalendarType` (either Gregorian or Persian) to be used for the new `Date`.
- **Return**: A pointer to the newly created `Date` object.
- **Details**:
  - Allocates memory for the `Date` object.
  - Initializes the `year`, `month`, and `day` fields to `-1`, which signifies that the date is not yet set.
  - Sets the `calendarType` to the provided value (Gregorian or Persian).
  - If memory allocation fails, it logs an error message and exits the program with a failure status.

---

### `Date* date_create_ymd(int y, int m, int d, CalendarType type)`

- **Purpose**: Creates a new `Date` object with the specified year, month, day, and calendar type, and validates the provided date components.
- **Parameters**:
  - `y`: The year component of the date.
  - `m`: The month component of the date.
  - `d`: The day component of the date.
  - `type`: The `CalendarType` (either Gregorian or Persian) to be used for the new `Date`.
- **Return**: A pointer to the newly created `Date` object if the date is valid. If the date is invalid, it logs an error, frees the allocated memory, and returns `NULL`.
- **Details**:
  - Allocates memory for the `Date` object.
  - Validates the `year`, `month`, and `day` components by calling the `date_is_valid_ymd` function.
  - If the provided date components are invalid, it logs an error message, frees the allocated memory, and returns `NULL` without exiting the program.
  - If the date is valid, it initializes the `Date` object with the given values and returns the pointer to the new object.

---

### `bool date_is_null(const Date* date)`

- **Purpose**: Checks whether a given `Date` object is `NULL`.
- **Parameters**:
  - `date`: A pointer to the `Date` object to be checked.
- **Return**: Returns `true` if the `Date` object is `NULL`, otherwise returns `false`.
- **Details**:
  - If the `Date` object is `NULL`, the function logs a warning message and returns `true`.
  - If the `Date` object is valid (not `NULL`), the function returns `false`.

---

### `Date* date_add_days(const Date* orig_date, int ndays)`

- **Purpose**: Adds or subtracts a specified number of days to/from a given date.
- **Parameters**:
  - `orig_date`: A pointer to the original `Date` object to which the days will be added.
  - `ndays`: The number of days to add (can be negative to subtract days).
- **Return**: Returns a pointer to the newly created `Date` object with the adjusted date.
- **Details**:
  - The function creates a new `Date` object and adjusts the day, month, and year fields based on the number of days added or subtracted.
  - It handles month and year overflows/underflows and adjusts accordingly based on the calendar type (Gregorian or Persian).
  - If the `orig_date` is `NULL` or invalid, it logs an error and returns `NULL`.
  - The function uses a loop to handle cases where adding or subtracting days may result in moving between months or years.

---

### `Date* date_add_months(const Date* orig_date, int nmonths)`

- **Purpose**: Adds a specified number of months to a given date.
- **Parameters**:
  - `orig_date`: A pointer to the original `Date` object to which the months will be added.
  - `nmonths`: The number of months to add.
- **Return**: Returns a pointer to the newly created `Date` object with the adjusted date.
- **Details**:
  - The function creates a new `Date` object and adjusts the month and year fields based on the number of months added.
  - It handles month overflows (e.g., going from December to January) and adjusts the year accordingly.
  - If the day in the original date exceeds the number of days in the resulting month, it is adjusted to the last valid day of the month.
  - If the `orig_date` is `NULL` or invalid, the function logs an error and returns `NULL`.
  - The function accounts for the Persian calendar's unique behavior, where some years have 13 months.

---

### `Date* date_add_years(const Date* orig_date, int nyears)`

- **Purpose**: Adds a specified number of years to a given date and adjusts the result to ensure the date remains valid, considering leap years.
- **Parameters**:
  - `orig_date`: A pointer to the original `Date` object.
  - `nyears`: The number of years to add.
- **Return**: A pointer to the newly created `Date` object with the adjusted date.
- **Details**:
  - If the original date is invalid or `NULL`, it logs an error and exits the program.
  - The function creates a new `Date` object and adds the specified number of years.
  - For leap years, the function adjusts the date if it falls on a day that doesn't exist in non-leap years (e.g., February 29 in a Gregorian leap year or the extra day in the Persian calendar).

---

### `void date_get_date(const Date* date, int *year, int *month, int *day)`

- **Purpose**: Retrieves the year, month, and day from a `Date` object.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
  - `year`: A pointer to an integer where the year will be stored.
  - `month`: A pointer to an integer where the month will be stored.
  - `day`: A pointer to an integer where the day will be stored.
- **Return**: None (void).
- **Details**:
  - If the `Date` object or any of the output pointers (`year`, `month`, or `day`) are `NULL`, it logs an error and returns without performing any actions.
  - It assigns the year, month, and day values from the `Date` object to the provided pointers.

---

### `int date_day(const Date* date)`

- **Purpose**: Retrieves the day of the month from a `Date` object.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The day of the month, or `-1` if the date is invalid or `NULL`.
- **Details**:
  - If the `Date` object is `NULL` or invalid, it logs an error and returns `-1`.
  - Otherwise, it returns the day value from the `Date` object.

---

### `int date_month(const Date* date)`

- **Purpose**: Retrieves the month from a `Date` object.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The month value, or `-1` if the date is invalid or `NULL`.
- **Details**:
  - If the `Date` object is `NULL` or invalid, it logs an error and returns `-1`.
  - Otherwise, it returns the month value from the `Date` object.

---

### `int date_year(const Date* date)`

- **Purpose**: Retrieves the year from a `Date` object.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The year value, or `-1` if the date is invalid or `NULL`.
- **Details**:
  - If the `Date` object is `NULL` or invalid, it logs an error and returns `-1`.
  - Otherwise, it returns the year value from the `Date` object.

---

### `int date_day_of_week(const Date* date)`

- **Purpose**: Returns the day of the week for the given `Date` object, with Monday as 1 through Sunday as 7.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The day of the week (1 = Monday, ..., 7 = Sunday), or `-1` if there is an error (e.g., a `NULL` date or unsupported calendar type).
- **Details**:
  - The function converts the given date to a Julian Day Number (JDN) using either the Gregorian or Persian calendar conversion functions.
  - The JDN is then used to calculate the day of the week.

---

### `int date_day_of_year(const Date* date)`

- **Purpose**: Calculates the day of the year for the given `Date` object (from 1 to 365 or 366 in a leap year).
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The day of the year (1 to 365/366), or `-1` if there is an error (e.g., a `NULL` date or unsupported calendar type).
- **Details**:
  - For the Gregorian calendar, it uses a static array to determine the cumulative days before the month, adjusting for leap years if necessary.
  - For the Persian calendar, it handles leap year logic for the 12th month.

---

### `int date_days_in_month(const Date* date)`

- **Purpose**: Returns the number of days in the month for the provided `Date` object.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The number of days in the month, or `-1` if there is an error (e.g., a `NULL` date or unsupported calendar type).
- **Details**:
  - For the Gregorian calendar, February has 28 days, and the function adds a day if it is a leap year.
  - For the Persian calendar, the first 6 months have 31 days, the next 5 months have 30 days, and the 12th month has 29 or 30 days depending on whether it’s a leap year.

---

### `int date_days_in_year(const Date* date)`

- **Purpose**: Returns the total number of days in the year for the provided `Date` object, considering leap years if applicable.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: The total number of days in the year (365 or 366), or `-1` if there is an error (e.g., a `NULL` date or unsupported calendar type).
- **Details**:
  - For the Gregorian calendar, the function returns 366 if it’s a leap year, or 365 otherwise.
  - For the Persian calendar, the function similarly checks for leap years and returns 366 or 365 accordingly.

---

### `int date_week_number(const Date* date, int* yearNumber)`

- **Purpose**: Calculates the ISO 8601 week number for a given date. The function considers both the Gregorian and Persian calendars and optionally returns the year the week belongs to.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
  - `yearNumber`: A pointer to an integer where the year the week belongs to will be stored (optional, can be `NULL`).
- **Return**: The ISO 8601 week number, or `-1` in case of an error (e.g., `NULL` date or unsupported calendar type).
- **Details**:
  - For the Gregorian calendar, it calculates the ISO week number by adjusting the day of the year and the day of the week, and it checks edge cases for weeks at the beginning or end of the year.
  - For the Persian calendar, it uses a simplified calculation for the week number based on the day of the year.
  - If `yearNumber` is provided, it adjusts the year when the week number indicates the week belongs to the previous or following year.

---

### `int date_days_to(const Date* from, const Date* to)`

- **Purpose**: Calculates the number of days between two dates. Both dates must belong to the same calendar type (Gregorian or Persian).
- **Parameters**:
  - `from`: A pointer to the start `Date` object.
  - `to`: A pointer to the end `Date` object.
- **Return**: The number of days between the two dates, or `-1` in case of an error (e.g., mismatched calendar types or invalid dates).
- **Details**:
  - The function checks for `NULL` date pointers, validates both dates, and ensures both dates use the same calendar type.
  - It converts both dates to their Julian Day Numbers (JDN) and returns the difference between them, which represents the number of days between the two dates.

---

### `bool date_is_equal(const Date* lhs, const Date* rhs)`

- **Purpose**: Compares two `Date` objects for equality. It checks if both dates have the same year, month, day, and calendar type.
- **Parameters**:
  - `lhs`: A pointer to the first `Date` object.
  - `rhs`: A pointer to the second `Date` object.
- **Return**: Returns `true` if the dates are equal; otherwise, returns `false`.
- **Details**: The function validates both dates and ensures they share the same calendar type before comparing the year, month, and day fields.

---

### `bool date_is_less_than(const Date* lhs, const Date* rhs)`

- **Purpose**: Determines if the first date is earlier than the second date.
- **Parameters**:
  - `lhs`: A pointer to the first `Date` object.
  - `rhs`: A pointer to the second `Date` object.
- **Return**: Returns `true` if the first date is earlier than the second; otherwise, returns `false`.
- **Details**: This function first compares the year, then the month, and finally the day to determine which date is earlier. It also validates that both dates are from the same calendar type.

---

### `bool date_is_less_than_or_equal(const Date* lhs, const Date* rhs)`

- **Purpose**: Determines if the first date is earlier than or equal to the second date.
- **Parameters**:
  - `lhs`: A pointer to the first `Date` object.
  - `rhs`: A pointer to the second `Date` object.
- **Return**: Returns `true` if the first date is earlier than or equal to the second; otherwise, returns `false`.
- **Details**: It checks if the first date is less than or equal to the second using the `date_is_less_than` and `date_is_equal` functions.

---

### `bool date_is_greater_than(const Date* lhs, const Date* rhs)`

- **Purpose**: Determines if the first date is later than the second date.
- **Parameters**:
  - `lhs`: A pointer to the first `Date` object.
  - `rhs`: A pointer to the second `Date` object.
- **Return**: Returns `true` if the first date is later than the second; otherwise, returns `false`.
- **Details**: The function compares the year, then the month, and finally the day to check if the first date is later. It ensures that both dates are valid and belong to the same calendar type.

---

### `bool date_is_greater_than_or_equal(const Date* lhs, const Date* rhs)`

- **Purpose**: Determines if the first date is later than or equal to the second date.
- **Parameters**:
  - `lhs`: A pointer to the first `Date` object.
  - `rhs`: A pointer to the second `Date` object.
- **Return**: Returns `true` if the first date is later than or equal to the second; otherwise, returns `false`.
- **Details**: This function uses `date_is_greater_than` and `date_is_equal` to check the relationship between the two dates.

---

### `bool date_is_not_equals(const Date* lhs, const Date* rhs)`

- **Purpose**: Compares two `Date` objects and checks if they are not equal.
- **Parameters**:
  - `lhs`: A pointer to the first `Date` object.
  - `rhs`: A pointer to the second `Date` object.
- **Return**: Returns `true` if the dates are not equal; otherwise, returns `false`.
- **Details**: It simply negates the result of the `date_is_equal` function to check if the two dates are not equal.

---

### `bool date_is_leap_year_y(int year, CalendarType type)`

- **Purpose**: Determines if a given year is a leap year based on the calendar type (Gregorian or Persian).
- **Parameters**:
  - `year`: The year to check.
  - `type`: The calendar type (Gregorian or Persian).
- **Return**: Returns `true` if the year is a leap year; otherwise, returns `false`.
- **Details**: For the Gregorian calendar, the function follows the standard leap year rules. For the Persian calendar, it uses the `is_persian_leap_year` function to check if the year is a leap year. If the calendar type is unsupported, it returns `false`.

---

### `bool date_is_leap_year(const Date* date)`

- **Purpose**: Checks if the year of the given `Date` object is a leap year.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
- **Return**: Returns `true` if the year is a leap year, based on the calendar type (Gregorian or Persian); otherwise, returns `false`.
- **Details**: This function validates if the `Date` is not null and then uses `date_is_leap_year_y` to determine if the year is a leap year based on the `Date` object's calendar type.

---

### `bool date_set_date(Date* date, int year, int month, int day, CalendarType type)`

- **Purpose**: Sets the year, month, day, and calendar type of a `Date` object. It also validates the parameters to ensure the date is valid for the specified calendar type.
- **Parameters**:
  - `date`: A pointer to the `Date` object.
  - `year`: The year to set.
  - `month`: The month to set (valid range: 1-12).
  - `day`: The day to set (1-31 depending on the month and calendar type).
  - `type`: The calendar type (Gregorian or Persian).
- **Return**: Returns `true` if the date was successfully set and valid; otherwise, returns `false`.
- **Details**: This function checks if the parameters are valid (e.g., month is between 1 and 12, the day is valid for the month and year). It updates the date fields if all validations pass.

---

### `Date* date_current_date(CalendarType type)`

- **Purpose**: Retrieves the current date and returns it as a new `Date` object, based on the provided calendar type (Gregorian or Persian).
- **Parameters**:
  - `type`: The calendar type (Gregorian or Persian).
- **Return**: Returns a pointer to the newly allocated `Date` object representing the current date. If an error occurs (e.g., memory allocation failure or unsupported calendar type), it returns `NULL`.
- **Details**: The function gets the current system time, then, depending on the calendar type, either returns the current Gregorian date or converts it to a Persian date using the `date_gregorian_to_solar` function. The caller is responsible for freeing the memory of the returned `Date` object.

---

### `Date* date_from_string(const char* string, const char* format, CalendarType type)`

- **Purpose**: Creates a `Date` object from a string representation of a date based on a specified format and calendar type (Gregorian or Persian).
- **Parameters**:
  - `string`: The string containing the date to be parsed.
  - `format`: The format string specifying how the date is formatted (e.g., `"%Y-%m-%d"` for Gregorian).
  - `type`: The calendar type, either `Gregorian` or `Persian`.
- **Return**: Returns a pointer to a newly allocated `Date` object, or `NULL` if the parsing or memory allocation fails.
- **Details**:
  - For the **Gregorian calendar**, the function uses the `strptime` function (or `win_strptime` on Windows) to parse the string according to the provided format. It then adjusts the `tm` structure values into a `Date` object.
  - For the **Persian calendar**, a simpler approach is used, parsing the string directly in the `"YYYY-MM-DD"` format using `sscanf`.
  - Memory is allocated dynamically for the `Date` object, and it is up to the caller to free this memory.

---

### `char* date_to_string(const Date* date, const char* format)`

- **Purpose**: Converts a `Date` object into a formatted string according to a specified format. It supports both the Gregorian and Persian calendars.
- **Parameters**:
  - `date`: A pointer to the `Date` object to be converted to a string.
  - `format`: A format string that specifies how to format the date. For Gregorian dates, it follows the rules of the `strftime` function, and for Persian dates, it uses a custom format (`"YYYY-MM-DD"`).
- **Return**: Returns a pointer to a dynamically allocated string containing the formatted date, or `NULL` if an error occurs (e.g., invalid input or memory allocation failure).
- **Details**:
  - For the **Gregorian calendar**, the function converts the `Date` object into a `tm` structure and uses `strftime` to generate the formatted date string.
  - For the **Persian calendar**, the function uses a custom formatting method, outputting the date as `"YYYY-MM-DD"` using `snprintf`.
  - Memory for the date string is dynamically allocated, and the caller is responsible for freeing it.

---

### `long date_to_julian_day(const Date* date)`

- **Purpose**: Converts a Gregorian `Date` object into a Julian Day Number (JDN), which is a continuous count of days since the beginning of the Julian Period. 
- **Parameters**:
  - `date`: A pointer to the `Date` object in the Gregorian calendar to be converted.
- **Return**: The corresponding Julian Day Number as a `long`. Returns `-1` if the date is `NULL` or invalid.
- **Details**:
  - The function first checks the validity of the input `Date` object.
  - It calculates the Julian Day Number using a formula that takes into account the year, month, and day of the Gregorian date.
  - The function returns the computed Julian Day Number.

---

### `Date* date_from_julian_day(long jd)`

- **Purpose**: Converts a Julian Day Number (JDN) into a `Date` object in the Gregorian calendar.
- **Parameters**:
  - `jd`: The Julian Day Number to be converted.
- **Return**: A pointer to a dynamically allocated `Date` object corresponding to the given Julian Day Number. Returns `NULL` if the Julian Day Number is invalid or if memory allocation fails.
- **Details**:
  - The function uses a series of mathematical transformations to convert the Julian Day Number into a Gregorian date.
  - It handles leap years and other calendar adjustments.
  - The function allocates memory for a new `Date` object and assigns the computed year, month, and day to it.

---

### `Date* date_gregorian_to_solar(const Date* gregorian_date)`

- **Purpose**: Converts a Gregorian date to the corresponding date in the Persian (Jalali) calendar.
- **Parameters**:
  - `gregorian_date`: A pointer to the `Date` object in the Gregorian calendar to be converted.
- **Return**: A pointer to a dynamically allocated `Date` object in the Persian calendar. Returns `NULL` if the input date is `NULL` or if memory allocation fails.
- **Details**:
  - The function computes the number of days between the Gregorian date and the start of the Jalali calendar.
  - It then uses this day count to calculate the corresponding Jalali year, month, and day.
  - The function allocates a new `Date` object for the Persian date and sets the `calendarType` to `Persian`.

---

### `Date* date_solar_to_gregorian(const Date* solar_date)`

- **Purpose**: Converts a given Persian (Jalali) date to its equivalent Gregorian date.
- **Parameters**:
  - `solar_date`: A pointer to the `Date` object representing the Persian date.
- **Return**: A pointer to a dynamically allocated `Date` object representing the corresponding Gregorian date. Returns `NULL` if the input date is `NULL` or memory allocation fails.
- **Details**:
  - The function first checks if the Persian date is valid, ensuring that the date is within the range of the Persian calendar and adjusting for leap years.
  - It calculates the total number of days since the base date in the Persian calendar and then converts these days into the equivalent Gregorian date.
  - The calculation includes handling leap years and month/day conversions based on both the Persian and Gregorian calendars.
  - The result is stored in a newly allocated `Date` object, which the caller is responsible for freeing later.
  - The function handles memory allocation errors and invalid date checks.

---

### `void date_deallocate(Date* date)`

- **Purpose**: Frees the memory associated with a dynamically allocated `Date` object.
- **Parameters**:
  - `date`: A pointer to the `Date` object to be deallocated.
- **Return**: None.
- **Details**:
  - The function checks if the provided pointer is `NULL`. If it is, a warning is printed, and no memory deallocation occurs.
  - If the pointer is valid, the memory allocated for the `Date` object is freed using the `free` function.

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

---

## Example 7: Convert Persian to Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    // show error here because this date is not valid 1403 i leap year not 1402
    Date* persian_for_conversion = date_create_ymd(1402, 12, 30, Persian);
    Date* converted_gregorian = date_solar_to_gregorian(persian_for_conversion);
    
    if (converted_gregorian && persian_for_conversion) {
        fmt_printf("Converted to Gregorian: %d-%d-%d\n", converted_gregorian->year, converted_gregorian->month, converted_gregorian->day);

        date_deallocate(converted_gregorian);
        date_deallocate(persian_for_conversion);
    }
    else {
        fmt_fprintf(stderr, "Can not allocate memory for date object\n");
    }

    
    return 0;
}
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

---

## Example 19: Check if a Date is Valid
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    // date is invalid because feb consists of 29 days
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

---

## License

This project is open-source and available under [ISC License].