/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Time
*/

#ifndef TIME_H
#define TIME_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif 

// #define TIME_LOGGING_ENABLE

#ifdef TIME_LOGGING_ENABLE
    #define TIME_LOG(fmt, ...) \
        do { fprintf(stderr, "[TIME LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define TIME_LOG(fmt, ...) do { } while (0)
#endif

typedef struct {
    int hour;    // Stores the hour part of the time (0-23)
    int minute;  // Stores the minute part of the time (0-59)
    int second;  // Stores the second part of the time (0-59)
    int msec;    // Stores the millisecond part of the time (0-999)
} Time;

char* time_to_string(const Time* t);

double time_current_time_in_seconds();
double time_current_time_in_microseconds();
double time_diff_in_seconds(const Time* from, const Time* to);

Time* time_create(int h, int m, int s, int ms);
Time* time_current_time(void);
Time* time_from_msecs_since_start_of_day(int msecs);

void time_add_msecs(Time *t, int ms);
void time_add_secs(Time* t, int s);
void time_deallocate(Time* t);
void time_sleep(unsigned int second);

int time_hour(const Time* t);
int time_minute(const Time* t);
int time_msec(const Time* t);
int time_msecs_since_start_of_day();
int time_msecs_to(Time* from, Time* to);
int time_second(const Time* t);
int time_secs_to(Time* from, Time* to);

bool time_is_valid_time(int h, int m, int s, int ms);
bool time_is_equal(const Time* lhs, const Time* rhs);
bool time_is_less_than(const Time* lhs, const Time* rhs);
bool time_is_less_than_or_equal(const Time* lhs, const Time* rhs);
bool time_is_greater_than(const Time* lhs, const Time* rhs);
bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs);
bool time_is_not_equal(const Time* lhs, const Time* rhs);
bool time_set_hms(Time *t, int h, int m, int s, int ms);
bool time_is_null(const Time* t);
bool time_is_valid(const Time* t);

#ifdef __cplusplus
}
#endif

#endif // QTIME_H
