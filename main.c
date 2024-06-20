#include "fmt/fmt.h"
#include "time/time.h"
#include <time.h>


int main() {
    Time* start_time = time_current_time();

    time_sleep(10);

    Time* end_time = time_current_time();
    double diff_time = time_diff_in_seconds(start_time, end_time);

    fmt_printf("Difference in Seconds : %lf", diff_time);

    time_deallocate(start_time);
    time_deallocate(end_time);

    return 0;
}