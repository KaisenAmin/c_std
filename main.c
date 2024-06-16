#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Machine Host Name is : %s\n", sysinfo_machine_host_name());

    return 0;
}