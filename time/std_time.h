/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Time
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in std_time.c (file-level convention).
 */

#ifndef TIME_H
#define TIME_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define TIME_LOGGING_ENABLE */

#ifdef TIME_LOGGING_ENABLE
    #define TIME_LOG(fmt, ...) \
        do { fprintf(stderr, "[TIME LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define TIME_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    int hour;    /* 0..23  */
    int minute;  /* 0..59  */
    int second;  /* 0..59  */
    int msec;    /* 0..999 */
} Time;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

Time*    time_create                          (int h, int m, int s, int ms);
Time*    time_copy                            (const Time* src);
Time*    time_current_time                    (void);
Time*    time_from_msecs_since_start_of_day   (int msecs);
Time*    time_from_seconds_of_day             (double secs);
Time*    time_from_string                     (const char* str);


/* ------------------------------------------------------------------ */
/* Destruction                                                        */
/* ------------------------------------------------------------------ */

void     time_deallocate                      (Time* t);


/* ------------------------------------------------------------------ */
/* Mutation                                                           */
/* ------------------------------------------------------------------ */

bool     time_set_hms                         (Time* t, int h, int m, int s, int ms);
void     time_add_msecs                       (Time* t, int ms);
void     time_add_secs                        (Time* t, int s);


/* ------------------------------------------------------------------ */
/* Component accessors                                                */
/* ------------------------------------------------------------------ */

int      time_hour                            (const Time* t);
int      time_minute                          (const Time* t);
int      time_second                          (const Time* t);
int      time_msec                            (const Time* t);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool     time_is_null                         (const Time* t);
bool     time_is_valid                        (const Time* t);
bool     time_is_midnight                     (const Time* t);
bool     time_is_valid_time                   (int h, int m, int s, int ms);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

int      time_compare                         (const Time* lhs, const Time* rhs);
bool     time_is_equal                        (const Time* lhs, const Time* rhs);
bool     time_is_not_equal                    (const Time* lhs, const Time* rhs);
bool     time_is_less_than                    (const Time* lhs, const Time* rhs);
bool     time_is_less_than_or_equal           (const Time* lhs, const Time* rhs);
bool     time_is_greater_than                 (const Time* lhs, const Time* rhs);
bool     time_is_greater_than_or_equal        (const Time* lhs, const Time* rhs);


/* ------------------------------------------------------------------ */
/* Differences / conversions to scalars                               */
/* ------------------------------------------------------------------ */

double   time_diff_in_seconds                 (const Time* from, const Time* to);
double   time_diff_in_msecs                   (const Time* from, const Time* to);
double   time_to_seconds_of_day               (const Time* t);
int      time_msecs_to                        (Time* from, Time* to);
int      time_secs_to                         (Time* from, Time* to);


/* ------------------------------------------------------------------ */
/* Wall-clock readers (independent of any Time*)                      */
/* ------------------------------------------------------------------ */

double   time_current_time_in_seconds         (void);
double   time_current_time_in_microseconds    (void);
int      time_msecs_since_start_of_day        (void);
int64_t  time_now_unix_msecs                  (void);
int64_t  time_monotonic_nsecs                 (void);
int64_t  time_monotonic_msecs                 (void);


/* ------------------------------------------------------------------ */
/* Formatting                                                         */
/* ------------------------------------------------------------------ */

char*    time_to_string                       (const Time* t);
char*    time_to_iso_string                   (const Time* t);
char*    time_format                          (const Time* t, const char* fmt);


/* ------------------------------------------------------------------ */
/* Sleep                                                              */
/* ------------------------------------------------------------------ */

void     time_sleep                           (unsigned int second);
void     time_sleep_ms                        (unsigned int milliseconds);


#ifdef __cplusplus
}
#endif

#endif 
