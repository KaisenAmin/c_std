#include "date/date.h"
#include "fmt/fmt.h"

int main() {
    long julianDayNumber = 2459580; // Example Julian Day Number
    Date* gregorianDate = date_from_julian_day(julianDayNumber);

    if (gregorianDate != NULL) {
        fmt_printf("Gregorian Date: %d-%02d-%02d\n", 
               gregorianDate->year, 
               gregorianDate->month, 
               gregorianDate->day);

        date_deallocate(gregorianDate);
    } 
    else {
        fmt_printf("Conversion failed.\n");
    }
    return 0;
}