#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    fmt_printf("Current Time in seconds %f\n", time_current_time_in_seconds());
    fmt_printf("Current Time in msec %f\n", time_current_time_in_microseconds());

    return 0;
}