#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "time/time.h"

int main ()
{
    Time* test = time_current_time();

    printf("%d %d %d %d\n", test->hour, test->minute, test->second, test->msec);
    
    time_add_msecs(test, 1500000);
    printf("%d %d %d %d\n", test->hour, test->minute, test->second, test->msec);

    time_add_secs(test, 120);
    printf("%d %d %d %d\n", test->hour, test->minute, test->second, test->msec);

    printf("%d\n", time_msecs_since_start_of_day());

    Time t1 = {10, 30, 15, 500}; // 10:30:15.500
    Time t2 = {11, 45, 20, 750}; // 11:45:20.750

    int diff = time_msecs_to(&t1, &t2);
    printf("Milliseconds difference: %d\n", diff);

    char * message = time_to_string(test);
    printf("%s", message);

    free(message);
    return 0;
}