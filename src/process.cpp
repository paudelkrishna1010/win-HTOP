#include "process.h"
#include <windows.h>
#include <psapi.h>

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
