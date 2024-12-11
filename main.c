#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {

    fmt_printf("Cpu cores %d\n", sysinfo_cpu_cores());
    return 0;
}