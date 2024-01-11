#ifndef DATE_H
#define DATE_H

#include <stdbool.h>
#include <time.h>

typedef struct {
    int year;
    int month;
    int day;
} Date;

// Constructors
Date* date_create();
Date* date_create_ymd(int y, int m, int d);

Date* date_add_days(const Date* date, int ndays);
Date* date_add_months(const Date* date, int nmonths);
Date* date_add_years(const Date* date, int nyears);

void date_get_date(const Date* date, int *year, int *month, int *day);

bool date_is_null(const Date* date);
bool date_is_valid(const Date* date);

// Date utility functions
int date_day(const Date* date);
int date_month(const Date* date);
int date_year(const Date* date);
int date_day_of_week(const Date* date);
int date_day_of_year(const Date* date);
int date_days_in_month(const Date* date);
int date_days_in_year(const Date* date);
long date_days_to(const Date* from, const Date* to);
char* date_to_string(const Date* date, const char* format);

Date* date_current_date();
bool date_is_leap_year(int year);
Date* date_from_string(const char* string, const char* format);

// Date comparison functions
bool date_equals(const Date* lhs, const Date* rhs);
bool date_less_than(const Date* lhs, const Date* rhs);
bool date_less_than_or_equal(const Date* lhs, const Date* rhs);
bool date_greater_than(const Date* lhs, const Date* rhs);
bool date_greater_than_or_equal(const Date* lhs, const Date* rhs);
bool date_not_equals(const Date* lhs, const Date* rhs);

#endif // DATE_H
