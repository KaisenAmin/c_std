#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    const char* service_name = "BITS"; 
    
    if (sysinfo_is_service_running(service_name)) {
        fmt_printf("The service '%s' is running.\n", service_name);
    } 
    else {
        fmt_printf("The service '%s' is not running.\n", service_name);
    }

    return 0;
}