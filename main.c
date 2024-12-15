#include "serial_port.h"
#include "fmt/fmt.h"

int main() {
    SerialPortInformation* ports = NULL;
    size_t count = 0;

    fmt_printf("Listing available serial ports...\n");

    if (serial_list_ports(&ports, &count) != 0) {
        fmt_printf("Failed to list serial ports.\n");
        return -1;
    }

    fmt_printf("Found %zu serial ports:\n", count);
    for (size_t i = 0; i < count; i++) {
        fmt_printf("Port: %s, Description: %s\n", ports[i].name, ports[i].description);
    }

    serial_free_ports(ports);
    fmt_printf("Freed port list memory.\n");

    return 0;
}