#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("OS Product Type: %s\n", sysinfo_product_type());

    return 0;
}