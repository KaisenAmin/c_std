#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Pretty Product Name : %s\n", sysinfo_pretty_product_name());

    return 0;
}