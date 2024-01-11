#ifndef DATE_H
#define DATE_H

#include <stdbool.h>
#include <time.h>

typedef struct {
    int year;
    int month;
    int day;
} Date;

void date_get_date(const Date* date, int *year, int *month, int *day);

// Constructors
Date* date_create();
Date* date_create_ymd(int y, int m, int d);
Date* date_add_days(const Date* date, int ndays);
Date* date_add_months(const Date* date, int nmonths);
Date* date_add_years(const Date* date, int nyears);
Date* date_from_julian_day(long jd);
Date* date_current_date();
Date* date_from_string(const char* string, const char* format);
Date* date_from_string_format(const char* string, const char* format);
Date* date_start_of_day(const Date* date);
Date* date_end_of_day(const Date* date);
Date* date_current_date();
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
bool date_is_leap_year(int year);
bool date_is_leap_year(const Date* date);
bool date_set_date(Date* date, int year, int month, int day);

int date_day(const Date* date);
int date_month(const Date* date);
int date_year(const Date* date);
int date_day_of_week(const Date* date);
int date_day_of_year(const Date* date);
int date_days_in_month(const Date* date);
int date_days_in_year(const Date* date);
int date_week_number(const Date* date, int* yearNumber);
int date_days_to(const Date* from, const Date* to);

long date_days_to(const Date* from, const Date* to);
long date_to_julian_day(const Date* date);

char* date_to_string(const Date* date, const char* format);
char* date_to_string_format(const Date* date, const char* format);

#endif // DATE_H
