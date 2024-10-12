#ifndef SYSINFO_H_
#define SYSINFO_H_

#include "../vector/vector.h"

typedef struct {
    char* interface_name;
    char* ip_address;
    
} SysinfoNetworkInterface;


Vector* sysinfo_running_services();
Vector* sysinfo_process_list();
Vector* sysinfo_network_interfaces();
Vector* sysinfo_open_ports();

void sysinfo_deallocate_network_interfaces(Vector* interfaces);
int sysinfo_cpu_cores();
bool sysinfo_is_virtualized();

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
char* sysinfo_disk_space(const char* path);

char** sysinfo_list_bluetooth_devices(int* count);

#endif 