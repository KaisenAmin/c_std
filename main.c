#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    double cpuUsage = sysinfo_cpu_usage();
    if (cpuUsage >= 0) {
        fmt_printf("CPU Usage: %.2f%%\n", cpuUsage);
    } 
    else {
        fmt_printf("Failed to retrieve CPU usage.\n");
    }
    
    return 0;
}
