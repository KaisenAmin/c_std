#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    Date* weekday_date = date_create_ymd(2024, 2, 29, Gregorian);
    int day_of_week = date_day_of_week(weekday_date);

    fmt_printf("Day of the week for 2024-02-29: %d (1=Monday, 7=Sunday)\n", day_of_week);

    date_deallocate(weekday_date);
    return 0;
}