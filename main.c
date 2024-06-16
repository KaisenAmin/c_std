#include "fmt/fmt.h"
#include "statistics/statistics.h"
#include <math.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>

int main() {
    char filename[100] = "hello.txt";

    if (remove(filename) != 0) {
        fprintf(stderr, "Error : %s", strerror(errno));
    }

    return 0;
}