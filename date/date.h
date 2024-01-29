#ifndef DATE_H_
#define DATE_H_

#include <time.h>
#include <stdbool.h>


typedef enum {
    Gregorian,
    Persian
} CalendarType;

typedef struct {
    int year;
    int month;
    int day;
    CalendarType calendarType;
} Date;

void date_get_date(const Date* date, int *year, int *month, int *day);
void date_deallocate(Date* date);

// Constructors
Date* date_create(CalendarType type);
Date* date_create_ymd(int y, int m, int d, CalendarType type);
Date* date_add_days(const Date* date, int ndays);
Date* date_add_months(const Date* date, int nmonths);
Date* date_add_years(const Date* date, int nyears);
Date* date_from_julian_day(long jd);
Date* date_current_date(CalendarType type);
Date* date_from_string(const char* string, const char* format, CalendarType type);
Date* date_gregorian_to_solar(const Date* gregorian_date);
Date* date_solar_to_gregorian(const Date* solar_date);

bool date_is_null(const Date* date);
bool date_is_valid(const Date* date);
bool date_is_equal(const Date* lhs, const Date* rhs);
bool date_is_less_than(const Date* lhs, const Date* rhs);
bool date_is_less_than_or_equal(const Date* lhs, const Date* rhs);
bool date_is_greater_than(const Date* lhs, const Date* rhs);
bool date_is_greater_than_or_equal(const Date* lhs, const Date* rhs);
bool date_is_not_equals(const Date* lhs, const Date* rhs);
bool date_is_leap_year_y(int year, CalendarType type);
bool date_is_leap_year(const Date* date);
bool date_set_date(Date* date, int year, int month, int day, CalendarType type);

int date_day(const Date* date);
int date_month(const Date* date);
int date_year(const Date* date);
int date_day_of_week(const Date* date);
int date_day_of_year(const Date* date);
int date_days_in_month(const Date* date);
int date_days_in_year(const Date* date);
int date_week_number(const Date* date, int* yearNumber);
int date_days_to(const Date* from, const Date* to);

long date_to_julian_day(const Date* date);
char* date_to_string(const Date* date, const char* format);

#if defined(_WIN32) || defined(_WIN64)
char* win_strptime(const char *buf, const char *fmt, struct tm *tm);
#endif

#endif // DATE_H
