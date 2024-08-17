# SysInfo Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The SysInfo library provides a set of functions for retrieving detailed information about the operating system and hardware of a machine. It is designed to work on both Windows and Linux systems, though it is not compatible with macOS. The library is lightweight and easy to integrate into C projects, offering developers essential system details such as OS version, kernel type, CPU architecture, and more.

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
