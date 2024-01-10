#include <stdio.h>
#include <time.h>
#include "time/time.h"

int main ()
{
    Time * test = time_current_time();

    printf("%d %d %d %d", test->hour, test->minute, test->second, test->msec);
  
    return 0;
}