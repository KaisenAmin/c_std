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

### Sysinfo Structures 

### `SysinfoNetworkInterface` Structure

- **Purpose**:  
  This structure stores information about a network interface, including its name and IP address.

- **Members**:
  - `char* interface_name`: The name of the network interface.
  - `char* ip_address`: The IP address associated with the interface.

---

### `SysinfoDiskPartition` Structure

- **Purpose**:  
  This structure holds information about a disk partition, including the mount point, total size, and free space.

- **Members**:
  - `char* mount_point`: The mount point of the partition (e.g., `/` or `C:\`).
  - `unsigned long long total_size`: The total size of the partition in bytes.
  - `unsigned long long free_space`: The available free space on the partition in bytes.


### Function Descriptions

### `Vector* sysinfo_running_services()`

- **Purpose**:  
  This function retrieves a list of currently running services on the system. It handles both Windows and Linux platforms, storing the names of the services in a dynamically allocated vector of strings (`Vector*`).
- **Parameters**:  
  - None.
- **Return Value**:  
  - A `Vector*` containing the names of the running services on the system.
  - Returns `NULL` if an error occurs (e.g., memory allocation failure).

- **Error Handling**:  
  Logs an error if the vector cannot be created or if any platform-specific functions fail.

---

### `Vector* sysinfo_process_list()`

- **Purpose**:  
  This function gathers the names of currently active processes on the system. It works on both Windows and Linux platforms and stores the process names in a dynamically allocated vector of strings.
- **Parameters**:  
  - None.
- **Return Value**:  
  - A `Vector*` containing the names of active processes.
  - Returns `NULL` if an error occurs (e.g., failure to allocate memory for the vector).
- **Error Handling**:  
  Logs errors for failure to capture process snapshots, read directories, or allocate memory.

---

### `Vector* sysinfo_network_interfaces()`

- **Purpose**:  
  This function retrieves the list of active network interfaces and their corresponding IP addresses. It handles both Windows and Linux systems, returning a vector of `SysinfoNetworkInterface` structures.
- **Parameters**:  
  - None.

- **Return Value**:  
  - A `Vector*` containing the active network interfaces and their IP addresses.
  - Returns `NULL` if an error occurs (e.g., failure to allocate memory for the vector).

- **Error Handling**:  
  Logs errors for memory allocation issues or system call failures on both platforms.

---

### `Vector* sysinfo_open_ports()`

- **Purpose**:  
  This function retrieves a list of currently open network ports on the system. It gathers information for both Windows and Linux platforms and returns a vector of integers representing the open ports.
- **Parameters**:  
  - None.
- **Return Value**:  
  - A `Vector*` containing the open network ports on the system.
  - Returns `NULL` if an error occurs (e.g., failure to allocate memory for the vector).

- **Error Handling**:  
  Logs errors if system calls fail or if there is a problem creating the vector.

---

### `Vector* sysinfo_disk_partitions()`

- **Purpose**:  
  This function retrieves information about the disk partitions on the system. The disk partitions are represented as `SysinfoDiskPartition` structures, which contain details like the mount point, total size, and free space. It handles both Windows and Linux platforms.

- **Parameters**:  
  - None.

- **Return Value**:  
  - A `Vector*` containing `SysinfoDiskPartition` structures representing the disk partitions.
  - Returns `NULL` if an error occurs or if the platform is unsupported.

- **Error Handling**:  
  Logs errors if the vector cannot be created or if system-specific functions fail.

---

### `void sysinfo_deallocate_network_interfaces(Vector* interfaces)`

- **Purpose**:  
  This function deallocates the memory used by a vector of `SysinfoNetworkInterface` structures, including the dynamically allocated `interface_name` and `ip_address` fields for each interface. It frees each individual interface and then deallocates the vector itself.
- **Parameters**:  
  - `interfaces`: A pointer to a `Vector` containing `SysinfoNetworkInterface` structures. The memory for each element in the vector is deallocated, and then the vector itself is freed.
- **Error Handling**:  
  - If `interfaces` is `NULL`, the function logs a warning and returns without performing any operations.
  - Logs details about each interface being deallocated.

---

### `void sysinfo_deallocate_disk_partitions(Vector* partitions)`

- **Purpose**:  
  This function deallocates a vector of `SysinfoDiskPartition` structures, specifically freeing the `mount_point` string for each partition and then deallocating the vector itself.

- **Parameters**:  
  - `partitions`: A pointer to a `Vector` containing `SysinfoDiskPartition` structures. The function frees the memory for each partition's `mount_point`, and then deallocates the vector.

- **Error Handling**:  
  - If `partitions` is `NULL`, the function logs a warning and returns without performing any operations.
  - Logs each partition’s deallocation process and any potential issues (e.g., `NULL` partitions).

---

### `int sysinfo_cpu_cores()`

- **Purpose**:  
  Retrieves the number of CPU cores available on the system. It uses platform-specific implementations for Windows and Linux to return the total number of CPU cores.

- **Parameters**:  
  None.

- **Return Value**:  
  - Returns the total number of CPU cores available on the system.
  - Returns `-1` if an error occurs (e.g., failure to retrieve core count due to system call issues).

---

### `bool sysinfo_is_virtualized()`

- **Purpose**:  
  Checks if the system is running in a virtualized environment by detecting specific processor features that are indicative of virtualization technology. It provides platform-specific checks for virtualization status (e.g., Hyper-V on Windows).

- **Parameters**:  
  None.

- **Return Value**:  
  - `true`: If the system is detected to be running in a virtualized environment (using `PF_VIRT_FIRMWARE_ENABLED` or `PF_HYPERVISOR_PRESENT` on Windows).
  - `false`: If the system is not running in a virtualized environment.

---

### `bool sysinfo_is_service_running(const char* service_name)`

- **Purpose**:  
  This function checks whether a specific service or process, given by `service_name`, is currently running on the system. It supports both Windows and Linux systems.

- **Parameters**:  
  - `service_name`: The name of the service or process to check.

- **Return Value**:  
  - `true`: If the service or process is running.
  - `false`: If the service or process is not running.

---

### `double sysinfo_cpu_usage()`

- **Purpose**:  
  This function calculates the current CPU usage percentage by comparing the CPU time spent performing work (in user mode and kernel mode) to the idle time. It provides an overview of how much of the CPU's capacity is being utilized at the moment.

- **Parameters**:  
  None.

- **Return Value**:  
  - The CPU usage as a percentage (`double`).
  - `-1.0`: If an error occurs (e.g., failure to retrieve CPU usage data from the system).

- **Usage Example**:  
  Use this function to monitor or log real-time CPU usage for performance tracking or system diagnostics.

---

### `double sysinfo_memory_usage()`

- **Purpose**:  
  This function calculates the current memory usage percentage based on the total available memory and the memory currently in use by the system. It provides insight into how much of the system's memory resources are being consumed.

- **Parameters**:  
  None.

- **Return Value**:  
  - The memory usage as a percentage (`double`).
  - `-1.0`: If an error occurs (e.g., failure to retrieve memory usage data from the system).

---

### `char* sysinfo_product_version()`

- **Purpose**:  
  This function retrieves the version of the operating system, providing key information about the specific release version of the OS ( Windows 10, Ubuntu 20.04). It helps identify the system's OS version for compatibility checks or system profiling.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the OS version (e.g., `"Windows 10"`, `"Ubuntu 20.04"`).
  - `"unknown"`: If the version cannot be determined.

---

### `char* sysinfo_product_type()`

- **Purpose**:  
  This function retrieves the product type or name of the operating system (e.g., `"Windows"`, `"Ubuntu"`). It identifies the specific OS to help determine system compatibility or environment details.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the OS product type.
  - `"unknown"`: If the product type cannot be determined.

---

### `char* sysinfo_kernel_version()`

- **Purpose**:  
  This function retrieves the kernel version of the operating system. The kernel version gives detailed information about the OS, which is useful for diagnostics, debugging, or ensuring compatibility (e.g., `"10.0.19045"` on Windows, `"6.8.0-35-generic"` on Linux).

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the kernel version.
  - `"unknown"`: If the kernel version cannot be determined.

---

### `char* sysinfo_kernel_type()`

- **Purpose**:  
  This function returns the type of the operating system kernel, providing a high-level classification (e.g., `"Windows NT"` for Windows systems or `"Linux"` for Linux-based systems).

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the kernel type.
  - `"unknown"`: If the kernel type cannot be determined.

---

### `char* sysinfo_boot_unique_id()`

- **Purpose**:  
  This function generates a unique identifier representing the current boot session of the machine. It is useful for tracking the current boot session or for session-specific operations.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the unique boot ID.
  - `"unknown"`: If the boot ID cannot be determined.

---

### `char* sysinfo_cpu_architecture()`

- **Purpose**:  
  This function detects and returns the CPU architecture of the system (e.g., `"x86_64"`, `"ARM"`, `"IA64"`). It helps to identify the processor type in use, useful for compatibility and system profiling.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the CPU architecture.
  - `"unknown"`: If the architecture cannot be determined.

---

### `char* sysinfo_machine_host_name()`

- **Purpose**:  
  This function retrieves the host name or fully qualified domain name (FQDN) of the machine. It is useful for network-related tasks where the machine's identity is required.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the machine's host name.
  - `"unknown"`: If the host name cannot be determined.

---

### `char* sysinfo_machine_unique_id()`

- **Purpose**:  
  This function provides a unique identifier for the machine, typically based on hardware properties or a system-specific identifier. It is useful for ensuring persistent identification of the machine across reboots.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the machine's unique ID.
  - `"unknown"`: If the ID cannot be determined.

---

### `char* sysinfo_pretty_product_name()`

- **Purpose**:  
  This function returns a human-readable string that combines the operating system’s product name and version in a user-friendly format, giving more intuitive system information.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the pretty OS product name.

---

### `char* sysinfo_build_abi()`

- **Purpose**:  
  This function returns information about the system's Application Binary Interface (ABI), including architecture, endianness, and data model. It is useful for determining system capabilities and compatibility with certain applications.

- **Parameters**:  
  None.

- **Return Value**:  
  - A static string containing the ABI details.
  - `"unknown"`: If the ABI cannot be determined.

---

### `char* sysinfo_system_uptime()`

- **Purpose**:  
  This function returns the system's uptime (time since last boot) in a human-readable format (e.g., "1 day, 2 hours, 30 minutes"). It is useful for system monitoring and diagnostics.

- **Parameters**:  
  None.

- **Return Value**:  
  - A dynamically allocated string containing the system uptime in days, hours, minutes, and seconds. The caller must free the memory.
  - `NULL`: If the uptime cannot be determined.

---

### `char* sysinfo_system_locale()`

- **Purpose**:  
  This function retrieves the system's current locale setting, useful for adapting applications to the user’s language and region preferences.

- **Parameters**:  
  None.

- **Return Value**:  
  - A dynamically allocated string containing the system locale. The caller must free the returned string.
  - `"unknown"`: If the locale cannot be determined.

---

### `char* sysinfo_disk_space(const char* path)`

- **Purpose**:  
  This function returns information about the total, used, and available disk space for the given path on the system. It is useful for disk space monitoring and system diagnostics.

- **Parameters**:  
  - `path`: A string representing the file path for which to check disk space.

- **Return Value**:  
  - A string containing the total, used, and available disk space in MB. The caller must free the memory.
  - `NULL`: If the disk space cannot be determined.

---

### `char** sysinfo_list_bluetooth_devices(int* count)`

- **Purpose**:  
  This function scans for Bluetooth devices and returns a list of names for the devices currently connected or remembered by the system.

- **Parameters**:  
  - `count`: A pointer to an integer where the function will store the number of found Bluetooth devices.

- **Return Value**:  
  - An array of strings representing Bluetooth device names. The caller must free each string and the array.
  - `NULL`: If no devices are found or the function fails.


---


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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

### Example 10: List Bluetooth Devices

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    int count = 0;
    char** devices = sysinfo_list_bluetooth_devices(&count);

    fmt_printf("Found %d Bluetooth devices:\n", count);
    for (int i = 0; i < count; i++) {
        fmt_printf("%s\n", devices[i]);
        free(devices[i]);
    }

    free(devices);
    return 0;
}
```

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

## License

This project is open-source and available under [ISC License].
