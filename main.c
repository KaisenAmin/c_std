#include "serial_port.h"
#include "fmt/fmt.h"

int main() {
    if (serial_init() != 0) {
        fprintf(stderr, "Failed to initialize the serial port library.\n");
        return -1;
    }
    fprintf(stdout, "Serial port library initialized successfully.\n");

    SerialPortInformation* ports = NULL;
    size_t port_count = 0;

    // List available serial ports
    if (serial_list_ports(&ports, &port_count) != 0) {
        fprintf(stderr, "Failed to list serial ports.\n");
        serial_cleanup();
        return -1;
    }

    fprintf(stdout, "Found %zu serial ports:\n", port_count);
    for (size_t i = 0; i < port_count; i++) {
        fprintf(stdout, "Port: %s, Description: %s\n", ports[i].name, ports[i].description);
    }

    if (port_count == 0) {
        fprintf(stdout, "No serial ports found. Exiting.\n");
        serial_free_ports(ports);
        serial_cleanup();
        return 0;
    }

    // Open the first available port
    SerialPort port;
    const char* selected_port = ports[0].name;
    fprintf(stdout, "Attempting to open port: %s\n", selected_port);

    if (serial_open(selected_port, &port) != 0) {
        fprintf(stderr, "Failed to open port: %s\n", selected_port);
        serial_free_ports(ports);
        serial_cleanup();
        return -1;
    }

    fprintf(stdout, "Port %s opened successfully.\n", selected_port);

    // Configure the port
    SerialConfig config = {
        .baud_rate = 9600,
        .data_bits = 8,
        .parity = SERIAL_PARITY_NONE,
        .stop_bits = SERIAL_STOP_BITS_ONE,
        .flow_control = SERIAL_FLOW_CONTROL_NONE
    };

    if (serial_configure(&port, &config) != 0) {
        fprintf(stderr, "Failed to configure port: %s\n", selected_port);
        serial_close(&port);
        serial_free_ports(ports);
        serial_cleanup();
        return -1;
    }
    fprintf(stdout, "Port %s configured successfully.\n", selected_port);

    // Get the current configuration
    SerialConfig current_config;
    if (serial_get_config(&port, &current_config) != 0) {
        fprintf(stderr, "Failed to get configuration for port: %s\n", selected_port);
        serial_close(&port);
        serial_free_ports(ports);
        serial_cleanup();
        return -1;
    }

    fprintf(stdout, "Current configuration for %s:\n", selected_port);
    fprintf(stdout, "  Baud Rate: %d\n", current_config.baud_rate);
    fprintf(stdout, "  Data Bits: %d\n", current_config.data_bits);
    fprintf(stdout, "  Parity: %s\n", current_config.parity == SERIAL_PARITY_NONE ? "None" :
            current_config.parity == SERIAL_PARITY_ODD ? "Odd" : "Even");
    fprintf(stdout, "  Stop Bits: %s\n", current_config.stop_bits == SERIAL_STOP_BITS_ONE ? "One" : "Two");
    fprintf(stdout, "  Flow Control: %s\n", current_config.flow_control == SERIAL_FLOW_CONTROL_NONE ? "None" :
            current_config.flow_control == SERIAL_FLOW_CONTROL_HARDWARE ? "Hardware" : "Software");

    // Write data to the serial port
    const char* message = "Hello, Serial Port!";
    if (serial_write(&port, message, strlen(message)) < 0) {
        fprintf(stderr, "Failed to write data to port: %s\n", selected_port);
        serial_close(&port);
        serial_free_ports(ports);
        serial_cleanup();
        return -1;
    }
    fprintf(stdout, "Sent message: %s\n", message);

    // Read data from the serial port (blocking)
    char buffer[128] = {0};
    if (serial_read(&port, buffer, sizeof(buffer) - 1) > 0) {
        fprintf(stdout, "Received data (blocking): %s\n", buffer);
    } else {
        fprintf(stderr, "No data received (blocking).\n");
    }

    // Read data from the serial port (non-blocking)
    memset(buffer, 0, sizeof(buffer));
    if (serial_read_nonblocking(&port, buffer, sizeof(buffer) - 1) > 0) {
        fprintf(stdout, "Received data (non-blocking): %s\n", buffer);
    } else {
        fprintf(stderr, "No data received (non-blocking).\n");
    }

    // Close the serial port
    if (serial_close(&port) != 0) {
        fprintf(stderr, "Failed to close port: %s\n", selected_port);
        serial_free_ports(ports);
        serial_cleanup();
        return -1;
    }
    fprintf(stdout, "Port %s closed successfully.\n", selected_port);

    // Free resources
    serial_free_ports(ports);

    if (serial_cleanup() != 0) {
        fprintf(stderr, "Failed to clean up serial port library.\n");
        return -1;
    }
    fprintf(stdout, "Serial port library cleaned up successfully.\n");

    return 0;
}