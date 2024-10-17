# SysInfo Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The SysInfo library provides a set of functions for retrieving detailed information about the operating system and hardware of a machine. It is designed to work on both Windows and Linux systems, though it is not tested on macOS. The library is lightweight and easy to integrate into C projects, offering developers essential system details such as OS version, kernel type, CPU architecture, and more.

## Installation and Compilation

To use this library, include `sysinfo.h` in your project and compile the source files with your C compiler. Here is an example using GCC:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./sysinfo/sysinfo.c
```

## Documentation

The SysInfo library offers a variety of functions to gather information about the system it is running on. Below is a detailed description of each function provided by the library.

### Function Descriptions

- **`char* sysinfo_product_version()`**: 
  - Returns the version of the operating system as a string. If the version cannot be determined, it returns `"unknown"`.
  - **Example (Linux)**: `"24.04"`
  - **Example (Windows)**: `"10.0"`

- **`char* sysinfo_product_type()`**: 
  - Returns the product name of the operating system. On Linux, it retrieves the distribution name; on Windows, it returns `"Windows"`.
  - **Example (Linux)**: `"Ubuntu"`
  - **Example (Windows)**: `"Windows"`

- **`char* sysinfo_kernel_version()`**: 
  - Returns the version of the operating system kernel. This is equivalent to the output of `uname -r` on Linux.
  - **Example (Linux)**: `"6.8.0-35-generic"`
  - **Example (Windows)**: `"10.0.19045"`

- **`char* sysinfo_kernel_type()`**: 
  - Returns the type of the kernel, such as `"Linux"` or `"Windows NT"`. This corresponds to the output of `uname -s` on Unix-like systems.
  - **Example (Linux)**: `"Linux"`
  - **Example (Windows)**: `"Windows NT"`

- **`char* sysinfo_boot_unique_id()`**: 
  - Returns a unique ID for the current boot session. On Linux, this ID is read from `/proc/sys/kernel/random/boot_id`. On Windows, it is derived from the system uptime using `GetTickCount64()`.
  - **Example (Linux)**: `"bba30543-fea2-4477-b9e4-5343f8746200"`
  - **Example (Windows)**: `"22066640"`

- **`char* sysinfo_cpu_architecture()`**: 
  - Returns the CPU architecture of the system. This could be values like `"x86_64"`, `"ARM"`, etc.
  - **Example (Linux/Windows)**: `"x86_64"`

- **`char* sysinfo_machine_host_name()`**: 
  - Returns the fully qualified domain name (FQDN) of the machine if available. If no FQDN is available, it returns the hostname.
  - **Example (Linux)**: `"kaisen-VirtualBox"`
  - **Example (Windows)**: `"DESKTOP-DF2A2F9"`

- **`char* sysinfo_machine_unique_id()`**: 
  - Returns a unique identifier for the machine. This ID is persistent across reboots and is usually derived from `/etc/machine-id` on Linux or the Windows registry.
  - **Example (Linux)**: `"4229c1dd3d8c41e1a7b9e1102912cde3"`
  - **Example (Windows)**: `"8ba376a1-9832-45a0-b7bd-5d507e7a1d68"`

- **`char* sysinfo_pretty_product_name()`**: 
  - Returns a user-friendly string representing the operating system name and version.
  - **Example (Linux)**: `"Ubuntu 24.04"`
  - **Example (Windows)**: `"Windows 10 Version 10.0 (Build 19045)"`

- **`char** sysinfo_list_bluetooth_devices(int* count)`**: 
  - Returns a list of Bluetooth devices currently connected or known to the system. The number of devices is returned via the `count` parameter. This function is not guaranteed to work on all systems and is primarily implemented for Linux and Windows.
  - **Example (Linux/Windows)**: List of Bluetooth device names.

- **`double sysinfo_cpu_usage()`**:
  - Retrieves the current CPU usage percentage of the system.

- **`double sysinfo_memory_usage()`**:
  - Retrieves the current memory usage percentage of the system.

- **`char* sysinfo_disk_space(const char* path)`**:
  - Retrieves the disk space information for a given path.

- **`char* sysinfo_system_uptime()`**:
  - Retrieves the system's uptime since the last boot.

- **`int sysinfo_cpu_cores()`**:
  - Retrieves the number of CPU cores available on the system.

- **`Vector* sysinfo_process_list()`**:
  - Retrieves a list of active processes on the system.

- **`Vector* sysinfo_running_services()`**:
  - Retrieves the list of currently running services on the system.

- **`Vector* sysinfo_network_interfaces()`**:
  - Retrieves the list of active network interfaces along with their IP addresses.

- **`void sysinfo_deallocate_network_interfaces(Vector* interfaces)`** :
  - Deallocates the memory used by a vector of SysinfoNetworkInterface structures.

- **`void sysinfo_deallocate_disk_partitions(Vector* partitions)`**:
  - Deallocates the memory used by a vector of SysinfoDiskPartition structures.

- **`Vector* sysinfo_open_ports()`**: 
  - Retrieves a list of open network ports on the system return all tcp and udp listening ports.

- **`Vector* sysinfo_disk_partitions()`**:
  - Retrieves a list of mounted disk partitions on the system.

- **`bool sysinfo_is_virtualized()`**:
  - Checks if the system is running in a virtualized environment.

- **`char* sysinfo_system_locale()`**:
  - Retrieves the current system locale as a string.

- **`bool sysinfo_is_service_running(const char*)`**:
  - Checks if a specific service or process is running.

## Examples

The following examples demonstrate how to use the SysInfo library in various scenarios.

### Example 1: Get OS Version

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("OS Version: %s\n", sysinfo_product_version());
    return 0;
}
```

**Output on Linux:**
```
OS Version: 24.04
```

**Output on Windows:**
```
OS Version: 10.0
```

### Example 2: Get Product Type

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("OS Product Type: %s\n", sysinfo_product_type());
    return 0;
}
```

**Output on Linux:**
```
OS Product Type: Ubuntu
```

**Output on Windows:**
```
OS Product Type: Windows
```

### Example 3: Get Kernel Version

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Kernel Version: %s\n", sysinfo_kernel_version());
    return 0;
}
```

**Output on Linux:**
```
Kernel Version: 6.8.0-35-generic
```

**Output on Windows:**
```
Kernel Version: 10.0.19045
```

### Example 4: Get Kernel Type

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Kernel Type: %s\n", sysinfo_kernel_type());
    return 0;
}
```

**Output on Linux:**
```
Kernel Type: Linux
```

**Output on Windows:**
```
Kernel Type: Windows NT
```

### Example 5: Get Boot Unique ID

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Boot Unique ID: %s\n", sysinfo_boot_unique_id());
    return 0;
}
```

**Output on Linux:**
```
Boot Unique ID: bba30543-fea2-4477-b9e4-5343f8746200
```

**Output on Windows:**
```
Boot Unique ID: 22066640
```

### Example 6: Get CPU Architecture

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("CPU Architecture: %s\n", sysinfo_cpu_architecture());
    return 0;
}
```

**Output on Linux/Windows:**
```
CPU Architecture: x86_64
```

### Example 7: Get Machine Host Name

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Machine Host Name: %s\n", sysinfo_machine_host_name());
    return 0;
}
```

**Output on Linux:**
```
Machine Host Name: kaisen-VirtualBox
```

**Output on Windows:**
```
Machine Host Name: DESKTOP-DF2A2F9
```

### Example 8: Get Machine Unique ID

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Machine Unique ID: %s\n", sysinfo_machine_unique_id());
    return 0;
}
```

**Output on Linux:**
```
Machine Unique ID: 4229c1dd3d8c41e1a7b9e1102912cde3
```

**Output on Windows:**
```
Machine Unique ID: 8ba376a1-9832-45a0-b7bd-5d507e7a1d68
```

### Example 9: Get Pretty Product Name

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Pretty Product Name: %s\n", sysinfo_pretty_product_name());
    return 0;
}
```

**Output on Linux:**
```
Pretty Product Name: Ubuntu 24.04
```

**Output on Windows:**
```
Pretty Product Name: Windows 10 Version 10.0 (Build 19045)
```

### Example 10: List Bluetooth Devices

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    int count = 0;
    char** devices = sysinfo_list_bluetooth_devices(&count);

    fmt_printf("Found %d Bluetooth devices:\n", count);
    for

 (int i = 0; i < count; i++) {
        fmt_printf("%s\n", devices[i]);
        free(devices[i]);
    }

    free(devices);
    return 0;
}
```

## Example 11 : get cpu usage with `sysinfo_cpu_usage`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    double cpuUsage = sysinfo_cpu_usage();

    if (cpuUsage >= 0) {
        fmt_printf("CPU Usage: %.2f%%\n", cpuUsage);
    } 
    else {
        fmt_printf("Failed to retrieve CPU usage.\n");
    }
    
    return 0;
}
```

**Output on Linux:**
```
CPU Usage: 5.60%
```

**Output on Windows:**
```
CPU Usage: 3.64%
```

## Example 12 : get memory usage percentage with `sysinfo_memory_usage`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    double memoryUsage = sysinfo_memory_usage();

    if (memoryUsage >= 0) {
        fmt_printf("Memory Usage: %.2f%%\n", memoryUsage);
    } 
    else {
        fmt_printf("Failed to retrieve memory usage.\n");
    }

    return 0;
}
```

**Output on Linux:**
```
Memory Usage: 55.60%
```

**Output on Windows:**
```
Memory Usage: 49.64%
```

## Example 13 : get disk space and avalible with `sysinfo_disk_space(const char* path)`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    const char* path = "C:\\";
    char* diskSpaceInfo = sysinfo_disk_space(path);

    if (diskSpaceInfo != NULL) {
        fmt_printf("Disk Space Info for %s: %s\n", path, diskSpaceInfo);
        free(diskSpaceInfo);
    } 
    else {
        fmt_printf("Failed to retrieve disk space information for %s\n", path);
    }

    return 0;
}
```

**Output on Linux:**
```
Disk Space Info for C:\: Total: 594597 MB, Used: 541148 MB, Available: 53449 MB
```

**Output on Windows:**
```
Disk Space Info for /: Total: 59459 MB, Used: 54114 MB, Available: 5344 MB
```

## Example 14 : get uptime of os with `sysinfo_system_uptime()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    char* uptime = sysinfo_system_uptime();

    if (uptime != NULL) {
        fmt_printf("System Uptime: %s\n", uptime);
        free(uptime);
    } 
    else {
        fmt_printf("Failed to retrieve system uptime.\n");
    }

    return 0;
}
```

**Output on Linux:**
```
System Uptime: 0 days, 0 hours, 29 minutes, 26 seconds
```

**Output on Windows:**
```
System Uptime: 6 days, 2 hours, 29 minutes, 26 seconds
```

## Example 15 : get list of running services as Vector in `Vector* sysinfo_running_services()`

```c
#include "sysinfo/sysinfo.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Vector *runningServices = sysinfo_running_services();

    for (size_t i = 0; i < vector_size(runningServices); ++i) {
        char **servicePtr = (char **)vector_at(runningServices, i);
        fmt_printf("Running service: %s\n", *servicePtr);

        free(*servicePtr); 
    }

    vector_deallocate(runningServices); 
    return 0;
}
```

## Example 16 : get number of cpu cores `int sysinfo_cpu_cores()`

```c
#include "sysinfo/sysinfo.h"
#include "fmt/fmt.h"

int main() {
    int cpu_cores = sysinfo_cpu_cores();

    if (cpu_cores != -1) {
        fmt_printf("Number of CPU cores: %d\n", cpu_cores);
    } 
    else {
        fmt_printf("Error retrieving the number of CPU cores.\n");
    }

    return 0;
}
```

## Example 17 : get list of process with `sysinfo_process_list()`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    Vector* processes = sysinfo_process_list();

    for (size_t i = 0; i < vector_size(processes); ++i) {
        char** processPtr = (char**)vector_at(processes, i);
        fmt_printf("Process: %s\n", *processPtr);

        free(*processPtr); 
    }

    vector_deallocate(processes); 
    return 0;
}
```

## Example 18 : get list of network interface with `sysinfo_network_interfaces()` also deallocate with `sysinfo_deallocate_network_interfaces()`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    Vector* interfaces = sysinfo_network_interfaces();

    for (size_t i = 0; i < vector_size(interfaces); ++i) {
        SysinfoNetworkInterface* iface = (SysinfoNetworkInterface*)vector_at(interfaces, i);
        fmt_printf("Interface: %s, IP Address: %s\n", iface->interface_name, iface->ip_address);
    }

    sysinfo_deallocate_network_interfaces(interfaces);
    return 0;
}
```

## Example 19 : get list of open listening ports with `sysinfo_open_ports()`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    Vector *openPorts = sysinfo_open_ports();

    fmt_printf("List of Open Ports:\n");
    
    for (size_t i = 0; i < vector_size(openPorts); ++i) {
        int **portPtr = (int **)vector_at(openPorts, i);
        fmt_printf("Port: %d\n", **portPtr);

        free(*portPtr);  
    }

    vector_deallocate(openPorts);
    return 0;
}
```

## Example 20 : check if system running or virtualized env or not with `sysinfo_is_vitrualized()`

```c
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    
    if (sysinfo_is_virtualized()) {
        fmt_printf("The system is running in a virtualized environment.\n");
    } 
    else {
        fmt_printf("The system is not virtualized.\n");
    }
    return 0;
}
```

## Example 21 : get system locale with `sysinfo_system_locale()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    char* locale = sysinfo_system_locale();
    fmt_printf("System Locale: %s\n", locale);

    free(locale);  
    return 0;
}
```

## Example 22 : check is service running or not with `sysinfo_is_service_running(const char*)`:

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    const char* service_name = "BITS"; 
    
    if (sysinfo_is_service_running(service_name)) {
        fmt_printf("The service '%s' is running.\n", service_name);
    } 
    else {
        fmt_printf("The service '%s' is not running.\n", service_name);
    }

    return 0;
}
```

## Example 23 : get list of partitions as Vector* with `sysinfo_disk_partitions()`

```c
#include "fmt/fmt.h"
#include "vector/vector.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    Vector* partitions = sysinfo_disk_partitions();

    for (size_t i = 0; i < vector_size(partitions); ++i) {
        SysinfoDiskPartition* partition = (SysinfoDiskPartition*)vector_at(partitions, i);
        fmt_printf("Mount point: %s, Total size: %llu MB, Free space: %llu MB\n",
            partition->mount_point,
            partition->total_size / (1024 * 1024),
            partition->free_space / (1024 * 1024));
    }

    sysinfo_deallocate_disk_partitions(partitions);
    return 0;
}
```
