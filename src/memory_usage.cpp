#include <windows.h>
// #include <psapi.h>
#include <memory_usage.h>

bool getProcessMemoryUsage(unsigned long *processIDArray, PROCESS_MEMORY_COUNTERS  *memoryUsageArray, unsigned long processCount)
{
    for (unsigned long i = 0; i < processCount; i++)
    {
        HANDLE processHandle = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            processIDArray[i]);

        if (!processHandle)
        {
            memoryUsageArray[i].WorkingSetSize = 0;
            continue;
        }

        DWORD cb = sizeof(PROCESS_MEMORY_COUNTERS);

        BOOL sucesss = GetProcessMemoryInfo(processHandle, &memoryUsageArray[i], cb);

        if (!sucesss)
            memoryUsageArray[i].WorkingSetSize = 0;

        CloseHandle(processHandle);
    }
    return true;
}