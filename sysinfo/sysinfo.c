#include "sysinfo.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <windows.h>
#include <rpc.h>
#include <objbase.h>
#include <bluetoothapis.h>

static char* get_windows_version() {
    static char version[128];
    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;

    dwVersion = GetVersion();

    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

    if (dwVersion < 0x80000000) {
        sprintf(version, "%ld.%ld", dwMajorVersion, dwMinorVersion);
    } 
    else {
        strcpy(version, "unknown");
    }
    return version;
}

static char* get_windows_product_type() {
    static char product_type[] = "Windows";
    return product_type;
}

static char* get_windows_kernel_version() {
    static char kernel_version[128];
    OSVERSIONINFOEX osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        sprintf(kernel_version, "%ld.%ld.%ld", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
    } 
    else {
        strcpy(kernel_version, "unknown");
    }

    return kernel_version;
}

static char* get_windows_kernel_type() {
    static char kernel_type[] = "Windows NT";
    return kernel_type;
}

static char* get_windows_boot_unique_id() {
    static char boot_id[128] = {0};
    ULONGLONG uptime = GetTickCount64();

    sprintf(boot_id, "%llu", uptime);

    return boot_id;
}

static char* get_windows_cpu_architecture() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    static char architecture[128];

    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            strcpy(architecture, "x86_64");
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            strcpy(architecture, "ARM");
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            strcpy(architecture, "IA64");
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            strcpy(architecture, "x86");
            break;
        default:
            strcpy(architecture, "unknown");
            break;
    }
    return architecture;
}

static char* get_windows_machine_host_name() {
    static char hostname[150] = {0};
    TCHAR infoBuf[150];
    DWORD bufCharCount = 150;

    if (GetComputerName(infoBuf, &bufCharCount)) {
        for (int i = 0; i < (int)bufCharCount; i++) {
            hostname[i] = infoBuf[i];
        }
    } 
    else {
        strcpy(hostname, "Unknown_Host_Name");
    }

    return hostname;
}

static char* get_windows_machine_unique_id() {
    static char unique_id[128] = {0};
    HKEY hKey;
    DWORD bufferSize = sizeof(unique_id);
    wchar_t buffer[128];

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"MachineGuid", NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            wcstombs(unique_id, buffer, sizeof(unique_id));  // convert wchar_t to char
        }
        RegCloseKey(hKey);
    }

    if (strlen(unique_id) == 0) {
        strcpy(unique_id, "unknown");
    }

    return unique_id;
}

typedef LONG NTSTATUS;
typedef NTSTATUS (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

static void get_windows_pretty_version(char* buffer, size_t buffer_size) {
    RTL_OSVERSIONINFOEXW osvi;
    memset(&osvi, 0, sizeof(RTL_OSVERSIONINFOEXW));
    osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
    
    HMODULE hNtdll = GetModuleHandle(TEXT("ntdll.dll"));
    if (hNtdll == NULL) {
        snprintf(buffer, buffer_size, "Unknown Windows Version");
        return;
    }

    // Use a union to safely cast the function pointer
    union {
        FARPROC proc;
        RtlGetVersionPtr func;
    } rtlGetVersionUnion;
    
    rtlGetVersionUnion.proc = GetProcAddress(hNtdll, "RtlGetVersion");
    if (rtlGetVersionUnion.proc == NULL) {
        snprintf(buffer, buffer_size, "Unknown Windows Version");
        return;
    }

    rtlGetVersionUnion.func((PRTL_OSVERSIONINFOW)&osvi);

    const char* productName;
    if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0) {
        productName = "Windows 10";
    } 
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
        productName = "Windows 8.1";
    } 
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
        productName = "Windows 8";
    } 
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
        productName = "Windows 7";
    } 
    else {
        productName = "Unknown Windows Version";
    }

    snprintf(buffer, buffer_size, "%s Version %lu.%lu (Build %lu)", productName, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
}

static char* get_windows_build_abi() {
    static char build_abi[128];
    memset(build_abi, 0, sizeof(build_abi));

    char* architecture = sysinfo_cpu_architecture();
    if (architecture) {
        strcpy(build_abi, architecture);
        strcat(build_abi, "-");
    } 
    else {
        strcpy(build_abi, "unknown-");
    }

    strcat(build_abi, "little_endian-");

    // On Windows, the data model is LLP64
    strcat(build_abi, "llp64");

    return build_abi;
}

char** get_sysinfo_list_bluetooth_devices_windows(int* count) {
    *count = 0;
    char** devices = NULL;

    BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams;
    BLUETOOTH_DEVICE_INFO deviceInfo;
    HBLUETOOTH_DEVICE_FIND hFind;

    memset(&searchParams, 0, sizeof(searchParams));
    searchParams.dwSize = sizeof(searchParams);
    searchParams.fReturnAuthenticated = TRUE;
    searchParams.fReturnRemembered = TRUE;
    searchParams.fReturnConnected = TRUE;
    searchParams.fReturnUnknown = TRUE;
    searchParams.fIssueInquiry = TRUE;
    searchParams.cTimeoutMultiplier = 4;

    deviceInfo.dwSize = sizeof(deviceInfo);

    hFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
    if (hFind != NULL) {
        do {
            (*count)++;
            devices = (char**)realloc(devices, sizeof(char*) * (*count));
            wchar_t* wname = deviceInfo.szName;
            int len = WideCharToMultiByte(CP_UTF8, 0, wname, -1, NULL, 0, NULL, NULL);
            devices[(*count) - 1] = (char*)malloc(len);
            WideCharToMultiByte(CP_UTF8, 0, wname, -1, devices[(*count) - 1], len, NULL, NULL);
        } while (BluetoothFindNextDevice(hFind, &deviceInfo));

        BluetoothFindDeviceClose(hFind);
    }

    return devices;
}

#elif __linux__

#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

static char* get_linux_version() {
    static char version[128];
    FILE* fp = popen("lsb_release -r | awk '{print $2}'", "r");

    if (fp == NULL) {
        strcpy(version, "unknown");
        return version;
    }
    if (fgets(version, sizeof(version) - 1, fp) != NULL) {
        version[strcspn(version, "\n")] = 0;  // remove newline 
    } 
    else {
        strcpy(version, "unknown");
    }

    pclose(fp);
    return version;
}

static char* get_linux_product_type() {
    static char product_type[128];
    FILE* fp = popen("lsb_release -i | awk -F: '{print $2}'", "r");

    if (fp == NULL) {
        strcpy(product_type, "unknown");
        return product_type;
    }
    if (fgets(product_type, sizeof(product_type) - 1, fp) != NULL) {
        product_type[strcspn(product_type, "\n")] = 0; 

        // trim whitespace
        char* start = product_type;
        while (*start == ' ' || *start == '\t') {
            start++;
        }
        memmove(product_type, start, strlen(start) + 1);
    } 
    else {
        strcpy(product_type, "unknown");
    }

    pclose(fp);
    return product_type;
}

static char* get_linux_kernel_version() {
    static char kernel_version[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_version, buffer.release, sizeof(kernel_version));
    } 
    else {
        strcpy(kernel_version, "unknown");
    }

    return kernel_version;
}

static char* get_linux_kernel_type() {
    static char kernel_type[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_type, buffer.sysname, sizeof(kernel_type));
    } 
    else {
        strcpy(kernel_type, "unknown");
    }

    return kernel_type;
}

static char* get_linux_boot_unique_id() {
    static char boot_id[128];
    FILE* fp = fopen("/proc/sys/kernel/random/boot_id", "r");

    if (fp == NULL) {
        strcpy(boot_id, "unknown");
        return boot_id;
    }
    if (fgets(boot_id, sizeof(boot_id) - 1, fp) != NULL) {
        boot_id[strcspn(boot_id, "\n")] = 0;  
    } 
    else {
        strcpy(boot_id, "unknown");
    }

    fclose(fp);
    return boot_id;
}

static char* get_linux_cpu_architecture() {
    static char architecture[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(architecture, buffer.machine, sizeof(architecture));
    } 
    else {
        strcpy(architecture, "unknown");
    }

    return architecture;
}

static char* get_linux_machine_host_name() {
    static char hostname[1024];
    hostname[1023] = '\0';

    if (gethostname(hostname, sizeof(hostname) - 1) != 0) {
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    struct addrinfo hints, *info, *p;
    int gai_result;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    for (p = info; p != NULL; p = p->ai_next) {
        if (p->ai_canonname) {
            strncpy(hostname, p->ai_canonname, sizeof(hostname) - 1);
            hostname[sizeof(hostname) - 1] = '\0';
            break;
        }
    }

    freeaddrinfo(info);
    return hostname;
}

static char* get_linux_machine_unique_id() {
    static char unique_id[128];
    FILE* fp = fopen("/etc/machine-id", "r");

    if (fp == NULL) {
        strcpy(unique_id, "unknown");
        return unique_id;
    }
    if (fgets(unique_id, sizeof(unique_id) - 1, fp) != NULL) {
        unique_id[strcspn(unique_id, "\n")] = 0;  // remove newline 
    } 
    else {
        strcpy(unique_id, "unknown");
    }

    fclose(fp);
    return unique_id;
}

static char* get_linux_build_abi() {
    static char build_abi[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        if (strcmp(buffer.machine, "x86_64") == 0) {
            strcpy(build_abi, "x86_64-");
        } 
        else {
            strcpy(build_abi, buffer.machine);
            strcat(build_abi, "-");
        }
    } 
    else {
        strcpy(build_abi, "unknown-");
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    strcat(build_abi, "little_endian-");
#else
    strcat(build_abi, "big_endian-");
#endif

    // On Unix-like systems, the data model is usually LP64
    strcat(build_abi, "lp64");

    return build_abi;
}

char** get_sysinfo_list_bluetooth_devices_linux(int* count) {
    *count = 0;
    char** devices = NULL;
    FILE* pipe = popen("hcitool dev", "r");

    if (pipe) {
        char buffer[128];

        while (fgets(buffer, 128, pipe) != NULL) {
            buffer[strcspn(buffer, "\r\n")] = 0; 
            (*count)++;
            devices = (char**)realloc(devices, sizeof(char*) * (*count));
            devices[(*count) - 1] = strdup(buffer);
        }

        pclose(pipe);
    }
    return devices;
}

#else

static char* get_unknown_version() {
    static char version[] = "unknown";
    return version;
}

static char* get_unknown_product_type() {
    static char product_type[] = "unknown";
    return product_type;
}

static char* get_unknown_kernel_version() {
    static char kernel_version[] = "unknown";
    return kernel_version;
}

static char* get_unknown_kernel_type() {
    static char kernel_type[] = "unknown";
    return kernel_type;
}

static char* get_unknown_boot_unique_id() {
    static char boot_id[] = "";
    return boot_id;
}

static char* get_unknown_cpu_architecture() {
    static char architecture[] = "unknown";
    return architecture;
}

static char* get_unknown_machine_host_name() {
    static char hostname[] = "Unknown_Host_Name";
    return hostname;
}

static char* get_unknown_machine_unique_id() {
    static char unique_id[] = "unknown";
    return unique_id;
}

static char* get_unknown_build_abi() {
    static char build_abi[] = "unknown";
    return build_abi;
}

char** sysinfo_list_bluetooth_devices(int* count) {
    *count = 0;
    return NULL;
}

#endif

/**
 * @brief This function returns the version of the operating system in a string format. 
 * The version provides important information about the specific release of the operating system being used.
 * 
 * @return char* A static string containing the OS version. If the version cannot 
 *               be determined, the function returns "unknown".
 */
char* sysinfo_product_version() {
#ifdef _WIN32
    return get_windows_version();
#elif __linux__
    return get_linux_version();
#else
    return get_unknown_version();
#endif
}

/**
 * @brief This function returns the product type or name of the operating system. For instance, 
 * it might return "Windows" for a Windows operating system or "Ubuntu" for an Ubuntu 
 * Linux distribution. This information is useful for identifying the specific type of 
 * operating system the application is running on.
 * 
 * @return char* A static string containing the OS product type. If the product type 
 *               cannot be determined, the function returns "unknown".
 */
char* sysinfo_product_type() {
#ifdef _WIN32
    return get_windows_product_type();
#elif __linux__
    return get_linux_product_type();
#else
    return get_unknown_product_type();
#endif
}

/**
 * @brief This function retrieves the kernel version of the operating system on which the application
 * is running. For example, it may return "10.0.19045" on Windows or "6.8.0-35-generic" on Linux.
 * The kernel version is essential for understanding the system's low-level OS details.
 * 
 * @return char* A static string containing the kernel version. If the version cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_kernel_version() {
#ifdef _WIN32
    return get_windows_kernel_version();
#elif __linux__
    return get_linux_kernel_version();
#else
    return get_unknown_kernel_version();
#endif
}

/**
 * @brief This function provides the type of the kernel that the application is running on. For example,
 * it may return "Windows NT" on Windows or "Linux" on Unix-like systems. This information is useful
 * for identifying the general type of the operating system kernel.
 * 
 * @return char* A static string containing the kernel type. If the type cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_kernel_type() {
#ifdef _WIN32
    return get_windows_kernel_type();
#elif __linux__
    return get_linux_kernel_type();
#else
    return get_unknown_kernel_type();
#endif
}

/**
 * @brief This function generates a unique identifier that represents the current boot session of the machine.
 * On Linux, this is typically obtained from `/proc/sys/kernel/random/boot_id`, while on Windows, it may
 * be derived from the system uptime. This unique ID can be used to track the current boot session of the system.
 * 
 * @return char* A static string containing the unique boot ID. If the ID cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_boot_unique_id() {
#ifdef _WIN32
    return get_windows_boot_unique_id();
#elif __linux__
    return get_linux_boot_unique_id();
#else
    return get_unknown_boot_unique_id();
#endif
}

/**
 * @brief This function detects and returns the architecture of the CPU that the application
 * is running on. For example, it may return "x86_64", "ARM", or "IA64", depending on
 * the system. The architecture string is useful for understanding the type of processor
 * the system is using.
 * 
 * @return char* A static string containing the CPU architecture.
 */
char* sysinfo_cpu_architecture() {
#ifdef _WIN32
    return get_windows_cpu_architecture();
#elif __linux__
    return get_linux_cpu_architecture();
#else
    return get_unknown_cpu_architecture();
#endif
}

/**
 * @brief This function retrieves the fully qualified domain name (FQDN) or the simple host name
 * of the machine on which the application is running. On Linux, it may attempt to resolve
 * the FQDN, while on Windows, it returns the computer name.
 * 
 * @return char* A static string containing the machine's host name.
 */
char* sysinfo_machine_host_name() {
#ifdef _WIN32
    return get_windows_machine_host_name();
#elif __linux__
    return get_linux_machine_host_name();
#else
    return get_unknown_machine_host_name();
#endif
}

/**
 * @brief This function provides a unique identifier for the machine, which is useful for identifying
 * the machine in network operations or other scenarios where a persistent unique ID is needed.
 * The method of obtaining this ID varies by platform.
 * 
 * @return char* A static string containing the machine's unique ID. If the ID cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_machine_unique_id() {
#ifdef _WIN32
    return get_windows_machine_unique_id();
#elif __linux__
    return get_linux_machine_unique_id();
#else
    return get_unknown_machine_unique_id();
#endif
}

/**
 * @brief This function generates a human-readable string that describes the operating system
 * name and version in a friendly format. On Windows, it uses the system's product name
 * and version. On Linux, it combines the product type and version. For other systems,
 * it falls back to the kernel type and version.
 * 
 * @return char* A static string containing the pretty product name.
 */
char* sysinfo_pretty_product_name() {
    static char pretty_name[256];
    memset(pretty_name, 0, sizeof(pretty_name));

#ifdef _WIN32
    get_windows_pretty_version(pretty_name, sizeof(pretty_name));
#elif __linux__
    snprintf(pretty_name, sizeof(pretty_name), "%s %s", sysinfo_product_type(), sysinfo_product_version());
#else
    snprintf(pretty_name, sizeof(pretty_name), "%s %s", sysinfo_kernel_type(), sysinfo_kernel_version());
#endif

    return pretty_name;
}

/**
 * @brief This function provides information about the system's ABI, which includes details
 * such as the architecture, endianness, and data model (e.g., LP64, LLP64). The ABI
 * is useful for understanding how the compiled binaries will interact with the system.
 * 
 * @return char* A static string containing the ABI information.
 */
char* sysinfo_build_abi() {
#ifdef _WIN32
    return get_windows_build_abi();
#elif __linux__
    return get_linux_build_abi();
#else
    return get_unknown_build_abi();
#endif
}

/**
 * @brief This function scans for Bluetooth devices and returns a list of device names that
 * are connected or remembered by the system. The number of devices found is returned
 * through the `count` parameter. The list of devices is dynamically allocated and should
 * be freed by the caller.
 * 
 * @param count Pointer to an integer that will store the number of Bluetooth devices found.
 * 
 * @return char** An array of strings, each representing a Bluetooth device name.
 *               The caller is responsible for freeing each string and the array itself.
 */
char** sysinfo_list_bluetooth_devices(int* count) {
#ifdef _WIN32
    return get_sysinfo_list_bluetooth_devices_windows(count);
#elif __linux__
    return get_sysinfo_list_bluetooth_devices_linux(count);
#else
    return NULL;
#endif
}