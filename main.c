#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Kernel Version: %s\n", sysinfo_kernel_version());

    return 0;
}