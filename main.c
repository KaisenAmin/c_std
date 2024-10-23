#include <stdlib.h>
#include "fmt/fmt.h"
#include "time/std_time.h"


int main () {
    Time* t1 = time_create(14, 30, 45, 500);
    char* str1 = time_to_string(t1);
    
    fmt_printf("Time create and to string: %s\n", str1);

    free(str1);
    time_deallocate(t1);
    
    return 0;
}