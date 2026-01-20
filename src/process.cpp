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
