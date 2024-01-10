#ifndef QTIME_H
#define QTIME_H

#include <stdbool.h>

typedef struct {
    int hour;
    int minute;
    int second;
    int msec;
} Time;

Time* time_create(int h, int m, int s, int ms);
Time time_add_msecs(Time t, int ms);
Time time_add_secs(Time t, int s);
int time_hour(Time t);
bool time_is_null(Time t);
bool time_is_valid(Time t);
int time_minute(Time t);
int time_msec(Time t);
int time_msecs_since_start_of_day(Time t);
int time_msecs_to(Time from, Time to);
int time_second(Time t);
int time_secs_to(Time from, Time to);
bool time_set_hms(Time *t, int h, int m, int s, int ms);
char* toString(const Time* time);

Time* time_current_time(void);
Time time_from_msecs_since_start_of_day(int msecs);
bool time_is_valid_time(int h, int m, int s, int ms);

bool time_equals(Time lhs, Time rhs);
bool time_less_than(Time lhs, Time rhs);
bool time_less_than_or_equal(Time lhs, Time rhs);
bool time_greater_than(Time lhs, Time rhs);
bool time_greater_than_or_equal(Time lhs, Time rhs);
bool time_is_not_equal(Time lhs, Time rhs);

#endif // QTIME_H
