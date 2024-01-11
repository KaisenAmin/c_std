#include "date/date.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Create a new date: 15th March 2021
    Date* date = date_create_ymd(2021, 3, 15);

    printf("Created date: %d-%02d-%02d\n", date->year, date->month, date->day);

    Date* new_date = date_add_days(date, 30);
    printf("New date after adding 30 days: %d-%02d-%02d\n", new_date->year, new_date->month, new_date->day);

    free(date);
    free(new_date);
    return 0;
}
