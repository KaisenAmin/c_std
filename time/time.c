#include "time.h"
#include <time.h>
#include <windows.h>
#include <stdlib.h>


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