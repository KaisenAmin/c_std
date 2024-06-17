# SysInfo Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License


## Overview

This SysInfo library in C provides a set of functions for gathering lots of information about system also operating system. and work fine in (windows and linux) not Mac :))

## Installation and Compilation

To use this library, include `sysinfo.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./sysinfo/sysinfo.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like generating random integers within specified ranges.


### Function Descriptions

- `char* sysinfo_product_version()`: This function returns the product version of the operating system in string form. If the version could not be determined, this function returns "unknown".

- `char* sysinfo_product_type()`: This function returns the product name of the operating system this application is running in.
- `char* sysinfo_kernel_version()`: This function returns the release version of the operating system kernel.
- `char* sysinfo_kernel_type()`: This function returns the type of the operating system kernel that program was compiled for. It's also the kernel the application is running on. on unix like systems it returns the same as the output of uname -s.

- `char* sysinfo_boot_unique_id()`: This function returns a unique ID for this machine's boot. in Linux, the boot unique ID is obtained from the file `/proc/sys/kernel/random/boot_id`. in Windows, the unique boot ID is derived from the system uptime in milliseconds since the last boot using `GetTickCount64()`.

- `char* sysinfo_cpu_architecture()`: This function returns the architecture of the Cpu that the application is running on.
- `char* sysinfo_machine_host_name()`: This function returns fully qualified name for a machine.  also in Linux version this function only work if the machine has a FQDN to give - if not, the result of the getaddrinfo() ends up being the same as the unqualified hostname.

- `char* sysinfo_machine_unique_id()`: This function returns the `id` for this machine. if no unique id could be determined . function returns an  `unknown` string. unique id is useful in network operations to identify this machine for an extended period of time.

## Example 

Several examples are provided to demonstrate the usage of the SysInfo library in various scenarios.

## Example 1: get prodouct version of system with `sysinfo_product_version`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"

int main() {
    fmt_printf("Os Version : %s\n", sysinfo_product_version());

    return 0;
}
```
**On Linux:**
```
Os Version : 24.04
```
**On Windows:**
```
Os Version : 10.0
```

## Example 2 : get product name of os with `sysinfo_product_type`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("OS Product Type: %s\n", sysinfo_product_type());

    return 0;
}
```
**On Linux:**
```
OS Product Type: Ubuntu
```
**On Windows:**
```
OS Product Type: Windows
```


### Example 3 : get kernel version with `sysinfo_kernel_version`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Kernel Version: %s\n", sysinfo_kernel_version());

    return 0;
}
```
**On Linux:**
```
Kernel Version: 6.8.0-35-generic
```
**On Windows:**
```
Kernel Version: 10.0.19045
```

## Example 4 : get kernel type with `sysinfo_kernel_type`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Kernel Type: %s\n", sysinfo_kernel_type());

    return 0;
}
```
**On Linux:**
```
Kernel Type: Linux
```
**On Windows:**
```
Kernel Type: Windows NT
```

## Example 5 : get boot unique id in linux in windows is not work `sysinfo_boot_unique_id`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Boot Unique Id : %s\n", sysinfo_boot_unique_id());

    return 0;
}
```
**On Linux:**
```
Boot Unique Id : bba30543-fea2-4477-b9e4-5343f8746200
```
**On Windows:**
```
Boot Unique Id : 22066640
```

## Example 6 : get cpu architecture of system with `sysinfo_cpu_architecture`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Cpu Architecture : %s\n", sysinfo_cpu_architecture());

    return 0;
}
```
**On Linux:**
```
Cpu Architecture : x86_64
```
**On Windows:**
```
Cpu Architecture : x86_64
```

## Example 7 : get machine host name with `sysinfo_machine_host_name`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Machine Host Name is : %s\n", sysinfo_machine_host_name());

    return 0;
}
```
**On Linux:**
```
Machine Host Name is : kaisen-VirtualBox
```
**On Windows:**
```
Machine Host Name is : DESKTOP-DF2A2F9
```

## Example 8 : get machine unique id  `sysinfo_machine_unique_id`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Machine Unique ID: %s\n", sysinfo_machine_unique_id());

    return 0;
}
```
**On Linux:**
```
Machine Unique ID: 4229c1dd3d8c41e1a7b9e1102912cde3
```
**On Windows:**
```
Machine Unique ID: 8ba376a1-9832-45a0-b7bd-5d507e7a1d68
```

## Example 9 : get product name in pretty way with `sysinfo_pretty_product_name`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("Pretty Product Name : %s\n", sysinfo_pretty_product_name());

    return 0;
}
```
**On Linux:**
```
Pretty Product Name : Ubuntu 24.04
```
**On Windows:**
```
Pretty Product Name : Windows 10 Version 10.0 (Build 19045)
```