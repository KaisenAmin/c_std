/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Sysinfo
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in sysinfo.c (file-level convention).
 */

#ifndef SYSINFO_H_
#define SYSINFO_H_

#include <stdio.h>
#include <stdbool.h>
#include "../vector/vector.h"

#ifdef __cplusplus
extern "C" {
#endif


/* #define SYSINFO_LOGGING_ENABLE */

#ifdef SYSINFO_LOGGING_ENABLE
    #define SYSINFO_LOG(fmt, ...) \
        do { fprintf(stderr, "[SYSINFO LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SYSINFO_LOG(...) do { } while (0)
#endif



typedef struct {
    char* interface_name;
    char* ip_address;
} SysinfoNetworkInterface;


typedef struct {
    char*              mount_point;
    unsigned long long total_size;
    unsigned long long free_space;
} SysinfoDiskPartition;


/* ------------------------------------------------------------------ */
/* Vector enumerators (caller frees the Vector)                       */
/* ------------------------------------------------------------------ */

Vector*            sysinfo_running_services        (void);
Vector*            sysinfo_process_list            (void);
Vector*            sysinfo_network_interfaces      (void);
Vector*            sysinfo_open_ports              (void);
Vector*            sysinfo_disk_partitions         (void);


/* ------------------------------------------------------------------ */
/* Deallocators for the Vectors returned above                        */
/* ------------------------------------------------------------------ */

void               sysinfo_deallocate_network_interfaces (Vector* interfaces);
void               sysinfo_deallocate_disk_partitions    (Vector* partitions);


/* ------------------------------------------------------------------ */
/* Numeric observers                                                  */
/* ------------------------------------------------------------------ */

int                sysinfo_cpu_cores               (void);
double             sysinfo_cpu_usage               (void);
double             sysinfo_memory_usage            (void);
unsigned long long sysinfo_total_memory_bytes      (void);
unsigned long long sysinfo_available_memory_bytes  (void);
long               sysinfo_process_id              (void);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool               sysinfo_is_virtualized          (void);
bool               sysinfo_is_service_running      (const char* service_name);
bool               sysinfo_is_64bit_process        (void);


/* ------------------------------------------------------------------ */
/* Identity strings — static storage                                  */
/* ------------------------------------------------------------------ */

char*              sysinfo_product_version         (void);
char*              sysinfo_product_type            (void);
char*              sysinfo_kernel_version          (void);
char*              sysinfo_kernel_type             (void);
char*              sysinfo_boot_unique_id          (void);
char*              sysinfo_cpu_architecture        (void);
char*              sysinfo_cpu_model               (void);
char*              sysinfo_machine_host_name       (void);
char*              sysinfo_machine_unique_id       (void);
char*              sysinfo_pretty_product_name     (void);
char*              sysinfo_build_abi               (void);
const char*        sysinfo_endianness              (void);


/* ------------------------------------------------------------------ */
/* Heap-allocated outputs — caller must free()                        */
/* ------------------------------------------------------------------ */

char*              sysinfo_system_uptime           (void);
char*              sysinfo_system_locale           (void);
char*              sysinfo_disk_space              (const char* path);


/* ------------------------------------------------------------------ */
/* User / environment paths — caller must free()                      */
/* ------------------------------------------------------------------ */

char*              sysinfo_username                (void);
char*              sysinfo_home_directory          (void);
char*              sysinfo_temp_directory          (void);
char*              sysinfo_current_working_directory (void);
char*              sysinfo_executable_path         (void);


/* ------------------------------------------------------------------ */
/* Bluetooth                                                          */
/* ------------------------------------------------------------------ */

char**             sysinfo_list_bluetooth_devices  (int* count);


/* ------------------------------------------------------------------ */
/* Additional observers                                               */
/* ------------------------------------------------------------------ */

unsigned long long sysinfo_used_memory_bytes       (void);
unsigned long long sysinfo_process_memory_bytes    (void);
unsigned long long sysinfo_total_swap_bytes        (void);
unsigned long long sysinfo_free_swap_bytes         (void);
long               sysinfo_uptime_seconds          (void);
long               sysinfo_process_count           (void);
bool               sysinfo_is_admin                (void);
unsigned long long sysinfo_disk_total_bytes        (const char* path);
unsigned long long sysinfo_disk_free_bytes         (const char* path);
int                sysinfo_battery_percentage      (void);
char*              sysinfo_default_shell           (void);   

#ifdef __cplusplus
}
#endif

#endif 
