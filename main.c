#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>

int main() {
    printf("Hello World\n");

    goto first;

    printf("adfa");

    first:
    printf("This is good\n");


    return 0;
}