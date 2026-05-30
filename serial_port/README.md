# SerialPort Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The SerialPort library provides a robust cross-platform interface for working with serial ports in C. It simplifies opening, configuring, reading, writing, and handling serial ports on both Windows and POSIX systems. This library is ideal for embedded systems, communication protocols, and hardware interfacing where serial communication is essential.

The library is designed to be lightweight, efficient, and easy to integrate into existing projects. It includes advanced features such as non-blocking reads, event callbacks, and comprehensive error handling.

---

## Key Features

- **Cross-Platform Support**: Compatible with Windows and POSIX systems (Linux, macOS, etc.).
- **Serial Port Listing**: Enumerates available serial ports.
- **Configurable Parameters**: Support for setting baud rate, data bits, parity, stop bits, and flow control.
- **Blocking and Non-Blocking Reads**: Options for synchronous and asynchronous data retrieval.
- **Event Handling**: Register callbacks for serial events (e.g., data received).
- **Error Handling**: Retrieve detailed error messages for diagnostics.
- **Logging**: Built-in logging for debugging and development.

---

## Installation and Compilation

To use this library, include `serial_port.h` in your project and compile the source files. Example command for GCC:

```bash
# Linux / macOS
gcc -std=c17 -O2 -Wall -Wextra -o main main.c serial_port/serial_port.c

# Windows (MSYS2 MinGW64) — requires SetupAPI for port enumeration
gcc -std=c17 -O2 -Wall -Wextra -o main main.c serial_port/serial_port.c -lsetupapi
```


### Function Descriptions

#### `int serial_init()`
- **Purpose**: Initializes the serial port library.
- **Return**: `0` on success, non-zero on failure.
- **Details**:
  - Performs platform-specific initialization.
  - Logs errors if initialization fails.

---

#### `int serial_cleanup()`
- **Purpose**: Cleans up the serial port library.
- **Return**: `0` on success, non-zero on failure.
- **Details**:
  - Frees resources and resets internal states.
  - Ensures safe cleanup in multi-session applications.

---

#### `int serial_list_ports(SerialPortInformation** ports, size_t* count)`
- **Purpose**: Lists all available serial ports.
- **Parameters**:
  - `ports`: Pointer to an array of `SerialPortInformation` structures (output).
  - `count`: Number of available ports (output).
- **Return**: `0` on success, non-zero on failure.
- **Details**:
  - Dynamically allocates memory for the port list. Must be freed using `serial_free_ports`.
  - Supports both Windows (using SetupAPI) and POSIX systems.

---

#### `void serial_free_ports(SerialPortInformation* ports)`
- **Purpose**: Frees memory allocated for the port list.
- **Parameters**:
  - `ports`: Pointer to the array of `SerialPortInformation` structures.

---

#### `int serial_open(const char* port_name, SerialPort* port)`
- **Purpose**: Opens a serial port.
- **Parameters**:
  - `port_name`: Name of the port (e.g., "COM1" or "/dev/ttyS0").
  - `port`: Pointer to a `SerialPort` structure to initialize.
- **Return**: `0` on success, non-zero on failure.

---

#### `int serial_close(SerialPort* port)`
- **Purpose**: Closes a serial port.
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
- **Return**: `0` on success, non-zero on failure.

---

#### `int serial_configure(SerialPort* port, const SerialConfig* config)`
- **Purpose**: Configures the serial port with specified parameters.
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
  - `config`: Pointer to a `SerialConfig` structure containing configuration settings.
- **Return**: `0` on success, non-zero on failure.
- **Details**:
  - Configurable parameters include baud rate, data bits, parity, stop bits, and flow control.

---

#### `int serial_get_config(SerialPort* port, SerialConfig* config)`
- **Purpose**: Retrieves the current configuration of a serial port.
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
  - `config`: Pointer to a `SerialConfig` structure to populate.
- **Return**: `0` on success, non-zero on failure.

---

#### `int serial_write(SerialPort* port, const void* data, size_t size)`
- **Purpose**: Writes data to the serial port.
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
  - `data`: Pointer to the data to write.
  - `size`: Size of the data to write.
- **Return**: Number of bytes written, or negative value on failure.

---

#### `int serial_read(SerialPort* port, void* buffer, size_t size)`
- **Purpose**: Reads data from the serial port (blocking).
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
  - `buffer`: Pointer to the buffer to store the read data.
  - `size`: Size of the buffer.
- **Return**: Number of bytes read, or negative value on failure.

---

#### `int serial_read_nonblocking(SerialPort* port, void* buffer, size_t size)`
- **Purpose**: Reads data from the serial port (non-blocking).
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
  - `buffer`: Pointer to the buffer to store the read data.
  - `size`: Size of the buffer.
- **Return**: Number of bytes read, or negative value on failure.

---

#### `int serial_set_event_callback(SerialPort* port, SerialEventCallback callback, void* user_data)`
- **Purpose**: Registers a callback function for serial port events.
- **Parameters**:
  - `port`: Pointer to the `SerialPort` structure representing the port.
  - `callback`: Function to handle serial events.
  - `user_data`: User-defined data passed to the callback.
- **Return**: `0` on success, non-zero on failure.
- **Details**:
  - Currently implemented for Windows systems. Not supported on POSIX.

---

#### `bool serial_is_open(const SerialPort* port)`
- **Purpose**: Returns `true` when `port` holds a valid OS handle and
  is safe to read from / write to. Returns `false` for NULL or a
  zero-initialised struct.

---

#### `int serial_bytes_available(SerialPort* port)`
- **Purpose**: How many bytes are sitting in the input queue right now.
  Useful for sizing the next `serial_read` exactly, or for polling
  without blocking. Returns -1 on error.

---

#### `int serial_flush_input(SerialPort* port)`
- **Purpose**: Discard everything currently in the input buffer.
  Windows: `PurgeComm(PURGE_RXCLEAR | PURGE_RXABORT)`. POSIX:
  `tcflush(TCIFLUSH)`. Returns 0 on success.

---

#### `int serial_flush_output(SerialPort* port)`
- **Purpose**: Block until the output buffer is drained. Windows:
  `FlushFileBuffers`. POSIX: `tcdrain`. Useful before closing the port
  so the last few bytes actually reach the wire.

---

#### `int serial_set_timeouts(SerialPort* port, int read_ms, int write_ms)`
- **Purpose**: Configure read and write timeouts in milliseconds. Pass
  `0` for either value to disable that timeout. On Windows this calls
  `SetCommTimeouts`; on POSIX it programs `VMIN`/`VTIME`. (POSIX
  `write_ms` is ignored — `write()` blocks until the kernel drains.)

---

## Examples

### Example 1 : open and close also init and cleanup `serial-port`

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"

int main() {
    fmt_printf("Initializing the serial port library...\n");

    if (serial_init() != 0) {
        fmt_printf("Failed to initialize the serial port library.\n");
        return -1;
    }
    fmt_printf("Serial port library initialized successfully.\n");

    const char* port_name;
#if defined(_WIN32) || defined(_WIN64)
    port_name = "COM1"; 
#else
    port_name = "/dev/ttyS0"; 
#endif

    SerialPort port;
    fmt_printf("Attempting to open the serial port: %s...\n", port_name);

    if (serial_open(port_name, &port) != 0) {
        fmt_printf("Failed to open the serial port: %s\n", port_name);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s opened successfully.\n", port_name);
    fmt_printf("Closing the serial port: %s...\n", port_name);

    if (serial_close(&port) != 0) {
        fmt_printf("Failed to close the serial port: %s\n", port_name);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s closed successfully.\n", port_name);

    fmt_printf("Cleaning up the serial port library...\n");
    if (serial_cleanup() != 0) {
        fmt_printf("Failed to clean up the serial port library.\n");
        return -1;
    }
    fmt_printf("Serial port library cleaned up successfully.\n");

    return 0;
}
```
**Result**
```
Initializing the serial port library...
Serial port library initialized successfully.
Attempting to open the serial port: COM1...
Serial port COM1 opened successfully.
Closing the serial port: COM1...
Serial port COM1 closed successfully.
Cleaning up the serial port library...
Serial port library cleaned up successfully.
```

---

### Example 2 : get list of open serial ports 

```c
#include "serial_port/serial_port.h"
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
```
**Result**
```
Listing available serial ports...
Found 2 serial ports:
Port: COM4, Description: Communications Port (COM4)
Port: COM1, Description: USB-SERIAL CH40 (COM1)
Freed port list memory.
```

---

### Example 3 : Serial Configurations 

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"

int main() {
    const char* port_name = "COM1"; 

    if (serial_init() != 0) {
        fmt_printf("Failed to initialize the serial port library.\n");
        return -1;
    }
    fmt_printf("Serial port library initialized successfully.\n");

    SerialPort port;
    SerialConfig config;

    if (serial_open(port_name, &port) != 0) {
        fmt_printf("Failed to open serial port %s.\n", port_name);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s opened successfully.\n", port_name);

    config.baud_rate = 9600;
    config.data_bits = 8;
    config.parity = SERIAL_PARITY_NONE;
    config.stop_bits = SERIAL_STOP_BITS_ONE;
    config.flow_control = SERIAL_FLOW_CONTROL_NONE;

    if (serial_configure(&port, &config) != 0) {
        fmt_printf("Failed to configure serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s configured successfully.\n", port_name);

    SerialConfig current_config;
    if (serial_get_config(&port, &current_config) != 0) {
        fmt_printf("Failed to retrieve configuration for serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }

    fmt_printf("Current configuration for %s:\n", port_name);
    fmt_printf("  Baud Rate: %d\n", current_config.baud_rate);
    fmt_printf("  Data Bits: %d\n", current_config.data_bits);
    fmt_printf("  Parity: %s\n", current_config.parity == SERIAL_PARITY_NONE ? "None" :current_config.parity == SERIAL_PARITY_ODD ? "Odd" : "Even");
    fmt_printf("  Stop Bits: %s\n", current_config.stop_bits == SERIAL_STOP_BITS_ONE ? "One" : "Two");
    fmt_printf("  Flow Control: %s\n", current_config.flow_control == SERIAL_FLOW_CONTROL_NONE ? "None" : current_config.flow_control == SERIAL_FLOW_CONTROL_HARDWARE ? "Hardware" : "Software");

    if (serial_close(&port) != 0) {
        fmt_printf("Failed to close serial port %s.\n", port_name);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s closed successfully.\n", port_name);

    if (serial_cleanup() != 0) {
        fmt_printf("Failed to cleanup the serial port library.\n");
        return -1;
    }
    fmt_printf("Serial port library cleaned up successfully.\n");

    return 0;
}
```
**Result**
```
Serial port library initialized successfully.
Serial port COM1 opened successfully.
Serial port COM1 configured successfully.
Current configuration for COM1:
  Baud Rate: 9600
  Data Bits: 8
  Parity: None
  Stop Bits: One
  Flow Control: None
Serial port COM1 closed successfully.
Serial port library cleaned up successfully.

```

---

### Example 4 : write and read over serial port 

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"
#include <string.h>

int main() {
    const char* port_name = "COM1"; 

    if (serial_init() != 0) {
        fmt_printf("Failed to initialize the serial port library.\n");
        return -1;
    }
    fmt_printf("Serial port library initialized successfully.\n");

    SerialPort port;
    SerialConfig config;

    if (serial_open(port_name, &port) != 0) {
        fmt_printf("Failed to open serial port %s.\n", port_name);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s opened successfully.\n", port_name);

    config.baud_rate = 19200;
    config.data_bits = 8;
    config.parity = SERIAL_PARITY_NONE;
    config.stop_bits = SERIAL_STOP_BITS_ONE;
    config.flow_control = SERIAL_FLOW_CONTROL_NONE;

    if (serial_configure(&port, &config) != 0) {
        fmt_printf("Failed to configure serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s configured successfully.\n", port_name);

    SerialConfig current_config;
    if (serial_get_config(&port, &current_config) != 0) {
        fmt_printf("Failed to retrieve configuration for serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }

    fmt_printf("Current configuration for %s:\n", port_name);
    fmt_printf("  Baud Rate: %d\n", current_config.baud_rate);
    fmt_printf("  Data Bits: %d\n", current_config.data_bits);
    fmt_printf("  Parity: %s\n", current_config.parity == SERIAL_PARITY_NONE ? "None" : current_config.parity == SERIAL_PARITY_ODD ? "Odd" : "Even");
    fmt_printf("  Stop Bits: %s\n", current_config.stop_bits == SERIAL_STOP_BITS_ONE ? "One" : "Two");
    fmt_printf("  Flow Control: %s\n", current_config.flow_control == SERIAL_FLOW_CONTROL_NONE ? "None" :
        current_config.flow_control == SERIAL_FLOW_CONTROL_HARDWARE ? "Hardware" : "Software");

    const char* message = "Hello, Serial Port!";
    int bytes_written = serial_write(&port, message, strlen(message));

    if (bytes_written < 0) {
        fmt_printf("Failed to write to serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Sent %d bytes: %s\n", bytes_written, message);

    char buffer[128] = {0};
    int bytes_read = serial_read(&port, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        fmt_printf("Failed to read from serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }
    buffer[bytes_read] = '\0'; 
    fmt_printf("Received %d bytes: %s\n", bytes_read, buffer);

    if (serial_close(&port) != 0) {
        fmt_printf("Failed to close serial port %s.\n", port_name);
        serial_cleanup();
        return -1;
    }
    fmt_printf("Serial port %s closed successfully.\n", port_name);

    if (serial_cleanup() != 0) {
        fmt_printf("Failed to cleanup the serial port library.\n");
        return -1;
    }
    fmt_printf("Serial port library cleaned up successfully.\n");

    return 0;
}
```
**Result**
```
Serial port library initialized successfully.
Serial port COM1 opened successfully.
Serial port COM1 configured successfully.
Current configuration for COM1:
  Baud Rate: 19200
  Data Bits: 8
  Parity: None
  Stop Bits: One
  Flow Control: None
Sent 19 bytes: Hello, Serial Port!
```

### Example 5 : none blocking read 

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"
#include <string.h>


void on_serial_event(int event, void* user_data) {
    fmt_printf("Serial event occurred: %d\n", event);
    (void)user_data;
}


int main() {
    const char* port_name = "COM4";
    SerialPort port;

    if (serial_init() != 0) {
        fmt_printf("Failed to initialize serial port library.\n");
        return -1;
    }
    if (serial_open(port_name, &port) != 0) {
        fmt_printf("Failed to open port %s.\n", port_name);
        serial_cleanup();
        return -1;
    }

    SerialConfig config = {
        .baud_rate = 9600,
        .data_bits = 8,
        .parity = SERIAL_PARITY_NONE,
        .stop_bits = SERIAL_STOP_BITS_ONE,
        .flow_control = SERIAL_FLOW_CONTROL_NONE
    };

    if (serial_configure(&port, &config) != 0) {
        fmt_printf("Failed to configure serial port %s.\n", port_name);
        serial_close(&port);
        serial_cleanup();
        return -1;
    }

    if (serial_set_event_callback(&port, on_serial_event, NULL) != 0) {
        fmt_printf("Failed to set event callback.\n");
    }

    char buffer[256] = {0};
    int bytes_read = serial_read_nonblocking(&port, buffer, sizeof(buffer));

    if (bytes_read > 0) {
        fmt_printf("Received %d bytes: %s\n", bytes_read, buffer);
    } 
    else {
        fmt_printf("No data received.\n");
    }

    const char* message = "Hello, COM1!";
    int bytes_written = serial_write(&port, message, strlen(message));
    
    if (bytes_written > 0) {
        fmt_printf("Sent %d bytes: %s\n", bytes_written, message);
    } 
    else {
        fmt_printf("Failed to send data.\n");
    }

    serial_close(&port);
    serial_cleanup();

    return 0;
}
```
**Result (when COM4 is present and no data is waiting):**
```
No data received.
```

**Result (when COM4 is present and data is waiting):**
```
Received 12 bytes: Hello, World!
Sent 12 bytes: Hello, COM1!
```

---

### Example 6 : check open state with `serial_is_open`

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    serial_init();

    SerialPort port;
    memset(&port, 0, sizeof port);
    fmt_printf("before open: is_open = %d\n", serial_is_open(&port));

#if defined(_WIN32) || defined(_WIN64)
    const char* name = "COM1";
#else
    const char* name = "/dev/ttyS0";
#endif

    if (serial_open(name, &port) == 0) {
        fmt_printf("after open:  is_open = %d\n", serial_is_open(&port));
        serial_close(&port);
        fmt_printf("after close: is_open = %d\n", serial_is_open(&port));
    } else {
        fmt_printf("open failed — port not present on this machine\n");
    }

    serial_cleanup();
    return 0;
}
```

**Sample output (port present)**

```
before open: is_open = 0
after open:  is_open = 1
after close: is_open = 0
```

---

### Example 7 : poll incoming bytes with `serial_bytes_available`

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"
#include <string.h>
#include <time.h>

int main(void) {
    serial_init();

    SerialPort port;
#if defined(_WIN32) || defined(_WIN64)
    if (serial_open("COM1", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#else
    if (serial_open("/dev/ttyS0", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#endif

    /* Wait up to 1 second for at least 1 byte to show up. */
    char buf[128];
    for (int i = 0; i < 100; ++i) {
        int n = serial_bytes_available(&port);

        if (n > 0) {
            if ((size_t)n > sizeof(buf)) {
                n = (int)sizeof(buf);
            }

            int got = serial_read(&port, buf, (size_t)n);
            fmt_printf("read %d bytes\n", got);

            break;
        }
        struct timespec ts = {.tv_sec = 0, .tv_nsec = 10 * 1000 * 1000};
        nanosleep(&ts, NULL);
    }

    serial_close(&port);
    serial_cleanup();

    return 0;
}
```

`serial_bytes_available` returns the input-queue size in O(1) without
touching the data, so polling like this never consumes a byte you
weren't planning to read.

---

### Example 8 : `serial_flush_input` before re-reading

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"

int main(void) {
    serial_init();

    SerialPort port;
#if defined(_WIN32) || defined(_WIN64)
    if (serial_open("COM1", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#else
    if (serial_open("/dev/ttyS0", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#endif

    /* Discard whatever was sitting in the buffer when we attached so
       our next read starts from a clean slate. */
    serial_flush_input(&port);
    fmt_printf("input queue cleared (bytes_available = %d)\n", serial_bytes_available(&port));

    serial_close(&port);
    serial_cleanup();

    return 0;
}
```

---

### Example 9 : drain output before closing with `serial_flush_output`

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    serial_init();

    SerialPort port;
#if defined(_WIN32) || defined(_WIN64)
    if (serial_open("COM1", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#else
    if (serial_open("/dev/ttyS0", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#endif

    const char* msg = "STATUS?\r\n";
    serial_write(&port, msg, strlen(msg));

    /* Block until the kernel has actually pushed the bytes out the
       UART before we close. Without this you can lose the tail of
       the message. */
    serial_flush_output(&port);

    serial_close(&port);
    serial_cleanup();
    return 0;
}
```

---

### Example 10 : `serial_set_timeouts` for bounded reads

```c
#include "serial_port/serial_port.h"
#include "fmt/fmt.h"

int main(void) {
    serial_init();

    SerialPort port;
#if defined(_WIN32) || defined(_WIN64)
    if (serial_open("COM1", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#else
    if (serial_open("/dev/ttyS0", &port) != 0) { 
        serial_cleanup(); 
        return 1; 
    }
#endif

    /* Cap each read at 500ms — if nothing arrives, serial_read returns 0. */
    serial_set_timeouts(&port, 500, 0);

    char buf[64];
    int n = serial_read(&port, buf, sizeof buf);
    if (n  > 0) {
        fmt_printf("got %d bytes\n", n);
    }
    else if (n == 0) {
        fmt_printf("timeout — no data within 500ms\n");
    }
    else {            
        fmt_printf("read error\n");
    }

    serial_close(&port);
    serial_cleanup();
    
    return 0;
}
```

---

## License

This project is open-source and available under [ISC License].