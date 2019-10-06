#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "memcheck.h"

struct sysinfo memInfo;

long MemCheck::getTotalVirtualMem() {
    sysinfo(&memInfo);
    long totalVirtualMem = memInfo.totalram;
    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;
    return totalVirtualMem;
}

long MemCheck::getProcessVirtualMem() {
    FILE* file = fopen("/proc/self/status", "r");
    long result = -1;
    char line[128];
    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            int i = strlen(line);
            const char* p = line;
            while (*p <'0' || *p > '9') p++;
            line[i-3] = '\0';
            result = atol(p) * 1024;
            break;
        }
    }
    fclose(file);
    return result;
}

long MemCheck::getTotalPhysicalMem() {
    sysinfo(&memInfo);
    long totalPhysicalMem = memInfo.totalram;
    totalPhysicalMem *= memInfo.mem_unit;
    return totalPhysicalMem;
}

long MemCheck::getProcessPhysicalMem() {
    FILE* file = fopen("/proc/self/status", "r");
    long result = -1;
    char line[128];
    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0){
            int i = strlen(line);
            const char* p = line;
            while (*p <'0' || *p > '9') p++;
            line[i-3] = '\0';
            result = atol(p) * 1024;
            break;
        }
    }
    fclose(file);
    return result;
}
