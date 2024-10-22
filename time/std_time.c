/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Time
*/

#include <string.h>
#include <time.h>
#include <stdlib.h>
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
    TIME_LOG("[time_create]: Entering function with params - hour: %d, minute: %d, second: %d, millisecond: %d.", h, m, s, ms);

    if (!time_is_valid_time(h, m, s, ms)) {
        TIME_LOG("[time_create]: Error: Invalid time parameters.");
        exit(-1);
    }

    Time* my_time = (Time*)malloc(sizeof(Time));
    if (!my_time) {
        TIME_LOG("[time_create]: Error: Memory allocation failed.");
        exit(-1);
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

    TIME_LOG("[time_add_msecs]: Adding %d milliseconds to Time object.", ms);
    t->msec += ms;  // Add milliseconds

    // Convert excess milliseconds to seconds
    while (t->msec >= 1000) {
        t->msec -= 1000;
        t->second++;
        TIME_LOG("[time_add_msecs]: Converted milliseconds to seconds.");
    }

    // Handle overflow of seconds to minutes
    while (t->second >= 60) {
        t->second -= 60;
        t->minute++;
        TIME_LOG("[time_add_msecs]: Converted seconds to minutes.");
    }

    // Handle overflow of minutes to hours
    while (t->minute >= 60) {
        t->minute -= 60;
        t->hour++;
        TIME_LOG("[time_add_msecs]: Converted minutes to hours.");
    }

    // Wrap hours around to 0 if they exceed 23
    while (t->hour >= 24) {
        t->hour -= 24;
        TIME_LOG("[time_add_msecs]: Wrapped hours around to 0.");
    }

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

    t->second += s;
    TIME_LOG("[time_add_secs]: Adding %d seconds to Time object.", s);

    while (t->second >= 60) {
        t->second -= 60;
        t->minute++;
        TIME_LOG("[time_add_secs]: Adjusted time: minute incremented.");
    }

    while (t->minute >= 60) {
        t->minute -= 60;
        t->hour++;
        TIME_LOG("[time_add_secs]: Adjusted time: hour incremented.");
    }

    while (t->hour >= 24) {
        t->hour -= 24;
        TIME_LOG("[time_add_secs]: Adjusted time: hour wrapped around to 0.");
    }

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

    // Convert each component to milliseconds and sum them up
    int from_msecs = from->hour * 3600000 + from->minute * 60000 + from->second * 1000 + from->msec;
    int to_msecs = to->hour * 3600000 + to->minute * 60000 + to->second * 1000 + to->msec;
    int diff = to_msecs - from_msecs; // Calculate the difference

    TIME_LOG("[time_msecs_to]: from_msecs = %d, to_msecs = %d, diff = %d", from_msecs, to_msecs, diff);

    // Ensure the result is within the range of -86400000 and 86400000 ms
    if (diff < -86400000) {
        diff += 86400000;
        TIME_LOG("[time_msecs_to]: Adjusted diff = %d (added 86400000).", diff);
    } 
    else if (diff > 86400000) {
        diff -= 86400000;
        TIME_LOG("[time_msecs_to]: Adjusted diff = %d (subtracted 86400000).", diff);
    }

    TIME_LOG("[time_msecs_to]: Returning diff = %d.", diff);
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

    // Convert each component to seconds and sum them up, ignoring milliseconds
    int from_secs = from->hour * 3600 + from->minute * 60 + from->second;
    int to_secs = to->hour * 3600 + to->minute * 60 + to->second;
    int diff = to_secs - from_secs; // Calculate the difference

    TIME_LOG("[time_secs_to]: from_secs = %d, to_secs = %d, diff = %d", from_secs, to_secs, diff);

    // Ensure the result is within the range of -86400 and 86400 seconds
    if (diff < -86400) {
        diff += 86400;
        TIME_LOG("[time_secs_to]: Adjusted diff = %d (added 86400).", diff);
    } 
    else if (diff > 86400) {
        diff -= 86400;
        TIME_LOG("[time_secs_to]: Adjusted diff = %d (subtracted 86400).", diff);
    }

    TIME_LOG("[time_secs_to]: Returning diff = %d.", diff);
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
        #include <unistd.h>
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