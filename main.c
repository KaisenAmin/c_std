#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Boot Unique Id : %s\n", sysinfo_boot_unique_id());

    return 0;
}