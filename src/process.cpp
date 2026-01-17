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

bool getProcessNameList(unsigned long *processIDArray, unsigned long &processCount, char **processNameArray)
{
    for (int i = 0; i < processCount; i++)
    {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processIDArray[i]);
        if (!processHandle)
        {
            processNameArray[i] = (char *)"[restricted]";
            continue;
        }
        processNameArray[i] = new char[1024];
        BOOL processNameFetched = GetModuleFileNameExA(processHandle, NULL, processNameArray[i], 1024);
        if (!processNameFetched)
        {
            processNameArray[i] = (char *)"[restricted]";
        }
        else
        {
            std::string temp = processNameArray[i];
            size_t pos = temp.find_last_of('\\');
            temp = temp.substr(pos + 1);
            processNameArray[i] = new char[temp.length() + 1];
            strcpy(processNameArray[i], temp.c_str());
        }
        CloseHandle(processHandle);
    }
    return true;
}