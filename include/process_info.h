#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct ProcessInfo
{
    DWORD pid;
    char name[1024];

    size_t memoryMB;
    
    char userName[1024];
    char userGroup[1024];
    enum Priority { IDLE,BELOW_NORMAL, NORMAL, ABOVE_NORMAL, HIGH, REALTIME, UNKNOWN };
    Priority priority;
    
    FILETIME prevCpuTime;
    FILETIME currCpuTime;
    double cpuUsage;
};

extern std::vector<ProcessInfo> processList;
extern std::vector<ProcessInfo> oldList;