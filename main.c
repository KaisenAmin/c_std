#include "date/date.h"
#include <stdio.h>
#include <stdlib.h>
int main() {
    long julianDayNumber = 2459580; // Example Julian Day Number
    Date* gregorianDate = date_from_julian_day(julianDayNumber);

    if (gregorianDate != NULL) {
        printf("Gregorian Date: %d-%02d-%02d\n", 
               gregorianDate->year, 
               gregorianDate->month, 
               gregorianDate->day);
        free(gregorianDate);
    } 
    else {
        printf("Conversion failed.\n");
    }

    return 0;
}