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

## Extended Cross-Platform Utility API

These ten helpers were added in the production-hardening pass. Each one is implemented for both Windows and Linux, with a safe fallback on other platforms.

### `char* sysinfo_username(void)`

- **Purpose**:  
  Returns the login name of the user who started the current process.
- **Parameters**: None.
- **Return Value**:  
  - A heap-allocated, null-terminated string the caller must `free()`.
  - Never returns `NULL`. Falls back to `"unknown"` if no source resolves a name.
- **Behaviour**:  
  - **Windows**: `GetUserNameA` into a `UNLEN+1` buffer.
  - **POSIX**: `getlogin_r`, then `getpwuid(geteuid())->pw_name`, then `$USER` / `$LOGNAME`.

---

### `char* sysinfo_home_directory(void)`

- **Purpose**:  
  Returns the home directory of the current user.
- **Parameters**: None.
- **Return Value**:  
  - A heap-allocated, null-terminated string the caller must `free()`.
  - Never `NULL`. Returns an empty string if nothing is available.
- **Behaviour**:  
  - **Windows**: `$USERPROFILE`, then `$HOMEDRIVE + $HOMEPATH`.
  - **POSIX**: `$HOME`, then `getpwuid(geteuid())->pw_dir`.

---

### `char* sysinfo_temp_directory(void)`

- **Purpose**:  
  Returns the system's default directory for temporary files.
- **Parameters**: None.
- **Return Value**:  
  - A heap-allocated, null-terminated string the caller must `free()`.
  - Never `NULL`. Trailing path separator is always trimmed, so the caller can safely join `<tmp> + "/" + name` without a double slash.
- **Behaviour**:  
  - **Windows**: `GetTempPathA` (trailing `\\` stripped).
  - **POSIX**: `$TMPDIR`, falling back to `/tmp`.

---

### `char* sysinfo_current_working_directory(void)`

- **Purpose**:  
  Returns the current working directory of the calling process.
- **Parameters**: None.
- **Return Value**:  
  - A heap-allocated, null-terminated string the caller must `free()`.
  - `NULL` only on unrecoverable I/O error.
- **Behaviour**:  
  Uses a buffer that doubles in size on overflow, so paths longer than `PATH_MAX` (legal on some POSIX systems) are still captured.

---

### `char* sysinfo_executable_path(void)`

- **Purpose**:  
  Returns the absolute path of the running executable.
- **Parameters**: None.
- **Return Value**:  
  - A heap-allocated, null-terminated string the caller must `free()`.
  - `NULL` if the OS cannot answer (e.g. macOS, where the library does not pull in `_NSGetExecutablePath`).
- **Behaviour**:  
  - **Windows**: `GetModuleFileNameA(NULL, ...)`.
  - **Linux**: `readlink("/proc/self/exe")`.

---

### `long sysinfo_process_id(void)`

- **Purpose**:  
  Returns the PID of the current process.
- **Parameters**: None.
- **Return Value**:  
  - `long` so it accommodates both Windows DWORDs and POSIX `pid_t`.
  - `-1` on the unsupported-platform path. Never fails on Windows or POSIX.
- **Behaviour**:  
  - **Windows**: `GetCurrentProcessId`.
  - **POSIX**: `getpid`.

---

### `unsigned long long sysinfo_total_memory_bytes(void)`

- **Purpose**:  
  Returns the total physical memory of the host in bytes.
- **Parameters**: None.
- **Return Value**:  
  - Unsigned byte count.
  - `0` if the OS cannot answer or the platform is unsupported.
- **Behaviour**:  
  - **Windows**: `GlobalMemoryStatusEx(ullTotalPhys)`.
  - **Linux**: parses `MemTotal` from `/proc/meminfo`.

---

### `unsigned long long sysinfo_available_memory_bytes(void)`

- **Purpose**:  
  Returns the amount of physical memory currently available, in bytes.
- **Parameters**: None.
- **Return Value**:  
  - Unsigned byte count.
  - `0` if the OS cannot answer.
- **Behaviour**:  
  - **Windows**: `GlobalMemoryStatusEx(ullAvailPhys)`.
  - **Linux**: prefers `MemAvailable` from `/proc/meminfo`; falls back to `MemFree` for kernels older than 3.14 that do not expose `MemAvailable`.

---

### `bool sysinfo_is_64bit_process(void)`

- **Purpose**:  
  Returns `true` if the current process is a 64-bit binary, `false` if it is 32-bit.
- **Parameters**: None.
- **Return Value**:  
  - `bool` — fully determined by `sizeof(void*) == 8`.
- **Notes**:  
  This describes the calling *process*, not the host OS. A 32-bit binary running on 64-bit Windows reports `false`, which is what most callers (release gating, structure layout decisions) actually want.

---

### `const char* sysinfo_endianness(void)`

- **Purpose**:  
  Returns the host CPU's byte order as a static string.
- **Parameters**: None.
- **Return Value**:  
  - One of `"little"`, `"big"`, or `"unknown"`. The returned pointer is a static literal — **do not free it**.
- **Behaviour**:  
  Runtime probe over the byte layout of a `uint16_t`. Independent of compiler macros such as `__BYTE_ORDER__`, so it works the same under any toolchain.

---

## Additional Observers

These ten functions extend the numeric/predicate API. All are cross-platform (Windows and Linux), allocation-free except where noted, and fast (a single syscall or `/proc` read).

### `unsigned long long sysinfo_used_memory_bytes(void)`

- **Purpose**:  
  Returns the amount of physical memory currently in use, in bytes.
- **Parameters**: None.
- **Return Value**:  
  - Unsigned byte count, equal to `sysinfo_total_memory_bytes() - sysinfo_available_memory_bytes()`.
  - `0` if either source returns 0 or the values are inconsistent.
- **Behaviour**:  
  Convenience wrapper over the existing total/available observers, so it is automatically correct on every platform those two support.

---

### `unsigned long long sysinfo_total_swap_bytes(void)`

- **Purpose**:  
  Returns the total size of swap space, in bytes.
- **Parameters**: None.
- **Return Value**:  
  - Unsigned byte count. `0` is legitimate (swapless systems / containers) and is also returned on error or unsupported platforms.
- **Behaviour**:  
  - **Windows**: there is no separate "swap"; the page file is the equivalent. Reported as `ullTotalPageFile - ullTotalPhys` (commit limit minus physical RAM).
  - **Linux**: parses `SwapTotal` from `/proc/meminfo`.

---

### `unsigned long long sysinfo_free_swap_bytes(void)`

- **Purpose**:  
  Returns the amount of free swap space, in bytes.
- **Parameters**: None.
- **Return Value**:  
  - Unsigned byte count, always `<= sysinfo_total_swap_bytes()`. `0` on error / unsupported platforms.
- **Behaviour**:  
  - **Windows**: `ullAvailPageFile` (remaining commit the system will grant) capped at the page-file size — large when little is committed, shrinking toward 0 as commit nears the limit.
  - **Linux**: parses `SwapFree` from `/proc/meminfo`.

---

### `long sysinfo_uptime_seconds(void)`

- **Purpose**:  
  Returns the system uptime in whole seconds (the numeric counterpart of `sysinfo_system_uptime()`).
- **Parameters**: None.
- **Return Value**:  
  - Seconds since boot, or `-1` if it cannot be determined.
- **Behaviour**:  
  - **Windows**: `GetTickCount64() / 1000`.
  - **Linux**: the first field of `/proc/uptime`.

---

### `long sysinfo_process_count(void)`

- **Purpose**:  
  Returns the number of processes currently running on the system.
- **Parameters**: None.
- **Return Value**:  
  - Process count (`>= 1`), or `-1` on failure.
- **Behaviour**:  
  - **Windows**: counts entries from a `CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS)`.
  - **Linux**: counts the numeric (PID) directories under `/proc`.

---

### `bool sysinfo_is_admin(void)`

- **Purpose**:  
  Returns `true` if the current process is running with administrative / root privileges.
- **Parameters**: None.
- **Return Value**:  
  - `true` when elevated/root, otherwise `false` (also `false` if privileges cannot be queried or the platform is unsupported).
- **Behaviour**:  
  - **Windows**: queries the process token's `TokenElevation` — true only when actually running elevated.
  - **POSIX**: `geteuid() == 0`.

---

### `unsigned long long sysinfo_disk_total_bytes(const char* path)`

- **Purpose**:  
  Returns the total size, in bytes, of the filesystem that contains `path` (the numeric counterpart of `sysinfo_disk_space()`).
- **Parameters**:  
  - `path`: any path on the volume of interest (e.g. `"/"` on Linux, `"C:\\"` on Windows).
- **Return Value**:  
  - Total bytes, or `0` on a `NULL`/empty path, on error, or on unsupported platforms.
- **Behaviour**:  
  - **Windows**: `GetDiskFreeSpaceExA` (total number of bytes).
  - **Linux**: `statvfs` (`f_blocks * f_frsize`).

---

### `unsigned long long sysinfo_disk_free_bytes(const char* path)`

- **Purpose**:  
  Returns the free space, in bytes, available to the caller on the filesystem that contains `path`.
- **Parameters**:  
  - `path`: any path on the volume of interest.
- **Return Value**:  
  - Free bytes available to an unprivileged caller (honours filesystem reservations), or `0` on a `NULL`/empty path, on error, or on unsupported platforms.
- **Behaviour**:  
  - **Windows**: `GetDiskFreeSpaceExA` (free bytes available to caller).
  - **Linux**: `statvfs` (`f_bavail * f_frsize`).

---

### `int sysinfo_battery_percentage(void)`

- **Purpose**:  
  Returns the battery charge level as a whole percentage (`0..100`).
- **Parameters**: None.
- **Return Value**:  
  - `0..100` when a battery is present, or `-1` when there is no battery (desktops, servers, most VMs/CI) or the level is unknown. Treat `-1` as "not applicable".
- **Behaviour**:  
  - **Windows**: `GetSystemPowerStatus` (`BatteryLifePercent`; 255 means unknown → `-1`).
  - **Linux**: reads `/sys/class/power_supply/BAT0|BAT1/capacity`.

---

### `char* sysinfo_default_shell(void)`

- **Purpose**:  
  Returns the path of the user's default shell / command interpreter.
- **Parameters**: None.
- **Return Value**:  
  - Heap-allocated string — **the caller must `free()` it**. Always non-NULL except on allocation failure.
- **Behaviour**:  
  - **Windows**: the `COMSPEC` environment variable (falls back to `C:\\Windows\\System32\\cmd.exe`).
  - **POSIX**: the `SHELL` environment variable (falls back to `/bin/sh`).

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

## Example 24: get the current user name with `sysinfo_username()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    char* user = sysinfo_username();
    fmt_printf("Hello, %s!\n", user);
    free(user);
    return 0;
}
```

**Sample output**
```
Hello, amint!
```

---

## Example 25: get the user's home directory with `sysinfo_home_directory()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    char* home = sysinfo_home_directory();
    fmt_printf("Per-user config under: %s/.myapp\n", home);

    free(home);
    return 0;
}
```

**Sample output (Windows)**
```
Per-user config under: C:\Users\amint/.myapp
```

---

## Example 26: build a temp-file path with `sysinfo_temp_directory()`

`sysinfo_temp_directory()` always strips the trailing separator, so you can join paths with a single `/` without worrying about a double slash.

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* tmp = sysinfo_temp_directory();
    char path[512];

    snprintf(path, sizeof(path), "%s/myapp-state.tmp", tmp);
    fmt_printf("Will write scratch state to: %s\n", path);

    free(tmp);
    return 0;
}
```

**Sample output**
```
Will write scratch state to: C:\Users\amint\AppData\Local\Temp/myapp-state.tmp
```

---

## Example 27: print the current working directory with `sysinfo_current_working_directory()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    char* cwd = sysinfo_current_working_directory();
    if (cwd) {
        fmt_printf("cwd = %s\n", cwd);
        free(cwd);
    }
    return 0;
}
```

**Sample output**
```
cwd = C:\Users\amint\Desktop\c_std
```

---

## Example 28: find where the current binary lives with `sysinfo_executable_path()`

Useful for locating sibling files (configs, plugins) that ship next to the executable.

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
    char* exe = sysinfo_executable_path();

    if (exe) {
        fmt_printf("Running from: %s\n", exe);
        free(exe);
    } 
    else {
        fmt_printf("Executable path unavailable on this platform.\n");
    }

    return 0;
}
```

**Sample output (Windows)**
```
Running from: C:\Users\amint\Desktop\c_std\main.exe
```

---

## Example 29: print the current PID with `sysinfo_process_id()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Our PID is %ld\n", sysinfo_process_id());
    return 0;
}
```

**Sample output**
```
Our PID is 11936
```

---

## Example 30: report total RAM with `sysinfo_total_memory_bytes()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    unsigned long long total = sysinfo_total_memory_bytes();
    fmt_printf("Total RAM: %.2f GiB (%llu bytes)\n", total / (1024.0 * 1024.0 * 1024.0), total);

    return 0;
}
```

**Sample output**
```
Total RAM: 23.90 GiB (25662439424 bytes)
```

---

## Example 31: refuse to start if not enough memory is free, using `sysinfo_available_memory_bytes()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdio.h>

int main() {
    unsigned long long avail = sysinfo_available_memory_bytes();
    const unsigned long long needed = 256ULL * 1024ULL * 1024ULL;  // 256 MiB

    if (avail < needed) {
        fmt_printf("Only %llu MiB available; refusing to start.\n", avail / (1024 * 1024));
        return 1;
    }
    fmt_printf("Available memory OK: %.2f GiB free\n", avail / (1024.0 * 1024.0 * 1024.0));

    return 0;
}
```

**Sample output**
```
Available memory OK: 14.73 GiB free
```

---

## Example 32: gate a feature on a 64-bit build with `sysinfo_is_64bit_process()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdio.h>

int main() {
    if (!sysinfo_is_64bit_process()) {
        fmt_printf("This release requires a 64-bit build. Aborting.\n");
        return 1;
    }
    fmt_printf("64-bit build detected; continuing.\n");
    
    return 0;
}
```

**Sample output**
```
64-bit build detected; continuing.
```

---

## Example 33: detect CPU byte order at runtime with `sysinfo_endianness()`

The returned pointer is a static literal — never `free()` it.

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <string.h>

int main() {
    const char* endian = sysinfo_endianness();
    fmt_printf("CPU is %s-endian\n", endian);

    // Branch on it cheaply if you care.
    if (strcmp(endian, "little") == 0) {
        fmt_printf("(Network code must swap multi-byte fields.)\n");
    }
    return 0;
}
```

**Sample output**
```
CPU is little-endian
(Network code must swap multi-byte fields.)
```

---

## Example 34: memory + swap snapshot with `sysinfo_used_memory_bytes()` / `sysinfo_total_swap_bytes()` / `sysinfo_free_swap_bytes()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    const double GiB = 1024.0 * 1024.0 * 1024.0;

    fmt_printf("RAM used : %.2f GiB\n", sysinfo_used_memory_bytes() / GiB);
    fmt_printf("Swap total: %.2f GiB\n", sysinfo_total_swap_bytes() / GiB);
    fmt_printf("Swap free : %.2f GiB\n", sysinfo_free_swap_bytes()  / GiB);

    return 0;
}
```

**Sample output**
```
RAM used : 0.84 GiB
Swap total: 3.00 GiB
Swap free : 2.94 GiB
```

---

## Example 35: uptime, process count and privilege with `sysinfo_uptime_seconds()` / `sysinfo_process_count()` / `sysinfo_is_admin()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    long up = sysinfo_uptime_seconds();
    if (up >= 0) {
        fmt_printf("Up %ld days, %ld hours\n", up / 86400, (up % 86400) / 3600);
    }
    
    fmt_printf("Processes running: %ld\n", sysinfo_process_count());
    fmt_printf("Elevated/root    : %s\n", sysinfo_is_admin() ? "yes" : "no");

    return 0;
}
```

**Sample output**
```
Up 0 days, 10 hours
Processes running: 47
Elevated/root    : no
```

---

## Example 36: disk space, battery and shell with `sysinfo_disk_total_bytes()` / `sysinfo_disk_free_bytes()` / `sysinfo_battery_percentage()` / `sysinfo_default_shell()`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"
#include <stdlib.h>

int main() {
#ifdef _WIN32
    const char* root = "C:\\";
#else
    const char* root = "/";
#endif

    fmt_printf("Disk total: %.1f GB\n", sysinfo_disk_total_bytes(root) / 1e9);
    fmt_printf("Disk free : %.1f GB\n", sysinfo_disk_free_bytes(root)  / 1e9);

    int bat = sysinfo_battery_percentage();
    if (bat < 0) {
        fmt_printf("Battery   : on AC / no battery\n");
    } 
    else {
        fmt_printf("Battery   : %d%%\n", bat);
    }

    char* shell = sysinfo_default_shell();
    fmt_printf("Shell     : %s\n", shell);
    free(shell);

    return 0;
}
```

**Sample output**
```
Disk total: 1081.1 GB
Disk free : 1010.6 GB
Battery   : on AC / no battery
Shell     : /bin/bash
```

---

## License

This project is open-source and available under [ISC License].
