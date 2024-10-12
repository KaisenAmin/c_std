#include "sysinfo.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <rpc.h>
#include <objbase.h>
#include <bluetoothapis.h>
#include <winternl.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>


// Define the function pointer type for NtQuerySystemInformation
typedef NTSTATUS (WINAPI *NtQuerySystemInformationFunc)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

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
#include <sys/statvfs.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

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

#ifdef _WIN32
static double get_sysinfo_cpu_usage_windows() {
    FILETIME idleTime, kernelTime, userTime;

    if (GetSystemTimes(&idleTime, &kernelTime, &userTime) == 0) {
        fprintf(stderr, "Error: Failed to get CPU times.\n");
        return -1.0;
    }

    static ULONGLONG prevIdleTime = 0, prevKernelTime = 0, prevUserTime = 0;

    ULONGLONG idle = ((ULONGLONG)idleTime.dwLowDateTime | ((ULONGLONG)idleTime.dwHighDateTime << 32));
    ULONGLONG kernel = ((ULONGLONG)kernelTime.dwLowDateTime | ((ULONGLONG)kernelTime.dwHighDateTime << 32));
    ULONGLONG user = ((ULONGLONG)userTime.dwLowDateTime | ((ULONGLONG)userTime.dwHighDateTime << 32));

    ULONGLONG idleDiff = idle - prevIdleTime;
    ULONGLONG kernelDiff = kernel - prevKernelTime;
    ULONGLONG userDiff = user - prevUserTime;

    prevIdleTime = idle;
    prevKernelTime = kernel;
    prevUserTime = user;

    if (kernelDiff + userDiff == 0) {
        return 0.0;
    }

    return (double)((kernelDiff + userDiff - idleDiff) * 100.0) / (kernelDiff + userDiff);
}

// Windows specific memory usage function
static double get_sysinfo_memory_usage_windows() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo) == 0) {
        fprintf(stderr, "Error: Failed to retrieve memory status.\n");
        return -1.0;
    }

    DWORDLONG totalMemory = memInfo.ullTotalPhys;
    DWORDLONG usedMemory = totalMemory - memInfo.ullAvailPhys;

    if (totalMemory == 0) {
        return -1.0;
    }

    return (double)(usedMemory * 100.0) / totalMemory;
}

static char* get_sysinfo_disk_space_windows(const char* path) {
    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;

    if (GetDiskFreeSpaceExA(path, &freeBytesAvailable, &totalBytes, &totalFreeBytes) == 0) {
        fprintf(stderr, "Error: Failed to get disk space information for path: %s\n", path);
        return NULL;
    }

    char* result = (char*)malloc(256);
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    snprintf(result, 256, "Total: %llu MB, Used: %llu MB, Available: %llu MB",
             totalBytes.QuadPart / (1024 * 1024),
             (totalBytes.QuadPart - totalFreeBytes.QuadPart) / (1024 * 1024),
             totalFreeBytes.QuadPart / (1024 * 1024));

    return result;
}

// Simpler Windows-specific function to get system uptime
static char* get_sysinfo_system_uptime_windows() {
    ULONGLONG uptimeMillis = GetTickCount64();  
    ULONGLONG uptimeSeconds = uptimeMillis / 1000; 

    unsigned long days = uptimeSeconds / (24 * 3600);
    uptimeSeconds %= (24 * 3600);
    unsigned long hours = uptimeSeconds / 3600;
    uptimeSeconds %= 3600;
    unsigned long minutes = uptimeSeconds / 60;
    unsigned long seconds = uptimeSeconds % 60;

    char* result = (char*)malloc(128);
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    snprintf(result, 128, "%lu days, %lu hours, %lu minutes, %lu seconds", days, hours, minutes, seconds);
    return result;
}

static void get_running_services_windows(Vector *services) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scm) {
        fprintf(stderr, "Error: Unable to open Service Control Manager.\n");
        return;
    }

    DWORD bytesNeeded = 0, servicesReturned = 0, resumeHandle = 0;
    EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &servicesReturned, &resumeHandle);

    LPENUM_SERVICE_STATUS serviceStatus = (LPENUM_SERVICE_STATUS)malloc(bytesNeeded);
    if (EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_STATE_ALL, serviceStatus, bytesNeeded, &bytesNeeded, &servicesReturned, &resumeHandle)) {
        for (DWORD i = 0; i < servicesReturned; i++) {
            char *serviceName = strdup(serviceStatus[i].lpServiceName);  // Using strdup to allocate memory for the service name
            vector_push_back(services, &serviceName);
        }
    } 
    else {
        fprintf(stderr, "Error: Failed to enumerate services.\n");
    }

    free(serviceStatus);
    CloseServiceHandle(scm);
}

static void get_open_ports_windows(Vector *ports) {
    PMIB_TCPTABLE tcpTable = NULL;
    DWORD size = 0;

    if (GetTcpTable(NULL, &size, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
        tcpTable = (MIB_TCPTABLE*)malloc(size);
    }

    if (GetTcpTable(tcpTable, &size, TRUE) == NO_ERROR) {
        for (DWORD i = 0; i < tcpTable->dwNumEntries; i++) {
            // Extract port number
            DWORD port = ntohs((u_short)tcpTable->table[i].dwLocalPort);
            int *port_ptr = (int*)malloc(sizeof(int));
            *port_ptr = (int)port;
            vector_push_back(ports, &port_ptr);
        }
    } 
    else {
        fprintf(stderr, "Error: Unable to retrieve TCP table.\n");
    }
    
    free(tcpTable);
}

#elif __linux__

// Linux specific CPU usage function
static double get_sysinfo_cpu_usage_linux() {
    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open /proc/stat.\n");
        return -1.0;
    }

    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
    if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8) {
        fclose(fp);
        fprintf(stderr, "Error: Failed to read CPU stats from /proc/stat.\n");
        return -1.0;
    }
    fclose(fp);

    static unsigned long long int prevUser = 0, prevNice = 0, prevSystem = 0, prevIdle = 0, prevIowait = 0, prevIrq = 0, prevSoftirq = 0, prevSteal = 0;

    unsigned long long int idleDiff = (idle + iowait) - (prevIdle + prevIowait);
    unsigned long long int totalDiff = (user + nice + system + idle + iowait + irq + softirq + steal) -
                                       (prevUser + prevNice + prevSystem + prevIdle + prevIowait + prevIrq + prevSoftirq + prevSteal);

    prevUser = user;
    prevNice = nice;
    prevSystem = system;
    prevIdle = idle;
    prevIowait = iowait;
    prevIrq = irq;
    prevSoftirq = softirq;
    prevSteal = steal;

    if (totalDiff == 0) {
        return 0.0;
    }

    return (double)((totalDiff - idleDiff) * 100.0) / totalDiff;
}

// Linux specific memory usage function
static double get_sysinfo_memory_usage_linux() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open /proc/meminfo.\n");
        return -1.0;
    }

    unsigned long totalMemory = 0, availableMemory = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "MemTotal: %lu kB", &totalMemory) == 1) {
            continue;
        }
        if (sscanf(buffer, "MemAvailable: %lu kB", &availableMemory) == 1) {
            break;
        }
    }

    fclose(fp);

    if (totalMemory == 0 || availableMemory == 0) {
        return -1.0;
    }

    unsigned long usedMemory = totalMemory - availableMemory;
    return (double)(usedMemory * 100.0) / totalMemory;
}

static char* get_sysinfo_disk_space_linux(const char* path) {
    struct statvfs vfs;

    if (statvfs(path, &vfs) != 0) {
        fprintf(stderr, "Error: Failed to get disk space information for path: %s\n", path);
        return NULL;
    }

    unsigned long totalBytes = vfs.f_blocks * vfs.f_frsize;
    unsigned long availableBytes = vfs.f_bavail * vfs.f_frsize;
    unsigned long usedBytes = totalBytes - availableBytes;

    char* result = (char*)malloc(256);
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    snprintf(result, 256, "Total: %lu MB, Used: %lu MB, Available: %lu MB",
             totalBytes / (1024 * 1024), usedBytes / (1024 * 1024), availableBytes / (1024 * 1024));

    return result;
}

static char* get_sysinfo_system_uptime_linux() {
    FILE* fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open /proc/uptime.\n");
        return NULL;
    }

    double uptimeSeconds = 0.0;
    if (fscanf(fp, "%lf", &uptimeSeconds) != 1) {
        fclose(fp);
        fprintf(stderr, "Error: Failed to read uptime from /proc/uptime.\n");
        return NULL;
    }
    fclose(fp);

    unsigned long days = uptimeSeconds / (24 * 3600);
    uptimeSeconds = fmod(uptimeSeconds, (24 * 3600));
    unsigned long hours = uptimeSeconds / 3600;
    uptimeSeconds = fmod(uptimeSeconds, 3600);
    unsigned long minutes = uptimeSeconds / 60;
    unsigned long seconds = (unsigned long)uptimeSeconds % 60;

    char* result = (char*)malloc(128);
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    snprintf(result, 128, "%lu days, %lu hours, %lu minutes, %lu seconds", days, hours, minutes, seconds);
    return result;
}

static void get_running_services_linux(Vector *services) {
    FILE *fp = popen("systemctl list-units --type=service --state=running --no-pager --no-legend", "r");
    if (!fp) {
        fprintf(stderr, "Error: Failed to run systemctl command.\n");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        char serviceName[256];
        sscanf(buffer, "%255s", serviceName); 

        char *service = strdup(serviceName); 
        vector_push_back(services, &service);
    }

    pclose(fp);
}

static void get_open_ports_linux(Vector *ports) {
    FILE *fp = popen("ss -lntu | awk 'NR>1 {print $5}'", "r");
    if (!fp) {
        fprintf(stderr, "Error: Failed to run ss command.\n");
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp)) {
        char *ip_port = strchr(buffer, ':');  
        if (ip_port) {
            int port;
            sscanf(ip_port + 1, "%d", &port);  // Extract the port number
            int *port_ptr = (int*)malloc(sizeof(int));
            *port_ptr = port;
            vector_push_back(ports, &port_ptr);
        }
    }

    pclose(fp);
}

// Linux specific virtualization detection remains the same
static bool check_virtualization_in_cpuinfo() {
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (!cpuinfo) {
        fprintf(stderr, "Error: Failed to open /proc/cpuinfo.\n");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), cpuinfo)) {
        if (strstr(line, "hypervisor")) {
            fclose(cpuinfo);
            return true;  // Found hypervisor flag in /proc/cpuinfo
        }
    }

    fclose(cpuinfo);
    return false;
}

static bool check_virtualization_with_systemd() {
    FILE *fp = popen("systemd-detect-virt", "r");
    if (!fp) {
        fprintf(stderr, "Error: Failed to run systemd-detect-virt.\n");
        return false;
    }

    char buffer[256];
    bool is_virtualized = false;
    if (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "none", 4) != 0) {
            is_virtualized = true;  // systemd-detect-virt detected virtualization
        }
    }

    pclose(fp);
    return is_virtualized;
}

/**
 * @brief Checks if the system is running in a virtualized environment.
 * 
 * This function attempts to detect if the system is virtualized by 
 * examining `/proc/cpuinfo` or using the `systemd-detect-virt` utility.
 *
 * On Linux, it first checks the `/proc/cpuinfo` file for any indicators 
 * of virtualization. If that fails, it tries to detect virtualization 
 * using `systemd-detect-virt`.
 *
 * @return bool Returns true if virtualization is detected, false otherwise.
 *
 * @note This function is primarily designed for Linux environments.
 *       The `check_virtualization_in_cpuinfo()` function checks for 
 *       known virtualization flags in the CPU information, and 
 *       `check_virtualization_with_systemd()` invokes `systemd-detect-virt`
 *       to identify if the system is running in a virtual machine.
 */
bool sysinfo_is_virtualized() {
    return check_virtualization_in_cpuinfo() || check_virtualization_with_systemd();
}

#endif

/**
 * @brief Retrieves the current CPU usage percentage of the system.
 * 
 * This function provides the CPU usage as a percentage by calculating the proportion
 * of time the CPU has spent performing work (in user mode and kernel mode) as compared to
 * idle time. The calculation is done over time, so repeated calls will provide more accurate
 * usage information.
 * 
 * On Windows, it uses the `GetSystemTimes()` function to get the amount of time spent in idle,
 * kernel, and user mode. On Linux, it reads from the `/proc/stat` file to obtain similar statistics.
 * 
 * @return double Returns the percentage of CPU usage. If an error occurs, it returns -1.0.
 * 
 * @note The function is platform-dependent and works on both Windows and Linux. On Windows, the 
 *       CPU usage is calculated based on time intervals provided by `GetSystemTimes()`. On Linux,
 *       the function parses the `/proc/stat` file to extract CPU usage statistics.
 */
double sysinfo_cpu_usage() {
#ifdef _WIN32
    return get_sysinfo_cpu_usage_windows();
#elif __linux__
    return get_sysinfo_cpu_usage_linux();
#endif 
}


/**
 * @brief Retrieves the current memory usage percentage of the system.
 * 
 * This function calculates the memory usage as a percentage of the total available memory on the system.
 * It retrieves memory statistics differently depending on the platform. On Windows, it uses the
 * `GlobalMemoryStatusEx()` function, while on Linux, it parses `/proc/meminfo`.
 * 
 * @return double Returns the percentage of memory being used. If an error occurs, it returns -1.0.
 * 
 * @note This function works on both Windows and Linux platforms. On Windows, it uses the 
 *       `GlobalMemoryStatusEx()` function to get the total and available physical memory. On Linux, 
 *       it reads from `/proc/meminfo` to retrieve memory statistics.
 */
double sysinfo_memory_usage() {
#ifdef _WIN32
    return get_sysinfo_memory_usage_windows();
#elif __linux__
    return get_sysinfo_memory_usage_linux();
#endif
}

/**
 * @brief Retrieves the disk space information for a given path.
 * 
 * This function provides the total, used, and available disk space for the specified path.
 * It retrieves disk space information using platform-specific methods.
 * 
 * On Windows, it uses the `GetDiskFreeSpaceEx()` function to get the total and free space.
 * On Linux, it uses the `statvfs()` system call to get the file system statistics.
 * 
 * @param path The file path to check the disk space for.
 * @return char* A string containing the total, used, and available disk space in MB.
 *               The caller is responsible for freeing the allocated memory.
 *               Returns NULL in case of an error.
 */
char* sysinfo_disk_space(const char* path) {
#ifdef _WIN32
    return get_sysinfo_disk_space_windows(path);
#elif __linux__
    return get_sysinfo_disk_space_linux(path);
#endif
}

/**
 * @brief Retrieves the system's uptime since the last boot.
 * 
 * This function returns the system uptime in a human-readable format, including days, hours,
 * minutes, and seconds.
 * 
 * On Windows, it uses the `GetTickCount64()` function to get the uptime in milliseconds.
 * On Linux, it parses the `/proc/uptime` file to get the uptime in seconds.
 * 
 * @return char* A string containing the system uptime in days, hours, minutes, and seconds.
 *               The caller is responsible for freeing the allocated memory.
 *               Returns NULL in case of an error.
 */
char* sysinfo_system_uptime() {
#ifdef _WIN32
    return get_sysinfo_system_uptime_windows();
#elif __linux__
    return get_sysinfo_system_uptime_linux();
#endif
}

/**
 * @brief Retrieves the list of currently running services on the system.
 * 
 * This function gathers the running services on both Windows and Linux systems.
 * The services are returned in a dynamically allocated vector of `String*`.
 * 
 * @return Vector* A vector containing the names of the running services.
 * 
 * @note The vector needs to be deallocated by the caller.
 */
Vector* sysinfo_running_services() {
    Vector *services = vector_create(sizeof(char*));

#ifdef _WIN32
    get_running_services_windows(services);
#elif __linux__
    get_running_services_linux(services);
#endif

    return services;
}

/**
 * @brief Retrieves the number of CPU cores available on the system.
 * 
 * This function returns the total number of CPU cores available for use on the system.
 * 
 * On Windows, it uses the `GetSystemInfo()` function to get the number of processors.
 * On Linux, it uses `sysconf(_SC_NPROCESSORS_ONLN)` to retrieve the number of available processors.
 * 
 * @return int Number of CPU cores. If an error occurs, it returns -1.
 */
int sysinfo_cpu_cores() {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif __linux__
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) {
        return -1; // Error occurred
    }
    return (int)nprocs;
#endif
}

/**
 * @brief Retrieves a list of active processes on the system.
 * 
 * This function returns a list of currently running processes on the system. 
 * The processes are returned in a dynamically allocated vector of strings (char*).
 * 
 * @return Vector* A vector containing the names of the active processes.
 * 
 * @note The vector needs to be deallocated by the caller.
 */
Vector* sysinfo_process_list() {
    Vector* processes = vector_create(sizeof(char*));

#ifdef _WIN32
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Unable to create process snapshot.\n");
        return processes;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return processes;
    }

    do {
        char* processName = strdup(pe32.szExeFile);
        vector_push_back(processes, &processName);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

#elif __linux__
    DIR* procDir = opendir("/proc");
    if (!procDir) {
        fprintf(stderr, "Error: Unable to open /proc directory.\n");
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != NULL) {
        if (entry->d_type == DT_DIR && strspn(entry->d_name, "0123456789") == strlen(entry->d_name)) {
            char procPath[300];  // Increase buffer size to 300 to avoid truncation warnings
            snprintf(procPath, sizeof(procPath), "/proc/%s/comm", entry->d_name);

            FILE* commFile = fopen(procPath, "r");
            if (commFile) {
                char processName[256];
                if (fgets(processName, sizeof(processName), commFile)) {
                    processName[strcspn(processName, "\n")] = 0;  // Strip newline character
                    char* process = strdup(processName);
                    vector_push_back(processes, &process);
                }
                fclose(commFile);
            }
        }
    }

    closedir(procDir);
#endif

    return processes;
}

/**
 * @brief Retrieves the list of active network interfaces along with their IP addresses.
 * 
 * This function gathers the list of active network interfaces and their associated IP addresses
 * on both Windows and Linux systems. The results are returned in a dynamically allocated vector
 * of `SysinfoNetworkInterface` structures.
 * 
 * @return Vector* A vector containing the network interfaces and their IP addresses.
 * 
 * @note The vector and its contents need to be deallocated by the caller.
 */
Vector* sysinfo_network_interfaces() {
    Vector* interfaces = vector_create(sizeof(SysinfoNetworkInterface));

#ifdef _WIN32
    ULONG bufferSize = 15000;
    PIP_ADAPTER_ADDRESSES adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);

    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        free(adapterAddresses);
        adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
    }

    DWORD result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize);
    if (result == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES adapter = adapterAddresses;
        while (adapter) {
            // Skip inactive adapters
            if (adapter->OperStatus == IfOperStatusUp) {
                SysinfoNetworkInterface iface;
                iface.interface_name = strdup(adapter->AdapterName);

                PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress;
                while (unicast) {
                    SOCKADDR* addr = unicast->Address.lpSockaddr;
                    if (addr->sa_family == AF_INET) {  // IPv4
                        struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr;
                        iface.ip_address = strdup(inet_ntoa(ipv4->sin_addr));
                        vector_push_back(interfaces, &iface);
                        break; // Only get one IP address per interface
                    }
                    unicast = unicast->Next;
                }
            }
            adapter = adapter->Next;
        }
    } else {
        fprintf(stderr, "Error: Failed to get network adapter addresses.\n");
    }

    free(adapterAddresses);

#elif __linux__
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        fprintf(stderr, "Error: Failed to open socket.\n");
        return interfaces;
    }

    struct ifconf ifc;
    char buffer[4096];
    ifc.ifc_len = sizeof(buffer);
    ifc.ifc_buf = buffer;

    if (ioctl(fd, SIOCGIFCONF, &ifc) == -1) {
        fprintf(stderr, "Error: Failed to get network interface configuration.\n");
        close(fd);
        return interfaces;
    }

    struct ifreq* ifr = ifc.ifc_req;
    int interfaceCount = ifc.ifc_len / sizeof(struct ifreq);

    for (int i = 0; i < interfaceCount; i++) {
        struct ifreq* item = &ifr[i];
        SysinfoNetworkInterface iface;
        iface.interface_name = strdup(item->ifr_name);

        if (ioctl(fd, SIOCGIFADDR, item) == 0) {
            struct sockaddr_in* addr = (struct sockaddr_in*)&item->ifr_addr;
            iface.ip_address = strdup(inet_ntoa(addr->sin_addr));
            vector_push_back(interfaces, &iface);
        }
    }

    close(fd);
#endif

    return interfaces;
}

/**
 * @brief Deallocates the memory used by a vector of SysinfoNetworkInterface structures.
 * 
 * @param interfaces The vector containing the network interfaces.
 */
void sysinfo_deallocate_network_interfaces(Vector* interfaces) {
    for (size_t i = 0; i < vector_size(interfaces); ++i) {
        SysinfoNetworkInterface* iface = (SysinfoNetworkInterface*)vector_at(interfaces, i);
        free(iface->interface_name);
        free(iface->ip_address);
    }
    vector_deallocate(interfaces);
}

/**
 * @brief Retrieves a list of open network ports on the system.
 * 
 * This function gathers open ports from both Windows and Linux systems.
 * It returns a dynamically allocated vector of integers representing the open ports.
 * 
 * @return Vector* A vector containing the open ports.
 * 
 * @note The vector needs to be deallocated by the caller.
 */
Vector* sysinfo_open_ports() {
    Vector *ports = vector_create(sizeof(int*));

#ifdef _WIN32
    get_open_ports_windows(ports);
#elif __linux__
    get_open_ports_linux(ports);
#endif

    return ports;
}

#ifdef _WIN32
/**
 * @brief Checks if the system is running in a virtualized environment.
 *
 * This function detects if the system is running in a virtualized environment 
 * by checking for specific processor features available on the platform.
 *
 * On Windows, it checks the presence of `PF_VIRT_FIRMWARE_ENABLED` and 
 * `PF_HYPERVISOR_PRESENT` features to determine virtualization status.
 *
 * On Linux, the detection is handled via `/proc/cpuinfo` or `systemd-detect-virt`, 
 * although these are not included in this Windows-specific implementation.
 *
 * @return bool Returns true if virtualization is detected, false otherwise.
 *
 * @note This function is platform-specific and currently supports Windows and Linux.
 *       The function checks for the availability of the `PF_VIRT_FIRMWARE_ENABLED` 
 *       feature on all Windows systems. The `PF_HYPERVISOR_PRESENT` feature is 
 *       conditionally checked if available on the platform.
 */
bool sysinfo_is_virtualized() {
    if (IsProcessorFeaturePresent(PF_VIRT_FIRMWARE_ENABLED)) {
        return true;
    }

#ifdef PF_HYPERVISOR_PRESENT
    if (IsProcessorFeaturePresent(PF_HYPERVISOR_PRESENT)) {
        return true;
    }
#endif

    return false;
}
#endif