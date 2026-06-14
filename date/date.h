/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Date
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in date.c (file-level convention).
 *
 * Calendar-aware date arithmetic — Gregorian + Persian (Jalali).
 */

#ifndef DATE_H_
#define DATE_H_

#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define DATE_LOGGING_ENABLE */

#ifdef DATE_LOGGING_ENABLE
    #define DATE_LOG(fmt, ...) \
        do { fprintf(stderr, "[DATE LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define DATE_LOG(...) do { } while (0)
#endif


typedef enum {
    Gregorian,
    Persian
} CalendarType;


typedef struct {
    int          year;
    int          month;
    int          day;
    CalendarType calendarType;
} Date;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Date*        date_create                       (CalendarType type);
Date*        date_create_ymd                   (int y, int m, int d, CalendarType type);
Date*        date_clone                        (const Date* date);
Date*        date_current_date                 (CalendarType type);
Date*        date_from_string                  (const char* string, const char* format, CalendarType type);
Date*        date_from_julian_day              (long jd);


/* ------------------------------------------------------------------ */
/* Destruction / mutation                                             */
/* ------------------------------------------------------------------ */

void         date_deallocate                   (Date* date);
bool         date_set_date                     (Date* date, int year, int month, int day, CalendarType type);


/* ------------------------------------------------------------------ */
/* Component accessors                                                */
/* ------------------------------------------------------------------ */

void         date_get_date                     (const Date* date, int* year, int* month, int* day);
int          date_year                         (const Date* date);
int          date_month                        (const Date* date);
int          date_day                          (const Date* date);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool         date_is_null                      (const Date* date);
bool         date_is_valid                     (const Date* date);
bool         date_is_leap_year                 (const Date* date);
bool         date_is_leap_year_y               (int year, CalendarType type);
bool         date_is_weekend                   (const Date* date);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

int          date_compare                      (const Date* lhs, const Date* rhs);
bool         date_is_equal                     (const Date* lhs, const Date* rhs);
bool         date_is_not_equals                (const Date* lhs, const Date* rhs);
bool         date_is_less_than                 (const Date* lhs, const Date* rhs);
bool         date_is_less_than_or_equal        (const Date* lhs, const Date* rhs);
bool         date_is_greater_than              (const Date* lhs, const Date* rhs);
bool         date_is_greater_than_or_equal     (const Date* lhs, const Date* rhs);


/* ------------------------------------------------------------------ */
/* Arithmetic                                                         */
/* ------------------------------------------------------------------ */

Date*        date_add_days                     (const Date* date, int ndays);
Date*        date_add_months                   (const Date* date, int nmonths);
Date*        date_add_years                    (const Date* date, int nyears);
Date*        date_min                          (const Date* lhs, const Date* rhs);
Date*        date_max                          (const Date* lhs, const Date* rhs);
Date*        date_first_day_of_month           (const Date* date);
Date*        date_last_day_of_month            (const Date* date);
int          date_days_to                      (const Date* from, const Date* to);


/* ------------------------------------------------------------------ */
/* Calendrical introspection                                          */
/* ------------------------------------------------------------------ */

int          date_day_of_week                  (const Date* date);
int          date_day_of_year                  (const Date* date);
int          date_days_in_month                (const Date* date);
int          date_days_in_year                 (const Date* date);
int          date_week_number                  (const Date* date, int* yearNumber);
int          date_quarter                      (const Date* date);
const char*  date_weekday_name                 (const Date* date);
const char*  date_month_name                   (const Date* date);


/* ------------------------------------------------------------------ */
/* Calendar conversion                                                */
/* ------------------------------------------------------------------ */

Date*        date_gregorian_to_solar           (const Date* gregorian_date);
Date*        date_solar_to_gregorian           (const Date* solar_date);
long         date_to_julian_day                (const Date* date);


/* ------------------------------------------------------------------ */
/* Unix-epoch conversion (date at UTC midnight)                       */
/* ------------------------------------------------------------------ */

int64_t      date_to_unix                      (const Date* date);
Date*        date_from_unix                    (int64_t unix_seconds, CalendarType type);


/* ------------------------------------------------------------------ */
/* Formatting                                                         */
/* ------------------------------------------------------------------ */

char*        date_to_string                    (const Date* date, const char* format);


/* ------------------------------------------------------------------ */
/* Windows compatibility                                              */
/* ------------------------------------------------------------------ */

#if defined(_WIN32) || defined(_WIN64)
char*        win_strptime                      (const char* buf, const char* fmt, struct tm* tm);
#endif


#ifdef __cplusplus
}
#endif

#endif 
