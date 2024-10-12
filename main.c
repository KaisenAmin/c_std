#include "vector/vector.h"
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    
    if (sysinfo_is_virtualized()) {
        fmt_printf("The system is running in a virtualized environment.\n");
    } 
    else {
        fmt_printf("The system is not virtualized.\n");
    }
    return 0;
}