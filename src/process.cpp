#include "process.h"
#include <windows.h>
#include <psapi.h>
#include <processthreadsapi.h>
#include <string>
#include <process_info.h>

bool getProcessIDList(unsigned long *processIDArray, unsigned long arraySizeInBytes, unsigned long *processCount)
{
    BOOL processList = EnumProcesses(processIDArray, arraySizeInBytes, processCount);
    *processCount /= sizeof(DWORD);
    if (processList)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void filterProcessArray(unsigned long *processIDArray, unsigned long &processCount)
{
    unsigned long writeIndex = 0;

    for (unsigned long i = 0; i < processCount; i++)
    {
        HANDLE processHandle = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            processIDArray[i]);

        if (!processHandle)
        {
            continue;
        }

        char fullPath[1024];
        BOOL ok = GetModuleFileNameExA(
            processHandle,
            NULL,
            fullPath,
            sizeof(fullPath));

        CloseHandle(processHandle);

        if (!ok)
        {
            continue;
        }

        processIDArray[writeIndex] = processIDArray[i];
        writeIndex++;
    }

    processCount = writeIndex;
}

void fetchProcessName(
    unsigned long processCount)
{
    HANDLE processHandle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processList[processCount].pid);

    if (!processHandle)
        return;

    char fullPath[1024];

    DWORD size = 1024;

    if (GetModuleFileNameExA(processHandle, NULL, fullPath, sizeof(fullPath)) ||
        QueryFullProcessImageNameA(processHandle, 0, fullPath, &size))
    {
        char *baseName = strrchr(fullPath, '\\');
        if (baseName)
            baseName++;
        else
            baseName = fullPath;

        strcpy(processList[processCount].name, baseName);
    }

    CloseHandle(processHandle);
    return;
}

void fetchPriority(
    unsigned long index)
{
    HANDLE processHandle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processList[index].pid);

    if (!processHandle)
    {
        processList[index].priority = ProcessInfo::UNKNOWN;
        return;
    }

    DWORD priority = GetPriorityClass(processHandle);

    if (priority==0)
    {
        processList[index].priority = ProcessInfo::UNKNOWN;
    }

    if (priority==0x40)
        processList[index].priority = ProcessInfo::IDLE;
    else if(priority==0x4000)
        processList[index].priority = ProcessInfo::BELOW_NORMAL;
    else if(priority==0x20)
        processList[index].priority = ProcessInfo::NORMAL;
    else if(priority==0x8000)
        processList[index].priority = ProcessInfo::ABOVE_NORMAL;
    else if(priority==0x80)
        processList[index].priority = ProcessInfo::HIGH;
    else if(priority==0x100)
        processList[index].priority = ProcessInfo::REALTIME;

    CloseHandle(processHandle);
}
