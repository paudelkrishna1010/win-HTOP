#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct ProcessInfo
{
    DWORD pid;
    char name[1024];

    size_t memoryMB;

    FILETIME prevCpuTime;
    FILETIME currCpuTime;
    double cpuUsage;
};

extern std::vector<ProcessInfo> processList;
extern std::vector<ProcessInfo> oldList;