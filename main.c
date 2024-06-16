#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Os Version : %s\n", sysinfo_product_version());

    return 0;
}