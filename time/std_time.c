/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Time
*/

#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "std_time.h"


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdint.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#define MS_PER_DAY 86400000


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
    TIME_LOG("[time_create]: Entering function with params - hour: %d, minute: %d, second: %d, millisecond: %d.", h, m, s, ms);

    if (!time_is_valid_time(h, m, s, ms)) {
        TIME_LOG("[time_create]: Error: Invalid time parameters.");
        return NULL;
    }

    Time* my_time = (Time*)malloc(sizeof(Time));
    if (!my_time) {
        TIME_LOG("[time_create]: Error: Memory allocation failed.");
        return NULL;
    }

    my_time->hour = h;
    my_time->minute = m;
    my_time->second = s;
    my_time->msec = ms;

    TIME_LOG("[time_create]: Time object created successfully with time %02d:%02d:%02d:%03d.", h, m, s, ms);
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
    TIME_LOG("[time_current_time]: Retrieving current system time.");

#if defined(_WIN32) || defined(_WIN64)
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);

    Time* timeObject = time_create(systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
    TIME_LOG("[time_current_time]: Current system time (Windows): %02d:%02d:%02d:%03d.", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

    return timeObject;

#else
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm* timeinfo = localtime(&tv.tv_sec);
    Time* timeObject = time_create(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tv.tv_usec / 1000);

    TIME_LOG("[time_current_time]: Current system time (Unix-like): %02d:%02d:%02d:%03d.", (int)timeinfo->tm_hour, (int)timeinfo->tm_min, (int)timeinfo->tm_sec, (int)tv.tv_usec / 1000);

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
    TIME_LOG("[time_is_valid]: Entering function.");

    if (t == NULL) {
        TIME_LOG("[time_is_valid]: Error: Time object is null.");
        return false;
    }

    if (t->hour < 0 || t->hour > 23) {
        TIME_LOG("[time_is_valid]: Invalid hour value: %d", t->hour);
        return false;
    }
    if (t->minute < 0 || t->minute > 59) {
        TIME_LOG("[time_is_valid]: Invalid minute value: %d", t->minute);
        return false;
    }
    if (t->second < 0 || t->second > 59) {
        TIME_LOG("[time_is_valid]: Invalid second value: %d", t->second);
        return false;
    }
    if (t->msec < 0 || t->msec > 999) {
        TIME_LOG("[time_is_valid]: Invalid millisecond value: %d", t->msec);
        return false;
    }

    TIME_LOG("[time_is_valid]: Time is valid.");
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
    TIME_LOG("[time_add_msecs]: Entering function.");

    if (!t) {
        TIME_LOG("[time_add_msecs]: Error: NULL Time pointer.");
        return;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_add_msecs]: Error: Invalid Time object.");
        return;
    }

    long long total = (long long)t->hour * 3600000LL + (long long)t->minute * 60000LL
                    + (long long)t->second * 1000LL + (long long)t->msec + (long long)ms;

    total %= MS_PER_DAY;
    if (total < 0) {
        total += MS_PER_DAY;
    }

    t->hour   = (int)(total / 3600000LL);
    total    %= 3600000LL;
    t->minute = (int)(total / 60000LL);
    total    %= 60000LL;
    t->second = (int)(total / 1000LL);
    t->msec   = (int)(total % 1000LL);

    TIME_LOG("[time_add_msecs]: Time updated to %02d:%02d:%02d:%03d.", t->hour, t->minute, t->second, t->msec);
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
    TIME_LOG("[time_add_secs]: Entering function.");

    if (!t) {
        TIME_LOG("[time_add_secs]: Error: NULL Time pointer.");
        return;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_add_secs]: Error: Invalid Time object.");
        return;
    }

    long long ms = (long long)s * 1000LL;
    ms %= MS_PER_DAY;            /* keeps the int-cast lossless */
    time_add_msecs(t, (int)ms);
    
    TIME_LOG("[time_add_secs]: Time updated to %02d:%02d:%02d.", t->hour, t->minute, t->second);
}


/**
 * @brief This function checks whether the provided Time object is NULL.
 * 
 * @param t Pointer to the Time object.
 * @return true if the Time object is NULL, false otherwise.
 */
bool time_is_null(const Time* t) {
    TIME_LOG("[time_is_null]: Entering function.");
    bool result = t == NULL ? true : false;

    TIME_LOG("[time_is_null]: Returning %s.", result ? "true" : "false");
    return result;
}


/**
 * @brief This function returns the hour component of the provided Time object. 
 * If the Time object is NULL or invalid, it returns -1.
 * 
 * @param t Pointer to the Time object.
 * @return The hour component of the Time object, or -1 if the Time object is NULL or invalid.
 */
int time_hour(const Time* t) {
    TIME_LOG("[time_hour]: Entering function.");

    if (!t) {
        TIME_LOG("[time_hour]: Error: NULL Time pointer.");
        return -1;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_hour]: Error: Invalid Time object.");
        return -1;
    }

    TIME_LOG("[time_hour]: Returning hour = %d.", t->hour);
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
    TIME_LOG("[time_minute]: Entering function.");

    if (!t) {
        TIME_LOG("[time_minute]: Error: NULL Time pointer.");
        return -1;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_minute]: Error: Invalid Time object.");
        return -1;
    }

    TIME_LOG("[time_minute]: Returning minute = %d.", t->minute);
    return t->minute;
}


/**
 * @brief This function returns the second component of the provided Time object.
 * 
 * @param t Pointer to the Time object.
 * @return The second component of the Time object, or -1 if the Time object is NULL or invalid.
 */
int time_second(const Time* t) {
    TIME_LOG("[time_second]: Entering function.");

    if (!t) {
        TIME_LOG("[time_second]: Error: NULL Time pointer.");
        return -1;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_second]: Error: Invalid Time object.");
        return -1;
    }

    TIME_LOG("[time_second]: Returning second = %d.", t->second);
    return t->second;
}


/**
 * @brief his function returns the millisecond component of the provided Time object.
 * 
 * @param t Pointer to the Time object.
 * @return The millisecond component of the Time object, or -1 if the Time object is NULL or invalid.
 */
int time_msec(const Time* t) {
    TIME_LOG("[time_msec]: Entering function.");

    if (!t) {
        TIME_LOG("[time_msec]: Error: NULL Time pointer.");
        return -1;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_msec]: Error: Invalid Time object.");
        return -1;
    }

    TIME_LOG("[time_msec]: Returning millisecond = %d.", t->msec);
    return t->msec;
}


/**
 * @brief This function retrieves the current system time and calculates the number of milliseconds that have
 * elapsed since the start of the day (00:00:00.000).
 * 
 * @return The number of milliseconds since the start of the day, or -1 if the current time could not be retrieved.
 */
int time_msecs_since_start_of_day() {
    TIME_LOG("[time_msecs_since_start_of_day]: Entering function.");

    Time* t = time_current_time();
    if (!t) {
        TIME_LOG("[time_msecs_since_start_of_day]: Error: Failed to retrieve current time.");
        return -1;
    }

    int msec_day = t->msec + (t->second * 1000) + (t->minute * 60 * 1000) + (t->hour * 60 * 60 * 1000);
    TIME_LOG("[time_msecs_since_start_of_day]: Calculated milliseconds since start of day = %d.", msec_day);

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
    TIME_LOG("[time_msecs_to]: Entering function.");

    if (!from || !to) {
        TIME_LOG("[time_msecs_to]: Error: NULL Time pointer(s).");
        return 0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        TIME_LOG("[time_msecs_to]: Error: Invalid Time object(s).");
        return 0;
    }

    // Each side is < MS_PER_DAY, so diff is naturally in (-MS_PER_DAY, MS_PER_DAY).
    int from_msecs = from->hour * 3600000 + from->minute * 60000 + from->second * 1000 + from->msec;
    int to_msecs   = to->hour   * 3600000 + to->minute   * 60000 + to->second   * 1000 + to->msec;
    int diff = to_msecs - from_msecs;

    TIME_LOG("[time_msecs_to]: from_msecs = %d, to_msecs = %d, diff = %d", from_msecs, to_msecs, diff);
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
    TIME_LOG("[time_secs_to]: Entering function.");

    if (!from || !to) {
        TIME_LOG("[time_secs_to]: Error: NULL Time pointer(s).");
        return 0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        TIME_LOG("[time_secs_to]: Error: Invalid Time object(s).");
        return 0;
    }

    // Each side is < 86400, so diff is naturally in (-86400, 86400).
    int from_secs = from->hour * 3600 + from->minute * 60 + from->second;
    int to_secs   = to->hour   * 3600 + to->minute   * 60 + to->second;
    int diff = to_secs - from_secs;

    TIME_LOG("[time_secs_to]: from_secs = %d, to_secs = %d, diff = %d", from_secs, to_secs, diff);
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
    TIME_LOG("[time_set_hms]: Entering function.");

    if (!t) {
        TIME_LOG("[time_set_hms]: Error: NULL Time pointer.");
        return false;
    }
    if (!time_is_valid_time(h, m, s, ms)) {
        TIME_LOG("[time_set_hms]: Error: Invalid time parameters (h: %d, m: %d, s: %d, ms: %d).", h, m, s, ms);
        return false;
    }

    t->hour = h;
    t->minute = m;
    t->second = s;
    t->msec = ms;

    TIME_LOG("[time_set_hms]: Time successfully set to (h: %d, m: %d, s: %d, ms: %d).", h, m, s, ms);
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
    TIME_LOG("[time_to_string]: Entering function.");

    if (!t) {
        TIME_LOG("[time_to_string]: Error: NULL Time pointer.");
        return NULL;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_to_string]: Error: Invalid Time object.");
        return NULL;
    }

    char* time_str = (char*) malloc(sizeof(char) * 16);
    if (!time_str) {
        TIME_LOG("[time_to_string]: Error: Memory allocation failed.");
        return NULL;
    }

    sprintf(time_str, "(%02d:%02d:%02d:%03d)", t->hour, t->minute, t->second, t->msec);
    TIME_LOG("[time_to_string]: Successfully converted Time to string: %s", time_str);

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
    TIME_LOG("[time_is_valid_time]: Entering function with parameters (h: %d, m: %d, s: %d, ms: %d).", h, m, s, ms);

    if ((h < 0 || h > 23) || (m < 0 || m > 59) || (s < 0 || s > 59) || (ms < 0 || ms > 999)) {
        TIME_LOG("[time_is_valid_time]: Error: Invalid time parameters.");
        return false;
    }

    TIME_LOG("[time_is_valid_time]: Parameters represent a valid time.");
    return true;
}

/**
 * @brief This function converts a time given in milliseconds since the start of the day to a Time object.
 * 
 * @param msecs The number of milliseconds since the start of the day. Must be between 0 and 86399999 (inclusive).
 * @return A pointer to the newly created Time object, or NULL if the input is invalid or memory allocation fails.
 */
Time* time_from_msecs_since_start_of_day(int msecs) {
    TIME_LOG("[time_from_msecs_since_start_of_day]: Entering function.");

    if (msecs < 0 || msecs >= 86400000) { // 24 hours * 60 minutes * 60 seconds * 1000 ms
        TIME_LOG("[time_from_msecs_since_start_of_day]: Error: Invalid milliseconds value: %d", msecs);
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
        TIME_LOG("[time_from_msecs_since_start_of_day]: Error: time_create failed.");
        return NULL;
    }

    TIME_LOG("[time_from_msecs_since_start_of_day]: Successfully created Time object.");
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
    TIME_LOG("[time_is_equal]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_is_equal]: Error: NULL Time pointer(s) provided.");
        return false;
    }
    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        TIME_LOG("[time_is_equal]: Error: Invalid Time object(s) provided.");
        return false;
    }

    bool result = (lhs->hour == rhs->hour && lhs->minute == rhs->minute && lhs->second == rhs->second && lhs->msec == rhs->msec);
    TIME_LOG("[time_is_equal]: Comparison result: %s", result ? "Equal" : "Not equal");

    return result;
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
    TIME_LOG("[time_is_less_than]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_is_less_than]: Error: NULL Time pointer(s) provided.");
        return false;
    }

    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        TIME_LOG("[time_is_less_than]: Error: Invalid Time object(s) provided.");
        return false;
    }

    bool result = (lhs->hour < rhs->hour) ||
                  (lhs->hour == rhs->hour && lhs->minute < rhs->minute) ||
                  (lhs->hour == rhs->hour && lhs->minute == rhs->minute && lhs->second < rhs->second) ||
                  (lhs->hour == rhs->hour && lhs->minute == rhs->minute && lhs->second == rhs->second && lhs->msec < rhs->msec);

    TIME_LOG("[time_is_less_than]: Comparison result: %s", result ? "Less than" : "Not less than");

    return result;
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
    TIME_LOG("[time_is_less_than_or_equal]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_is_less_than_or_equal]: Error: NULL Time pointer(s) provided.");
        return false;
    }
    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        TIME_LOG("[time_is_less_than_or_equal]: Error: Invalid Time object(s) provided.");
        return false;
    }

    bool result = time_is_equal(lhs, rhs) || time_is_less_than(lhs, rhs);
    TIME_LOG("[time_is_less_than_or_equal]: Result of comparison: %s", result ? "Less than or equal" : "Greater");

    return result;
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
    TIME_LOG("[time_is_greater_than]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_is_greater_than]: Error: NULL Time pointer(s) provided.");
        return false;
    }

    bool result = time_is_less_than(rhs, lhs);
    TIME_LOG("[time_is_greater_than]: Result of comparison: %s", result ? "Greater" : "Not greater");

    return result;
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
    TIME_LOG("[time_is_greater_than_or_equal]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_is_greater_than_or_equal]: Error: NULL Time pointer(s) provided.");
        return false;
    }

    bool result = time_is_greater_than(lhs, rhs) || time_is_equal(rhs, lhs);
    TIME_LOG("[time_is_greater_than_or_equal]: Result of comparison: %s", result ? "Greater than or equal" : "Not greater than or equal");

    return result;
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
    TIME_LOG("[time_is_not_equal]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_is_not_equal]: Error: NULL Time pointer(s) provided.");
        return true; 
    }
    
    bool result = !time_is_equal(lhs, rhs);
    TIME_LOG("[time_is_not_equal]: Time objects comparison result: %s", result ? "Not equal" : "Equal");

    return result;
}


/**
 * @brief This function frees the memory allocated for a Time object. If the pointer to the Time object is NULL,
 * the function will simply return without performing any action.
 * 
 * @param t Pointer to the Time object to be deallocated.
 */
void time_deallocate(Time* t) {
    TIME_LOG("[time_deallocate]: Entering function.");
    if (!t) {
        TIME_LOG("[time_deallocate]: Warning: Attempted to deallocate a NULL Time pointer.");
        return;
    }

    free(t);
    TIME_LOG("[time_deallocate]: Time object successfully deallocated.");
}


/**
 * @brief This function returns the current time in seconds, with fractional seconds represented as a double.
 * It is platform-specific, using `GetSystemTimeAsFileTime` on Windows and `gettimeofday` on Unix-like systems.
 * 
 * @return The current time in seconds since the Unix epoch as a double.
 */
double time_current_time_in_seconds() {
    TIME_LOG("[time_current_time_in_seconds]: Entering function.");

    #if defined(_WIN32) || defined(_WIN64)
        TIME_LOG("[time_current_time_in_seconds]: Detected Windows platform.");

        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);

        // Convert the 64-bit FILETIME to a 64-bit integer
        uint64_t time = (((uint64_t)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
        // Convert the time from 100-nanosecond intervals to seconds and adjust for the epoch difference
        double current_time = (double)(time / 10000000.0) - 11644473600.0;
        TIME_LOG("[time_current_time_in_seconds]: Current time in seconds (Windows): %.6f", current_time);
        return current_time;

    #else
        TIME_LOG("[time_current_time_in_seconds]: Detected Unix-like platform.");

        struct timeval tv;
        gettimeofday(&tv, NULL);

        double current_time = (double)(tv.tv_sec) + (double)(tv.tv_usec) / 1000000.0;
        TIME_LOG("[time_current_time_in_seconds]: Current time in seconds (Unix-like): %.6f", current_time);
        return current_time;
    #endif
}


/**
 * @brief This function returns the current time in microseconds, with fractional microseconds represented as a double.
 * It is platform-specific, using `GetSystemTimeAsFileTime` on Windows and `gettimeofday` on Unix-like systems.
 * 
 * @return The current time in microseconds since the Unix epoch as a double.
 */
double time_current_time_in_microseconds() {
    TIME_LOG("[time_current_time_in_microseconds]: Entering function.");

    #if defined(_WIN32) || defined(_WIN64)
        TIME_LOG("[time_current_time_in_microseconds]: Detected Windows platform.");

        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);

        // Convert the 64-bit FILETIME to a 64-bit integer
        uint64_t time = (((uint64_t)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;

        // Convert the time from 100-nanosecond intervals to microseconds and adjust for the epoch difference
        // 10 microseconds = 1 100-nanosecond interval
        double microseconds = (double)(time / 10.0) - 11644473600000000.0;

        TIME_LOG("[time_current_time_in_microseconds]: Current time in microseconds (Windows): %.3f", microseconds);
        return microseconds;

    #else
        TIME_LOG("[time_current_time_in_microseconds]: Detected Unix-like platform.");

        struct timeval tv;
        gettimeofday(&tv, NULL);

        // Calculate time in microseconds
        double microseconds = (double)(tv.tv_sec) * 1000000.0 + (double)(tv.tv_usec);
        TIME_LOG("[time_current_time_in_microseconds]: Current time in microseconds (Unix-like): %.3f", microseconds);
        return microseconds;
    #endif
}


/**
 * @brief This function is platform-dependent and utilizes the appropriate system calls to achieve the delay.
 * On Windows, it uses the Sleep function, and on Unix-like systems, it uses the sleep function.
 * 
 * @param second The number of seconds to sleep.
 */
void time_sleep(unsigned int second) {
    TIME_LOG("[time_sleep]: Entering function with delay: %u seconds.", second);

    #if defined(_WIN32) || defined(_WIN64)
        TIME_LOG("[time_sleep]: Using Windows Sleep function.");
        Sleep(second * 1000); // Sleep function takes milliseconds
    #else
        TIME_LOG("[time_sleep]: Using Unix-like sleep function.");
        sleep(second);
    #endif

    TIME_LOG("[time_sleep]: Finished sleeping for %u seconds.", second);
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
    TIME_LOG("[time_diff_in_seconds]: Entering with Time from: %p, Time to: %p", (void*)from, (void*)to);

    if (!from || !to) {
        TIME_LOG("[time_diff_in_seconds]: Error: NULL Time pointer(s).");
        return 0.0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        TIME_LOG("[time_diff_in_seconds]: Error: Invalid Time object(s).");
        return 0.0;
    }

    TIME_LOG("[time_diff_in_seconds]: Converting 'from' Time to seconds.");
    double from_secs = from->hour * 3600 + from->minute * 60 + from->second + from->msec / 1000.0;

    TIME_LOG("[time_diff_in_seconds]: Converting 'to' Time to seconds.");
    double to_secs = to->hour * 3600 + to->minute * 60 + to->second + to->msec / 1000.0;

    TIME_LOG("[time_diff_in_seconds]: Calculating difference in seconds.");
    double diff = to_secs - from_secs;

    // Result should be within the range of -86400 and 86400 seconds (for a full day)
    if (diff < -86400) {
        TIME_LOG("[time_diff_in_seconds]: Adjusting time difference (adding 86400).");
        diff += 86400;
    } 
    else if (diff > 86400) {
        TIME_LOG("[time_diff_in_seconds]: Adjusting time difference (subtracting 86400).");
        diff -= 86400;
    }

    TIME_LOG("[time_diff_in_seconds]: Final time difference: %.3f seconds", diff);
    return diff;
}


/**
 * @brief Deep copy of a Time object.
 *
 * Allocates a new Time and copies the four fields. The source is not modified.
 * Returns NULL on NULL/invalid input or allocation failure, so it never aborts
 * the host process.
 *
 * @param src Source Time pointer. May be NULL.
 * @return Newly allocated Time* the caller must free with time_deallocate(),
 *         or NULL on error.
 *
 * @code
 * Time* a = time_create(13, 45, 30, 250);
 * Time* b = time_copy(a);
 * // b is independent of a; modifying b does not affect a
 * time_deallocate(a);
 * time_deallocate(b);
 * @endcode
 */
Time* time_copy(const Time* src) {
    TIME_LOG("[time_copy]: Entering function.");

    if (!src) {
        TIME_LOG("[time_copy]: Error: NULL source pointer.");
        return NULL;
    }
    if (!time_is_valid(src)) {
        TIME_LOG("[time_copy]: Error: Invalid source Time object.");
        return NULL;
    }

    Time* copy = (Time*)malloc(sizeof(Time));
    if (!copy) {
        TIME_LOG("[time_copy]: Error: Memory allocation failed.");
        return NULL;
    }

    copy->hour   = src->hour;
    copy->minute = src->minute;
    copy->second = src->second;
    copy->msec   = src->msec;

    TIME_LOG("[time_copy]: Copy succeeded.");
    return copy;
}


/**
 * @brief Format a Time as an ISO 8601 extended local time string.
 *
 * Produces "HH:MM:SS.mmm" (always 12 characters, zero-padded). This is the
 * partial-time form from RFC 3339 with millisecond precision and is portable
 * across most logging, telemetry and parsing pipelines.
 *
 * @param t Source Time. May be NULL.
 * @return Newly malloc'd null-terminated string the caller must free(), or
 *         NULL on NULL/invalid input or allocation failure.
 *
 * @code
 * Time* t = time_create(9, 5, 7, 42);
 * char* iso = time_to_iso_string(t);   // "09:05:07.042"
 * free(iso);
 * time_deallocate(t);
 * @endcode
 */
char* time_to_iso_string(const Time* t) {
    TIME_LOG("[time_to_iso_string]: Entering function.");

    if (!t) {
        TIME_LOG("[time_to_iso_string]: Error: NULL Time pointer.");
        return NULL;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_to_iso_string]: Error: Invalid Time object.");
        return NULL;
    }

    char* out = (char*)malloc(16);
    if (!out) {
        TIME_LOG("[time_to_iso_string]: Error: Memory allocation failed.");
        return NULL;
    }

    snprintf(out, 16, "%02d:%02d:%02d.%03d", t->hour, t->minute, t->second, t->msec);
    TIME_LOG("[time_to_iso_string]: produced %s", out);
    return out;
}


/**
 * @brief Parse a textual time representation back into a Time object.
 *
 * Accepted forms (whitespace at the front is skipped, trailing characters
 * after a valid match are not allowed):
 *   - "HH:MM:SS"               (msec assumed 0)
 *   - "HH:MM:SS.mmm"           (ISO 8601 extended; 1-3 fractional digits)
 *   - "HH:MM:SS:mmm"           (the format produced by time_to_string)
 *   - "(HH:MM:SS:mmm)"         (parenthesised form from time_to_string)
 *
 * Each component must be in range; any out-of-range value rejects the input.
 *
 * @param str Null-terminated input string. May be NULL.
 * @return Newly allocated Time* the caller must free with time_deallocate(),
 *         or NULL on NULL/invalid input.
 *
 * @code
 * Time* a = time_from_string("(14:30:45:678)");
 * Time* b = time_from_string("09:00:00.500");
 * Time* c = time_from_string("23:59:59");   // msec defaults to 0
 * time_deallocate(a); time_deallocate(b); time_deallocate(c);
 * @endcode
 */
Time* time_from_string(const char* str) {
    TIME_LOG("[time_from_string]: Entering function.");

    if (!str) {
        TIME_LOG("[time_from_string]: Error: NULL string.");
        return NULL;
    }

    /* Skip leading whitespace and a single optional '(' */
    const char* p = str;
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    bool paren = false;
    if (*p == '(') { 
        paren = true; 
        p++; 
    }

    int h = -1, m = -1, s = -1, ms = 0;
    int consumed = 0;

    /* Try HH:MM:SS.mmm first (ISO style). */
    if (sscanf(p, "%d:%d:%d.%d%n", &h, &m, &s, &ms, &consumed) == 4) {
        /* matched */
    }
    /* Then HH:MM:SS:mmm (project's own to_string style). */
    else if (sscanf(p, "%d:%d:%d:%d%n", &h, &m, &s, &ms, &consumed) == 4) {
        /* matched */
    }
    /* Finally bare HH:MM:SS. */
    else if (sscanf(p, "%d:%d:%d%n", &h, &m, &s, &consumed) == 3) {
        ms = 0;
    }
    else {
        TIME_LOG("[time_from_string]: Error: could not parse '%s'", str);
        return NULL;
    }

    const char* tail = p + consumed;
    if (paren) {
        if (*tail != ')') {
            TIME_LOG("[time_from_string]: Error: missing closing parenthesis.");
            return NULL;
        }
        tail++;
    }
    while (*tail && isspace((unsigned char)*tail)) tail++;
    if (*tail != '\0') {
        TIME_LOG("[time_from_string]: Error: trailing characters '%s'", tail);
        return NULL;
    }

    if (!time_is_valid_time(h, m, s, ms)) {
        TIME_LOG("[time_from_string]: Error: components out of range.");
        return NULL;
    }

    return time_create(h, m, s, ms);
}


/**
 * @brief Total seconds since midnight, including the fractional millisecond
 *        component, as a double.
 *
 * The return is in [0.0, 86400.0). Useful for math against time_diff_in_seconds()
 * or when interpolating between two times.
 *
 * @param t Source Time. May be NULL.
 * @return Seconds since 00:00:00.000, or -1.0 if t is NULL or invalid.
 *
 * @code
 * Time* t = time_create(1, 30, 0, 500);
 * double s = time_to_seconds_of_day(t);   // 5400.5
 * time_deallocate(t);
 * @endcode
 */
double time_to_seconds_of_day(const Time* t) {
    TIME_LOG("[time_to_seconds_of_day]: Entering function.");

    if (!t) {
        TIME_LOG("[time_to_seconds_of_day]: Error: NULL Time pointer.");
        return -1.0;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_to_seconds_of_day]: Error: Invalid Time object.");
        return -1.0;
    }

    return (double)t->hour * 3600.0
         + (double)t->minute * 60.0
         + (double)t->second
         + (double)t->msec / 1000.0;
}

/**
 * @brief Build a Time object from a floating-point seconds-of-day value.
 *
 * @p secs must be in [0.0, 86400.0); milliseconds are taken from the fractional
 * part with rounding to the nearest millisecond. The function returns NULL on
 * out-of-range or non-finite input — it never wraps silently, so callers can
 * detect bad data.
 *
 * @param secs Seconds since midnight, fractional permitted.
 * @return Newly allocated Time* (caller frees with time_deallocate()) or NULL.
 *
 * @code
 * Time* t = time_from_seconds_of_day(5400.5);  // 01:30:00.500
 * time_deallocate(t);
 * @endcode
 */
Time* time_from_seconds_of_day(double secs) {
    TIME_LOG("[time_from_seconds_of_day]: Entering function.");

    /* Reject NaN / Inf without including <math.h> */
    if (!(secs >= 0.0 && secs < 86400.0)) {
        TIME_LOG("[time_from_seconds_of_day]: Error: out of range.");
        return NULL;
    }

    /* Convert to milliseconds with rounding to nearest. */
    long total_ms = (long)(secs * 1000.0 + 0.5);
    if (total_ms < 0)               total_ms = 0;
    if (total_ms >= 86400000L)      total_ms = 86399999L;

    int h  = (int)(total_ms / 3600000L);
    long rem = total_ms % 3600000L;
    int m  = (int)(rem / 60000L);
    rem %= 60000L;
    int s  = (int)(rem / 1000L);
    int ms = (int)(rem % 1000L);

    return time_create(h, m, s, ms);
}


/**
 * @brief Milliseconds since the Unix epoch (1970-01-01T00:00:00 UTC).
 *
 * Uses platform-specific high-resolution clocks: GetSystemTimeAsFileTime on
 * Windows, gettimeofday on POSIX. The result is a 64-bit signed integer and
 * does not suffer the 2038 wraparound of 32-bit time_t.
 *
 * @return Current Unix time in milliseconds.
 *
 * @code
 * int64_t t0 = time_now_unix_msecs();
 * // ... do work ...
 * int64_t t1 = time_now_unix_msecs();
 * printf("elapsed: %lld ms\n", (long long)(t1 - t0));
 * @endcode
 */
int64_t time_now_unix_msecs(void) {
    TIME_LOG("[time_now_unix_msecs]: Entering function.");

#if defined(_WIN32) || defined(_WIN64)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    /* FILETIME counts 100-ns ticks since 1601-01-01 UTC. */
    uint64_t ticks = (((uint64_t)ft.dwHighDateTime) << 32) | (uint64_t)ft.dwLowDateTime;
    /* Convert to ms, then shift epoch by 11644473600000 ms. */
    int64_t ms = (int64_t)(ticks / 10000ULL) - (int64_t)11644473600000LL;
    return ms;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000LL + (int64_t)(tv.tv_usec / 1000);
#endif
}


/**
 * @brief Is the Time exactly midnight (00:00:00.000)?
 *
 * Useful as a sentinel in day-rollover logic. Returns false for NULL or invalid
 * inputs so callers don't have to NULL-check separately.
 *
 * @param t Source Time. May be NULL.
 * @return true if t == 00:00:00.000, false otherwise.
 *
 * @code
 * Time* t = time_create(0, 0, 0, 0);
 * bool yes = time_is_midnight(t);  // true
 * time_deallocate(t);
 * @endcode
 */
bool time_is_midnight(const Time* t) {
    TIME_LOG("[time_is_midnight]: Entering function.");

    if (!t || !time_is_valid(t)) {
        TIME_LOG("[time_is_midnight]: Returning false (NULL or invalid).");
        return false;
    }
    return t->hour == 0 && t->minute == 0 && t->second == 0 && t->msec == 0;
}


/**
 * @brief Three-way comparison of two Time objects.
 *
 * Returns -1 if @p lhs comes strictly before @p rhs, +1 if strictly after,
 * 0 if equal. The two are first reduced to milliseconds-since-midnight so the
 * comparison is exact. On NULL or invalid input the function returns 0 — this
 * mirrors qsort_r style comparators while remaining safe.
 *
 * @param lhs First Time.
 * @param rhs Second Time.
 * @return -1, 0, or +1.
 *
 * @code
 * Time* a = time_create(10, 0, 0, 0);
 * Time* b = time_create(11, 0, 0, 0);
 * int cmp = time_compare(a, b);   // -1
 * time_deallocate(a); time_deallocate(b);
 * @endcode
 */
int time_compare(const Time* lhs, const Time* rhs) {
    TIME_LOG("[time_compare]: Entering function.");

    if (!lhs || !rhs) {
        TIME_LOG("[time_compare]: NULL pointer(s), returning 0.");
        return 0;
    }
    if (!time_is_valid(lhs) || !time_is_valid(rhs)) {
        TIME_LOG("[time_compare]: Invalid Time(s), returning 0.");
        return 0;
    }

    long l = (long)lhs->hour * 3600000L + (long)lhs->minute * 60000L
           + (long)lhs->second * 1000L + (long)lhs->msec;
    long r = (long)rhs->hour * 3600000L + (long)rhs->minute * 60000L
           + (long)rhs->second * 1000L + (long)rhs->msec;

    if (l < r) return -1;
    if (l > r) return  1;
    return 0;
}


/**
 * @brief Exact difference between two Time objects in milliseconds.
 *
 * Returns a double for parity with time_diff_in_seconds() but with millisecond
 * precision. The result is naturally in (-86_400_000, 86_400_000) ms because
 * both operands lie within a 24-hour window. NULL or invalid input yields 0.0.
 *
 * @param from Starting Time.
 * @param to   Ending Time.
 * @return     to - from, expressed in milliseconds.
 *
 * @code
 * Time* a = time_create(10, 0, 0, 0);
 * Time* b = time_create(10, 0, 1, 250);
 * double d = time_diff_in_msecs(a, b);   // 1250.0
 * time_deallocate(a); time_deallocate(b);
 * @endcode
 */
double time_diff_in_msecs(const Time* from, const Time* to) {
    TIME_LOG("[time_diff_in_msecs]: Entering function.");

    if (!from || !to) {
        TIME_LOG("[time_diff_in_msecs]: NULL pointer(s).");
        return 0.0;
    }
    if (!time_is_valid(from) || !time_is_valid(to)) {
        TIME_LOG("[time_diff_in_msecs]: Invalid Time(s).");
        return 0.0;
    }

    long f = (long)from->hour * 3600000L + (long)from->minute * 60000L
           + (long)from->second * 1000L + (long)from->msec;
    long t = (long)to->hour   * 3600000L + (long)to->minute   * 60000L
           + (long)to->second   * 1000L + (long)to->msec;

    return (double)(t - f);
}


/**
 * @brief strftime-like formatter for Time objects.
 *
 * Recognised specifiers (all others pass through literally):
 *   %H  - hour 00..23
 *   %I  - hour 01..12 (12-hour clock)
 *   %M  - minute 00..59
 *   %S  - second 00..59
 *   %f  - millisecond 000..999 (3 digits)
 *   %p  - "AM" or "PM"
 *   %%  - literal '%'
 *
 * The function does not honor locale; it always emits ASCII. Returns a newly
 * malloc'd string the caller must free(). NULL or invalid input returns NULL.
 *
 * @param t   Source Time. May be NULL.
 * @param fmt Format string. May be NULL.
 * @return Newly allocated string or NULL.
 *
 * @code
 * Time* t = time_create(14, 5, 30, 7);
 * char* a = time_format(t, "%H:%M:%S.%f");   // "14:05:30.007"
 * char* b = time_format(t, "%I:%M %p");      // "02:05 PM"
 * free(a); free(b); time_deallocate(t);
 * @endcode
 */
char* time_format(const Time* t, const char* fmt) {
    TIME_LOG("[time_format]: Entering function.");

    if (!t || !fmt) {
        TIME_LOG("[time_format]: NULL input.");
        return NULL;
    }
    if (!time_is_valid(t)) {
        TIME_LOG("[time_format]: Invalid Time object.");
        return NULL;
    }

    /* Worst-case expansion is ~5 chars per specifier ("AM"/"PM" or 3-digit ms).
     * Provide ample headroom so we can do a single pass without realloc. */
    size_t fmt_len = strlen(fmt);
    size_t cap = fmt_len * 6 + 16;
    char* out = (char*)malloc(cap);
    if (!out) {
        TIME_LOG("[time_format]: Memory allocation failed.");
        return NULL;
    }

    size_t oi = 0;
    int hour12 = t->hour % 12;
    if (hour12 == 0) hour12 = 12;
    const char* ampm = (t->hour < 12) ? "AM" : "PM";

    for (size_t i = 0; i < fmt_len; i++) {
        char c = fmt[i];
        if (c != '%' || i + 1 >= fmt_len) {
            out[oi++] = c;
            continue;
        }
        char spec = fmt[++i];
        int written = 0;
        switch (spec) {
            case 'H':
                written = snprintf(out + oi, cap - oi, "%02d", t->hour);
                break;
            case 'I':
                written = snprintf(out + oi, cap - oi, "%02d", hour12);
                break;
            case 'M':
                written = snprintf(out + oi, cap - oi, "%02d", t->minute);
                break;
            case 'S':
                written = snprintf(out + oi, cap - oi, "%02d", t->second);
                break;
            case 'f':
                written = snprintf(out + oi, cap - oi, "%03d", t->msec);
                break;
            case 'p':
                written = snprintf(out + oi, cap - oi, "%s", ampm);
                break;
            case '%':
                out[oi++] = '%';
                continue;
            default:
                /* Unknown specifier: emit literally */
                out[oi++] = '%';
                out[oi++] = spec;
                continue;
        }
        if (written < 0 || (size_t)written >= cap - oi) {
            free(out);
            TIME_LOG("[time_format]: snprintf overflow guard tripped.");
            return NULL;
        }
        oi += (size_t)written;
    }
    out[oi] = '\0';

    TIME_LOG("[time_format]: produced %s", out);
    return out;
}