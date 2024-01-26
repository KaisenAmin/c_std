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

- **Date Creation**
  - `Date* date_create(CalendarType type)`: Initializes a new Date object.
  - `Date* date_create_ymd(int y, int m, int d, CalendarType type)`: Initializes a Date object with a specific year, month, and day.

- **Date Manipulation**
  - `Date* date_add_days(const Date* date, int ndays)`: Adds a specified number of days to a Date object.
  - `Date* date_add_months(const Date* date, int nmonths)`: Adds a specified number of months to a Date object.
  - `Date* date_add_years(const Date* date, int nyears)`: Adds a specified number of years to a Date object.

- **Date Comparison**
  - `bool date_is_equal(const Date* lhs, const Date* rhs)`: Compares two Date objects for equality.
  - `bool date_is_less_than(const Date* lhs, const Date* rhs)`: Checks if one Date object is less than another.
  - `bool date_is_greater_than(const Date* lhs, const Date* rhs)`: Checks if one Date object is greater than another.

- **Utility Functions**
  - `int date_day_of_week(const Date* date)`: Returns the day of the week for a given date.
  - `int date_day_of_year(const Date* date)`: Returns the day of the year for a given date.
  - `int date_days_in_month(const Date* date)`: Returns the number of days in the month of a given date.
  - `int date_days_in_year(const Date* date)`: Returns the number of days in the year of a given date.

- **Conversion Functions**
  - `Date* date_gregorian_to_solar(const Date* gregorian_date)`: Converts a Gregorian date to a Persian date.
  - `Date* date_solar_to_gregorian(const Date* solar_date)`: Converts a Persian date to a Gregorian date.

- **Leap Year Calculation**
  - `bool date_is_leap_year(const Date* date)`: Determines if the year of a given date is a leap year.

- **String Conversion**
  - `char* date_to_string(const Date* date, const char* format)`: Converts a Date object to a string.
  - `Date* date_from_string(const char* string, const char* format, CalendarType type)`: Creates a Date object from a string.


## Example 1: Current Date in Gregorian Calendar
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* current_gregorian = date_current_date(Gregorian);
    if (current_gregorian) {
        fmt_fmt_printf("Current Gregorian Date: %d-%d-%d\n", current_gregorian->year, current_gregorian->month, current_gregorian->day);
        free(current_gregorian);
    }
    
    date_deallocate(current_gregorian);
    return 0;
}
```

## Example 2: Current Date in Persian Calendar
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* current_persian = date_current_date(Persian);
    if (current_persian) {
        fmt_fmt_printf("Current Persian Date: %d-%d-%d\n", current_persian->year, current_persian->month, current_persian->day);
        free(current_persian);
    }

    date_deallocate(current_persian);
    return 0;
}
```

## Example 3: Add 10 Days to a Specific Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* specific_date = date_create_ymd(2024, 1, 15, Gregorian);
    Date* new_date = date_add_days(specific_date, 10);
    
    if (new_date) {
        fmt_fmt_printf("New Gregorian Date: %d-%d-%d\n", new_date->year, new_date->month, new_date->day);
        free(new_date);
    }
    
    date_deallocate(specific_date);   
    date_deallocate(new_date);
    return 0;
}
```

## Example 4: Add 5 Months to a Specific Persian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* persian_date = date_create_ymd(1402, 7, 1, Persian);
    Date* new_persian_date = date_add_months(persian_date, 5);
    if (new_persian_date) {
        fmt_printf("New Persian Date: %d-%d-%d\n", new_persian_date->year, new_persian_date->month, new_persian_date->day);
        free(new_persian_date);
    }

    date_deallocate(persian_date);    
    date_deallocate(new_persian_date);
    return 0;
}
```

## Example 5: Subtract 2 Years from a Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* date_to_subtract = date_create_ymd(2024, 5, 20, Gregorian);
    Date* subtracted_date = date_add_years(date_to_subtract, -2);
    if (subtracted_date) {
        fmt_printf("Subtracted Gregorian Date: %d-%d-%d\n", subtracted_date->year, subtracted_date->month, subtracted_date->day);
        free(subtracted_date);
    }

    date_deallocate(date_to_subtract);    
    date_deallocate(subtracted_date);
    return 0;
}
```

## Example 6: Convert Gregorian to Persian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* gregorian_for_conversion = date_create_ymd(2024, 3, 21, Gregorian);
    Date* converted_persian = date_gregorian_to_solar(gregorian_for_conversion);
    if (converted_persian) {
        fmt_printf("Converted to Persian: %d-%d-%d\n", converted_persian->year, converted_persian->month, converted_persian->day);
        free(converted_persian);
    }

    date_deallocate(gregorian_for_conversion);
    date_deallocate(converted_persian);
    return 0;
}
```

## Example 7: Convert Persian to Gregorian Date
```c
#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    // show error here because this date is not valid 1403 i leap year not 1402
    Date* persian_for_conversion = date_create_ymd(1402, 12, 30, Persian);
    Date* converted_gregorian = date_solar_to_gregorian(persian_for_conversion);
    if (converted_gregorian) {
        fmt_printf("Converted to Gregorian: %d-%d-%d\n", converted_gregorian->year, converted_gregorian->month, converted_gregorian->day);
        date_deallocate(converted_gregorian);
    }

    date_deallocate(persian_for_conversion);
    return 0;
}
```

## Example 8: Create a Persian Date String
```c
#include "date/date.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Date* persian_date_string = date_create_ymd(1402, 5, 14, Persian);
    char* format = "%Y-%m-%d";
    char* date_str = date_to_string(persian_date_string, format);
    if (date_str) {
        fmt_printf("Persian Date String: %s\n", date_str);
        free(date_str);
    }

    date_deallocate(persian_date_string);
    return 0;
}
```

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