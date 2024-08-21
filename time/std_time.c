#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../fmt/fmt.h"
#include "std_time.h"


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdint.h> 
#else 
#include <sys/time.h>
#include <unistd.h>
#endif 

/**
 * @brief This function initializes a Time object with the given parameters. If the provided time parameters 
 * are invalid, it will log an error (if logging is enabled) and terminate the program.
 * 
 * @param h The hour (0-23).
 * @param m The minute (0-59).
 * @param s The second (0-59).
 * @param ms The millisecond (0-999).
 * 
 * @return A pointer to the newly created Time object.
 */
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

/**
 * @brief This function returns a Time object representing the current system time. 
 * It is compatible with both Windows and Unix-like systems. On Windows, it uses the 
 * GetLocalTime function, while on Unix-like systems, it uses gettimeofday.
 * 
 * @return A pointer to a Time object representing the current system time.
 */
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

/**
 * @brief This function checks if the provided Time object is valid. A valid Time object 
 * has hours in the range 0-23, minutes in the range 0-59, seconds in the range 0-59, 
 * and milliseconds in the range 0-999. It also logs an error if the Time object is NULL 
 * or invalid (if logging is enabled).
 * 
 * @param t Pointer to the Time object to validate.
 * @return true if the Time object is valid, false otherwise.
 */
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

/**
 * @brief This function adds the specified number of milliseconds to the provided Time object. 
 * It handles the overflow of milliseconds to seconds, seconds to minutes, and minutes to hours. 
 * If the hour exceeds 23, it wraps around to 0.
 * 
 * @param t Pointer to the Time object.
 * @param ms Number of milliseconds to add.
 */
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

/**
 * @brief This function adds the specified number of seconds to the provided Time object. 
 * It handles the overflow of seconds to minutes and minutes to hours. 
 * If the hour exceeds 23, it wraps around to 0.
 * 
 * @param t Pointer to the Time object.
 * @param s Number of seconds to add.
 */
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

/**
 * @brief This function checks whether the provided Time object is NULL.
 * 
 * @param t Pointer to the Time object.
 * @return true if the Time object is NULL, false otherwise.
 */
bool time_is_null(const Time* t) {
    return t == NULL? true: false;
}

/**
 * @brief This function returns the hour component of the provided Time object. 
 * If the Time object is NULL or invalid, it returns -1.
 * 
 * @param t Pointer to the Time object.
 * @return The hour component of the Time object, or -1 if the Time object is NULL or invalid.
 */
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

/**
 * @brief This function returns the minute component of the provided Time object. 
 * If the Time object is NULL or invalid, it returns -1.
 * 
 * @param t Pointer to the Time object.
 * @return The minute component of the Time object, or -1 if the Time object is NULL or invalid.
 */
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

/**
 * @brief This function returns the second component of the provided Time object.
 * 
 * @param t Pointer to the Time object.
 * @return The second component of the Time object, or -1 if the Time object is NULL or invalid.
 */
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

/**
 * @brief his function returns the millisecond component of the provided Time object.
 * 
 * @param t Pointer to the Time object.
 * @return The millisecond component of the Time object, or -1 if the Time object is NULL or invalid.
 */
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

/**
 * @brief This function retrieves the current system time and calculates the number of milliseconds that have
 * elapsed since the start of the day (00:00:00.000).
 * 
 * @return The number of milliseconds since the start of the day, or -1 if the current time could not be retrieved.
 */
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

/**
 * @brief This function computes the difference in milliseconds between the `to` and `from` Time objects.
 * The result is adjusted to ensure it is within the range of -86400000 and 86400000 milliseconds 
 * (the number of milliseconds in a day).
 * 
 * @param from Pointer to the starting Time object.
 * @param to Pointer to the ending Time object.
 * 
 * @return The difference in milliseconds between the two Time objects. Returns 0 if either Time object is NULL or invalid.
 */
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

/**
 * @brief This function computes the difference in seconds between the `to` and `from` Time objects.
 * The result is adjusted to ensure it is within the range of -86400 and 86400 seconds 
 * (the number of seconds in a day).
 * 
 * @param from Pointer to the starting Time object.
 * @param to Pointer to the ending Time object.
 * 
 * @return The difference in seconds between the two Time objects. Returns 0 if either Time object is NULL or invalid.
 */
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

/**
 * @brief This function sets the hour, minute, second, and millisecond components of a Time object. 
 * The provided values must represent a valid time.
 * 
 * @param t Pointer to the Time object to be modified.
 * @param h The hour (0-23).
 * @param m The minute (0-59).
 * @param s The second (0-59).
 * @param ms The millisecond (0-999).
 * 
 * @return true if the time was successfully set, false if the Time object is NULL or the time parameters are invalid.
 */
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

/**
 * @brief This function converts a Time object to a human-readable string in the format "(hh:mm:ss:ms)".
 * 
 * @param t Pointer to the Time object to be converted.
 * @return A pointer to a string representing the Time object. The caller is responsible for freeing the allocated memory. 
 * Returns NULL if the Time object is NULL or invalid, or if memory allocation fails.
 */
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

/**
 * @brief This function checks if the provided hour, minute, second, and millisecond values represent a valid time.
 * 
 * @param h The hour (0-23).
 * @param m The minute (0-59).
 * @param s The second (0-59).
 * @param ms The millisecond (0-999).
 * 
 * @return true if the parameters represent a valid time, false otherwise.
 */
bool time_is_valid_time(int h, int m, int s, int ms) {
    if ((h < 0 || h > 23) || (m < 0 || m > 59) || (s < 0 || s > 59) || (ms < 0 || ms > 999)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: is not valid time with this params in time_is_valid_time.\n");
        #endif 
        return false;
    } 
    return true;
}

/**
 * @brief This function converts a time given in milliseconds since the start of the day to a Time object.
 * 
 * @param msecs The number of milliseconds since the start of the day. Must be between 0 and 86399999 (inclusive).
 * @return A pointer to the newly created Time object, or NULL if the input is invalid or memory allocation fails.
 */
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

/**
 * @brief This function checks if two Time objects represent the same time. It returns true if the hour, minute, 
 * second, and millisecond values of both Time objects are identical.
 * 
 * @param lhs Pointer to the first Time object.
 * @param rhs Pointer to the second Time object.
 * 
 * @return true if the two Time objects are equal, false otherwise. Returns false if either pointer is NULL or the Time objects are invalid.
 */
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

/**
 * @brief This function checks if the first Time object (`lhs`) represents an earlier time than the second Time object (`rhs`). 
 * It compares the hour, minute, second, and millisecond values in that order.
 * 
 * @param lhs Pointer to the first Time object.
 * @param rhs Pointer to the second Time object.
 * 
 * @return true if `lhs` is less than `rhs`, false otherwise. Returns false if either pointer is NULL or the Time objects are invalid.
 */
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

/**
 * @brief This function checks if the first Time object (`lhs`) represents an earlier time or the same time as the second Time object (`rhs`).
 * It returns true if `lhs` is less than or equal to `rhs`.
 * 
 * @param lhs Pointer to the first Time object.
 * @param rhs Pointer to the second Time object.
 * 
 * @return true if `lhs` is less than or equal to `rhs`, false otherwise. Returns false if either pointer is NULL or the Time objects are invalid.
 */
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

/**
 * @brief This function compares two Time objects and returns true if the first Time object (`lhs`) 
 * represents a later time than the second Time object (`rhs`).
 * 
 * @param lhs Pointer to the first Time object.
 * @param rhs Pointer to the second Time object.
 * 
 * @return true if `lhs` is greater than `rhs`, false otherwise. If either pointer is NULL, returns false.
 */
bool time_is_greater_than(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_greater_than.\n");
        #endif 
        return false;
    }
    return time_is_less_than(rhs, lhs);
}

/**
 * @brief This function compares two Time objects and returns true if the first Time object (`lhs`) 
 * represents the same time or a later time than the second Time object (`rhs`).
 * 
 * @param lhs Pointer to the first Time object.
 * @param rhs Pointer to the second Time object.
 * 
 * @return true if `lhs` is greater than or equal to `rhs`, false otherwise. If either pointer is NULL, returns false.
 */
bool time_is_greater_than_or_equal(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_greater_than_or_equal.\n");
        #endif 
        return false;
    }
    return (time_is_greater_than(lhs, rhs) || time_is_equal(rhs, lhs))? true: false;
}

/**
 * @brief This function compares two Time objects and returns true if they do not represent the same time.
 * 
 * @param lhs Pointer to the first Time object.
 * @param rhs Pointer to the second Time object.
 * 
 * @return true if `lhs` is not equal to `rhs`, false otherwise. If either pointer is NULL, returns true.
 */
bool time_is_not_equal(const Time* lhs, const Time* rhs) {
    if (!lhs || !rhs) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_is_not_equal.\n");
        #endif 
        return true; // If one is NULL and the other is not, they are not equal.
    }
    return !time_is_equal(lhs, rhs);
}

/**
 * @brief This function frees the memory allocated for a Time object. If the pointer to the Time object is NULL,
 * the function will simply return without performing any action.
 * 
 * @param t Pointer to the Time object to be deallocated.
 */
void time_deallocate(Time* t) {
    if (!t) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Warning: Attempted to deallocate a NULL Time pointer in time_deallocate.\n");
        #endif
        return;
    }
    free(t);
}

/**
 * @brief This function returns the current time in seconds, with fractional seconds represented as a double.
 * It is platform-specific, using `GetSystemTimeAsFileTime` on Windows and `gettimeofday` on Unix-like systems.
 * 
 * @return The current time in seconds since the Unix epoch as a double.
 */
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

/**
 * @brief This function returns the current time in microseconds, with fractional microseconds represented as a double.
 * It is platform-specific, using `GetSystemTimeAsFileTime` on Windows and `gettimeofday` on Unix-like systems.
 * 
 * @return The current time in microseconds since the Unix epoch as a double.
 */
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

/**
 * @brief This function is platform-dependent and utilizes the appropriate system calls to achieve the delay.
 * On Windows, it uses the Sleep function, and on Unix-like systems, it uses the sleep function.
 * 
 * @param second The number of seconds to sleep.
 */
void time_sleep(unsigned int second) {
    #if defined(_WIN32) || defined(_WIN64)
        Sleep(second * 1000);
    #else 
        #include <unistd.h>
        sleep(second);
    #endif 
}

/**
 * @brief This function computes the difference in time between two Time objects, returning the result in seconds.
 * If the time difference spans across a day, the result will be adjusted to fall within the range of -86400 to 86400 seconds.
 * 
 * @param from Pointer to the starting Time object.
 * @param to Pointer to the ending Time object.
 * @return The difference in time between the two Time objects in seconds, as a double.
 *         Returns 0.0 if either Time object is NULL or invalid.
 */
double time_diff_in_seconds(const Time* from, const Time* to) {
    if (!from || !to) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: NULL Time pointer(s) in time_diff_in_seconds.\n");
        #endif 
        return 0.0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        #ifdef TIME_LOGGINH_ENABLE
            fmt_fprintf(stderr, "Error: Invalid Time object(s) in time_diff_in_seconds.\n");
        #endif 
        return 0.0;
    }

    // Convert each component to seconds and sum them up, including milliseconds
    double from_secs = from->hour * 3600 + from->minute * 60 + from->second + from->msec / 1000.0;
    double to_secs = to->hour * 3600 + to->minute * 60 + to->second + to->msec / 1000.0;
    double diff = to_secs - from_secs; 

    //  result should be within the range of -86400 and 86400 seconds
    if (diff < -86400) {
        diff += 86400;
    } 
    else if (diff > 86400) {
        diff -= 86400;
    }

    return diff;
}