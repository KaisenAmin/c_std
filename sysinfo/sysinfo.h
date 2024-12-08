/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Sysinfo
*/

#ifndef SYSINFO_H_
#define SYSINFO_H_

#include <stdio.h>
#include "../vector/vector.h"

#ifdef __cplusplus 
extern "C" {
#endif 

// #define SYSINFO_LOGGING_ENABLE

#ifdef SYSINFO_LOGGING_ENABLE
    #define SYSINFO_LOG(fmt, ...) \
        do { fprintf(stderr, "[SYSINFO LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SYSINFO_LOG(fmt, ...) do { } while (0)
#endif


typedef struct {
    char* interface_name;
    char* ip_address;
    
} SysinfoNetworkInterface;

typedef struct {
    char* mount_point;  
    unsigned long long total_size;  
    unsigned long long free_space;

} SysinfoDiskPartition;

Vector* sysinfo_running_services();
Vector* sysinfo_process_list();
Vector* sysinfo_network_interfaces();
Vector* sysinfo_open_ports();
Vector* sysinfo_disk_partitions();

void sysinfo_deallocate_network_interfaces(Vector* interfaces);
void sysinfo_deallocate_disk_partitions(Vector* partitions);

int sysinfo_cpu_cores();

bool sysinfo_is_virtualized();
bool sysinfo_is_service_running(const char* service_name);

double sysinfo_cpu_usage();
double sysinfo_memory_usage();

char* sysinfo_product_version();
char* sysinfo_product_type();
char* sysinfo_kernel_version();
char* sysinfo_kernel_type();
char* sysinfo_boot_unique_id();
char* sysinfo_cpu_architecture();
char* sysinfo_machine_host_name();
char* sysinfo_machine_unique_id();
char* sysinfo_pretty_product_name();
char* sysinfo_build_abi();
char* sysinfo_system_uptime();
char* sysinfo_system_locale();
char* sysinfo_disk_space(const char* path);

char** sysinfo_list_bluetooth_devices(int* count);

#ifdef __cplusplus 
}
#endif 

#endif 