#include <windows.h>
#include <psapi.h>
#include <memory_usage.h>
#include <process_info.h>

void fetchMemoryUsage(unsigned long processCount)
{
    HANDLE processHandle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processList[processCount].pid);

    if (!processHandle)
    {
        processList[processCount].memoryMB = 0;
        return;
    }

    DWORD cb = sizeof(PROCESS_MEMORY_COUNTERS);
    PROCESS_MEMORY_COUNTERS tempMem;

    BOOL sucesss = GetProcessMemoryInfo(processHandle, &tempMem, cb);

    if (sucesss)
    {
        processList[processCount].memoryMB = tempMem.WorkingSetSize / (1024 * 1024);
        CloseHandle(processHandle);
        return;
    }

    processList[processCount].memoryMB = 0;
    CloseHandle(processHandle);
    return;
}