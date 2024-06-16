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

## Example 2 : get product name of os with `sysinfo_product_type`

```c
#include "fmt/fmt.h"
#include "sysinfo/sysinfo.h"


int main() {
    fmt_printf("OS Product Type: %s\n", sysinfo_product_type());

    return 0;
}
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