#include "process.h"
#include <windows.h>
#include <psapi.h>
#include <processthreadsapi.h>
#include <string>

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

void extractBaseName(char **processNameArray, int i, char *fullPath)
{
    char *baseName = strrchr(fullPath, '\\');
    if (baseName)
        baseName++;
    else
        baseName = fullPath;
    strcpy(processNameArray[i], baseName);
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
            CloseHandle(processHandle);
            continue;
        }

        char fullPath[1024];

        if (!GetModuleFileNameExA(
                processHandle,
                NULL,
                fullPath,
                sizeof(fullPath)))
        {
            DWORD size = 1024;
            if (QueryFullProcessImageNameA(processHandle, 0, fullPath, &size))
            {
                extractBaseName(processNameArray, i, fullPath);
            }
            else
            {
                processNameArray[i] = strdup("[restricted]");
            }
        }

        extractBaseName(processNameArray, i, fullPath);

        CloseHandle(processHandle);
    }

    return true;
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
