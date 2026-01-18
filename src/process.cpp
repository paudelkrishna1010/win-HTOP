#include "process.h"
#include <windows.h>
#include <psapi.h>
#include <processthreadsapi.h>
#include <string>

bool getProcessIDList(unsigned long *processIDArray, unsigned long arraySizeInBytes, unsigned long &processCount)
{
    // DWORD processIDArray[8196];
    // DWORD processCount;
    BOOL processList = EnumProcesses(processIDArray, arraySizeInBytes, &processCount);
    processCount /= sizeof(DWORD);
    if (processList)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool getProcessNameList(
    unsigned long *processIDArray,
    unsigned long processCount,
    char **processNameArray)
{
    for (unsigned long i = 0; i < processCount; i++)
    {
        HANDLE processHandle = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            processIDArray[i]);

        
        processNameArray[i] = new char[256];

        if (!processHandle)
        {
            strcpy(processNameArray[i], "[restricted]");
            continue;
        }

        char fullPath[1024];

        if (!GetModuleFileNameExA(
                processHandle,
                NULL,
                fullPath,
                sizeof(fullPath)))
        {
            strcpy(processNameArray[i], "[restricted]");
            CloseHandle(processHandle);
            continue;
        }


        char *baseName = strrchr(fullPath, '\\');
        if (baseName)
            baseName++;
        else
            baseName = fullPath;

        strcpy(processNameArray[i], baseName);

        CloseHandle(processHandle);
    }

    return true;
}
