#include "time.h"
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else 
#include <sys/time.h>
#endif 

Time* time_create(int h, int m, int s, int ms) {
    Time* my_time = (Time*)malloc(sizeof(Time));
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
        perror("time is null");
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
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null");
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
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null");
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
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null");
        return -1;
    } 
    return t->hour;
}

int time_minute(const Time* t) {
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null");
        return -1;
    }
    return t->minute;
}

int time_second(const Time* t) {
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null");
        return -1;
    }
    return t->second;
}

int time_msec(const Time* t) {
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null");
        return -1;
    }
    return t->msec;
}

int time_msecs_since_start_of_day() {
    Time* t = time_current_time();

    if (t == NULL) {
        perror("time is null");
        return -1;
    }
    int msec_day = t->msec + (t->second * 1000) + (t->minute * 60 * 1000) + (t->hour * 60 * 60 * 1000);

    free(t);
    return msec_day;
}

int time_msecs_to(Time* from, Time* to) {
    if (from == NULL || to == NULL || !time_is_valid(from) || !time_is_valid(to)) {
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
    if (from == NULL || to == NULL || !time_is_valid(from) || !time_is_valid(to)) {
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
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null or not valid");
        return false;
    }

    t->hour = h;
    t->minute = m;
    t->second = s;
    t->msec = ms;

    return true;
}

char* time_to_string(const Time* t) {
    if (t == NULL || !time_is_valid(t)) {
        perror("time is null or not valid");
        return NULL;
    }
    char* time = (char*) malloc(sizeof(char) * 16);

    if (!time) {
        perror("Can not allocate memory");
        return NULL;
    }
    sprintf(time, "(%d:%02d:%02d:%03d)", t->hour, t->minute, t->second, t->msec);
    return time;
}