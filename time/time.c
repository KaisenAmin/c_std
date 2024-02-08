#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../fmt/fmt.h"
#include "time.h"


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdint.h> 
#else 
#include <sys/time.h>
#endif 

Time* time_create(int h, int m, int s, int ms) {
    if (!time_is_valid_time(h, m, s, ms)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid time parameters in time_create.\n");
        #endif 
        exit(-1);
    }

    Time* my_time = (Time*)malloc(sizeof(Time));
    if (!my_time) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed in time_create.\n");
        #endif 
        exit(-1);
    }
    my_time->hour = h;
    my_time->minute = m;
    my_time->second = s;
    my_time->msec = ms;

    return my_time;
}

Time* time_current_time(void) {
#if defined(_WIN32) || defined(_WIN64)
    // Get the current time
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    Time *timeObject = time_create(systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

    return timeObject;
#else 
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm* timeinfo = localtime(&tv.tv_sec);
    Time *timeObject = time_create(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tv.tv_usec / 1000);

    return timeObject;
#endif 
}

bool time_is_valid(const Time* t) {
    if (t == NULL) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Time object is null and invalid in time_is_valid.\n");
        #endif 
        return false;
    }

    if (t->hour < 0 || t->hour > 23) {
        return false;
    }
    if (t->minute < 0 || t->minute > 59) {
        return false;
    }
    if (t->second < 0 || t->second > 59) {
        return false;
    }
    if (t->msec < 0 || t->msec > 999) {
        return false;
    }

    return true;
} 

void time_add_msecs(Time *t, int ms) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_add_msecs.\n");
        #endif 
        return;
    }
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid time in time_add_msecs.\n");
        #endif 
        return;
    }
    t->msec += ms;  // Add milliseconds

    // Convert excess milliseconds to seconds
    while (t->msec >= 1000) {
        t->msec -= 1000;
        t->second++;
    }

    // Handle overflow of seconds to minutes
    while (t->second >= 60) {
        t->second -= 60;
        t->minute++;
    }

    // Handle overflow of minutes to hours
    while (t->minute >= 60) {
        t->minute -= 60;
        t->hour++;
    }
    while (t->hour >= 24) {
        t->hour -= 24;
    }
}

void time_add_secs(Time* t, int s) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_add_secs.\n");
        #endif 
        return;
    }
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid time in time_add_secs.\n");
        #endif 
        return;
    }
    t->second += s;

    while (t->second >= 60) {
        t->second -= 60;
        t->minute++;
    }

    while (t->minute >= 60) {
        t->minute -= 60;
        t->hour++;
    }
    while (t->hour >= 24) {
        t->hour -= 24;
    }
}

bool time_is_null(const Time* t) {
    return t == NULL? true: false;
}

int time_hour(const Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_hour.\n");
        #endif 
        return -1;
    } 
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object in time_hour.\n");
        #endif 
        return -1;
    }
    return t->hour;
}

int time_minute(const Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_minute.\n");
        #endif 
        return -1;
    }
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object in time_minute.\n");
        #endif 
        return -1;
    }
    return t->minute;
}

int time_second(const Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_second.\n");
        #endif 
        return -1;
    }
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object in time_second.\n");
        #endif 
        return -1;
    }
    return t->second;
}

int time_msec(const Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_msec.\n");
        #endif 
        return -1;
    }
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object in time_msec.\n");
        #endif 
        return -1;
    }
    return t->msec;
}

int time_msecs_since_start_of_day() {
    Time* t = time_current_time();
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Failed to retrieve current time in time_msecs_since_start_of_day.\n");
        #endif 
        return -1;
    }

    int msec_day = t->msec + (t->second * 1000) + (t->minute * 60 * 1000) + (t->hour * 60 * 60 * 1000);
    free(t);
    return msec_day;
}

int time_msecs_to(Time* from, Time* to) {
    if (!from || !to) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_msecs_to.\n");
        #endif 
        return 0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object(s) in time_msecs_to.\n");
        #endif 
        return 0;
    }

    // Convert each component to milliseconds and sum them up
    int from_msecs = from->hour * 3600000 + from->minute * 60000 + from->second * 1000 + from->msec;
    int to_msecs = to->hour * 3600000 + to->minute * 60000 + to->second * 1000 + to->msec;
    int diff = to_msecs - from_msecs; // Calculate the difference

    // Ensure the result is within the range of -86400000 and 86400000 ms (the number of milliseconds in a day)
    if (diff < -86400000) {
        diff += 86400000;
    } 
    else if (diff > 86400000) {
        diff -= 86400000;
    }
    return diff;
}

int time_secs_to(Time* from, Time* to) {
    if (!from || !to) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_secs_to.\n");
        #endif 
        return 0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object(s) in time_secs_to.\n");
        #endif 
        return 0;
    }

    // Convert each component to seconds and sum them up, ignoring milliseconds
    int from_secs = from->hour * 3600 + from->minute * 60 + from->second;
    int to_secs = to->hour * 3600 + to->minute * 60 + to->second;
    int diff = to_secs - from_secs; // Calculate the difference

    // Ensure the result is within the range of -86400 and 86400 seconds
    if (diff < -86400) {
        diff += 86400;
    } 
    else if (diff > 86400) {
        diff -= 86400;
    }
    return diff;
}

bool time_set_hms(Time *t, int h, int m, int s, int ms) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_set_hms.\n");
        #endif 
        return false;
    }
    if (!time_is_valid_time(h, m, s, ms)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid time parameters in time_set_hms.\n");
        #endif 
        return false;
    }

    t->hour = h;
    t->minute = m;
    t->second = s;
    t->msec = ms;

    return true;
}

char* time_to_string(const Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer in time_to_string.\n");
        #endif 
        return NULL;
    }
    if (!time_is_valid(t)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object in time_to_string.\n");
        #endif 
        return NULL;
    }

    char* time_str = (char*) malloc(sizeof(char) * 16);
    if (!time_str) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Memory allocation failed in time_to_string.\n");
        #endif 
        return NULL;
    }
    
    sprintf(time_str, "(%d:%02d:%02d:%03d)", t->hour, t->minute, t->second, t->msec);
    return time_str;
}

bool time_is_valid_time(int h, int m, int s, int ms) {
    if ((h < 0 || h > 23) || (m < 0 || m > 59) || (s < 0 || s > 59) || (ms < 0 || ms > 999)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: is not valid time with this params in time_is_valid_time.\n");
        #endif 
        return false;
    } 
    return true;
}

Time* time_from_msecs_since_start_of_day(int msecs) {
    if (msecs < 0 || msecs >= 86400000) { // 24 hours * 60 minutes * 60 seconds * 1000 ms
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid milliseconds value in time_from_msecs_since_start_of_day.\n");
        #endif 
        return NULL;
    }

    int seconds = msecs / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    msecs = msecs % 1000;
    seconds = seconds % 60;
    minutes = minutes % 60;

    Time* my_time = time_create(hours, minutes, seconds, msecs);
    if (!my_time) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: time_create failed in time_from_msecs_since_start_of_day.\n");
        #endif 
        return NULL;
    }
    return my_time;
}

bool time_is_equal(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_equal.\n");
        #endif 
        return false;
    }
    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object(s) in time_is_equal.\n");
        #endif 
        return false;
    }
    if (lhs->hour == rhs->hour && lhs->minute  == rhs->minute && lhs->second == rhs->second && lhs->msec == rhs->msec) {
        return true;
    }
    return false;
}

bool time_is_less_than(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_less_than.\n");
        #endif 
        return false;
    }
    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object(s) in time_is_less_than.\n");
        #endif 
        return false;
    }
    if ((lhs->hour) < (rhs->hour) || (lhs->minute) < (rhs->minute) || (lhs->second) < (rhs->second) || (lhs->msec) < (rhs->msec)) {
        return true;
    }
    return false;
}

bool time_is_less_than_or_equal(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_less_than_or_equal.\n");
        #endif 
        return false;
    }
    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object(s) in time_is_less_than_or_equal.\n");
        #endif 
        return false;
    }
    return time_is_equal(lhs, rhs) || time_is_less_than(lhs, rhs);
}

bool time_is_greater_than(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_greater_than.\n");
        #endif 
        return false;
    }
    return time_is_less_than(rhs, lhs);
}

bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_greater_than_or_equal.\n");
        #endif 
        return false;
    }
    return (time_is_greater_than(lhs, rhs) || time_is_equal(rhs, lhs))? true: false;
}

bool time_is_not_equal(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_not_equal.\n");
        #endif 
        return true; // If one is NULL and the other is not, they are not equal.
    }
    return !time_is_equal(lhs, rhs);
}

void time_deallocate(Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Warning: Attempted to deallocate a NULL Time pointer in time_deallocate.\n");
        #endif
        return;
    }
    free(t);
}

double time_current_time_in_seconds() {
#if defined(_WIN32) || defined(_WIN64)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    // Convert the 64-bit FILETIME to a 64-bit integer
    uint64_t time = (((uint64_t)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    // Convert the time from 100-nanosecond intervals to seconds and adjust for the epoch difference
    return (double)(time / 10000000.0) - 11644473600.0;
#else 
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (double)(tv.tv_sec) + (double)(tv.tv_usec) / 1000000.0;
#endif 
}

double time_current_time_in_microseconds() {
#if defined(_WIN32) || defined(_WIN64)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    // Convert the 64-bit FILETIME to a 64-bit integer
    uint64_t time = (((uint64_t)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;

    // Convert the time from 100-nanosecond intervals to microseconds and adjust for the epoch difference
    // 10 microseconds = 1 100-nanosecond interval
    return (double)(time / 10.0) - 11644473600000000.0;
#else 
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Return the time in microseconds
    return (double)(tv.tv_sec) * 1000000.0 + (double)(tv.tv_usec);
#endif 
}

void time_sleep(unsigned int second) {
    #if defined(_WIN32) || defined(_WIN64)
        Sleep(second * 1000);
    #else 
        #include <unistd.h>
        sleep(second);
    #endif 
}