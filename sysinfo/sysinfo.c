/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Sysinfo
*/

#include "sysinfo.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include <rpc.h>
#include <objbase.h>
#include <bluetoothapis.h>
#include <winternl.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <wbemidl.h>


// Define the function pointer type for NtQuerySystemInformation
typedef NTSTATUS (WINAPI *NtQuerySystemInformationFunc)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef LONG NTSTATUS;
typedef NTSTATUS (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);


static char* get_windows_version() {
    SYSINFO_LOG("[get_windows_version]: Entering function.");
    
    static char version[128];
    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;

    dwVersion = GetVersion();
    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

    if (dwVersion < 0x80000000) {
        sprintf(version, "%ld.%ld", dwMajorVersion, dwMinorVersion);
        SYSINFO_LOG("[get_windows_version]: Detected version: %s", version);
    } 
    else {
        strcpy(version, "unknown");
        SYSINFO_LOG("[get_windows_version]: Unable to detect version, setting to unknown.");
    }

    SYSINFO_LOG("[get_windows_version]: Exiting function.");
    return version;
}


static char* get_windows_product_type() {
    SYSINFO_LOG("[get_windows_product_type]: Returning Windows product type.");
    static char product_type[] = "Windows";
    return product_type;
}


static char* get_windows_kernel_version() {
    SYSINFO_LOG("[get_windows_kernel_version]: Entering function.");

    static char kernel_version[128];
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        sprintf(kernel_version, "%ld.%ld.%ld", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
        SYSINFO_LOG("[get_windows_kernel_version]: Kernel version: %s", kernel_version);
    } 
    else {
        strcpy(kernel_version, "unknown");
        SYSINFO_LOG("[get_windows_kernel_version]: Failed to retrieve kernel version, setting to unknown.");
    }

    SYSINFO_LOG("[get_windows_kernel_version]: Exiting function.");
    return kernel_version;
}


static char* get_windows_kernel_type() {
    SYSINFO_LOG("[get_windows_kernel_type]: Returning Windows NT kernel type.");
    static char kernel_type[] = "Windows NT";

    return kernel_type;
}


static char* get_windows_boot_unique_id() {
    SYSINFO_LOG("[get_windows_boot_unique_id]: Entering function.");

    static char boot_id[128] = {0};
    ULONGLONG uptime = GetTickCount64();
    sprintf(boot_id, "%llu", uptime);
    
    SYSINFO_LOG("[get_windows_boot_unique_id]: Boot ID: %s", boot_id);
    SYSINFO_LOG("[get_windows_boot_unique_id]: Exiting function.");

    return boot_id;
}


static char* get_windows_cpu_architecture() {
    SYSINFO_LOG("[get_windows_cpu_architecture]: Entering function.");
    
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

    SYSINFO_LOG("[get_windows_cpu_architecture]: Detected CPU architecture: %s", architecture);
    SYSINFO_LOG("[get_windows_cpu_architecture]: Exiting function.");
    
    return architecture;
}


static char* get_windows_machine_host_name() {
    SYSINFO_LOG("[get_windows_machine_host_name]: Entering function.");

    static char hostname[150] = {0};
    TCHAR infoBuf[150];
    DWORD bufCharCount = 150;

    if (GetComputerName(infoBuf, &bufCharCount)) {
        for (int i = 0; i < (int)bufCharCount; i++) {
            hostname[i] = infoBuf[i];
        }
        SYSINFO_LOG("[get_windows_machine_host_name]: Host name: %s", hostname);
    } 
    else {
        strcpy(hostname, "Unknown_Host_Name");
        SYSINFO_LOG("[get_windows_machine_host_name]: Failed to retrieve host name, setting to Unknown_Host_Name.");
    }

    SYSINFO_LOG("[get_windows_machine_host_name]: Exiting function.");
    return hostname;
}


static char* get_windows_machine_unique_id() {
    SYSINFO_LOG("[get_windows_machine_unique_id]: Entering function.");

    static char unique_id[128] = {0};
    HKEY hKey;
    DWORD bufferSize = sizeof(unique_id);
    wchar_t buffer[128];

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"MachineGuid", NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            wcstombs(unique_id, buffer, sizeof(unique_id));  // convert wchar_t to char
            SYSINFO_LOG("[get_windows_machine_unique_id]: Retrieved Machine GUID: %s", unique_id);
        }
        RegCloseKey(hKey);
    }

    if (strlen(unique_id) == 0) {
        strcpy(unique_id, "unknown");
        SYSINFO_LOG("[get_windows_machine_unique_id]: Failed to retrieve Machine GUID, setting to unknown.");
    }

    SYSINFO_LOG("[get_windows_machine_unique_id]: Exiting function.");
    return unique_id;
}


static void get_windows_pretty_version(char* buffer, size_t buffer_size) {
    SYSINFO_LOG("[get_windows_pretty_version]: Entering function.");
    
    RTL_OSVERSIONINFOEXW osvi;
    memset(&osvi, 0, sizeof(RTL_OSVERSIONINFOEXW));
    osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
    
    HMODULE hNtdll = GetModuleHandle(TEXT("ntdll.dll"));
    if (hNtdll == NULL) {
        SYSINFO_LOG("[get_windows_pretty_version]: Failed to get handle to ntdll.dll.");
        snprintf(buffer, buffer_size, "Unknown Windows Version");
        return;
    }

    union {
        FARPROC proc;
        RtlGetVersionPtr func;
    } rtlGetVersionUnion;
    
    rtlGetVersionUnion.proc = GetProcAddress(hNtdll, "RtlGetVersion");
    if (rtlGetVersionUnion.proc == NULL) {
        SYSINFO_LOG("[get_windows_pretty_version]: Failed to get RtlGetVersion function address.");
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

    SYSINFO_LOG("[get_windows_pretty_version]: OS Version detected: %s", buffer);
    SYSINFO_LOG("[get_windows_pretty_version]: Exiting function.");
}


static char* get_windows_build_abi() {
    SYSINFO_LOG("[get_windows_build_abi]: Entering function.");

    static char build_abi[128];
    const char* architecture = sysinfo_cpu_architecture();

    if (!architecture) {
        architecture = "unknown";
    }
  
    snprintf(build_abi, sizeof(build_abi), "%s-little_endian-llp64", architecture);
    SYSINFO_LOG("[get_windows_build_abi]: Final build ABI: %s", build_abi);
    return build_abi;
}


static char** get_sysinfo_list_bluetooth_devices_windows(int* count) {
    SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Entering function.");
    
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
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Found Bluetooth devices.");
        do {
            char** tmp = (char**)realloc(devices, sizeof(char*) * ((size_t)(*count) + 1));
            if (!tmp) {
                SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: realloc failed; stopping enumeration.");
                break;
            }
            devices = tmp;
            wchar_t* wname = deviceInfo.szName;
            int len = WideCharToMultiByte(CP_UTF8, 0, wname, -1, NULL, 0, NULL, NULL);
            if (len <= 0) {
                SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: invalid wchar size, skipping.");
                continue;
            }
            char* name = (char*)malloc((size_t)len);
            if (!name) {
                SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: malloc for name failed; stopping.");
                break;
            }
            WideCharToMultiByte(CP_UTF8, 0, wname, -1, name, len, NULL, NULL);
            devices[*count] = name;
            (*count)++;

            SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Device found: %s", name);
        } while (BluetoothFindNextDevice(hFind, &deviceInfo));

        BluetoothFindDeviceClose(hFind);
    }
    else {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: No Bluetooth devices found.");
    }

    SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Exiting function. Devices found: %d", *count);
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
#include <locale.h>


static char* get_linux_version() {
    SYSINFO_LOG("[get_linux_version]: Retrieving Linux version.");
    
    static char version[128];
    FILE* fp = popen("lsb_release -r | awk '{print $2}'", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_version]: Failed to open command to get Linux version.");
        strcpy(version, "unknown");
        return version;
    }
    
    if (fgets(version, sizeof(version) - 1, fp) != NULL) {
        version[strcspn(version, "\n")] = 0;  // Remove newline
        SYSINFO_LOG("[get_linux_version]: Linux version retrieved: %s", version);
    } 
    else {
        SYSINFO_LOG("[get_linux_version]: Failed to retrieve Linux version.");
        strcpy(version, "unknown");
    }

    pclose(fp);
    return version;
}


static char* get_linux_product_type() {
    SYSINFO_LOG("[get_linux_product_type]: Retrieving Linux product type.");

    static char product_type[128];
    FILE* fp = popen("lsb_release -i | awk -F: '{print $2}'", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_product_type]: Failed to open command to get Linux product type.");
        strcpy(product_type, "unknown");
        return product_type;
    }
    
    if (fgets(product_type, sizeof(product_type) - 1, fp) != NULL) {
        product_type[strcspn(product_type, "\n")] = 0;  // Remove newline

        // Trim whitespace
        char* start = product_type;
        while (*start == ' ' || *start == '\t') {
            start++;
        }
        memmove(product_type, start, strlen(start) + 1);
        SYSINFO_LOG("[get_linux_product_type]: Linux product type retrieved: %s", product_type);
    } 
    else {
        SYSINFO_LOG("[get_linux_product_type]: Failed to retrieve Linux product type.");
        strcpy(product_type, "unknown");
    }

    pclose(fp);
    return product_type;
}


static char* get_linux_kernel_version() {
    SYSINFO_LOG("[get_linux_kernel_version]: Retrieving Linux kernel version.");
    
    static char kernel_version[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_version, buffer.release, sizeof(kernel_version));
        SYSINFO_LOG("[get_linux_kernel_version]: Linux kernel version retrieved: %s", kernel_version);
    } 
    else {
        SYSINFO_LOG("[get_linux_kernel_version]: Failed to retrieve Linux kernel version.");
        strcpy(kernel_version, "unknown");
    }

    return kernel_version;
}


static char* get_linux_kernel_type() {
    SYSINFO_LOG("[get_linux_kernel_type]: Retrieving Linux kernel type.");
    
    static char kernel_type[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_type, buffer.sysname, sizeof(kernel_type));
        SYSINFO_LOG("[get_linux_kernel_type]: Linux kernel type retrieved: %s", kernel_type);
    } 
    else {
        SYSINFO_LOG("[get_linux_kernel_type]: Failed to retrieve Linux kernel type.");
        strcpy(kernel_type, "unknown");
    }

    return kernel_type;
}


static char* get_linux_boot_unique_id() {
    SYSINFO_LOG("[get_linux_boot_unique_id]: Retrieving boot unique ID.");
    
    static char boot_id[128];
    FILE* fp = fopen("/proc/sys/kernel/random/boot_id", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_boot_unique_id]: Failed to open /proc/sys/kernel/random/boot_id.");
        strcpy(boot_id, "unknown");
        return boot_id;
    }

    if (fgets(boot_id, sizeof(boot_id) - 1, fp) != NULL) {
        boot_id[strcspn(boot_id, "\n")] = 0;  // Remove newline
        SYSINFO_LOG("[get_linux_boot_unique_id]: Boot unique ID retrieved: %s", boot_id);
    } 
    else {
        SYSINFO_LOG("[get_linux_boot_unique_id]: Failed to read boot unique ID.");
        strcpy(boot_id, "unknown");
    }

    fclose(fp);
    return boot_id;
}


static char* get_linux_cpu_architecture() {
    SYSINFO_LOG("[get_linux_cpu_architecture]: Retrieving CPU architecture.");
    
    static char architecture[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(architecture, buffer.machine, sizeof(architecture));
        SYSINFO_LOG("[get_linux_cpu_architecture]: CPU architecture retrieved: %s", architecture);
    } 
    else {
        SYSINFO_LOG("[get_linux_cpu_architecture]: Failed to retrieve CPU architecture.");
        strcpy(architecture, "unknown");
    }

    return architecture;
}


static char* get_linux_machine_host_name() {
    SYSINFO_LOG("[get_linux_machine_host_name]: Retrieving machine host name.");
    
    static char hostname[1024];
    hostname[1023] = '\0';

    if (gethostname(hostname, sizeof(hostname) - 1) != 0) {
        SYSINFO_LOG("[get_linux_machine_host_name]: Failed to retrieve hostname using gethostname.");
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    struct addrinfo hints, *info, *p;
    int gai_result;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
        SYSINFO_LOG("[get_linux_machine_host_name]: getaddrinfo failed: %s", gai_strerror(gai_result));
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    for (p = info; p != NULL; p = p->ai_next) {
        if (p->ai_canonname) {
            strncpy(hostname, p->ai_canonname, sizeof(hostname) - 1);
            hostname[sizeof(hostname) - 1] = '\0';
            SYSINFO_LOG("[get_linux_machine_host_name]: Canonical name retrieved: %s", hostname);
            break;
        }
    }

    freeaddrinfo(info);
    return hostname;
}


static char* get_linux_machine_unique_id() {
    SYSINFO_LOG("[get_linux_machine_unique_id]: Retrieving machine unique ID.");
    
    static char unique_id[128];
    FILE* fp = fopen("/etc/machine-id", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_machine_unique_id]: Failed to open /etc/machine-id.");
        strcpy(unique_id, "unknown");
        return unique_id;
    }
    if (fgets(unique_id, sizeof(unique_id) - 1, fp) != NULL) {
        unique_id[strcspn(unique_id, "\n")] = 0;  // Remove newline
        SYSINFO_LOG("[get_linux_machine_unique_id]: Machine ID retrieved: %s", unique_id);
    } 
    else {
        SYSINFO_LOG("[get_linux_machine_unique_id]: Failed to read machine ID.");
        strcpy(unique_id, "unknown");
    }

    fclose(fp);
    return unique_id;
}

static char* get_linux_build_abi() {
    SYSINFO_LOG("[get_linux_build_abi]: Retrieving build ABI information.");

    static char build_abi[128];
    struct utsname buffer;
    const char* machine = "unknown";
    if (uname(&buffer) == 0) {
        machine = buffer.machine;
        SYSINFO_LOG("[get_linux_build_abi]: Machine type retrieved: %s", machine);
    } 
    else {
        SYSINFO_LOG("[get_linux_build_abi]: Failed to retrieve machine type via uname.");
    }

    // Single bounded write — no risk of strcpy+strcat overflow if `machine`
    // returned from the kernel is unusually long.
#if __BYTE_ORDER == __LITTLE_ENDIAN
    const char* endian = "little_endian";
#else
    const char* endian = "big_endian";
#endif
    snprintf(build_abi, sizeof(build_abi), "%s-%s-lp64", machine, endian);
    SYSINFO_LOG("[get_linux_build_abi]: Build ABI: %s", build_abi);

    return build_abi;
}


char** get_sysinfo_list_bluetooth_devices_linux(int* count) {
    SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Retrieving Bluetooth devices.");
    
    *count = 0;
    char** devices = NULL;
    FILE* pipe = popen("hcitool dev", "r");

    if (pipe) {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Successfully opened pipe.");

        char buffer[128];
        while (fgets(buffer, 128, pipe) != NULL) {
            buffer[strcspn(buffer, "\r\n")] = 0;  // Remove newline characters
            SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Found device: %s", buffer);

            // Grow into a temporary so we can recover from realloc failure
            // without leaking the previously-allocated strings.
            char** tmp = (char**)realloc(devices, sizeof(char*) * ((size_t)(*count) + 1));
            if (!tmp) {
                SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: realloc failed; stopping.");
                break;
            }
            devices = tmp;
            char* dup = strdup(buffer);
            if (!dup) {
                SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: strdup failed; stopping.");
                break;
            }
            devices[*count] = dup;
            (*count)++;
        }

        pclose(pipe);
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Total devices found: %d", *count);
    }
    else {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Failed to open pipe.");
    }

    return devices;
}

#else

static char* get_unknown_version() {
    SYSINFO_LOG("[get_unknown_version]: Returning unknown version.");
    static char version[] = "unknown";

    return version;
}


static char* get_unknown_product_type() {
    SYSINFO_LOG("[get_unknown_product_type]: Returning unknown product type.");
    static char product_type[] = "unknown";

    return product_type;
}


static char* get_unknown_kernel_version() {
    SYSINFO_LOG("[get_unknown_kernel_version]: Returning unknown kernel version.");
    static char kernel_version[] = "unknown";

    return kernel_version;
}


static char* get_unknown_kernel_type() {
    SYSINFO_LOG("[get_unknown_kernel_type]: Returning unknown kernel type.");
    static char kernel_type[] = "unknown";

    return kernel_type;
}


static char* get_unknown_boot_unique_id() {
    SYSINFO_LOG("[get_unknown_boot_unique_id]: Returning empty boot unique ID.");
    static char boot_id[] = "";

    return boot_id;
}


static char* get_unknown_cpu_architecture() {
    SYSINFO_LOG("[get_unknown_cpu_architecture]: Returning unknown CPU architecture.");
    static char architecture[] = "unknown";

    return architecture;
}


static char* get_unknown_machine_host_name() {
    SYSINFO_LOG("[get_unknown_machine_host_name]: Returning unknown host name.");
    static char hostname[] = "Unknown_Host_Name";

    return hostname;
}


static char* get_unknown_machine_unique_id() {
    SYSINFO_LOG("[get_unknown_machine_unique_id]: Returning unknown machine unique ID.");
    static char unique_id[] = "unknown";

    return unique_id;
}


static char* get_unknown_build_abi() {
    SYSINFO_LOG("[get_unknown_build_abi]: Returning unknown build ABI.");
    static char build_abi[] = "unknown";

    return build_abi;
}


char** sysinfo_list_bluetooth_devices(int* count) {
    SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: No Bluetooth devices found.");
    *count = 0;

    return NULL;
}

#endif


/**
 * @brief Return the OS version (release) string.
 *
 * Cross-platform dispatcher:
 *   - Windows: `get_windows_version()` (uses `GetVersion`).
 *   - Linux:   `get_linux_version()` (parses `lsb_release -r`).
 *   - Other:   `"unknown"`.
 *
 * @return Pointer to a static (do NOT free) string. Never NULL.
 */
char* sysinfo_product_version(void) {
    SYSINFO_LOG("[sysinfo_product_version]: enter");
#ifdef _WIN32
    char* version = get_windows_version();
    SYSINFO_LOG("[sysinfo_product_version]: exit (Windows) -> %s", version);

    return version;
#elif __linux__
    char* version = get_linux_version();
    SYSINFO_LOG("[sysinfo_product_version]: exit (Linux) -> %s", version);

    return version;
#else
    char* version = get_unknown_version();
    SYSINFO_LOG("[sysinfo_product_version]: exit (unsupported platform) -> %s", version);

    return version;
#endif
}


/**
 * @brief Return the OS product type / distribution name.
 *
 * Cross-platform dispatcher: returns `"Windows"` on Windows, the
 * distributor ID (e.g. `"Ubuntu"`) on Linux via `lsb_release -i`, or
 * `"unknown"` on other platforms.
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_product_type(void) {
    SYSINFO_LOG("[sysinfo_product_type]: enter");
#ifdef _WIN32
    char* product_type = get_windows_product_type();
#elif __linux__
    char* product_type = get_linux_product_type();
#else
    char* product_type = get_unknown_product_type();
#endif
    SYSINFO_LOG("[sysinfo_product_type]: exit -> %s", product_type ? product_type : "unknown");
    return product_type;
}


/**
 * @brief Return the kernel version string (e.g. "10.0.26100" /
 *        "6.8.0-35-generic").
 *
 * On Windows uses `GetVersionEx`; on Linux uses `uname.release`.
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_kernel_version(void) {
    SYSINFO_LOG("[sysinfo_kernel_version]: enter");
#ifdef _WIN32
    char* kernel_version = get_windows_kernel_version();
#elif __linux__
    char* kernel_version = get_linux_kernel_version();
#else
    char* kernel_version = get_unknown_kernel_version();
#endif
    SYSINFO_LOG("[sysinfo_kernel_version]: exit -> %s", kernel_version ? kernel_version : "unknown");
    return kernel_version;
}


/**
 * @brief Return the kernel type identifier (e.g. "Windows NT", "Linux").
 *
 * Useful for branching on a coarse OS family without parsing version
 * strings.
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_kernel_type(void) {
    SYSINFO_LOG("[sysinfo_kernel_type]: enter");
#ifdef _WIN32
    char* kernel_type = get_windows_kernel_type();
#elif __linux__
    char* kernel_type = get_linux_kernel_type();
#else
    char* kernel_type = get_unknown_kernel_type();
#endif
    SYSINFO_LOG("[sysinfo_kernel_type]: exit -> %s", kernel_type ? kernel_type : "unknown");
    return kernel_type;
}


/**
 * @brief Return a unique identifier for the current boot session.
 *
 *   - Linux:   `/proc/sys/kernel/random/boot_id`.
 *   - Windows: an integer derived from `GetTickCount64()`.
 *   - Other:   the empty string.
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_boot_unique_id(void) {
    SYSINFO_LOG("[sysinfo_boot_unique_id]: enter");
#ifdef _WIN32
    char* boot_id = get_windows_boot_unique_id();
#elif __linux__
    char* boot_id = get_linux_boot_unique_id();
#else
    char* boot_id = get_unknown_boot_unique_id();
#endif
    SYSINFO_LOG("[sysinfo_boot_unique_id]: exit -> %s", boot_id ? boot_id : "unknown");
    return boot_id;
}


/**
 * @brief Return the CPU architecture string ("x86_64", "ARM", ...).
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_cpu_architecture(void) {
    SYSINFO_LOG("[sysinfo_cpu_architecture]: enter");
#ifdef _WIN32
    char* cpu_arch = get_windows_cpu_architecture();
#elif __linux__
    char* cpu_arch = get_linux_cpu_architecture();
#else
    char* cpu_arch = get_unknown_cpu_architecture();
#endif
    SYSINFO_LOG("[sysinfo_cpu_architecture]: exit -> %s", cpu_arch ? cpu_arch : "unknown");
    return cpu_arch;
}


/**
 * @brief Return the machine host name (or fully-qualified domain name
 *        where the OS exposes one).
 *
 * On Linux walks `getaddrinfo(AI_CANONNAME)` to surface the FQDN; on
 * Windows returns `GetComputerName()`.
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_machine_host_name(void) {
    SYSINFO_LOG("[sysinfo_machine_host_name]: enter");
#ifdef _WIN32
    char* host_name = get_windows_machine_host_name();
#elif __linux__
    char* host_name = get_linux_machine_host_name();
#else
    char* host_name = get_unknown_machine_host_name();
#endif
    SYSINFO_LOG("[sysinfo_machine_host_name]: exit -> %s", host_name ? host_name : "unknown");
    return host_name;
}


/**
 * @brief Return a persistent unique identifier for the machine itself.
 *
 *   - Windows: registry GUID at HKLM\\SOFTWARE\\Microsoft\\Cryptography.
 *   - Linux:   contents of /etc/machine-id.
 *   - Other:   "unknown".
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_machine_unique_id(void) {
    SYSINFO_LOG("[sysinfo_machine_unique_id]: enter");
#ifdef _WIN32
    char* id = get_windows_machine_unique_id();
#elif __linux__
    char* id = get_linux_machine_unique_id();
#else
    char* id = get_unknown_machine_unique_id();
#endif
    SYSINFO_LOG("[sysinfo_machine_unique_id]: exit -> %s", id ? id : "unknown");
    return id;
}


/**
 * @brief Return a human-readable "<product> Version <version> (Build N)"
 *        string suitable for display.
 *
 * Windows uses RtlGetVersion + a tiny build-name map. Linux concatenates
 * `<product_type> <product_version>`. Other platforms fall back to
 * `<kernel_type> <kernel_version>`.
 *
 * @return Pointer to a static buffer. Never NULL.
 */
char* sysinfo_pretty_product_name(void) {
    static char pretty_name[256];
    memset(pretty_name, 0, sizeof(pretty_name));
    SYSINFO_LOG("[sysinfo_pretty_product_name]: enter");

#ifdef _WIN32
    get_windows_pretty_version(pretty_name, sizeof(pretty_name));
#elif __linux__
    snprintf(pretty_name, sizeof(pretty_name), "%s %s",
             sysinfo_product_type(), sysinfo_product_version());
#else
    snprintf(pretty_name, sizeof(pretty_name), "%s %s",
             sysinfo_kernel_type(), sysinfo_kernel_version());
#endif

    SYSINFO_LOG("[sysinfo_pretty_product_name]: exit -> %s", pretty_name);
    return pretty_name;
}


/**
 * @brief Return an ABI descriptor combining architecture + endianness +
 *        data-model (e.g. "x86_64-little_endian-llp64" on 64-bit Win,
 *        "x86_64-little_endian-lp64" on 64-bit Linux).
 *
 * @return Pointer to a static string. Never NULL.
 */
char* sysinfo_build_abi(void) {
    SYSINFO_LOG("[sysinfo_build_abi]: enter");
#ifdef _WIN32
    char* abi = get_windows_build_abi();
#elif __linux__
    char* abi = get_linux_build_abi();
#else
    char* abi = get_unknown_build_abi();
#endif
    SYSINFO_LOG("[sysinfo_build_abi]: exit -> %s", abi ? abi : "unknown");
    return abi;
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
/**
 * @brief Enumerate Bluetooth devices known to the system.
 *
 * Wraps `BluetoothFindFirstDevice` / `BluetoothFindNextDevice` on
 * Windows and `popen("hcitool dev")` on Linux. The number of devices
 * is written to @p count; the returned array contains exactly that
 * many `char*` entries.
 *
 * Ownership: every device-name string AND the outer array are
 * heap-allocated. The caller must `free()` each entry and then
 * `free()` the outer pointer.
 *
 * @param count Out-arg receiving the number of devices found (0 if
 *              the system has no Bluetooth hardware / driver). Must
 *              be non-NULL.
 * @return Heap-allocated `char**` (NOT NULL-terminated), or NULL on
 *         platforms without a backend / when no devices were found.
 */
char** sysinfo_list_bluetooth_devices(int* count) {
    SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: enter count=%p", (void*)count);
#ifdef _WIN32
    char** devices = get_sysinfo_list_bluetooth_devices_windows(count);
#elif __linux__
    char** devices = get_sysinfo_list_bluetooth_devices_linux(count);
#else
    if (count) {
        *count = 0;
    }
    char** devices = NULL;
#endif

    if (devices) {
        SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: exit -> %d device(s)",
                    count ? *count : 0);
    } 
    else {
        SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: exit -> no devices");
    }
    return devices;
}

#ifdef _WIN32
static double get_sysinfo_cpu_usage_windows() {
    SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: Retrieving CPU usage.");

    FILETIME idleTime, kernelTime, userTime;

    if (GetSystemTimes(&idleTime, &kernelTime, &userTime) == 0) {
        SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: Error - Failed to get CPU times.");
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
        SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: CPU usage is 0 (no difference in times).");
        return 0.0;
    }

    double cpuUsage = (double)((kernelDiff + userDiff - idleDiff) * 100.0) / (kernelDiff + userDiff);
    SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: CPU usage is %.2f%%", cpuUsage);

    return cpuUsage;
}


// Windows specific memory usage function
static double get_sysinfo_memory_usage_windows() {
    SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Retrieving memory usage.");

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo) == 0) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Error - Failed to retrieve memory status.");
        return -1.0;
    }

    DWORDLONG totalMemory = memInfo.ullTotalPhys;
    DWORDLONG usedMemory = totalMemory - memInfo.ullAvailPhys;

    if (totalMemory == 0) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Error - Total memory is zero.");
        return -1.0;
    }

    double memoryUsage = (double)(usedMemory * 100.0) / totalMemory;
    SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Memory usage is %.2f%%", memoryUsage);

    return memoryUsage;
}


static char* get_sysinfo_disk_space_windows(const char* path) {
    SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Retrieving disk space for path: %s", path);

    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;

    if (GetDiskFreeSpaceExA(path, &freeBytesAvailable, &totalBytes, &totalFreeBytes) == 0) {
        SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Error - Failed to get disk space information for path: %s", path);
        return NULL;
    }

    char* result = (char*)malloc(256);
    if (result == NULL) {
        SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 256, "Total: %llu MB, Used: %llu MB, Available: %llu MB",
             totalBytes.QuadPart / (1024 * 1024),
             (totalBytes.QuadPart - totalFreeBytes.QuadPart) / (1024 * 1024),
             totalFreeBytes.QuadPart / (1024 * 1024));

    SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Successfully retrieved disk space info: %s", result);
    return result;
}


// Simpler Windows-specific function to get system uptime
static char* get_sysinfo_system_uptime_windows() {
    SYSINFO_LOG("[get_sysinfo_system_uptime_windows]: Retrieving system uptime.");

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
        SYSINFO_LOG("[get_sysinfo_system_uptime_windows]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 128, "%lu days, %lu hours, %lu minutes, %lu seconds", days, hours, minutes, seconds);
    SYSINFO_LOG("[get_sysinfo_system_uptime_windows]: Successfully retrieved uptime: %s", result);

    return result;
}


static void get_running_services_windows(Vector *services) {
    SYSINFO_LOG("[get_running_services_windows]: Retrieving running services.");

    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scm) {
        SYSINFO_LOG("[get_running_services_windows]: Error - Unable to open Service Control Manager.");
        return;
    }

    DWORD bytesNeeded = 0, servicesReturned = 0, resumeHandle = 0;
    EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &servicesReturned, &resumeHandle);

    LPENUM_SERVICE_STATUS serviceStatus = (LPENUM_SERVICE_STATUS)malloc(bytesNeeded);
    if (!serviceStatus) {
        SYSINFO_LOG("[get_running_services_windows]: Error - Memory allocation failed for service status.");
        CloseServiceHandle(scm);
        return;
    }

    if (EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_STATE_ALL, serviceStatus, bytesNeeded, &bytesNeeded, &servicesReturned, &resumeHandle)) {
        for (DWORD i = 0; i < servicesReturned; i++) {
            char *serviceName = strdup(serviceStatus[i].lpServiceName);  // Allocate memory for service name
            if (serviceName) {
                vector_push_back(services, &serviceName);
                SYSINFO_LOG("[get_running_services_windows]: Added service: %s", serviceName);
            } 
            else {
                SYSINFO_LOG("[get_running_services_windows]: Error - Memory allocation failed for service name.");
            }
        }
    } 
    else {
        SYSINFO_LOG("[get_running_services_windows]: Error - Failed to enumerate services.");
    }

    free(serviceStatus);
    CloseServiceHandle(scm);
    SYSINFO_LOG("[get_running_services_windows]: Finished retrieving running services.");
}


static void get_open_ports_windows(Vector *ports) {
    SYSINFO_LOG("[get_open_ports_windows]: Retrieving open TCP ports.");

    PMIB_TCPTABLE tcpTable = NULL;
    DWORD size = 0;

    if (GetTcpTable(NULL, &size, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
        tcpTable = (MIB_TCPTABLE*)malloc(size);
        if (!tcpTable) {
            SYSINFO_LOG("[get_open_ports_windows]: Error - Memory allocation failed for TCP table.");
            return;
        }
    }

    if (GetTcpTable(tcpTable, &size, TRUE) == NO_ERROR) {
        for (DWORD i = 0; i < tcpTable->dwNumEntries; i++) {
            DWORD port = ntohs((u_short)tcpTable->table[i].dwLocalPort);
            int *port_ptr = (int*)malloc(sizeof(int));
            if (port_ptr) {
                *port_ptr = (int)port;
                vector_push_back(ports, &port_ptr);
                SYSINFO_LOG("[get_open_ports_windows]: Added port %d to the list.", *port_ptr);
            } 
            else {
                SYSINFO_LOG("[get_open_ports_windows]: Error - Memory allocation failed for port.");
            }
        }
    } 
    else {
        SYSINFO_LOG("[get_open_ports_windows]: Error - Unable to retrieve TCP table.");
    }

    free(tcpTable);
    SYSINFO_LOG("[get_open_ports_windows]: Finished retrieving open TCP ports.");
}


static void get_disk_partitions_windows(Vector *partitions) {
    SYSINFO_LOG("[get_disk_partitions_windows]: Retrieving disk partitions.");

    DWORD drive_mask = GetLogicalDrives();
    if (drive_mask == 0) {
        SYSINFO_LOG("[get_disk_partitions_windows]: Error - Unable to retrieve logical drives.");
        return;
    }

    char drive_letter[4] = "A:\\";
    for (int i = 0; i < 26; ++i) {
        if (drive_mask & (1 << i)) {
            drive_letter[0] = 'A' + i;

            ULARGE_INTEGER free_bytes_available, total_bytes, total_free_bytes;
            if (GetDiskFreeSpaceExA(drive_letter, &free_bytes_available, &total_bytes, &total_free_bytes)) {
                SysinfoDiskPartition partition;
                partition.mount_point = strdup(drive_letter);
                partition.total_size = total_bytes.QuadPart;  
                partition.free_space = total_free_bytes.QuadPart;

                vector_push_back(partitions, &partition);
                SYSINFO_LOG("[get_disk_partitions_windows]: Added partition %s (Total: %llu, Free: %llu).",
                            partition.mount_point, partition.total_size, partition.free_space);
            } 
            else {
                SYSINFO_LOG("[get_disk_partitions_windows]: Error - Unable to retrieve disk space for drive %s.", drive_letter);
            }
        }
    }

    SYSINFO_LOG("[get_disk_partitions_windows]: Finished retrieving disk partitions.");
}

#elif __linux__

// Linux specific CPU usage function
static double get_sysinfo_cpu_usage_linux() {
    SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Retrieving CPU usage.");

    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Error - Failed to open /proc/stat.");
        return -1.0;
    }

    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
    if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8) {
        fclose(fp);
        SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Error - Failed to read CPU stats from /proc/stat.");
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
        SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Warning - Total CPU diff is zero.");
        return 0.0;
    }

    double usage = (double)((totalDiff - idleDiff) * 100.0) / totalDiff;
    SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: CPU usage: %.2f%%", usage);

    return usage;
}


// Linux specific memory usage function
static double get_sysinfo_memory_usage_linux() {
    SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Retrieving memory usage.");

    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Error - Failed to open /proc/meminfo.");
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
        SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Error - Failed to retrieve total or available memory.");
        return -1.0;
    }

    unsigned long usedMemory = totalMemory - availableMemory;
    double usage = (double)(usedMemory * 100.0) / totalMemory;

    SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Memory usage: %.2f%%", usage);

    return usage;
}


static char* get_sysinfo_disk_space_linux(const char* path) {
    /* Guard NULL / empty path: statvfs(NULL, ...) dereferences the path and
       reads unaddressable memory (valgrind: "statfs(path) points to
       unaddressable byte(s)"). Treat a missing path as a normal failure. */
    if (!path || !*path) {
        SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Error - NULL/empty path.");
        return NULL;
    }
    SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Retrieving disk space for path: %s", path);

    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) {
        SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Error - Failed to get disk space information for path: %s", path);
        return NULL;
    }

    unsigned long totalBytes = vfs.f_blocks * vfs.f_frsize;
    unsigned long availableBytes = vfs.f_bavail * vfs.f_frsize;
    unsigned long usedBytes = totalBytes - availableBytes;

    char* result = (char*)malloc(256);
    if (result == NULL) {
        SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 256, "Total: %lu MB, Used: %lu MB, Available: %lu MB",
             totalBytes / (1024 * 1024), usedBytes / (1024 * 1024), availableBytes / (1024 * 1024));

    SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Disk space retrieved successfully for path: %s", path);
    return result;
}


static char* get_sysinfo_system_uptime_linux() {
    SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Retrieving system uptime.");

    FILE* fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Error - Failed to open /proc/uptime.");
        return NULL;
    }

    double uptimeSeconds = 0.0;
    if (fscanf(fp, "%lf", &uptimeSeconds) != 1) {
        fclose(fp);
        SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Error - Failed to read uptime from /proc/uptime.");
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
        SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 128, "%lu days, %lu hours, %lu minutes, %lu seconds", days, hours, minutes, seconds);

    SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: System uptime retrieved successfully.");
    return result;
}


static void get_running_services_linux(Vector *services) {
    SYSINFO_LOG("[get_running_services_linux]: Running systemctl to list running services.");
    
    FILE *fp = popen("systemctl list-units --type=service --state=running --no-pager --no-legend", "r");
    if (!fp) {
        SYSINFO_LOG("[get_running_services_linux]: Error - Failed to run systemctl command.");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        char serviceName[256];
        sscanf(buffer, "%255s", serviceName);

        SYSINFO_LOG("[get_running_services_linux]: Found service: %s", serviceName);

        char *service = strdup(serviceName); 
        vector_push_back(services, &service);
    }

    pclose(fp);
    SYSINFO_LOG("[get_running_services_linux]: Completed listing running services.");
}


static void get_open_ports_linux(Vector *ports) {
    SYSINFO_LOG("[get_open_ports_linux]: Running ss command to list open ports.");
    
    FILE *fp = popen("ss -lntu | awk 'NR>1 {print $5}'", "r");
    if (!fp) {
        SYSINFO_LOG("[get_open_ports_linux]: Error - Failed to run ss command.");
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp)) {
        char *ip_port = strchr(buffer, ':');
        if (ip_port) {
            int port;
            sscanf(ip_port + 1, "%d", &port);  // Extract the port number
            SYSINFO_LOG("[get_open_ports_linux]: Found open port: %d", port);

            // Check malloc — the previous code dereferenced port_ptr
            // unconditionally and would segfault on OOM.
            int *port_ptr = (int*)malloc(sizeof(int));
            if (!port_ptr) {
                SYSINFO_LOG("[get_open_ports_linux]: malloc failed for port; skipping.");
                continue;
            }
            *port_ptr = port;
            vector_push_back(ports, &port_ptr);
        }
    }

    pclose(fp);
    SYSINFO_LOG("[get_open_ports_linux]: Completed listing open ports.");
}


// Linux specific virtualization detection remains the same
static bool check_virtualization_in_cpuinfo() {
    SYSINFO_LOG("[check_virtualization_in_cpuinfo]: Checking /proc/cpuinfo for hypervisor.");
    
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (!cpuinfo) {
        SYSINFO_LOG("[check_virtualization_in_cpuinfo]: Error - Failed to open /proc/cpuinfo.");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), cpuinfo)) {
        if (strstr(line, "hypervisor")) {
            SYSINFO_LOG("[check_virtualization_in_cpuinfo]: Virtualization detected in /proc/cpuinfo.");
            fclose(cpuinfo);
            return true;  // Found hypervisor flag in /proc/cpuinfo
        }
    }

    fclose(cpuinfo);
    SYSINFO_LOG("[check_virtualization_in_cpuinfo]: No virtualization detected in /proc/cpuinfo.");
    return false;
}


static bool check_virtualization_with_systemd() {
    SYSINFO_LOG("[check_virtualization_with_systemd]: Running systemd-detect-virt.");
    
    FILE *fp = popen("systemd-detect-virt", "r");
    if (!fp) {
        SYSINFO_LOG("[check_virtualization_with_systemd]: Error - Failed to run systemd-detect-virt.");
        return false;
    }

    char buffer[256];
    bool is_virtualized = false;
    if (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "none", 4) != 0) {
            SYSINFO_LOG("[check_virtualization_with_systemd]: Virtualization detected via systemd.");
            is_virtualized = true;  // systemd-detect-virt detected virtualization
        } 
        else {
            SYSINFO_LOG("[check_virtualization_with_systemd]: No virtualization detected via systemd.");
        }
    }

    pclose(fp);
    return is_virtualized;
}


/**
 * @brief Return true if the process is running inside a virtual
 *        machine / hypervisor.
 *
 * Backend:
 *   - Windows: `IsProcessorFeaturePresent(PF_VIRT_FIRMWARE_ENABLED)`
 *     then `PF_HYPERVISOR_PRESENT` (if the SDK exposes it).
 *   - Linux:   greps `/proc/cpuinfo` for the `hypervisor` flag and
 *     also falls back to `systemd-detect-virt`.
 *   - Other:   returns false.
 *
 * @code
 *     if (sysinfo_is_virtualized()) {
 *         puts("running inside a VM");
 *     }
 * @endcode
 *
 * @return true if any backend reports virtualization, false otherwise.
 */
bool sysinfo_is_virtualized(void) {
    SYSINFO_LOG("[sysinfo_is_virtualized/linux]: enter");

    bool cpu_virtualization = check_virtualization_in_cpuinfo();
    bool systemd_virtualization = check_virtualization_with_systemd();

    if (cpu_virtualization) {
        SYSINFO_LOG("[sysinfo_is_virtualized/linux]: virtualization detected via CPU info");
    }
    else if (systemd_virtualization) {
        SYSINFO_LOG("[sysinfo_is_virtualized/linux]: virtualization detected via systemd");
    }
    else {
        SYSINFO_LOG("[sysinfo_is_virtualized/linux]: no virtualization detected");
    }

    bool out = cpu_virtualization || systemd_virtualization;
    SYSINFO_LOG("[sysinfo_is_virtualized/linux]: exit -> %s",
                out ? "true" : "false");
    return out;
}

static void get_disk_partitions_linux(Vector *partitions) {
    SYSINFO_LOG("[get_disk_partitions_linux]: Entering function.");
    
    FILE *fp = fopen("/proc/mounts", "r");
    if (!fp) {
        SYSINFO_LOG("[get_disk_partitions_linux]: Error - Failed to open /proc/mounts.");
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char device[256], mount_point[256], fs_type[256];
        sscanf(line, "%255s %255s %255s", device, mount_point, fs_type);

        // Skip non-physical partitions (e.g., tmpfs, devtmpfs)
        if (strcmp(fs_type, "tmpfs") == 0 || strcmp(fs_type, "devtmpfs") == 0) {
            SYSINFO_LOG("[get_disk_partitions_linux]: Skipping non-physical partition: %s", fs_type);
            continue;
        }

        struct statvfs vfs;
        if (statvfs(mount_point, &vfs) == 0) {
            unsigned long total = (unsigned long)vfs.f_blocks * (unsigned long)vfs.f_frsize;
            unsigned long avail = (unsigned long)vfs.f_bavail * (unsigned long)vfs.f_frsize;

            /* Skip pseudo / zero-capacity filesystems (proc, sysfs, cgroup,
               tmpfs, ...): they are not real disk partitions and report a
               total size of 0 (and sometimes free > total). This keeps the
               result to actual storage partitions, matching the Windows side. */
            if (total == 0) {
                SYSINFO_LOG("[get_disk_partitions_linux]: Skipping zero-size pseudo fs at %s", mount_point);
                continue;
            }
            if (avail > total) {
                avail = total;   /* defensive: never report free > total */
            }

            SysinfoDiskPartition partition;
            partition.mount_point = strdup(mount_point);
            partition.total_size = total;   // Total size in bytes
            partition.free_space = avail;   // Available space in bytes

            SYSINFO_LOG("[get_disk_partitions_linux]: Found partition at mount point: %s", mount_point);
            vector_push_back(partitions, &partition);
        }
        else {
            SYSINFO_LOG("[get_disk_partitions_linux]: Error - Failed to get stats for mount point: %s", mount_point);
        }
    }

    fclose(fp);
    SYSINFO_LOG("[get_disk_partitions_linux]: Exiting function.");
}

#endif

/**
 * @brief Return the current CPU usage as a percentage in [0, 100].
 *
 * The first call seeds the previous-times baseline; subsequent calls
 * compute usage relative to that baseline. So treat the very first
 * call as a "prime" and read the second call for a meaningful value.
 *
 * Backend:
 *   - Windows: GetSystemTimes (idle / kernel / user).
 *   - Linux:   parses the `cpu` row of /proc/stat.
 *   - Other:   returns -1.0.
 *
 * @return Percentage in [0, 100] on success, -1.0 on failure /
 *         unsupported platform.
 */
double sysinfo_cpu_usage(void) {
    SYSINFO_LOG("[sysinfo_cpu_usage]: enter");

#ifdef _WIN32
    double cpu_usage = get_sysinfo_cpu_usage_windows();
#elif __linux__
    double cpu_usage = get_sysinfo_cpu_usage_linux();
#else
    /* Unsupported platform*/
    double cpu_usage = -1.0;
#endif
    if (cpu_usage < 0) {
        SYSINFO_LOG("[sysinfo_cpu_usage]: exit (failure) -> -1.0");
    } 
    else {
        SYSINFO_LOG("[sysinfo_cpu_usage]: exit -> %.2f%%", cpu_usage);
    }
    return cpu_usage;
}


/**
 * @brief Return memory usage as a percentage of total physical RAM in
 *        [0, 100].
 *
 * Backend:
 *   - Windows: GlobalMemoryStatusEx.
 *   - Linux:   `MemTotal` / `MemAvailable` from /proc/meminfo.
 *   - Other:   -1.0.
 *
 * @return Percentage in [0, 100] on success, -1.0 on failure.
 */
double sysinfo_memory_usage(void) {
    SYSINFO_LOG("[sysinfo_memory_usage]: enter");

#ifdef _WIN32
    double memory_usage = get_sysinfo_memory_usage_windows();
#elif __linux__
    double memory_usage = get_sysinfo_memory_usage_linux();
#else
    double memory_usage = -1.0;
#endif
    if (memory_usage < 0) {
        SYSINFO_LOG("[sysinfo_memory_usage]: exit (failure) -> -1.0");
    } 
    else {
        SYSINFO_LOG("[sysinfo_memory_usage]: exit -> %.2f%%", memory_usage);
    }
    return memory_usage;
}


/**
 * @brief Return a "Total: N MB, Used: U MB, Available: A MB" string for
 *        the filesystem containing @p path.
 *
 * Backend:
 *   - Windows: GetDiskFreeSpaceExA.
 *   - Linux:   statvfs.
 *   - Other:   NULL.
 *
 * @param path  Any path on the filesystem to query. NULL produces NULL
 *              (no crash).
 * @return Heap-allocated string the caller must `free()`, or NULL on
 *         failure / unsupported platform.
 */
char* sysinfo_disk_space(const char* path) {
    SYSINFO_LOG("[sysinfo_disk_space]: enter path=%s",
                path ? path : "(null)");

#ifdef _WIN32
    char* disk_space = get_sysinfo_disk_space_windows(path);
#elif __linux__
    char* disk_space = get_sysinfo_disk_space_linux(path);
#else
    (void)path;
    char* disk_space = NULL;
#endif
    if (!disk_space) {
        SYSINFO_LOG("[sysinfo_disk_space]: exit (failure) -> NULL");
    } 
    else {
        SYSINFO_LOG("[sysinfo_disk_space]: exit -> %s", disk_space);
    }
    return disk_space;
}


/**
 * @brief Return the system uptime since boot, formatted as
 *        "N days, H hours, M minutes, S seconds".
 *
 * Backend:
 *   - Windows: `GetTickCount64()`.
 *   - Linux:   `/proc/uptime`.
 *   - Other:   NULL.
 *
 * @return Heap-allocated string the caller must `free()`, or NULL on
 *         failure / unsupported platform.
 */
char* sysinfo_system_uptime(void) {
    SYSINFO_LOG("[sysinfo_system_uptime]: enter");

#ifdef _WIN32
    char* uptime = get_sysinfo_system_uptime_windows();
#elif __linux__
    char* uptime = get_sysinfo_system_uptime_linux();
#else
    char* uptime = NULL;
#endif
    if (!uptime) {
        SYSINFO_LOG("[sysinfo_system_uptime]: exit (failure) -> NULL");
    } 
    else {
        SYSINFO_LOG("[sysinfo_system_uptime]: exit -> %s", uptime);
    }
    return uptime;
}


/**
 * @brief Enumerate currently-running services / daemons.
 *
 * Backend:
 *   - Windows: SCM `EnumServicesStatus`.
 *   - Linux:   `systemctl list-units --type=service --state=running`.
 *
 * Ownership: returned `Vector` holds `char*` entries that were strdup'd
 * by the backend. The caller must free each entry AND `vector_deallocate`
 * the Vector itself.
 *
 * @return Heap-allocated Vector (possibly empty), or NULL on
 *         vector_create failure.
 */
Vector* sysinfo_running_services(void) {
    SYSINFO_LOG("[sysinfo_running_services]: enter");

    Vector* services = vector_create(sizeof(char*));
    if (!services) {
        SYSINFO_LOG("[sysinfo_running_services]: vector_create failed -> NULL");
        return NULL;
    }

#ifdef _WIN32
    get_running_services_windows(services);
#elif __linux__
    get_running_services_linux(services);
#endif

    SYSINFO_LOG("[sysinfo_running_services]: exit -> %zu service(s)",
                vector_size(services));
    return services;
}


/**
 * @brief Return the number of logical CPU cores available to the process.
 *
 * Backend:
 *   - Windows: `GetSystemInfo().dwNumberOfProcessors`.
 *   - Linux:   `sysconf(_SC_NPROCESSORS_ONLN)`.
 *   - Other:   -1.
 *
 * @return Positive core count on success, -1 on failure / unsupported
 *         platform.
 */
int sysinfo_cpu_cores(void) {
    SYSINFO_LOG("[sysinfo_cpu_cores]: Entering function.");

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_cpu_cores]: Fetching CPU core count on Windows.");

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int cores = sysinfo.dwNumberOfProcessors;

    SYSINFO_LOG("[sysinfo_cpu_cores]: Number of CPU cores on Windows: %d.", cores);
    return cores;

#elif __linux__
    SYSINFO_LOG("[sysinfo_cpu_cores]: Fetching CPU core count on Linux.");

    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) {
        SYSINFO_LOG("[sysinfo_cpu_cores]: Error - Failed to get CPU core count on Linux.");
        return -1; 
    }

    SYSINFO_LOG("[sysinfo_cpu_cores]: Number of CPU cores on Linux: %ld.", nprocs);
    return (int)nprocs;

#else
    SYSINFO_LOG("[sysinfo_cpu_cores]: Unsupported platform, returning -1.");
    return -1;
#endif
}


/**
 * @brief Return a Vector of currently-running process names.
 *
 * Each element is a `char*` allocated with `strdup`. The Vector itself
 * is allocated with `vector_create`. The caller is responsible for
 * freeing each string and then the Vector — typically:
 *
 * @code
 *     Vector* procs = sysinfo_process_list();
 *     for (size_t i = 0; i < vector_size(procs); ++i) {
 *         char** s = vector_at(procs, i);
 *         puts(*s);
 *         free(*s);
 *     }
 *     vector_deallocate(procs);
 * @endcode
 *
 * Backend:
 *   - Windows: Toolhelp32 snapshot (Process32First / Process32Next).
 *   - Linux:   walks the numeric subdirs of /proc and reads `comm`.
 *   - Other:   returns an empty Vector.
 *
 * @return Newly-allocated Vector. May be empty but is never NULL on
 *         success. Returns NULL only if the Vector itself could not
 *         be allocated.
 */
Vector* sysinfo_process_list(void) {
    SYSINFO_LOG("[sysinfo_process_list]: enter");

    Vector* processes = vector_create(sizeof(char*));
    if (!processes) {
        SYSINFO_LOG("[sysinfo_process_list]: failed to create vector -> NULL");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_process_list]: backend=windows");

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        SYSINFO_LOG("[sysinfo_process_list]: CreateToolhelp32Snapshot failed -> returning empty vector");
        return processes;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        SYSINFO_LOG("[sysinfo_process_list]: Process32First failed -> returning empty vector");
        CloseHandle(hProcessSnap);
        return processes;
    }

    do {
        char* processName = strdup(pe32.szExeFile);
        if (processName) {
            vector_push_back(processes, &processName);
            SYSINFO_LOG("[sysinfo_process_list]: added '%s'", processName);
        }
        else {
            SYSINFO_LOG("[sysinfo_process_list]: strdup failed for '%s'", pe32.szExeFile);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

#elif __linux__
    SYSINFO_LOG("[sysinfo_process_list]: backend=linux");

    DIR* procDir = opendir("/proc");
    if (!procDir) {
        SYSINFO_LOG("[sysinfo_process_list]: opendir(/proc) failed -> returning empty vector");
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != NULL) {
        if (entry->d_type == DT_DIR && strspn(entry->d_name, "0123456789") == strlen(entry->d_name)) {
            char procPath[300];
            snprintf(procPath, sizeof(procPath), "/proc/%s/comm", entry->d_name);

            FILE* commFile = fopen(procPath, "r");
            if (commFile) {
                char processName[256];
                if (fgets(processName, sizeof(processName), commFile)) {
                    processName[strcspn(processName, "\n")] = 0;
                    char* process = strdup(processName);
                    if (process) {
                        vector_push_back(processes, &process);
                        SYSINFO_LOG("[sysinfo_process_list]: added '%s'", processName);
                    }
                }
                fclose(commFile);
            }
        }
    }

    closedir(procDir);

#else
    SYSINFO_LOG("[sysinfo_process_list]: backend=unknown (returning empty vector)");
#endif

    SYSINFO_LOG("[sysinfo_process_list]: exit -> %zu processes", vector_size(processes));
    return processes;
}


/**
 * @brief Return a Vector of active network interfaces and their IPv4
 *        addresses.
 *
 * Each element is a `SysinfoNetworkInterface` struct with two
 * `strdup`-allocated strings (`interface_name`, `ip_address`). Use
 * `sysinfo_deallocate_network_interfaces` to release everything
 * together.
 *
 * @code
 *     Vector* ifaces = sysinfo_network_interfaces();
 *     for (size_t i = 0; i < vector_size(ifaces); ++i) {
 *         SysinfoNetworkInterface* it = vector_at(ifaces, i);
 *         printf("%s -> %s\n", it->interface_name, it->ip_address);
 *     }
 *     sysinfo_deallocate_network_interfaces(ifaces);
 * @endcode
 *
 * Backend:
 *   - Windows: `GetAdaptersAddresses`, with auto-resize on
 *     ERROR_BUFFER_OVERFLOW. Reports only adapters with
 *     `OperStatus == IfOperStatusUp`.
 *   - Linux:   `SIOCGIFCONF` + `SIOCGIFADDR` via UDP socket.
 *   - Other:   returns an empty Vector.
 *
 * @return Newly-allocated Vector. NULL only on allocation failure.
 */
Vector* sysinfo_network_interfaces(void) {
    SYSINFO_LOG("[sysinfo_network_interfaces]: enter");

    Vector* interfaces = vector_create(sizeof(SysinfoNetworkInterface));
    if (!interfaces) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: failed to create vector -> NULL");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_network_interfaces]: backend=windows");

    ULONG bufferSize = 15000;
    PIP_ADAPTER_ADDRESSES adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
    if (!adapterAddresses) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: initial malloc failed -> NULL");
        vector_deallocate(interfaces);
        return NULL;
    }

    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: buffer too small, reallocating to %lu bytes",
                    (unsigned long)bufferSize);
        free(adapterAddresses);
        adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);

        if (!adapterAddresses) {
            SYSINFO_LOG("[sysinfo_network_interfaces]: realloc failed -> NULL");

            vector_deallocate(interfaces);
            return NULL;
        }
    }

    DWORD result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize);
    if (result == NO_ERROR) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: GetAdaptersAddresses ok");
        PIP_ADAPTER_ADDRESSES adapter = adapterAddresses;
        while (adapter) {
            if (adapter->OperStatus == IfOperStatusUp) {
                SYSINFO_LOG("[sysinfo_network_interfaces]: active adapter '%s'", adapter->AdapterName);

                SysinfoNetworkInterface iface;
                iface.interface_name = strdup(adapter->AdapterName);
                iface.ip_address = NULL;

                PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress;
                while (unicast) {
                    SOCKADDR* addr = unicast->Address.lpSockaddr;
                    if (addr->sa_family == AF_INET) {
                        struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr;
                        iface.ip_address = strdup(inet_ntoa(ipv4->sin_addr));

                        vector_push_back(interfaces, &iface);
                        SYSINFO_LOG("[sysinfo_network_interfaces]: '%s' -> %s", iface.interface_name, iface.ip_address);
                        break;
                    }
                    unicast = unicast->Next;
                }

                if (!iface.ip_address) {
                    /* no IPv4 address attached: drop the dup */
                    free(iface.interface_name);
                }
            }
            adapter = adapter->Next;
        }
    }
    else {
        SYSINFO_LOG("[sysinfo_network_interfaces]: GetAdaptersAddresses failed, code=%lu",
                    (unsigned long)result);
    }

    free(adapterAddresses);

#elif __linux__
    SYSINFO_LOG("[sysinfo_network_interfaces]: backend=linux");

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: socket() failed -> empty vector");
        return interfaces;
    }

    struct ifconf ifc;
    char buffer[4096];
    ifc.ifc_len = sizeof(buffer);
    ifc.ifc_buf = buffer;

    if (ioctl(fd, SIOCGIFCONF, &ifc) == -1) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: SIOCGIFCONF failed -> empty vector");
        close(fd);
        return interfaces;
    }

    struct ifreq* ifr = ifc.ifc_req;
    int interfaceCount = ifc.ifc_len / sizeof(struct ifreq);

    for (int i = 0; i < interfaceCount; i++) {
        struct ifreq* item = &ifr[i];
        SysinfoNetworkInterface iface;
        iface.interface_name = strdup(item->ifr_name);
        iface.ip_address = NULL;
        SYSINFO_LOG("[sysinfo_network_interfaces]: candidate '%s'",
                    iface.interface_name);

        if (ioctl(fd, SIOCGIFADDR, item) == 0) {
            struct sockaddr_in* addr = (struct sockaddr_in*)&item->ifr_addr;
            iface.ip_address = strdup(inet_ntoa(addr->sin_addr));
            vector_push_back(interfaces, &iface);
            SYSINFO_LOG("[sysinfo_network_interfaces]: '%s' -> %s",
                        iface.interface_name, iface.ip_address);
        }
        else {
            free(iface.interface_name);
        }
    }

    close(fd);

#else
    SYSINFO_LOG("[sysinfo_network_interfaces]: backend=unknown (empty vector)");
#endif

    SYSINFO_LOG("[sysinfo_network_interfaces]: exit -> %zu interfaces",
                vector_size(interfaces));
    return interfaces;
}

/**
 * @brief Release a Vector previously returned by
 *        `sysinfo_network_interfaces`.
 *
 * Frees each element's `interface_name` and `ip_address`, then
 * deallocates the Vector itself. NULL-safe.
 *
 * @param interfaces Vector of `SysinfoNetworkInterface`. May be NULL.
 */
void sysinfo_deallocate_network_interfaces(Vector* interfaces) {
    SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: enter (interfaces=%p)",
                (void*)interfaces);

    if (!interfaces) {
        SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: NULL, no-op");
        return;
    }

    for (size_t i = 0; i < vector_size(interfaces); ++i) {
        SysinfoNetworkInterface* iface = (SysinfoNetworkInterface*)vector_at(interfaces, i);
        free(iface->interface_name);
        free(iface->ip_address);
        SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: freed element %zu", i);
    }

    vector_deallocate(interfaces);
    SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: exit");
}


/**
 * @brief Return a Vector of open TCP listening ports on the system.
 *
 * Each element is an `int` (boxed via the vector's element size of
 * `sizeof(int*)` to remain ABI-compatible with the original API
 * shape). The Vector itself must be deallocated with
 * `vector_deallocate` by the caller.
 *
 * Backend:
 *   - Windows: GetTcpTable / GetTcp6Table.
 *   - Linux:   parses /proc/net/tcp.
 *   - Other:   returns an empty Vector.
 *
 * @code
 *     Vector* ports = sysinfo_open_ports();
 *     for (size_t i = 0; i < vector_size(ports); ++i) {
 *         int* p = (int*)vector_at(ports, i);
 *         printf("open: %d\n", *p);
 *     }
 *     vector_deallocate(ports);
 * @endcode
 *
 * @return Newly-allocated Vector. NULL only on allocation failure.
 */
Vector* sysinfo_open_ports(void) {
    SYSINFO_LOG("[sysinfo_open_ports]: enter");

    Vector* ports = vector_create(sizeof(int*));
    if (!ports) {
        SYSINFO_LOG("[sysinfo_open_ports]: failed to create vector -> NULL");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_open_ports]: backend=windows");
    get_open_ports_windows(ports);
#elif __linux__
    SYSINFO_LOG("[sysinfo_open_ports]: backend=linux");
    get_open_ports_linux(ports);
#else
    SYSINFO_LOG("[sysinfo_open_ports]: backend=unknown (empty vector)");
#endif

    SYSINFO_LOG("[sysinfo_open_ports]: exit -> %zu open ports",
                vector_size(ports));
    return ports;
}

#ifdef _WIN32
/* Windows implementation of sysinfo_is_virtualized. The Doxygen
 * contract lives above the Linux implementation higher up in this
 * file. */
bool sysinfo_is_virtualized(void) {
    SYSINFO_LOG("[sysinfo_is_virtualized/win32]: enter");

    if (IsProcessorFeaturePresent(PF_VIRT_FIRMWARE_ENABLED)) {
        SYSINFO_LOG("[sysinfo_is_virtualized/win32]: detected via PF_VIRT_FIRMWARE_ENABLED -> true");
        return true;
    }

#ifdef PF_HYPERVISOR_PRESENT
    if (IsProcessorFeaturePresent(PF_HYPERVISOR_PRESENT)) {
        SYSINFO_LOG("[sysinfo_is_virtualized/win32]: detected via PF_HYPERVISOR_PRESENT -> true");
        return true;
    }
#endif

    SYSINFO_LOG("[sysinfo_is_virtualized/win32]: exit -> false");
    return false;
}
#elif !defined(__linux__)
/* Fallback definition for platforms that are neither Windows nor
 * Linux. Without this, the symbol was simply missing on macOS/BSD
 * and any caller that referenced it produced a linker error. */
bool sysinfo_is_virtualized(void) {
    SYSINFO_LOG("[sysinfo_is_virtualized/other]: unsupported platform -> false");
    return false;
}
#endif

/**
 * @brief Return the current system locale as a newly-allocated string
 *        (e.g. "en-US", "fa_IR.UTF-8").
 *
 * Backend:
 *   - Windows: `GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, ...)`,
 *     then converted from wide-char to narrow.
 *   - POSIX:   reads `LC_ALL`, then `LC_MESSAGES`, then `LANG`, then
 *     finally `setlocale(LC_ALL, NULL)`.
 *
 * Always returns a heap-allocated copy — never a static pointer —
 * so the caller can safely `free` it.
 *
 * @code
 *     char* loc = sysinfo_system_locale();
 *     printf("locale: %s\n", loc);
 *     free(loc);
 * @endcode
 *
 * @return strdup()'d locale string. Returns the string "unknown" (also
 *         heap-allocated) if the OS could not report a locale, or
 *         NULL only if the duplication itself failed.
 */
char* sysinfo_system_locale(void) {
    SYSINFO_LOG("[sysinfo_system_locale]: enter");

#ifdef _WIN32
    wchar_t w_locale_buffer[128];
    char locale_buffer[128];

    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, w_locale_buffer, sizeof(w_locale_buffer) / sizeof(wchar_t)) == 0) {
        SYSINFO_LOG("[sysinfo_system_locale]: GetLocaleInfoEx failed, falling back to 'unknown'");
        strcpy(locale_buffer, "unknown");
    }
    else {
        wcstombs(locale_buffer, w_locale_buffer, sizeof(locale_buffer));
        SYSINFO_LOG("[sysinfo_system_locale]: win32 locale = '%s'", locale_buffer);
    }

#else
    static char locale_buffer[128];
    const char* locale = getenv("LC_ALL");

    if (!locale || strlen(locale) == 0) {
        SYSINFO_LOG("[sysinfo_system_locale]: LC_ALL empty, trying LC_MESSAGES");
        locale = getenv("LC_MESSAGES");
    }
    if (!locale || strlen(locale) == 0) {
        SYSINFO_LOG("[sysinfo_system_locale]: LC_MESSAGES empty, trying LANG");
        locale = getenv("LANG");
    }
    if (!locale || strlen(locale) == 0) {
        SYSINFO_LOG("[sysinfo_system_locale]: env vars empty, falling back to setlocale()");
        locale = setlocale(LC_ALL, NULL);
    }

    if (locale) {
        strncpy(locale_buffer, locale, sizeof(locale_buffer) - 1);
        locale_buffer[sizeof(locale_buffer) - 1] = '\0';
        SYSINFO_LOG("[sysinfo_system_locale]: posix locale = '%s'", locale_buffer);
    }
    else {
        SYSINFO_LOG("[sysinfo_system_locale]: no locale source, falling back to 'unknown'");
        strcpy(locale_buffer, "unknown");
    }
#endif

    char* result = strdup(locale_buffer);
    if (!result) {
        SYSINFO_LOG("[sysinfo_system_locale]: strdup failed -> NULL");
    }
    else {
        SYSINFO_LOG("[sysinfo_system_locale]: exit -> '%s'", result);
    }
    return result;
}


/**
 * @brief Checks if a specific service or process is running.
 * 
 * This function checks if a service or process by the given name is running on the system.
 * 
 * @param service_name The name of the service or process to check.
 * @return bool Returns true if the service/process is running, false otherwise.
 */
bool sysinfo_is_service_running(const char* service_name) {
    SYSINFO_LOG("[sysinfo_is_service_running]: Checking if service '%s' is running.", service_name);

    #ifdef _WIN32
        SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
        if (!scm) {
            SYSINFO_LOG("Error: Unable to open Service Control Manager.");
            return false;
        }

        SC_HANDLE service = OpenService(scm, service_name, SERVICE_QUERY_STATUS);
        if (!service) {
            SYSINFO_LOG("Error: Unable to open service '%s'.", service_name);
            CloseServiceHandle(scm);
            return false;
        }

        SERVICE_STATUS_PROCESS serviceStatus;
        DWORD bytesNeeded;
        bool isRunning = false;

        if (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&serviceStatus, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
            if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
                SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is running.", service_name);
                isRunning = true;
            } else {
                SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is not running.", service_name);
            }
        } else {
            SYSINFO_LOG("Error: Failed to query service status for '%s'.", service_name);
        }

        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return isRunning;

    #elif __linux__
        if (!service_name || !*service_name) {
            SYSINFO_LOG("[sysinfo_is_service_running]: empty/NULL service name -> not running.");
            return false;
        }
        char command[256];
        snprintf(command, sizeof(command), "systemctl is-active --quiet %s", service_name);
        int result = system(command);

        if (result == 0) {
            SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is running (systemctl check).", service_name);
            return true;
        }

        // Fallback to checking the process list with `ps`
        SYSINFO_LOG("[sysinfo_is_service_running]: Fallback to 'ps' command for service '%s'.", service_name);
        FILE* proc_fp = popen("ps -e", "r");
        if (!proc_fp) {
            SYSINFO_LOG("Error: Unable to execute 'ps' command.");
            return false;
        }

        char buffer[256];
        bool found = false;

        size_t snlen = strlen(service_name);
        while (fgets(buffer, sizeof(buffer), proc_fp)) {
            /* Match service_name as a whitespace/'/'-delimited token (a process
               command), not a loose substring -- otherwise short names like "a"
               match almost every process line, giving a false positive. */
            for (char* hit = strstr(buffer, service_name); hit; hit = strstr(hit + 1, service_name)) {
                char before = (hit == buffer) ? ' ' : hit[-1];
                char after  = hit[snlen];
                if ((before == ' ' || before == '\t' || before == '/' || before == '\n') &&
                    (after == '\0' || after == '\n' || after == ' ' || after == '\t')) {
                    found = true;
                    break;
                }
            }
            if (found) {
                SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' found in process list.", service_name);
                break;
            }
        }
        pclose(proc_fp);

        if (!found) {
            SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is not running (ps check).", service_name);
        }
        return found;
    #else
        /* Platform without a service manager — answer false so callers
           reading is_service_running() on macOS/BSD don't fall off the
           end of the function (undefined behaviour). */
        (void)service_name;
        return false;
    #endif
}

/**
 * @brief Retrieves the disk partitions on the system.
 * 
 * This function gathers information about the disk partitions available on the system and stores 
 * them in a vector of `SysinfoDiskPartition` structures. It handles both Windows and Linux platforms. 
 * On unsupported platforms, the function logs an error and returns `NULL`.
 * 
 * On Windows, it calls the `get_disk_partitions_windows` function to populate the vector with partition 
 * information. On Linux, it calls the `get_disk_partitions_linux` function.
 * 
 * @return A pointer to a `Vector` of `SysinfoDiskPartition` structures representing the disk partitions 
 * on the system. If the function fails or the platform is unsupported, it returns `NULL`.
 * 
 * @note The caller is responsible for deallocating the returned vector using `vector_deallocate`.
 */
Vector* sysinfo_disk_partitions() {
    SYSINFO_LOG("[sysinfo_disk_partitions]: Entering function.");

    Vector* partitions = vector_create(sizeof(SysinfoDiskPartition));
    if (!partitions) {
        SYSINFO_LOG("[sysinfo_disk_partitions]: Error - Failed to create partitions vector.");
        return NULL;
    }

    #ifdef _WIN32
        SYSINFO_LOG("[sysinfo_disk_partitions]: Calling get_disk_partitions_windows.");
        get_disk_partitions_windows(partitions);
    #elif __linux__
        SYSINFO_LOG("[sysinfo_disk_partitions]: Calling get_disk_partitions_linux.");
        get_disk_partitions_linux(partitions);
    #else
        SYSINFO_LOG("[sysinfo_disk_partitions]: Error - Unsupported platform.");
        vector_deallocate(partitions);
        return NULL;
    #endif

    SYSINFO_LOG("[sysinfo_disk_partitions]: Exiting function with %zu partitions.", vector_size(partitions));
    return partitions;
}

/**
 * @brief Deallocates the memory used by a vector of SysinfoDiskPartition structures.
 * 
 * This function iterates over a vector of `SysinfoDiskPartition` structures and frees the
 * dynamically allocated memory for each partition's `mount_point`. After freeing the memory
 * for each individual partition, it deallocates the vector itself.
 * 
 * @param partitions A pointer to a `Vector` containing `SysinfoDiskPartition` structures.
 *                   The vector and its contents are deallocated by this function.
 * 
 * @warning Ensure that you do not use the `partitions` vector or its elements after calling
 *          this function, as the memory will have been deallocated.
 */
void sysinfo_deallocate_disk_partitions(Vector* partitions) {
    SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Entering function to deallocate disk partitions.");

    if (!partitions) {
        SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Warning - NULL partitions vector passed.");
        return;
    }

    for (size_t i = 0; i < vector_size(partitions); ++i) {
        SysinfoDiskPartition* partition = (SysinfoDiskPartition*)vector_at(partitions, i);
        if (partition) {
            SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Freeing mount point of partition %zu.", i);
            free(partition->mount_point);
        } 
        else {
            SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Warning - NULL partition at index %zu.", i);
        }
    }

    SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Deallocating partitions vector.");
    vector_deallocate(partitions);

    SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Exiting function.");
}


#ifdef _WIN32
#  include <direct.h>          /* _getcwd */
#  include <process.h>         /* _getpid (not used; kept for clarity)   */
/* GetUserNameA, GetTempPathA, GetModuleFileNameA, GetCurrentProcessId,
   and GlobalMemoryStatusEx are already available via <windows.h>. */
#else
#  include <unistd.h>          /* getcwd, getpid, getlogin_r, readlink   */
#  include <sys/types.h>
#  include <pwd.h>             /* getpwuid                                */
#  include <limits.h>          /* PATH_MAX                                */
#  ifndef PATH_MAX
#    define PATH_MAX 4096
#  endif
#endif

/* Small helper: dup a string but never return NULL. Falls back to a
   newly-allocated copy of `fallback` (which itself must not be NULL).
   Used by the username / home / temp helpers so the public API can
   make the strict "never returns NULL" guarantee documented in the
   header. */
static char* sysinfo_strdup_or(const char* src, const char* fallback) {
    const char* s = (src && src[0] != '\0') ? src : fallback;
    if (!s) {
        s = "";
    }
    size_t n = strlen(s);
    char* out = (char*)malloc(n + 1);
    if (!out) {
        SYSINFO_LOG("[sysinfo_strdup_or]: malloc failed.");
        return NULL;
    }
    memcpy(out, s, n + 1);
    return out;
}

/**
 * @brief Returns the current user's login name as a freshly-allocated string.
 *
 * Implementation:
 *  - Windows: `GetUserNameA` into a UNLEN+1 buffer.
 *  - POSIX:   `getlogin_r`, then `getpwuid(geteuid())`, then `$USER` /
 *             `$LOGNAME` as a final fallback.
 *
 * Always returns a non-NULL string. Falls back to `"unknown"` if every
 * source fails (very rare — a controlled environment with no passwd
 * database and no environment).
 *
 * @code
 * char* user = sysinfo_username();
 * printf("Hello, %s!\n", user);
 * free(user);
 * @endcode
 */
char* sysinfo_username(void) {
    SYSINFO_LOG("[sysinfo_username]: Entering function.");
#ifdef _WIN32
    /* UNLEN is 256 in <lmcons.h>; allocate one byte more for the NUL. */
    char name[257];
    DWORD len = (DWORD)sizeof(name);
    if (GetUserNameA(name, &len) && len > 0) {
        return sysinfo_strdup_or(name, "unknown");
    }
    return sysinfo_strdup_or(NULL, "unknown");
#else
    char buf[256];
    if (getlogin_r(buf, sizeof(buf)) == 0 && buf[0] != '\0') {
        return sysinfo_strdup_or(buf, "unknown");
    }
    struct passwd* pw = getpwuid(geteuid());
    if (pw && pw->pw_name && pw->pw_name[0] != '\0') {
        return sysinfo_strdup_or(pw->pw_name, "unknown");
    }
    const char* env = getenv("USER");
    if (!env || env[0] == '\0') env = getenv("LOGNAME");
    return sysinfo_strdup_or(env, "unknown");
#endif
}


/**
 * @brief Returns the current user's home directory as a freshly-allocated string.
 *
 * Implementation:
 *  - Windows: prefers `$USERPROFILE`; if absent falls back to combining
 *             `$HOMEDRIVE` and `$HOMEPATH`.
 *  - POSIX:   `$HOME`, then `getpwuid(geteuid())->pw_dir`.
 *
 * Never returns NULL. Returns an empty string when nothing is available.
 *
 * @code
 * char* home = sysinfo_home_directory();
 * printf("Config under: %s/.myapp\n", home);
 * free(home);
 * @endcode
 */
char* sysinfo_home_directory(void) {
    SYSINFO_LOG("[sysinfo_home_directory]: Entering function.");
#ifdef _WIN32
    const char* up = getenv("USERPROFILE");
    if (up && up[0] != '\0') {
        return sysinfo_strdup_or(up, "");
    }

    const char* drv = getenv("HOMEDRIVE");
    const char* pth = getenv("HOMEPATH");
    if (drv && pth) {
        size_t n1 = strlen(drv), n2 = strlen(pth);
        char* out = (char*)malloc(n1 + n2 + 1);

        if (out) {
            memcpy(out, drv, n1);
            memcpy(out + n1, pth, n2);
            out[n1 + n2] = '\0';
            return out;
        }
    }
    return sysinfo_strdup_or("", "");
#else
    const char* env = getenv("HOME");
    if (env && env[0] != '\0') {
        return sysinfo_strdup_or(env, "");
    }

    struct passwd* pw = getpwuid(geteuid());
    if (pw && pw->pw_dir) {
        return sysinfo_strdup_or(pw->pw_dir, "");
    }

    return sysinfo_strdup_or("", "");
#endif
}


/**
 * @brief Returns the system's default temporary-files directory.
 *
 * Implementation:
 *  - Windows: `GetTempPathA`, with any trailing `\\` trimmed.
 *  - POSIX:   `$TMPDIR`, then `/tmp`.
 *
 * Never returns NULL. The returned string never has a trailing
 * separator, so callers can do `<tmp> + "/" + name` without worrying
 * about a double slash.
 *
 * @code
 * char* tmp = sysinfo_temp_directory();
 * char buf[512];
 * snprintf(buf, sizeof(buf), "%s/myapp.tmp", tmp);
 * free(tmp);
 * @endcode
 */
char* sysinfo_temp_directory(void) {
    SYSINFO_LOG("[sysinfo_temp_directory]: Entering function.");
#ifdef _WIN32
    char buf[MAX_PATH + 1];
    DWORD n = GetTempPathA((DWORD)sizeof(buf), buf);
    if (n == 0 || n >= sizeof(buf)) {
        return sysinfo_strdup_or("C:\\Temp", "C:\\Temp");
    }
    /* Trim trailing slash/backslash to normalise the contract. */
    while (n > 0 && (buf[n - 1] == '\\' || buf[n - 1] == '/')) {
        buf[--n] = '\0';
    }
    return sysinfo_strdup_or(buf, "C:\\Temp");
#else
    const char* env = getenv("TMPDIR");
    if (env && env[0] != '\0') {
        /* Trim trailing slash for consistency with the Windows path. */
        size_t n = strlen(env);
        while (n > 1 && env[n - 1] == '/') n--;
        char* out = (char*)malloc(n + 1);
        if (out) {
            memcpy(out, env, n);
            out[n] = '\0';

            return out;
        }
    }
    return sysinfo_strdup_or("/tmp", "/tmp");
#endif
}


/**
 * @brief Returns the current working directory of the calling process.
 *
 * Uses a growing buffer so paths longer than `PATH_MAX` (rare but
 * legal on some POSIX systems) are still captured. Caller owns the
 * returned string. Returns NULL on unrecoverable error.
 *
 * @code
 * char* cwd = sysinfo_current_working_directory();
 * if (cwd) { puts(cwd); free(cwd); }
 * @endcode
 */
char* sysinfo_current_working_directory(void) {
    SYSINFO_LOG("[sysinfo_current_working_directory]: Entering function.");
    size_t cap = 256;
    while (cap <= (1u << 16)) {
        char* buf = (char*)malloc(cap);
        if (!buf) {
            SYSINFO_LOG("[sysinfo_current_working_directory]: malloc failed.");
            return NULL;
        }
#ifdef _WIN32
        if (_getcwd(buf, (int)cap) != NULL) {
            return buf;
        }
#else
        if (getcwd(buf, cap) != NULL) {
            return buf;
        }
#endif
        free(buf);
        cap *= 2;
    }
    return NULL;
}


/**
 * @brief Returns the absolute path to the currently-running executable.
 *
 * Implementation:
 *  - Windows: `GetModuleFileNameA(NULL, ...)` with an MAX_PATH buffer.
 *  - Linux:   `readlink("/proc/self/exe")`.
 *
 * Caller owns the returned string. Returns NULL on the unsupported-
 * platform path (macOS support would require `_NSGetExecutablePath`,
 * intentionally not pulled in here to keep the dependency surface
 * small).
 *
 * @code
 * char* exe = sysinfo_executable_path();
 * if (exe) { printf("running from %s\n", exe); free(exe); }
 * @endcode
 */
char* sysinfo_executable_path(void) {
    SYSINFO_LOG("[sysinfo_executable_path]: Entering function.");
#ifdef _WIN32
    char buf[MAX_PATH + 1];
    DWORD n = GetModuleFileNameA(NULL, buf, (DWORD)sizeof(buf));
    if (n == 0 || n >= sizeof(buf)) {
        SYSINFO_LOG("[sysinfo_executable_path]: GetModuleFileNameA failed or truncated.");
        return NULL;
    }

    return sysinfo_strdup_or(buf, NULL);
#elif defined(__linux__)
    char buf[PATH_MAX + 1];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n <= 0) {
        SYSINFO_LOG("[sysinfo_executable_path]: readlink /proc/self/exe failed.");
        return NULL;
    }
    buf[n] = '\0';
    return sysinfo_strdup_or(buf, NULL);
#else
    return NULL;
#endif
}


/**
 * @brief Returns the process ID of the current process.
 *
 * Implementation:
 *  - Windows: `GetCurrentProcessId()` (DWORD widened to long).
 *  - POSIX:   `getpid()` (pid_t widened to long).
 *
 * Returns `-1` only on the unsupported-platform path. Never fails on
 * Windows or POSIX.
 *
 * @code
 * printf("Our pid is %ld\n", sysinfo_process_id());
 * @endcode
 */
long sysinfo_process_id(void) {
    SYSINFO_LOG("[sysinfo_process_id]: Entering function.");
#ifdef _WIN32
    return (long)GetCurrentProcessId();
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    return (long)getpid();
#else
    return -1;
#endif
}


/**
 * @brief Returns the total amount of physical memory in bytes.
 *
 * Implementation:
 *  - Windows: `GlobalMemoryStatusEx(ullTotalPhys)`.
 *  - Linux:   parses `MemTotal` from `/proc/meminfo`.
 *
 * Returns 0 if the OS cannot answer or the platform is unsupported.
 *
 * @code
 * unsigned long long total = sysinfo_total_memory_bytes();
 * printf("Total RAM: %.2f GiB\n", total / (1024.0 * 1024.0 * 1024.0));
 * @endcode
 */
unsigned long long sysinfo_total_memory_bytes(void) {
    SYSINFO_LOG("[sysinfo_total_memory_bytes]: Entering function.");
#ifdef _WIN32
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        return (unsigned long long)mem.ullTotalPhys;
    }
    return 0ULL;
#elif defined(__linux__)
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) return 0ULL;
    char line[256];
    unsigned long long kb = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %llu kB", &kb) == 1) {
            break;
        }
    }
    fclose(fp);
    return kb * 1024ULL;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns the amount of physical memory currently available, in bytes.
 *
 * Implementation:
 *  - Windows: `GlobalMemoryStatusEx(ullAvailPhys)`.
 *  - Linux:   prefers `MemAvailable` from `/proc/meminfo`; falls back
 *             to `MemFree` for old kernels (<3.14) that don't expose it.
 *
 * Returns 0 if the OS cannot answer or the platform is unsupported.
 *
 * @code
 * unsigned long long avail = sysinfo_available_memory_bytes();
 * if (avail < (256ULL << 20)) {
 *     fprintf(stderr, "Less than 256 MiB free; refusing to start.\n");
 * }
 * @endcode
 */
unsigned long long sysinfo_available_memory_bytes(void) {
    SYSINFO_LOG("[sysinfo_available_memory_bytes]: Entering function.");
#ifdef _WIN32
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        return (unsigned long long)mem.ullAvailPhys;
    }
    return 0ULL;
#elif defined(__linux__)
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) return 0ULL;
    char line[256];
    unsigned long long available = 0, mem_free = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemAvailable: %llu kB", &available) == 1) {
            break;
        }
        if (sscanf(line, "MemFree: %llu kB", &mem_free) == 1) {
            /* Keep scanning — MemAvailable, if present, is preferred. */
        }
    }
    fclose(fp);
    if (available > 0) return available * 1024ULL;
    return mem_free * 1024ULL;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns true if the current process is a 64-bit binary.
 *
 * Determined from `sizeof(void*) == 8` — no syscalls, no platform
 * dispatch. This reflects the bit-width of *this* process, not of the
 * host OS (a 32-bit binary running on 64-bit Windows will return
 * false here, which is what most callers actually want).
 *
 * @code
 * if (!sysinfo_is_64bit_process()) {
 *     fputs("This release requires a 64-bit build.\n", stderr);
 * }
 * @endcode
 */
bool sysinfo_is_64bit_process(void) {
    return sizeof(void*) == 8;
}


/**
 * @brief Returns the byte order of the host CPU as a static string.
 *
 * Either `"little"` or `"big"`. The check is a runtime probe over the
 * representation of a `uint16_t`, so it does not depend on
 * `__BYTE_ORDER__` being defined by the compiler. Mixed-endian and
 * exotic platforms fall back to `"unknown"`.
 *
 * The returned pointer is to a static literal — do not free it.
 *
 * @code
 * const char* endian = sysinfo_endianness();
 * printf("CPU is %s-endian\n", endian);
 * @endcode
 */
const char* sysinfo_endianness(void) {
    union { uint16_t u16; unsigned char bytes[2]; } probe;
    probe.u16 = 0x0102;
    if (probe.bytes[0] == 0x01 && probe.bytes[1] == 0x02) {
        return "big";
    }
    if (probe.bytes[0] == 0x02 && probe.bytes[1] == 0x01) {
        return "little";
    }
    return "unknown";
}


/**
 * @brief Returns the amount of physical memory currently in use, in bytes.
 *
 * Convenience wrapper computed as
 * `sysinfo_total_memory_bytes() - sysinfo_available_memory_bytes()`.
 * Returns 0 if either source returns 0, or if the values are inconsistent
 * (available greater than total), so it is automatically cross-platform.
 *
 */
unsigned long long sysinfo_used_memory_bytes(void) {
    SYSINFO_LOG("[sysinfo_used_memory_bytes]: Entering function.");
    unsigned long long total = sysinfo_total_memory_bytes();
    unsigned long long avail = sysinfo_available_memory_bytes();
    if (total == 0ULL || avail > total) {
        return 0ULL;
    }
    return total - avail;
}


/**
 * @brief Returns the total size of swap space, in bytes.
 *
 * Implementation:
 *  - Linux:   parses `SwapTotal` from `/proc/meminfo`.
 *  - Windows: there is no separate "swap"; the equivalent is the page file.
 *             Reported as `ullTotalPageFile - ullTotalPhys` (the commit limit
 *             minus physical RAM), i.e. the page-file portion of the commit
 *             limit. Returns 0 when no page file is configured.
 *
 * Returns 0 if the OS cannot answer or the platform is unsupported. A value
 * of 0 is legitimate (e.g. swapless containers / systems).
 *
 */
unsigned long long sysinfo_total_swap_bytes(void) {
    SYSINFO_LOG("[sysinfo_total_swap_bytes]: Entering function.");
#ifdef _WIN32
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        if (mem.ullTotalPageFile > mem.ullTotalPhys) {
            return (unsigned long long)(mem.ullTotalPageFile - mem.ullTotalPhys);
        }
        return 0ULL;
    }
    return 0ULL;
#elif defined(__linux__)
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return 0ULL;
    }
    char line[256];
    unsigned long long kb = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "SwapTotal: %llu kB", &kb) == 1) {
            break;
        }
    }
    fclose(fp);
    return kb * 1024ULL;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns the amount of free swap space, in bytes.
 *
 * Implementation:
 *  - Linux:   parses `SwapFree` from `/proc/meminfo`.
 *  - Windows: `ullAvailPageFile` is the remaining commit the system will grant
 *             (commit limit minus current commit charge), capped at the
 *             page-file size. When little is committed the page file is
 *             effectively all free; as commit nears the limit the value shrinks
 *             toward 0. (Windows exposes no exact "free page file" counter via
 *             `GlobalMemoryStatusEx`, so this is the closest faithful mapping.)
 *
 * The result is always `<= sysinfo_total_swap_bytes()`. Returns 0 on error /
 * unsupported platforms.
 *
 */
unsigned long long sysinfo_free_swap_bytes(void) {
    SYSINFO_LOG("[sysinfo_free_swap_bytes]: Entering function.");
#ifdef _WIN32
    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
        unsigned long long totalSwap = (mem.ullTotalPageFile > mem.ullTotalPhys)
            ? (unsigned long long)(mem.ullTotalPageFile - mem.ullTotalPhys) : 0ULL;
        unsigned long long availCommit = (unsigned long long)mem.ullAvailPageFile;

        return (availCommit < totalSwap) ? availCommit : totalSwap;
    }
    return 0ULL;
#elif defined(__linux__)
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return 0ULL;
    }

    char line[256];
    unsigned long long kb = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "SwapFree: %llu kB", &kb) == 1) {
            break;
        }
    }
    fclose(fp);

    return kb * 1024ULL;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns the system uptime in whole seconds.
 *
 * The numeric counterpart of `sysinfo_system_uptime()` (which formats a
 * human-readable string). Useful for arithmetic, thresholds and logging.
 *
 * Implementation:
 *  - Windows: `GetTickCount64() / 1000`.
 *  - Linux:   the first field of `/proc/uptime`.
 *
 * Returns -1 if the value cannot be determined.
 *
 */
long sysinfo_uptime_seconds(void) {
    SYSINFO_LOG("[sysinfo_uptime_seconds]: Entering function.");
#ifdef _WIN32
    return (long)(GetTickCount64() / 1000ULL);
#elif defined(__linux__)
    FILE* fp = fopen("/proc/uptime", "r");
    if (!fp) {
        return -1;
    }

    double up = 0.0;
    int n = fscanf(fp, "%lf", &up);
    fclose(fp);
    if (n != 1 || up < 0.0) {
        return -1;
    }

    return (long)up;
#else
    return -1;
#endif
}


/**
 * @brief Returns the number of processes currently running on the system.
 *
 * Implementation:
 *  - Windows: counts entries from a `CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS)`.
 *  - Linux:   counts the numeric (PID) directories under `/proc`.
 *
 * Returns -1 if the count cannot be obtained.
 *
 */
long sysinfo_process_count(void) {
    SYSINFO_LOG("[sysinfo_process_count]: Entering function.");
#ifdef _WIN32
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return -1;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    long count = 0;
    if (Process32First(snap, &pe)) {
        do {
            count++;
        } while (Process32Next(snap, &pe));
    }

    CloseHandle(snap);
    return count;
#elif defined(__linux__)
    DIR* d = opendir("/proc");
    if (!d) {
        return -1;
    }

    long count = 0;
    struct dirent* e;
    while ((e = readdir(d)) != NULL) {
        const char* p = e->d_name;
        bool all_digits = (p[0] != '\0');
        for (const char* q = p; *q; ++q) {
            if (*q < '0' || *q > '9') {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            count++;
        }
    }

    closedir(d);
    return count;
#else
    return -1;
#endif
}


/**
 * @brief Returns true if the current process has administrative / root rights.
 *
 * Implementation:
 *  - Windows: queries the process token's `TokenElevation` — true only when
 *             the process is actually running elevated.
 *  - POSIX:   `geteuid() == 0`.
 *
 * Returns false on unsupported platforms or if privileges cannot be queried.
 *
 */
bool sysinfo_is_admin(void) {
    SYSINFO_LOG("[sysinfo_is_admin]: Entering function.");
#ifdef _WIN32
    BOOL elevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD sz = sizeof(elevation);
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &sz)) {
            elevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return elevated ? true : false;
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    return geteuid() == 0;
#else
    return false;
#endif
}


/**
 * @brief Returns the total size, in bytes, of the filesystem containing @p path.
 *
 * The numeric counterpart of `sysinfo_disk_space()`. Pass any path on the
 * volume of interest (e.g. `"/"` on Linux or `"C:\\"` on Windows).
 *
 * Implementation:
 *  - Windows: `GetDiskFreeSpaceExA` (total number of bytes).
 *  - Linux:   `statvfs` (`f_blocks * f_frsize`).
 *
 * Returns 0 on NULL/empty path, on error, or on unsupported platforms.
 *
 */
unsigned long long sysinfo_disk_total_bytes(const char* path) {
    SYSINFO_LOG("[sysinfo_disk_total_bytes]: Entering function.");
    if (!path || !*path) {
        return 0ULL;
    }
#ifdef _WIN32
    ULARGE_INTEGER freeAvail, total, totalFree;
    if (GetDiskFreeSpaceExA(path, &freeAvail, &total, &totalFree)) {
        return (unsigned long long)total.QuadPart;
    }

    return 0ULL;
#elif defined(__linux__)
    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) {
        return 0ULL;
    }

    return (unsigned long long)vfs.f_blocks * (unsigned long long)vfs.f_frsize;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns the free space, in bytes, available to the caller on the
 *        filesystem containing @p path.
 *
 * Reports space available to an unprivileged caller (it honours filesystem
 * reservations), i.e. `f_bavail` on Linux and `lpFreeBytesAvailableToCaller`
 * on Windows.
 *
 * Implementation:
 *  - Windows: `GetDiskFreeSpaceExA` (free bytes available to caller).
 *  - Linux:   `statvfs` (`f_bavail * f_frsize`).
 *
 * Returns 0 on NULL/empty path, on error, or on unsupported platforms.
 *
 */
unsigned long long sysinfo_disk_free_bytes(const char* path) {
    SYSINFO_LOG("[sysinfo_disk_free_bytes]: Entering function.");
    if (!path || !*path) {
        return 0ULL;
    }
#ifdef _WIN32
    ULARGE_INTEGER freeAvail, total, totalFree;
    if (GetDiskFreeSpaceExA(path, &freeAvail, &total, &totalFree)) {
        return (unsigned long long)freeAvail.QuadPart;
    }

    return 0ULL;
#elif defined(__linux__)
    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) {
        return 0ULL;
    }

    return (unsigned long long)vfs.f_bavail * (unsigned long long)vfs.f_frsize;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns the battery charge level as a whole percentage (0..100).
 *
 * Implementation:
 *  - Windows: `GetSystemPowerStatus` (`BatteryLifePercent`; 255 means unknown).
 *  - Linux:   reads `/sys/class/power_supply/BAT0|BAT1/capacity`.
 *
 * Returns -1 when there is no battery (desktops, servers, most VMs/CI) or the
 * level is unknown — callers should treat -1 as "not applicable".
 *
 */
int sysinfo_battery_percentage(void) {
    SYSINFO_LOG("[sysinfo_battery_percentage]: Entering function.");
#ifdef _WIN32
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        if (sps.BatteryLifePercent <= 100) {
            return (int)sps.BatteryLifePercent;
        }
    }
    return -1;
#elif defined(__linux__)
    static const char* const paths[] = {
        "/sys/class/power_supply/BAT0/capacity",
        "/sys/class/power_supply/BAT1/capacity"
    };
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        FILE* fp = fopen(paths[i], "r");
        if (fp) {
            int pct = -1;
            int n = fscanf(fp, "%d", &pct);
            fclose(fp);
            
            if (n == 1 && pct >= 0 && pct <= 100) {
                return pct;
            }
        }
    }
    return -1;
#else
    return -1;
#endif
}


/**
 * @brief Returns the path of the user's default shell / command interpreter.
 *
 * Implementation:
 *  - Windows: the `COMSPEC` environment variable (falls back to
 *             `C:\\Windows\\System32\\cmd.exe`).
 *  - POSIX:   the `SHELL` environment variable (falls back to `/bin/sh`).
 *
 * The returned string is heap-allocated — the caller must `free()` it. Always
 * returns a non-NULL string except on allocation failure.

 */
char* sysinfo_default_shell(void) {
    SYSINFO_LOG("[sysinfo_default_shell]: Entering function.");
#ifdef _WIN32
    return sysinfo_strdup_or(getenv("COMSPEC"), "C:\\Windows\\System32\\cmd.exe");
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    return sysinfo_strdup_or(getenv("SHELL"), "/bin/sh");
#else
    return sysinfo_strdup_or(NULL, "unknown");
#endif
}


/**
 * @brief Returns the resident memory (RSS / working set) of the CALLING
 *        process, in bytes.
 *
 * This is the per-process complement to the system-wide memory observers
 * (`sysinfo_total_memory_bytes` / `sysinfo_used_memory_bytes`): it reports how
 * much physical RAM *this* process is currently occupying — the single most
 * useful self-monitoring metric for a long-running service (footprint
 * reporting, leak detection, deciding when to shed load or restart).
 *
 * Implementation:
 *  - Windows: `GetProcessMemoryInfo(...).WorkingSetSize`.
 *  - Linux:   the resident-pages field (2nd) of `/proc/self/statm`, multiplied
 *             by the page size from `sysconf(_SC_PAGESIZE)`.
 *
 * Returns 0 if the OS cannot answer or the platform is unsupported.
 *
 * @code
 * unsigned long long rss = sysinfo_process_memory_bytes();
 * printf("This process is using %.1f MiB\n", rss / (1024.0 * 1024.0));
 * @endcode
 */
unsigned long long sysinfo_process_memory_bytes(void) {
    SYSINFO_LOG("[sysinfo_process_memory_bytes]: Entering function.");
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    ZeroMemory(&pmc, sizeof(pmc));
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return (unsigned long long)pmc.WorkingSetSize;
    }
    return 0ULL;
#elif defined(__linux__)
    FILE* fp = fopen("/proc/self/statm", "r");
    if (!fp) {
        return 0ULL;
    }
    unsigned long long total_pages = 0, resident_pages = 0;
    /* statm: size resident shared text lib data dt (all in pages). */
    if (fscanf(fp, "%llu %llu", &total_pages, &resident_pages) != 2) {
        fclose(fp);
        return 0ULL;
    }
    fclose(fp);
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        return 0ULL;
    }
    return resident_pages * (unsigned long long)page_size;
#else
    return 0ULL;
#endif
}


/**
 * @brief Returns the human-readable CPU brand/model string (static storage).
 *
 * Complements `sysinfo_cpu_architecture` ("x86_64") and `sysinfo_cpu_cores`
 * with the marketing name of the processor, e.g.
 * "Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz" or "AMD Ryzen 7 5800X" — the
 * value telemetry, inventory, and support diagnostics universally record.
 *
 * Implementation:
 *  - Windows: the `ProcessorNameString` value under
 *             `HKLM\HARDWARE\DESCRIPTION\System\CentralProcessor\0`.
 *  - Linux:   the first `model name` line of `/proc/cpuinfo` (falls back to
 *             `Hardware` on boards — e.g. ARM — that omit `model name`).
 *
 * Leading/trailing whitespace is trimmed. Returns the literal `"unknown"`
 * when the CPU name cannot be determined. The returned pointer is to static
 * storage — do NOT free it, and copy it if you need to keep it across calls.
 *
 * @code
 * printf("CPU: %s\n", sysinfo_cpu_model());
 * @endcode
 */
char* sysinfo_cpu_model(void) {
    SYSINFO_LOG("[sysinfo_cpu_model]: Entering function.");
    static char model[256];
    strcpy(model, "unknown");

#ifdef _WIN32
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buf[256];
        DWORD len = sizeof(buf);
        DWORD type = 0;
        if (RegQueryValueExA(hKey, "ProcessorNameString", NULL, &type,
                             (LPBYTE)buf, &len) == ERROR_SUCCESS &&
            type == REG_SZ && len > 0) {
            buf[(len < sizeof(buf)) ? len : sizeof(buf) - 1] = '\0';
            buf[sizeof(buf) - 1] = '\0';
            snprintf(model, sizeof(model), "%s", buf);
        }
        RegCloseKey(hKey);
    }
#elif defined(__linux__)
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[512];
        char fallback[256] = "";
        bool found = false;
        while (fgets(line, sizeof(line), fp)) {
            const char* key = NULL;
            if (strncmp(line, "model name", 10) == 0) {
                key = line + 10;
            }
            else if (fallback[0] == '\0' && strncmp(line, "Hardware", 8) == 0) {
                key = line + 8;
            }
            if (!key) {
                continue;
            }
            const char* colon = strchr(key, ':');
            if (!colon) {
                continue;
            }
            const char* val = colon + 1;
            while (*val == ' ' || *val == '\t') {
                val++;
            }
            char tmp[256];
            snprintf(tmp, sizeof(tmp), "%s", val);
            size_t tl = strlen(tmp);
            while (tl > 0 && (tmp[tl - 1] == '\n' || tmp[tl - 1] == '\r' ||
                              tmp[tl - 1] == ' '  || tmp[tl - 1] == '\t')) {
                tmp[--tl] = '\0';
            }
            if (line[0] == 'm') {              /* "model name" -> definitive */
                snprintf(model, sizeof(model), "%s", tmp);
                found = true;
                break;
            }
            else if (fallback[0] == '\0') {    /* "Hardware" -> remember as fallback */
                snprintf(fallback, sizeof(fallback), "%s", tmp);
            }
        }
        fclose(fp);
        if (!found && fallback[0] != '\0') {
            snprintf(model, sizeof(model), "%s", fallback);
        }
    }
#endif

    SYSINFO_LOG("[sysinfo_cpu_model]: Exiting with model: %s", model);
    return model;
}
