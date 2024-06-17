#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Machine Unique ID: %s\n", sysinfo_machine_unique_id());

    return 0;
}